/*
 *  CudaLinearBvh.cpp
 *  cudabvh
 *
 *  Created by jian zhang on 1/15/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <CudaBase.h>
#include <BaseBuffer.h>
#include <CUDABuffer.h>
#include "BvhTriangleMesh.h"
#include "CudaLinearBvh.h"
#include <radixsort_implement.h>
#include "createBvh_implement.h"
#include "reduceBox_implement.h"
#include "CudaReduction.h"
#include "bvh_dbg.h"

CudaLinearBvh::CudaLinearBvh() 
{ 
	m_numLeafNodes = 0; 
	m_leafAabbs = new CUDABuffer;
	m_internalNodeAabbs = new CUDABuffer;
	m_leafHash[0] = new CUDABuffer;
	m_leafHash[1] = new CUDABuffer;
	m_internalNodeCommonPrefixValues = new CUDABuffer;
	m_internalNodeCommonPrefixLengths = new CUDABuffer;
	m_leafNodeParentIndices = new CUDABuffer;
	m_internalNodeChildIndices = new CUDABuffer;
	m_internalNodeParentIndices = new CUDABuffer;
	m_rootNodeIndexOnDevice = new CUDABuffer;
	m_distanceInternalNodeFromRoot = new CUDABuffer;
	m_findMaxDistance = new CudaReduction;
#if DRAW_BVH_HASH
	m_hostLeafHash = new BaseBuffer;
	m_hostLeafBox = new BaseBuffer;
#endif
#if DRAW_BVH_HIERARCHY
	m_hostInternalAabb = new BaseBuffer;
#endif
}

CudaLinearBvh::~CudaLinearBvh() {}

void CudaLinearBvh::setNumLeafNodes(unsigned n)
{ m_numLeafNodes = n; }

void CudaLinearBvh::initOnDevice()
{
	m_leafAabbs->create(numLeafNodes() * sizeof(Aabb));
// assume numInternalNodes() >> ReduceMaxBlocks
	m_internalNodeAabbs->create(numInternalNodes() * sizeof(Aabb));
	
	m_leafHash[0]->create(nextPow2(numLeafNodes()) * sizeof(KeyValuePair));
	m_leafHash[1]->create(nextPow2(numLeafNodes()) * sizeof(KeyValuePair));
	
	m_internalNodeCommonPrefixValues->create(numInternalNodes() * sizeof(uint64));
	m_internalNodeCommonPrefixLengths->create(numInternalNodes() * sizeof(int));
	
	m_leafNodeParentIndices->create(numLeafNodes() * sizeof(int));
	m_internalNodeChildIndices->create(numInternalNodes() * sizeof(int2));
	m_internalNodeParentIndices->create(numInternalNodes() * sizeof(int));
	m_rootNodeIndexOnDevice->create(sizeof(int));
	m_distanceInternalNodeFromRoot->create(numInternalNodes() * sizeof(int));
	
	m_findMaxDistance->initOnDevice();

#if DRAW_BVH_HASH
	m_hostLeafBox->create(numLeafNodes() * sizeof(Aabb));
	m_hostLeafHash->create(nextPow2(numLeafNodes()) * sizeof(KeyValuePair));
#endif

}

const unsigned CudaLinearBvh::numLeafNodes() const 
{ return m_numLeafNodes; }

const unsigned CudaLinearBvh::numInternalNodes() const 
{ return numLeafNodes() - 1; }

void CudaLinearBvh::getBound(Aabb * dst)
{ cudaMemcpy(dst, combineAabbsBuffer(), 24, cudaMemcpyDeviceToHost); }

void CudaLinearBvh::getRootNodeIndex(int * dst)
{ m_rootNodeIndexOnDevice->deviceToHost((void *)dst, m_rootNodeIndexOnDevice->bufferSize()); }

void CudaLinearBvh::getInternalAabbs(BaseBuffer * dst)
{ m_internalNodeAabbs->deviceToHost(dst->data(), m_internalNodeAabbs->bufferSize()); }

void CudaLinearBvh::getInternalDistances(BaseBuffer * dst)
{ m_distanceInternalNodeFromRoot->deviceToHost(dst->data(), m_distanceInternalNodeFromRoot->bufferSize()); }

void CudaLinearBvh::getInternalChildIndex(BaseBuffer * dst)
{ m_internalNodeChildIndices->deviceToHost(dst->data(), m_internalNodeChildIndices->bufferSize()); }

void * CudaLinearBvh::rootNodeIndex()
{ return m_rootNodeIndexOnDevice->bufferOnDevice(); }

void * CudaLinearBvh::internalNodeChildIndices()
{ return m_internalNodeChildIndices->bufferOnDevice(); }

void * CudaLinearBvh::internalNodeAabbs()
{ return m_internalNodeAabbs->bufferOnDevice(); }

void * CudaLinearBvh::leafAabbs()
{ return m_leafAabbs->bufferOnDevice(); }

void CudaLinearBvh::getLeafAabbsAt(char * dst)
{ m_leafAabbs->deviceToHost(dst, 0, m_leafAabbs->bufferSize()); }

void * CudaLinearBvh::leafHash()
{ return m_leafHash[0]->bufferOnDevice(); }

void * CudaLinearBvh::combineAabbsBuffer()
{ return m_internalNodeAabbs->bufferOnDevice(); }
 
void CudaLinearBvh::update()
{
	combineAabb();
	computeAndSortLeafHash();
	buildInternalTree();
}

void CudaLinearBvh::combineAabb()
{
    void * pdst = combineAabbsBuffer();
    unsigned threads, blocks;
    unsigned n = numLeafNodes();
	getReduceBlockThread(blocks, threads, n);
	
	// std::cout<<"n "<<n<<" blocks x threads : "<<blocks<<" x "<<threads<<" sharedmem size "<<threads * sizeof(Aabb)<<"\n";
	
	bvhReduceAabbByAabb((Aabb *)pdst, (Aabb *)leafAabbs(), numLeafNodes(), blocks, threads);
	
	n = blocks;
	while(n > 1) {
		getReduceBlockThread(blocks, threads, n);
		
		// std::cout<<"n "<<n<<" blocks x threads : "<<blocks<<" x "<<threads<<" sharedmem size "<<threads * sizeof(Aabb)<<"\n";
	
		bvhReduceAabbByAabb((Aabb *)pdst, (Aabb *)pdst, n, blocks, threads);
		
		n = (n + (threads*2-1)) / (threads*2);
	}
}

void CudaLinearBvh::computeAndSortLeafHash()
{
    Aabb bound;
    getBound(&bound);
#if PRINT_BOUND
    std::cout<<" bound (("<<bound.low.x<<" "<<bound.low.y<<" "<<bound.low.z;
    std::cout<<"),("<<bound.high.x<<" "<<bound.high.y<<" "<<bound.high.z<<"))";
#endif
	void * dst = m_leafHash[0]->bufferOnDevice();
	void * src = leafAabbs();
	bvhCalculateLeafHash((KeyValuePair *)dst, (Aabb *)src, numLeafNodes(), nextPow2(numLeafNodes()),
	    bound);
	void * tmp = m_leafHash[1]->bufferOnDevice();
	RadixSort((KeyValuePair *)dst, (KeyValuePair *)tmp, nextPow2(numLeafNodes()), 32);
}

void CudaLinearBvh::buildInternalTree()
{
	void * morton = m_leafHash[0]->bufferOnDevice();
	void * commonPrefix = m_internalNodeCommonPrefixValues->bufferOnDevice();
	void * commonPrefixLengths = m_internalNodeCommonPrefixLengths->bufferOnDevice();
	
	bvhComputeAdjacentPairCommonPrefix((KeyValuePair *)morton,
										(uint64 *)commonPrefix,
										(int *)commonPrefixLengths,
										numInternalNodes());
	
	void * leafNodeParentIndex = m_leafNodeParentIndices->bufferOnDevice();
	void * internalNodeChildIndex = m_internalNodeChildIndices->bufferOnDevice();
	
	bvhConnectLeafNodesToInternalTree((int *)commonPrefixLengths, 
								(int *)leafNodeParentIndex,
								(int2 *)internalNodeChildIndex, 
								numLeafNodes());
								
	void * internalNodeParentIndex = m_internalNodeParentIndices->bufferOnDevice();
	void * rootInd = m_rootNodeIndexOnDevice->bufferOnDevice();
	bvhConnectInternalTreeNodes((uint64 *)commonPrefix, (int *)commonPrefixLengths,
								(int2 *)internalNodeChildIndex,
								(int *)internalNodeParentIndex,
								(int *)rootInd,
								numInternalNodes());
	
	void * distanceFromRoot = m_distanceInternalNodeFromRoot->bufferOnDevice();
	bvhFindDistanceFromRoot((int *)rootInd, (int *)internalNodeParentIndex,
							(int *)distanceFromRoot, 
							numInternalNodes());
				
	int maxDistance = -1;
	m_findMaxDistance->maxI(maxDistance, (int *)m_distanceInternalNodeFromRoot->bufferOnDevice(), numInternalNodes());
#if PRINT_BVH_MAXLEVEL
	std::cout<<" bvh max level "<<maxDistance<<"\n";
#endif
	if(maxDistance < 0)
		CudaBase::CheckCudaError("finding bvh max level");
	
	formInternalTreeAabbsIterative(maxDistance);
}

void CudaLinearBvh::formInternalTreeAabbsIterative(int maxDistance)
{
	void * distances = m_distanceInternalNodeFromRoot->bufferOnDevice();
	void * boxes = m_leafHash[0]->bufferOnDevice();
	void * internalNodeChildIndex = m_internalNodeChildIndices->bufferOnDevice();
	void * leafNodeAabbs = m_leafAabbs->bufferOnDevice();
	void * internalNodeAabbs = m_internalNodeAabbs->bufferOnDevice();
	for(int distanceFromRoot = maxDistance; distanceFromRoot >= 0; --distanceFromRoot) {		
		bvhFormInternalNodeAabbsAtDistance((int *)distances, (KeyValuePair *)boxes,
											(int2 *)internalNodeChildIndex,
											(Aabb *)leafNodeAabbs, (Aabb *)internalNodeAabbs,
											maxDistance, distanceFromRoot, 
											numInternalNodes());
	}
}

const unsigned CudaLinearBvh::usedMemory() const
{
	unsigned n = m_leafAabbs->bufferSize();
	n += m_internalNodeAabbs->bufferSize();
	n += m_leafHash[0]->bufferSize() * 2;
	n += m_internalNodeCommonPrefixValues->bufferSize();
	n += m_internalNodeCommonPrefixLengths->bufferSize();
	n += m_leafNodeParentIndices->bufferSize();
	n += m_internalNodeChildIndices->bufferSize();
	n += m_internalNodeParentIndices->bufferSize();
	n += m_rootNodeIndexOnDevice->bufferSize();
    n += m_distanceInternalNodeFromRoot->bufferSize();
	return n;
}

#if DRAW_BVH_HASH
void * CudaLinearBvh::hostLeafHash()
{ return m_hostLeafHash->data(); }

void * CudaLinearBvh::hostLeafBox()
{ return m_hostLeafBox->data(); }
#endif

void CudaLinearBvh::sendDbgToHost()
{
#if DRAW_BVH_HASH
	m_leafAabbs->deviceToHost(m_hostLeafBox->data(), m_leafAabbs->bufferSize());
	m_leafHash[0]->deviceToHost(m_hostLeafHash->data(), m_leafHash[0]->bufferSize());
#endif
}

#if DRAW_BVH_HIERARCHY
void * CudaLinearBvh::hostInternalAabb()
{ return m_hostInternalAabb->data(); }
#endif
