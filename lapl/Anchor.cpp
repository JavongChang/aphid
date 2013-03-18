/*
 *  Anchor.cpp
 *  lapl
 *
 *  Created by jian zhang on 3/19/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "Anchor.h"
#include <Vertex.h>

Anchor::Anchor(SelectionArray & sel) 
{
	Vector3F cen(0.f, 0.f, 0.f);
	const unsigned nv = sel.numVertices();
	for(unsigned i=0; i < nv; i++) {
		Vertex * v = sel.getVertex(i);
		AnchorPoint *a = new AnchorPoint();
		a->p = *v->m_v;
		a->w = 0.5f;
		m_anchorPoints[v->getIndex()] = a;
		cen += *v->m_v;
	}
	cen /= nv;
	m_space.setIdentity();
	m_space.setTranslation(cen);
	
	Matrix44F invs = m_space;
	invs.inverse();
	
	for(m_anchorPointIt = m_anchorPoints.begin(); m_anchorPointIt != m_anchorPoints.end(); ++m_anchorPointIt) {
		Vector3F & pos = ((*m_anchorPointIt).second)->p;
		pos = invs.transform(pos);
	}
}

Anchor::~Anchor() 
{
	for(m_anchorPointIt = m_anchorPoints.begin(); m_anchorPointIt != m_anchorPoints.end(); ++m_anchorPointIt) {
		delete (*m_anchorPointIt).second;
	}
	m_anchorPoints.clear();
}

Anchor::AnchorPoint * Anchor::firstPoint(unsigned &idx)
{
	m_anchorPointIt = m_anchorPoints.begin();
	idx = (*m_anchorPointIt).first;
	return (*m_anchorPointIt).second;
}

Anchor::AnchorPoint * Anchor::nextPoint(unsigned &idx)
{
	m_anchorPointIt++;
	if(!hasPoint()) return 0;
	idx = (*m_anchorPointIt).first;
	return (*m_anchorPointIt).second;
}

bool Anchor::hasPoint()
{
	return m_anchorPointIt != m_anchorPoints.end();
}

void Anchor::spaceMatrix(float m[16]) const
{
	m[0] = m_space.M(0,0); m[1] = m_space.M(0,1); m[2] = m_space.M(0,2); m[3] = 0.0;
    m[4] = m_space.M(1,0); m[5] = m_space.M(1,1); m[6] = m_space.M(1,2); m[7] = 0.0;
    m[8] = m_space.M(2,0); m[9] = m_space.M(2,1); m[10] =m_space.M(2,2); m[11] = 0.0;
    m[12] = m_space.M(3,0); m[13] = m_space.M(3,1); m[14] = m_space.M(3,2) ; m[15] = 1.0;
}
