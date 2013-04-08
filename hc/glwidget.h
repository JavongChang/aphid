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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <Base3DView.h>
#include <RayIntersectionContext.h>
class MeshLaplacian;
class KdTreeDrawer;
class KdTree;
class HarmonicCoord;
class SelectionArray;
class WeightHandle;
class Ray;
class DeformationTarget;
class AccumulateDeformer;
class ControlGraph;

//! [0]
class GLWidget : public Base3DView
{
    Q_OBJECT

public:
    enum InteractMode {
        SelectCompnent,
        TransformAnchor
    };
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

	virtual void clientDraw();
    virtual void clientSelect(Vector3F & origin, Vector3F & displacement, Vector3F & hit);
    virtual void clientDeselect();
    virtual void clientMouseInput(Vector3F & origin, Vector3F & displacement, Vector3F & stir);
	
	void anchorSelected(float wei);
	void startDeform();
	bool pickupAnchor(const Ray & ray, Vector3F & hit);
	bool pickupComponent(const Ray & ray, Vector3F & hit);
	
	void setControlGraph(ControlGraph * graph);
//! [2]
protected:
    
//! [3]
private:
    MeshLaplacian * m_mesh;
	MeshLaplacian * m_mesh1;
	MeshLaplacian * m_mesh2;
    KdTreeDrawer * m_drawer;
	KdTree * m_tree;
	SelectionArray * m_selected;
	InteractMode m_mode;
	std::vector<WeightHandle *> m_anchors;
	WeightHandle * m_activeAnchor;
	HarmonicCoord * m_harm;
	RayIntersectionContext * m_intersectCtx;
	AccumulateDeformer * m_deformer;
	ControlGraph * m_graph;
private slots:
    void simulate();
	
public slots:
    void onHandleChanged(unsigned ihandle);

};
//! [3]

#endif
