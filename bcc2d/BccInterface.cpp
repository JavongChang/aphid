/*
 *  BccInterface.cpp
 *  larix
 *
 *  Created by jian zhang on 7/24/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "BccInterface.h"
#include <GeometryArray.h>
#include <ATriangleMesh.h>
#include <ATetrahedronMeshGroup.h>
#include "BccWorld.h"
#include <KdTreeDrawer.h>
#include "tetrahedron_math.h"
#include <KdIntersection.h>
#include <HesperisFile.h>
#include <MeshSeparator.h>
#include <PrincipalComponents.h>
#include <BaseLog.h>
#include <boost/timer.hpp>

BccInterface::BccInterface() 
{ 
    m_cells = new BaseBuffer;
	m_patchSeparator = new MeshSeparator;
	m_patchMesh = NULL;
}

BccInterface::~BccInterface() 
{ 
    delete m_cells;
	delete m_patchSeparator;
	m_patchBoxes.clear();
}

bool BccInterface::createWorld(BccWorld * world)
{
	bool hasCrv = loadCurveGeometry(world, FileName);
	bool hasTri = loadTriangleGeometry(world, FileName);
	
	if(hasCrv && hasTri) 
		world->buildTetrahedronMesh();

	return true;
}

bool BccInterface::loadTriangleGeometry(BccWorld * world, const std::string & filename)
{
	const std::string oldFilename = FileName;
	FileName = filename;
	GeometryArray * trigeo = new GeometryArray;
	bool res = ReadTriangleData(trigeo);
	if(res) {
		const unsigned n = trigeo->numGeometries();
		std::cout<<"\n bcc interface loading "<<n<<" triangle mesh geometries. ";
		world->setTiangleGeometry(trigeo);
	}
	else {
		std::cout<<"\n hes file contains no triangle mesh geometry. ";
	}
	FileName = oldFilename;
	return res;
}

bool BccInterface::loadCurveGeometry(BccWorld * world, const std::string & filename)
{
	const std::string oldFilename = FileName;
	FileName = filename;
	GeometryArray curvegeo;
	bool res = ReadCurveData(&curvegeo);
	if(res) {
		const unsigned n = curvegeo.numGeometries();
		std::cout<<"\n bcc interface loading "<<n<<" curve geometries. ";
		unsigned i=0;
		for(;i<n;i++)
			world->addCurveGroup((CurveGroup *)curvegeo.geometry(i));
	}
	else {
		std::cout<<"\n hes file contains no curve geometry. ";
	}
	FileName = oldFilename;
	return res;
}

bool BccInterface::loadPatchGeometry(BccWorld * world, const std::string & filename)
{
	const std::string oldFilename = FileName;
	FileName = filename;
	GeometryArray * trigeo = new GeometryArray;
	bool res = ReadTriangleData(trigeo);
	if(res) {
		const unsigned n = trigeo->numGeometries();
		std::cout<<"\n bcc interface loading "<<n<<" triangle mesh geometries as patch ";
		m_patchMesh = (ATriangleMesh *)trigeo->geometry(0);
		unsigned i=0;
		for(;i<n;i++) {
			if(separate((ATriangleMesh *)trigeo->geometry(i)))
				world->addPatchBoxes(m_patchBoxes);
		}
	}
	else {
		std::cout<<"\n hes file contains no triangle mesh geometry. ";
	}
	FileName = oldFilename;
	return res;
}

void BccInterface::drawWorld(BccWorld * world, KdTreeDrawer * drawer)
{
	const float das = world->drawAnchorSize();
	const unsigned n = world->numTetrahedronMeshes();
	unsigned i;
	for(i=0;i<n;i++) {
		drawTetrahedronMesh(world->tetrahedronMesh(i), drawer);
		drawAnchors(world->tetrahedronMesh(i), drawer, das);
	}
	
	if(world->triangleGeometries()) drawGeometry(world->triangleGeometries(), drawer);
	
	unsigned igroup;
	GeometryArray * selected = world->selectedGroup(igroup);
	if(selected) {
		drawer->setGroupColorLight(igroup);
		glDisable(GL_DEPTH_TEST);
		drawer->geometry(selected);
	}
	
	if(m_patchMesh) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(.03f, .14f, .44f);
		drawer->geometry(m_patchMesh);
	}
	
	glColor3f(0.f, 0.99f, 0.f);
	glBegin(GL_LINES);
	std::vector<AOrientedBox>::const_iterator it = m_patchBoxes.begin();
	for(;it!=m_patchBoxes.end();++it)
		drawer->orientedBox(&(*it));
	glEnd();
}

void BccInterface::drawTetrahedronMesh(ATetrahedronMesh * m, KdTreeDrawer * drawer)
{
	glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.51f, 0.53f, 0.52f);
	drawer->tetrahedronMesh(m);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(.03f, .14f, .44f);
    drawer->tetrahedronMesh(m);
}

void BccInterface::drawGeometry(GeometryArray * geos, KdTreeDrawer * drawer)
{
	glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.63f, 0.64f, 0.65f);
	drawer->geometry(geos);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(.03f, .14f, .44f);
    drawer->geometry(geos);
}

void BccInterface::drawAnchors(AGenericMesh * mesh, KdTreeDrawer * drawer,
								float drawSize)
{
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	unsigned j;
    glColor3f(.59f, .21f, 0.f);
	Vector3F * p = (Vector3F *)mesh->points();
	unsigned * a = (unsigned *)mesh->anchors();
	const unsigned n = mesh->numPoints();
	for(j=0;j<n;j++) {
		if(a[j]>0) drawer->alignedDisc(p[j], drawSize);
	}
}

bool BccInterface::saveWorld(BccWorld * world)
{
	HesperisFile hes;
	hes.setWriteComponent(HesperisFile::WTetra);
	
	ATetrahedronMeshGroup * cm = world->combinedTetrahedronMesh();
	hes.addTetrahedron("tetra_c", cm);

	if(!hes.open(FileName)) return false;
	hes.setDirty();
	hes.save();
	hes.close();

	delete cm;
    
	return true;
}

bool BccInterface::separate(ATriangleMesh * mesh)
{
	std::cout<<"\n mesh n tri "<<mesh->numTriangles();
	m_patchSeparator->separate(mesh);
	const unsigned n = m_patchSeparator->numPatches();
	if(n < 2) {
		std::cout<<"\n cannot separate one continuous mesh ";
		return false;
	}
	
	std::cout<<"\n sep to n patches "<<n;
	m_patchBoxes.clear();
	PrincipalComponents pca;
	BaseBuffer pos;
	m_patchSeparator->patchBegin();
	while(!m_patchSeparator->patchEnd()) {
		const unsigned np = m_patchSeparator->getPatchCvs(&pos, mesh);
		m_patchSeparator->nextPatch();
		
		m_patchBoxes.push_back( pca.analyze((Vector3F *)pos.data(), np) );
	}
	
	float totalLength = 0.f;
	std::vector<AOrientedBox>::const_iterator it = m_patchBoxes.begin();
	for(;it!=m_patchBoxes.end();++it)
		totalLength += (*it).extent().x;
		
	totalLength *= 2.f;
	std::cout<<"\n total patch length "<<totalLength;
	return true;
}
//:~