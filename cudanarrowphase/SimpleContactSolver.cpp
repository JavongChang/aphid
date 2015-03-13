/*
 *  SimpleContactSolver.cpp
 *  testnarrowpahse
 *
 *  Created by jian zhang on 3/8/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "SimpleContactSolver.h"
#include "CudaNarrowphase.h"
#include <CUDABuffer.h>
#include "simpleContactSolver_implement.h"
#include "scan_implement.h"
#include <ScanUtil.h>
SimpleContactSolver::SimpleContactSolver() 
{
	m_sortedInd[0] = new CUDABuffer;
	m_sortedInd[1] = new CUDABuffer;
	m_splitPair = new CUDABuffer;
	m_bodyCount = new CUDABuffer;
	m_splitInverseMass = new CUDABuffer;
	m_contactInverseMass = new CUDABuffer;
	m_lambda = new CUDABuffer;
	m_linearVelocityA = new CUDABuffer;
	m_linearVelocityB = new CUDABuffer;
	m_angularVelocityA = new CUDABuffer; 
	m_angularVelocityB = new CUDABuffer;
	m_deltaLinearVelocity = new CUDABuffer;
	m_deltaAngularVelocity = new CUDABuffer;
}

SimpleContactSolver::~SimpleContactSolver() {}

CUDABuffer * SimpleContactSolver::contactPairHashBuf()
{ return m_sortedInd[0]; }

CUDABuffer * SimpleContactSolver::bodySplitLocBuf()
{ return m_splitPair; }

void SimpleContactSolver::solveContacts(unsigned numContacts,
										void * contacts,
										CUDABuffer * pairBuf,
										void * objectData)
{
	if(numContacts < 1) return; 
	
	const unsigned indBufLength = nextPow2(numContacts * 2);
	m_sortedInd[0]->create(indBufLength * 8);
	m_sortedInd[1]->create(indBufLength * 8);
	
	void * dstInd = m_sortedInd[0]->bufferOnDevice();
	void * pairs = pairBuf->bufferOnDevice();

	simpleContactSolverWriteContactIndex((KeyValuePair *)dstInd, (uint *)pairs, numContacts * 2, indBufLength);
	
	void * tmp = m_sortedInd[1]->bufferOnDevice();
	RadixSort((KeyValuePair *)dstInd, (KeyValuePair *)tmp, indBufLength, 32);
	
	m_splitPair->create(numContacts * 8);
	void * splits = m_splitPair->bufferOnDevice();
	
	const unsigned splitBufLength = numContacts * 2;
	simpleContactSolverComputeSplitBufLoc((uint2 *)splits, (uint2 *)pairs, (KeyValuePair *)dstInd, splitBufLength);
	
	m_bodyCount->create(splitBufLength * 4);
	void * dstCount = m_bodyCount->bufferOnDevice();
	simpleContactSolverCountBody((uint *)dstCount, (KeyValuePair *)dstInd, splitBufLength);
	
	m_splitInverseMass->create(splitBufLength * 4);
	void * splitMass = m_splitInverseMass->bufferOnDevice();
	
	simpleContactSolverComputeSplitInverseMass((float *)splitMass, (uint *)dstCount, splitBufLength);
	
	m_contactInverseMass->create(numContacts * 4);
	void * contactMass = m_contactInverseMass->bufferOnDevice();
	
	m_lambda->create(numContacts * 4);
	void * lambda = m_lambda->bufferOnDevice();
	
	m_linearVelocityA->create(numContacts * 12);
	void * linVelA = m_linearVelocityA->bufferOnDevice();
	m_linearVelocityB->create(numContacts * 12);
	void * linVelB = m_linearVelocityB->bufferOnDevice();
	
	m_angularVelocityA->create(numContacts * 12);
	void * angVelA = m_angularVelocityA->bufferOnDevice();
	m_angularVelocityB->create(numContacts * 12);
	void * angVelB = m_angularVelocityB->bufferOnDevice();
	
	CudaNarrowphase::CombinedObjectBuffer * objectBuf = (CudaNarrowphase::CombinedObjectBuffer *)objectData;
	void * pos = objectBuf->m_pos->bufferOnDevice();
	void * vel = objectBuf->m_vel->bufferOnDevice();
	void * ind = objectBuf->m_ind->bufferOnDevice();
	void * perObjPointStart = objectBuf->m_pointCacheLoc->bufferOnDevice();
	void * perObjectIndexStart = objectBuf->m_indexCacheLoc->bufferOnDevice();
	
	simpleContactSolverSetContactConstraint((float3 *)linVelA,
	                                        (float3 *)linVelB,
	    (float3 *)angVelA,
	    (float3 *)angVelB,
	    (float *)lambda, (float *)contactMass, (float *)splitMass, 
	    (uint2 *)splits, 
	    (uint2 *)pairs,
	    (float3 *)pos,
	    (float3 *)vel,
        (uint4 *)ind,
        (uint * )perObjPointStart,
        (uint * )perObjectIndexStart,
        numContacts);
	
	m_deltaLinearVelocity->create(splitBufLength * 12);
	m_deltaAngularVelocity->create(splitBufLength * 12);
	
	void * deltaLinVel = m_deltaLinearVelocity->bufferOnDevice();
	void * deltaAngVel = m_deltaAngularVelocity->bufferOnDevice();
	simpleContactSolverClearDeltaVelocity((float3 *)deltaLinVel, (float3 *)deltaAngVel, splitBufLength);
	
	
	/*
	const unsigned scanBufLength = iRound1024(numContacts * 2);
	m_bodyCount->create(scanBufLength * 4);
	m_scanBodyCount[0]->create(scanBufLength * 4);
	m_scanBodyCount[1]->create(scanBufLength * 4);
	
	
	void * scanResult = m_scanBodyCount[0]->bufferOnDevice();
	void * scanIntermediate = m_scanBodyCount[1]->bufferOnDevice();
	scanExclusive((uint *)scanResult, (uint *)dstCount, (uint *)scanIntermediate, scanBufLength / 1024, 1024);
	
	const unsigned numSplitBodies = ScanUtil::getScanResult(m_bodyCount, m_scanBodyCount[0], scanBufLength);
	*/
	
	
	int i;
	for(i=0; i<7; i++) {
	    simpleContactSolverSolveContact((float3 *)deltaLinVel,
	                    (float3 *)deltaAngVel,
	                    (uint2 *)splits,
	                    (float *)splitMass,
	                    (float3 *)vel,
                        (uint2 *)pairs,
						(uint4 *)ind,
						(uint * )perObjPointStart,
                        (uint * )perObjectIndexStart,
                        numContacts);
	}
	
}