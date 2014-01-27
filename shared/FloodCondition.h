/*
 *  FloodCondition.h
 *  aphid
 *
 *  Created by jian zhang on 11/23/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <SelectCondition.h>
class BaseTexture;
class FloodCondition : public SelectCondition {
public:	
	FloodCondition();
	void setMinDistance(float d);
	float minDistance() const;

	void setDensityMap(BaseTexture * tex);
private:
	BaseTexture * m_density;
	float m_minDistance;
};