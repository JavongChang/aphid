/*
 *  BaseMesh.h
 *  brdf
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllMath.h>
#include <Geometry.h>
#include <IntersectionContext.h>

class BaseMesh : public Geometry {
public:
	BaseMesh();
	virtual ~BaseMesh();
	
	void verbose() const;
	
	void createVertices(unsigned num);
	void createIndices(unsigned num);
	void createQuadIndices(unsigned num);
	void createPolygonCounts(unsigned num);
	void createPolygonIndices(unsigned num);
	
	unsigned processTriangleFromPolygon();
	unsigned processQuadFromPolygon();

	const BoundingBox calculateBBox() const;
	const BoundingBox calculateBBox(const unsigned &idx) const;
	const int faceOnSideOf(const unsigned &idx, const int &axis, const float &pos) const;
	
	Vector3F * vertices();
	Vector3F * normals();
	unsigned * indices();
	unsigned * quadIndices();
	
	void setVertex(unsigned idx, float x, float y, float z);
	void setTriangle(unsigned idx, unsigned a, unsigned b, unsigned c);
	void move(float x, float y, float z);
	
	void getTriangle(unsigned idx, unsigned *vertexId) const;
	
	unsigned getNumFaces() const;
	unsigned getNumQuads() const;
	unsigned getNumPolygons() const;
	unsigned getNumVertices() const;
	unsigned getNumFaceVertices() const;
	
	Vector3F * getVertices() const;
	Vector3F * getNormals() const;
	unsigned * getIndices() const;
	virtual Matrix33F getTangentFrame(const unsigned& idx) const;
	
	char intersect(unsigned idx, const Ray & ray, IntersectionContext * ctx) const;
	char intersect(const Ray & ray, IntersectionContext * ctx) const;
	char closestPoint(unsigned idx, const Vector3F & origin, IntersectionContext * ctx) const;
	char insideTriangle(const Vector3F & p, const Vector3F & a, const Vector3F & b, const Vector3F & c, const Vector3F & n) const;
	
	Vector3F * _vertices;
	Vector3F * m_normals;
	unsigned * _indices;
	unsigned * m_quadIndices;
	unsigned _numVertices;
	unsigned _numFaces;
	unsigned _numFaceVertices;
	unsigned m_numQuadVertices;
	unsigned m_numPolygons;
	unsigned m_numQuads;
	unsigned * m_polygonCounts;
	unsigned m_numPolygonVertices;
	unsigned * m_polygonIndices;
private:
};