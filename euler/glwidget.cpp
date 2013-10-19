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

#include "KdTreeDrawer.h"
#include <Anchor.h>
#include <KdTree.h>
#include <IntersectionContext.h>
#include <BaseTransform.h>
#include <BaseBrush.h>
#include <TransformManipulator.h>

GLWidget::GLWidget(QWidget *parent) : Base3DView(parent)
{
	BaseTransform * grp0 = new BaseTransform;
	grp0->setName("/group0");
	m_groups.push_back(grp0);
	
	BaseTransform * child1 = new BaseTransform(grp0);
	child1->setName("/group0/child1");
	child1->setTranslation(Vector3F(36.f, 0.f, 13.f));
	grp0->addChild(child1);
	m_groups.push_back(child1);
	
	BaseTransform * child2 = new BaseTransform(child1);
	child2->setName("/group0/child1/child2");
	child2->setTranslation(Vector3F(24.f, 0.f, 12.f));
	child1->addChild(child2);
	m_groups.push_back(child2);
	
	solve();
}

GLWidget::~GLWidget()
{
}

void GLWidget::clientDraw()
{
	getDrawer()->coordsys(m_space, 8.f);

	std::vector<BaseTransform *>::iterator it = m_groups.begin();
	for(; it != m_groups.end(); ++it) {
		getDrawer()->transform(*it);
	}
	
	getDrawer()->manipulator(manipulator());
}

void GLWidget::clientSelect()
{	
	const Ray * ray = getIncidentRay();
	std::vector<BaseTransform *>::iterator it = m_groups.begin();
	BaseTransform * subject = *it;
	for(; it != m_groups.end(); ++it) {
		if((*it)->intersect(*ray)) subject = *it;
	}
	manipulator()->attachTo(subject);
	manipulator()->start(ray);
}

void GLWidget::clientDeselect()
{
}

void GLWidget::clientMouseInput()
{
	const Ray * ray = getIncidentRay();
	manipulator()->perform(ray);
}

void GLWidget::setAngleAlpha(double x)
{
	m_angles.x = x;
	solve();
}

void GLWidget::setAngleBeta(double x)
{
	m_angles.y = x;
	solve();
}

void GLWidget::setAngleGamma(double x)
{
	m_angles.z = x;
	solve();
}

void GLWidget::solve()
{
	m_space.setIdentity();
	m_space.rotateEuler(DegreeToAngle(m_angles.x), DegreeToAngle(m_angles.y), DegreeToAngle(m_angles.z));
	update();
}
