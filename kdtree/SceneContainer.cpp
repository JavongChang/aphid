/*
 *  SceneContainer.cpp
 *  qtbullet
 *
 *  Created by jian zhang on 7/13/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SceneContainer.h"


SceneContainer::SceneContainer() 
{
	fDrawer = new ShapeDrawer;
	m_mesh = new RandomMesh(1364);
	m_tree = new KdTree;
	m_tree->create(m_mesh);
}

SceneContainer::~SceneContainer() {}

void SceneContainer::renderWorld()
{
	fDrawer->box(32.f, 32.f, 32.f);
	fDrawer->setGrey(.3f);
	fDrawer->setWired(0);
	fDrawer->drawMesh(m_mesh);
	fDrawer->setWired(1);
	fDrawer->setColor(0.15f, 1.f, 0.5f);
	fDrawer->drawKdTree(m_tree);
}
