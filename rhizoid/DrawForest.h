/*
 *  DrawForest.h
 *  proxyPaint
 *
 *  Created by jian zhang on 1/30/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ModifyForest.h"
#include <ViewDepthCull.h>
#include <DrawBox.h>

class CircleCurve;

class DrawForest : public sdb::ModifyForest, public ViewDepthCull, public DrawBox
{
	
    Matrix44F m_useMat;
    BoundingBox m_defBox;
	float m_boxExtent;
	float m_transbuf[16];
	float m_scalbuf[3];
	CircleCurve * m_circle;
	float m_defBoxCenter[3];
	float m_defBoxScale[3];
	
public:
    DrawForest();
    virtual ~DrawForest();
    
protected:
	void setScaleMuliplier(float x, int idx);
    void drawGround();
	BoundingBox * defBoxP();
	const BoundingBox & defBox() const;
	void draw_solid_box() const;
	void draw_a_box() const;
	void draw_coordsys() const;
	int activePlantId() const;
	virtual float plantSize(int idx) const;
	Vector3F plantCenter(int idx) const;
	float plantExtent(int idx) const;
	void drawPlants();
	void drawWiredPlants();
	void drawGridBounding();
	void drawGrid();
	void drawActivePlants();
	void drawViewFrustum();
	void drawBrush();
	void drawDepthCull(double * localTm);
	bool isVisibleInView(sdb::Plant * pl, 
					const float lowLod, const float highLod);
    void setDefBox(const float & a, 
					const float & b,
					const float & c,
					const float & d,
					const float & e,
					const float & f);
	
private:
    void drawFaces(Geometry * geo, sdb::Sequence<unsigned> * components);
	void drawPlants(sdb::Array<int, sdb::Plant> * cell);
	void drawPlant(sdb::PlantData * data);
	void drawWiredPlants(sdb::Array<int, sdb::Plant> * cell);
	void drawWiredPlant(sdb::PlantData * data);
	void drawCircle() const;
    void calculateDefExtent();
	
};