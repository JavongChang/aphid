/*
 *  BaseMesh.cpp
 *  brdf
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <BarycentricCoordinate.h>
#include "BaseMesh.h"

BaseMesh::BaseMesh()
{
	_vertices = 0;
	_indices = 0;
	m_normals = 0;
	m_quadIndices = 0;
	m_polygonCounts = 0;
	m_polygonIndices = 0;
	m_realEdges = 0;
	setMeshType();
}

BaseMesh::~BaseMesh()
{
	if(_vertices) delete[] _vertices;
	if(_indices) delete[] _indices;
	if(m_normals) delete[] m_normals;
	if(m_quadIndices) delete[] m_quadIndices;
	if(m_polygonCounts) delete[] m_polygonCounts;
	if(m_polygonIndices) delete[] m_polygonIndices;
	if(m_realEdges) delete[] m_realEdges;
}

void BaseMesh::createVertices(unsigned num)
{
	_vertices = new Vector3F[num];
	m_normals = new Vector3F[num];
	_numVertices = num;
}

void BaseMesh::createIndices(unsigned num)
{
	_indices = new unsigned[num];
	m_realEdges = new char[num];
	_numFaceVertices = num;
}

void BaseMesh::createQuadIndices(unsigned num)
{
	m_quadIndices = new unsigned[num];
	m_numQuadVertices = num;
}

void BaseMesh::createPolygonCounts(unsigned num)
{
    m_numPolygons = num;
    m_polygonCounts = new unsigned[num];
}

void BaseMesh::createPolygonIndices(unsigned num)
{
    m_numPolygonVertices = num;
    m_polygonIndices = new unsigned[num];
}

unsigned BaseMesh::processTriangleFromPolygon()
{
	_numFaces = 0;
	
	unsigned i, j;
    for(i = 0; i < m_numPolygons; i++)
        _numFaces += m_polygonCounts[i] - 2;
		
	createIndices(_numFaces * 3);
    
    unsigned curTri = 0;
    unsigned curFace = 0;
    for(i = 0; i < m_numPolygons; i++) {
        for(j = 1; j < m_polygonCounts[i] - 1; j++) {
            _indices[curTri] = m_polygonIndices[curFace];
            _indices[curTri + 1] = m_polygonIndices[curFace + j];
            _indices[curTri + 2] = m_polygonIndices[curFace + j + 1];
            curTri += 3;
        }
        curFace += m_polygonCounts[i];
    }
	
	return _numFaces;
}

unsigned BaseMesh::processQuadFromPolygon()
{
	unsigned i, j;
	m_numQuads = 0;
    for(i = 0; i < m_numPolygons; i++) {
		if(m_polygonCounts[i] < 5)
			m_numQuads++;
	}
		
	if(m_numQuads < 1) return 0;
	
	createQuadIndices(m_numQuads * 4);
	
	unsigned ie = 0;
	unsigned curFace = 0;
	for(i = 0; i < m_numPolygons; i++) {
		if(m_polygonCounts[i] == 4) {
			for(j = 0; j < 4; j++) {
				m_quadIndices[ie] = m_polygonIndices[curFace + j];
				ie++;
			}
		}
		else if(m_polygonCounts[i] == 3) {
			for(j = 0; j < 3; j++) {
				m_quadIndices[ie] = m_polygonIndices[curFace + j];
				ie++;
			}
			m_quadIndices[ie] = m_quadIndices[ie - 3];
			ie++;
		}
		curFace += m_polygonCounts[i];
	}
	
	return m_numQuads;
}

void BaseMesh::processRealEdgeFromPolygon()
{
	unsigned i, j;
	unsigned curTri = 0;
	unsigned ntri;
	for(i = 0; i < m_numPolygons; i++) {
		ntri = m_polygonCounts[i] - 2;
		
        for(j = 0; j < ntri; j++) {
			m_realEdges[curTri + j * 3] = 0;
			m_realEdges[curTri + j * 3 + 1] = 1;
			m_realEdges[curTri + j * 3 + 2] = 0;
        }
		
		m_realEdges[curTri] = 1;
		m_realEdges[curTri + ntri * 3 - 1] = 1;
		
		curTri += ntri * 3;
    }
}

const BoundingBox BaseMesh::calculateBBox() const
{
	BoundingBox box;
	const unsigned nv = getNumVertices();
	for(unsigned i = 0; i < nv; i++) {
		box.updateMin(_vertices[i]);
		box.updateMax(_vertices[i]);
	}
	return box;
}

const BoundingBox BaseMesh::calculateBBox(const unsigned &idx) const
{
	BoundingBox box;
	unsigned *trii = &_indices[idx * 3];
	Vector3F *p0 = &_vertices[*trii];
	trii++;
	Vector3F *p1 = &_vertices[*trii];
	trii++;
	Vector3F *p2 = &_vertices[*trii];
		
	box.updateMin(*p0);
	box.updateMax(*p0);
	box.updateMin(*p1);
	box.updateMax(*p1);
	box.updateMin(*p2);
	box.updateMax(*p2);

	return box;
}

const int BaseMesh::faceOnSideOf(const unsigned &idx, const int &axis, const float &pos) const
{
	unsigned *trii = &_indices[idx * 3];
	Vector3F *p0 = &_vertices[*trii];
	trii++;
	Vector3F *p1 = &_vertices[*trii];
	trii++;
	Vector3F *p2 = &_vertices[*trii];
	if(axis == 0) {
		if(p0->x < pos && p1->x < pos && p2->x < pos)
			return 0;
		if(p0->x >= pos && p1->x >= pos && p2->x >= pos)
			return 2;
		return 1;
	}
	else if(axis == 1) {
		if(p0->y < pos && p1->y < pos && p2->y < pos)
			return 0;
		if(p0->y >= pos && p1->y >= pos && p2->y >= pos)
			return 2;
		return 1;
	}
	if(p0->z < pos && p1->z < pos && p2->z < pos)
		return 0;
	if(p0->z >= pos && p1->z >= pos && p2->z >= pos)
		return 2;
	return 1;
}

Vector3F * BaseMesh::vertices()
{
	return _vertices;
}

Vector3F * BaseMesh::normals()
{
	return m_normals;
}

unsigned * BaseMesh::indices()
{
	return _indices;
}

unsigned * BaseMesh::quadIndices()
{
    return m_quadIndices;
}

void BaseMesh::setVertex(unsigned idx, float x, float y, float z)
{
	Vector3F *v = _vertices;
	v += idx;
	v->x = x;
	v->y = y;
	v->z = z;
}

void BaseMesh::setTriangle(unsigned idx, unsigned a, unsigned b, unsigned c)
{
	unsigned *i = _indices;
	i += idx * 3;
	*i = a;
	i++;
	*i = b;
	i++;
	*i = c;
}

void BaseMesh::move(float x, float y, float z)
{
	const unsigned nv = getNumVertices();
	for(unsigned i = 0; i < nv; i++) {
		_vertices[i].x += x;
		_vertices[i].y += y;
		_vertices[i].y += z;
	}
}

unsigned BaseMesh::getNumFaces() const
{
	return _numFaceVertices / 3;
}

unsigned BaseMesh::getNumQuads() const
{
	return m_numQuads;
}

unsigned BaseMesh::getNumVertices() const
{
	return _numVertices;
}

unsigned BaseMesh::getNumFaceVertices() const
{
	return _numFaceVertices;
}

Vector3F * BaseMesh::getVertices() const
{
	return _vertices;
}

Vector3F * BaseMesh::getNormals() const
{
	return m_normals;
}

unsigned * BaseMesh::getIndices() const
{
	return _indices;
}

void BaseMesh::verbose() const
{
    printf("mesh parameters:\n  face count:%d\n", _numFaces);
    printf("  face vertex count:%d\n", _numFaceVertices);
    printf("  vertex count:%d\n", _numVertices);
    printf("  face connection:");
    for(unsigned i = 0; i < _numFaceVertices; i++)
        printf(" %d", _indices[i]);
}

Matrix33F BaseMesh::getTangentFrame(const unsigned& idx) const
{
	return Matrix33F();
}

char BaseMesh::intersect(unsigned idx, const Ray & ray, IntersectionContext * ctx) const
{
	Vector3F a = _vertices[_indices[idx * 3]];
	Vector3F b = _vertices[_indices[idx * 3 + 1]];
	Vector3F c = _vertices[_indices[idx * 3 + 2]];
	Vector3F ab = b - a;
	Vector3F ac = c - a;
	Vector3F nor = ab.cross(ac);
	nor.normalize();
	
	float ddotn = ray.m_dir.dot(nor);
		
	if(ddotn > 0.f) return 0;
	
	float t = (a.dot(nor) - ray.m_origin.dot(nor)) / ddotn;
	
	if(t < 0.f || t > ray.m_tmax) return 0;
	
	//printf("face %i %f %f", idx, t, ctx->m_minHitDistance);
	
	if(t > ctx->m_minHitDistance) return 0;
	
	Vector3F onplane = ray.m_origin + ray.m_dir * t;
	Vector3F e01 = b - a;
	Vector3F x0 = onplane - a;
	if(e01.cross(x0).dot(nor) < 0.f) return 0;
	
	//printf("pass a\n");

	Vector3F e12 = c - b;
	Vector3F x1 = onplane - b;
	if(e12.cross(x1).dot(nor) < 0.f) return 0;
	
	//printf("pass b\n");
	
	Vector3F e20 = a - c;
	Vector3F x2 = onplane - c;
	if(e20.cross(x2).dot(nor) < 0.f) return 0;
	
	//printf("pass c\n");
	
	ctx->m_hitP = onplane;
	ctx->m_hitN = nor;
	ctx->m_minHitDistance = t;
	ctx->m_geometry = (Geometry*)this;
	ctx->m_componentIdx = idx;

	if(ctx->getComponentFilterType() == PrimitiveFilter::TFace) {
	    ctx->m_componentIdx = idx;
	}
	else {
	    ctx->m_componentIdx = _indices[idx * 3];
	    float mind = (a - onplane).length();
	    float d = (b - onplane).length();
	    if(d < mind) {
	        ctx->m_componentIdx = _indices[idx * 3 + 1];
	        mind = d;
	    }
	    d = (c - onplane).length();
	    if(d < mind)
	        ctx->m_componentIdx = _indices[idx * 3 + 2];
	}
	
	return 1;
}

char BaseMesh::intersect(const Ray & ray, IntersectionContext * ctx) const
{
	unsigned nf = getNumFaces();
	for(unsigned i = 0; i < nf; i++) {
		if(intersect(i, ray, ctx)) return 1;
	}
	return 0;
}

void BaseMesh::getTriangle(unsigned idx, unsigned *vertexId) const
{
	unsigned *i = _indices;
	i += idx * 3;
	vertexId[0] = *i;
	i++;
	vertexId[1] = *i;
	i++;
	vertexId[2] = *i;
}

char BaseMesh::closestPoint(unsigned idx, const Vector3F & origin, IntersectionContext * ctx) const
{
	Vector3F a = _vertices[_indices[idx * 3]];
	Vector3F b = _vertices[_indices[idx * 3 + 1]];
	Vector3F c = _vertices[_indices[idx * 3 + 2]];
	
	BarycentricCoordinate bar;
	bar.create(a, b, c);
	float d = bar.project(origin);
	if(d >= ctx->m_minHitDistance) return 0;
	
	if(ctx->m_enableNormalRef) {
		if(bar.getNormal().dot(ctx->m_refN) < 0.1f) return 0;
	}
	
	bar.compute();
	if(!bar.insideTriangle()) {
		bar.computeClosest();
	}
	
	char hit = 0;
	Vector3F clampledP = bar.getClosest();
	d = (clampledP - origin).length();

	if(d < ctx->m_minHitDistance) {
		ctx->m_minHitDistance = d;
		ctx->m_componentIdx = idx;
		ctx->m_coord[0] = bar.getV(0);
		ctx->m_coord[1] = bar.getV(1);
		ctx->m_coord[2] = bar.getV(2);
		ctx->m_closest = clampledP;
		
		ctx->m_hitP = clampledP;
		hit = 1;
	}
	return hit;
}

char BaseMesh::insideTriangle(const Vector3F & p, const Vector3F & a, const Vector3F & b, const Vector3F & c, const Vector3F & n) const
{
	Vector3F e01 = b - a;
	Vector3F x0 = p - a;
	if(e01.cross(x0).dot(n) < 0.f) return 0;
	
	Vector3F e12 = c - b;
	Vector3F x1 = p - b;
	if(e12.cross(x1).dot(n) < 0.f) return 0;
	
	Vector3F e20 = a - c;
	Vector3F x2 = p - c;
	if(e20.cross(x2).dot(n) < 0.f) return 0;
	
	return 1;
}
//:~
