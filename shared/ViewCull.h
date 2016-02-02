/*
 *  ViewCull.h
 *  proxyPaint
 *
 *  Created by jian zhang on 2/2/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AFrustum.h>

class ViewCull {
	
	AFrustum m_frustum;
	Matrix44F m_space, m_invSpace;
	bool m_enabled;
	
public:
	ViewCull();
	virtual ~ViewCull();
	
	void enable();
	void disable();
	
/// cliping is negative in camera space
	void setFrustum(const float & horizontalApeture,
			const float & verticalApeture,
			const float & focalLength,
			const float & clipNear,
			const float & clipFar);
			
	Matrix44F *	cameraSpaceP();
	Matrix44F * cameraInvSpaceP();
	const Matrix44F & cameraSpace() const;
	const AFrustum & frustum() const;
	
	bool cullByFrustum(const Vector3F & center, const float & radius) const;
	
protected:
	
private:

};