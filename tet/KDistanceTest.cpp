/*
 *  KDistanceTest.cpp
 *  foo
 *
 *  Created by jian zhang on 7/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "KDistanceTest.h"
#include <NTreeDrawer.h>
#include <iostream>

using namespace aphid;
namespace ttg {

KDistanceTest::KDistanceTest(const std::string & filename) 
{
	m_fileName = filename;
}

KDistanceTest::~KDistanceTest() 
{}
	
const char * KDistanceTest::titleStr() const
{ return "Kd-Tree + Adaptive Grid Test"; }

bool KDistanceTest::viewPerspective() const
{ return true; }

bool KDistanceTest::init()
{
/// no grid
	m_container.readTree(m_fileName, 0);
	
	BoundingBox tb = m_container.tree()->getBBox();
	const float gz = tb.getLongestDistance() * .53f;
	std::cout<<"\n gz "<<gz;
	const Vector3F cent = tb.center();
	tb.setMin(cent.x - gz, cent.y - gz, cent.z - gz );
	tb.setMax(cent.x + gz, cent.y + gz, cent.z + gz );
	setColorScale(1.f / gz);
	setNodeDrawSize(gz * GDT_FAC_ONEOVER32 );
	
	m_msh.fillBox(tb, gz*.83f);
	
	m_distFunc.addTree(m_container.tree() );
	m_distFunc.setDomainDistanceRange(gz * GDT_FAC_ONEOVER16 * 1.9f );
	
#if 1
/// grid is very coarse relative to input mesh, error will be large
/// just subdivide to level ignoring error
	m_msh.discretize<BDistanceFunction>(&m_distFunc, 4, gz * GDT_FAC_ONEOVER16 );
	
	m_msh.buildGrid();
	m_msh.buildMesh();
	m_msh.buildGraph();
	std::cout<<"\n grid n cell "<<m_msh.grid()->size()
			<<"\n n node "<<m_msh.numNodes()
			<<"\n n edge "<<m_msh.numEdges();
	m_msh.calculateDistance<BDistanceFunction>(&m_distFunc, gz * GDT_FAC_ONEOVER16 );
#else
#define MAX_BUILD_LEVEL 4
#define MAX_BUILD_ERROR .5f	
	m_msh.adaptiveBuild<BDistanceFunction>(&m_distFunc, MAX_BUILD_LEVEL, MAX_BUILD_ERROR);
#endif

	m_msh.triangulateFront();
	
	std::cout.flush();
	return true;
}

void KDistanceTest::draw(GeoDrawer * dr)
{
#define SHO_TREE 0
#define SHO_CELL 0
#define SHO_NODE 0
#define SHO_EDGE 1
#define SHO_ERR 0
#define SHO_FRONT 1
#define SHO_FRONT_WIRE 0

#if SHO_NODE
	drawNodes(&m_msh, dr, true);
#endif

#if SHO_TREE
	drawTree(dr);
#endif

#if SHO_CELL
	dr->m_wireProfile.apply();
	drawGridCell<AdaptiveBccGrid3>(m_msh.grid(), dr);
#endif

#if SHO_EDGE	
	dr->m_wireProfile.apply();
	drawFrontEdges(&m_msh, dr);
	dr->setColor(0.f, 0.f, .5f);
	drawEdges(&m_msh, dr, true);
#endif

#if SHO_FRONT	
	dr->m_surfaceProfile.apply();
	dr->setColor(0.f, .4f, .5f);
	drawFront<FieldTriangulation >(&m_msh);
#endif
		
#if SHO_FRONT_WIRE	
	dr->m_wireProfile.apply();
	dr->setColor(0.1f, .1f, .1f);
	drawFrontWire<FieldTriangulation >(&m_msh);
#endif
	
}

void KDistanceTest::drawTree(aphid::GeoDrawer * dr)
{
#if 0
	if(!m_container.tree() ) return; 
	
	dr->m_wireProfile.apply();
	dr->setColor(.15f, .25f, .35f);
	dr->boundingBox(m_container.tree()->getBBox() );
	
	NTreeDrawer tdr;
	tdr.drawTree<cvx::Triangle>(m_container.tree() );
#endif

	if(!m_container.source() ) return;
	
	dr->m_surfaceProfile.apply();
	dr->setColor(.8f, .8f, .8f);
		
	const sdb::VectorArray<cvx::Triangle> * src = m_container.source();
	const int n = src->size();
	glBegin(GL_TRIANGLES);
	int i=0;
	for(;i<n;++i) {
		const cvx::Triangle * t = src->get(i);
		
		glNormal3fv((GLfloat *)&t->N(0) );
		glVertex3fv((GLfloat *)t->p(0) );
		
		glNormal3fv((GLfloat *)&t->N(1) );
		glVertex3fv((GLfloat *)t->p(1) );
		
		glNormal3fv((GLfloat *)&t->N(2) );
		glVertex3fv((GLfloat *)t->p(2) );
	}
	glEnd();
}

}