/*
 *  DrawForest.cpp
 *  proxyPaint
 *
 *  Created by jian zhang on 1/30/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "DrawForest.h"
#include <gl_heads.h>
#include <CircleCurve.h>
#include <DepthCull.h>
#include <ExampVox.h>

DrawForest::DrawForest() 
{
	m_scalbuf[0] = 1.f; 
	m_scalbuf[1] = 1.f; 
	m_scalbuf[2] = 1.f; 
    m_circle = new CircleCurve;
}

DrawForest::~DrawForest() {}

void DrawForest::setScaleMuliplier(float x, int idx)
{ m_scalbuf[idx] = x; }

void DrawForest::drawGround() 
{
	glPushAttrib(GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glColor3f(.57f, .37f, 0.f);
	
	glBegin(GL_TRIANGLES);
	SelectionContext * active = activeGround();
	std::map<Geometry *, sdb::Sequence<unsigned> * >::iterator it = active->geometryBegin();
	for(; it != active->geometryEnd(); ++it) {
		drawFaces(it->first, it->second);
	}
	glEnd();
	glPopAttrib();
}

void DrawForest::drawFaces(Geometry * geo, sdb::Sequence<unsigned> * components)
{
	ATriangleMesh * mesh = static_cast<ATriangleMesh *>(geo);
	Vector3F *p = mesh->points();
	components->begin();
	while(!components->end()) {
		unsigned * tri = mesh->triangleIndices(components->key() );
		glVertex3fv((GLfloat *)&p[tri[0]]);
		glVertex3fv((GLfloat *)&p[tri[1]]);
		glVertex3fv((GLfloat *)&p[tri[2]]);
		components->next();
	}
}

void DrawForest::drawWiredPlants()
{
	glDepthFunc(GL_LEQUAL);
	
	sdb::WorldGrid<sdb::Array<int, sdb::Plant>, sdb::Plant > * g = grid();
	if(g->isEmpty() ) return;
	g->begin();
	while(!g->end() ) {
		drawWiredPlants(g->value() );
		g->next();
	}
}

void DrawForest::drawWiredPlants(sdb::Array<int, sdb::Plant> * cell)
{
	cell->begin();
	while(!cell->end() ) {
		drawWiredPlant(cell->value()->index);
		cell->next();
	}
}

void DrawForest::drawWiredPlant(sdb::PlantData * data)
{
	glPushMatrix();
    
	float m[16];
	data->t1->glMatrix(m);
	glMultMatrixf((const GLfloat*)m);
	const ExampVox * v = plantExample(*data->t3);
	drawWireBox(v->geomCenterV(), v->geomScale() );
		
	glPopMatrix();
}

void DrawForest::drawPlants()
{
    glDepthFunc(GL_LEQUAL);
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_LIGHTING);
		
	sdb::WorldGrid<sdb::Array<int, sdb::Plant>, sdb::Plant > * g = grid();
	if(g->isEmpty() ) return;
	const float margin = g->gridSize() * .1f;
	g->begin();
	while(!g->end() ) {
        BoundingBox cellBox = g->coordToGridBBox(g->key() );
        cellBox.expand(margin);
        if(!cullByFrustum(cellBox ) )
            drawPlants(g->value() );
		g->next();
	}
	
	glDisable(GL_LIGHTING);
	glPopAttrib();
}

void DrawForest::drawPlants(sdb::Array<int, sdb::Plant> * cell)
{
	cell->begin();
	while(!cell->end() ) {
		drawPlant(cell->value()->index);
		cell->next();
	}
}

void DrawForest::drawPlant(sdb::PlantData * data)
{
	glPushMatrix();
    
	data->t1->glMatrix(m_transbuf);
	glMultMatrixf(m_transbuf);
	glScalef(m_scalbuf[0], m_scalbuf[1], m_scalbuf[2]);
	const ExampVox * v = plantExample(*data->t3);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, v->diffuseMaterialColor() );
	drawSolidBox(v->geomCenterV(), v->geomScale() );
		
	glPopMatrix();
}

void DrawForest::drawGridBounding()
{
	if(numPlants() < 1) return;
	drawBoundingBox(&gridBoundingBox() );
}

void DrawForest::drawGrid()
{
	sdb::WorldGrid<sdb::Array<int, sdb::Plant>, sdb::Plant > * g = grid();
	if(g->isEmpty() ) return;
	g->begin();
	while(!g->end() ) {
		drawBoundingBox(&g->coordToGridBBox(g->key() ) );
		g->next();
	}
}

void DrawForest::drawActivePlants()
{
	if(numActivePlants() < 1) return;
	glDepthFunc(GL_LEQUAL);
	glColor3f(.1f, .9f, .43f);
	sdb::Array<int, sdb::PlantInstance> * arr = activePlants();
	arr->begin();
	while(!arr->end() ) {
		drawWiredPlant(arr->value()->m_reference->index );
		arr->next();
	}
}

void DrawForest::drawViewFrustum()
{
	const AFrustum & fr = frustum();
	glBegin(GL_LINES);
	for(int i=0; i < 4; i++) {
		glVertex3fv((const GLfloat*)fr.v(i) );
		glVertex3fv((const GLfloat*)fr.v(i+4) );
	}
	glEnd();
}

void DrawForest::drawBrush()
{
    const float & radius = selectionRadius();
    const Vector3F & position = selectionCenter();
    const Vector3F & direction = selectionNormal();
    const float offset = radius * 0.05f;
    const float part = radius * 0.33f;
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glTranslatef(direction.x * offset, direction.y * offset, direction.z * offset);
    
    glBegin(GL_LINES);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(direction.x * part, direction.y * part, direction.z * part);
    glEnd();
    
    glPushMatrix();
    glScalef(radius, radius, radius);
    glPushMatrix();
    
    m_useMat.setFrontOrientation(direction);
    m_useMat.glMatrix(m_transbuf);
    glMultMatrixf(m_transbuf);
	
    drawCircle();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    
}

void DrawForest::drawCircle() const
{
	Vector3F p;
	glBegin(GL_LINE_STRIP);
	for(unsigned i = 0; i < m_circle->numVertices(); i++) {
		p = m_circle->getCv(i);
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();
}

void DrawForest::drawDepthCull(double * localTm)
{
	DepthCull * culler = depthCuller();
	culler->setLocalSpace(localTm);
	culler->frameBufferBegin();
	culler->drawFrameBuffer(groundMeshes() );
	culler->frameBufferEnd();
	//culler->showFrameBuffer();
}

bool DrawForest::isVisibleInView(sdb::Plant * pl,
					const float lowLod, const float highLod)
{
	sdb::PlantData * d = pl->index;
	int typ = *d->t3;
	ExampVox * v = plantExample(typ);
	const Vector3F & localP = v->geomCenter();
	Vector3F worldP = d->t1->transform(localP);
	float r = v->geomExtent() * d->t1->getSide().length();
	if(cullByFrustum(worldP, r) ) return false;
	float camZ;
	if(cullByDepth(worldP, r, camZ) ) return false;
	if(lowLod > 0.f || highLod < 1.f) {
		if(cullByLod(camZ, r, lowLod, highLod ) ) return false;
	}
	return true;
}
//:~