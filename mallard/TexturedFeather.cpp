/*
 *  TexturedFeather.cpp
 *  mallard
 *
 *  Created by jian zhang on 12/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "TexturedFeather.h"
#include "MlVane.h"
ZEXRImage TexturedFeather::ColorTextureFile;
TexturedFeather::TexturedFeather() 
{
	m_vane = new MlVane[2];
}

TexturedFeather::~TexturedFeather() 
{
	delete[] m_vane;
}

void TexturedFeather::computeTexcoord()
{
	BaseFeather::computeTexcoord();
	
	if(m_vane[0].gridU() != numSegment())
		m_vane[0].create(numSegment(), 3);
	if(m_vane[1].gridU() != numSegment())
		m_vane[1].create(numSegment(), 3);
	
	const short numSeg = numSegment();
	for(short i = 0; i <= numSeg; i++) {	
		*m_vane[0].railCV(i, 0) = Vector3F(*segmentQuillTexcoord(i));
		*m_vane[1].railCV(i, 0) = Vector3F(*segmentQuillTexcoord(i));
		for(short j = 0; j < 3; j++) {
			*m_vane[0].railCV(i, j + 1) = Vector3F(*segmentVaneTexcoord(i, 0, j));
			*m_vane[1].railCV(i, j + 1) = Vector3F(*segmentVaneTexcoord(i, 1, j));
		}
	}

	m_vane[0].computeKnots();
	m_vane[1].computeKnots();
}

void TexturedFeather::translateUV(const Vector2F & d)
{
	BaseFeather::translateUV(d);
	const short numSeg = numSegment();
	for(short i = 0; i <= numSeg; i++) {	
		*m_vane[0].railCV(i, 0) = Vector3F(*segmentQuillTexcoord(i));
		*m_vane[1].railCV(i, 0) = Vector3F(*segmentQuillTexcoord(i));
		for(short j = 0; j < 3; j++) {
			*m_vane[0].railCV(i, j + 1) = Vector3F(*segmentVaneTexcoord(i, 0, j));
			*m_vane[1].railCV(i, j + 1) = Vector3F(*segmentVaneTexcoord(i, 1, j));
		}
	}
}

MlVane * TexturedFeather::uvVane(short side) const
{
	return &m_vane[side];
}

void TexturedFeather::sampleColor(unsigned gridU, unsigned gridV, Vector3F * dst)
{
	if(!ColorTextureFile.isOpened()) {
		const unsigned ns = (gridU + 1) * (gridV + 1) * 2;
		for(unsigned i = 0; i < ns; i++) dst[i].set(1.f, 1.f, 1.f);
		return;
	}
	
	const float du = 1.f/(float)gridU;
	const float dv = 1.f/(float)gridV;
	
	unsigned acc = 0;
	Vector3F coord;
	for(unsigned i = 0; i <= gridU; i++) {
		m_vane[0].setU(du*i);
		for(unsigned j = 0; j <= gridV; j++) {
			m_vane[0].pointOnVane(dv * j, coord);
			ColorTextureFile.sample(coord.x, coord.y, 3, (float *)&dst[acc]);
			acc++;
		}
	}
	
	for(unsigned i = 0; i <= gridU; i++) {
		m_vane[1].setU(du*i);
		for(unsigned j = 0; j <= gridV; j++) {
			m_vane[1].pointOnVane(dv * j, coord);
			ColorTextureFile.sample(coord.x, coord.y, 3, (float *)&dst[acc]);
			acc++;
		}
	}
}
