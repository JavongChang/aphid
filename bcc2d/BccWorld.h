#ifndef BCCWORLD_H
#define BCCWORLD_H
#include <ALLMath.h>
#include <BoundingBox.h>
#include <AOrientedBox.h>
#include <HTetrahedronMesh.h>

class CurveGroup;
class KdTreeDrawer;
class BaseBuffer;
class KdCluster;
class KdIntersection;
class GeometryArray;
class APointCloud;
struct BezierSpline;
class BccMesh;
class ATriangleMesh;
class CurveReduction;
class ATetrahedronMeshGroup;
class BlockBccMeshBuilder;
class FitBccMeshBuilder;

class BccWorld {
public:
	BccWorld();
    virtual ~BccWorld();
	
	void setTiangleGeometry(GeometryArray * x);
	void addCurveGroup(CurveGroup * m);
	bool buildTetrahedronMesh();
	void addPatchBoxes(const std::vector<AOrientedBox> & src);
    
	void select(const Ray * r);
	void clearSelection();
    void reduceSelected(float x);
    void rebuildTetrahedronsMesh(float deltaNumGroups);
    const float totalCurveLength() const;
    const unsigned numCurves() const;
	const unsigned numTetrahedrons() const;
	unsigned numTriangles() const;
	const unsigned numPoints() const;
	unsigned numTetrahedronMeshes() const;
	ATetrahedronMesh * tetrahedronMesh(unsigned i);
	GeometryArray * triangleGeometries() const;
	GeometryArray * selectedGroup(unsigned & idx) const;
	float drawAnchorSize() const;
	ATetrahedronMeshGroup * combinedTetrahedronMesh();
	const std::vector<AOrientedBox> * patchBoxes() const;
    
private:
	bool createCurveCluster();
	void addCurveGeometriesToCluster(CurveGroup * data);
	bool createTriangleIntersection();
	
	void clearTetrahedronMesh();
	void reduceAllGroups();
	void reduceGroup(unsigned igroup);
	float groupCurveLength(GeometryArray * geos);
	void rebuildGroupTetrahedronMesh(unsigned igroup, GeometryArray * geos);
	
	void createTetrahedronMeshesByFitCurves();
	void createTetrahedronMeshesByBlocks();
	ATetrahedronMeshGroup * fitAGroup(GeometryArray * geos);
	
private:
	KdCluster * m_curveCluster;
	KdIntersection * m_triIntersect;
	GeometryArray * m_triangleMeshes;

	std::vector<ATetrahedronMeshGroup *> m_tetrahedonMeshes;
	std::vector<CurveGroup *> m_curveGeos;
	std::vector<AOrientedBox> m_patchBoxes;
    CurveReduction * m_reducer;
	BlockBccMeshBuilder * m_blockBuilder;
	FitBccMeshBuilder * m_fitBuilder;
	unsigned m_numCurves, m_totalNumTetrahedrons, m_totalNumPoints;
    float m_totalCurveLength, m_estimatedNumGroups;
};

#endif        //  #ifndef BCCWORLD_H

