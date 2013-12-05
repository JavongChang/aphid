/*
 *  SingleModelView.cpp
 *  fit
 *
 *  Created by jian zhang on 5/6/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <QtGui>
#include <QtOpenGL>
#include <math.h>
#include "SingleModelView.h"
#include <KdTreeDrawer.h>
#include <PatchMesh.h>
#include <KdTree.h>
#include <Ray.h>
#include <EasemodelUtil.h>
#include <AnchorGroup.h>
#include <MeshTopology.h>
#include <ToolContext.h>

SingleModelView::SingleModelView(QWidget *parent) : Base3DView(parent)
{
	m_tree = new KdTree;
	m_mesh = new PatchMesh;
	m_topo = new MeshTopology;
	
	m_anchors = new AnchorGroup;
	m_anchors->setHitTolerance(.3f);
	m_shouldRebuildTree = false;
}
//! [0]

//! [1]
SingleModelView::~SingleModelView()
{
}

void SingleModelView::clientDraw()
{
	KdTreeDrawer *drawer = getDrawer();
	drawer->hiddenLine(mesh());	
	drawSelection();
	drawAnchors();
}
//! [7]

//! [9]
void SingleModelView::clientSelect()
{
	Vector3F hit;
	Ray ray = *getIncidentRay();
	if(interactMode() == ToolContext::SelectVertex) {
		pickupComponent(ray, hit);
	}
	else {
		m_anchors->pickupAnchor(ray, hit);
	}
}
//! [9]

void SingleModelView::clientDeselect() {}

//! [10]
void SingleModelView::clientMouseInput()
{
	Vector3F hit;
	Ray ray = *getIncidentRay();
	if(interactMode() == ToolContext::SelectVertex) {
		pickupComponent(ray, hit);
	}
}

Vector3F SingleModelView::sceneCenter() const
{
	if(m_tree->isEmpty()) return Base3DView::sceneCenter();
	return m_tree->getBBox().center();
}

bool SingleModelView::anchorSelected(float wei)
{
	if(getActiveComponent()->numVertices() < 1) return false;
	Anchor *a = new Anchor(*getActiveComponent());
	a->setWeight(wei);
	m_anchors->addAnchor(a);
	clearSelection();
	return true;
}

bool SingleModelView::removeLastAnchor(unsigned & idx)
{
    if(m_anchors->numAnchors() < 1) return false;
    m_anchors->removeLast();
    idx = (unsigned)m_anchors->numAnchors();
    return true;
}

bool SingleModelView::removeActiveAnchor(unsigned & idx)
{
    if(!m_anchors->hasActiveAnchor()) return false;
    m_anchors->activeAnchorIdx(idx);
    m_anchors->removeActive();
    return true;
}

bool SingleModelView::pickupComponent(const Ray & ray, Vector3F & hit)
{
	getIntersectionContext()->reset(ray);
	if(!m_tree->intersect(getIntersectionContext())) 
		return false;
	hit = getIntersectionContext()->m_hitP;
	addHitToSelection();
	return true;
}

bool SingleModelView::hitTest(const Ray & ray, Vector3F & hit)
{
	getIntersectionContext()->reset(ray);
	if(!m_tree->intersect(getIntersectionContext())) 
		return false;
	hit = getIntersectionContext()->m_hitP;
	return true;
}

void SingleModelView::buildTree()
{
	if(m_tree) delete m_tree;
	m_tree = new KdTree;
	m_tree->addMesh(mesh());
	m_tree->create();
}

void SingleModelView::buildTopology()
{
	m_topo->buildTopology(mesh());
	getActiveComponent()->setTopology(m_topo);
    m_topo->calculateNormal();
}

void SingleModelView::open()
{
	QString temQStr = QFileDialog::getOpenFileName(this, 
		tr("Open Model File As Temple"), "../", tr("Mesh(*.m)"));
	
	if(temQStr == NULL)
		return;
		
	loadMesh(temQStr.toStdString());
}

void SingleModelView::loadMesh(std::string filename)
{
	//ESMUtil::Import(filename.c_str(), mesh());
	ESMUtil::ImportPatch(filename.c_str(), mesh());
	buildTopology();
	buildTree();
	update();
}

void SingleModelView::saveMesh(std::string filename)
{
	ESMUtil::Export(filename.c_str(), mesh());
}

void SingleModelView::save()
{
	QString temQStr = QFileDialog::getSaveFileName(this, 
		tr("Save Template to Model File"), "./untitled.m", tr("Mesh(*.m)"));
	
	if(temQStr == NULL)
		return;
		
	saveMesh(temQStr.toStdString());
	QMessageBox::information(this, tr("Success"), QString("Template saved as ").append(temQStr));
}

void SingleModelView::keyPressEvent(QKeyEvent *e)
{
    unsigned nouse;
	if(e->key() == Qt::Key_A) {
		anchorSelected(1.f);
	}
	else if(e->key() == Qt::Key_Z) {
		removeLastAnchor(nouse);
	}
	else if(e->key() == Qt::Key_X) {
		removeActiveAnchor(nouse);
	}

	Base3DView::keyPressEvent(e);
}

void SingleModelView::focusInEvent(QFocusEvent * event)
{
	if(m_shouldRebuildTree) {
		buildTree();
		m_shouldRebuildTree = false;
	}
	Base3DView::focusInEvent(event);
}

void SingleModelView::setRebuildTree()
{
	m_shouldRebuildTree = true;
}

bool SingleModelView::shouldRebuildTree() const
{
	return m_shouldRebuildTree;
}

void SingleModelView::drawAnchors()
{
	if(m_anchors->numAnchors() < 1) return;
	
	Anchor *a;
	for(a = m_anchors->firstAnchor(); m_anchors->hasAnchor(); a = m_anchors->nextAnchor())
		getDrawer()->anchor(a, a == m_anchors->activeAnchor());

	if(!m_anchors->hasActiveAnchor()) return;
	    
	getDrawer()->spaceHandle(m_anchors->activeAnchor());
}

AnchorGroup * SingleModelView::getAnchors() const
{
	return m_anchors;
}

KdTree * SingleModelView::getTree() const
{
	return m_tree;
}

PatchMesh * SingleModelView::mesh()
{
	return m_mesh;
}

void SingleModelView::drawIntersection() const
{
    /*IntersectionContext * ctx = getIntersectionContext();
    if(!ctx->m_success) return;
    
	getDrawer()->drawPrimitivesInNode(m_tree, (KdTreeNode *)ctx->m_cell);*/
	
	Base3DView::drawIntersection();
}

void SingleModelView::clearTree()
{
	m_tree->cleanup();
}

void SingleModelView::clearTopology()
{
	m_topo->cleanup();
}
//:~