/*
 *  KdTreeBuilder.cpp
 *  kdtree
 *
 *  Created by jian zhang on 10/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "KdTreeBuilder.h"
#include <boost/thread.hpp>  

BuildKdTreeContext * KdTreeBuilder::GlobalContext = NULL;

KdTreeBuilder::KdTreeBuilder()
{
	m_bins = new MinMaxBins[SplitEvent::Dimension];
	m_event = new SplitEvent[SplitEvent::NumEventPerDimension * SplitEvent::Dimension];
}

KdTreeBuilder::~KdTreeBuilder() 
{
	delete[] m_event;
	delete[] m_bins;
}

void KdTreeBuilder::setContext(BuildKdTreeContext &ctx) 
{
	m_context = &ctx;
	m_numPrimitive = ctx.getNumPrimitives();
	m_bbox = ctx.getBBox();
	
	calculateBins();
	calculateSplitEvents();
}

void KdTreeBuilder::calculateBins()
{
	BoundingBox *primBoxes = GlobalContext->primitiveBoxes();
	unsigned *indices = m_context->indices();
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		if(m_bbox.distance(axis) < 1e-3f) {
		    m_bins[axis].setFlat();
			continue;
		}
		m_bins[axis].create(SplitEvent::NumBinPerDimension, m_bbox.getMin(axis), m_bbox.getMax(axis));
	
		for(unsigned i = 0; i < m_numPrimitive; i++) {
			const BoundingBox * primBox = &primBoxes[indices[i]];
			m_bins[axis].add(primBox->getMin(axis), primBox->getMax(axis));
		}
		
		m_bins[axis].scan();
	}
}

void KdTreeBuilder::calculateSplitEvents()
{
	const unsigned numEvent = SplitEvent::NumEventPerDimension * SplitEvent::Dimension;
	int dimOffset;
	unsigned leftNumPrim, rightNumPrim;
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		if(m_bins[axis].isFlat())
			continue;
		dimOffset = SplitEvent::NumEventPerDimension * axis;	
		const float min = m_bbox.getMin(axis);
		const float delta = m_bbox.distance(axis) / SplitEvent::NumBinPerDimension;
		for(int i = 0; i < SplitEvent::NumEventPerDimension; i++) {
			SplitEvent &event = m_event[dimOffset + i];
			event.setAxis(axis);
			event.setPos(min + delta * (i + 1));
			m_bins[axis].get(i, leftNumPrim, rightNumPrim);
			event.setLeftRightNumPrim(leftNumPrim, rightNumPrim);
		}
	}
	
#if 0
	boost::thread boxThread[3];
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		if(m_bins[axis].isFlat())
			continue;
		boxThread[axis] = boost::thread(boost::bind(&KdTreeBuilder::updateEventBBoxAlong, this, axis));
	}
	
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		if(m_bins[axis].isFlat())
			continue;
		boxThread[axis].join();
	}
#else
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		if(m_bins[axis].isFlat())
			continue;
		updateEventBBoxAlong(axis);
	}
#endif
	
	for(unsigned i = 0; i < numEvent; i++) {
		m_event[i].calculateCost(m_bbox.area());
	}
}

void KdTreeBuilder::updateEventBBoxAlong(const int &axis)
{
	SplitEvent * eventOffset = &m_event[axis * SplitEvent::NumEventPerDimension];
	
	const float min = m_bbox.getMin(axis);
	const float delta = m_bbox.distance(axis) / SplitEvent::NumBinPerDimension;
	int g, minGrid, maxGrid;
	BoundingBox *primBoxes = GlobalContext->primitiveBoxes();
	unsigned *indices = m_context->indices();
	
	for(unsigned i = 0; i < m_numPrimitive; i++) {
		const BoundingBox * primBox = &primBoxes[indices[i] ];
		
		minGrid = (primBox->getMin(axis) - min) / delta;
		
		if(minGrid < 0) minGrid = 0;
		
		for(g = minGrid; g < SplitEvent::NumEventPerDimension; g++)
			eventOffset[g].updateLeftBox(*primBox);
		
		maxGrid = (primBox->getMax(axis) - min) / delta;
		
		if(maxGrid > SplitEvent::NumEventPerDimension) maxGrid = SplitEvent::NumEventPerDimension;

		for(g = maxGrid; g > 0; g--)
			eventOffset[g - 1].updateRightBox(*primBox);
	}
}

const SplitEvent *KdTreeBuilder::bestSplit()
{
	m_bestEventIdx = 0;
	byLowestCost(m_bestEventIdx);
	
	unsigned lc = 0;
	if(byCutoffEmptySpace(lc)) {
		if(m_event[lc].getCost() < m_event[m_bestEventIdx].getCost() * 2.f)
			m_bestEventIdx = lc;
	}
		
	return &m_event[m_bestEventIdx];
}

SplitEvent * KdTreeBuilder::splitAt(int axis, int idx)
{
	return &m_event[axis * SplitEvent::NumEventPerDimension + idx];
}

void KdTreeBuilder::byLowestCost(unsigned & dst)
{
	float lowest = 1e28f;
	
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		for(int i = 1; i < SplitEvent::NumEventPerDimension - 1; i++) {
			SplitEvent * e = splitAt(axis, i);
			if(e->getCost() < lowest && e->hasBothSides()) {
				lowest = e->getCost();
				dst = i + SplitEvent::NumEventPerDimension * axis;
			}
		}
	}
}

char KdTreeBuilder::byCutoffEmptySpace(unsigned &dst)
{
	int res = -1;
	float vol, emptyVolume = -1.f;
	const int minHead = 5;
	const int maxTail = SplitEvent::NumEventPerDimension - 6;
	int i, head, tail;
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		head = 0;
		SplitEvent * cand = splitAt(axis, 0);
		if(cand->leftCount() == 0) {
			for(i = 1; i < SplitEvent::NumEventPerDimension - 1; i++) {
				cand = splitAt(axis, i);
				if(cand->leftCount() == 0)
					head = i;
			}
			
			if(head > minHead) {
				vol = head;
				if(vol > emptyVolume) {
					emptyVolume = vol;
					res = SplitEvent::NumEventPerDimension * axis + head;
				}
			}
		}
		tail = SplitEvent::NumEventPerDimension - 1;
		cand = splitAt(axis, SplitEvent::NumEventPerDimension - 1);
		if(cand->rightCount() == 0) {
			for(i = 1; i < SplitEvent::NumEventPerDimension - 1; i++) {
				cand = splitAt(axis, SplitEvent::NumEventPerDimension - 1 - i);
				if(cand->rightCount() == 0)
					tail = SplitEvent::NumEventPerDimension - 1 - i;
			}
			if(tail < maxTail) {
				vol = SplitEvent::NumEventPerDimension - tail;
				if(vol > emptyVolume) {
					emptyVolume = vol;
					res = SplitEvent::NumEventPerDimension * axis + tail;
				}
			}
		}
	}
	if(res > 0) {
		dst = res;
		//printf("cutoff at %i: %i left %i right %i\n", res/SplitEvent::NumEventPerDimension,  res%SplitEvent::NumEventPerDimension, m_event[res].leftCount(), m_event[res].rightCount());
	}
	return res>0;
}

void KdTreeBuilder::partition(BuildKdTreeContext &leftCtx, BuildKdTreeContext &rightCtx)
{
	
	SplitEvent &e = m_event[m_bestEventIdx];
	if(e.leftCount() > 0)
		leftCtx.createIndirection(e.leftCount());
	if(e.rightCount() > 0)
		rightCtx.createIndirection(e.rightCount());
	
	BoundingBox leftBox, rightBox;

	m_bbox.split(e.getAxis(), e.getPos(), leftBox, rightBox);
	leftCtx.setBBox(leftBox);
	rightCtx.setBBox(rightBox);
	
	BoundingBox *boxSrc = GlobalContext->primitiveBoxes();
	unsigned *indices = m_context->indices();
	
	unsigned *leftIdxDst = leftCtx.indices();
	unsigned *rightIdxDst = rightCtx.indices();
	
	int side;
	unsigned ind;
	for(unsigned i = 0; i < m_numPrimitive; i++) {
		ind = *indices;
		const BoundingBox * primBox = &boxSrc[ind];
		
		side = e.side(*primBox);
		
		if(side < 2) {
			if(primBox->touch(leftBox)) {
			*leftIdxDst = ind;
			leftIdxDst++;
			
			}
		}
		if(side > 0) {
			if(primBox->touch(rightBox)) {
			*rightIdxDst = ind;
			rightIdxDst++;
			}
		}
		indices++;
	}
}

void KdTreeBuilder::verbose() const
{
	printf("unsplit cost %f = 2 * %i box %f\n", 2.f * m_numPrimitive, m_numPrimitive, m_bbox.area());
	m_event[m_bestEventIdx].verbose();
	printf("chose split %i: %i\n", m_bestEventIdx/SplitEvent::NumEventPerDimension,  m_bestEventIdx%SplitEvent::NumEventPerDimension);
}
