/*
 *  VectorArray.h
 *  
 *
 *  Created by jian zhang on 1/2/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 *  un-organized array of *
 */
#pragma once
#include <vector>
#include "Entity.h"

namespace aphid {

namespace sdb {

template <typename T>
class VectorArray : public Entity {
	
	std::vector<T *> m_blocks;
	std::vector<T *> m_data;
	T * m_buf;
	int m_numData;
	
public:
	VectorArray(Entity * parent = NULL) : Entity(parent),
	m_numData(0) {}
	
	virtual ~VectorArray() 
	{ clear(); }
	
	void clear()
	{
		m_numData = 0;
		const int n = m_blocks.size();
		int i = 0;
		for(; i<n; i++) delete[] m_blocks[i];
		m_blocks.clear();
		m_data.clear();
	}
	
	void insert()
	{
		if((m_numData & 1023)==0) {
			m_buf = new T[1025];
			m_blocks.push_back(m_buf);
		}
		
		T * d = &m_buf[m_numData & 1023];
		m_data.push_back(d);
		m_numData++;
	}
	
	void insert(const T & a) 
	{
		if((m_numData & 1023)==0) {
			m_buf = new T[1025];
			m_blocks.push_back(m_buf);
		}
		
		T * d = &m_buf[m_numData & 1023];
		*d = a;
		m_data.push_back(d);
		m_numData++;
	}
	
	const int & size() const
	{ return m_numData; }
	
	T * get(int idx) const
	{ return m_data[idx]; }
	
	T * operator[] (int idx)
	{ return m_data[idx]; }
	
	const T * operator[] (int idx) const
	{ return m_data[idx]; }
};

}

}