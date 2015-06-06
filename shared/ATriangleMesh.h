#ifndef ATRIANGLEMESH_H
#define ATRIANGLEMESH_H

/*
 *  ATriangleMesh.h
 *  aphid
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include "AGenericMesh.h"

class ATriangleMesh : public AGenericMesh {
public:
	ATriangleMesh();
	virtual ~ATriangleMesh();
	
	virtual const Type type() const;
	virtual const unsigned numComponents() const;
	virtual const BoundingBox calculateBBox(unsigned icomponent) const;
	const unsigned numTriangles() const;
	
	void create(unsigned np, unsigned nt);
	unsigned * triangleIndices(unsigned idx) const;
	
	void setDagName(const std::string & name);
	const std::string dagName() const;
protected:
	
private:
	std::string m_dagName;
};
#endif        //  #ifndef ATRIANGLEMESH_H
