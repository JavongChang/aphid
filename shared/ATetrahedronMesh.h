/*
 *  ATetrahedronMesh.h
 *  aphid
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include "AGenericMesh.h"

class ATetrahedronMesh : public AGenericMesh {
public:
	ATetrahedronMesh();
	virtual ~ATetrahedronMesh();
	
	virtual const Type type() const;
	virtual const unsigned numComponents() const;
	virtual const BoundingBox calculateBBox(unsigned icomponent) const;
	const unsigned numTetrahedrons() const;
protected:
	
private:
	
};