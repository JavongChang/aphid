/*
 *  WorldGrid.h
 *  
 *
 *  Created by jian zhang on 1/2/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 *  Highest level of spatial division, using grid(x,y,z) as key
 */

#pragma once
#include <Sequence.h>
#include <BoundingBox.h>
namespace sdb {

template<typename ChildType, typename ValueType>
class WorldGrid : public Sequence<Coord3>
{
	BoundingBox m_bbox;
	float m_gridSize;
	
public:
	WorldGrid(Entity * parent = NULL) : Sequence<Coord3>(parent) 
	{
		m_gridSize = 1.f;
	}
	
	virtual ~WorldGrid() {}
	
	void setGridSize(float x)
	{ m_gridSize = x; }
	
	ChildType * value() 
	{ return static_cast<ChildType *>(Sequence<Coord3>::currentIndex() ); }
	
	const Coord3 key() const 
	{ return Sequence<Coord3>::currentKey(); }
	
/// put v into grid
	void insert(const float * at, ValueType * v);
	void insert(const Coord3 & x , ValueType * v);
	
/// find grid coord of point(x,y,z)
	const Coord3 gridCoord(const float * p) const;

	void calculateBBox();
	const BoundingBox coordToGridBBox(const Coord3 & c) const;
	const BoundingBox boundingBox() const;

	float gridSize() const;
	
	void displace(ValueType * v, const Vector3F & pref);
	
protected:
	
private:
	
};

template<typename ChildType, typename ValueType>
const Coord3 WorldGrid<ChildType, ValueType>::gridCoord(const float * p) const
{
	Coord3 r;
	r.x = p[0] / m_gridSize; if(p[0] < 0.f) r.x--;
	r.y = p[1] / m_gridSize; if(p[1] < 0.f) r.y--;
	r.z = p[2] / m_gridSize; if(p[2] < 0.f) r.z--;
	return r;
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::insert(const float * at, ValueType * v) 
{
	const Coord3 x = gridCoord(at);
	insert(x, v);
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::insert(const Coord3 & x , ValueType * v)
{
	Pair<Coord3, Entity> * p = Sequence<Coord3>::insert(x);
	if(!p->index) p->index = new ChildType(this);
	static_cast<ChildType *>(p->index)->insert(v->key, v);
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::calculateBBox()
{
	m_bbox.reset();
	begin();
	while(!end()) {
		m_bbox.expandBy(coordToGridBBox(currentKey() ));
		next();
	}
}

template<typename ChildType, typename ValueType>
const BoundingBox WorldGrid<ChildType, ValueType>::coordToGridBBox(const Coord3 & c) const
{
	BoundingBox bb;
	bb.setMin(m_gridSize * c.x, m_gridSize * c.y, m_gridSize * c.z);
	bb.setMax(m_gridSize * (c.x + 1), m_gridSize * (c.y + 1), m_gridSize * (c.z + 1));
	return bb;
}

template<typename ChildType, typename ValueType>
const BoundingBox WorldGrid<ChildType, ValueType>::boundingBox() const
{ return m_bbox; }

template<typename ChildType, typename ValueType>
float WorldGrid<ChildType, ValueType>::gridSize() const
{ return m_gridSize; }

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::displace(ValueType * v, const Vector3F & pref)
{
	Coord3 c1 = gridCoord((float *)(v->index)->t1);
	Coord3 c0 = gridCoord((float *)&pref);
	if(c0 == c1) return;
	
	ValueType * nv = new ValueType;
	nv->key = v->key;
	nv->index = v->index;
	
	Pair<Entity *, Entity> p = findEntity(c0);
	if(p.index) {
		ChildType * g = static_cast<ChildType *>(p.index);
		g->remove(v->key);
		if(g->isEmpty()) {
			// std::cout<<"\n remove grid "<<c0;
			remove(c0);
		}
	}
	insert(c1, nv);
}

} //end namespace sdb
