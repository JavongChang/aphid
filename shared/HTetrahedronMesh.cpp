/*
 *  HTetrahedronMesh.cpp
 *  testbcc
 *
 *  Created by jian zhang on 4/23/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "HTetrahedronMesh.h"
#include <AllHdf.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <CurveGroup.h>
#include <BaseBuffer.h>
#include <ATetrahedronMesh.h>

HTetrahedronMesh::HTetrahedronMesh(const std::string & path) : HBase(path) 
{
}

HTetrahedronMesh::~HTetrahedronMesh() {}

char HTetrahedronMesh::verifyType()
{
	if(!hasNamedAttr(".nt"))
		return 0;

	if(!hasNamedAttr(".nv"))
		return 0;
	
	return 1;
}

char HTetrahedronMesh::save(ATetrahedronMesh * tetra)
{
	int nv = tetra->numPoints();
	if(!hasNamedAttr(".nv"))
		addIntAttr(".nv");
	
	writeIntAttr(".nv", &nv);
	
	int nt = tetra->numTetrahedrons();
	if(!hasNamedAttr(".nt"))
		addIntAttr(".nt");
	
	writeIntAttr(".nt", &nt);
	
	if(!hasNamedData(".p"))
	    addVector3Data(".p", nv);
	
	writeVector3Data(".p", nv, (Vector3F *)tetra->points());
	
	if(!hasNamedData(".a"))
	    addIntData(".a", nv);
	
	writeIntData(".a", nv, (int *)tetra->anchors());
		
	if(!hasNamedData(".v"))
	    addIntData(".v", nt * 4);
	
	writeIntData(".v", nt * 4, (int *)tetra->indices());

	return 1;
}

char HTetrahedronMesh::load(ATetrahedronMesh * tetra)
{
	if(!verifyType()) return false;
	int nv = 4;
	
	readIntAttr(".nv", &nv);
	
	int nt = 1;
	
	readIntAttr(".nt", &nt);
	
	tetra->create(nv, nt);
	
	readVector3Data(".p", nv, (Vector3F *)tetra->points());
	readIntData(".a", nv, (unsigned *)tetra->anchors());
	readIntData(".v", nt * 4, (unsigned *)tetra->indices());
	
	return 1;
}
//:~