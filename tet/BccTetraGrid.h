/*
 *  BccTetraGrid.h
 *  
 *
 *  Created by jian zhang on 6/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TTG_BCC_TET_GRID_H
#define TTG_BCC_TET_GRID_H

#include "BccCell.h"

namespace ttg {

class BccTetraGrid : public aphid::sdb::WorldGrid<aphid::sdb::Array<int, BccNode>, BccNode > 
{
	
public:
	BccTetraGrid();
	virtual ~BccTetraGrid();
	
	void buildNodes();
	int numNodes();
	void extractNodePositions(aphid::Vector3F * dest);
	void buildTetrahedrons(std::vector<ITetrahedron *> & dest);
	void moveNodeIn(const aphid::Vector3F & cellCenter,
					const aphid::Vector3F * pos, 
					const int & n,
					aphid::Vector3F * X,
					int * prop);
	bool moveRedNodeTo(const aphid::Vector3F & cellCenter,
					const float & d,
					const aphid::Vector3F & pos,
					aphid::Vector3F * X,
					int * prop);
	void moveBlueNodes(const aphid::Vector3F & cellCenter,
					const aphid::Vector3F & redP,
					const std::vector<aphid::Vector3F> & samples,
					aphid::Vector3F * X,
					int * prop);
	bool isBlueCloseToRed(const aphid::Vector3F & p,
					const aphid::Vector3F & blueP,
					const aphid::Vector3F & redP,
					const float & r) const;				
	void moveRedNodeIn(const aphid::Vector3F & cellCenter,
					const  aphid::Vector3F & pos,
					aphid::Vector3F * X,
					int * prop);
	void smoothBlueNodeIn(const aphid::Vector3F & cellCenter,
					aphid::Vector3F * X);
					
protected:

private:
	void countNodes();
	void countNodesIn(aphid::sdb::Array<int, BccNode> * cell, int & c);
	void extractNodePositionsIn(aphid::Vector3F * dest,
						aphid::sdb::Array<int, BccNode> * cell);
	
};

}

#endif