/*
 *  SplitEvent.cpp
 *  kdtree
 *
 *  Created by jian zhang on 10/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include "SplitEvent.h"
#include <BaseMesh.h>
#include <BuildKdTreeContext.h>
int SplitEvent::Dimension = 3;
int SplitEvent::NumBinPerDimension = 33;
int SplitEvent::NumEventPerDimension = 32;
//float SplitEvent::ParentBoxArea = 1.f;
//BoundingBox SplitEvent::ParentBox;
SplitEvent::SplitEvent() : m_isEmpty(1)
{
	m_cost = 10e28;
}

void SplitEvent::setPos(float val)
{
	m_pos = val;
}

void SplitEvent::setAxis(int val)
{
	m_isEmpty = 0;
	m_axis = val;
}
	
float SplitEvent::getPos() const
{
	return m_pos;
}

int SplitEvent::getAxis() const
{
	return m_axis;
}

void SplitEvent::setLeftRightNumPrim(const unsigned &leftNumPrim, const unsigned &rightNumPrim)
{
	m_leftNumPrim = leftNumPrim;
	m_rightNumPrim = rightNumPrim;
}

const float SplitEvent::getCost() const
{
	return m_cost;
}

int SplitEvent::leftCount() const
{
	return m_leftNumPrim;
}

int SplitEvent::rightCount() const
{
	return m_rightNumPrim;
}

int SplitEvent::side(const BoundingBox &box) const
{
	int side = 1;
	if(box.getMax(m_axis) < m_pos)
		side = 0;
	else if(box.getMin(m_axis) >= m_pos)
		side = 2;
	return side;
}

void SplitEvent::calculateTightBBoxes(const BoundingBox &box, BoundingBox &leftBBox, BoundingBox &rightBBox)
{
	const int s = side(box);
	if(s == 0) {
		leftBBox.expandBy(box);
	}
	else if(s == 2 ) {
		rightBBox.expandBy(box);
	}
	else {
		leftBBox.expandBy(box);
		rightBBox.expandBy(box);
	}
}

void SplitEvent::updateLeftBox(const BoundingBox &box)
{
	m_leftBox.expandBy(box);
}

void SplitEvent::updateRightBox(const BoundingBox &box)
{
	m_rightBox.expandBy(box);	
}

void SplitEvent::calculateCost(float x)
{/*
	BoundingBox leftBBox, rightBBox;
	for(unsigned i = 0; i < NumPrimitive; i++) {
		//unsigned &primIdx = PrimitiveIndices[i];
		BoundingBox &primBox = PrimitiveBoxes[i];
		calculateTightBBoxes(primBox, leftBBox, rightBBox);
	}
	
	m_cost = 15.f + 20.f * (leftBBox.area() * m_leftNumPrim + rightBBox.area() * m_rightNumPrim) / ParentBoxArea;
	*/
	if(m_isEmpty) return;
	m_cost = 1.5f + 2.f * (m_leftBox.area() * m_leftNumPrim + m_rightBox.area() * m_rightNumPrim) / x;
}

float SplitEvent::area() const
{
	return m_leftBox.area() + m_rightBox.area();
}

float SplitEvent::hasBothSides() const
{
	return (m_leftNumPrim > 0 && m_rightNumPrim > 0);
}

void SplitEvent::verbose() const
{
	printf("cost %f left %i %f right %i %f\n", m_cost, m_leftNumPrim, m_leftBox.area(), m_rightNumPrim, m_rightBox.area());
}
