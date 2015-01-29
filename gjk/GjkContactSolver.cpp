/*
 *  GjkContactSolver.cpp
 *  proof
 *
 *  Created by jian zhang on 1/22/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "GjkContactSolver.h"
#include "EpaPenetrationSolver.h"

GjkContactSolver::GjkContactSolver() {}

void GjkContactSolver::distance(const PointSet & A, const PointSet & B, ClosestTestContext * result)
{
    int k = 0;
	float v2;
	Vector3F w, pa, pb;
	Vector3F v = A.X[0];
	Vector3F localA, localB;
	
	for(int i=0; i < 99; i++) {
		// SA-B(-v)
	    pa = A.supportPoint(v.reversed(), result->transformA, localA);
		pb = B.supportPoint(v, result->transformB, localB);
		w = pa - pb;// + v.normal() * MARGIN_DISTANCE;
	    
		// terminate when v is close enough to v(A - B).
	    // http://www.bulletphysics.com/ftp/pub/test/physics/papers/jgt04raycast.pdf
		v2 = v.length2();
	    if(v2 - w.dot(v) < 0.0001f * v2) {
	        // std::cout<<" v is close to w "<<v2 - w.dot(v)<<"\n";
			result->hasResult = 0;
			break;
	    }
	    
	    addToSimplex(result->W, w, localB);
 
	    if(isPointInsideSimplex(result->W, result->referencePoint)) {
	        // std::cout<<" Minkowski difference contains the reference point\n";
			result->hasResult = 1;
			break;
	    }
	    
	    closestOnSimplex(result);
	    v = result->closestPoint - result->referencePoint;
		interpolatePointB(result);
		// in world space
		result->contactNormal = v;
		smallestSimplex(result);
	    k++;
	}
	
	if(result->hasResult) {
		resetSimplex(result->W);
	    const Vector3F r = result->rayDirection;
		const Vector3F startP = Vector3F::Zero - result->rayDirection * 99.f;
		Vector3F hitP = startP;
		// from origin to startP
		v = hitP;
		Vector3F p;
		float lamda = 0.f;
		float vdotw, vdotr;

		k = 0;
		for(; k < 39; k++) {
			vdotr = v.dot(r);
	    
			// SA-B(v)
			pa = A.supportPoint(v, result->transformA, localA);
			pb = B.supportPoint(v.reversed(), result->transformB, localB);
			p = pa - pb;
			w = hitP - p;
			vdotw = v.dot(w); 
			
			if(vdotw > 0.f) {
                if(vdotr >= 0.f) {
                    break;
                }
                lamda -= vdotw / vdotr;
                hitP = startP + r * lamda;
            }
					
			addToSimplex(result->W, p, localB);
	    
			result->hasResult = 0;
			result->distance = 1e9;
			result->referencePoint = hitP;
	    
			closestOnSimplex(result);
	    
            v = hitP - result->closestPoint;
			
			interpolatePointB(result);
		
			if(v.length2() < TINY_VALUE) break;
			
			result->contactNormal = v;
		
			smallestSimplex(result);
		}
		
		result->hasResult = 1;
		result->penetrateDepth = hitP.length();
		result->contactNormal.normalize();
	}
}

void GjkContactSolver::rayCast(const PointSet & A, const PointSet & B, ClosestTestContext * result)
{
	distance(A, B, result);
	if(result->hasResult) return;
	
	resetSimplex(result->W);
	
	const Vector3F r = result->rayDirection;
	float lamda = 0.f;
	// ray started at origin
	const Vector3F startP = Vector3F::Zero;
	Vector3F hitP = startP;
	Vector3F hitN; hitN.setZero();
	Vector3F v = hitP - result->closestPoint;
	Vector3F w, p, pa, pb;
	
	float vdotw, vdotr;
	int k = 0;
	for(; k < 39; k++) {
	    vdotr = v.dot(r);
	    
	    // SA-B(v)
		pa = A.supportPoint(v);
		pb = B.supportPoint(v.reversed());
	    p = pa - pb + v.normal() * MARGIN_DISTANCE;
	    
		w = hitP - p;
	    vdotw = v.dot(w); 
	    
	    if(vdotw > 0.f) {
			// std::cout<<" v.w > 0\n";
			if(vdotr >= 0.f) {
				// std::cout<<" v.r >= 0 missed\n";
				result->hasResult = 0;
				return;
			}
			lamda -= vdotw / vdotr;
			hitP = startP + r * lamda;
			hitN = v;
		}
		
	    addToSimplex(result->W, p, pb);
	    
	    result->hasResult = 0;
	    result->distance = 1e9;
	    result->referencePoint = hitP;
	    
	    closestOnSimplex(result);
	    
	    v = hitP - result->closestPoint;
		
		interpolatePointB(result);
		
		if(v.length2() < TINY_VALUE) break;
		
		smallestSimplex(result);
	}
	
	if(k==39) std::cout<<"    max iterations reached!\n";
	// std::cout<<" k"<<k<<" ||v|| "<<v.length()<<"\n";
	result->hasResult = 1;
	result->contactNormal = hitN.normal();
	result->distance = lamda;
}
