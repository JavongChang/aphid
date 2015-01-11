/*
 *  BvhSolver.cpp
 *  
 *
 *  Created by jian zhang on 10/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <QtCore>
#include <CudaBase.h>
#include <CUDABuffer.h>
#include "BvhSolver.h"
#include "plane_implement.h"
#include "createBvh_implement.h"
#include "reduceBox_implement.h"

unsigned UDIM = 121;
unsigned UDIM1 = 122;

BvhSolver::BvhSolver(QObject *parent) : BaseSolverThread(parent) 
{
	m_alpha = 0;
}

BvhSolver::~BvhSolver() {}

// i,j  i1,j  
// i,j1 i1,j1
//
// i,j  i1,j  
// i,j1
//		i1,j  
// i,j1 i1,j1

void BvhSolver::init()
{
	CudaBase::CheckCUDevice();
	CudaBase::SetDevice();
	qDebug()<<"solverinit";
	m_vertexBuffer = new CUDABuffer;
	m_vertexBuffer->create(numVertices() * 12);
	m_displayVertex = new BaseBuffer;
	m_displayVertex->create(numVertices() * 12);
	m_numTriangles = UDIM * UDIM * 2;
	m_numTriIndices = m_numTriangles * 3;
	m_triIndices = new unsigned[m_numTriIndices];
	unsigned i, j, i1, j1;
	unsigned *ind = &m_triIndices[0];
	for(j=0; j < UDIM; j++) {
	    j1 = j + 1;
		for(i=0; i < UDIM; i++) {
		    i1 = i + 1;
			*ind = j * UDIM1 + i;
			ind++;
			*ind = j1 * UDIM1 + i;
			ind++;
			*ind = j * UDIM1 + i1;
			ind++;

			*ind = j * UDIM1 + i1;
			ind++;
			*ind = j1 * UDIM1 + i;
			ind++;
			*ind = j1 * UDIM1 + i1;
			ind++;
		}
	}
	
	m_numEdges = UDIM * UDIM1 + UDIM * UDIM1 + UDIM * UDIM;
	m_edges = new BaseBuffer;
	m_edges->create(m_numEdges * sizeof(EdgeContact));
	EdgeContact * edge = (EdgeContact *)(&m_edges->data()[0]);
	
	for(j=0; j < UDIM1; j++) {
	    j1 = j + 1;
		for(i=0; i < UDIM; i++) {
		    i1 = i + 1;
		    if(j==0) {
		        edge->v[0] = i1;
		        edge->v[1] = i;
		        edge->v[2] = UDIM1 + i;
		        edge->v[3] = MAX_INDEX;
		    }
		    else if(j==UDIM) {
		        edge->v[0] = j * UDIM1 + i;
		        edge->v[1] = j * UDIM1 + i1;
		        edge->v[2] = (j - 1) * UDIM1 + i1;
		        edge->v[3] = MAX_INDEX;
		    }
		    else {
		        edge->v[0] = j * UDIM1 + i;
		        edge->v[1] = j * UDIM1 + i1;
		        edge->v[2] = (j - 1) * UDIM1 + i1;
		        edge->v[3] = j1 * UDIM1 + i;
		    }
		    edge++;
		}
	}
	
	for(j=0; j < UDIM; j++) {
	    j1 = j + 1;
		for(i=0; i < UDIM1; i++) {
		    i1 = i + 1;
		    if(i==0) {
		        edge->v[0] = j * UDIM1 + i;
		        edge->v[1] = j1 * UDIM1 + i;
		        edge->v[2] = j * UDIM1 + i1;
		        edge->v[3] = MAX_INDEX;
		    }
		    else if(i==UDIM) {
		        edge->v[0] = j1 * UDIM1 + i;
		        edge->v[1] = j * UDIM1 + i;
		        edge->v[2] = j1 * UDIM1 + i - 1;
		        edge->v[3] = MAX_INDEX;
		    }
		    else {
		        edge->v[0] = j1 * UDIM1 + i;
		        edge->v[1] = j * UDIM1 + i;
		        edge->v[2] = j1 * UDIM1 + i - 1;
		        edge->v[3] = j * UDIM1 + i1;
		    }
		    edge++;
		}
	}
	
	for(j=0; j < UDIM; j++) {
	    j1 = j + 1;
		for(i=0; i < UDIM; i++) {
		    i1 = i + 1;
		    edge->v[0] = j1 * UDIM1 + i;
		    edge->v[1] = j * UDIM1 + i1;
		    edge->v[2] = j  * UDIM1 + i;
		    edge->v[3] = j1 * UDIM1 + i1;
			edge++;
		}
	}
	
	
	m_edgeContactIndices = new CUDABuffer;
	m_edgeContactIndices->create(m_numEdges * sizeof(EdgeContact));
	m_edgeContactIndices->hostToDevice(m_edges->data(), m_edgeContactIndices->bufferSize());
	
	m_leafAabbs = new CUDABuffer;
	m_leafAabbs->create(numLeafNodes() * sizeof(Aabb));
	m_combinedAabb = new CUDABuffer;
	m_combinedAabb->create(ReduceMaxBlocks * sizeof(Aabb));
	
	m_leafHash[0] = new CUDABuffer;
	m_leafHash[0]->create(numLeafNodes() * sizeof(KeyValuePair));
	m_leafHash[1] = new CUDABuffer;
	m_leafHash[1]->create(numLeafNodes() * sizeof(KeyValuePair));
	
	m_internalNodeCommonPrefixValues = new CUDABuffer;
	m_internalNodeCommonPrefixValues->create(numInternalNodes() * sizeof(uint64));
	m_internalNodeCommonPrefixLengths = new CUDABuffer;
	m_internalNodeCommonPrefixLengths->create(numInternalNodes() * sizeof(int));
	
	m_leafNodeParentIndices = new CUDABuffer;
	m_leafNodeParentIndices->create(numLeafNodes() * sizeof(int));
	m_internalNodeChildIndices = new CUDABuffer;
	m_internalNodeChildIndices->create(numInternalNodes() * sizeof(int2));
	m_internalNodeParentIndices = new CUDABuffer;
	m_internalNodeParentIndices->create(numInternalNodes() * sizeof(int));
	m_rootNodeIndexOnDevice = new CUDABuffer;
	m_rootNodeIndexOnDevice->create(sizeof(int));

#ifdef BVHSOLVER_DBG_DRAW	
	m_displayAabbs = new BaseBuffer;
	m_displayAabbs->create(m_numEdges * sizeof(Aabb));
	m_displayCombinedAabb = new BaseBuffer;
	m_displayCombinedAabb->create(ReduceMaxBlocks * sizeof(Aabb));
	m_displayLeafHash = new BaseBuffer;
	m_displayLeafHash->create(numLeafNodes() * sizeof(KeyValuePair));
#endif

	m_lastReduceBlk = new BaseBuffer;
	m_lastReduceBlk->create(getReduceLastNThreads(m_numEdges) * sizeof(Aabb));
	
	qDebug()<<"num points "<<numVertices();
	qDebug()<<"num triangles "<<m_numTriangles;
	qDebug()<<"num edges "<<m_numEdges;
	qDebug()<<"num internal nodes "<<numInternalNodes();
	qDebug()<<"num leaf nodes "<<numLeafNodes();
}

void BvhSolver::stepPhysics(float dt)
{
	formPlane(m_alpha);
	combineAabb();
	formLeafAabbs();
	calcLeafHash();
	buildInternalTree();
}

void BvhSolver::formPlane(float alpha)
{
	void *dptr = m_vertexBuffer->bufferOnDevice();
	wavePlane((float3 *)dptr, UDIM, 2.0, alpha);
	m_vertexBuffer->deviceToHost(m_displayVertex->data(), m_vertexBuffer->bufferSize());
}

void BvhSolver::formLeafAabbs()
{
    void * cvs = m_vertexBuffer->bufferOnDevice();
    void * edges = m_edgeContactIndices->bufferOnDevice();
    void * dst = m_leafAabbs->bufferOnDevice();
    bvhCalculateLeafAabbs((Aabb *)dst, (float3 *)cvs, (EdgeContact *)edges, m_numEdges, numVertices());
    
#ifdef BVHSOLVER_DBG_DRAW
    m_leafAabbs->deviceToHost(m_displayAabbs->data(), m_leafAabbs->bufferSize());
#endif
}

void BvhSolver::combineAabb()
{
	void * psrc = m_vertexBuffer->bufferOnDevice();
    void * pdst = m_combinedAabb->bufferOnDevice();
	
	unsigned n = numVertices();
	unsigned threads, blocks;
	getReduceBlockThread(blocks, threads, n);
	
	// std::cout<<"n0 "<<n<<" blocks X threads : "<<blocks<<" X "<<threads<<"\n";
	
	bvhReduceAabbByPoints((Aabb *)pdst, (float3 *)psrc, n, blocks, threads);
	
	n = blocks;
	while(n > 1) {
		getReduceBlockThread(blocks, threads, n);
		
		// std::cout<<"n "<<n<<" blocks X threads : "<<blocks<<" X "<<threads<<"\n";
	
		bvhReduceAabbByAabb((Aabb *)pdst, (Aabb *)pdst, n, blocks, threads);
		
		n = (n + (threads*2-1)) / (threads*2);
	}
	
	m_combinedAabb->deviceToHost(m_lastReduceBlk->data(), m_lastReduceBlk->bufferSize());
	Aabb * c = (Aabb *)m_lastReduceBlk->data();
	m_bigAabb = c[0];

	
#ifdef BVHSOLVER_DBG_DRAW
	m_combinedAabb->deviceToHost(m_displayCombinedAabb->data(), m_combinedAabb->bufferSize());
#endif
}

void BvhSolver::calcLeafHash()
{
	void * dst = m_leafHash[0]->bufferOnDevice();
	void * src = m_leafAabbs->bufferOnDevice();
	bvhCalculateLeafHash((KeyValuePair *)dst, (Aabb *)src, numLeafNodes(), m_bigAabb);
	void * tmp = m_leafHash[1]->bufferOnDevice();
	RadixSort((KeyValuePair *)dst, (KeyValuePair *)tmp, numLeafNodes(), 32);
	
#ifdef BVHSOLVER_DBG_DRAW
	m_leafHash[0]->deviceToHost(m_displayLeafHash->data(), m_leafHash[0]->bufferSize()); 
#endif
}

void BvhSolver::buildInternalTree()
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
	
	bvhConnectInternalTreeNodes((uint64 *)commonPrefix, (int *)commonPrefixLengths,
								(int2 *)internalNodeChildIndex,
								(int *)internalNodeParentIndex,
								(int *)m_rootNodeIndexOnDevice->bufferOnDevice(),
								numInternalNodes());
								
	// TODO findDistanceFromRoot
	// TODO buildBinaryRadixTreeAabbsRecursive
	
	// m_rootNodeIndexOnDevice->deviceToHost((void *)&m_rootNodeIndex, m_rootNodeIndexOnDevice->bufferSize());
	// qDebug()<<"root node index "<<(m_rootNodeIndex & (~0x80000000));
	// printLeafInternalNodeConnection();
	// printInternalNodeConnection();
}

const unsigned BvhSolver::numVertices() const { return UDIM1 * UDIM1; }

unsigned BvhSolver::getNumTriangleFaceVertices() const { return m_numTriIndices; }
unsigned * BvhSolver::getIndices() const { return m_triIndices; }
float * BvhSolver::displayVertex() { return (float *)m_displayVertex->data(); }
EdgeContact * BvhSolver::edgeContacts() { return (EdgeContact *)m_edges->data(); }
const unsigned BvhSolver::numEdges() const { return m_numEdges; }

#ifdef BVHSOLVER_DBG_DRAW
Aabb * BvhSolver::displayAabbs() { return (Aabb *)m_displayAabbs->data(); }
Aabb * BvhSolver::displayCombinedAabb() { return (Aabb *)m_displayCombinedAabb->data(); }
KeyValuePair * BvhSolver::displayLeafHash() { return (KeyValuePair *)m_displayLeafHash->data(); }
#endif

void BvhSolver::setAlpha(float x) { m_alpha = x; }

const Aabb BvhSolver::combinedAabb() const { return m_bigAabb; }

const unsigned BvhSolver::numLeafNodes() const { return m_numEdges; }
const unsigned BvhSolver::numInternalNodes() const { return numLeafNodes() - 1; }

void BvhSolver::printLeafInternalNodeConnection()
{
	int * pfl = new int[numInternalNodes()];
	m_internalNodeCommonPrefixLengths->deviceToHost(pfl, m_internalNodeCommonPrefixLengths->bufferSize());
	
	int * lp = new int[numLeafNodes()];
	m_leafNodeParentIndices->deviceToHost(lp,  m_leafNodeParentIndices->bufferSize());
	 
	int2 * cp = new int2[numInternalNodes()];
	m_internalNodeChildIndices->deviceToHost(cp,  m_internalNodeChildIndices->bufferSize());
	
	qDebug()<<"\n leaf - internal node connection:\n";
	int ind = -1;
	for(unsigned i=0; i < numLeafNodes(); i++) {
		if(lp[i] != ind) {
			ind = lp[i];
			qDebug()<<"leaf["<<i<<"] : itl["<<ind<<"] <- ("<<cp[ind].x<<" , "<<cp[ind].y<<") "<<pfl[ind];
		}
		else 
			qDebug()<<"leaf["<<i<<"]";
	}
	delete[] lp;delete[] cp;
}

void BvhSolver::printInternalNodeConnection()
{
	int * ipi = new int[numInternalNodes()];
	m_internalNodeParentIndices->deviceToHost(ipi, m_internalNodeParentIndices->bufferSize());
	
	int2 * cp = new int2[numInternalNodes()];
	m_internalNodeChildIndices->deviceToHost(cp,  m_internalNodeChildIndices->bufferSize());
	
	qDebug()<<"\n internal node connection:\n";
	for(unsigned i=0; i < numInternalNodes(); i++) {
		qDebug()<<ipi[i]<<" <- i["<<i<<"] <- ("<<cp[i].x<<" , "<<cp[i].y<<")";
	}
	
	delete[] cp;
	delete[] ipi;
}
