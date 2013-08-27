/*
 *  ToolBox.cpp
 *  masq
 *
 *  Created by jian zhang on 5/5/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <QtGui>
#include "ToolBox.h"
#include <ContextIconFrame.h>
#include <ActionIconFrame.h>

ToolBox::ToolBox(QWidget *parent) : QToolBar(parent) 
{
	createContext();
	
	createAction();
	
	for(std::vector<ContextIconFrame *>::iterator it = m_contextFrames.begin(); it != m_contextFrames.end(); ++it) {
		addWidget(*it);
		connect(*it, SIGNAL(contextEnabled(int)), this, SLOT(onContextFrameChanged(int)));
	}
	
	addSeparator();
	
	for(std::vector<ActionIconFrame *>::iterator it = m_actionFrames.begin(); it != m_actionFrames.end(); ++it) {
		addWidget(*it);
		connect(*it, SIGNAL(actionTriggered(int)), this, SLOT(onActionFrameTriggered(int)));
	}
}

ToolBox::~ToolBox() {}

void ToolBox::onContextFrameChanged(int c)
{
	setContext((InteractMode)c);
	for(std::vector<ContextIconFrame *>::iterator it = m_contextFrames.begin(); it != m_contextFrames.end(); ++it) {
		if((*it)->getContext() != c)
			(*it)->setIconIndex(0);
		else
			(*it)->setIconIndex(1);
	}
	emit contextChanged(c);
}

void ToolBox::onActionFrameTriggered(int a)
{
	emit actionTriggered(a);
}

void ToolBox::createContext()
{
    ContextIconFrame * createContour = new ContextIconFrame(this);
	
	createContour->addIconFile(":mallardFeatherGreen.png");
	createContour->addIconFile(":mallardFeatherGreenAct.png");
	createContour->setIconIndex(1);
	createContour->setContext(CreateBodyContourFeather);
	
	ContextIconFrame * selectAnchor = new ContextIconFrame(this);
	selectAnchor->addIconFile(":seledge.png");
	selectAnchor->addIconFile(":seledgeact.png");
	selectAnchor->setIconIndex(0);
	selectAnchor->setContext(SelectEdge);
	
	m_contextFrames.push_back(createContour);
	m_contextFrames.push_back(selectAnchor);
}

void ToolBox::createAction()
{
    ActionIconFrame * setWale = new ActionIconFrame(this);
	setWale->addIconFile(":setwale.png");
	setWale->addIconFile(":setwaleact.png");
	setWale->setIconIndex(0);
	setWale->setAction(SetWaleEdge);
	
	ActionIconFrame * setSingleWale = new ActionIconFrame(this);
	setSingleWale->addIconFile(":setsinglewale.png");
	setSingleWale->addIconFile(":setsinglewaleact.png");
	setSingleWale->setIconIndex(0);
	setSingleWale->setAction(SetSingleWaleEdge);
	
	ActionIconFrame * increaseWale = new ActionIconFrame(this);
	increaseWale->addIconFile(":increasewale.png");
	increaseWale->addIconFile(":increasewaleact.png");
	increaseWale->setIconIndex(0);
	increaseWale->setAction(IncreaseWale);
	
	ActionIconFrame * decreaseWale = new ActionIconFrame(this);
	decreaseWale->addIconFile(":decreasewale.png");
	decreaseWale->addIconFile(":decreasewaleact.png");
	decreaseWale->setIconIndex(0);
	decreaseWale->setAction(DecreaseWale);
	
	ActionIconFrame * increaseCourse = new ActionIconFrame(this);
	increaseCourse->addIconFile(":increasecourse.png");
	increaseCourse->addIconFile(":increasecourseact.png");
	increaseCourse->setIconIndex(0);
	increaseCourse->setAction(IncreaseCourse);
	
	ActionIconFrame * decreaseCourse = new ActionIconFrame(this);
	decreaseCourse->addIconFile(":decreasecourse.png");
	decreaseCourse->addIconFile(":decreasecourseact.png");
	decreaseCourse->setIconIndex(0);
	decreaseCourse->setAction(DecreaseCourse);

	m_actionFrames.push_back(setWale);
	m_actionFrames.push_back(setSingleWale);
	m_actionFrames.push_back(increaseWale);
	m_actionFrames.push_back(decreaseWale);
	m_actionFrames.push_back(increaseCourse);
	m_actionFrames.push_back(decreaseCourse);
}
