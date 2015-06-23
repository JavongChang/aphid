#include "BccWorld.h"
#include <BezierCurve.h>
#include <KdTreeDrawer.h>
#include "BccGrid.h"
#include <CurveGroup.h>
#include "bcc_common.h"
#include <line_math.h>
#include <HesperisFile.h>
#include <KdCluster.h>
#include <KdIntersection.h>
#include <GeometryArray.h>
#include <CurveBuilder.h>
#include <BezierCurve.h>
#include <RandomCurve.h>
#include <bezierPatch.h>
#include <APointCloud.h>
#include <BccMesh.h>
#include <FitBccMesh.h>
#include <ATriangleMesh.h>
#include "FitBccMeshBuilder.h"

BccWorld::BccWorld(KdTreeDrawer * drawer)
{
    m_numCurves = 0;
    m_totalCurveLength = 0.f;
    m_estimatedNumGroups = 2500.f;
	m_drawer = drawer;
	m_curves = new CurveGroup;
	if(!createCurveGeometryFromFile())
#if WORLD_TEST_SINGLE
		createTestCurveGeometry();
#else
		createRandomCurveGeometry();
#endif
	
	createCurveStartP();
	createAnchorIntersect();
	
	std::cout<<" creating kd tree\n";
	m_cluster = new KdCluster;
	m_cluster->addGeometry(m_allGeo);
	
	KdTree::MaxBuildLevel = 16;
	KdTree::NumPrimitivesInLeafThreashold = 31;
	
	m_cluster->create();
	
	createTetrahedronMeshes();
	
	createTriangleMeshesFromFile();
	
	std::cout<<" done initialize.\n";
}

BccWorld::~BccWorld() 
{
	delete m_curves;
	delete m_cluster;
	delete m_anchorIntersect;
	delete m_allGeo;
	delete m_curveStartP;
	delete[] m_meshes;
}

bool BccWorld::createCurveGeometryFromFile()
{
	if(!readCurveDataFromFile()) return false;
	
	m_allGeo = new GeometryArray;
	m_allGeo->create(m_curves->numCurves());
	
	const unsigned n = m_curves->numCurves();
	m_allGeo = new GeometryArray;
	m_allGeo->create(n);
	
	unsigned * cc = m_curves->counts();
	Vector3F * cvs = m_curves->points();
	
	m_totalCurveLength = 0.f;
	CurveBuilder cb;
	
	unsigned ncv;
	unsigned cvDrift = 0;
	
	unsigned i, j;
	for(i=0; i< n; i++) {
		
		ncv = cc[i];
		
		for(j=0; j < ncv; j++)			
			cb.addVertex(cvs[j + cvDrift]);
		
		BezierCurve * c = new BezierCurve;
		cb.finishBuild(c);
		
		m_totalCurveLength += c->length();
		
		m_allGeo->setGeometry(c, i);
		
		cvDrift += ncv;
	}
    std::cout<<" n curves "<<n
	    <<" total curve length: "<<m_totalCurveLength<<"\n";
    m_numCurves = n;
	return true;
}

void BccWorld::createTestCurveGeometry()
{
	std::cout<<" gen 1 test curve";
	m_curves->create(1, 9);
	m_curves->counts()[0] = 9;
	
	Vector3F * cvs = m_curves->points();
	cvs[0].set(8.f + RandomFn11(), 1.f + RandomFn11(), 4.1f);
    cvs[1].set(2.f + RandomFn11(), 9.4f + RandomFn11(), 1.11f);
    cvs[2].set(14.f + RandomFn11(), 8.4f + RandomFn11(), -3.13f);
    cvs[3].set(12.f + RandomFn11(), 1.4f + RandomFn11(), 1.14f);
    cvs[4].set(19.f + RandomFn11(), 2.4f + RandomFn11(), 2.16f);
    cvs[5].set(20.f + RandomFn11(), 3.4f + RandomFn11(), 5.17f);
    cvs[6].set(18.f + RandomFn11(), 12.2f + RandomFn11(), 3.18f);
    cvs[7].set(12.f + RandomFn11(), 12.2f + RandomFn11(), 2.19f);
    cvs[8].set(13.f + RandomFn11(), 8.2f + RandomFn11(), -2.18f);
    
    for(unsigned i=0; i<9;i++) {
        cvs[i] -= Vector3F(12.f, 0.f, 0.f);
        cvs[i] *= 3.f;
    }
	
	m_allGeo = new GeometryArray;
	m_allGeo->create(1);
	
	CurveBuilder cb;
	
	unsigned i;
	for(i=0; i< 9; i++)
		cb.addVertex(cvs[i]);
	
	BezierCurve * c = new BezierCurve;
	cb.finishBuild(c);
	m_allGeo->setGeometry(c, 0);
}

void BccWorld::createRandomCurveGeometry()
{
	const unsigned n = 15 * 15;
	m_allGeo = new GeometryArray;
	m_allGeo->create(n);
	
	BezierPatch bp;
	bp.resetCvs();
	
	int i=0;
	bp._contorlPoints[0].y += -.2f;
	bp._contorlPoints[1].y += -.4f;
	bp._contorlPoints[2].y += -.4f;
	bp._contorlPoints[3].y += -.5f;
	
	bp._contorlPoints[4].y += -.5f;
	bp._contorlPoints[5].y += .1f;
	bp._contorlPoints[6].y += .5f;
	bp._contorlPoints[7].y += .1f;
	
	bp._contorlPoints[9].y += .5f;
	bp._contorlPoints[10].y += .5f;
	
	bp._contorlPoints[13].y += -.4f;
	bp._contorlPoints[14].y += -.85f;
	bp._contorlPoints[15].y += -.21f;
	
	i=0;
	for(;i<16;i++) {
		bp._contorlPoints[i] *= 80.f;
		bp._contorlPoints[i].y += 10.f;
		bp._contorlPoints[i].z -= 10.f;
	}
	
	RandomCurve rc;
	rc.create(m_allGeo, 15, 15,
				&bp,
				Vector3F(-.15f, 1.f, 0.33f), 
				11, 21,
				.9f);
}

void BccWorld::createCurveStartP()
{
	const unsigned n = m_allGeo->numGeometries();
	m_curveStartP = new APointCloud;
	m_curveStartP->create(n);
	Vector3F * p = m_curveStartP->points();
	
	unsigned i=0;
	for(;i<n;i++) p[i] = ((BezierCurve *)m_allGeo->geometry(i))->m_cvs[0];	
}

void BccWorld::createAnchorIntersect()
{
	m_anchorIntersect = new KdIntersection;
	m_anchorIntersect->addGeometry(m_curveStartP);
	KdTree::MaxBuildLevel = 32;
	KdTree::NumPrimitivesInLeafThreashold = 9;
	m_anchorIntersect->create();
}

void BccWorld::createTetrahedronMeshes()
{
    if(totalCurveLength()<1.f) {
        std::cout<<" invalid total curve length "<<totalCurveLength()<<" !\n";
        return;
    }
#if WORLD_USE_FIT
    FitBccMeshBuilder::EstimatedGroupSize = totalCurveLength() / m_estimatedNumGroups;
    std::cout<<"\n estimate group size "<<FitBccMeshBuilder::EstimatedGroupSize;
#endif	
	unsigned n = m_cluster->numGroups();
#if WORLD_USE_FIT
	m_meshes = new FitBccMesh[n];
#else
	m_meshes = new BccMesh[n];
#endif
	
	unsigned ntet = 0;
	unsigned nvert = 0;
    float vlm;
	unsigned i=0;
	for(;i<n;i++) {
#if WORLD_USE_FIT
        m_meshes[i].create(m_cluster->group(i), m_anchorIntersect);
#else
		m_meshes[i].create(m_cluster->group(i), m_anchorIntersect, 5);
#endif
        vlm = m_meshes[i].calculateVolume();
        m_meshes[i].setVolume(vlm);
		ntet += m_meshes[i].numTetrahedrons();
		nvert += m_meshes[i].numPoints();
	}
	
	std::cout<<"\n n tetrahedron meshes "<<n
	<<"\n total n tetrahedrons "<<ntet
	<<"\n total n points "<<nvert
	<<"\n";
	m_numMeshes = n;
}

void BccWorld::createTriangleMeshesFromFile()
{
	m_triangleMeshes = new GeometryArray;
	if(!readTriangleDataFromFile()) return;
	
	std::cout<<"\n n triangle mesh: "<<m_triangleMeshes->numGeometries()
	<<"\n";
	for(unsigned i=0; i<m_triangleMeshes->numGeometries(); i++) {
		ATriangleMesh * m = (ATriangleMesh *)m_triangleMeshes->geometry(0);
		std::cout<<m->dagName()
		<<"\n n triangle: "<<m->numTriangles()
		<<"\n n points: "<<m->numPoints()
		<<"\n";
	}
	/*
	if(m_triangleMeshes->numGeometries()>0) {
		ATriangleMesh * m = (ATriangleMesh *)m_triangleMeshes->geometry(0);		
		unsigned i = 0;
		for(;i<m->numIndices();i++) std::cout<<" "<<m->indices()[i];
		for(i=0;i<m->numPoints();i++) std::cout<<" "<<m->points()[i];
	}*/
}
 
void BccWorld::draw()
{
    // BoundingBox box;
    // m_grid->getBounding(box);
    
    // glColor3f(.21f, .21f, .21f);
    // m_drawer->boundingBox(box);
    
    // m_grid->draw(m_drawer, (unsigned *)m_mesh.m_anchorBuf->data());

	drawTetrahedronMesh();
	// drawAnchor();
	drawTriangleMesh();
    
	glDisable(GL_DEPTH_TEST);
    // glColor3f(.59f, .02f, 0.f);
    // drawCurves();
	// drawCurveStars();
	
	//for(unsigned i=0; i<m_cluster->numGroups(); i++) {
	//	m_drawer->setGroupColorLight(i);
	//	m_drawer->geometry(m_cluster->group(i));
	//}
	unsigned selectedCurveGrp = m_cluster->currentGroup();
	if(selectedCurveGrp<m_cluster->numGroups()) {
		m_drawer->setGroupColorLight(selectedCurveGrp);
		m_drawer->geometry(m_cluster->group(selectedCurveGrp));
	}
}

bool BccWorld::readCurveDataFromFile()
{
	if(BaseFile::InvalidFilename(BccGlobal::FileName)) 
		return false;
	
	if(!BaseFile::FileExists(BccGlobal::FileName)) {
		BccGlobal::FileName = "unknown";
		return false;
	}
	
	HesperisFile hes;
	hes.setReadComponent(HesperisFile::RCurve);
	hes.addCurve("curves", m_curves);
	if(!hes.open(BccGlobal::FileName)) return false;
	hes.close();
	
	return true;
}

bool BccWorld::readTriangleDataFromFile()
{
	if(BaseFile::InvalidFilename(BccGlobal::FileName)) 
		return false;
	
	if(!BaseFile::FileExists(BccGlobal::FileName)) {
		BccGlobal::FileName = "unknown";
		return false;
	}
	
	HesperisFile hes;
	hes.setReadComponent(HesperisFile::RTri);
	if(!hes.open(BccGlobal::FileName)) return false;
	hes.close();
	
	hes.extractTriangleMeshes(m_triangleMeshes);
	
	return true;
}

void BccWorld::drawTetrahedronMesh()
{
	unsigned i=0;
	for(;i<m_numMeshes; i++) {
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.51f, 0.53f, 0.52f);
	drawTetrahedronMesh(m_meshes[i].numTetrahedrons(), m_meshes[i].points(),
	         m_meshes[i].indices());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(.03f, .14f, .44f);
    drawTetrahedronMesh(m_meshes[i].numTetrahedrons(), m_meshes[i].points(),
	         m_meshes[i].indices());
	}
}

void BccWorld::drawTetrahedronMesh(unsigned nt, Vector3F * points, unsigned * indices)
{
    glBegin(GL_TRIANGLES);
    unsigned i, j;
    Vector3F q;
    unsigned * tet;
    for(i=0; i< nt; i++) {
        tet = &indices[i*4];
        for(j=0; j< 12; j++) {
            q = points[ tet[ TetrahedronToTriangleVertex[j] ] ];
            glVertex3fv((GLfloat *)&q);
        }
    }
    glEnd();
}

void BccWorld::drawAnchor()
{
/*
    const float csz = m_grid->span() / 1024.f;
	m_drawer->setWired(0);
	unsigned nt = m_mesh.m_numTetrahedrons;
    Vector3F * p = (Vector3F *)m_mesh.m_pointBuf->data();
    unsigned * a = (unsigned *)m_mesh.m_anchorBuf->data();
    unsigned * t = (unsigned *)m_mesh.m_indexBuf->data();
    unsigned i, j;
    Vector3F q;
    glColor3f(.59f, .21f, 0.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for(i=0; i< nt; i++) {
        bool anchored = 1;
        unsigned * tet = &t[i*4];
        for(j=0; j<4; j++) {
            if(a[tet[j]] < 1) {
                anchored = 0;
                break;
            }
        }
        if(!anchored) continue;
        for(j=0; j< 12; j++) {
            q = p[ tet[ TetrahedronToTriangleVertex[j] ] ];
            glVertex3fv((GLfloat *)&q);
        }
    }
    glEnd();
*/
}

void BccWorld::drawTriangleMesh()
{
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.63f, 0.64f, 0.65f);
	m_drawer->geometry(m_triangleMeshes);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(.03f, .14f, .44f);
    m_drawer->geometry(m_triangleMeshes);
}

bool BccWorld::save()
{
	if(BaseFile::InvalidFilename(BccGlobal::FileName)) {
		std::cout<<" no specifc file to save\n";
		return false;
	}
	
	HesperisFile hes;
	hes.setWriteComponent(HesperisFile::WTetra);
	
	unsigned i = 0;
	for(; i < m_numMeshes; i++) {
		std::stringstream sst;
		sst<<"tetra_"<<i;
		hes.addTetrahedron(sst.str(), &m_meshes[i]);
	}
	
	if(!hes.open(BccGlobal::FileName)) return false;
	hes.setDirty();
	hes.save();
	hes.close();
	return true;
}

const float BccWorld::totalCurveLength() const
{ return m_totalCurveLength; }

const unsigned BccWorld::numCurves() const
{ return m_numCurves; }

void BccWorld::clearTetrahedronMesh()
{
    if(m_meshes) delete[] m_meshes;
}

void BccWorld::rebuildTetrahedronsMesh(float deltaNumGroups)
{
    m_estimatedNumGroups += deltaNumGroups * numCurves();
    if(m_estimatedNumGroups < 100.f) m_estimatedNumGroups = 100.f;
    
    clearTetrahedronMesh();
    createTetrahedronMeshes();
    std::cout<<" done rebuild. \n";
}

void BccWorld::selectCluster(const Ray * r)
{
	BaseCurve::RayIntersectionTolerance = totalCurveLength() / m_estimatedNumGroups * .47f;
	m_cluster->intersectRay(r);
}
//:~