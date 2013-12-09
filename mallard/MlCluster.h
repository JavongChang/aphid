/*
 *  MlCluster.h
 *  mallard
 *
 *  Created by jian zhang on 12/7/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <KMeansClustering.h>
class MlCalamusArray;
class AccPatchMesh;
class MlCluster : public KMeansClustering {
public:
	MlCluster();
	virtual ~MlCluster();
	
	void compute(MlCalamusArray * calamus, AccPatchMesh * mesh, unsigned begin, unsigned end);
	
	void computeAngles();
	float * angles(unsigned idx) const;
protected:
    virtual void setK(unsigned k);
	virtual void setN(unsigned n);

private:
    short * m_featherIndices;
    unsigned * m_angleStart;
    float * m_angles;
};