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

	aphid::sdb::Sequence<int > m_frontNodes;
	aphid::Vector3F m_insideOutsidePref;
	float m_errorThreshold;
	
public:
	AdaptiveBccField();
	virtual ~AdaptiveBccField();
	
/// subdive grid until either max level and max error condition is met
/// levelLimit < adaptive grid maxLevel
/// threshold is min error distance triggers next level
	template<typename Tf>
	void adaptiveBuild(Tf * distanceFunc,
				const int & levelLimit, 
				const float & threshold)
	{
		m_errorThreshold = threshold;
		
		buildGrid();
		buildMesh();
		buildGraph();
		
		calculateDistance<Tf>(distanceFunc, 0.f);
		obtainGridNodeVal<AdaptiveBccGrid3, BccNode3 >(nodes(), grid() );
		updateMinMaxError();
		
		verbose();
		
		float curErr = maxError();
		
		int level = 0;
		while(level < levelLimit && curErr > threshold) {
			
			std::cout<<"\n subdiv level"<<level<<std::endl;
			subdivideGridByError(threshold, level);
			
			std::cout<<"\n build grid level"<<level<<std::endl;
			buildGrid();
			buildMesh();
			buildGraph();
			
			calculateDistance<Tf>(distanceFunc, 0.f);
			
			if(level+1 < levelLimit) {
				obtainGridNodeVal<AdaptiveBccGrid3, BccNode3 >(nodes(), grid() );
				updateMinMaxError();
			}
			verbose();
		
			level++;
			curErr = maxError();
			
		}
		// printErrEdges(threshold);
		std::cout<<"\n build to level "<<level<<"\n";
		std::cout.flush();
	}

/// grid is very coarse relative to input mesh, error will be large
/// first subdivide to level ignoring error	
/// subdivide front to find negative distance
	template<typename Tf>
	void frontAdaptiveBuild(Tf * distanceFunc, 
							int discreteLevel,
							int maxLevel,
							const float & thre)
	{
		int curLevel = discreteLevel, nbound;
		discretize<Tf>(distanceFunc, curLevel);
		
		buildGrid();
		buildMesh();
		buildGraph();
		
		float thickness = grid()->levelCellSize(curLevel) * thre;
		distanceFunc->setShellThickness(thickness );
		calculateDistance2<Tf>(distanceFunc);
		verbose();
		
		while(curLevel < maxLevel) {
			obtainGridNodeVal<AdaptiveBccGrid3, BccNode3 >(nodes(), grid() );
		
			std::cout<<"\n subdiv level"<<curLevel<<std::endl;
		
			subdivideByFront(curLevel);
			nbound = m_frontNodes.size();
			std::cout<<"\n n boundary nodes "<<nbound;
			
			buildGrid();
			buildMesh();
			buildGraph();
			
			thickness = grid()->levelCellSize(curLevel+1) * thre;
			distanceFunc->setShellThickness(thickness);
			moveFront(thickness);
			calculateDistance2<Tf>(distanceFunc);
		
			std::cout<<"\n subdiv to level"<<curLevel;
			verbose();
			
			curLevel++;
		}
		
	}
	
	void buildGrid();
	
/// push tetra node and edge to graph
	void buildGraph();
	
	template<typename Tf>
	void calculateDistance(Tf * func, const float & shellThickness)
	{
		clearDirtyEdges();
		markUnknownNodes();
		
		typename aphid::cvx::Tetrahedron;
		aphid::cvx::Tetrahedron tetshp;

		const int nt = numTetrahedrons();
		int i = 0;
		for(;i<nt;++i) {
			
			getTetraShape(tetshp, i);

/// intersect any tetra			
			if(func-> template broadphase <aphid::cvx::Tetrahedron>(&tetshp ) ) {
				markTetraOnFront(i);
			}
		}
		
		messureFrontNodes(func, shellThickness);
		fastMarchingMethod();
		int ifar = findFarInd();
		markInsideOutside2(func, ifar, true);
		estimateFrontEdgeError(func);
		
	}
	
	template<typename Tf>
	void calculateDistance2(Tf * func)
	{
		clearDirtyEdges();
		markUnknownNodes();
		
		typename aphid::cvx::Tetrahedron;
		aphid::cvx::Tetrahedron tetshp;

		const int nt = numTetrahedrons();
		int i = 0;
		for(;i<nt;++i) {
			
			getTetraShape(tetshp, i);

/// intersect any tetra			
			if(func-> template broadphase <aphid::cvx::Tetrahedron>(&tetshp ) ) {
				markTetraOnFront(i);
			}
		}
		
		messureFrontNodes2(func);
		//int ifar = findFarInd();
		//markInsideOutside2(func, ifar, false);
		fastMarchingMethod();
		//estimateFrontEdgeError(func);
		
	}
	
	void verbose();
	
	void getTetraShape(aphid::cvx::Tetrahedron & b, const int & i) const;
	void setInsideOutsidePref(const aphid::Vector3F & q);
	const float & errorThreshold() const;
								
protected:
	void markTetraOnFront(const int & i);
	
private:
	void pushIndices(const std::vector<int> & a,
					std::vector<int> & b);
	void subdivideGridByError(const float & threshold,
						const int & level);
	void subdivideByFront(int level);
/// background node ind in cell closest to pref 
	int findFarInd();
	bool isTetraOnFront(int iv0, int iv1, int iv2, int iv3) const;
	bool isTetraOnFrontBoundary(int iv0, int iv1, int iv2, int iv3) const;
	bool isTetraAllPositive(int iv0, int iv1, int iv2, int iv3) const;
	void moveFront(const float & x);
	
};

}