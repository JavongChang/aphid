#ifndef CURVEGROUP_H
#define CURVEGROUP_H

/*
 *  CurveGroup.h
 *  hesperis
 *
 *  Created by jian zhang on 4/21/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <AllMath.h>
class CurveGroup {
public:
	CurveGroup();
	virtual ~CurveGroup();
	void create(unsigned n, unsigned ncvs);
	void clear();
	
	Vector3F * points();
	unsigned * counts();
	
	const unsigned numPoints() const;
	const unsigned numCurves() const;
	
	void verbose() const;
protected:

private:
	Vector3F * m_points;
	unsigned * m_counts;
	unsigned m_numCurves, m_numPoints;
};
#endif        //  #ifndef CURVEGROUP_H
