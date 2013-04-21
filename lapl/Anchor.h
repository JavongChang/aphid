/*
 *  Anchor.h
 *  lapl
 *
 *  Created by jian zhang on 3/19/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <map>
#include <Vector3F.h>
#include <Matrix44F.h>
#include <SelectionArray.h>
class Ray;
class Anchor {
public:
	struct AnchorPoint {
		Vector3F p, worldP;
		float w;
	};
	Anchor();
	Anchor(SelectionArray & sel);
	virtual ~Anchor();
	
	void placeAt(const Vector3F & cen);
	
	void addPoint(unsigned vertexId);
	
	void setWeight(float wei);
	void addWeight(float delta);
	
	unsigned numPoints() const;
	AnchorPoint * firstPoint(unsigned &idx);
	AnchorPoint * nextPoint(unsigned &idx);
	bool hasPoint();
	void spaceMatrix(float m[16]) const;
	Vector3F getCenter() const;
	Vector3F displacement() const;
	bool intersect(const Ray &ray, float &t, float threshold) const;
	
	virtual void translate(Vector3F & dis);
	AnchorPoint *getPoint(unsigned idx) const;
	
private:
	std::map<unsigned, AnchorPoint *> m_anchorPoints;
	std::map<unsigned, AnchorPoint *>::iterator m_anchorPointIt;
	std::vector<AnchorPoint *> m_points;
	Matrix44F m_space;
	Matrix44F m_space0;
};
