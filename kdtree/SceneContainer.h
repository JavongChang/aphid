/*
 *  SceneContainer.h
 *  qtbullet
 *
 *  Created by jian zhang on 7/13/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

class RandomMesh;
class KdTreeDrawer;
class BezierCurve;
class KdCluster;
class GeometryArray;
class SceneContainer {
public:
	SceneContainer(KdTreeDrawer * drawer);
	virtual ~SceneContainer();
	
	void renderWorld();
	void upLevel();
	void downLevel();
protected:

private:
	void testMesh();
	void testCurve();

private:
	KdTreeDrawer * m_drawer;
	RandomMesh * m_mesh[4];
	GeometryArray * m_curves;
	KdCluster * m_cluster;
	int m_level;
};