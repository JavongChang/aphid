/*
 *  BuildKdTreeContext.h
 *  kdtree
 *
 *  Created by jian zhang on 10/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <BoundingBox.h>
#include <KdTreeNodeArray.h>
#include <BoundingBoxList.h>
#include <IndexList.h>
#include <SplitEvent.h>
#include <BuildKdTreeStream.h>
#include <WorldGrid.h>
#include <Array.h>

class GroupCell : public sdb::Sequence<unsigned>
{
public:
	GroupCell(sdb::Entity * parent = NULL) : sdb::Sequence<unsigned>(parent) {}
	BoundingBox m_box;
};

class BuildKdTreeContext {
	sdb::WorldGrid<GroupCell, unsigned > * m_grid;
	
public:
	BuildKdTreeContext();
	BuildKdTreeContext(BuildKdTreeStream &data, const BoundingBox & b);
	~BuildKdTreeContext();
	
	void createIndirection(const unsigned &count);

	const unsigned & getNumPrimitives() const;

	unsigned *indices();
	BoundingBox * primitiveBoxes();
	
	void verbose() const;

	void setBBox(const BoundingBox &bbox);
	const BoundingBox & getBBox() const;
	float visitCost() const;
private:
	BoundingBox m_bbox;
	IndexList m_indices;
	BoundingBoxList m_primitiveBoxes;
	unsigned m_numPrimitive;
};