/*
 *  HOocArray.h
 *  julia
 *
 *  Created by jian zhang on 1/3/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 *  out-of-core 2d array
 */

#pragma once
#include "H2dDataset.h"

/// BufSize is max num columns in-core

template <int DataRank, int NRows, int BufSize>
class HOocArray : public H2dDataset<DataRank, NRows> {
	
/// in-core buffer
	char * m_data;
/// num char per column
	int m_nbits;
/// in-core buffer usage
	int m_incoreSize;
/// total space usage
	int m_size;
	hid_t m_parentId;
	
public:
	HOocArray(const std::string & name) : H2dDataset<DataRank, NRows>(name)
	{
		m_nbits = H2dDataset<DataRank, NRows>::NumBitsPerCol();
		m_data = new char[m_nbits * BufSize];
		m_incoreSize = 0;
		m_size = 0;
	}
	
	virtual ~HOocArray() 
	{
		delete m_data;
	}
	
	bool createStorage(hid_t parentId);
	bool openStorage(hid_t parentId);
	
	void insert(char * d);
	void finishInsert();
	
	int size() const
	{ return m_size; }
	
	void read(char * d, int offset, int ncols);
	void readIncore(int offset, int ncols);
	
	char * incoreBuf() const;
	
	void printValues();
	
protected:

private:
	void writeCurrentBuf(int ncols);
	void printAValue(char * v);
};

template <int DataRank, int NRows, int BufSize>
bool HOocArray<DataRank, NRows, BufSize>::createStorage(hid_t parentId)
{
	if(H2dDataset<DataRank, NRows>::create(parentId) ) {
		H2dDataset<DataRank, NRows>::close();
		m_parentId = parentId;
	}
	else
		m_parentId = 0;
	return m_parentId > 0;
}

template <int DataRank, int NRows, int BufSize>
bool HOocArray<DataRank, NRows, BufSize>::openStorage(hid_t parentId)
{
	if(H2dDataset<DataRank, NRows>::open(parentId) ) {
		m_parentId = parentId;
	}
	else
		m_parentId = 0;
	return m_parentId > 0;
}

template <int DataRank, int NRows, int BufSize>
void HOocArray<DataRank, NRows, BufSize>::insert(char * d) 
{
	memcpy( &m_data[m_nbits * m_incoreSize], d, m_nbits );
	m_incoreSize++;
	m_size++;
	if(m_incoreSize == BufSize) {
		writeCurrentBuf(m_incoreSize);
		m_incoreSize = 0;
	}
}

template <int DataRank, int NRows, int BufSize>
void HOocArray<DataRank, NRows, BufSize>::finishInsert()
{
	if(m_incoreSize == 0) return;
	
	writeCurrentBuf(m_incoreSize);
	m_incoreSize = 0;
}

template <int DataRank, int NRows, int BufSize>
void HOocArray<DataRank, NRows, BufSize>::writeCurrentBuf(int ncols)
{
	hdata::Select2DPart part;
	part.start[0] = m_size - ncols;
	part.start[1] = 0;
	part.count[0] = ncols;
	part.count[1] = NRows;
	
	H2dDataset<DataRank, NRows>::open(m_parentId);
	H2dDataset<DataRank, NRows>::write(m_data, &part);
	H2dDataset<DataRank, NRows>::close();
}

template <int DataRank, int NRows, int BufSize>
void HOocArray<DataRank, NRows, BufSize>::read(char * d, int offset, int ncols)
{
	hdata::Select2DPart part;
	part.start[0] = offset;
	part.start[1] = 0;
	part.count[0] = ncols;
	part.count[1] = NRows;
	
	H2dDataset<DataRank, NRows>::open(m_parentId);
	H2dDataset<DataRank, NRows>::read(d, &part);
	H2dDataset<DataRank, NRows>::close();
}

template <int DataRank, int NRows, int BufSize>
void HOocArray<DataRank, NRows, BufSize>::readIncore(int offset, int ncols)
{ read(m_data, offset, ncols); }

template <int DataRank, int NRows, int BufSize>
char * HOocArray<DataRank, NRows, BufSize>::incoreBuf() const
{ return m_data; }

template <int DataRank, int NRows, int BufSize>
void HOocArray<DataRank, NRows, BufSize>::printValues()
{
	std::cout<<"\n size "<<m_size;
	int numBlks = m_size / BufSize;
	if((m_size & (BufSize-1)) > 0) numBlks++;
	int ncols = BufSize;
	const int bpp = H2dDataset<DataRank, NRows>::NumBitsPerPnt();
	int i, j, k, it = 0;
	for(i=0;i<numBlks;++i) {
		if((m_size - i*BufSize) < BufSize) ncols = m_size - i*BufSize;
		std::cout<<"\n read n col "<<ncols;
		readIncore(i*BufSize, ncols);
		for(j=0;j<ncols;++j) {
			for(k=0;k<NRows;++k) {
				std::cout<<"\n ["<<it<<"]";
				printAValue(&m_data[(j*NRows+k) * bpp]);
				it++;
			}
		}
	}
}

template <int DataRank, int NRows, int BufSize>
void HOocArray<DataRank, NRows, BufSize>::printAValue(char * v)
{
	switch (DataRank) {
		case hdata::TChar:
			std::cout<<" "<<*v;
			break;
		case hdata::TShort:
			std::cout<<" "<<*(short *)v;
			break;
		case hdata::TInt:
			std::cout<<" "<<*(int *)v;
			break;
		case hdata::TLongInt:
			std::cout<<" "<<*(long long *)v;
			break;
		case hdata::TFloat:
			std::cout<<" "<<*(float *)v;
			break;
		case hdata::TDouble:
			std::cout<<" "<<*(double *)v;
			break;
		default:
			break;
	}
}
