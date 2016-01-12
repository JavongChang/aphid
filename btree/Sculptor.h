/*
 *  Sculptor.h
 *  btree
 *
 *  Created by jian zhang on 5/10/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <ActiveGroup.h>
#include <WorldGrid.h>
#include <RayMarch.h>
#include <Dropoff.h>
#include <SimpleTopology.h>
namespace sdb {
class Sculptor {
public:
	Sculptor();
	virtual ~Sculptor();
	
	void beginAddVertices(const float & gridSize);
	void insertVertex(VertexP * v);
	void endAddVertices();
	
	void setSelectRadius(const float & x);
	const float selectRadius() const;
	
	void setStrength(const float & x);
	
	void setTopology(SimpleTopology * topo);
	
	void selectPoints(const Ray * incident);
	void deselectPoints();
	
	WorldGrid<Array<int, VertexP>, VertexP > * allPoints() const;
	ActiveGroup * activePoints() const;
	
	void pullPoints();
	void pushPoints();
	void pinchPoints();
	void spreadPoints();
	void smudgePoints(const Vector3F & x);
	void smoothPoints();
	void inflatePoints();
	
	void undo();
	void redo();
    Array<int, VertexP> * lastStage();
	
private:
	bool intersect(Array<int, VertexP> * ps, const Ray & ray);
	void addToActive(VertexP * v);
	void addToStage(VertexP * v);
	Array<int, VertexP> * currentStage();
    void finishStage();
	void appendStage();
	void revertStage(sdb::Array<int, VertexP> * stage, bool isBackward = true);
	void movePointsAlong(const Vector3F & d, const float & fac);
	void movePointsToward(const Vector3F & d, const float & fac, bool normalize =false, Vector3F * vmod = NULL);
private:
	RayMarch m_march;
	ActiveGroup * m_active;
	WorldGrid<Array<int, VertexP>, VertexP > * m_tree;
	SimpleTopology * m_topo;
	float m_strength;
/// multiple stages tracking vertex position at selection and de-selection
	std::deque<Array<int, VertexP> * > m_stages;
    Array<int, VertexP> * m_lastStage;
	int m_activeStageId;
};
} // end namespace sdb