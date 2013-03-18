#ifndef MESHLAPLACIAN_H
#define MESHLAPLACIAN_H

#include "TriangleMesh.h"
#include <vector>
class VertexAdjacency;
class Matrix33F;
class Facet;
class MeshLaplacian : public TriangleMesh {
public:
    MeshLaplacian();
    MeshLaplacian(const char * filename);
    virtual ~MeshLaplacian();
	
	char buildTopology();
	
	VertexAdjacency * connectivity();
	Matrix33F getTangentFrame(const unsigned &idx) const;
	Facet * getFace(const unsigned &idx) const;
private:
    VertexAdjacency * m_adjacency;
	std::vector<Facet *> m_faces;
};
#endif        //  #ifndef MESHLAPLACIAN_H

