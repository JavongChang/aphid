#ifndef BOXPAINTTOOL_H
#define BOXPAINTTOOL_H

/*
 *  proxyPaintTool.h
 *  hair
 *
 *  Created by jian zhang on 6/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MPxContext.h>
#include <maya/M3dView.h>
#include "ProxyVizNode.h"
#include <maya/MSceneMessage.h>

class proxyPaintContext : public MPxContext
{
    
	M3dView					view;
	static ProxyViz * PtrViz;
	
	double clipNear, clipFar;
	float m_brushWeight;
	float m_min_scale, m_max_scale, m_rotation_noise, m_createMargin;
	
    enum Operation {
        opUnknown = 0,
        opErase = 1,
        opSelect = 2,
        opResize = 3,
        opMove = 4,
        opRotateY = 5,
        opRotateZ = 6,
        opRotateX = 7,
        opResizeBrush = 8,
        opSelectGround = 9,
        opReplace = 10,
        opCreate = 11,
        opClean = 99,
        opFlood = 100,
        opExtract = 102
    };
    
    Operation m_currentOpt, mOpt;
    
	unsigned m_growAlongNormal, m_multiCreate;
	int m_extractGroupCount, m_plantType;
	short					start_x, start_y;
	short					last_x, last_y;

	MGlobal::ListAdjustment	m_listAdjustment;
	MCallbackId fBeforeNewCB;
	MCallbackId fBeforeOpenCB;
	
public:
					proxyPaintContext();
	virtual ~proxyPaintContext();
	virtual void	toolOnSetup( MEvent & event );
	virtual MStatus	doPress( MEvent & event );
	virtual MStatus	doDrag( MEvent & event );
	virtual MStatus	doRelease( MEvent & event );
	virtual MStatus	doEnterRegion( MEvent & event );
	
	virtual	void	getClassName(MString & name) const;
	
	void setOperation(short val);
	unsigned getOperation() const;
	void setBrushRadius(float val);
	float getBrushRadius() const;
	void setScaleMin(float val);
	float getScaleMin() const;
	void setScaleMax(float val);
	float getScaleMax() const;
	void setRotationNoise(float val);
	float getRotationNoise() const;
	void setBrushWeight(float val);
	float getBrushWeight() const;
	void setGrowAlongNormal(unsigned val);
	unsigned getGrowAlongNormal() const;
	void setMultiCreate(unsigned val);
	unsigned getMultiCreate() const;
	void setInstanceGroupCount(unsigned val);
    unsigned getInstanceGroupCount() const;
	void setWriteCache(MString filename);
	void setReadCache(MString filename);
	void cleanup();
	char getSelectedViz();
	void setCreateMargin(float x);
	const float & createMargin();
	void setPlantType(int x);
	const int & plantType() const;
	
private:
	void resize();
	void grow();
	void flood();
	void snap();
	void erase();
	void move();
	void extractSelected();
	void erectSelected();
	void rotateAroundAxis(short axis);
	void moveAlongAxis(short axis);
	void startProcessSelect();
	void processSelect();
	char validateViz(const MSelectionList &sels);
	char validateSelection();
	void smoothSelected();
	void selectGround();
	void startSelectGround();
	void setGrowOption(ProxyViz::GrowOption & opt);
	void finishGrow();
	void replace();
    void scaleBrush();
	void attachSceneCallbacks();
	void detachSceneCallbacks();
	static void releaseCallback(void* clientData);
	
};
#endif        //  #ifndef BOXPAINTTOOL_H

