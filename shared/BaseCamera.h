/*
 *  BaseCamera.h
 *  lbm3d
 *
 *  Created by jian zhang on 7/25/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <Matrix44F.h>
#include <Vector3F.h>
class BaseCamera {
public:
	BaseCamera();
	virtual ~BaseCamera();
	
	virtual bool isOrthographic() const;
	void reset();
	void lookFromTo(Vector3F & from, Vector3F & to);
	void setPortWidth(unsigned w);
	void setPortHeight(unsigned h);
	void setHorizontalAperture(float w);
	void updateInverseSpace();
	void getMatrix(float* m) const;
	void tumble(int x, int y);
	void track(int x, int y);
	void zoom(int y);
	void moveForward(int y);
	
	char intersection(int x, int y, Vector3F & worldPos) const;
	void screenToWorld(int x, int y, Vector3F & worldVec) const;
	void incidentRay(int x, int y, Vector3F & origin, Vector3F & worldVec) const;
	Vector3F eyePosition() const;
	float aspectRatio() const;
	float getHorizontalAperture() const;
	
	Matrix44F fSpace, fInverseSpace;
	Vector3F fCenterOfInterest;
	unsigned fPortWidth, fPortHeight;
	float fHorizontalAperture;
};