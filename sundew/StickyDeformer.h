/*
 *  StickyDeformer.h
 *  Manuka
 *
 *  Created by jian zhang on 1/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <maya/MPxDeformerNode.h>
#include <maya/MTypeId.h> 
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include "DifferentialData.h"

class StickyDeformer : public MPxDeformerNode
{
public:
						StickyDeformer();
	virtual				~StickyDeformer();

	static  void*		creator();
	static  MStatus		initialize();

    virtual MStatus   	deform(MDataBlock& 		block,
							   MItGeometry& 	iter,
							   const MMatrix& 	mat,
							   unsigned int 	multiIndex);
	virtual MStatus connectionMade ( const MPlug & plug, const MPlug & otherPlug, bool asSrc );
	virtual MStatus connectionBroken ( const MPlug & plug, const MPlug & otherPlug, bool asSrc );
	
	static MObject ainMeanX;
	static MObject ainMeanY;
	static MObject ainMeanZ;
	static MObject ainMean;
	static MObject aradius;
	
	static MTypeId		id;

private:
	
};
