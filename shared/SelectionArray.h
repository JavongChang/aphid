/*
 *  SelectionArray.h
 *  lapl
 *
 *  Created by jian zhang on 3/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <vector>
#include <PrimitiveFilter.h>
class Vector3F;
class Primitive;
class Geometry;
class VertexPath;
class ComponentConversion;
class MeshTopology;

class SelectionArray : public PrimitiveFilter {
public:
	SelectionArray();
	virtual ~SelectionArray();
	
	void reset();
	void add(Geometry * geo, unsigned icomp, const Vector3F & atP);
	
	unsigned numPrims() const;
	Primitive * getPrimitive(const unsigned & idx) const;
	
	unsigned numVertices() const;
	unsigned getVertexId(const unsigned & idx) const;
	unsigned lastVertexId() const;
	Vector3F getVertexP(const unsigned & idx) const;
	
	Geometry * getGeometry() const;
	
	unsigned numFaces() const;
	unsigned getFaceId(const unsigned & idx) const;
	
	void grow();
	void shrink();
	
	void enableVertexPath();
	void disableVertexPath();
	
	void addVertex(unsigned idx, const Vector3F & atP);
	
	void setTopology(MeshTopology * topo);
	
	void extendToEdgeRing();
	void asEdges(std::vector<unsigned> & dst) const;
	void asVertices(std::vector<unsigned> & dst) const;
	void asPolygons(std::vector<unsigned> & polyIds, std::vector<unsigned> & vppIds) const;
private:
	bool isVertexSelected(unsigned idx) const;
	bool isFaceSelected(unsigned idx) const;
	
	std::vector<Vector3F> m_vertexPs;
	std::vector<Primitive *> m_prims;
	std::vector<unsigned> m_vertexIds;
	std::vector<unsigned> m_faceIds;
	std::vector<unsigned> m_edgeIds;
	VertexPath * m_vertexPath;
	ComponentConversion * m_compconvert;
    Geometry * m_geometry;
	
	bool m_needVertexPath;
};