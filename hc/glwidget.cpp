/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"

#include "MeshLaplacian.h"
#include "KdTreeDrawer.h"
#include <KdTree.h>
#include <Ray.h>
#include <SelectionArray.h>
#include <HarmonicCoord.h>
#include <DeformationTarget.h>
#include <AccumulateDeformer.h>
#include <TargetGraph.h>
#include <ControlGraph.h>
#include <EasemodelUtil.h>
#include <Anchor.h>

static Vector3F rayo(15.299140, 20.149620, 97.618355), raye(-141.333694, -64.416885, -886.411499);
	
//! [0]
GLWidget::GLWidget(QWidget *parent) : Base3DView(parent)
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(simulate()));
	timer->start(30);
	
#ifdef WIN32
	EasyModel * eye = new EasyModel("D:/aphid/mdl/eye.m");
	EasyModel * eyeU = new EasyModel("D:/aphid/mdl/eyeU.m");
	EasyModel * eyeL = new EasyModel("D:/aphid/mdl/eyeL.m");
#else
	EasyModel * eye = new EasyModel("/Users/jianzhang/aphid/mdl/eye.m");
	EasyModel * eyeU = new EasyModel("/Users/jianzhang/aphid/mdl/eyeU.m");
	EasyModel * eyeL = new EasyModel("/Users/jianzhang/aphid/mdl/eyeL.m");
#endif
	m_mesh = new MeshLaplacian;
	m_mesh1 = new MeshLaplacian;
	m_mesh2 = new MeshLaplacian;
	
	ESMUtil::copy(eye, m_mesh);
	ESMUtil::copy(eyeU, m_mesh1);
	ESMUtil::copy(eyeL, m_mesh2);
	
	delete eye;
	delete eyeU;
	delete eyeL;
	
	m_mesh->buildTopology();
	m_mesh1->buildTopology();
	m_mesh2->buildTopology();

	m_drawer = new KdTreeDrawer;
	
	m_harm = new HarmonicCoord;
	m_harm->setMesh(m_mesh);
	m_harm->setTopology(m_mesh->connectivity());
	m_harm->addValue(1);
	m_harm->addValue(2);
	
	m_tree = new KdTree;
	m_tree->addMesh(m_mesh);
	m_tree->create();
	
	m_selected = new SelectionArray;
	m_selected->setComponentFilterType(PrimitiveFilter::TVertex);
	
	m_mode = SelectCompnent;
	m_intersectCtx = new RayIntersectionContext;
	m_intersectCtx->setComponentFilterType(PrimitiveFilter::TVertex);
	
	DeformationTarget * analysis1 = new DeformationTarget;
	analysis1->setMeshes(m_mesh, m_mesh1);
	analysis1->setWeightMap(m_harm->value(1));
	
	DeformationTarget * analysis2 = new DeformationTarget;
	analysis2->setMeshes(m_mesh, m_mesh2);
	analysis2->setWeightMap(m_harm->value(2));
	
	m_deformer = new AccumulateDeformer;
	m_deformer->setMesh(m_mesh);
	m_deformer->addTargetAnalysis(analysis1);
	m_deformer->addTargetAnalysis(analysis2);
}
//! [0]

//! [1]
GLWidget::~GLWidget()
{
}

void GLWidget::clientDraw()
{
	if(m_mode != TransformAnchor) {
		m_drawer->setWired(1);
		m_drawer->setGrey(0.9f);
		m_drawer->drawMesh(m_mesh);
	}
    else {
		m_drawer->setWired(0);
		glTranslatef(0,-20,0);
		m_harm->plotColor(1);
		m_drawer->field(m_harm);
		glTranslatef(0,-20,0);
		m_harm->plotColor(2);
		m_drawer->field(m_harm);
		glTranslatef(0,40,0);
	}	
	m_drawer->setGrey(0.5f);
	//m_drawer->drawKdTree(m_tree);
	//m_drawer->setWired(0);
	m_drawer->setColor(0.f, 1.f, 0.4f);
	m_drawer->components(m_selected);
	for(std::vector<Anchor *>::iterator it = m_anchors.begin(); it != m_anchors.end(); ++it)
		m_drawer->anchor(*it);
	/*	
	const unsigned nv = m_analysis->numVertices();
	Vector3F vi, dc;
	
	for(unsigned i = 0; i < nv; i++) {
		vi = m_analysis->restP(i);
		Matrix33F orient = m_analysis->getR(i);
		glPushMatrix();
		glTranslatef(vi.x, vi.y, vi.z);
		//m_drawer->coordsys(orient);
		glPopMatrix();
		
		m_drawer->beginLine();
		dc = m_analysis->getT(i);
		m_drawer->setColor(0.f, 0.f, 1.f);
		glVertex3f(vi.x, vi.y, vi.z);
		glVertex3f(vi.x + dc.x, vi.y + dc.y, vi.z + dc.z);
		m_drawer->end();
	}
	*/
	glTranslatef(20,0,0);
	m_drawer->setWired(1);
	m_drawer->setColor(0.f, 1.f, .4f);
	m_drawer->drawMesh(m_mesh, m_deformer);
	glTranslatef(0,-20,0);
	m_drawer->setColor(0.f, 4.f, 1.f);
	m_drawer->drawMesh(m_mesh1);
	glTranslatef(0,-20,0);
	m_drawer->setColor(1.f, 4.f, 0.f);
	m_drawer->drawMesh(m_mesh2);
/*	
	glBegin(GL_LINES);
	glColor3f(1,0,0);
	glVertex3f(rayo.x, rayo.y, rayo.z);
	glColor3f(0,0,1);
	glVertex3f(raye.x, raye.y, raye.z);
	glEnd();

	m_drawer->setWired(1);
	m_drawer->setColor(0.f, 1.f, 1.f);
	//m_drawer->box(intersectCtx.getBBox());
	m_drawer->setWired(0);
*/
}
//! [7]

//! [9]
void GLWidget::clientSelect(Vector3F & origin, Vector3F & displacement, Vector3F & hit)
{
	rayo = origin;
	raye = origin + displacement;
	
	Ray ray(rayo, raye);
	if(m_mode == SelectCompnent) {
		if(!pickupComponent(ray, hit))
			m_selected->reset();
	}
	else {
		m_activeAnchor = 0;
		pickupAnchor(ray, hit);
	}
}
//! [9]

void GLWidget::clientDeselect()
{

}

//! [10]
void GLWidget::clientMouseInput(Vector3F & origin, Vector3F & displacement, Vector3F & stir)
{
	rayo = origin;
	raye = origin + displacement;
	Ray ray(rayo, raye);
	if(m_mode == SelectCompnent) {
		Vector3F hit;
		pickupComponent(ray, hit);
	}
	else {
	    //if(!m_activeAnchor) return;
		//m_activeAnchor->translate(stir);
		//m_harm->solve();
		//m_deformer->solve();
	}
}
//! [10]

void GLWidget::simulate()
{
    update();
}

void GLWidget::anchorSelected(float wei)
{
	if(m_selected->numVertices() < 1) return;
	Anchor *a = new Anchor(*m_selected);
	a->setWeight(wei);
	m_anchors.push_back(a);
	m_selected->reset();
}

void GLWidget::startDeform()
{
	if(m_anchors.size() < 1) return;
	m_harm->precompute(m_anchors);
	m_deformer->precompute();
	m_mode = TransformAnchor;
}

bool GLWidget::pickupAnchor(const Ray & ray, Vector3F & hit)
{
	float minDist = 10e8;
	float t;
	for(std::vector<Anchor *>::iterator it = m_anchors.begin(); it != m_anchors.end(); ++it) {
		if((*it)->intersect(ray, t, 1.f)) {
			if(t < minDist) {
				m_activeAnchor = (*it);
				minDist = t;
				hit = ray.travel(t);
			}
		}
	}
	return minDist < 10e8;
}

bool GLWidget::pickupComponent(const Ray & ray, Vector3F & hit)
{
	m_intersectCtx->reset();
	if(m_tree->intersect(ray, m_intersectCtx)) {
	    m_selected->add(m_intersectCtx->m_geometry, m_intersectCtx->m_componentIdx);
		hit = m_intersectCtx->m_hitP;
		return true;
	}
	return false;
}

void GLWidget::setControlGraph(ControlGraph * graph)
{
	m_graph = graph;
}

void GLWidget::onHandleChanged(unsigned ihandle)
{
	if(m_mode != TransformAnchor) return;
	//qDebug()<<" handle moved "<< ihandle;
	TargetGraph * graph = m_graph->activeGraph();
	for(unsigned i = graph->firstDirtyTarget(); graph->hasDirtyTarget(); i = graph->nextDirtyTarget()) {
		//qDebug()<<" target "<< i;
		updateConstrains(i);
		m_harm->solve(i);
	}
	m_deformer->solve();
}

void GLWidget::updateConstrains(unsigned itarget)
{
	const unsigned nanchor = m_harm->numAnchorPoints();
	for(unsigned i = 0; i < nanchor; i++) {
		float wei = 0.f;
		TargetGraph * graph = m_graph->getGraph(i);
		if(graph) wei = graph->targetWeight(itarget);
		
		m_harm->setConstrain(i, wei);
	}
}

