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

	aphid::Vector3F m_insideOutsidePref;
	float m_errorThreshold;
	
public:
	AdaptiveBccField();
	virtual ~AdaptiveBccField();
	
/// subdive grid until either max level and max error condition is met
/// levelLimit < adaptive grid maxLevel
/// errorThreshild is min error distance triggers next level
	template<typename Tf>
	void AdaptiveBccField::build(Tf * distanceFunc,
				const int & levelLimit, 
				const float & errorThreshild)
	{
		m_errorThreshold = errorThreshild;
		
		buildGrid();
		buildMesh();
		buildGraph();
		
		calculateDistance<Tf>(distanceFunc, 0.f);
		estimateError<Tf>(distanceFunc, 0.f);
		
		verbose();
		
		float curErr = maxError();
		
		int level = 0;
		while(level < levelLimit && curErr > errorThreshild) {
		
			subdivideGridByError(errorThreshild, level);
			
			buildGrid();
			buildMesh();
			buildGraph();
			
			calculateDistance<Tf>(distanceFunc, 0.f);
			estimateError<Tf>(distanceFunc, 0.f);
			
			verbose();
		
			level++;
			curErr = maxError();
		}
		
		std::cout<<"\n build to level "<<level;
	}
	
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
		
		markInsideOutside(findFarInd() );
	}
	
	void verbose();
	
	void getTetraShape(aphid::cvx::Tetrahedron & b, const int & i) const;
	void setInsideOutsidePref(const aphid::Vector3F & q);
								
protected:
	void markTetraOnFront(const int & i);
	float seperateDistance() const;
	
private:
	void pushIndices(const std::vector<int> & a,
					std::vector<int> & b);
	void subdivideGridByError(const float & threshold,
						const int & level);
/// background node ind in cell closest to pref 
	int findFarInd();
	
};

}