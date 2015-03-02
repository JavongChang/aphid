/*
 *  CudaBroadphase.cpp
 *  cudabvh
 *
 *  Created by jian zhang on 2/20/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <BaseBuffer.h>
#include <CUDABuffer.h>
#include "CudaLinearBvh.h"
#include "CudaBroadphase.h"
#include "broadphase_implement.h"
#include "scan_implement.h"
#include <CudaBase.h>
CudaBroadphase::CudaBroadphase() 
{
	m_numObjects = 0;
	m_pairCacheLength = 0;
	m_pairCounts = new CUDABuffer;
	m_pairStart = new CUDABuffer;
	m_scanIntermediate = new CUDABuffer;
	m_pairCache[0] = new CUDABuffer;
	m_pairCache[1] = new CUDABuffer;
	m_uniquePair = new CUDABuffer;
	m_scanUniquePair = new CUDABuffer;
}

CudaBroadphase::~CudaBroadphase() {}

const unsigned CudaBroadphase::numBoxes() const
{ return m_numBoxes; }

const unsigned CudaBroadphase::numUniquePairs() const
{ return m_numUniquePairs; }

const unsigned CudaBroadphase::pairCacheLength() const
{ return m_pairCacheLength; }

const unsigned CudaBroadphase::objectStart(unsigned ind) const
{ return m_objectStart[ind]; }

void CudaBroadphase::getOverlappingPairCounts(BaseBuffer * dst)
{ m_pairCounts->deviceToHost(dst->data(), dst->bufferSize()); }

void CudaBroadphase::getOverlappingPairCache(BaseBuffer * dst)
{ m_pairCache[0]->deviceToHost(dst->data(), dst->bufferSize()); }

void CudaBroadphase::getOverlappingPairs(BaseBuffer * dst)
{ m_pairCache[1]->deviceToHost(dst->data(), dst->bufferSize()); }

void CudaBroadphase::getScanCounts(BaseBuffer * dst)
{ m_pairStart->deviceToHost(dst->data(), dst->bufferSize()); }

void CudaBroadphase::getUniquePairs(BaseBuffer * dst)
{ m_uniquePair->deviceToHost(dst->data(), dst->bufferSize()); }

void CudaBroadphase::getScanUniquePairs(BaseBuffer * dst)
{ m_scanUniquePair->deviceToHost(dst->data(), dst->bufferSize()); }

void CudaBroadphase::addBvh(CudaLinearBvh * bvh)
{
	if(m_numObjects==CUDABROADPHASE_MAX_NUMOBJECTS) return;
	m_objects[m_numObjects] = bvh;
	m_numObjects++;
}

void CudaBroadphase::initOnDevice()
{
	if(m_numObjects < 1) return;
	m_objectStart[0] = 0;
	m_numBoxes = 0;
	for(unsigned i = 0; i<m_numObjects; i++) {
		m_objects[i]->initOnDevice();
		m_numBoxes += m_objects[i]->numLeafNodes();
		if(i<m_numObjects-1) {
			m_objectStart[i+1] = m_numBoxes;
		}
	}
	m_scanBufferLength = iDivUp(m_numBoxes, 1024) * 1024;
	m_pairCounts->create(m_scanBufferLength * 4);
	m_pairStart->create(m_scanBufferLength * 4);
	m_scanIntermediate->create(m_scanBufferLength * 4);
}

void CudaBroadphase::update()
{
	if(m_numObjects < 1) return;
	unsigned i, j;
	for(i = 0; i<m_numObjects; i++)
		m_objects[i]->update();
		
	resetPairCounts();
	for(j = 0; j<m_numObjects; j++) {
		for(i = 0; i<m_numObjects; i++) {
			countOverlappingPairs(j, i);
		}
	}
	
	prefixSumPairCounts();
	
	m_pairCacheLength = getScanResult(m_pairCounts, m_pairStart, m_scanBufferLength - 1);
	if(m_pairCacheLength < 1) return;
	
	m_pairCache[0]->create(nextPow2(m_pairCacheLength) * 8);
	m_pairCache[1]->create(nextPow2(m_pairCacheLength) * 8);
	
	for(j = 0; j<m_numObjects; j++) {
		for(i = 0; i<m_numObjects; i++) {
			writeOverlappingPairs(j, i);
		}
	}
	
	squeezeOverlappingPairs();
}

void CudaBroadphase::resetPairCounts()
{
	broadphaseResetPairCounts((uint *)m_pairCounts->bufferOnDevice(), m_scanBufferLength);
}

void CudaBroadphase::countOverlappingPairs(unsigned a, unsigned b)
{
	uint * counts = (uint *)m_pairCounts->bufferOnDevice();
	counts += m_objectStart[a];
	
	CudaLinearBvh * query = m_objects[a];
	CudaLinearBvh * tree = m_objects[b];
	
	void * boxes = (Aabb *)query->leafAabbs();
	const unsigned numBoxes = query->numLeafNodes();
	
	void * rootNodeIndex = tree->rootNodeIndex();
	void * internalNodeChildIndex = tree->internalNodeChildIndices();
	void * internalNodeAabbs = tree->internalNodeAabbs();
	void * leafNodeAabbs = tree->leafAabbs();
	void * mortonCodesAndAabbIndices = tree->leafHash();
	
	broadphaseComputePairCounts(counts, (Aabb *)boxes, numBoxes,
							(int *)rootNodeIndex, 
							(int2 *)internalNodeChildIndex, 
							(Aabb *)internalNodeAabbs, 
							(Aabb *)leafNodeAabbs,
							(KeyValuePair *)mortonCodesAndAabbIndices,
							(a == b));							
}

void CudaBroadphase::prefixSumPairCounts()
{
    void * scanInput = m_pairCounts->bufferOnDevice();
    void * scanResult = m_pairStart->bufferOnDevice();
    void * scanIntermediate = m_scanIntermediate->bufferOnDevice();
    scanExclusive((uint *)scanResult, (uint *)scanInput, (uint *)scanIntermediate, m_scanBufferLength / 1024, 1024);
}

void CudaBroadphase::writeOverlappingPairs(unsigned a, unsigned b)
{
    uint * counts = (uint *)m_pairCounts->bufferOnDevice();
	counts += m_objectStart[a];
	
	uint * starts = (uint *)m_pairStart->bufferOnDevice();
	starts += m_objectStart[a];
	
	CudaLinearBvh * query = m_objects[a];
	CudaLinearBvh * tree = m_objects[b];
	
	void * boxes = (Aabb *)query->leafAabbs();
	const unsigned numBoxes = query->numLeafNodes();
	
	void * rootNodeIndex = tree->rootNodeIndex();
	void * internalNodeChildIndex = tree->internalNodeChildIndices();
	void * internalNodeAabbs = tree->internalNodeAabbs();
	void * leafNodeAabbs = tree->leafAabbs();
	void * mortonCodesAndAabbIndices = tree->leafHash();
	
	void * cache = m_pairCache[0]->bufferOnDevice();
	
	broadphaseResetPairCache((uint2 *)cache, nextPow2(m_pairCacheLength));
	
	broadphaseWritePairCache((uint2 *)cache, starts, counts, 
	                         (Aabb *)boxes, numBoxes,
							(int *)rootNodeIndex, 
							(int2 *)internalNodeChildIndex, 
							(Aabb *)internalNodeAabbs, 
							(Aabb *)leafNodeAabbs,
							(KeyValuePair *)mortonCodesAndAabbIndices,
							a, b);
}

void CudaBroadphase::getBoxes(BaseBuffer * dst)
{
    char * hbox = (char *)dst->data();
    unsigned i;
    for(i = 0; i<m_numObjects; i++) {
        const unsigned numBoxes = m_objects[i]->numLeafNodes();
        m_objects[i]->getLeafAabbsAt(hbox);
		hbox += numBoxes * 24;
	}
}

void CudaBroadphase::squeezeOverlappingPairs()
{
    void * dst = m_pairCache[0]->bufferOnDevice();
    void * tmp = m_pairCache[1]->bufferOnDevice();
    RadixSort((KeyValuePair *)dst, (KeyValuePair *)tmp, nextPow2(m_pairCacheLength), 32);
    
    const unsigned scanUniquePairLength = iDivUp(m_pairCacheLength, 1024) * 1024;
	m_uniquePair->create(scanUniquePairLength * 4);
	
	void * unique = m_uniquePair->bufferOnDevice();
	broadphaseUniquePair((uint *)unique, (uint2 *)dst, m_pairCacheLength, scanUniquePairLength);
	
	m_scanUniquePair->create(scanUniquePairLength * 4);
	m_scanIntermediate->create(scanUniquePairLength * 4);
	
	void * scanResult = m_scanUniquePair->bufferOnDevice();
	void * scanIntermediate = m_scanIntermediate->bufferOnDevice();
	scanExclusive((uint *)scanResult, (uint *)unique, (uint *)scanIntermediate, scanUniquePairLength / 1024, 1024);

// squeeze to [1]
	broadphaseCompactUniquePairs((uint2 *)tmp, (uint2 *)dst, (uint *)unique, (uint *)scanResult, m_pairCacheLength);
	
	m_numUniquePairs = getScanResult(m_uniquePair, m_scanUniquePair, m_pairCacheLength - 1);
}

unsigned CudaBroadphase::getScanResult(CUDABuffer * counts, CUDABuffer * sums, unsigned n)
{
    unsigned a, b;
    counts->deviceToHost(&a, 4*n, 4);
    sums->deviceToHost(&b, 4*n, 4);
    return a + b;
}

