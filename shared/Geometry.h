/*
 *  Geometry.h
 *  kdtree
 *
 *  Created by jian zhang on 10/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <TypedEntity.h>
#include <BoundingBox.h>
class Geometry : public TypedEntity {
public:
	Geometry();
	virtual ~Geometry();
	
	virtual const unsigned numComponents() const;
	virtual const BoundingBox calculateBBox() const;
	virtual const BoundingBox calculateBBox(unsigned icomponent) const;
	virtual bool intersectBox(const BoundingBox & box);
	virtual bool intersectTetrahedron(const Vector3F * tet);
	virtual bool intersectBox(unsigned icomponent, const BoundingBox & box);
	virtual bool intersectTetrahedron(unsigned icomponent, const Vector3F * tet);
protected:
	
private:
};