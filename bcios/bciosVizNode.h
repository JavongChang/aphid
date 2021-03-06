#ifndef BCIOSVIZNODE_H
#define BCIOSVIZNODE_H

/*
 *  bciosVizNode.h
 *  barycentric interpolation on sphere
 *
 *  Created by jian zhang on 6/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MPxLocatorNode.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/M3dView.h>
#include <maya/MMatrixArray.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <Matrix44F.h>
#include <Vector3F.h>
#include <HullContainer.h>

class BCIViz : public MPxLocatorNode
{
public:
	BCIViz();
	virtual ~BCIViz(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 

	static  void *          creator();
	static  MStatus         initialize();

	static MObject ainput;
	static MObject atargets;
	static MObject outValue;
	
public: 
	static	MTypeId		id;
	
private:
	char checkHull() const;
	char checkTarget() const;
	char checkFirstFour(const MPointArray & p) const;
	void drawDriver() const;
	void drawTargets() const;
	void drawNeighbours() const;
	void drawWeights() const;
	
	char constructHull();
	void findNeighbours();
	void calculateWeight();
	
	HullContainer * m_hull;
	Vector3F m_hitP;
	int neighbourId[3];
	float fAlpha, fBeta, fGamma;
	MPoint fDriverPos;
	MPointArray fTargetPositions;
	MDoubleArray m_resultWeights;
	int m_hitTriangle;
};
#endif        //  #ifndef BCIOSVIZNODE_H
