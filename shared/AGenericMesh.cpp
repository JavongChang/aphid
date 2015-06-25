/*
 *  AGenericMesh.cpp
 *  aphid
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "AGenericMesh.h"
#include "BaseBuffer.h"

AGenericMesh::AGenericMesh() 
{
	m_points = new BaseBuffer;
	m_indices = new BaseBuffer;
	m_anchors = new BaseBuffer;
	m_numPoints = m_numIndices = 0;
}

AGenericMesh::~AGenericMesh() 
{
	delete m_points;
	delete m_indices;
	delete m_anchors;
}

const TypedEntity::Type AGenericMesh::type() const
{ return TGenericMesh; }

const BoundingBox AGenericMesh::calculateBBox() const
{
	BoundingBox box;
	const unsigned nv = numPoints();
	Vector3F * p = points();
	for(unsigned i = 0; i < nv; i++) {
		box.updateMin(p[i]);
		box.updateMax(p[i]);
	}
	return box;
}

void AGenericMesh::createBuffer(unsigned np, unsigned ni)
{
	m_points->create(np * 12);
	m_indices->create(ni * 4);
	m_anchors->create(np * 4);
}

Vector3F * AGenericMesh::points() const
{ return (Vector3F *)m_points->data(); }
	
unsigned * AGenericMesh::indices() const
{ return (unsigned *)m_indices->data(); }

unsigned * AGenericMesh::anchors() const
{ return (unsigned *)m_anchors->data(); }

void AGenericMesh::setNumPoints(unsigned n)
{ m_numPoints = n; }

void AGenericMesh::setNumIndices(unsigned n)
{ m_numIndices = n; }

const unsigned AGenericMesh::numPoints() const
{ return m_numPoints; }

const unsigned AGenericMesh::numIndices() const
{ return m_numIndices; }

void AGenericMesh::resetAnchors(unsigned n)
{
	unsigned * anchor = (unsigned *)m_anchors->data();
	unsigned i=0;
	for(; i < n; i++)
		anchor[i] = 0;
}

void AGenericMesh::copyStripe(AGenericMesh * inmesh, unsigned driftP, unsigned driftI)
{
	Vector3F * dstP = & ((Vector3F *)m_points->data())[driftP];
	unsigned * dstA = & ((unsigned *)m_anchors->data())[driftP];
	unsigned * dstI = & ((unsigned *)m_indices->data())[driftI];
	const unsigned np = inmesh->numPoints();
	const unsigned ni = inmesh->numIndices();
	unsigned i=0;
	Vector3F * srcP = inmesh->points();
	unsigned * srcA = inmesh->anchors();
	unsigned * srcI = inmesh->indices();
	for(;i<np;i++) dstP[i] = srcP[i]; 
	for(i=0;i<np;i++) dstA[i] = srcA[i]; 
	for(i=0;i<ni;i++) dstI[i] = driftP + srcI[i]; 
}
//:~