#ifndef GJKCONTACTSOLVER_H
#define GJKCONTACTSOLVER_H

/*
 *  GjkContactSolver.h
 *  proof
 *
 *  Created by jian zhang on 1/22/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "Gjk.h"

class GjkContactSolver {
public:
	GjkContactSolver();
	
	void distance(const PointSet & A, const PointSet & B, ClosestTestContext * result);
	
	const Simplex W() const;
private:
	Simplex m_W;
};
#endif        //  #ifndef GJKCONTACTSOLVER_H
