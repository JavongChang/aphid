#pragma once

#include <Vector3F.h>
class BaseMesh;
class BaseDeformer
{
public:
    
    BaseDeformer();
    virtual ~BaseDeformer();
	
	virtual void setMesh(BaseMesh * mesh);
	
	void reset();
	
	Vector3F * getDeformedData() const;
	
	Vector3F restP(unsigned idx) const;
    
	virtual char solve();

    unsigned m_numVertices;
	
	Vector3F * m_deformedV;
	
	BaseMesh * m_mesh;
private:
    
};
