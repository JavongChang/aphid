/*
 *  BaseMesh.cpp
 *  brdf
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>

#include "BaseMesh.h"

BaseMesh::BaseMesh() : _vertices(0), _indices(0) 
{
	setMeshType();
}

BaseMesh::~BaseMesh()
{
	if(_vertices) delete[] _vertices;
	if(_indices) delete[] _indices;
}

void BaseMesh::createVertices(unsigned num)
{
	_vertices = new Vector3F[num];
	_numVertices = num;
}

void BaseMesh::createIndices(unsigned num)
{
	_indices = new unsigned[num];
	_numFaceVertices = num;
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

unsigned * BaseMesh::indices()
{
	return _indices;
}

unsigned BaseMesh::getNumFaces() const
{
	return _numFaceVertices / 3;
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

char BaseMesh::intersect(unsigned idx, const Ray & ray, Vector3F &hitP, Vector3F& hitN, float &hitD) const
{
	Vector3F a = _vertices[_indices[idx * 3]];
	Vector3F b = _vertices[_indices[idx * 3 + 1]];
	Vector3F c = _vertices[_indices[idx * 3 + 2]];
	Vector3F ab = b - a;
	Vector3F ac = c - a;
	Vector3F nor = ab.cross(ac);
	nor.normalize();
	
	float ddotn = ray.m_dir.dot(nor);
		
	if(ddotn > -10e-5) return 0;
	
	float t = (a.dot(nor) - ray.m_origin.dot(nor)) / ddotn;
	
	if(t < 0.f || t > ray.m_tmax) return 0;
	
	Vector3F onplane = ray.m_origin + ray.m_dir * t;
	Vector3F e01 = b - a;
	Vector3F x0 = onplane - a;
	if(e01.cross(x0).dot(nor) < 0.f) return 0;
	
	Vector3F e12 = c - b;
	Vector3F x1 = onplane - b;
	if(e12.cross(x1).dot(nor) < 0.f) return 0;
	
	Vector3F e20 = a - c;
	Vector3F x2 = onplane - c;
	if(e20.cross(x2).dot(nor) < 0.f) return 0;
	
	hitP = onplane;
	hitN = nor;
	hitD = t;
	
	return 1;
}
//:~
