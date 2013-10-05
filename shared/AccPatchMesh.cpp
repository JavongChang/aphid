/*
 *  AccPatchMesh.cpp
 *  mallard
 *
 *  Created by jian zhang on 8/30/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "AccPatchMesh.h"
#include <accPatch.h>
#include <accStencil.h>
#include <MeshTopology.h>
#include <PointInsidePolygonTest.h>
#include <BiLinearInterpolate.h>
#include <InverseBilinearInterpolate.h>

AccPatchMesh::AccPatchMesh() 
{
	if(!AccPatch::stencil) {
		AccStencil* sten = new AccStencil();
		AccPatch::stencil = sten;
	}
}

AccPatchMesh::~AccPatchMesh() 
{
	delete[] m_bezier;
}

void AccPatchMesh::setup(MeshTopology * topo)
{
	Vector3F* cvs = getVertices();
	Vector3F* normal = getNormals();
	float* ucoord = us();
	float* vcoord = vs();
	unsigned * uvs = uvIds();
	const unsigned numFace = getNumFaces();

	AccStencil* sten = AccPatch::stencil;
	sten->setVertexPosition(cvs);
	sten->setVertexNormal(normal);
	
	sten->m_vertexAdjacency = topo->getTopology();

	m_bezier = new AccPatch[numFace];
	unsigned * quadV = quadIndices();
	for(unsigned j = 0; j < numFace; j++) {
		sten->m_patchVertices[0] = quadV[0];
		sten->m_patchVertices[1] = quadV[1];
		sten->m_patchVertices[2] = quadV[2];
		sten->m_patchVertices[3] = quadV[3];
		
		m_bezier[j].setTexcoord(ucoord, vcoord, &uvs[j * 4]);
		m_bezier[j].evaluateContolPoints();
		m_bezier[j].evaluateTangents();
		m_bezier[j].evaluateBinormals();
		
		quadV += 4;
	}
}

AccPatch* AccPatchMesh::beziers() const
{
	return m_bezier;
}

const BoundingBox AccPatchMesh::calculateBBox() const
{
	BoundingBox box;
	const unsigned numFace = getNumFaces();
	for(unsigned i = 0; i < numFace; i++) {
		box.expandBy(calculateBBox(i));
	}
	return box;
}

const BoundingBox AccPatchMesh::calculateBBox(const unsigned &idx) const
{
	return beziers()[idx].controlBBox();
}

char AccPatchMesh::intersect(unsigned idx, IntersectionContext * ctx) const
{
    PatchSplitContext split;
	split.reset();
    if(!recursiveBezierIntersect(&beziers()[idx], ctx, split, 0)) return 0;

	postIntersection(idx, ctx);
	
	return 1;
}

char AccPatchMesh::closestPoint(unsigned idx, const Vector3F & origin, IntersectionContext * ctx) const
{
	PatchSplitContext split;
	split.reset();
	ctx->m_elementHitDistance = 10e8;
	recursiveBezierClosestPoint(origin, &beziers()[idx], ctx, split, 0);
	return 1;
}

char AccPatchMesh::recursiveBezierIntersect(BezierPatch* patch, IntersectionContext * ctx, const PatchSplitContext split, int level) const
{
    Ray ray = ctx->m_ray;
    BoundingBox controlbox = patch->controlBBox();
	float hitt0, hitt1;
	if(!controlbox.intersect(ray, &hitt0, &hitt1))
		return 0;

	if(hitt1 > ctx->m_minHitDistance) return 0;
	
	if(level > 3 || controlbox.area() < .1f) {
	    Vector3F fourCorners[4];
	    fourCorners[0] = patch->_contorlPoints[0];
	    fourCorners[1] = patch->_contorlPoints[3];
	    fourCorners[2] = patch->_contorlPoints[15];
	    fourCorners[3] = patch->_contorlPoints[12];
		
		PointInsidePolygonTest pa(fourCorners[0], fourCorners[1], fourCorners[2], fourCorners[3]);
		if(!patchIntersect(pa, ctx)) return 0;
		
		BiLinearInterpolate bili;
		ctx->m_patchUV = bili.interpolate2(ctx->m_patchUV.x, ctx->m_patchUV.y, split.patchUV);
		//printf("uv %f %f\n", ctx->m_patchUV.x, ctx->m_patchUV.y);
		return 1;
	}
	
	level++;
	
	BezierPatch children[4];
	patch->decasteljauSplit(children);
	
	PatchSplitContext childUV[4];
	patch->splitPatchUV(split, childUV);
	if(recursiveBezierIntersect(&children[0], ctx, childUV[0], level)) return 1;
	if(recursiveBezierIntersect(&children[1], ctx, childUV[1], level)) return 1;
	if(recursiveBezierIntersect(&children[2], ctx, childUV[2], level)) return 1;
	if(recursiveBezierIntersect(&children[3], ctx, childUV[3], level)) return 1;
	
	return 0;
}

void AccPatchMesh::recursiveBezierClosestPoint(const Vector3F & origin, BezierPatch* patch, IntersectionContext * ctx, const PatchSplitContext split, int level) const
{//std::cout<<" l "<<level;
	Vector3F fourCorners[4];
	fourCorners[0] = patch->_contorlPoints[0];
	fourCorners[1] = patch->_contorlPoints[3];
	fourCorners[2] = patch->_contorlPoints[15];
	fourCorners[3] = patch->_contorlPoints[12];
	
	PointInsidePolygonTest pl(fourCorners[0], fourCorners[1], fourCorners[2], fourCorners[3]);
	Vector3F px;
	const float d = pl.distanceTo(origin, px);
	
	char converged = 0;
	if(d >= ctx->m_elementHitDistance) {
		if(d > ctx->m_minHitDistance)
		return;
	}
	else {
		if(ctx->m_minHitDistance - d < 10e3)
			converged = 1;
	}
		
	ctx->m_elementHitDistance = d;
		
	BoundingBox controlbox = patch->controlBBox();
	if(level > 3 || controlbox.area() < .1f || !pl.isPointInside(px) || converged) {
		if(d > ctx->m_minHitDistance) return;
		ctx->m_minHitDistance = d;
		//ctx->m_componentIdx = idx;
		ctx->m_closestP = px;
		ctx->m_hitP = px;
		//BiLinearInterpolate bili;
		//ctx->m_patchUV = bili.interpolate2(ctx->m_patchUV.x, ctx->m_patchUV.y, split.patchUV);
		return;
	}
	
	level++;
	
	BezierPatch children[4];
	patch->decasteljauSplit(children);
	
	PatchSplitContext childUV[4];
	patch->splitPatchUV(split, childUV);
	recursiveBezierClosestPoint(origin, &children[0], ctx, childUV[0], level);
	recursiveBezierClosestPoint(origin, &children[1], ctx, childUV[1], level);
	recursiveBezierClosestPoint(origin, &children[2], ctx, childUV[2], level);
	recursiveBezierClosestPoint(origin, &children[3], ctx, childUV[3], level);
}

void AccPatchMesh::pointOnPatch(unsigned idx, float u, float v, Vector3F & dst) const
{
	beziers()[idx].evaluateSurfacePosition(u, v, &dst);
}

void AccPatchMesh::normalOnPatch(unsigned idx, float u, float v, Vector3F & dst) const
{
	beziers()[idx].evaluateSurfaceNormal(u, v, &dst);
}

Matrix33F AccPatchMesh::tangentFrame(unsigned idx, float u, float v) const
{
	return beziers()[idx].tangentFrame(u, v);
}

void AccPatchMesh::pushPlane(unsigned idx, Patch::PushPlaneContext * ctx) const
{
	ctx->m_componentMaxAngle = -3.14f;
	recursiveBezierPushPlane(&beziers()[idx], ctx, 0);
}

void AccPatchMesh::recursiveBezierPushPlane(BezierPatch* patch, Patch::PushPlaneContext * ctx, int level) const
{
	Vector3F fourCorners[4];
	fourCorners[0] = patch->_contorlPoints[0];
	fourCorners[1] = patch->_contorlPoints[3];
	fourCorners[2] = patch->_contorlPoints[15];
	fourCorners[3] = patch->_contorlPoints[12];
	
	Patch pl(fourCorners[0], fourCorners[1], fourCorners[2], fourCorners[3]);
	if(!pl.pushPlane(ctx))
		return;
		
	BoundingBox controlbox = patch->controlBBox();
	if(level > 3 || controlbox.area() < .01f) {
		if(ctx->m_currentAngle > ctx->m_maxAngle) 
			ctx->m_maxAngle = ctx->m_currentAngle;

		return;
	}
	
	level++;
	
	BezierPatch children[4];
	patch->decasteljauSplit(children);
	
	recursiveBezierPushPlane(&children[0], ctx, level);
	recursiveBezierPushPlane(&children[1], ctx, level);
	recursiveBezierPushPlane(&children[2], ctx, level);
	recursiveBezierPushPlane(&children[3], ctx, level);
}
//:~