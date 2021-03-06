/*
 *  PlantSelection.h
 *  proxyPaint
 *
 *  Created by jian zhang on 1/31/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <Vector3F.h>
#include <WorldGrid.h>
#include <Array.h>
#include <SelectionContext.h>

namespace aphid {

struct GroundBind {
	float m_w0, m_w1, m_w2;
	int m_geomComp;
	void setGeomComp(int geom, int comp)
	{
		m_geomComp = ((geom<<22) | comp);
	}
	void getGeomComp(int & geom, int & comp)
	{
		geom = m_geomComp>>22;
		comp = (m_geomComp << 10)>>10;
	}
};

/// (plant id, (transformation, triangle bind, plant type id) )
typedef sdb::Triple<Matrix44F, GroundBind, int > PlantData;
class Plant : public sdb::Pair<int, PlantData>
{
public:
	
	const bool operator==(const Plant & another) const {
		return index == another.index;
	}
	
};

class PlantInstance
{
public:
	~PlantInstance()
	{
		delete m_backup;
	}
	
	Plant * m_reference;
	Plant * m_backup;
	float m_weight;
	
};

class PlantSelection {
	
	Vector3F m_center, m_direction;
	float m_radius;
	unsigned m_numSelected;
    int m_typeFilter;
	sdb::WorldGrid<sdb::Array<int, Plant>, Plant > * m_grid;
	sdb::Array<int, PlantInstance> * m_plants;
	
public:
	PlantSelection(sdb::WorldGrid<sdb::Array<int, Plant>, Plant > * grid);
	virtual ~PlantSelection();
	
    void setRadius(float x);
	void setCenter(const Vector3F & center, const Vector3F & direction);
	void select(SelectionContext::SelectMode mode);
    void deselect();
	const unsigned & numSelected() const;
	sdb::Array<int, PlantInstance> * data();
	void calculateWeight();
	void select(Plant * p);
	void updateNumSelected();
    const float & radius() const;
    
    void setTypeFilter(int x);
    
	bool touchCell(const Ray & incident, const sdb::Coord3 & c, 
					Vector3F & pnt);
	
protected:

private:
	void select(const sdb::Coord3 & c, SelectionContext::SelectMode mode);
	
};

}