/*
 *  AOrientedBox.cpp
 *  aphid
 *
 *  Created by jian zhang on 8/24/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "AOrientedBox.h"

namespace aphid {

AOrientedBox::AOrientedBox() {}
AOrientedBox::~AOrientedBox() {}

void AOrientedBox::setCenter(const Vector3F & p)
{ m_center = p; }

void AOrientedBox::setOrientation(const Matrix33F & m)
{ m_orientation = m; }

void AOrientedBox::setExtent(const Vector3F & p)
{ m_extent = p; }

const Vector3F & AOrientedBox::center() const
{ return m_center; }

const Matrix33F & AOrientedBox::orientation() const
{ return m_orientation; }

const Vector3F & AOrientedBox::extent() const
{ return m_extent; }

Vector3F AOrientedBox::majorPoint(bool low) const
{ return (m_center + majorVector(low) * m_extent.x); }

Vector3F AOrientedBox::majorVector(bool low) const
{ 
	float dir = 1.f;
	if(low) dir = -1.f;
	return m_orientation.row(0) * dir;
}

Vector3F AOrientedBox::minorPoint(bool low) const
{ return (m_center + minorVector(low) * m_extent.y); }

Vector3F AOrientedBox::minorVector(bool low) const
{
    float dir = 1.f;
	if(low) dir = -1.f;
	return m_orientation.row(1) * dir;
}

void AOrientedBox::getBoxVertices(Vector3F * dst) const
{
	const Vector3F rx = m_orientation.row(0);
	const Vector3F ry = m_orientation.row(1);
	const Vector3F rz = m_orientation.row(2);
	const Vector3F & eh = m_extent;
	dst[0] = m_center - rx * eh.x - ry * eh.y - rz * eh.z;
	dst[1] = m_center + rx * eh.x - ry * eh.y - rz * eh.z;
	dst[2] = m_center - rx * eh.x + ry * eh.y - rz * eh.z;
	dst[3] = m_center + rx * eh.x + ry * eh.y - rz * eh.z;
	dst[4] = m_center - rx * eh.x - ry * eh.y + rz * eh.z;
	dst[5] = m_center + rx * eh.x - ry * eh.y + rz * eh.z;
	dst[6] = m_center - rx * eh.x + ry * eh.y + rz * eh.z;
	dst[7] = m_center + rx * eh.x + ry * eh.y + rz * eh.z;
}

const TypedEntity::Type AOrientedBox::type() const
{ return TOrientedBox; }

const BoundingBox AOrientedBox::calculateBBox() const
{
	Vector3F p[8];
	getBoxVertices(p);
	BoundingBox box;
	for(unsigned i = 0; i < 8; i++) {
		box.updateMin(p[i]);
		box.updateMax(p[i]);
	}
	return box;
}

void AOrientedBox::set8DOPExtent(const float & x0, const float & x1,
						const float & y0, const float & y1)
{
	m_8DOPExtent[0] = x0;
	m_8DOPExtent[1] = x1;
	m_8DOPExtent[2] = y0;
	m_8DOPExtent[3] = y1;
}

void AOrientedBox::get8DOPVertices(Vector3F * dst) const
{
	const Vector3F dx = m_orientation.row(0);
	const Vector3F dy = m_orientation.row(1);
	const Vector3F dz = m_orientation.row(2);
	Vector3F rgt = dx + dy;
	Vector3F up = dz.cross(rgt);
	up.normalize();
	rgt.normalize();
	
	Matrix33F rot(rgt, up, dz);
	
	const Vector3F rx = rot.row(0);
	const Vector3F ry = rot.row(1);
	const Vector3F rz = rot.row(2);
	const float ez = m_extent.z;
	dst[0] = m_center + rx * m_8DOPExtent[0] + ry * m_8DOPExtent[2] - rz * ez;
	dst[1] = m_center + rx * m_8DOPExtent[1] + ry * m_8DOPExtent[2] - rz * ez;
	dst[2] = m_center + rx * m_8DOPExtent[0] + ry * m_8DOPExtent[3] - rz * ez;
	dst[3] = m_center + rx * m_8DOPExtent[1] + ry * m_8DOPExtent[3] - rz * ez;
	dst[4] = m_center + rx * m_8DOPExtent[0] + ry * m_8DOPExtent[2] + rz * ez;
	dst[5] = m_center + rx * m_8DOPExtent[1] + ry * m_8DOPExtent[2] + rz * ez;
	dst[6] = m_center + rx * m_8DOPExtent[0] + ry * m_8DOPExtent[3] + rz * ez;
	dst[7] = m_center + rx * m_8DOPExtent[1] + ry * m_8DOPExtent[3] + rz * ez;
}

}
//:~