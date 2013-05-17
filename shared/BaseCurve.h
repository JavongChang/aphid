/*
 *  BaseCurve.h
 *  fit
 *
 *  Created by jian zhang on 4/22/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <AllMath.h>
#include <vector>

class BaseCurve {
public:
	BaseCurve();
	virtual ~BaseCurve();
	
	void cleanup();
	
	void addVertex(const Vector3F & vert);
	unsigned numVertices() const;
	void computeKnots();
	
	Vector3F getCv(unsigned idx) const;
	float getKnot(unsigned idx) const;
	
	void fitInto(BaseCurve & another);
	
	virtual Vector3F interpolate(float param, Vector3F * data) const;
	virtual Vector3F interpolateByKnot(float param, Vector3F * data) const;
	
	Vector3F calculateStraightPoint(float param, unsigned k0, unsigned k1, Vector3F * data) const;
	
	void findNeighborKnots(float param, unsigned & nei0, unsigned & nei1) const;
	
	std::vector<Vector3F> m_vertices;
	Vector3F * m_cvs;
	float * m_knots;
private:
	float m_length;
	unsigned m_numVertices;
};