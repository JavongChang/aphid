/*
 *  BaseTransform.h
 *  eulerRot
 *
 *  Created by jian zhang on 10/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllMath.h>
#include <TypedEntity.h>
#include <NamedEntity.h>

class BaseTransform : public TypedEntity, public NamedEntity {
public:
	BaseTransform(BaseTransform * parent = 0);
	virtual ~BaseTransform();
	
	BaseTransform * parent() const;
	
	void translate(const Vector3F & v);
	void setTranslation(const Vector3F & v);
	Vector3F translation() const;
	
	void rotate(const Vector3F & v);
	void setRotation(const Vector3F & v);
	void setRotation(const Matrix33F & m);
	Matrix33F rotation() const;
	Vector3F rotationAngles() const;
	
protected:

private:
	Matrix33F m_rotation;
	Vector3F m_translation, m_angles;
	BaseTransform * m_parent;
};