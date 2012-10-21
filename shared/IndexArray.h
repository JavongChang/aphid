/*
 *  IndexArray.h
 *  kdtree
 *
 *  Created by jian zhang on 10/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <BaseArray.h>

class IndexArray : public BaseArray {
public:
	IndexArray();
	virtual ~IndexArray();
	
	unsigned *asIndex(unsigned index);
	unsigned *asIndex();
};