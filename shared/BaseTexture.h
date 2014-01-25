/*
 *  BaseTexture.h
 *  aphid
 *
 *  Created by jian zhang on 1/25/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllMath.h>
#include <TypedEntity.h>
#include <NamedEntity.h>
class BaseTexture : public TypedEntity, public NamedEntity {
public:
	enum TexelFormat {
		FUChar = 0,
		FFloat = 1
	};
	
	enum TexelDepth {
		DOne = 0,
		DThree = 1
	};
	
	BaseTexture();
	virtual ~BaseTexture();
	virtual unsigned numTexels() const;
	virtual void * data();
	virtual void * data() const;
	virtual void sample(const unsigned & faceIdx, const float & faceU, const float & faceV, Float3 & dst) const;

	void setTextureFormat(TexelFormat f);
	void setTextureDepth(TexelDepth d);
	TexelFormat textureFormat() const;
	TexelDepth textureDepth() const;
protected:

private:
	TexelFormat m_format;
	TexelDepth m_depth;
};