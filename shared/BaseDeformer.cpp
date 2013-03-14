#include "BaseDeformer.h"
#include "BaseMesh.h"
BaseDeformer::BaseDeformer() : m_deformedV(0) {}
BaseDeformer::~BaseDeformer() 
{
	if(m_deformedV) delete[] m_deformedV;
}

void BaseDeformer::setMesh(BaseMesh * mesh)
{
	m_mesh = mesh;
	m_numVertices = mesh->getNumVertices();
	m_deformedV = new Vector3F[m_numVertices];
	Vector3F *v = mesh->getVertices();
	for(int i = 0; i < (int)m_numVertices; i++) {
		m_deformedV[i] = v[i];
	}
}

char BaseDeformer::solve()
{
	return 1;
}

Vector3F * BaseDeformer::getDeformedData() const
{
	return m_deformedV;
}
