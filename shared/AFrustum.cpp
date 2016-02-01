/*
 *  AFrustum.cpp
 *  
 *
 *  Created by jian zhang on 2/2/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "AFrustum.h"
#include <iostream>
void AFrustum::set(const float & horizontalApeture,
			const float & verticalApeture,
			const float & focalLength,
			const float & clipNear,
			const float & clipFar,
			const Matrix44F & mat)
{
	float hfov = horizontalApeture * 0.5f / ( focalLength * 0.03937f );
	float aspectRatio = verticalApeture / horizontalApeture;
	
	float nearRight = clipNear * hfov;
	float nearLeft = -nearRight;
	float nearUp = nearRight * aspectRatio;
	float nearBottom = -nearUp;
	float farRight = clipFar * hfov;
	float farLeft = -farRight;
	float farUp = farRight * aspectRatio;
	float farBottom = -farUp;
	
	m_v[0] = mat.transform(Vector3F(nearLeft, nearBottom, clipNear));
	m_v[1] = mat.transform(Vector3F(nearRight, nearBottom, clipNear));
	m_v[2] = mat.transform(Vector3F(nearRight, nearUp, clipNear));
	m_v[3] = mat.transform(Vector3F(nearLeft, nearUp, clipNear));
	m_v[4] = mat.transform(Vector3F(farLeft, farBottom, clipFar));
	m_v[5] = mat.transform(Vector3F(farRight, farBottom, clipFar));
	m_v[6] = mat.transform(Vector3F(farRight, farUp, clipFar));
	m_v[7] = mat.transform(Vector3F(farLeft, farUp, clipFar));
}

const Vector3F * AFrustum::v(int idx) const
{ return &m_v[idx]; }
