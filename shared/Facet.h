/*
 *  Facet.h
 *  convexHull
 *
 *  Created by jian zhang on 9/5/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <vector>
#include <Vertex.h>
#include <Edge.h>
#include <GeoElement.h>
class Facet : public GeoElement {
public:
	Facet();
	Facet(Vertex *a, Vertex *b, Vertex *c, Vertex *d);
	virtual ~Facet();
	
	void connectTo(Facet *another, Vertex *a, Vertex *b);
	Edge * matchedEdge(Vertex * a, Vertex * b);
	
	Vertex * vertex(int idx);
	Vertex * thirdVertex(Vertex *a, Vertex *b);
	Vertex getVertex(int idx) const;
	Vector3F getCentroid() const;
	Vector3F getNormal() const;
	char isVertexAbove(const Vertex & v) const;
	
	void getEdgeOnHorizon(std::vector<Edge *> & horizons) const;
private:
	void createEdges();
	
	Vertex *m_vertices[3];
	Edge *m_edges[3];
	Vector3F m_normal;
};