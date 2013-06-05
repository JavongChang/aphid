/*
 *  YarnPatch.cpp
 *  knitfabric
 *
 *  Created by jian zhang on 6/5/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "YarnPatch.h"

YarnPatch::YarnPatch() 
{
	m_numWaleEdges = 0;
}

void YarnPatch::setQuadVertices(unsigned *v)
{
	m_quadVertices = v;
}

void YarnPatch::findWaleEdge(unsigned v0, unsigned v1)
{
	short i, j;
	for(i = 0; i < 4; i++) {
		j = i + 1;
		if(j == 4) j = 0;
		if(m_quadVertices[i] == v0 && m_quadVertices[j] == v1) {
			setWaleEdge(i, j);
			setSecondWaleEdge(j, 1);
		}
	}
	for(i = 3; i >= 0; i--) {
		j = i - 1;
		if(j < 0) j = 3;
		if(m_quadVertices[i] == v0 && m_quadVertices[j] == v1) {
			setWaleEdge(i, j);
			setSecondWaleEdge(j, 0);
		}
	}
}

void YarnPatch::setWaleEdge(short i, short j)
{
	if(m_numWaleEdges == 2) m_numWaleEdges = 0;
	m_waleVertices[m_numWaleEdges * 2] = i;
	m_waleVertices[m_numWaleEdges * 2 + 1] = j;
	m_numWaleEdges++;
}

void YarnPatch::setSecondWaleEdge(short j, char dir)
{
	short a, b;
	if(dir) {
		a = j + 1; 
		if(a == 4) a = 0;
		b = a + 1;
		if(b == 4) b = 0;
	}
	else {
		a = j - 1;
		if(a < 0) a = 3;
		b = a - 1;
		if(b < 0) b = 3;
	}
	setWaleEdge(b, a);
}

void YarnPatch::waleEdges(short &n, unsigned * v) const
{
	if(m_numWaleEdges < 1) return;
	n = m_numWaleEdges;
	v[0] = m_quadVertices[m_waleVertices[0]];
	v[1] = m_quadVertices[m_waleVertices[1]];
	if(m_numWaleEdges < 2) return;
	v[2] = m_quadVertices[m_waleVertices[2]];
	v[3] = m_quadVertices[m_waleVertices[3]];
}
