/*
 *  BarbWorks.cpp
 *  mallard
 *
 *  Created by jian zhang on 1/1/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "BarbWorks.h"
#include "MlSkin.h"
#include "MlFeather.h"
#include "MlCalamus.h"
BarbWorks::BarbWorks() 
{
	m_skin = 0;
}

BarbWorks::~BarbWorks() {}

void BarbWorks::setSkin(MlSkin * skin)
{
	m_skin = skin;
}

MlSkin * BarbWorks::skin() const
{
	return m_skin;
}

unsigned BarbWorks::numFeathers() const
{
	return m_skin->numFeathers();
}

void BarbWorks::computeFeather(MlCalamus * c)
{
	Vector3F p;
	skin()->getPointOnBody(c, p);
	
	Matrix33F space;
	skin()->calamusSpace(c, space);
	computeFeather(c, p, space);
}

void BarbWorks::computeFeather(MlCalamus * c, const Vector3F & p, const Matrix33F & space)
{
	skin()->touchBy(c, p, space);
	c->bendFeather(p, space);
	c->curlFeather();
	c->computeFeatherWorldP(p, space);
}

char BarbWorks::isTimeCached(const std::string & stime)
{
	return isCached("/ang", stime) > 0;
}

void BarbWorks::openCache(const std::string & stime)
{
	useDocument();
	openEntry("/ang");
	openSliceFloat("/ang", stime);
	openEntry("/p");
	openSliceVector3("/p", stime);
	openEntry("/tang");
	openSliceMatrix33("/tang", stime);
}

void BarbWorks::closeCache(const std::string & stime)
{
	closeSlice("/ang", stime);
	closeEntry("/ang");
	closeSlice("/p", stime);
	closeEntry("/p");
	closeSlice("/tang", stime);
	closeEntry("/tang");
}

void BarbWorks::createBarbBuffer()
{
	const unsigned nc = numFeathers();
	unsigned i;
	
	BoundingBox box;
	Matrix33F space;
	Vector3F p;

	for(i = 0; i < nc; i++) {
		MlCalamus * c = skin()->getCalamus(i);
		skin()->calamusSpace(c, space);
		skin()->getPointOnBody(c, p);
		
		computeFeather(c, p, space);

		MlFeather * f = c->feather();
		f->getBoundingBox(box);
	}
}
