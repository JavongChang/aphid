/*
 *  KdTree.cpp
 *  kdtree
 *
 *  Created by jian zhang on 10/16/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include "KdTree.h"
#include <QElapsedTimer>

const char *byte_to_binary(int x)
{
    static char b[33];
    b[32] = '\0';

    for (int z = 0; z < 32; z++) {
        b[31-z] = ((x>>z) & 0x1) ? '1' : '0';
    }

    return b;

}

KdTree::KdTree() 
{
	m_root = new KdTreeNode;
	printf("axis mask        %s\n", byte_to_binary(KdTreeNode::EInnerAxisMask));
	printf("type        mask %s\n", byte_to_binary(KdTreeNode::ETypeMask));
	printf("indirection mask %s\n", byte_to_binary(KdTreeNode::EIndirectionMask));
	
	printf("32 bit align mask %s\n", byte_to_binary(0xffffffff - 31));
	
	unsigned lc = 127;
	lc = lc & (0xffffffff - 31);
	printf("32 bit align lc %d\n", lc);
	printf("32 bit align lc %s\n", byte_to_binary(lc));
	printf("2199              %s\n", byte_to_binary(2199));

	printf("31              %s\n", byte_to_binary(31));
	
	printf("2199 & 31         %s\n", byte_to_binary(2199 & 31));
	printf("2199 & 31         %d\n", 2199 & 31);
	
	printf("node sz %d\n", (int)sizeof(KdTreeNode));
	printf("prim sz %d\n", (int)sizeof(Primitive));
	printf("event sz %d\n", (int)sizeof(SplitEvent));
	printf("bbox sz %d\n", (int)sizeof(BoundingBox));
}

KdTree::~KdTree() 
{
	delete m_root;
}

KdTreeNode* KdTree::getRoot() const
{ 
	return m_root; 
}

void KdTree::addMesh(BaseMesh* mesh)
{
	unsigned nf = mesh->getNumFaces();
	printf("add %i triangles\n", nf);
	m_stream.appendMesh(mesh);
	const BoundingBox box = mesh->calculateBBox();
	m_bbox.expandBy(box);
}

void KdTree::create()
{	
	printf("input primitive count %d\n", m_stream.getNumPrimitives());
	printf("tree bbox: %f %f %f - %f %f %f\n", m_bbox.m_min.x, m_bbox.m_min.y, m_bbox.m_min.z, m_bbox.m_max.x, m_bbox.m_max.y, m_bbox.m_max.z);
	
	QElapsedTimer timer;
	timer.start();
	
	BuildKdTreeContext *ctx = new BuildKdTreeContext(m_stream);
	ctx->setBBox(m_bbox);
	
	subdivide(m_root, *ctx, 0);
	ctx->verbose();
	delete ctx;
	std::cout << "kd tree finished after " << timer.elapsed() << "ms\n";
}

void KdTree::subdivide(KdTreeNode * node, BuildKdTreeContext & ctx, int level)
{
	if(ctx.getNumPrimitives() < 64 || level == 3) {
		node->setLeaf(true);
		return;
	}
	
	printf("subdiv node level %i\n", level);
	
	KdTreeBuilder builder(ctx);

	const SplitEvent *plane = builder.bestSplit();
	
	//plane->verbose();
	
	node->setAxis(plane->getAxis());
	node->setSplitPos(plane->getPos());
	KdTreeNode* branch = m_stream.createTreeBranch();
	
	node->setLeft(branch);
	node->setLeaf(false);

	PartitionBound subBound;
	
	if(plane->leftCount() > 0) {
		BuildKdTreeContext *leftCtx = new BuildKdTreeContext();
		builder.partitionLeft(*leftCtx);
	
		subdivide(branch, *leftCtx, level + 1);
		delete leftCtx;
	}
	if(plane->rightCount() > 0) {
		BuildKdTreeContext *rightCtx = new BuildKdTreeContext();
		builder.partitionRight(*rightCtx);
	
		subdivide(branch + 1, *rightCtx, level + 1);
		delete rightCtx;
	}
	//printf("subdiv end %i\n", level);
}
