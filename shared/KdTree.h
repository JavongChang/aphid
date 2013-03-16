/*
 *  KdTree.h
 *  kdtree
 *
 *  Created by jian zhang on 10/16/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include <KdTreeNode.h>
#include <BaseMesh.h>
#include <BoundingBox.h>
#include <Primitive.h>
#include <BuildKdTreeStream.h>
#include <KdTreeBuilder.h>
class RayIntersectionContext;
typedef Primitive * primitivePtr;
	
class KdTree
{
public:
	KdTree();
	~KdTree();
	
	KdTreeNode* getRoot() const;
	
	void addMesh(BaseMesh* mesh);
	void create();
	
	char intersect(const Ray &ray, RayIntersectionContext & ctx);

	BoundingBox m_bbox;
	
private:
	void subdivide(KdTreeNode * node, BuildKdTreeContext & ctx, int level);
	char recusiveIntersect(KdTreeNode *node, const Ray &ray, RayIntersectionContext & ctx);
	char leafIntersect(KdTreeNode *node, const Ray &ray, RayIntersectionContext & ctx);
	BuildKdTreeStream m_stream;
	KdTreeNode *m_root;
};