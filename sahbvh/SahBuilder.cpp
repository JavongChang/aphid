/*
 *  SahBuilder.cpp
 *  testsah
 *
 *  Created by jian zhang on 5/4/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
input primitive range 0,9
node 0

node_count = 1 (root)
N_segments = 1

N=10 input primitives
0 1 2 3 4 5 6 7 8 9 

morton_code[N]
0 0 0 0 1 1 1 1 1 1
0 1 1 1 0 0 1 1 1 1
1 0 1 1 1 1 0 0 1 1

head_to_node[N]
0 -1 -1 -1 -1 -1 -1 -1 -1

segment_heads[N]
0 0 0 0 0 0 0 0 0 0

segment_id[N]
0 0 0 0 0 0 0 0 0 0 

N_segments = 1 + segment_id[N-1]

P = (1<<p) - 1 where p=3

block_splits[N * 3] = {-1, -1, -1, ...}

block_splits[N*3]
0 1 2 3 4 5 6 7 8 9
        1           level1
  1         1       level2
    1           1   level3

segment_heads[N]
0 1 1 0 1 0 1 0 1 0 combined


segment_id[N]
0 1 2 2 3 3 4 4 5 5 inclusive_scan(segment_heads)

N_segments = 1 + 5

head_to_node[N]
0 1 2 -1 4 -1 6 -1 8 -1

N_splits = 5 sum(segment_heads)

node_count += N_splits * 2

block_offset[N_segments + 1]
0 1 2 3 4 5 6 7 8 9
        1          
  1         1      
    1           1   
1 1 1   1   1   1
       0           
  1         2      
    3           4  
5 6 7  8    9   10

block_offset by scan block_splits and head_to_node>-1

internal node (child0, child1)

0: 1,2   
1: 5,3   
2: 8,4   
3: 6,7
4: 9,10

leaf node (primitive0, primitive1)
5: 0,0
6: 1,1
7: 2,3
8: 4,5
9: 6,7
10: 8,9

3 level, max 7 splits, max 6 new internal nodes, max 8 leaf nodes

n splits spawn n-1 internal nodes, n+1 leaf nodes, n+1 segments, 
each segment will be input primitive range of next 3 level splits

each node holds segment begin/end initially. 
internal nodes will look for child nodes based on correct segment begin/end. For example.

node1 should connect to node3 and node4 by default, 
but non-split leads to node3 begin with 1, so go to next node, 
until find node5 begin with 0. then looking for first node end with 3, 
which is node3. So node1 has child node5 and node3 

input n_blocks
	root_id
      block_offset (relative to last node)

output:
n_blocks 1
root_id 0
block_offset 0
spawn node  1 2 3 4 5 6 7 8 9 10

output:
n_blocks 6
root_id (leaf) 5 6 7 8 9 10
block_offset 11 25 39 53 67 81

give 14 nodes to each block
 *
 */

#include "SahBuilder.h"
#include <CudaDbgLog.h>
#include <CudaReduction.h>
#include <CudaLinearBvh.h>
#include <sahbvh_implement.h>
#include <CudaBase.h>
#include <CudaScan.h>

CudaDbgLog sahlg("sah.txt");

static std::vector<std::pair<int, int> > binDesc;
static std::vector<std::pair<int, int> > emissionDesc;

SahBuilder::SahBuilder() 
{
    binDesc.push_back(std::pair<int, int>(0, 0));
	binDesc.push_back(std::pair<int, int>(0, 4));
	binDesc.push_back(std::pair<int, int>(0, 8));
	binDesc.push_back(std::pair<int, int>(0, 12));
	binDesc.push_back(std::pair<int, int>(0, 16));
	binDesc.push_back(std::pair<int, int>(0, 20));
	binDesc.push_back(std::pair<int, int>(0, 24));
	binDesc.push_back(std::pair<int, int>(0, 28));
	binDesc.push_back(std::pair<int, int>(0, 32));
	binDesc.push_back(std::pair<int, int>(0, 36));
	binDesc.push_back(std::pair<int, int>(0, 40));
	binDesc.push_back(std::pair<int, int>(0, 44));
	binDesc.push_back(std::pair<int, int>(0, 48));
	binDesc.push_back(std::pair<int, int>(0, 52));
	binDesc.push_back(std::pair<int, int>(1, 56));
	binDesc.push_back(std::pair<int, int>(1, 60));
	
	emissionDesc.push_back(std::pair<int, int>(0, 0));
	emissionDesc.push_back(std::pair<int, int>(0, 4));
	emissionDesc.push_back(std::pair<int, int>(0, 8));
    emissionDesc.push_back(std::pair<int, int>(0, 12));
	
    std::cout<<"size of bin "<<sizeof(SplitBin)<<"\n";
    m_mortonBits = new CUDABuffer;
    m_clusterAabb = new CUDABuffer;
	m_runHeads = new CUDABuffer;
	m_runIndices = new CUDABuffer;
	m_runHash = new CUDABuffer;
	m_runLength = new CUDABuffer;
	m_compressedRunHeads = new CUDABuffer;
	m_emissions[0] = new CUDABuffer;
	m_emissions[1] = new CUDABuffer;
    m_splitBins = new CUDABuffer;
    m_splitIds = new CUDABuffer;
    m_emissionBlocks = new CUDABuffer;
    m_totalNodeCount = new CUDABuffer;
}

SahBuilder::~SahBuilder() 
{
    delete m_mortonBits;
    delete m_clusterAabb;
	delete m_runHeads;
	delete m_runIndices;
	delete m_runHash;
	delete m_runLength;
	delete m_compressedRunHeads;
	delete m_emissions[0];
	delete m_emissions[1];
    delete m_splitBins;
    delete m_splitIds;
    delete m_emissionBlocks;
	delete m_totalNodeCount;
}

void SahBuilder::initOnDevice()
{
    m_emissions[0]->create(SAH_MAX_N_BLOCKS * SIZE_OF_EMISSIONEVENT);
    m_emissions[1]->create(SAH_MAX_N_BLOCKS * SIZE_OF_EMISSIONEVENT);
    m_clusterAabb->create(SAH_MAX_N_BLOCKS * 24);
    m_splitBins->create(256 * SIZE_OF_SPLITBIN * SAH_MAX_NUM_BINS * 3);
    m_totalNodeCount->create(4);
    BvhBuilder::initOnDevice();
}

void SahBuilder::build(CudaLinearBvh * bvh)
{
// create data
	const unsigned n = bvh->numPrimitives();
	createSortAndScanBuf(n);
	
    m_mortonBits->create(n * 4);
	m_runHeads->create(CudaScan::getScanBufferLength(n) * 4);
	m_runIndices->create(CudaScan::getScanBufferLength(n) * 4);
    
// one splitId for each cluster or primitive
    m_splitIds->create(n * SIZE_OF_SPLITID);
    
	void * primitiveHash = bvh->primitiveHash();
	void * primitiveAabb = bvh->primitiveAabb();
	
	float bounding[6];
	computeMortionHash(primitiveHash, primitiveAabb, n, bounding);
	
	unsigned numClusters = 0;
	if(n<=2048) {
        sort(primitiveHash, n, 32);
        numClusters = n;
    }
	else {
        numClusters = sortPrimitives(primitiveHash, primitiveAabb,
            n, 10, 5);
    }
      
    // sahlg.writeMortonHash(bvh->primitiveHashBuf(), n, "sorted_primitive_hash", CudaDbgLog::FOnce);

// set root node range
    int rr[2];
    rr[0] = 0;
    rr[1] = numClusters - 1;
    
	bvh->setRootChildAndAabb(rr, bounding);
    
    unsigned one = 1;
    m_totalNodeCount->hostToDevice(&one, 4);
    
// emit from root node
	EmissionEvent eb;
	eb.root_id = 0;
	eb.node_offset = 0;
	
	m_emissions[0]->hostToDevice(&eb, SIZE_OF_EMISSIONEVENT);
	unsigned numEmissions = 1;
	unsigned numNodes = 1;

// emission-block relationship	
	m_emissionBlocks->create((n * SIZE_OF_EMISSIONBLOCK)>>2);
 
// 16 bins for each dimension for first split
    m_splitBins->create(numEmissions * SIZE_OF_SPLITBIN * SAH_MAX_NUM_BINS * 3);

	sahbvh_emitSahSplit((EmissionEvent *)m_emissions[1]->bufferOnDevice(),
	    (EmissionEvent *)m_emissions[0]->bufferOnDevice(),
	    (int2 *)bvh->internalNodeChildIndices(),
	    (Aabb *)bvh->internalNodeAabbs(),
	    (KeyValuePair *)m_runHash->bufferOnDevice(),
        (Aabb *)clusterAabbs(),
        (SplitBin *)splitBins(),
        (EmissionBlock *)emissionBlocks(),
        (SplitId *)splitIds(),
        (uint *)(&((char *)m_totalNodeCount->bufferOnDevice())[4]),
        (uint *)m_totalNodeCount->bufferOnDevice(),
	    numClusters,
        SAH_MAX_NUM_BINS,
	    numEmissions,
	    numNodes);

    CudaBase::CheckCudaError("sah split");
	sahlg.writeStruct(m_splitBins, numEmissions * SAH_MAX_NUM_BINS * 3, 
            "bins", 
            binDesc,
            SIZE_OF_SPLITBIN,
            CudaDbgLog::FOnce);
    
    sahlg.writeInt2(m_splitIds, numClusters, "emission_id", CudaDbgLog::FOnce);
    
    sahlg.writeStruct(m_emissions[0], numEmissions, 
            "in_emissions", 
            emissionDesc,
            SIZE_OF_EMISSIONEVENT,
            CudaDbgLog::FOnce);
    
    const unsigned numNodeb4 = numNodes;
    m_totalNodeCount->deviceToHost(&numNodes, 4);
	
    sahlg.writeInt2(bvh->internalChildBuf(), numNodes, "internal_node", CudaDbgLog::FOnce);
    sahlg.writeAabb(bvh->internalAabbBuf(), numNodes, "internal_box", CudaDbgLog::FOnce);
    
	numEmissions = numNodes - numNodeb4;
    sahlg.writeStruct(m_emissions[1], numEmissions, 
            "out_emissions", 
            emissionDesc,
            SIZE_OF_EMISSIONEVENT,
            CudaDbgLog::FOnce);
}

int SahBuilder::countTreeBits(void * morton, unsigned numPrimitives)
{
    sahbvh_countTreeBits((uint *)m_mortonBits->bufferOnDevice(), 
                            (KeyValuePair *)morton,
                            numPrimitives);
    int nBits[2];
    reducer()->minMax<int2, int>((int *)nBits, (int *)m_mortonBits->bufferOnDevice(), numPrimitives);
    return nBits[1] - nBits[0];
}

int SahBuilder::getM(int n, int m)
{
    int r = n - 3 * m;
    while(r<=0) {
        m--;
        r = n - 3 * m;
    }
    return m;
}

unsigned SahBuilder::sortPrimitives(void * morton, void * primitiveAabbs, 
                                unsigned numPrimitives, int n, int m)
{
    // std::cout<<" sort by first 3*m bits "<<m<<" ";
	const unsigned scanLength = CudaScan::getScanBufferLength(numPrimitives);
	
	const unsigned d = 3*(n - m);

	sahbvh_computeRunHead((uint *)m_runHeads->bufferOnDevice(), 
							(KeyValuePair *)morton,
							d,
							numPrimitives,
							scanLength);
	
	CudaBase::CheckCudaError("finding bvh run heads");
	
	sahlg.writeUInt(m_runHeads, numPrimitives, "run_heads", CudaDbgLog::FOnce);
			
	const unsigned numRuns = scanner()->prefixSum(m_runIndices, 
												m_runHeads, scanLength);
												
	CudaBase::CheckCudaError("scan bvh run heads");
	
	sahlg.writeUInt(m_runIndices, numPrimitives, "scanned_run_heads", CudaDbgLog::FOnce);
	
	m_compressedRunHeads->create(numRuns * 4);
	
	sahbvh_compressRunHead((uint *)m_compressedRunHeads->bufferOnDevice(), 
							(uint *)m_runHeads->bufferOnDevice(),
							(uint *)m_runIndices->bufferOnDevice(),
							numPrimitives);
							
	sahlg.writeUInt(m_compressedRunHeads, numRuns, "compressed_run_heads", CudaDbgLog::FOnce);
	
	const unsigned sortRunLength = nextPow2(numRuns);
	m_runHash->create((sortRunLength * sizeof(KeyValuePair)));
	
	sahbvh_computeRunHash((KeyValuePair *)m_runHash->bufferOnDevice(), 
						(KeyValuePair *)morton,
						(uint *)m_compressedRunHeads->bufferOnDevice(),
                        m,
						d,
						numRuns,
						sortRunLength);
	
	CudaBase::CheckCudaError("write run hash");
						
	sort(m_runHash->bufferOnDevice(), numRuns, 3*m);
	
	sahlg.writeMortonHash(m_runHash, numRuns, "sorted_run_heads", CudaDbgLog::FOnce);

	m_runLength->create(scanLength * 4);
	
	sahbvh_computeRunLength((uint *)m_runLength->bufferOnDevice(),
							(uint *)m_compressedRunHeads->bufferOnDevice(),
							(KeyValuePair *)m_runHash->bufferOnDevice(),
							numRuns,
							numPrimitives,
							scanLength);
							
	sahlg.writeUInt(m_runLength, numRuns, "run_length", CudaDbgLog::FOnce);

	scanner()->prefixSum(m_runIndices, m_runLength, scanLength);
	
	sahlg.writeUInt(m_runIndices, numRuns, "offset", CudaDbgLog::FOnce);

	sahbvh_copyHash((KeyValuePair *)sortIntermediate(),
					(KeyValuePair *)morton,
					numPrimitives);
					
	sahbvh_decompressIndices((uint *)m_runHeads->bufferOnDevice(),
					(uint *)m_compressedRunHeads->bufferOnDevice(),
					(KeyValuePair *)m_runHash->bufferOnDevice(),
					(uint *)m_runIndices->bufferOnDevice(),
					(uint *)m_runLength->bufferOnDevice(),
					numRuns);
					
	sahlg.writeUInt(m_runHeads, numPrimitives, "decompressed_ind", CudaDbgLog::FOnce);
   
    sahbvh_computeClusterAabbs((Aabb *)clusterAabbs(),
            (Aabb *)primitiveAabbs,
            (uint *)m_compressedRunHeads->bufferOnDevice(),
            (KeyValuePair *)m_runHash->bufferOnDevice(),
            (uint *)m_runIndices->bufferOnDevice(),
            (uint *)m_runLength->bufferOnDevice(),
            numRuns);
    
    // sahlg.writeFlt(m_clusterAabb, numRuns * 6, "cluster_box", CudaDbgLog::FOnce);
	
	sahbvh_writeSortedHash((KeyValuePair *)morton,
							(KeyValuePair *)sortIntermediate(),
							(uint *)m_runHeads->bufferOnDevice(),
							numPrimitives);
	return numRuns;
}

void * SahBuilder::splitBins()
{ return m_splitBins->bufferOnDevice(); }

void * SahBuilder::splitIds()
{ return m_splitIds->bufferOnDevice(); }

void * SahBuilder::clusterAabbs()
{ return m_clusterAabb->bufferOnDevice(); }

void * SahBuilder::emissionBlocks()
{ return m_emissionBlocks->bufferOnDevice(); }
//:~
