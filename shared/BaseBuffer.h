/*
 *  BaseBuffer.h
 *  brdf
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

class BaseBuffer {
public:
	BaseBuffer();
	virtual ~BaseBuffer();
	
	virtual void create(float * data, unsigned size);
	virtual void destroy();
	
	void createVBO(float * data, unsigned size);
	void destroyVBO();
	
	unsigned getBufferName() const;
	
	unsigned _buffereName;
};