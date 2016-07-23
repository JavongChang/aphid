/*
 *  AdaptiveBccField.h
 *  foo
 *
 *  Created by jian zhang on 7/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "AdaptiveBccMesher.h"
#include <ADistanceField.h>
#include <ConvexShape.h>

namespace ttg {

class AdaptiveBccField : public AdaptiveBccMesher, public aphid::ADistanceField {

public:
	AdaptiveBccField();
	virtual ~AdaptiveBccField();
	
/// push tetra node and edge to graph
	void buildGraph();
	
	template<typename Tf>
	void calculateDistance(const Tf * func, const float & shellThickness)
	{
		clearDirtyEdges();
		
		typename aphid::cvx::Tetrahedron;
		aphid::cvx::Tetrahedron tetshp;

		const int nt = numTetrahedrons();
		int i = 0;
		for(;i<nt;++i) {
			
			getTetraShape(tetshp, i);

/// intersect any tetra			
			if(func-> template intersect <aphid::cvx::Tetrahedron>(&tetshp) ) {
				markTetraOnFront(i);
			}
		}
		
		calculateDistanceOnFront(func, shellThickness);
		fastMarchingMethod();
		markInsideOutside();
	}
	
	void buildRefinedMesh();
	bool checkTetraVolume();
	void verbose();
	
	void getTetraShape(aphid::cvx::Tetrahedron & b, const int & i) const;
	void getTriangleShape(aphid::cvx::Triangle & t, const int & i) const;
	
protected:
	void markTetraOnFront(const int & i);
	
private:
	void pushIndices(const std::vector<int> & a,
					std::vector<int> & b);
	
};

}