/*
 *  BuildKdTreeStream.h
 *  kdtree
 *
 *  Created by jian zhang on 10/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <BoundingBox.h>
#include <PrimitiveArray.h>
#include <IndexArray.h>
#include <KdTreeNodeArray.h>
#include <BaseMesh.h>
class BuildKdTreeStream {
public:
	BuildKdTreeStream();
	~BuildKdTreeStream();
	void appendMesh(BaseMesh* mesh);
	
	const unsigned getNumPrimitives() const;
	const PrimitiveArray &getPrimitives() const;
	const IndexArray &getIndices() const;
	
	PrimitiveArray &primitives();
	IndexArray &indices();
	
	KdTreeNode *createTreeBranch();
	KdTreeNode *firstTreeBranch();
	
private:
	PrimitiveArray m_primitives;
	IndexArray m_indices;
	KdTreeNodeArray m_nodes;
};