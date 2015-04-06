/*
 *  BezierCurve.cpp
 *  knitfabric
 *
 *  Created by jian zhang on 5/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "BezierCurve.h"
#include <BoundingBox.h>

BezierCurve::BezierCurve() {}
BezierCurve::~BezierCurve() {}

Vector3F BezierCurve::interpolate(float param) const
{	
	unsigned seg = segmentByParameter(param);
	Vector3F p[4];
	
	calculateCage(seg, p);
	float t = param * numSegments() - seg;
	
	return calculateBezierPoint(t, p);
}

void BezierCurve::calculateCage(unsigned seg, Vector3F *p) const
{
	if(seg == 0) p[0] = m_cvs[0];
	else p[0] = (m_cvs[seg] * 2.f + m_cvs[seg - 1] + m_cvs[seg + 1]) * .25f;

	if(seg >= numSegments() - 1) p[3] = m_cvs[numSegments()];
	else p[3] = (m_cvs[seg + 1] * 2.f + m_cvs[seg] + m_cvs[seg + 2]) * .25f;

	p[1] = (m_cvs[seg + 1] * 0.5f + m_cvs[seg] * 0.5f) * .5f + p[0] * .5f;
	p[2] = (m_cvs[seg] * 0.5f + m_cvs[seg + 1] * 0.5f) * .5f + p[3] * .5f;
}

Vector3F BezierCurve::calculateBezierPoint(float t, Vector3F * data) const
{
	float u = 1.f - t;
	float tt = t * t;
	float uu = u*u;
	float uuu = uu * u;
	float ttt = tt * t;

	Vector3F p0 = data[0];
	Vector3F p1 = data[1];
	Vector3F p2 = data[2];
	Vector3F p3 = data[3];

	Vector3F p = p0 * uuu; //first term
	p += p1 * 3.f * uu * t; //second term
	p += p2 * 3.f * u * tt; //third term
	p += p3 * ttt; //fourth term
	return p;
}
/*
void BezierCurve::getAccSegmentCurves(BezierCurve * dst) const
{
    unsigned i, j;
    unsigned v[4];
    for(j = 0; j < numSegments(); j++) {
        v[0] = j-1;
        v[1] = j;
        v[2] = j+1;
        v[3] = j+2;
        if(j==0) v[0] = 0;
        if(j==numSegments()-1) v[3] = j+1;
        
        if(j==0) dst[j].m_cvs[0] = m_cvs[v[0]];
        else dst[j].m_cvs[0] = m_cvs[v[0]] * .25f + m_cvs[v[1]] * .5f + m_cvs[v[2]] * .25f ;
        dst[j].m_cvs[1] = (dst[j].m_cvs[0] * 5.f + m_cvs[v[1]] * 2.f + m_cvs[v[2]] * 2.f) * (1.f / 9.f);
        
        if(j==numSegments()-1) dst[j].m_cvs[3] = m_cvs[v[3]];
        else dst[j].m_cvs[3] = m_cvs[v[1]] * .25f + m_cvs[v[2]] * .5f + m_cvs[v[3]] * .25f ;
        dst[j].m_cvs[2] = (dst[j].m_cvs[3] * 5.f + m_cvs[v[1]] * 2.f + m_cvs[v[2]] * 2.f) * (1.f / 9.f);
    }
}
*/
void BezierCurve::getSegmentSpline(unsigned i, BezierSpline & spline) const
{
	unsigned v[4];
	v[0] = i-1;
	v[1] = i;
	v[2] = i+1;
	v[3] = i+2;
	if(i==0) v[0] = 0;
	if(i==numSegments()-1) v[3] = i+1;
	
	if(i==0) spline.cv[0] = m_cvs[v[0]];
	else spline.cv[0] = m_cvs[v[0]] * .25f + m_cvs[v[1]] * .5f + m_cvs[v[2]] * .25f ;
	
	spline.cv[1] = spline.cv[0] * .5f + m_cvs[v[1]] * .25f + m_cvs[v[2]] * .25f;
	
	if(i==numSegments()-1) spline.cv[3] = m_cvs[v[3]];
	else spline.cv[3] = m_cvs[v[1]] * .25f + m_cvs[v[2]] * .5f + m_cvs[v[3]] * .25f ;
	
	spline.cv[2] = spline.cv[3] * .5f + m_cvs[v[1]] * .25f + m_cvs[v[2]] * .25f;
}

float BezierCurve::distanceToPoint(const Vector3F & toP, Vector3F & closestP) const
{
    float minD = 1e8;
    const unsigned ns = numSegments();
    for(unsigned i=0; i < ns; i++) {
        BezierSpline sp;
        getSegmentSpline(i, sp);   
        distanceToPoint(sp, toP, minD, closestP);
    }
    return minD;
}

void BezierCurve::distanceToPoint(BezierSpline & spline, const Vector3F & pnt, float & minDistance, Vector3F & closestP) const
{
	BoundingBox box;
	box.expandBy(spline.cv[0]);
	box.expandBy(spline.cv[1]);
	box.expandBy(spline.cv[2]);
	box.expandBy(spline.cv[3]);
	
	if(box.distanceTo(pnt) > minDistance) return;
	
    float paramMin = 0.f;
    float paramMax = 1.f;
    Vector3F line[2];
    
    line[0] = spline.calculateBezierPoint(paramMin);
    line[1] = spline.calculateBezierPoint(paramMax);
    
    Vector3F pol;
    float t;
    for(;;) {
        float d = closestDistanceToLine(line, pnt, pol, t);
        
        const float tt = paramMin * (1.f - t) + paramMax * t;
        
// end of line
        if((tt <= 0.f || tt >= 1.f) && paramMax - paramMin < 0.1f) {
            if(d < minDistance) {
                minDistance = d;
                closestP = pol;
            }
            break;
        }
        
        const float h = .5f * (paramMax - paramMin);

// small enought        
        if(h < 1e-3) {
            if(d < minDistance) {
                minDistance = d;
                closestP = pol;
            }
            break;
        }
        
        if(t > .5f)
            paramMin = tt - h * ((t - .5f)/.5f * .5f + .5f);
        else
            paramMax = tt + h * ((.5f - t)/.5f * .5f + .5f);
            
        line[0] = spline.calculateBezierPoint(paramMin);
        line[1] = spline.calculateBezierPoint(paramMax);
    }
}

bool BezierCurve::intersectBox(const BoundingBox & box) const
{
    BoundingBox ab;
    getAabb(&ab);
    if(!ab.intersect(box)) return false;
    
	const unsigned ns = numSegments();
    for(unsigned i=0; i < ns; i++) {
        BezierSpline sp;
        getSegmentSpline(i, sp);   
        if(intersectBox(sp, box)) return true;
    }
	
	return false;
}

bool BezierCurve::intersectBox(BezierSpline & spline, const BoundingBox & box) const
{
	BoundingBox abox;
	abox.expandBy(spline.cv[0]);
	abox.expandBy(spline.cv[1]);
	abox.expandBy(spline.cv[2]);
	abox.expandBy(spline.cv[3]);
	
	if(!abox.intersect(box)) return false;
	
	if(abox.inside(box)) return true;
	
	BezierSpline stack[64];
	int stackSize = 2;
	spline.deCasteljauSplit(stack[0], stack[1]);
	
	while(stackSize > 0) {
		BezierSpline c = stack[stackSize - 1];
		stackSize--;
		
		abox.reset();
		abox.expandBy(c.cv[0]);
		abox.expandBy(c.cv[1]);
		abox.expandBy(c.cv[2]);
		abox.expandBy(c.cv[3]);
		
		if(abox.inside(box)) return true;
		
		if(abox.intersect(box)) {
			if(abox.area() < 0.007f) return true;
			
			BezierSpline a, b;
			c.deCasteljauSplit(a, b);
			
			stack[ stackSize ] = a;
			stackSize++;
			stack[ stackSize ] = b;
			stackSize++;
		}
	}
	
	return false;
}
