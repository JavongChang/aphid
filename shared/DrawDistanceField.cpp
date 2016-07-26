/*
 *  DrawDistanceField.cpp
 *  
 *
 *  Created by jian zhang on 7/23/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "DrawDistanceField.h"

namespace aphid {

DrawDistanceField::DrawDistanceField() 
{
	m_nodeColScl = 1.f;
	m_nodeDrawSize = .03f;
}

DrawDistanceField::~DrawDistanceField()
{}

void DrawDistanceField::setColorScale(const float & x)
{ m_nodeColScl = x; }

void DrawDistanceField::setNodeDrawSize(const float & x)
{ m_nodeDrawSize = x; }
	
void DrawDistanceField::drawNodes(const ADistanceField * fld, GeoDrawer * dr)
{
	const DistanceNode * v = fld->nodes();
	const int nv = fld->numNodes();

	Vector3F col;
	for(int i = 0;i<nv;++i) {
		const DistanceNode & vi = v[i];
		
		fld->nodeColor(col, vi, m_nodeColScl);
		dr->setColor(col.x, col.y, col.z);
		dr->cube(vi.pos, m_nodeDrawSize);
		
	}
}

void DrawDistanceField::drawEdges(const ADistanceField * fld, GeoDrawer * dr)
{
	const DistanceNode * v = fld->nodes();
	
	const IDistanceEdge * e = fld->edges();
	const int ne = fld->numEdges();
	
	glBegin(GL_LINES);
	for(int i = 0;i<ne;++i) {
		const IDistanceEdge & ei = e[i];
		
		dr->vertex(v[ei.vi.x].pos);
		dr->vertex(v[ei.vi.y].pos);
		
	}
	glEnd();
}
	
const float & DrawDistanceField::nodeDrawSize() const
{ return m_nodeDrawSize; }

void DrawDistanceField::drawErrors(const ADistanceField * fld,
					sdb::Sequence<sdb::Coord2 > * edgeInds,
					const float & eps)
{
	const DistanceNode * v = fld->nodes();
	const IDistanceEdge * e = fld->edges();
	
	glColor3f(1.f, 0.f, 0.f);
				
	glBegin(GL_LINES);
	edgeInds->begin();
	while(!edgeInds->end() ) {
		
		const sdb::Coord2 & ei = edgeInds->key();
		const IDistanceEdge * ae = fld->edge(ei.x, ei.y );
		if(ae) {
			if(ae->err > eps) {
				
				glVertex3fv((const float *)&v[ei.x].pos);
				glVertex3fv((const float *)&v[ei.y].pos);
			}
		}
		
		edgeInds->next();
	}
	glEnd();
}

}
