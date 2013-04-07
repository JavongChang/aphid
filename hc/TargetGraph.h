/*
 *  TargetGraph.h
 *  hc
 *
 *  Created by jian zhang on 4/7/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <BaseMesh.h>
#include <BarycentricCoordinate.h>
class TargetGraph : public BaseMesh {
public:
	TargetGraph();
	virtual ~TargetGraph();
	void createVertexWeights(unsigned num);
	void createTargetIndices(unsigned num);
	void setTargetTriangle(unsigned idx, unsigned a, unsigned b, unsigned c);
	void setControlId(unsigned idx);
	void initCoords();
	void reset();
	void computeWeight(unsigned faceIdx, const Vector3F & pos);
	Vector3F getHandlePos() const;

private:
	unsigned m_controlId;
	float * m_vertexWeights;
	unsigned * m_targetIndices;
	BarycentricCoordinate * m_baryc;
	Vector3F m_handlePos;
};