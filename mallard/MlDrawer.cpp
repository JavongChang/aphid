/*
 *  MlDrawer.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/15/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "MlDrawer.h"
#include <AccPatchMesh.h>
#include <PointInsidePolygonTest.h>
MlDrawer::MlDrawer() {}
MlDrawer::~MlDrawer() {}

void MlDrawer::drawFeather(MlSkin * skin) const
{//printf("hit\n");
	const unsigned nf = skin->numFeathers();
	if(nf < 1) return;
	useDepthTest(0);
	setColor(1.f, 0.f, 0.f);
	AccPatchMesh * mesh = skin->bodyMesh();
	Vector3F p;
	for(unsigned i = 0; i < nf; i++) {
		MlCalamus * c = skin->getCalamus(i);
		mesh->pointOnPatch(c->faceIdx(), c->patchU(), c->patchV(), p);
		const PointInsidePolygonTest pa = mesh->patchAt(c->faceIdx());
		Matrix33F frm = pa.tangentFrame();
		
		Matrix33F space;
		space.rotateX(c->rotateX());
		
		space.multiply(frm);
		
		Vector3F d(0.f, 0.f, c->scale());
		d = space.transform(d);
		d = p + d;
		arrow(p, d);
		
	}
	
	drawActiveFeather(skin);
}

void MlDrawer::drawActiveFeather(MlSkin * skin) const
{
	const unsigned num = skin->numActiveFeather();
	if(num < 1) return;
	
	AccPatchMesh * mesh = skin->bodyMesh();
	Vector3F p;
	unsigned i;
	for(i = 0; i < num; i++) {
		MlCalamus * c = skin->getActive(i);
		mesh->pointOnPatch(c->faceIdx(), c->patchU(), c->patchV(), p);
		const PointInsidePolygonTest pa = mesh->patchAt(c->faceIdx());
		Matrix33F frm = pa.tangentFrame();
		
		Matrix33F space;
		space.rotateX(c->rotateX());
		
		space.multiply(frm);
		
		Vector3F d(0.f, 0.f, c->scale());
		d = space.transform(d);
		d = p + d;
		arrow(p, d);
	}
}
