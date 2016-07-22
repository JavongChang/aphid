/*
 *  BccCell3.h
 *  foo
 *
 *  Created by jian zhang on 7/22/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <Array.h>
#include <AdaptiveGrid3.h>
#include "BccCell.h"

namespace ttg {

class BccCell3 : public aphid::sdb::Array<int, BccNode > {

	bool m_hasChild;
	BccCell3 * m_parentCell;
	int m_childI;
	
public:
	BccCell3(Entity * parent = NULL);
	virtual ~BccCell3();
	
	void setHasChild();
	void setParentCell(BccCell3 * x, const int & i);
	void insertRed(const aphid::Vector3F & pref);
	void insertBlue(const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	void insertYellow(const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	void insertCyan(const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	BccNode * findBlue(const aphid::Vector3F & pref);
	void insertFaceOnBoundary(const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	
	const bool & hasChild() const;
	
	BccNode * blueNode(const int & i,
					const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	BccNode * yellowNode(const int & i,
					const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	BccNode * cyanNode(const int & i,
					const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
private:
	BccNode * findBlueNodeInNeighbor(const int & i,
					const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	BccNode * findCyanNodeInNeighbor(const int & i,
					const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
	BccNode * derivedBlueNode(const int & i,
					const aphid::sdb::Coord4 & cellCoord,
					aphid::sdb::AdaptiveGrid3<BccCell3, BccNode > * grid);
															
};

}