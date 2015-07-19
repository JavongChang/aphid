/*
 *  FitBccMesh.cpp
 *  testbcc
 *
 *  Created by jian zhang on 4/27/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "FitBccMesh.h"
#include <BccGlobal.h>
#include "FitBccMeshBuilder.h"
FitBccMesh::FitBccMesh() 
{
}

FitBccMesh::~FitBccMesh() 
{
}

void FitBccMesh::create(GeometryArray * geoa, KdIntersection * anchorMesh)
{
	std::vector<Vector3F > tetrahedronP;
	std::vector<unsigned > tetrahedronInd;
	
	FitBccMeshBuilder builder;
	builder.build(geoa, tetrahedronP, tetrahedronInd);
	
	unsigned nt = tetrahedronInd.size()/4;
	unsigned np = tetrahedronP.size();
	
	// nt = 16;
	// np = 18;
	
	//std::cout<<" n tetrahedrons "<<nt<<"\n";
	//std::cout<<" n vertices "<<np<<"\n";
	
	setNumPoints(np);
	setNumIndices(nt * 4);
	createBuffer(np, nt * 4);
	
	unsigned i;
	for(i=0;i<np;i++) points()[i] = tetrahedronP[i];
	
	for(i=0;i<nt*4;i++) indices()[i] = tetrahedronInd[i];
	
	resetAnchors(np);
	
	addAnchors(builder.startPoints(), builder.tetrahedronDrifts(), geoa->numGeometries(), anchorMesh);
}

void FitBccMesh::addAnchors(Vector3F * anchorPoints, unsigned * tetraDrifts, unsigned n,
						KdIntersection * anchorMesh)
{
	unsigned i, j, k;
	unsigned * anchorTri = new unsigned[n];
	Geometry::ClosestToPointTestResult cls;
	for(i=0; i< n; i++) {
		cls.reset(anchorPoints[i], 1e8f);
		anchorMesh->closestToPoint(&cls);
		anchorTri[i] = cls._icomponent;
	}
	
	BoundingBox box;
	for(k = 0; k <n; k++) {
	for(i=0; i< 6; i++) {
        unsigned * tet = tetrahedronIndices(tetraDrifts[k] + i);
		
		box.reset();
        for(j=0; j< 4; j++)
            box.expandBy(points()[tet[j]], 1e-3f); 
        
		if(box.center().distanceTo(anchorPoints[k]) > box.radius())
			continue;
		
		for(j=0; j< 4; j++) {
			anchors()[tet[j]] = (1<<24 | anchorTri[k]);
		}
    }
	}
	delete[] anchorTri;
}
//:~