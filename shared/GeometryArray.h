/*
 *  GeometryArray.h
 *  
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include <Geometry.h>

class GeometryArray : public Geometry {
public:
	GeometryArray();
	virtual ~GeometryArray();
	
	void create(unsigned n);
	void setGeometry(Geometry * geo, unsigned i);
	const unsigned numGeometies() const;
	Geometry * geometry(unsigned icomponent) const;
	virtual const unsigned numComponents() const;
	virtual const BoundingBox calculateBBox() const;
	virtual const BoundingBox calculateBBox(unsigned icomponent) const;
	virtual const Type type() const;
	
	void setComponentType(Type t);
	Type componentType() const;
protected:

private:
	Geometry ** m_geos;
	unsigned m_numGeometies;
	Type m_componentType;
};