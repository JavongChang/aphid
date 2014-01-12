/*
 *  SceneEdit.h
 *  mallard
 *
 *  Created by jian zhang on 1/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
class MlScene;
#include <QDialog>
class SceneTreeModel;
QT_BEGIN_NAMESPACE
class QTreeView;
QT_END_NAMESPACE

class SceneEdit : public QDialog
{
    Q_OBJECT
	
public:
	SceneEdit(MlScene * scene, QWidget *parent = 0);
	virtual ~SceneEdit();

signals:
	
public slots:
	
private:
	QTreeView * m_view;
	SceneTreeModel * m_model;
};
