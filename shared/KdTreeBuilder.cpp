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
#define NUMEVENTTHREAD 96

KdTreeBuilder::KdTreeBuilder(BuildKdTreeContext &ctx) 
{
	m_context = &ctx;
	m_numPrimitive = ctx.getNumPrimitives();
	m_primitiveClassification = new char[m_numPrimitive];
	m_bbox = ctx.getBBox();
	
	calculateBins();
	calculateSplitEvents();
}

KdTreeBuilder::~KdTreeBuilder() 
{
	//printf("builder quit\n");
	delete[] m_event;
	delete[] m_bins;
	delete[] m_primitiveClassification;
}

void KdTreeBuilder::calculateBins()
{
	BoundingBox *primBoxes = m_context->m_primitiveBoxes;
	m_bins = new MinMaxBins[SplitEvent::Dimension];
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		m_bins[axis].create(33, m_bbox.getMin(axis), m_bbox.getMax(axis));
	
		for(unsigned i = 0; i < m_numPrimitive; i++) {
			BoundingBox &primBox = primBoxes[i];
			m_bins[axis].add(primBox.getMin(axis), primBox.getMax(axis));
		}
		
		m_bins[axis].scan();
	}
}

void KdTreeBuilder::calculateSplitEvents()
{
	SplitEvent::NumPrimitive = m_numPrimitive;
	//SplitEvent::PrimitiveIndices = m_context->indices();
	SplitEvent::PrimitiveBoxes = m_context->m_primitiveBoxes;
	SplitEvent::ParentBoxArea = m_bbox.area();
	
	const unsigned numEvent = numEvents();
	m_event = new SplitEvent[numEvent];
	int eventIdx = 0;
	unsigned leftNumPrim, rightNumPrim;
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		const float min = m_bbox.getMin(axis);
		const float max = m_bbox.getMax(axis);
		const float delta = (max - min) / 33.f;
		for(int i = 1; i < 33; i++) {
			SplitEvent &event = m_event[eventIdx];
			event.setAxis(axis);
			event.setPos(min + delta * i);
			m_bins[axis].get(i - 1, leftNumPrim, rightNumPrim);
			event.setLeftRightNumPrim(leftNumPrim, rightNumPrim);
			eventIdx++;
		}
	}
/*	
	boost::thread eventThread[NUMEVENTTHREAD];
	
	for(unsigned i = 0; i < NUMEVENTTHREAD; i++) {
		eventThread[i] = boost::thread(&SplitEvent::calculateCost, &m_event[i]);
	}
	
	for(unsigned i = 0; i < NUMEVENTTHREAD; i++) {
		eventThread[i].join();
	}
	*/

/*
	for(unsigned i = 0; i < numEvent; i++) {
		m_event[i].calculateCost();
	}
*/

	BoundingBox *primBoxes = m_context->m_primitiveBoxes;
	for(unsigned i = 0; i < m_numPrimitive; i++) {
		const BoundingBox &primBox = primBoxes[i];
		for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
			const float min = m_bbox.getMin(axis);
			const float max = m_bbox.getMax(axis);
			const float delta = (max - min) / 33.f;
			const int eventOffset = axis * 32;
			
			int minGrid = (primBox.getMin(axis) - min) / delta;
			
			if(minGrid < 0) minGrid = 0;
			else if(minGrid > 31) minGrid = 31;
			
			for(int g = minGrid + 1; g < 32; g++)
				m_event[eventOffset + g].updateLeftBox(primBox);

			int maxGrid = (primBox.getMax(axis) - min) / delta;
			
			if(maxGrid < 0) maxGrid = 0;
			else if(maxGrid > 31) maxGrid = 31;
			
			for(int g = 0; g <= maxGrid - 1; g++)
				m_event[eventOffset + g].updateRightBox(primBox);
		}
	}
	
	for(unsigned i = 0; i < numEvent; i++) {
		m_event[i].calculateCost();
	}
	
	//for(unsigned j = 0; j < m_numPrimitive; j++) {
		//unsigned &primIdx = m_indices[j];
		//BoundingBox &primBox = primBoxes[primIdx];
		
		
		//Primitive *prim = m_primitives[i];
		//BaseMesh *mesh = (BaseMesh *)(prim->getGeometry());
		//unsigned triIdx = prim->getComponentIndex();
		
		//mesh->calculateBBox(triIdx);
	//}

}

void KdTreeBuilder::calculateSides()
{
	BoundingBox *primBoxes = m_context->m_primitiveBoxes;

	SplitEvent &e = m_event[m_bestEventIdx];
	for(unsigned i = 0; i < m_numPrimitive; i++) {
		BoundingBox &primBox = primBoxes[i];
		m_primitiveClassification[i] = e.side(primBox);
	}
}

const SplitEvent *KdTreeBuilder::bestSplit()
{
	m_bestEventIdx = 0;//axis * 32 + 16;
	float lowest = m_event[0].getCost();
	for(unsigned i = 0; i < numEvents(); i++) {
		//m_event[i].verbose();
		if(m_event[i].getCost() < lowest) {
			lowest = m_event[i].getCost();
			m_bestEventIdx = i;
		}
	}
	cutoffEmptySpace();
	calculateSides();
	return &m_event[m_bestEventIdx];
}

void KdTreeBuilder::cutoffEmptySpace()
{
	IndexLimit emptySpace[3];
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		float preCost = -1.f;
		int eventStart = 32 * axis;
		EmptySpace cutoff;
		for(unsigned i = 0; i < 32; i++) {
			if(m_event[eventStart + i].getCost() != preCost) {
				IndexLimit block;
				block.low = i;
				cutoff.push_back(block);
			}
			else {
				IndexLimit &lastBlock = cutoff.back();
				lastBlock.high = i;
			}
			preCost = m_event[eventStart + i].getCost();
		}
		
		for (std::vector<IndexLimit>::iterator it = cutoff.begin() ; it < cutoff.end(); it++ ) {
			IndexLimit block = *it;
			if(block.high - block.low > 7) {
				if(block.high - block.low > emptySpace[axis].high - emptySpace[axis].low)
					emptySpace[axis] = block;
			}
		}
	}
	
	float emptyArea[3];
	
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		if(emptySpace[axis].high > emptySpace[axis].low) {
			emptyArea[axis] = m_bbox.distance(axis) * m_bbox.crossSectionArea(axis) * (emptySpace[axis].high - emptySpace[axis].low);
		}
		else
			emptyArea[axis] = -99.f;
	}
	
	float maxEmptySpace = -1.f;
	int maxEmptyAxis = -1;
	for(int axis = 0; axis < SplitEvent::Dimension; axis++) {
		if(emptyArea[axis] > maxEmptySpace) {
			maxEmptySpace = emptyArea[axis];
			maxEmptyAxis = axis;
		}
	}
	
	if(maxEmptySpace < 0.f || maxEmptyAxis < 0) return;
	
	//printf("%i: empty %i - %i\n", maxEmptyAxis, emptySpace[maxEmptyAxis].low, emptySpace[maxEmptyAxis].high);
	if(emptySpace[maxEmptyAxis].low == 0)
		m_bestEventIdx = maxEmptyAxis * 32 + emptySpace[maxEmptyAxis].high;
	else
		m_bestEventIdx = maxEmptyAxis * 32 + emptySpace[maxEmptyAxis].low;
}

unsigned KdTreeBuilder::numEvents() const
{
	return 32 * SplitEvent::Dimension;
}

void KdTreeBuilder::partitionLeft(BuildKdTreeContext &ctx)
{	
	SplitEvent &e = m_event[m_bestEventIdx];
	ctx.create(e.leftCount());
	
	BoundingBox leftBox, rightBox;

	m_bbox.split(e.getAxis(), e.getPos(), leftBox, rightBox);
	ctx.setBBox(leftBox);
	
	unsigned *indices = m_context->indices();
	BoundingBox *boxes = m_context->m_primitiveBoxes;
	int count = 0;
	for(unsigned i = 0; i < m_numPrimitive; i++) {
		int side = m_primitiveClassification[i];
		if(side < 2) {
			ctx.setPrimitiveIndex(count, indices[i]);
			ctx.setPrimitiveBBox(count, boxes[i]);
			count++;
		}
	}
	printf("%i to left side\n", count);	
}

void KdTreeBuilder::partitionRight(BuildKdTreeContext &ctx)
{	
	SplitEvent &e = m_event[m_bestEventIdx];
	ctx.create(e.rightCount());
	
	BoundingBox leftBox, rightBox;

	m_bbox.split(e.getAxis(), e.getPos(), leftBox, rightBox);
	ctx.setBBox(rightBox);
	
	unsigned *indices = m_context->indices();
	BoundingBox *boxes = m_context->m_primitiveBoxes;

	int count = 0;
	for(unsigned i = 0; i < m_numPrimitive; i++) {
		int side = m_primitiveClassification[i];
		if(side > 0) {
			ctx.setPrimitiveIndex(count, indices[i]);
			ctx.setPrimitiveBBox(count, boxes[i]);
			count++;
		}
	}
	printf("%i to right side\n", count);
}

