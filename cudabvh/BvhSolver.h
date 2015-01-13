/*
 *  BvhSolver.h
 *  
 *
 *  Created by jian zhang on 10/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <BaseSolverThread.h>
#include <bvh_common.h>
#include <radixsort_implement.h>
#include <app_define.h>

class BaseBuffer;
class CUDABuffer;

class BvhSolver : public BaseSolverThread
{
public:
	BvhSolver(QObject *parent = 0);
	virtual ~BvhSolver();
	
	void init();
	void createPoint(uint n);
	void createEdges(BaseBuffer * onhost, uint n);
	void createRays(uint m, uint n);
	
	const unsigned numPoints() const;
	const unsigned numLeafNodes() const;
	const unsigned numInternalNodes() const;
	const unsigned numRays() const;
	
	void setAlpha(float x);
	void setPlaneUDim(uint x);
	
	int getRootNodeIndex();
	void getRootNodeAabb(Aabb * dst); 
	void getPoints(BaseBuffer * dst);
	void getRays(BaseBuffer * dst);
	
#ifdef BVHSOLVER_DBG_DRAW
	Aabb * displayLeafAabbs();
	Aabb * displayInternalAabbs();
	KeyValuePair * displayLeafHash();
	int * displayInternalDistances();
	void hostInternalNodeChildIndex(int2 * ptr);
#endif
	
	

protected:
    virtual void stepPhysics(float dt);	
private:
	void formPlane();
	void formLeafAabbs();
	void combineAabb();
	void calcLeafHash();
	void buildInternalTree();
	void findMaxDistanceFromRoot();
	void formInternalTreeAabbsIterative();
	void formRays();
	void rayTraverse();
	
	void printLeafInternalNodeConnection();
	void printInternalNodeConnection();
	
private:
	CUDABuffer * m_vertexBuffer;
	CUDABuffer * m_edgeContactIndices;
	CUDABuffer * m_leafAabbs;
	CUDABuffer * m_internalNodeAabbs;
	CUDABuffer * m_leafHash[2];
	CUDABuffer * m_internalNodeCommonPrefixValues;
	CUDABuffer * m_internalNodeCommonPrefixLengths;
	CUDABuffer * m_leafNodeParentIndices;
	CUDABuffer * m_internalNodeChildIndices;
	CUDABuffer * m_internalNodeParentIndices;
	CUDABuffer * m_rootNodeIndexOnDevice;
    CUDABuffer * m_distanceInternalNodeFromRoot;
	CUDABuffer * m_reducedMaxDistance;
	CUDABuffer * m_rays;
	CUDABuffer * m_ntests;
    
#ifdef BVHSOLVER_DBG_DRAW
	BaseBuffer * m_displayLeafAabbs;
	BaseBuffer * m_displayInternalAabbs;
	BaseBuffer * m_displayInternalDistance;
	BaseBuffer * m_displayLeafHash;
#endif
	unsigned m_numPoints, m_numLeafNodes, m_numRays, m_rayDim, m_planeUDim;
	float m_alpha;
};