/*
 *  BaseDrawer.h
 *  qtbullet
 *
 *  Created by jian zhang on 7/17/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include <BaseMesh.h>
#include <BaseDeformer.h>
#include <BaseField.h>
#include <BoundingBox.h>
#include <SelectionArray.h>
#include <Anchor.h>
#include <GeodesicSphereMesh.h>
class BaseDrawer {
public:
	BaseDrawer ();
	virtual ~BaseDrawer ();
	
	void box(float width, float height, float depth);
	void solidCube(float x, float y, float z, float size);
	void setGrey(float g);
	void setColor(float r, float g, float b);
	void end();
	void beginSolidTriangle();
	void beginWireTriangle();
	void beginLine();
	void beginPoint();
	void beginQuad();
	void aVertex(float x, float y, float z);
	void drawSphere();
	void drawCircleAround(const Vector3F& center);
	void drawMesh(const BaseMesh * mesh, const BaseDeformer * deformer = 0);
	void field(const BaseField * f);
	void tangentFrame(const BaseMesh * mesh, const BaseDeformer * deformer = 0);
	void box(const BoundingBox & b);
	void triangle(const BaseMesh * mesh, unsigned idx);
	void components(SelectionArray * arr);
	void primitive(Primitive * prim);
	void coordsys(float scale = 1.f);
	void coordsys(const Matrix33F & orient, float scale = 1.f);
	void setWired(char var);
	void anchor(Anchor *a);
	void sphere(float size = 1.f);
	
private:
    char m_wired;
	GeodesicSphereMesh * m_sphere;
};