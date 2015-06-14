#include "Overlapping.cuh"
#include "Overlapping2.cuh"
#include "TetrahedronSystemInterface.h"

namespace bvhoverlap {

void writeLocation(uint * dst, uint * src, uint n)
{
    int tpb = 512;
    dim3 block(tpb, 1, 1);
    unsigned nblk = iDivUp(n, tpb);
    
    dim3 grid(nblk, 1, 1);
    
    startAsWriteLocation_kernel<<< grid, block >>>(dst, src, n);
}

void countPairsSelfCollideExclS(uint * dst, 
                                Aabb * boxes, 
                                uint numBoxes,
								int2 * internalNodeChildIndex, 
								Aabb * internalNodeAabbs, 
								Aabb * leafNodeAabbs,
								KeyValuePair * mortonCodesAndAabbIndices,
								int * exclusionIndices)
{
    int nThreads = 64;
	dim3 block(nThreads, 1, 1);
    int nblk = iDivUp(numBoxes, nThreads);
    dim3 grid(nblk, 1, 1);

	countPairsSExclS_kernel<TETRAHEDRONSYSTEM_VICINITY_LENGTH, 64> <<< grid, block>>>(dst,
                                boxes,
                                numBoxes,
								internalNodeChildIndex, 
								internalNodeAabbs, 
								leafNodeAabbs,
								mortonCodesAndAabbIndices,
								exclusionIndices);
}

void writePairCacheSelfCollideExclS(uint2 * dst, uint * locations, 
                                uint * starts, uint * counts,
                                Aabb * boxes, uint numBoxes,
								int * rootNodeIndex, 
								int2 * internalNodeChildIndex, 
								Aabb * internalNodeAabbs, 
								Aabb * leafNodeAabbs,
								KeyValuePair * mortonCodesAndAabbIndices,
								unsigned queryIdx,
								int * exclusionIndices)
{
    int nThreads = 64;
	dim3 block(nThreads, 1, 1);
    int nblk = iDivUp(numBoxes, nThreads);
    dim3 grid(nblk, 1, 1);
	
    writePairCacheSExclS_kernel<TETRAHEDRONSYSTEM_VICINITY_LENGTH, 64> <<< grid, block>>>(dst, 
                                locations,
                                starts, counts,
                                boxes,
                                numBoxes,
								internalNodeChildIndex, 
								internalNodeAabbs, 
								leafNodeAabbs,
								mortonCodesAndAabbIndices,
								queryIdx,
								exclusionIndices);
}

void countPairs(uint * dst,
                                Aabb * boxes,
                                uint numBoxes,
								int2 * internalNodeChildIndex, 
								Aabb * internalNodeAabbs, 
								Aabb * leafNodeAabbs,
								KeyValuePair * mortonCodesAndAabbIndices)
{ 
    int tpb = 64;
    dim3 block(tpb, 1, 1);
    unsigned nblk = iDivUp(numBoxes, tpb);
    
    dim3 grid(nblk, 1, 1);
    
    countPairs_kernel<64> <<< grid, block, 16320 >>>(dst,
                                boxes,
                                numBoxes,
								internalNodeChildIndex, 
								internalNodeAabbs, 
								leafNodeAabbs,
								mortonCodesAndAabbIndices);
}

void writePairCache(uint2 * dst, uint * locations, 
                                uint * starts, uint * counts,
                              Aabb * boxes, uint numBoxes,
								int2 * internalNodeChildIndex, 
								Aabb * internalNodeAabbs, 
								Aabb * leafNodeAabbs,
								KeyValuePair * mortonCodesAndAabbIndices,
								unsigned queryIdx, unsigned treeIdx)
{
    int tpb = 64;
    dim3 block(tpb, 1, 1);
    unsigned nblk = iDivUp(numBoxes, tpb);
    
    dim3 grid(nblk, 1, 1);
    
    writePairCache_kernel<64> <<< grid, block, 16320 >>>(dst, 
                                locations,
                                starts, counts,
                                boxes,
                                numBoxes,
								internalNodeChildIndex, 
								internalNodeAabbs, 
								leafNodeAabbs,
								mortonCodesAndAabbIndices,
								queryIdx, treeIdx);
}
}
