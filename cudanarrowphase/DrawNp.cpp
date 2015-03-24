/*
 *  DrawNp.cpp
 *  testnarrowpahse
 *
 *  Created by jian zhang on 3/3/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "DrawNp.h"
#include <GeoDrawer.h>
#include <TetrahedronSystem.h>
#include <BaseBuffer.h>
#include <CudaNarrowphase.h>
#include <SimpleContactSolver.h>
#include "narrowphase_implement.h"
#include <CUDABuffer.h>
#include "radixsort_implement.h"

DrawNp::DrawNp() 
{
	m_x1 = new BaseBuffer;
	m_coord = new BaseBuffer;
	m_contact = new BaseBuffer;
	m_counts = new BaseBuffer;
	m_contactPairs = new BaseBuffer;
	m_scanResult = new BaseBuffer;
	m_pairsHash = new BaseBuffer;
	m_linearVelocity = new BaseBuffer;
	m_angularVelocity = new BaseBuffer;
	m_impulse = new BaseBuffer;
	m_deltaJ = new BaseBuffer;
	m_massTensor = new BaseBuffer;
	m_pntTetHash = new BaseBuffer;
}

DrawNp::~DrawNp() {}

void DrawNp::setDrawer(GeoDrawer * drawer)
{ m_drawer = drawer; }

void DrawNp::drawTetra(TetrahedronSystem * tetra)
{
	glColor3f(0.1f, 0.4f, 0.3f);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
    glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)tetra->hostX());
	glDrawElements(GL_TRIANGLES, tetra->numTriangleFaceVertices(), GL_UNSIGNED_INT, tetra->hostTriangleIndices());

	glDisableClientState(GL_VERTEX_ARRAY);
}

void DrawNp::drawTetraAtFrameEnd(TetrahedronSystem * tetra)
{
	computeX1(tetra);
		
	glColor3f(0.21f, 0.21f, 0.24f);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
    glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)m_x1->data());
	glDrawElements(GL_TRIANGLES, tetra->numTriangleFaceVertices(), GL_UNSIGNED_INT, tetra->hostTriangleIndices());

	glDisableClientState(GL_VERTEX_ARRAY);
}

void DrawNp::drawSeparateAxis(CudaNarrowphase * phase, BaseBuffer * pairs, TetrahedronSystem * tetra)
{
    computeX1(tetra, 0.f);
    Vector3F * ptet = (Vector3F *)m_x1->data();
    
    const unsigned np = phase->numPairs();
	m_contact->create(np * 48);
	
	phase->getContact0(m_contact);
	
	unsigned * pairInd = (unsigned *)pairs->data();
	unsigned * tetInd = (unsigned *)tetra->hostTretradhedronIndices();
	
	ContactData * contact = (ContactData *)m_contact->data();
	
	unsigned i;
	
	Vector3F dst, cenA, cenB;
	for(i=0; i < np; i++) {
		ContactData & cf = contact[i];
		
		if(cf.separateAxis.w < .1f) continue;
	    cenA = tetrahedronCenter(ptet, tetInd, pairInd[i * 2]);
	    cenB = tetrahedronCenter(ptet, tetInd, pairInd[i * 2 + 1]);
	    
	    glColor3f(0.1f, 0.2f, 0.06f);
	    m_drawer->arrow(cenA, cenA + tetrahedronVelocity(tetra, tetInd, pairInd[i * 2]));
	    m_drawer->arrow(cenB, cenB + tetrahedronVelocity(tetra, tetInd, pairInd[i * 2 + 1]));
		
		m_drawer->setColor(.5f, 0.f, 0.f);
		
		m_drawer->arrow(cenB + Vector3F(cf.localB.x, cf.localB.y, cf.localB.z), 
		cenB + Vector3F(cf.localB.x + cf.separateAxis.x, cf.localB.y + cf.separateAxis.y, cf.localB.z + cf.separateAxis.z));
		
		m_drawer->setColor(0.f, .5f, 0.f);
		m_drawer->arrow(cenA, cenA + Vector3F(cf.localA.x, cf.localA.y, cf.localA.z));
		m_drawer->arrow(cenB, cenB + Vector3F(cf.localB.x, cf.localB.y, cf.localB.z));
	}
}

void DrawNp::drawConstraint(SimpleContactSolver * solver, CudaNarrowphase * phase, TetrahedronSystem * tetra)
{
    const unsigned nc = phase->numContacts();
    if(nc < 1) return;
    
    computeX1(tetra, 0.f);
    Vector3F * ptet = (Vector3F *)m_x1->data();
    unsigned * tetInd = (unsigned *)tetra->hostTretradhedronIndices();
    
    m_contactPairs->create(nc * 8);
    phase->getContactPairs(m_contactPairs);
    
    unsigned * c = (unsigned *)m_contactPairs->data();
    unsigned i, j;
    glColor3f(0.4f, 0.9f, 0.6f);
	Vector3F dst, cenA, cenB;
	for(i=0; i < nc; i++) {
	    // cenA = tetrahedronCenter(ptet, tetInd, c[i*2]);
	    // cenB = tetrahedronCenter(ptet, tetInd, c[i*2+1]);
		// m_drawer->arrow(cenB, cenA);
	}
    
	CUDABuffer * bodyPair = solver->contactPairHashBuf();
	m_pairsHash->create(bodyPair->bufferSize());
	bodyPair->deviceToHost(m_pairsHash->data(), m_pairsHash->bufferSize());
	
	m_linearVelocity->create(nc * 2 * 12);
	solver->deltaLinearVelocityBuf()->deviceToHost(m_linearVelocity->data(), 
	    m_linearVelocity->bufferSize());
	Vector3F * linVel = (Vector3F *)m_linearVelocity->data();
	
	m_angularVelocity->create(nc * 2 * 12);
	solver->deltaAngularVelocityBuf()->deviceToHost(m_angularVelocity->data(), 
	    m_angularVelocity->bufferSize());
	Vector3F * angVel = (Vector3F *)m_angularVelocity->data();
	
	m_impulse->create(nc * 4);
	solver->impulseBuf()->deviceToHost(m_impulse->data(), m_impulse->bufferSize());
	float * J = (float *)m_impulse->data();
	
	m_contact->create(nc * 48);
	phase->contactBuffer()->deviceToHost(m_contact->data(), m_contact->bufferSize());
	ContactData * contact = (ContactData *)m_contact->data();
	
	const unsigned njacobi = solver->numIterations();
	
	m_deltaJ->create(nc * njacobi * 4);
	solver->deltaJBuf()->deviceToHost(m_deltaJ->data(), m_deltaJ->bufferSize());
	float * dJ = (float *)m_deltaJ->data();
	
	m_massTensor->create(nc * 4);
	solver->MinvBuf()->deviceToHost(m_massTensor->data(), m_massTensor->bufferSize());
	float * Minv = (float *)m_massTensor->data();
	
	Vector3F N;
	bool isA;
	unsigned iPairA, iBody, iPair;
	unsigned * bodyAndPair = (unsigned *)m_pairsHash->data();
	for(i=0; i < nc * 2; i++) {

	    iBody = bodyAndPair[i*2];
	    iPair = bodyAndPair[i*2+1];
	    
	    iPairA = iPair * 2;
// left or right
        isA = (iBody == c[iPairA]);

	    cenA = tetrahedronCenter(ptet, tetInd, iBody);
 
	    //cenB = cenA + angVel[i];
	    
	    //glColor3f(0.1f, 0.7f, 0.3f);
	    //m_drawer->arrow(cenA, cenB);
	    
	    ContactData & cd = contact[iPair];
	    float4 sa = cd.separateAxis;
	    N.set(sa.x, sa.y, sa.z);
	    N.reverse();
	    N.normalize();
	    
	    if(isA) {
// show contact normal for A
		    cenB = cenA + Vector3F(cd.localA.x, cd.localA.y, cd.localA.z);
		    m_drawer->setColor(0.f, .3f, .9f);
		    m_drawer->arrow(cenB, cenB + N);
// reverse N for A
            N.reverse();
		}
	    // cenB = cenA + linVel[i];
	    
	    // std::cout<<" "<<deltaLinVel[i]<<"\n";
	    // std::cout<<" NJ - dV "<<Vector3F(N * J[i], deltaLinVel[i]).length();
	    // glColor3f(0.9f, 0.8f, 0.1f);
	    // m_drawer->arrow(cenA, cenA + N * J[iPair]);
	    
		glColor3f(0.73f, 0.68f, 0.1f);
		m_drawer->arrow(cenA, cenA + linVel[i]);
		
		glColor3f(0.1f, 0.68f, 0.72f);
		m_drawer->arrow(cenA, cenA + angVel[i]);
		
		
		if(isA) {
		    std::cout<<"\npair["<<iPair<<"] \n Minv "<<Minv[iPair]<<"\n J "<<J[iPair]<<"\n";
            for(j=0; j< njacobi; j++) {
                std::cout<<" dJ["<<j<<"] "<<dJ[iPair * njacobi + j]<<"\n";
            }
        }
	}
	
	m_pntTetHash->create(nc * 2 * 4 * 8);
	solver->pntTetHashBuf()->deviceToHost(m_pntTetHash->data(), m_pntTetHash->bufferSize());
	KeyValuePair * pntHash = (KeyValuePair *)m_pntTetHash->data();
	
	std::cout<<"pnt-tet hash\n";
	for(i=0; i < nc * 8; i++) {
	    std::cout<<" pnt["<<i<<"] ("<<pntHash[i].key<<", "<<pntHash[i].value<<")\n";
	}
}

void DrawNp::printCoord(CudaNarrowphase * phase, BaseBuffer * pairs)
{
    const unsigned nc = phase->numContacts();
    m_coord->create(nc * 16);
    phase->getCoord(m_coord);
    float * coord = (float *)m_coord->data();
    unsigned i;
    for(i=0; i < nc; i++) {
        std::cout<<" "<<i<<":("<<coord[i*4]<<" "<<coord[i*4+1]<<" "<<coord[i*4+2]<<" "<<coord[i*4+3]<<") ";
    }
}

void DrawNp::printTOI(CudaNarrowphase * phase, BaseBuffer * pairs)
{
    const unsigned np = phase->numPairs();
    m_contact->create(np * 48);
	m_counts->create(np * 4);
    phase->getContact0(m_contact);
	phase->getContactCounts(m_counts);
    ContactData * contact = (ContactData *)m_contact->data();
	unsigned * counts = (unsigned *)m_counts->data();
    unsigned i;
    for(i=0; i < np; i++) {
        // if(counts[i])
		if(contact[i].timeOfImpact < .016667f) 
		std::cout<<" "<<i<<" "<<contact[i].timeOfImpact<<" ";
    }
	
	return;
	
	m_contactPairs->create(np * 8);
	m_scanResult->create(np * 4);
	
	if(phase->numContacts() < 1) return;
	
	phase->getContactPairs(m_contactPairs);
	phase->getScanResult(m_scanResult);
	
	unsigned * scans = (unsigned *)m_scanResult->data();
	
	for(i=0; i < np; i++) if(counts[i]) std::cout<<" i "<<i<<" to "<<scans[i]<<"\n";
	
	unsigned * squeezedPairs = (unsigned *)m_contactPairs->data();
	
	CudaNarrowphase::CombinedObjectBuffer * objectBuf = phase->objectBuffer();
	std::cout<<" n points "<<phase->numPoints();
	
	for(i=0; i < phase->numContacts(); i++) {
		std::cout<<" "<<i<<" ("<<squeezedPairs[i*2]<<", "<<squeezedPairs[i*2 + 1]<<")\n";
	}
}

void DrawNp::computeX1(TetrahedronSystem * tetra, float h)
{
    m_x1->create(tetra->numPoints() * 12);
	float * x1 = (float *)m_x1->data();
	
	float * x0 = tetra->hostX();
	float * vel = tetra->hostV();
	
	const float nf = tetra->numPoints() * 3;
	unsigned i;
	for(i=0; i < nf; i++)
		x1[i] = x0[i] + vel[i] * h;
}

Vector3F DrawNp::tetrahedronVelocity(TetrahedronSystem * tetra, unsigned * v, unsigned i)
{
    Vector3F * vel = (Vector3F *)tetra->hostV();
    Vector3F r = vel[v[i*4]];
    r += vel[v[i * 4 + 1]];
    r += vel[v[i * 4 + 2]];
    r += vel[v[i * 4 + 3]];
    r *= .25f;
    return r;
}

Vector3F DrawNp::tetrahedronCenter(Vector3F * p, unsigned * v, unsigned i)
{
    Vector3F r = p[v[i * 4]];
    r += p[v[i * 4 + 1]];
    r += p[v[i * 4 + 2]];
    r += p[v[i * 4 + 3]];
    r *= .25f;
    return r;
}

Vector3F DrawNp::interpolatePointTetrahedron(Vector3F * p, unsigned * v, unsigned i, float * wei)
{
    Vector3F r = Vector3F::Zero;
    
    if(wei[0]> 1e-5) r += p[v[i * 4]] * wei[0];
    if(wei[1]> 1e-5) r += p[v[i * 4 + 1]] * wei[1];
    if(wei[2]> 1e-5) r += p[v[i * 4 + 2]] * wei[2];
    if(wei[3]> 1e-5) r += p[v[i * 4 + 3]] * wei[3];
    return r;
}

void DrawNp::printContactPairHash(SimpleContactSolver * solver, unsigned numContacts)
{
	if(numContacts < 1) return;
	CUDABuffer * hashp = solver->contactPairHashBuf();
	
	std::cout<<" nc "<<numContacts<<"\n";
	
	m_pairsHash->create(hashp->bufferSize());
	hashp->deviceToHost(m_pairsHash->data(), m_pairsHash->bufferSize());
	
	unsigned * v = (unsigned *)m_pairsHash->data();
	
	unsigned i;
	std::cout<<" body-contact hash ";
	for(i=0; i < numContacts * 2; i++) {
		std::cout<<" "<<i<<" ("<<v[i*2]<<","<<v[i*2+1]<<")\n";
	}
	
	hashp = solver->bodySplitLocBuf();
	m_pairsHash->create(hashp->bufferSize());
	hashp->deviceToHost(m_pairsHash->data(), m_pairsHash->bufferSize());
	
	std::cout<<" body-split pairs ";
	for(i=0; i < numContacts; i++) {
		std::cout<<" "<<i<<" ("<<v[i*2]<<","<<v[i*2+1]<<")\n";
	}
}
