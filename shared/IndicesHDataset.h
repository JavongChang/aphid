/*
 *  IndicesHDataset.h
 *  opium
 *
 *  Created by jian zhang on 6/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <HDataset.h>
namespace aphid {

class IndicesHDataset : public HDataset {
public:
	IndicesHDataset(const std::string & path);
	virtual ~IndicesHDataset();
	
	void setNumIndices(int num);
	int numIndices() const;
	
	virtual hid_t dataType();
};

}