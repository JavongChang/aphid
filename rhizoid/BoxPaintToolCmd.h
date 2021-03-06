#ifndef BOXPAINTTOOLCMD_H
#define BOXPAINTTOOLCMD_H

/*
 *  BoxPaintToolCmd.h
 *  proxyPaint
 *
 *  Created by jian zhang on 2/1/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MString.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MItSelectionList.h>
#include <maya/MSelectionList.h>
#include <maya/MPxToolCommand.h> 
#include <maya/MSyntax.h>
#include <maya/MArgParser.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <VectorArray.h>
#include <ConvexShape.h>

class proxyPaintTool : public MPxToolCommand
{
    int m_currentVoxInd;
	aphid::Matrix33F::RotateOrder m_rotPca;
    
public:
					proxyPaintTool(); 
	virtual			~proxyPaintTool(); 
	static void*	creator();

	MStatus			doIt(const MArgList& args);
	MStatus			parseArgs(const MArgList& args);
	static MSyntax	newSyntax();
	MStatus			finalize();

private:
	MStatus connectGroundSelected();
	MStatus connectVoxelSelected();
	bool connectMeshToViz(MObject & meshObj, MObject & vizObj, unsigned & slot);
	bool connectVoxToViz(MObject & voxObj, MObject & vizObj, unsigned & slot);
	void connectTransform(MObject & transObj, MObject & vizObj, const unsigned & slot);
	MStatus saveCacheSelected();
	MStatus loadCacheSelected();
	MObject getSelectedViz(const MSelectionList & sels, 
							const MString & typName,
							MStatus & stat);
	MStatus voxelizeSelected();
	MObject createViz(const MString & typName, const MString & transName);
	void checkOutputConnection(MObject & node, const MString & outName);
	void getMeshTris(aphid::sdb::VectorArray<aphid::cvx::Triangle> & tris,
					aphid::BoundingBox & bbox,
					const MDagPath & meshPath);
	MStatus performPCA();
	MStatus performDFT();
	void strToRotateOrder(const MString & srod);
    
private:
	enum Operation {
		opUnknown = 0,
		opBeginPick = 1,
		opDoPick = 2,
		opEndPick = 3,
		opGetPick = 4,
		opConnectGround = 5,
		opSaveCache = 6,
		opLoadCache = 7,
		opVoxelize = 8,
		opConnectVoxel = 9,
        opPrincipalComponent = 10,
		opDistanceFieldTriangulate = 11
	};
	
	Operation m_operation;
	
	unsigned opt, nseg;
	int m_dftLevel;
	float lseg;
	double m_dftScale, m_dftRound;
	MString fBlockerName, fVizName, m_cacheName;
	
};
#endif        //  #ifndef BOXPAINTTOOLCMD_H

