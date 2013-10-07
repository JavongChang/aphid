/*
 *  BaseScene.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/26/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "BaseScene.h"

BaseScene::BaseScene() : BaseFile("untitled")
{
	m_isDirty = false;
}

BaseScene::~BaseScene() {}

void BaseScene::setDirty()
{
	m_isDirty = true;
}

void BaseScene::setClean()
{
	m_isDirty = false;
}

bool BaseScene::isDirty() const
{
	return m_isDirty;
}

bool BaseScene::isUntitled() const
{
	return m_fileName == "untitled";
}

bool BaseScene::newScene()
{
	if(isDirty())
		if(!discardConfirm()) return false;
	
	clearScene();
	
	return true;
}

bool BaseScene::openScene(const std::string & fileName)
{
	if(isDirty()) {
		if(!discardConfirm()) return false;
	}
	
	clearScene();
	
	if(!FileExists(fileName)) {
		m_error = FileNotFound;
		return false;
	}
	
	readSceneFromFile(fileName);
	m_fileName = fileName;
	setClean();
	return true;
}

bool BaseScene::saveScene()
{
	if(writeSceneToFile(m_fileName)) {
		setClean();
		return true;
	}
	return false;
}

bool BaseScene::saveSceneAs(const std::string & fileName)
{
	m_fileName = fileName;
	return saveScene();
}

bool BaseScene::revertScene()
{
	std::string fileToRevert = fileName();
	clearScene();
	readSceneFromFile(fileToRevert);
	m_fileName = fileToRevert;
	setClean();
	return true;
}

void BaseScene::setLatestError(BaseScene::ErrorMsg err)
{
	m_error = err;
}

BaseScene::ErrorMsg BaseScene::latestError() const
{
	return m_error;
}

bool BaseScene::shouldSave()
{
	return false;
}

bool BaseScene::discardConfirm()
{
	return true;
}

void BaseScene::clearScene()
{
	setClean();
	m_fileName = "untitled";
}

bool BaseScene::writeSceneToFile(const std::string & fileName)
{
	return true;
}

bool BaseScene::readSceneFromFile(const std::string & fileName)
{
	return true;
}
