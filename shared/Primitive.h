/*
 *  Primitive.h
 *  kdtree
 *
 *  Created by jian zhang on 10/16/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

class Primitive {
public:
	Primitive();

	void setType(short t);
	const short & getType() const;
private:
	short m_type;
};