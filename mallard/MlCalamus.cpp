/*
 *  MlCalamus.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/14/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "MlCalamus.h"

MlCalamus::MlCalamus() 
{
	m_patchU = m_patchV = m_rotX = m_rotY = m_scale = 0.f;
}

void MlCalamus::bindToFace(unsigned faceIdx, float u, float v)
{
	m_faceIdx = faceIdx;
	m_patchU = u; 
	m_patchV = v;
}

unsigned MlCalamus::faceIdx() const
{
	return m_faceIdx;
}

float MlCalamus::patchU() const
{
	return m_patchU;
}

float MlCalamus::patchV() const
{
	return m_patchV;
}

void MlCalamus::setRotate(const float& x, const float y)
{
	m_rotX = x;
	m_rotY = y;
}

void MlCalamus::setScale(const float & x)
{
	m_scale = x;
}

float MlCalamus::rotateX() const
{
	return m_rotX;
}

float MlCalamus::rotateY() const
{
	return m_rotY;
}

float MlCalamus::scale() const
{
	return m_scale;
}