/*
 *  BaseArray.cpp
 *  kdtree
 *
 *  Created by jian zhang on 10/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include "BaseArray.h"

#define BASEARRAYBLOCK 524288

BaseArray::BaseArray() 
{
	m_current = 0;
	m_elementSize = 1;
}

BaseArray::~BaseArray() {}

void BaseArray::clear() 
{
	for (std::vector<PtrTup *>::iterator it = m_blocks.begin(); 
				it != m_blocks.end(); ++it)
		delete *it;
	m_blocks.clear();
	m_current = 0;
}

char *BaseArray::expandBy(unsigned size)
{
	if(m_current / m_elementSize + size > capacity()) {
		unsigned blockToCreate = (m_current + size * m_elementSize) / BASEARRAYBLOCK + 1 - numBlock();
		for(unsigned i = 0; i < blockToCreate; i++) {
			m_blocks.push_back(new PtrTup);
		}
	}
	return current();
}

void BaseArray::shrinkTo(unsigned size)
{
	if(size >= capacity()) return;
	unsigned blockIdx = size * m_elementSize / BASEARRAYBLOCK;
	unsigned i = 0;
	for (std::vector<PtrTup *>::iterator it = m_blocks.begin(); 
				it != m_blocks.end(); ++it) {
		if(i > blockIdx) {
			delete *it;
		}
		i++;
	}
	m_blocks.resize(blockIdx+1);
}

void BaseArray::begin()
{
	m_current = 0;
}

void BaseArray::next()
{
	m_current += m_elementSize;
}

char BaseArray::end() const
{
	return m_current >= capacity();
}

unsigned BaseArray::index() const
{
	return m_current / m_elementSize;
}

void BaseArray::setIndex(unsigned index)
{
	m_current = index * m_elementSize;
}

char *BaseArray::current()
{
	unsigned blockIdx = m_current / BASEARRAYBLOCK;
	unsigned offset = m_current % BASEARRAYBLOCK;
	return m_blocks[blockIdx]->aligned + offset;
}

char *BaseArray::at(unsigned index)
{
	unsigned blockIdx = index * m_elementSize / BASEARRAYBLOCK;
	unsigned offset = index * m_elementSize % BASEARRAYBLOCK;
	return m_blocks[blockIdx]->aligned + offset;
}

char *BaseArray::at(unsigned index) const
{
	unsigned blockIdx = index * m_elementSize / BASEARRAYBLOCK;
	unsigned offset = index * m_elementSize % BASEARRAYBLOCK;
	return m_blocks[blockIdx]->aligned + offset;
}

unsigned BaseArray::capacity() const 
{
	return numBlock() * numElementPerBlock();
}

unsigned BaseArray::numElementPerBlock() const
{
	return BASEARRAYBLOCK / m_elementSize;
}

unsigned BaseArray::numBlock() const
{
	return m_blocks.size();
}

void BaseArray::setElementSize(unsigned size)
{
	m_elementSize = size;
}

unsigned BaseArray::getElementSize() const
{
	return m_elementSize;
}

const char *byte_to_binary1(int x)
{
    static char b[33];
    b[32] = '\0';

    for (int z = 0; z < 32; z++) {
        b[31-z] = ((x>>z) & 0x1) ? '1' : '0';
    }

    return b;

}

void BaseArray::verbose() const
{
	std::cout<<"base array:\n";
    std::cout<<"elem size "<<getElementSize()<<"\n";
    std::cout<<"elem per blk "<<numElementPerBlock()<<"\n";
	std::cout<<"num blk "<<numBlock()<<"\n";
    std::cout<<"capacity "<<capacity()<<"\n";
    std::cout<<"current index "<<index()<<"\n";
    return;
	unsigned pre;
	int i = 0;
	for (std::vector<PtrTup *>::const_iterator it = m_blocks.begin(); 
				it != m_blocks.end(); ++it) {
			if(i > 0)
				printf("p[%i] at %i \n", i, (unsigned)(*it)->aligned - pre);
			pre = (unsigned)(*it)->aligned;
			
			printf("p[%i] at %s \n", i, byte_to_binary1(pre));
			//printf("p[%i] at %s \n", i, byte_to_binary1((unsigned)(*it)->raw));
			i++;
		}
		//
}
