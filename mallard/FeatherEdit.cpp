/*
 *  FeatherEdit.cpp
 *  mallard
 *
 *  Created by jian zhang on 10/2/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "FeatherEdit.h"
#include <QtGui>
#include <MlUVView.h>
#include <ToolBox.h>
#include <FeatherEditTool.h>

FeatherEdit::FeatherEdit(QWidget *parent)
    : QDialog(parent)
{
	m_view = new MlUVView(this);
	
	FeatherEditTool *tools = new FeatherEditTool(this);
	m_view->setInteractContext(tools);
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(tools);
	layout->setStretch(0, 0);
	layout->addWidget(m_view);
	layout->setStretch(1, 1);
	setLayout(layout);
	setWindowTitle(tr("Feather Editor"));
	
	setContentsMargins(0, 0, 0, 0);
	layout->setContentsMargins(0, 0, 0, 0);
	
	connect(tools, SIGNAL(actionTriggered(int)), this, SLOT(receiveToolAction(int)));
}

void FeatherEdit::receiveToolAction(int a)
{
	if(a == ToolContext::AddFeatherExample)
		m_view->addFeather();
	else if(a == ToolContext::RemoveFeatherExample)
		m_view->removeSelectedFeather();
	else if(a == ToolContext::IncreaseFeathExampleNSegment)
		m_view->changeSelectedFeatherNSegment(1);
	else if(a == ToolContext::DecreaseFeathExampleNSegment)
		m_view->changeSelectedFeatherNSegment(-1);
}
