/*
 *  MlDrawer.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/15/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "MlDrawer.h"
#include "MlCalamus.h"
#include "MlSkin.h"
#include "MlTessellate.h"
#include <AccPatchMesh.h>
#include <PointInsidePolygonTest.h>
MlDrawer::MlDrawer()
{
	m_featherTess = new MlTessellate; 
}

MlDrawer::~MlDrawer() 
{
	delete m_featherTess;
}

void MlDrawer::drawFeather(MlSkin * skin) const
{
	if(skin->numFeathers() > 0) drawBuffer();

	const unsigned num = skin->numCreated();
	if(num < 1) return;
	
	unsigned i;
	for(i = 0; i < num; i++) {
		MlCalamus * c = skin->getCreated(i);
		drawAFeather(skin, c);
	}
}

void MlDrawer::drawAFeather(MlSkin * skin, MlCalamus * c) const
{
	Vector3F p;
	skin->getPointOnBody(c, p);
	Matrix33F frm = skin->tangentFrame(c);
	
	Matrix33F space;
	space.rotateX(c->rotateX());
	
	space.multiply(frm);
	
	c->collideWith(skin);
	c->computeFeatherWorldP(p, space);
	m_featherTess->setFeather(c->feather());
	m_featherTess->evaluate(c->feather());
	
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, m_featherTess->vertices() );
	
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer( GL_FLOAT, 0, m_featherTess->normals() );
	
	glDrawElements( GL_QUADS, m_featherTess->numIndices(), GL_UNSIGNED_INT, m_featherTess->indices());
	
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
}

void MlDrawer::hideAFeather(MlCalamus * c)
{
	const unsigned loc = c->bufferStart();
	m_featherTess->setFeather(c->feather());
	const unsigned nipf = m_featherTess->numIndices();
	for(unsigned i = 0; i < nipf; i++) {
		indices()[loc + i] = indices()[loc];
	}
}

void MlDrawer::hideActive(MlSkin * skin)
{
	const unsigned num = skin->numActive();
	if(num < 1) return;
	
	unsigned i;
	for(i = 0; i < num; i++) {
		MlCalamus * c = skin->getActive(i);
		hideAFeather(c);
	}
}

void MlDrawer::updateActive(MlSkin * skin)
{
	const unsigned num = skin->numActive();
	if(num < 1) return;
	
	unsigned i;
	for(i = 0; i < num; i++) {
		MlCalamus * c = skin->getActive(i);
		computeAFeather(skin, c);
	}
}

void MlDrawer::computeAFeather(MlSkin * skin, MlCalamus * c)
{
	tessellate(skin, c);
	
	const unsigned nvpf = m_featherTess->numVertices();
	const unsigned startv = indices()[c->bufferStart()];
	for(unsigned i = 0; i < nvpf; i++) {
		vertices()[startv + i] = m_featherTess->vertices()[i];
		normals()[startv + i] = m_featherTess->normals()[i];
	}
}

void MlDrawer::rebuildBuffer(MlSkin * skin)
{
    const unsigned nf = skin->numFeathers();
	if(nf < 1) return;
	
	unsigned nv = 0, ni = 0;
    unsigned i, j;
	for(i = 0; i < nf; i++) {
		MlCalamus * c = skin->getCalamus(i);
		m_featherTess->setFeather(c->feather());
		nv += m_featherTess->numVertices();
		ni += m_featherTess->numIndices();
	}
	
	createBuffer(nv, ni);
	
	unsigned curv = 0, curi = 0, nvpf, nipf;
	for(i = 0; i < nf; i++) {
		MlCalamus * c = skin->getCalamus(i);
		tessellate(skin, c);
		
		nvpf = m_featherTess->numVertices();
		for(j = 0; j < nvpf; j++) {
		    vertices()[curv + j] = m_featherTess->vertices()[j];
		    normals()[curv + j] = m_featherTess->normals()[j];
		}
		
		nipf = m_featherTess->numIndices();
		for(j = 0; j < nipf; j++) {
		    indices()[curi + j] = curv + m_featherTess->indices()[j];
		}
		
		c->setBufferStart(curi);
		
		curv += nvpf;
		curi += nipf;
	}
	
	//std::cout<<"nv "<< curv;
	//std::cout<<"ni "<< curi;
}

void MlDrawer::tessellate(MlSkin * skin, MlCalamus * c)
{
	Vector3F p;
	skin->getPointOnBody(c, p);
	Matrix33F frm = skin->tangentFrame(c);

	Matrix33F space;
	space.rotateX(c->rotateX());

	space.multiply(frm);

	c->collideWith(skin);
	c->computeFeatherWorldP(p, space);
	m_featherTess->setFeather(c->feather());
	m_featherTess->evaluate(c->feather());
}
