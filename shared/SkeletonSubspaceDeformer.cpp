/*
 *  SkeletonSubspaceDeformer.cpp
 *  aphid
 *
 *  Created by jian zhang on 10/25/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "SkeletonSubspaceDeformer.h"
#include <SkeletonSystem.h>
#include <SkeletonJoint.h>
SkeletonSubspaceDeformer::SkeletonSubspaceDeformer() 
{
	m_skeleton = 0;
	m_jointIds = 0;
	m_subspaceP = 0;
	m_jointWeights = 0;
}

SkeletonSubspaceDeformer::~SkeletonSubspaceDeformer() {}

void SkeletonSubspaceDeformer::clear()
{
	if(m_jointIds) delete[] m_jointIds;
	if(m_subspaceP) delete[] m_subspaceP;
	if(m_jointWeights) delete[] m_jointWeights;
	m_jointIds = 0;
	m_subspaceP = 0;
	m_jointWeights = 0;
	BaseDeformer::clear();
}

void SkeletonSubspaceDeformer::setMesh(BaseMesh * mesh)
{
	BaseDeformer::setMesh(mesh);
	m_jointIds = new VectorN<unsigned>[numVertices()];
	//m_subspaceP = new Vector3F[numVertices()];
}

void SkeletonSubspaceDeformer::bindToSkeleton(SkeletonSystem * skeleton)
{
	m_skeleton = skeleton;
	std::vector<float> ws;
	for(unsigned i = 0; i < numVertices(); i++)
		bindVertexToSkeleton(i, ws);
	
	const unsigned n = ws.size();
	
	m_jointWeights = new float[n];
	m_subspaceP = new Vector3F[n];
	
	for(unsigned i = 0; i < n; i++)
		m_jointWeights[i] = ws[i];
	
	ws.clear();
	
	calculateSubspaceP();
}

void SkeletonSubspaceDeformer::bindVertexToSkeleton(unsigned vi, std::vector<float> & wei)
{
	const Vector3F vp = getDeformedP()[vi];
	Matrix44F spaceInv;
	VectorN<unsigned> ids;
	VectorN<float> weights;
	m_skeleton->calculateBindWeights(vp, ids, weights);
	
	unsigned n0;
	unsigned n = ids._ndim;
	
	m_jointIds[vi].setZero(n + 1);
	
	for(unsigned i=0; i < n; i++) *m_jointIds[vi].at(i) = ids[i];

	if(vi == 0) *m_jointIds[vi].at(n) = 0;
	else {
		n0 = m_jointIds[vi - 1]._ndim - 1;
		*m_jointIds[vi].at(n) = m_jointIds[vi - 1][n0] + n0;
	}
	
	for(unsigned i=0; i < n; i++) wei.push_back(weights[i]);
}

void SkeletonSubspaceDeformer::calculateSubspaceP()
{
	unsigned i, j, n, nj, vstart;
	Matrix44F spaceInv;
	Vector3F p;
	for(i = 0; i < numVertices(); i++) {
		p = getDeformedP()[i];
		n = m_jointIds[i]._ndim;
		nj = n - 1;
		vstart = m_jointIds[i][nj];
		
		for(j = 0; j < nj; j++) {
			SkeletonJoint * joint = m_skeleton->jointByIndex(m_jointIds[i][j]);
			spaceInv = joint->worldSpace();
			spaceInv.inverse();
			m_subspaceP[vstart + j] = spaceInv.transform(p);
		}
	}
}

char SkeletonSubspaceDeformer::solve()
{
	if(!m_skeleton) return 0;
	unsigned i, j, n, nj, vstart;
	Matrix44F space;
	Vector3F * p = deformedP();
	for(i = 0; i < numVertices(); i++) {
		n = m_jointIds[i]._ndim;
		nj = n - 1;
		vstart = m_jointIds[i][nj];
		
		Vector3F &q = p[i];
		q.setZero();
		for(j = 0; j < nj; j++) {
			SkeletonJoint * joint = m_skeleton->jointByIndex(m_jointIds[i][j]);
			space = joint->worldSpace();
			q += space.transform(m_subspaceP[vstart + j]) * m_jointWeights[vstart + j];
		}
	}
	return 1;
}
