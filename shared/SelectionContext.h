/*
 *  SelectionContext.h
 *  aphid
 *
 *  Created by jian zhang on 1/22/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllMath.h>
#include <BoundingBox.h>
#include <Geometry.h>
#include <Sequence.h>
#include <deque>
#include <map>

class SelectionContext {
	std::map<Geometry *, sdb::Sequence<unsigned> * > m_geoComponents;
	
public:
	enum SelectMode {
		Replace = 0,
		Append = 1,
		Remove = 2
	};
	
	SelectionContext();
	virtual ~SelectionContext();
	
	void reset();
	void reset(const Vector3F & center, const float & radius);
	void finish();
	
	void setDirection(const Vector3F & d);
	
	void setCenter(const Vector3F & center);
	const Vector3F & center() const;
	
	void setRadius(const float & radius);
	float radius() const;
	
	void setBBox(const BoundingBox &bbox);
	const BoundingBox & getBBox() const;
	
	char closeTo(const BoundingBox & b) const;
	char closeTo(const Vector3F & v) const;
	void addToSelection(const unsigned idx);
	
	void select(Geometry * geo, unsigned componentId);
	void deselect();
	unsigned countComponents();
	std::map<Geometry *, sdb::Sequence<unsigned> * >::iterator geometryBegin();
	std::map<Geometry *, sdb::Sequence<unsigned> * >::iterator geometryEnd();
	
	unsigned numSelected() const;
	const std::deque<unsigned> & selectedQue() const;
	
	void setSelectMode(SelectMode m);
	SelectMode getSelectMode() const;
	void discard();
	
	void verbose() const;
private:
	void finishAdd();
	void finishRemove();
	void remove(const unsigned & idx);
private:
	std::deque<unsigned> m_indices;
	std::deque<unsigned> m_removeIndices;
	BoundingBox m_bbox;
	Vector3F m_center, m_normal;
	float m_radius;
	SelectMode m_mode;
	char m_enableDirection;
};