/*
 *  SuperformulaPoisson.cpp
 *  
 *
 *  Created by jian zhang on 6/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "SuperformulaPoisson.h"
#include <iostream>

using namespace aphid;
namespace ttg {

SuperformulaPoisson::SuperformulaPoisson()
{}

SuperformulaPoisson::~SuperformulaPoisson() 
{}

const char * SuperformulaPoisson::titleStr() const
{ return "Superformula / Poisson-disk Sampling"; }

bool SuperformulaPoisson::createSamples()
{
	BoundingBox box;
	int i, j;
	const float du = 2.f * 3.14159269f / 120.f;
	const float dv = 3.14159269f / 60.f;
	
	for(j=0;j<60;++j) {
		for(i=0;i<120;++i) {
			box.expandBy(randomPnt(du * i - 3.1415927f, dv * j - 1.507963f) );
		}
	}

/// 1 / 32 of size of the boundary
	float r = box.getLongestDistance() * .03125f;
/// finest grid size
	float gridSize = r * 2.f;
	
	m_bkg.clear();
	m_bkg.setGridSize(gridSize);
	
/// limit of n
	setN(4000);
	
	int numAccept = 0, preN = 0;
	Disk cand;
	cand.r = r * .5f;
/// 20 times of n
	for(i=0; i<80000; ++i) {
	
		cand.pos = randomPnt(RandomFn11() * 3.14159269f, 
								RandomFn11() * 3.14159269f * .5f);
		
		if(!m_bkg.reject(&cand) ) {
			
			Disk * samp = new Disk;
			samp->pos = cand.pos;
			samp->r = cand.r;
			samp->key = numAccept;
			
			m_bkg.insert((const float *)&cand.pos, samp);
			
			X()[numAccept++] = cand.pos;
		}
		
		if((i+1) & 255 == 0) {
			if(numAccept == preN) break;
			preN = numAccept;
		}
		if(numAccept >= 4000) break;
	}
	
	setNDraw(numAccept);
	
	std::cout<<"\n n accept "<<numAccept;
	
	std::cout<<"\n n bkg "<<m_bkg.size();
	
	m_bkg1.clear();
	m_bkg1.setGridSize(gridSize * 2.f);
	
	fillBackgroud(&m_bkg1, &m_bkg, numAccept);
	
	std::cout<<"\n n bkg1 "<<m_bkg1.size();
	
	std::cout.flush();
	return true;
}

void SuperformulaPoisson::fillBackgroud(PoissonSequence<Disk> * dst,
						PoissonSequence<Disk> * frontGrid,
						int & n)
{
	const float frontSize = frontGrid->gridSize();
	Vector3F center;
	sdb::Coord3 ccenter;
	Disk cand;
	cand.r = frontSize * .499f;
	
	frontGrid->begin();
	while(!frontGrid->end() ) {
		
		center = frontGrid->coordToCellCenter(frontGrid->key() );
		
		for(int i=0; i<6; ++i) {
			cand.pos = center + Vector3F( frontSize * PoissonSequence<Disk>::TwentySixNeighborCoord[i][0],
										 frontSize * PoissonSequence<Disk>::TwentySixNeighborCoord[i][1],
										 frontSize * PoissonSequence<Disk>::TwentySixNeighborCoord[i][2]);
			fillBackgroudAt(dst, frontGrid, 
							cand, n);
		}

		frontGrid->next();
	}
}

bool SuperformulaPoisson::fillBackgroudAt(PoissonSequence<Disk> * dst,
						PoissonSequence<Disk> * frontGrid,
						Disk & cand,
						int & n)
{	
	bool rjt = false;
	sdb::Array<int, Disk > * frontCell = frontGrid->findCell(cand.pos);
/// only check no-empty front cells
	if(frontCell) {
		rjt = frontGrid->rejectIn(frontCell, &cand);
	}
	
	if(!rjt ) {
		rjt = dst->reject(&cand);
	}
		
	if(!rjt ) {
		
		Disk * samp = new Disk;
		samp->pos = cand.pos;
		samp->r = cand.r;
		samp->key = n;
		
		dst->insert((const float *)&cand.pos, samp);
		
		n++;
		return true;
	}
	return false;
}

void SuperformulaPoisson::draw(GeoDrawer * dr)
{
	dr->setColor(.125f, .3f, .15f);
	m_bkg.begin();
	while(!m_bkg.end() ) {
	
		dr->boundingBox(m_bkg.coordToGridBBox(m_bkg.key() ) );
		m_bkg.next();
	}
	
	dr->setColor(.125f, .117f, .43f);
	m_bkg1.begin();
	while(!m_bkg1.end() ) {
	
		dr->boundingBox(m_bkg1.coordToGridBBox(m_bkg1.key() ) );
		
		drawSamplesIn(dr, m_bkg1.value() );
		m_bkg1.next();
	}
	
	SuperformulaTest::draw(dr);
}

void SuperformulaPoisson::drawSamplesIn(GeoDrawer * dr,
						sdb::Array<int, Disk > * cell)
{
	cell->begin();
	while(!cell->end() ) {
		
		Disk * v = cell->value();
		dr->cube(v->pos, .0125f);
		
		cell->next();
	}
}

}
