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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE
//! [0]
class GLWidget;
class ToolBox;
class BrushControl;
class FeatherEdit;
class TimeControl;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
	virtual void closeEvent(QCloseEvent *event);
private:
    GLWidget *glWidget;
	ToolBox * m_tools;
	BrushControl * m_brushControl;
	FeatherEdit * m_featherEdit;
	TimeControl * m_timeControl;
	QMenu * fileMenu;
	QMenu *recentFilesMenu;
	QMenu * windowMenu;
	QAction * showBrushControlAct;
	QAction * showFeatherEditAct;
	QAction * showTimeControlAct;
	QAction * newSceneAct;
	QAction * openSceneAct;
	QAction * saveSceneAct;
	QAction * saveSceneAsAct;
	QAction * importMeshAct;
	QAction * importBakeAct;
	QAction * revertAct;
	QAction * exportBakeAct;
	enum { MaxRecentFiles = 5 };
	QAction *recentFileActs[MaxRecentFiles];
private:
	void createActions();
	void createMenus();
	void updateRecentFileActions();
	
public slots:
	void receiveToolContext(int a);
    void receiveToolAction(int a);
	void setWorkTitle(QString name);
	void showMessage(QString msg);
	
private slots:
	void openFile();
	void openRecentFile();
};
//! [0]

#endif
