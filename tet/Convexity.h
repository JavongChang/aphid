/*
 *  Convexity.h
 *  
 *
 *  Created by jian zhang on 7/8/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include <Plane.h>
namespace ttg {

class Convexity {

public:

	Convexity();
	
	static bool CheckDistanceFourPoints(const aphid::Vector3F & a,
						const aphid::Vector3F & b,
						const aphid::Vector3F & c,
						const aphid::Vector3F & d,
						const aphid::Vector3F & p0,
						const float & r);
						
	static bool CheckDistanceTwoPlanes(const aphid::Vector3F & a,
						const aphid::Vector3F & b,
						const aphid::Vector3F & c,
						const aphid::Vector3F & d,
						const aphid::Vector3F & p0,
						const float & r);
						
	static bool CheckTetraVolume(const aphid::Vector3F & a,
						const aphid::Vector3F & b,
						const aphid::Vector3F & c,
						const aphid::Vector3F & d);
	
};

}