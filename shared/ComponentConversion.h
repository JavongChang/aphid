/*
 *  ComponentConversion.h
 *  knitfabric
 *
 *  Created by jian zhang on 6/3/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllGeometry.h>
#include <vector>
class MeshTopology;

class ComponentConversion {
public:
	ComponentConversion();
	
	void setTopology(MeshTopology * topo);
	
	void facetToPolygon(const std::vector<unsigned> & src, std::vector<unsigned> & polyIds) const;
	void vertexToEdge(const std::vector<unsigned> & src, std::vector<unsigned> & dst) const;
	void vertexToPolygon(const std::vector<unsigned> & src, std::vector<unsigned> & polyIds, std::vector<unsigned> & vppIds) const;
private:
	char appendUnique(unsigned idx, std::vector<unsigned> & dst) const;
	MeshTopology * m_topology;
};