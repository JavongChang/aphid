/*
 *  Suspension.h
 *  wheeled
 *
 *  Created by jian zhang on 5/31/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "Damper.h"
#include "Wheel.h"
namespace caterpillar {
class Suspension {
public:
	class Profile {
	public:
		Profile();		
		float _upperWishboneAngle[2];
		float _lowerWishboneAngle[2];
		float _wheelHubX;
		float _wheelHubR;
		float _upperJointY, _lowerJointY;
		float _steerArmJointZ;
		float _upperWishboneLength, _lowerWishboneLength;
		float _upperWishboneTilt, _lowerWishboneTilt;
		float _damperY;
		bool _steerable, _powered;
	};
	
	Suspension();
	virtual ~Suspension();
	void setProfile(const Profile & info);
	const float width() const;
	const float wheelHubX() const;
	const bool isPowered() const;
	const bool isSteerable() const;
	
	btRigidBody* create(const Vector3F & pos, bool isLeft = true);
	void connectWheel(Wheel * wheel, bool isLeft);
	
	void update();
	void steerAndDrive(const Vector3F & turnAround, const float & turnAngle, const float & wheelSpan, const Vector3F & targetVelocity, const float & wheelR, bool goForward = true);
	
	static float RodRadius;
	static btRigidBody * ChassisBody;
	static Vector3F ChassisOrigin;

private:
	btRigidBody* createCarrier(const Matrix44F & tm, bool isLeft);
	btRigidBody* createWishbone(btRigidBody* carrier, const Matrix44F & tm, bool isUpper, bool isLeft);
	btRigidBody* createSteeringArm(btRigidBody* carrier, const Matrix44F & tm, bool isLeft);
	btRigidBody* createDamper(btRigidBody * lowerArm, const Matrix44F & tm, bool isLeft);
	btRigidBody* createSwayBar(const Matrix44F & tm, btRigidBody * arm, bool isLeft);
	void connectSwayBar(const Matrix44F & tm, btRigidBody * bar);
	btCompoundShape* createWishboneShape(bool isUpper, bool isLeft);
	const Matrix44F wishboneHingTMLocal(bool isUpper, bool isLeft, bool isFront) const;
	void wishboneLA(bool isUpper, bool isLeft, bool isFront, float & l, float & a) const;
	void connectArm(btRigidBody* arm, const Matrix44F & tm, bool isUpper, bool isLeft, bool isFront);
	void powerDrive(const float & ang, const float & wheelSpan, const Vector3F & targetVelocity, const float & wheelR, bool goForward = true);
	void steer(const Vector3F & around, const float & wheelSpan);
	void steerWheel(const float & ang, int i);
	void applyBrake(bool enable);
	void applyMotor(float rps, const int & i, float force);
	const Matrix44F wheelHubTM(const int & i) const;
	const Vector3F wheelVelocity(const int & i) const;
	float limitDrive(const int & i, const float & targetSpeed, const float & r, const float & differential = 0.f, bool goForward = true);
	Profile m_profile;
	btGeneric6DofConstraint* m_steerJoint[2];
	btGeneric6DofConstraint* m_driveJoint[2];
	btRigidBody * m_wheelHub[2];
	Wheel * m_wheel[2];
	btRigidBody * m_swayBarLeft;
	Damper * m_damper[2];
};
}