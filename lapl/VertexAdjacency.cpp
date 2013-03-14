/*
 *  VertexAdjacency.cpp
 *  lapl
 *
 *  Created by jian zhang on 3/9/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "VertexAdjacency.h"
#include "Edge.h"
#include "Facet.h"
#include "Matrix44F.h"

#include <cmath>
VertexAdjacency::VertexAdjacency() {}
VertexAdjacency::~VertexAdjacency() 
{
	m_edges.clear();
}

void VertexAdjacency::addEdge(Edge * e)
{
	m_edges.push_back(e);
}

char VertexAdjacency::findOneRingNeighbors()
{
	int nneib = 0;
	Edge outgoing;
	firstOutgoingEdge(outgoing);
	
	addNeighbor(outgoing);
	
	m_idxInOrder[outgoing.v1()->getIndex()] = nneib;
	nneib++;
	Edge incoming;
	if(!findIncomming(outgoing, incoming)) {
		printf("cannot find incoming edge ");
		return 0;
	}
	//printf("e %i-%i ", incoming.v0()->getIndex(), incoming.v1()->getIndex());
	
	if(!findOppositeEdge(incoming, outgoing)) {
		printf("cannot find outgoing edge ");
		return 0;
	}
	for(int i = 1; i < (int)m_edges.size() / 2; i++) {
		addNeighbor(outgoing);
		m_idxInOrder[outgoing.v1()->getIndex()] = nneib;
		nneib++;
		findIncomming(outgoing, incoming);
		findOppositeEdge(incoming, outgoing);
	}
	return 1;
}

void VertexAdjacency::computeWeights()
{
	Vector3F vij, vij0, vij1;
	float dist, theta0, theta1, wij;

	const unsigned numNeighbors = m_neighbors.size();
	for(unsigned i = 0; i < numNeighbors; i++) {
		getVijs(i, vij, vij0, vij1);
		
		dist = vij.length();
		vij.normalize();
		vij0.normalize();
		vij1.normalize();
		
		theta0 = acos(vij.dot(vij0));
		theta1 = acos(vij.dot(vij1));
		
		wij = (tan(theta0 * 0.5f) + tan(theta1 * 0.5f))/dist;
		m_neighbors[i]->weight = wij;
	}
	
	float sum = 0;
	for(unsigned i = 0; i < numNeighbors; i++) {
		sum += m_neighbors[i]->weight;
	}
	
	for(unsigned i = 0; i < numNeighbors; i++) {
		m_neighbors[i]->weight /= sum;
	}
	
	m_mvcoord = Vector3F(0.f, 0.f, 0.f);
	
	for(unsigned i = 0; i < numNeighbors; i++) {
		m_mvcoord += *(m_neighbors[i]->v) * m_neighbors[i]->weight;
	}
	
	m_mvcoord -= *this;
}

void VertexAdjacency::computeTangentFrame()
{
    const unsigned numNeighbors = m_neighbors.size();
	
    m_normal = Vector3F(0.f, 0.f, 0.f);
    for(unsigned i = 0; i < numNeighbors; i++) {
		Facet *f = m_neighbors[i]->f;
		m_normal += f->getNormal() * f->getArea();
	}
	m_normal.normalize();
	
	const Vector3F x1 = *(m_neighbors[0]->v) - *this;
	
	Vector3F binormal = x1.cross(m_normal);
	binormal.normalize();
	
	Vector3F tangent = m_normal.cross(binormal);
	tangent.normalize();
	
	m_tangentFrame.fill(tangent, binormal, m_normal);
}

void VertexAdjacency::computeDiscreteForms()
{
    float delta;
    Vector3F x_hat_ij, vij0, x_hat_ij1;
    Vector3F x_bar_ij, x_bar_ij1;
    const unsigned numNeighbors = m_neighbors.size();
    m_g1.resize(numNeighbors);
    m_g2.resize(numNeighbors);
    m_g3.resize(numNeighbors);
    m_L.resize(numNeighbors);
    m_O.resize(numNeighbors);
    m_x_bar.resize(numNeighbors);
    
	for(unsigned i = 0; i < numNeighbors; i++) {
		getVijs(i, x_hat_ij, vij0, x_hat_ij1);
		x_bar_ij = m_normal.cross(x_hat_ij.cross(m_normal));
		x_bar_ij1 = m_normal.cross(x_hat_ij1.cross(m_normal));
		
		delta = Matrix44F::Determinant33(x_bar_ij.x, x_bar_ij.y, x_bar_ij.z, x_bar_ij1.x, x_bar_ij1.y, x_bar_ij1.z, m_normal.x, m_normal.y, m_normal.z);
		
		m_g1[i] = x_bar_ij.dot(x_bar_ij);
		m_g2[i] = x_bar_ij.dot(x_bar_ij1);
		m_g3[i] = x_bar_ij1.dot(x_bar_ij1);
		m_L[i] = x_hat_ij.dot(m_normal);
		//m_O[i] = sign(delta);
		m_x_bar[i] = x_bar_ij;
	}
}

char VertexAdjacency::findOppositeEdge(Edge & e, Edge &dest) const
{
	std::vector<Edge *>::const_iterator it;
	for(it = m_edges.begin(); it < m_edges.end(); it++) {
		//printf("e %i-%i ", (*it)->v0()->getIndex(), (*it)->v1()->getIndex());
		if((*it)->isOppositeOf(&e)) {
			dest = *(*it);
			return 1;
		}
	}
	return 0;
}

char VertexAdjacency::firstOutgoingEdge(Edge & e) const
{
	std::vector<Edge *>::const_iterator it;
	for(it = m_edges.begin(); it < m_edges.end(); it++) {
		if((*it)->v0()->getIndex() == getIndex()) {
			e = *(*it);
			return 1;
		}
	}
	return 0;
}

char VertexAdjacency::findIncomming(Edge & eout, Edge & ein) const
{
	const int faceId = ((Facet *)eout.getFace())->getIndex();
	std::vector<Edge *>::const_iterator it;
	for(it = m_edges.begin(); it < m_edges.end(); it++) {
		//printf("ee %i-%i ", (*it)->v0()->getIndex(), (*it)->v1()->getIndex());
	
		if((*it)->v1()->getIndex() == getIndex() && ((Facet *)(*it)->getFace())->getIndex() == faceId) {
			ein = *(*it);
			
			return 1;
		}
	}
	return 0;
}

std::map<int,int> VertexAdjacency::getNeighborOrder() const
{
	return m_idxInOrder;
}

unsigned VertexAdjacency::getNumNeighbors() const
{
    return (unsigned)m_neighbors.size();
}

void VertexAdjacency::getNeighbor(const int & idx, int & vertexIdx, float & weight) const
{
	vertexIdx = m_neighbors[idx]->v->getIndex();
	weight = m_neighbors[idx]->weight;
}

float VertexAdjacency::getDeltaCoordX() const
{
	return m_mvcoord.x;
}
	
float VertexAdjacency::getDeltaCoordY() const
{
	return m_mvcoord.y;
}
	
float VertexAdjacency::getDeltaCoordZ() const
{
	return m_mvcoord.z;
}

void VertexAdjacency::addNeighbor(Edge &outgoing)
{
	VertexNeighbor *aneighbor = new VertexNeighbor;
	aneighbor->v = outgoing.v1();
	aneighbor->e = &outgoing;
	aneighbor->f = (Facet *)outgoing.getFace();
	aneighbor->weight = 1.f;
	
	m_neighbors.push_back(aneighbor);
}

void VertexAdjacency::getVijs(const int & idx, Vector3F &vij, Vector3F &vij0, Vector3F &vij1) const
{
    const int numNeighbors = (int)m_neighbors.size();
    vij = *(m_neighbors[idx]->v) - *this;
    if(idx == 0)
        vij0 = *(m_neighbors[numNeighbors - 1]->v) - *this;
    else
        vij0 = *(m_neighbors[idx - 1]->v) - *this;
    if(idx == numNeighbors - 1)
        vij1 = *(m_neighbors[0]->v) - *this;
    else
        vij1 = *(m_neighbors[idx + 1]->v) - *this;
}

Matrix33F VertexAdjacency::getTangentFrame() const
{
    return m_tangentFrame;
}

void VertexAdjacency::verbose() const
{
	printf("\nv %i\n adjacent edge count: %i\n", getIndex(), (int)m_edges.size());
	std::vector<Edge *>::const_iterator eit;
	for(eit = m_edges.begin(); eit < m_edges.end(); eit++) {
		printf(" %d - %d", (*eit)->v0()->getIndex(), (*eit)->v1()->getIndex());
	}
	std::vector<VertexNeighbor *>::const_iterator it;
	for(it = m_neighbors.begin(); it < m_neighbors.end(); it++) {
		printf(" %i ", (*it)->v->getIndex());
	}
	printf("\n delta-coordinate %f %f %f \n", m_mvcoord.x, m_mvcoord.y, m_mvcoord.z);
}