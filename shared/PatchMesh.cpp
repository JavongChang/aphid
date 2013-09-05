/*
 *  PatchMesh.cpp
 *  catmullclark
 *
 *  Created by jian zhang on 5/13/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "PatchMesh.h"
#include <PointInsidePolygonTest.h>
#include <InverseBilinearInterpolate.h>

PatchMesh::PatchMesh() 
{
    setEntityType(TypedEntity::TPatchMesh);
}

PatchMesh::~PatchMesh() 
{
	delete[] m_u;
	delete[] m_v;
	delete[] m_uvIds;
}

void PatchMesh::prePatchUV(unsigned numUVs, unsigned numUVIds)
{
	m_u = new float[numUVs];
	m_v = new float[numUVs];
	m_uvIds = new unsigned[numUVIds];
	m_numUVs = numUVs;
	m_numUVIds = numUVIds;
}

unsigned PatchMesh::numPatches() const
{
	return m_numQuads;
}

float * PatchMesh::us()
{
	return m_u;
}

float * PatchMesh::vs()
{
	return m_v;
}

unsigned * PatchMesh::uvIds()
{
	return m_uvIds;
}

const BoundingBox PatchMesh::calculateBBox(const unsigned &idx) const
{
    BoundingBox box;
	unsigned *qudi = &m_quadIndices[idx * 4];
	Vector3F *p0 = &_vertices[*qudi];
	qudi++;
	Vector3F *p1 = &_vertices[*qudi];
	qudi++;
	Vector3F *p2 = &_vertices[*qudi];
	qudi++;
	Vector3F *p3 = &_vertices[*qudi];
		
	box.updateMin(*p0);
	box.updateMax(*p0);
	box.updateMin(*p1);
	box.updateMax(*p1);
	box.updateMin(*p2);
	box.updateMax(*p2);
	box.updateMin(*p3);
	box.updateMax(*p3);

	return box;
}

char PatchMesh::intersect(unsigned idx, IntersectionContext * ctx) const
{
	PointInsidePolygonTest pa = patchAt(idx);
	
	if(!patchIntersect(pa, ctx)) return 0;
	
	postIntersection(idx, ctx);
	
	return 1;
}

char PatchMesh::patchIntersect(PointInsidePolygonTest & pa, IntersectionContext * ctx) const
{
	Vector3F px;
	if(!pa.intersect(ctx->m_ray, px)) return 0;
	
	float d = Vector3F(ctx->m_ray.m_origin, px).length();
	if(d > ctx->m_minHitDistance) return 0;
	
	InverseBilinearInterpolate invbil;
	invbil.setVertices(pa.vertex(0), pa.vertex(1), pa.vertex(3), pa.vertex(2));
	
	ctx->m_patchUV = invbil(px);
	ctx->m_hitP = px;
	pa.getNormal(ctx->m_hitN);
	ctx->m_minHitDistance = d;
	ctx->m_geometry = (Geometry*)this;
	ctx->m_success = 1;
	
	return 1;
}

unsigned PatchMesh::closestVertex(unsigned idx, const Vector3F & px) const
{
	unsigned *qudi = &m_quadIndices[idx * 4];
	float mag, minDist = 10e8;
	unsigned vert = 0;
	for(int i = 0; i < 4; i++) {
		Vector3F v = _vertices[*qudi] - px;
		
		mag = v.length();
		
		if(mag < minDist) {
			minDist = mag;
			vert = *qudi;
		}
		qudi++;
	}
	return vert;
}

char PatchMesh::closestPoint(unsigned idx, const Vector3F & origin, IntersectionContext * ctx) const
{
	PointInsidePolygonTest pa = patchAt(idx);
	
	Vector3F px;
	float d = pa.distanceTo(origin, px);
	
	if(d > ctx->m_minHitDistance) 
		return 0;
		
	ctx->m_minHitDistance = d;
	ctx->m_componentIdx = idx;
	ctx->m_closest = px;
	ctx->m_hitP = px;

	return 1;
}

PointInsidePolygonTest PatchMesh::patchAt(unsigned idx) const
{
	Vector3F po[4];
	unsigned *qudi = &m_quadIndices[idx * 4];
	po[0] = _vertices[*qudi];
	qudi++;
	po[1] = _vertices[*qudi];
	qudi++;
	po[2] = _vertices[*qudi];
	qudi++;
	po[3] = _vertices[*qudi];
	
	return PointInsidePolygonTest(po[0], po[1], po[2], po[3]);
}
//:~
