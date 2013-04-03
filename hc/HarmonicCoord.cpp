#include "HarmonicCoord.h"
#include "VertexAdjacency.h"
#include "MeshLaplacian.h"
#include <Eigen/SVD>
HarmonicCoord::HarmonicCoord() {}
HarmonicCoord::~HarmonicCoord() {}

void HarmonicCoord::setMesh(BaseMesh * mesh)
{
	BaseField::setMesh(mesh);
	
	printf("init laplace deformer");
	MeshLaplacian * msh = static_cast <MeshLaplacian *>(m_mesh);
	m_topology = msh->connectivity();
	initialCondition();
}

void HarmonicCoord::precompute(std::vector<WeightHandle *> & anchors)
{
	m_anchors = anchors;
	
	m_numAnchors = 0;
	for(std::vector<WeightHandle *>::iterator it = anchors.begin(); it != anchors.end(); ++it) {
		unsigned idx;
		for(Anchor::AnchorPoint * ap = (*it)->firstPoint(idx); (*it)->hasPoint(); ap = (*it)->nextPoint(idx)) {
			m_numAnchors++;
		}
	}
	
	int neighborIdx, lastNeighbor;
	LaplaceMatrixType L(m_numVertices + numAnchorPoints(), m_numVertices);
	for(int i = 0; i < (int)m_numVertices; i++) {
		VertexAdjacency & adj = m_topology[i];
		lastNeighbor = -1;
		
		VertexAdjacency::VertexNeighbor *neighbor;
		for(neighbor = adj.firstNeighborOrderedByVertexIdx(); !adj.isLastNeighborOrderedByVertexIdx(); neighbor = adj.nextNeighborOrderedByVertexIdx()) {
			neighborIdx = neighbor->v->getIndex();
			if(neighborIdx > i && lastNeighbor < i) {
				L.insert(i, i) = 1.0f;
			}
			L.insert(i, neighborIdx) = -neighbor->weight;
			lastNeighbor = neighborIdx; 
		}
		if(lastNeighbor < i)
			L.insert(i, i) = 1.0f;
	}
	
	int irow = (int)m_numVertices;

	for(std::vector<WeightHandle *>::iterator it = anchors.begin(); it != anchors.end(); ++it) {
		unsigned idx;
		for(Anchor::AnchorPoint * ap = (*it)->firstPoint(idx); (*it)->hasPoint(); ap = (*it)->nextPoint(idx)) {
			L.insert(irow, idx) = 1.f;
			irow++;
		}
	}
	
	m_LT = L.transpose();
	LaplaceMatrixType m_M = m_LT * L;
	m_llt.compute(m_M);
}

unsigned HarmonicCoord::numAnchorPoints() const
{
	return m_numAnchors;
}

void HarmonicCoord::initialCondition()
{
	int ne = 0;
	for(int i = 0; i < (int)m_numVertices; i++) {
		VertexAdjacency & adj = m_topology[i];
		ne += adj.getNumNeighbors();
	}
	printf("\nedge count: %i\n", ne);
}

void HarmonicCoord::prestep()
{
	m_b.resize(m_numVertices + numAnchorPoints());
	m_b.setZero();
	int irow = (int)m_numVertices;

	for(std::vector<WeightHandle *>::iterator it = m_anchors.begin(); it != m_anchors.end(); ++it) {
		unsigned idx;
		for(Anchor::AnchorPoint * ap = (*it)->firstPoint(idx); (*it)->hasPoint(); ap = (*it)->nextPoint(idx)) {
			m_b(irow) = ap->w;
			irow++;
		}
	}
	
	m_b = m_LT * m_b;
}

char HarmonicCoord::solve()
{
	prestep();
	Eigen::VectorXf x = m_llt.solve(m_b);
	
	float r;
	
	for(int i = 0; i < (int)m_numVertices; i++) {
		r = x(i);
		if(r < 0.f) r = 0.f;
		if(r > 1.f) r = 1.f;
		m_value[i] = r;
	}
	plotColor();
	return 1;
}