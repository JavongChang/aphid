/*
 *  Matrix33F.cpp
 *
 *  Created by jian zhang on 10/30/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <cmath>
#include "Matrix33F.h"
#include <iostream>

Matrix33F Matrix33F::IdentityMatrix;

Matrix33F::Matrix33F() 
{
	setIdentity();
}

Matrix33F::Matrix33F(const Matrix33F & a)
{
	for(int i = 0; i < 9; i++) v[i] = a.v[i];
}

Matrix33F::Matrix33F(const Vector3F& r0, const Vector3F & r1, const Vector3F & r2)
{
    fill(r0, r1, r2);
}

Matrix33F::~Matrix33F() {}

float Matrix33F::operator() (int i, int j)
{
	return v[i * 3 + j];
}

float Matrix33F::operator() (int i, int j) const
{
	return v[i * 3 + j];
}

Vector3F Matrix33F::operator*( Vector3F other ) const
{
	Vector3F v;
	v.x = M(0, 0) * other.x + M(0, 1) * other.y + M(0, 2) * other.z;
	v.y = M(1, 0) * other.x + M(1, 1) * other.y + M(1, 2) * other.z;
	v.z = M(2, 0) * other.x + M(2, 1) * other.y + M(2, 2) * other.z;
	
	return v;
}

Matrix33F Matrix33F::operator*( Matrix33F other ) const
{
    return multiply(other);
}

Matrix33F Matrix33F::operator*( float scaling ) const
{
    Matrix33F t(*this);
    t *= scaling;
    return t;
}

Matrix33F Matrix33F::operator+( Matrix33F other ) const
{
	Matrix33F a;
	*a.m(0, 0) = M(0,0) + other.M(0,0);
	*a.m(0, 1) = M(0,1) + other.M(0,1);
	*a.m(0, 2) = M(0,2) + other.M(0,2);
	*a.m(1, 0) = M(1,0) + other.M(1,0);
	*a.m(1, 1) = M(1,1) + other.M(1,1);
	*a.m(1, 2) = M(1,2) + other.M(1,2);
	*a.m(2, 0) = M(2,0) + other.M(2,0);
	*a.m(2, 1) = M(2,1) + other.M(2,1);
	*a.m(2, 2) = M(2,2) + other.M(2,2);
	return a;
}

void Matrix33F::operator*= (float scaling)
{
    for(int i = 0; i < 9; i++) v[i] *= scaling;
}

void Matrix33F::operator+=(Matrix33F other)
{
    for(int i = 0; i < 9; i++) v[i] += other.v[i];
}

float* Matrix33F::m(int i, int j)
{
	return &v[i * 3 + j];
}

float Matrix33F::M(int i, int j) const
{
	return v[i * 3 + j];
}

void Matrix33F::setIdentity()
{
	*m(0, 0) = *m(1, 1) = *m(2, 2) = 1.0f;
	*m(0, 1) = *m(0, 2) = *m(1, 0) = *m(1, 2) = *m(2, 0) = *m(2, 1) = 0.0f;
}

void Matrix33F::setZero()
{
	for(int i = 0; i < 9; i++) v[i] = 0.f;
}

void Matrix33F::fill(const Vector3F& a, const Vector3F& b, const Vector3F& c)
{
	*m(0, 0) = a.x;
	*m(0, 1) = a.y;
	*m(0, 2) = a.z;
	*m(1, 0) = b.x;
	*m(1, 1) = b.y;
	*m(1, 2) = b.z;
	*m(2, 0) = c.x;
	*m(2, 1) = c.y;
	*m(2, 2) = c.z;
}

/*
   i
   00 01 02
   10 11 12
   20 21 22
           j
*/

void Matrix33F::transpose()
{
    float tmp;
    tmp = M(0, 1);
    *m(0, 1) = M(1, 0);
    *m(1, 0) = tmp;

    tmp = M(0, 2);
    *m(0, 2) = M(2, 0);
    *m(2, 0) = tmp;
    
    tmp = M(1, 2);
    *m(1, 2) = M(2, 1);
    *m(2, 1) = tmp;
}

void Matrix33F::multiply(const Matrix33F& a)
{
	Matrix33F t(*this);
	setZero();
	int i, j, k;
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			for(k = 0; k < 3; k++) {
				*m(i, j) += t.M(i, k) * a.M(k, j);
			}
		}
	}
}

Matrix33F Matrix33F::multiply(const Matrix33F & a) const
{
	Matrix33F t;
	t.setZero();
	int i, j, k;
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			for(k = 0; k < 3; k++) {
				*t.m(i, j) += M(i, k) * a.M(k, j);
			}
		}
	}
	return t;
}

Vector3F Matrix33F::transform(const Vector3F & a) const
{
	Vector3F b;
	b.x = a.x * M(0, 0) + a.y * M(1, 0) + a.z * M(2, 0);
	b.y = a.x * M(0, 1) + a.y * M(1, 1) + a.z * M(2, 1);
	b.z = a.x * M(0, 2) + a.y * M(1, 2) + a.z * M(2, 2);
	return b;
}

void Matrix33F::glMatrix(float m[16]) const
{
	m[0] = M(0,0); m[1] = M(0,1); m[2] = M(0,2); m[3] = 0.f;
    m[4] = M(1,0); m[5] = M(1,1); m[6] = M(1,2); m[7] = 0.f;
    m[8] = M(2,0); m[9] = M(2,1); m[10] =M(2,2); m[11] = 0.f;
    m[12] = m[13] = m[14] = 0.f; m[15] = 1.f;
}

/*
 *
 *  | a00 a01 a02 |-1             |   a22a11-a21a12  -(a22a01-a21a02)   a12a01-a11a02  |
 *  | a10 a11 a12 |    =  1/DET * | -(a22a10-a20a12)   a22a00-a20a02  -(a12a00-a10a02) |
 *  | a20 a21 a22 |               |   a21a10-a20a11  -(a21a00-a20a01)   a11a00-a10a01  |
 *
 *  with DET  =  a00(a22a11-a21a12)-a10(a22a01-a21a02)+a20(a12a01-a11a02)
 *
 */

void Matrix33F::inverse()
{
    const float det = determinant();
    
    *m(0, 0) =  determinant22(M(2, 2), M(1, 1), M(2, 1), M(1, 2)) / det;
    *m(0, 1) = -determinant22(M(2, 2), M(1, 0), M(2, 0), M(1, 2)) / det;
    *m(0, 2) =  determinant22(M(2, 1), M(1, 0), M(2, 0), M(1, 1)) / det;
	
	*m(1, 0) = -determinant22(M(2, 2), M(0, 1), M(2, 1), M(0, 2)) / det;
	*m(1, 1) =  determinant22(M(2, 2), M(0, 0), M(2, 0), M(0, 2)) / det;
	*m(1, 2) = -determinant22(M(2, 1), M(0, 0), M(2, 0), M(0, 1)) / det;
	
	*m(2, 0) =  determinant22(M(1, 2), M(0, 1), M(1, 1), M(0, 2)) / det;
	*m(2, 1) = -determinant22(M(1, 2), M(0, 0), M(1, 0), M(0, 2)) / det;
	*m(2, 2) =  determinant22(M(1, 1), M(0, 0), M(1, 0), M(0, 1)) / det;
	
	transpose();
}

float Matrix33F::determinant() const
{
    return M(0, 0) * determinant22(M(2, 2), M(1, 1), M(2, 1), M(1, 2)) - M(1, 0) * determinant22(M(2, 2), M(0, 1), M(2, 1), M(0, 2)) + M(2, 0) * determinant22(M(1, 2), M(0, 1), M(1, 1), M(0, 2));
}

float Matrix33F::determinant22(float a, float b, float c, float d) const
{
    return a * b - c * d;
}

void Matrix33F::rotateX(float alpha)
{
	const float c = cos(alpha);
	const float s = sin(alpha);
	Matrix33F r;
	*r.m(1, 1) =  c; *r.m(1, 2) = s;
	*r.m(2, 1) = -s; *r.m(2, 2) = c;
	multiply(r);
}

void Matrix33F::rotateY(float beta)
{
	const float c = cos(beta);
	const float s = sin(beta);
	Matrix33F r;
	*r.m(0, 0) = c; *r.m(0, 2) = -s;
	*r.m(2, 0) = s; *r.m(2, 2) = c;
	multiply(r);
}

void Matrix33F::rotateZ(float gamma)
{
	const float c = cos(gamma);
	const float s = sin(gamma);
	Matrix33F r;
	*r.m(0, 0) =  c; *r.m(0, 1) = s;
	*r.m(1, 0) = -s; *r.m(1, 1) = c;
	multiply(r);
}

void Matrix33F::rotateEuler(float phi, float theta, float psi, RotateOrder order)
{
	Matrix33F B;
	const float cpsi = cos(phi);
	const float spsi = sin(phi);
	*B.m(1, 1) = cpsi;
	*B.m(1, 2) = spsi;
	*B.m(2, 1) = -spsi;
	*B.m(2, 2) = cpsi;
	
	const float ctheta = cos(theta);
	const float stheta = sin(theta);
	Matrix33F C;
	*C.m(0, 0) = ctheta;
	*C.m(0, 2) = -stheta;
	*C.m(2, 0) = stheta;
	*C.m(2, 2) = ctheta;
	
	const float cphi = cos(psi);
	const float sphi = sin(psi);
	Matrix33F D;
	*D.m(0, 0) = cphi;
	*D.m(0, 1) = sphi;
	*D.m(1, 0) = -sphi;
	*D.m(1, 1) = cphi;
	
	if(order == XYZ) {
		multiply(B);
		multiply(C);
		multiply(D);
	}
	else {
		multiply(D);
		multiply(C);
		multiply(B);
	}
}

Vector3F Matrix33F::scale() const
{
	Vector3F vx(M(0, 0), M(0, 1), M(0, 2));
	Vector3F vy(M(1, 0), M(1, 1), M(1, 2));
	Vector3F vz(M(2, 0), M(2, 1), M(2, 2));
	return Vector3F(vx.length(), vy.length(), vz.length());
}

void Matrix33F::orthoNormalize()
{
    Vector3F r0(M(0, 0), M(0, 1), M(0, 2));
    Vector3F r1(M(1, 0), M(1, 1), M(1, 2));
    Vector3F r2(M(2, 0), M(2, 1), M(2, 2));
    
    float l0 = r0.length();
    if(l0 > 0.f) r0 /= l0;
    
    r1 -= r0 * r0.dot(r1);
    float l1 = r1.length();
    if(l1 > 0.f) r1 /= l1;
    
    r2 = r0.cross(r1);
    fill(r0, r1, r2);
}

void Matrix33F::set(const Quaternion & q)
{
    const float xx = q.x * q.x;
	const float yy = q.y * q.y;
	const float zz = q.z * q.z;
	*m(0, 0) = 1.f - 2.f * yy - 2.f * zz; 
	*m(0, 1) = 2.f * q.x * q.y - 2.f * q.w * q.z; 
	*m(0, 2) = 2.f * q.x * q.z + 2.f * q.w * q.y;  
	*m(1, 0) = 2.f * q.x * q.y + 2.f * q.w * q.z; 
	*m(1, 1) = 1.f - 2.f * xx - 2.f * zz; 
	*m(1, 2) = 2.f * q.y * q.z - 2.f * q.w * q.x;  
	*m(2, 0) = 2.f * q.x * q.z - 2.f * q.w * q.y; 
	*m(2, 1) = 2.f * q.y * q.z + 2.f * q.w * q.x; 
	*m(2, 2) = 1.f - 2.f * xx - 2.f * yy;
}

const std::string Matrix33F::str() const
{
	std::stringstream sst;
	sst.str("");
    sst<<"["<<v[0]<<" "<<v[1]<<" "<<v[2]<<"]\n";
    sst<<"["<<v[3]<<" "<<v[4]<<" "<<v[5]<<"]\n";
	sst<<"["<<v[6]<<" "<<v[7]<<" "<<v[8]<<"]\n";
	
	return sst.str();
}

