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

#include <ManipulateView.h>
#include <MlScene.h>

class BezierDrawer;
class MlDrawer;
class MlEngine;
//! [0]
class GLWidget : public ManipulateView, public MlScene
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
	virtual void setFeatherTexture(const std::string & name);
	virtual void clientSelect();
	virtual void clientMouseInput();
	virtual void clientDeselect();
    virtual PatchMesh * activeMesh();
	
	virtual bool confirmDiscardChanges();
	virtual std::string chooseOpenFileName();
	virtual std::string chooseSaveFileName();
	virtual void doClear();
	virtual void doClose();
	virtual void beforeSave();
	
	void finishEraseFeather();
	void deselectFeather();
	void rebuildFeather();
	void clearFeather();
	void bakeFrames();
	void testRender();
	
	QString openSheet(QString name);
	
	virtual bool selectFeatherExample(unsigned x);
	virtual void setUseDisplaySize(bool x);
signals:
	void sceneNameChanged(QString name);
	void sendMessage(QString msg);
	void sendFeatherEditBackground(QString name);
	void renderResChanged(QSize s);
	void featherSelectionChanged();
	void sceneOpened();
public slots:
	void importBodyMesh();
	void cleanSheet();
	void saveSheet();
	void saveSheetAs();
    void revertSheet();
	void receiveFeatherEditBackground(QString name);
	void receiveFeatherAdded();
	void chooseBake();
	void updateOnFrame(int x);
	void exportBake();
	void importFeatherDistributionMap();
	void receiveFloodRegion(int state);
	void receiveEraseRegion(int state);
	void receiveCancelRender();
	void receiveBarbChanged();
	void receiveCameraChanged();
protected:
    virtual void clientDraw();
	virtual void focusOutEvent(QFocusEvent * event);
	virtual void clearSelection();
	virtual void resizeEvent( QResizeEvent * event );
	virtual char selectLight(const Ray & incident);
	virtual void importBody(const std::string & fileName);
	virtual void afterOpen();
private:
	void selectFeather(char byRegion = 0);
	void selectRegion();
	void floodFeather();
	void beginBaking();
	void endBaking();
	bool isBaking() const;
	void loadFeatherDistribution(const std::string & name);
	void showLights() const;
	void testPatch();
private:
	BezierDrawer * m_bezierDrawer;
	MlDrawer * m_featherDrawer;
	MlEngine * m_engine;
	int m_featherTexId, m_featherDistrId;
	char m_floodByRegion, m_eraseByRegion;
	
	void testCurvature();
};
//! [3]

#endif
