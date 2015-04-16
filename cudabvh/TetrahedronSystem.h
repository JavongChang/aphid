#ifndef TETRAHEDRONSYSTEM_H
#define TETRAHEDRONSYSTEM_H

/*
 *  TetrahedronSystem.h
 *  cudabvh
 *
 *  Created by jian zhang on 2/14/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
class BaseBuffer;
class TetrahedronSystem {
public:
	TetrahedronSystem();
	virtual ~TetrahedronSystem();
	void create(const unsigned & maxNumTetrahedrons, const unsigned & maxNumPoints);
	void addPoint(float * src);
	void addTetrahedron(unsigned a, unsigned b, unsigned c, unsigned d);
	void setDensity(float x);
	const unsigned numTetrahedrons() const;
	const unsigned numPoints() const;
	const unsigned numTriangles() const;
	const unsigned numTriangleFaceVertices() const;
	float * hostX();
	float * hostXi();
	float * hostV();
	float * hostMass();
	unsigned * hostTretradhedronIndices();
	unsigned * hostTriangleIndices();
protected:
    void calculateMass();
    const unsigned maxNumPoints() const;
	const unsigned maxNumTetradedrons() const;
private:
    void addTriangle(unsigned a, unsigned b, unsigned c);
private:
	BaseBuffer * m_hostX;
	BaseBuffer * m_hostXi;
	BaseBuffer * m_hostV;
	BaseBuffer * m_hostMass;
	BaseBuffer * m_hostTretradhedronIndices;
	BaseBuffer * m_hostTriangleIndices;
	unsigned m_numTetrahedrons, m_numPoints, m_numTriangles;
	unsigned m_maxNumTetrahedrons, m_maxNumPoints, m_maxNumTriangles;
	float m_density;
};
#endif        //  #ifndef TETRAHEDRONSYSTEM_H
