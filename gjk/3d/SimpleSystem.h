#ifndef SIMPLESYSTEM_H
#define SIMPLESYSTEM_H

/*
 *  SimpleSystem.h
 *  proof
 *
 *  Created by jian zhang on 1/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include "GjkContactSolver.h"

struct RigidBody {
	Quaternion orientation;
	Vector3F position;
	Vector3F linearVelocity;
	Vector3F angularVelocity;
	PointSet * shape;
};

class TetrahedronShape : public PointSet {
public:
	TetrahedronShape() {}
	virtual const float angularMotionDisc() const
    {
		Aabb box;
		resetAabb(box);
        for(int i=0; i < 4; i++) 
			expandAabb(box, p[i]);

        const Vector3F center = box.low * 0.5f + box.high * 0.5f;
        const Vector3F d = box.high - box.low;
        return center.length() + d.length() * 0.5f;
    }
	
	virtual const Vector3F supportPoint(const Vector3F & v, const Matrix44F & space, Vector3F & localP) const
    {
        float maxdotv = -1e8;
        float dotv;
        
        Vector3F res;
        Vector3F worldP;
        
        for(int i=0; i < 4; i++) {
            worldP = space.transform(p[i]);
            dotv = worldP.dot(v);
            if(dotv > maxdotv) {
                maxdotv = dotv;
                res = worldP;
                localP = p[i];
            }
        }
        
        return res;
    }
	
	Vector3F p[4];
};

class CuboidShape : public PointSet {
public:
	CuboidShape(float w, float h, float d)
	{
		m_w = w;
		m_h = h;
		m_d = d;
	}
	
	virtual const float angularMotionDisc() const
    {
        return Vector3F(m_w, m_h, m_d).length();
    }
	
	virtual const Vector3F supportPoint(const Vector3F & v, const Matrix44F & space, Vector3F & localP) const
    {
        float maxdotv = -1e8;
        float dotv;
        
        Vector3F res;
        Vector3F worldP;
		
		Vector3F p[8];
		fillP(p);
        
        for(int i=0; i < 8; i++) {
            worldP = space.transform(p[i]);
            dotv = worldP.dot(v);
            if(dotv > maxdotv) {
                maxdotv = dotv;
                res = worldP;
                localP = p[i];
            }
        }
        
        return res;
    }
	
	void fillP(Vector3F * p) const
	{
		p[0].set(-m_w, -m_h, -m_d);
        p[1].set( m_w, -m_h, -m_d);
        p[2].set(-m_w,  m_h, -m_d);
        p[3].set( m_w,  m_h, -m_d);
        p[4].set(-m_w, -m_h,  m_d);
        p[5].set( m_w, -m_h,  m_d);
        p[6].set(-m_w,  m_h,  m_d);
        p[7].set( m_w,  m_h,  m_d);
	}

	float m_w, m_h, m_d; 
};

class SimpleSystem {
public:
	SimpleSystem();
	
	Vector3F * X() const;
	const unsigned numFaceVertices() const;
	unsigned * indices() const;
	
	Vector3F * groundX() const;
	const unsigned numGroundFaceVertices() const;
	unsigned * groundIndices() const;
	
	Vector3F * Vline() const;
	const unsigned numVlineVertices() const;
	unsigned * vlineIndices() const;
	
	void setDrawer(KdTreeDrawer * d);
	
	void progress();
	
	RigidBody * rb();
	RigidBody * ground();
private:
	void applyGravity();
	void applyImpulse();
	void applyVelocity();
	void continuousCollisionDetection(const RigidBody & A, const RigidBody & B);
private:
	RigidBody m_rb;
	RigidBody m_ground;
	GjkContactSolver m_gjk;
	ContinuousCollisionContext m_ccd;
	Vector3F * m_X;
	unsigned * m_indices;
	
	Vector3F * m_V;
	Vector3F * m_Vline;
	unsigned * m_vIndices;
	
	Vector3F * m_groundX;
	unsigned * m_groundIndices;
};
#endif        //  #ifndef SIMPLESYSTEM_H
