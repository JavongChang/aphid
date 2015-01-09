/*
 *  BaseBuffer.cpp
 *  brdf
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <gl_heads.h>
#include "BaseBuffer.h"

BaseBuffer::BaseBuffer() : m_bufferName(0), m_bufferSize(0), m_native(0)
{
	m_bufferType = kUnknown;
}

BaseBuffer::~BaseBuffer() 
{
    if(m_native) delete[] m_native;
}

void BaseBuffer::create(float * data, unsigned size)
{
	m_bufferSize = size;
	createVBO(data, size);
	setBufferType(kVBO);
}

void BaseBuffer::create(unsigned size)
{
    m_bufferSize = size;
    m_native = new char[size];
    setBufferType(kOnHost);
}

void BaseBuffer::destroy()
{
	destroyVBO();
}

void BaseBuffer::createVBO(float * data, unsigned size)
{
	glGenBuffers(1, &m_bufferName);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferName);
	
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BaseBuffer::destroyVBO()
{
	if(m_bufferName == 0) return;
	
	glBindBuffer(1, m_bufferName);
	glDeleteBuffers(1, &m_bufferName);
}

const unsigned BaseBuffer::bufferName() const { return m_bufferName; }

const unsigned BaseBuffer::bufferSize() const { return m_bufferSize; }

void BaseBuffer::setBufferType(BaseBuffer::BufferType t) { m_bufferType = t; }
const BaseBuffer::BufferType BaseBuffer::bufferType() const { return m_bufferType; }
void BaseBuffer::setBufferSize(unsigned x) { m_bufferSize = x; }
char * BaseBuffer::data() const {return m_native; }