/*
 *  RedBlueRefine.cpp
 *  
 *
 *  Created by jian zhang on 7/16/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "RedBlueRefine.h"

using namespace aphid;

namespace ttg {

RedBlueRefine::RedBlueRefine()
{}
	
void RedBlueRefine::set(int a, int b, int c, int d)
{
	m_a = a; m_b = b;
	m_c = c; m_d = d;
	setTetrahedronVertices(m_tet[0], a, b, c, d);
	m_N = 1;
/// reset
	m_red[0] = m_red[1] = -1;
	m_blue[0] = m_blue[1] = m_blue[2] = m_blue[3] = -1;
}

void RedBlueRefine::evaluateDistance(float a, float b, float c, float d)
{
	int cred = 0, cblue = 0;
	if(splitCondition(a, b) ) {
		m_red[0] = 0;
		cred++;
	}
		
	if(splitCondition(c, d) ) {
		m_red[1] = 0;
		cred++;
	}
		
	if(splitCondition(a, c) ) {
		m_blue[0] = 0;
		cblue++;
	}
		
	if(splitCondition(a, d) ) {
		m_blue[1] = 0;
		cblue++;
	}
		
	if(splitCondition(b, c) ) {
		m_blue[2] = 0;
		cblue++;
	}
		
	if(splitCondition(b, d) ) {
		m_blue[3] = 0;
		cblue++;
	}
		
	m_opt = SpNone;
	
	if(cred == 0) {
		if(cblue == 1)
			m_opt = SpOneBlue;
		else if(cblue == 2) {
			if(m_blue[0] > -1 && m_blue[1] > -1)
				m_opt = SpTwoBlueOneSide;
			else
				m_opt = SpTwoBlueTwoSide;
		}
		else if(cblue == 4)
			m_opt = SpFourBlue;
	}
	else if(cred == 1) {
		if(cblue == 0)
			m_opt = SpOneRed;
		else if(cblue == 1)
			m_opt = SpOneRedOneBlue;
		else if(cblue == 2)
			m_opt = SpOneRedTwoBlue;
	}
	else {
		if(cblue == 4)
			m_opt = SpTwoRedFourBlue;
	}
	
	auxiliarySplit();
	
}

void RedBlueRefine::verbose() const
{
	std::string strOpt("none");
	switch (m_opt) {
		case SpOneRed:
			strOpt = "1 red";
			break;
		case SpOneRedOneBlue:
			strOpt = "1 red 1 blue";
			break;
		case SpOneRedTwoBlue:
			strOpt = "1 red 2 blue";
			break;
		case SpTwoRedFourBlue:
			strOpt = "2 red 4 blue";
			break;
		case SpOneBlue:
			strOpt = "1 blue";
			break;
		case SpTwoBlueOneSide:
			strOpt = "2 blue 1 side";
			break;
		case SpTwoBlueTwoSide:
			strOpt = "2 blue 2 side";
			break;
		case SpFourBlue:
			strOpt = "4 blue";
			break;
		default:
			break;
	}
	std::cout<<"\n split option "<<strOpt;
	
	std::cout<<"\n red "<<m_red[0]<<", "<<m_red[1]
			<<"\n blue "<<m_blue[0]<<", "<<m_blue[1]<<", "<<m_blue[2]<<", "<<m_blue[3];
}

void RedBlueRefine::auxiliarySplit()
{
	if(m_opt == SpOneRedOneBlue) {
/// same side blue
		if(m_blue[0] > -1)
			m_blue[1] = 0;
		else if(m_blue[1] > -1)
			m_blue[0] = 0;
		else if(m_blue[2] > -1)
			m_blue[3] = 0;
		else if(m_blue[3] > -1)
			m_blue[2] = 0;
	}
	else if(m_opt == SpOneRedTwoBlue) {
/// other red
		if(m_red[0] > -1)
			m_red[1] = 0;
		else if(m_red[1] > -1)
			m_red[0] = 0;
	}
	else if(m_opt == SpTwoBlueOneSide) {
/// red up
		m_red[1] = 0;
	}
	else if(m_opt == SpTwoBlueTwoSide) {
/// red bottom
		m_red[0] = 0;
	}
	else if(m_opt == SpFourBlue) {
/// both red
		m_red[0] = 0;
		m_red[1] = 0;
	}	
}

const int & RedBlueRefine::numTetra() const
{ return m_N; }

const ITetrahedron * RedBlueRefine::tetra(int i) const
{ return &m_tet[i]; }

void RedBlueRefine::splitRedEdge(int i, int v)
{ m_red[i] = v; }

void RedBlueRefine::splitBlueEdge(int i, int v)
{ m_blue[i] = v; }

bool RedBlueRefine::needSplitRedEdge(int i)
{ return m_red[i] > -1; }

bool RedBlueRefine::needSplitBlueEdge(int i)
{ return m_blue[i] > -1; }

bool RedBlueRefine::splitCondition(const float & a,
					const float & b) const
{ return (a * b) < 0.f; }

Vector3F RedBlueRefine::splitPos(float a, float b,
						const Vector3F & pa,
						const Vector3F & pb) const
{
	if(!splitCondition(a, b) )
		return (pa + pb) * .5f;
		
	float sa = Absolute<float>(a);
	float sb = Absolute<float>(b);
	return pa * sb / (sa + sb) + pb * sa / (sa + sb);
}

void RedBlueRefine::refine()
{
	if(m_opt == SpNone) return;
	
	switch (m_opt) {
		case SpOneRed:
			oneRedRefine();
			break;
		case SpOneBlue:
			oneBlueRefine();
			break;
		case SpOneRedOneBlue:
			oneRedOneBlueRefine();
			break;
		default:
			break;
	}
}
	
void RedBlueRefine::oneRedRefine()
{
	if(m_red[0] > 0) {
		splitRed(0, m_tet[0], m_tet[1], m_red[0]);
	}
	else {
		splitRed(1, m_tet[0], m_tet[1], m_red[1]);
	}
	m_N = 2;
}

void RedBlueRefine::oneBlueRefine()
{
	if(m_blue[0] > 0) {
		splitBlue(0, m_tet[0], m_tet[1], m_blue[0]);
	
	}
	else if(m_blue[1] > 0) {
		splitBlue(1, m_tet[0], m_tet[1], m_blue[1]);
		
	}
	else if(m_blue[2] > 0) {
		splitBlue(2, m_tet[0], m_tet[1], m_blue[2]);
		
	}
	else {
		splitBlue(3, m_tet[0], m_tet[1], m_blue[3]);

	}
	m_N = 2;
}

void RedBlueRefine::splitRed(int i, ITetrahedron & t0, ITetrahedron & t1,
								int v)
{
	int a = t0.iv0;
	int b = t0.iv1;
	int c = t0.iv2;
	int d = t0.iv3;
	if(i==0) {
		setTetrahedronVertices(t0, a, v, c, d);
		setTetrahedronVertices(t1, v, b, c, d);
	}
	else {
		setTetrahedronVertices(t0, a, b, c, v);
		setTetrahedronVertices(t1, a, b, v, d);
	}
}

void RedBlueRefine::splitBlue(int i, ITetrahedron & t0, ITetrahedron & t1,
								int v)
{
	int a = t0.iv0;
	int b = t0.iv1;
	int c = t0.iv2;
	int d = t0.iv3;
	if(i==0) {
		setTetrahedronVertices(t0, v, b, c, d);
		setTetrahedronVertices(t1, a, b, v, d);
	}
	else if(i==1) {
		setTetrahedronVertices(t0, v, b, c, d);
		setTetrahedronVertices(t1, a, b, c, v);
	}
	else if(i==2) {
		setTetrahedronVertices(t0, a, v, c, d);
		setTetrahedronVertices(t1, a, b, v, d);
	}
	else {
		setTetrahedronVertices(t0, a, v, c, d);
		setTetrahedronVertices(t1, a, b, c, v);
	}
}

void RedBlueRefine::oneRedOneBlueRefine()
{
	if(m_red[0] > 0) {
		splitRed(0, m_tet[0], m_tet[1], m_red[0]);
		
		if(m_blue[0] > 0) {
			splitBlue(0, m_tet[0], m_tet[2], m_blue[0]);
	
		}
		else if(m_blue[1] > 0) {
			splitBlue(1, m_tet[0], m_tet[2], m_blue[1]);
			
		}
		else if(m_blue[2] > 0) {
			splitBlue(2, m_tet[1], m_tet[2], m_blue[2]);
			
		}
		else {
			splitBlue(3, m_tet[1], m_tet[2], m_blue[3]);

		}
	}
	else {
		splitRed(1, m_tet[0], m_tet[1], m_red[1]);
		
		if(m_blue[0] > 0) {
			splitBlue(0, m_tet[0], m_tet[2], m_blue[0]);
	
		}
		else if(m_blue[1] > 0) {
			splitBlue(1, m_tet[1], m_tet[2], m_blue[1]);
			
		}
		else if(m_blue[2] > 0) {
			splitBlue(2, m_tet[0], m_tet[2], m_blue[2]);
			
		}
		else {
			splitBlue(3, m_tet[1], m_tet[2], m_blue[3]);

		}
	}
	m_N = 3;
}

}