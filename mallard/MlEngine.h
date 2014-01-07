/*
 *  MlEngine.h
 *  mallard
 *
 *  Created by jian zhang on 12/31/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "AnorldFunc.h"
#include <boost/thread.hpp>
class BarbWorks;
class MlEngine : public AnorldFunc {
public:
	MlEngine();
	MlEngine(BarbWorks * w);
	virtual ~MlEngine();
	void setWorks(BarbWorks * w);
	virtual void preRender();
	virtual void render();
	void interruptRender();
protected:

private:
    void fineOutput();
	void testOutput();
	void monitorProgressing(BarbWorks * work);
private:
	boost::thread m_workingThread;
	boost::thread m_progressingThread;
	BarbWorks * m_barb;
};