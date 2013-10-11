/*
 *  MlDrawer.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/15/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "MlDrawer.h"
#include "MlCalamus.h"
#include "MlFeather.h"
#include "MlSkin.h"
#include "MlTessellate.h"
#include <AccPatchMesh.h>
#include <PointInsidePolygonTest.h>
#include <HBase.h>
#include <sstream>
MlDrawer::MlDrawer()
{
	m_featherTess = new MlTessellate;
	m_currentFrame = -9999;
}

MlDrawer::~MlDrawer() 
{
	delete m_featherTess;
}

void MlDrawer::drawFeather(MlSkin * skin) const
{
	if(skin->numFeathers() > 0) drawBuffer();
}

void MlDrawer::hideAFeather(MlCalamus * c)
{
	const unsigned loc = c->bufferStart();
	
	setIndex(loc);
	
	m_featherTess->setFeather(c->feather());
	
	unsigned i;
	const unsigned nvpf = m_featherTess->numIndices();
	for(i = 0; i < nvpf; i++) {
		vertices()[0] = 0.f;
		vertices()[1] = 0.f;
		vertices()[2] = 0.f;
		next();
	}
}

void MlDrawer::hideActive(MlSkin * skin)
{
	const unsigned num = skin->numActive();
	if(num < 1) return;
	
	unsigned i;
	for(i = 0; i < num; i++) {
		MlCalamus * c = skin->getActive(i);
		hideAFeather(c);
	}
}

void MlDrawer::updateActive(MlSkin * skin)
{
	const unsigned num = skin->numActive();
	if(num < 1) return;
	
	unsigned i;
	for(i = 0; i < num; i++) {
		MlCalamus * c = skin->getActive(i);
		computeFeather(skin, c);
		updateBuffer(c);
	}
}

void MlDrawer::updateBuffer(MlCalamus * c)
{
	tessellate(c->feather());
	
	const unsigned nvpf = m_featherTess->numIndices();
	const unsigned startv = c->bufferStart();
	setIndex(startv);
	
	unsigned i, j;
	Vector3F v;
	for(i = 0; i < nvpf; i++) {
		j = m_featherTess->indices()[i];
		v = m_featherTess->vertices()[j];
		vertices()[0] = v.x;
		vertices()[1] = v.y;
		vertices()[2] = v.z;
		
		v = m_featherTess->normals()[j];
		normals()[0] = v.x;
		normals()[1] = v.y;
		normals()[2] = v.z;
		
		next();
	}
}

void MlDrawer::addToBuffer(MlSkin * skin)
{
	const unsigned num = skin->numCreated();
	if(num < 1) return;
	
	unsigned loc = taken();
	
	unsigned i, nvpf;
	Vector3F v;
	for(i = 0; i < num; i++) {
		MlCalamus * c = skin->getCreated(i);
		
		m_featherTess->setFeather(c->feather());
		
		setIndex(loc);
		c->setBufferStart(loc);
		
		nvpf = m_featherTess->numIndices();

		expandBy(nvpf);
		
		loc += nvpf;
	}
}

void MlDrawer::rebuildBuffer(MlSkin * skin)
{
	const unsigned nc = skin->numFeathers();
	if(nc < 1) return;
	
	std::stringstream sst;
	sst.str("");
	sst<<m_currentFrame;
	
	useDocument();
	//std::cout<<"draw hdoc "<<HObject::FileIO.fileName()<<"\n";
	openEntry("/p");
	openSlice("/p", sst.str());
	
	//std::cout<<"building feather draw buffer\n num feathers "<<nc<<"\n";
	unsigned loc = 0;
	
	unsigned i, nvpf;
	Vector3F v;
	for(i = 0; i < nc; i++) {
		MlCalamus * c = skin->getCalamus(i);
		
		m_featherTess->setFeather(c->feather());
		
		setIndex(loc);
		c->setBufferStart(loc);
		
		nvpf = m_featherTess->numIndices();

		expandBy(nvpf);
		
		loc += nvpf;
	}
	
	if(isCached("/p", sst.str())) {
		readFromCache(skin, sst.str());
	}
	else {
		std::cout<<"caching frame "<<sst.str()<<"\n";
		writeToCache(skin, sst.str());
	}
		
	closeSlice("/p", sst.str());
	closeEntry("/p");
}

void MlDrawer::computeFeather(MlSkin * skin, MlCalamus * c)
{
    Vector3F p;
	skin->getPointOnBody(c, p);
	c->collideWith(skin, p);
	
	Matrix33F space = skin->rotationFrame(c);
	c->computeFeatherWorldP(p, space);
}

void MlDrawer::tessellate(MlFeather * f)
{
	m_featherTess->setFeather(f);
	m_featherTess->evaluate(f);
}

void MlDrawer::setCurrentFrame(int x)
{
	m_currentFrame = x;
}

void MlDrawer::writeToCache(MlSkin * skin, const std::string & sliceName)
{
	const unsigned nc = skin->numFeathers();
	Vector3F wpb[8192];
	unsigned i, j, iblock = 0, ifull = 0;
	for(i = 0; i < nc; i++) {
		MlCalamus * c = skin->getCalamus(i);
		computeFeather(skin, c);
		
		MlFeather * f = c->feather();
		for(j = 0; j < f->numWorldP(); j++) {
			wpb[iblock] = f->worldP()[j];
			iblock++;
			ifull++;
			if(iblock == 8192) {
				writeSliceVector3("/p", sliceName, ifull - iblock, iblock, wpb);
				iblock = 0;
			}
		}
		
		updateBuffer(c);
	}
	
	if(iblock > 0)
		writeSliceVector3("/p", sliceName, ifull - iblock, iblock, wpb);
		
	saveEntrySize("/p", ifull);
	setCached("/p", sliceName, ifull);
}

void MlDrawer::readFromCache(MlSkin * skin, const std::string & sliceName)
{
	const unsigned nc = skin->numFeathers();
	const unsigned blockL = 2048;
	Vector3F wpb[blockL];
	unsigned i, j, iblock = 0, ifull = 0;
	readSliceVector3("/p", sliceName, 0, blockL, wpb);
	
	for(i = 0; i < nc; i++) {
		
		MlCalamus * c = skin->getCalamus(i);
		
		MlFeather * f = c->feather();
		
		Vector3F * dst = f->worldP();
		for(j = 0; j < f->numWorldP(); j++) {
			dst[j] = wpb[iblock];
			iblock++;
			ifull++;
			if(iblock == blockL) {
				readSliceVector3("/p", sliceName, ifull, blockL, wpb);
				iblock = 0;
			}
		}
		
		updateBuffer(c);
	}
}
