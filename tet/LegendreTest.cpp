/*
 *  LegendreTest.cpp
 *  foo
 *
 *  http://mathfaculty.fullerton.edu/mathews/n2003/LegendrePolyMod.html
 *  http://faculty.washington.edu/finlayso/ebook/bvp/OC/Legendre.htm
 *  http://www.mhtlab.uwaterloo.ca/courses/me755/web_chap5.pdf
 *  http://archive.lib.msu.edu/crcmath/math/math/l/l175.htm
 *  http://web.cs.iastate.edu/~cs577/handouts/orthogonal-polys.pdf
 *  
 *  Created by jian zhang on 7/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "LegendreTest.h"
#include <GridTables.h>
#include <Calculus.h>
#include <iostream>
#include <iomanip>

using namespace aphid;
namespace ttg {

const float LegendreTest::NormWeightF[11] = {
1.f,
1.f,
.6666667f,
.4f,
.22857143f,
.12698413f,
.06926407f,
.037296037f,
.01989122f,
.01053064f,
.005542445f
};

LegendreTest::LegendreTest() 
{}

LegendreTest::~LegendreTest() 
{}
	
const char * LegendreTest::titleStr() const
{ return "Legendre Polynomial Approximation Test"; }

float LegendreTest::evaluateExact(const float & x) const
{ 
	//return 10.f - x* 2.f + x * x / 10.f;
	//return x*x*x*x - x*x * 2 / 3 + 1.0 / 9.0;
	//return x*x*x - x * 3.0 / 5.0;
	//return x*x*x*x*x*x - x*x*x*x * 6.0 / 5.0 + x*x * 9.0 / 25.0;
	//return (x*x - 1.0/3.0);
	//return exp(x) * (x*x - 1.0 / 3.0);
	//return exp(x) * (x*x*x - 3.0 *x / 5.0);
	//return 1.1f + exp(x*.5) * cos(x*3); 
	return exp(x); 
}

bool LegendreTest::init()
{
	int i;
	for(i=0;i<M_NUM_EVAL;++i) {
		m_exactEvaluate[i] = evaluateExact(INTERVAL_A + DX_SAMPLE * i);
	}
	
	//std::cout<<"\n test integral "<<calc::trapezIntegral(INTERVAL_A, INTERVAL_B, M_NUM_EVAL, m_exactEvaluate);
	
	computeLegendrePolynomials();
	computePipi();
	
	int m = 7, n = 6;
	std::cout<<"\n deg "<<n<<" sample "<<m;
	
	const float dx = (INTERVAL_B - INTERVAL_A) / (float)(m-1);
	
	float * X = new float[m];
	float * Y = new float[m];
	float * Px = new float[m];
	
	for(i=0;i<m;++i) {
		X[i] = INTERVAL_A + dx * i;
		Y[i] = evaluateExact(X[i]);
	}
	
	printValues("X", m, X);
	printValues("Y", m, Y);
	
	for(i=0;i<m;++i) {
		Px[i] = samplePolyValue(0, X[i]);
	}
	printValues("P0(x)", m, Px);
	
	for(i=0;i<m;++i) {
		Px[i] = samplePolyValue(1, X[i]);
	}
	printValues("P1(x)", m, Px);
	
	for(i=0;i<m;++i) {
		Px[i] = samplePolyValue(2, X[i]);
	}
	printValues("P2(x)", m, Px);
	
	for(i=0;i<m;++i) {
		Px[i] = samplePolyValue(3, X[i]);
	}
	printValues("P3(x)", m, Px);
		
/// interpolate y through out samples
	for(i=0;i<M_NUM_EVAL;++i) {
		m_approximateEvaluate[i] = calc::interpolate(m, Y, M_NUM_EVAL, INTERVAL_A + DX_SAMPLE * i,
										INTERVAL_A, INTERVAL_B, DX_SAMPLE);
	}

	computeCoeff(m_coeff, n, m_approximateEvaluate, m_pipi);

	computeApproximated(m_approximateEvaluate, n, m_coeff);
	
	delete[] X;
	delete[] Y;
	delete[] Px;
	std::cout<<"\n done!";
	std::cout.flush();
	return true;
}

float LegendreTest::samplePolyValue(int i, float x) const
{
	int j = (x - INTERVAL_A) / DX_SAMPLE;
	return m_v[M_NUM_EVAL * i + j];
}

void LegendreTest::computeLegendrePolynomials()
{
	calc::legendreRules(M_NUM_EVAL, POLY_MAX_DEG, m_v, INTERVAL_A, INTERVAL_B);
}

float LegendreTest::sampleAt(int i) const
{ return INTERVAL_A + DX_SAMPLE * i; }

void LegendreTest::computePipi()
{
	float px2[M_NUM_EVAL];
	int i, j;
	for(i=0; i<=POLY_MAX_DEG; ++i) {
		for(j=0;j<M_NUM_EVAL; ++j) {
			px2[j] = m_v[M_NUM_EVAL * i + j]  * NormWeightF[i]; /// normalize
			px2[j] *= px2[j]; /// <pi, pi>
		}
		
		m_pipi[i] = calc::trapezIntegral(INTERVAL_A, INTERVAL_B, M_NUM_EVAL, px2);
			
		std::cout<<"\n <p"<<i<<",p"<<i<<"> "<<m_pipi[i];
	}
	
#if 0
	std::cout<<"\n test orthogonality";
	
	for(j=0;j<M_NUM_EVAL; ++j) {
			px2[j] = m_v[M_NUM_EVAL * 1 + j]; /// normalize
			px2[j] *= m_v[M_NUM_EVAL * 2 + j]; /// <pi, pi>
	}
	
	std::cout<<"\n <p2,p1> "<<calc::trapezIntegral(INTERVAL_A, INTERVAL_B, M_NUM_EVAL, px2);
#endif

}

void LegendreTest::computeCoeff(float * coeff, int n, const float * y,
						const float * pipi) const
{
	float Px[M_NUM_EVAL];
	
	int i, j;
	for(i=0; i<=n; ++i) {
		for(j=0;j<M_NUM_EVAL;++j) {
			Px[j] = m_v[M_NUM_EVAL * i + j] * NormWeightF[i];
			Px[j] *= y[j];
		}
			
		coeff[i] = calc::trapezIntegral(INTERVAL_A, INTERVAL_B, M_NUM_EVAL, Px);
		
		//std::cout<<"\n c"<<i<<" "<<coeff[i];
		coeff[i] /= pipi[i];
		
		std::cout<<"  c"<<i<<" /<pi,pi> "<<coeff[i];
	}
}

void LegendreTest::computeApproximated(float * yhat, int n, const float * coeff) const
{
	int i, j;
	for(i=0;i<M_NUM_EVAL;++i) {
		yhat[i] = 0.f;
		for(j=0; j<=n; ++j) {
			yhat[i] += coeff[j] * m_v[j*M_NUM_EVAL + i] * NormWeightF[j];
		}
	}
}

void LegendreTest::computePix(float * m_v, float * pipi, float * xpipi,
					int m, int n, const float * x) const
{
	int i, k;
	
	float pm1 = 0.f, pm2 = 0.f;
	for(k=0; k<=m;++k) {
		for(i=0; i<n; ++i) {
			if(k>1) {
				pm1 = m_v[i*(m+1) + k - 1];
				pm2 = m_v[i*(m+1) + k - 2];
			}
			
			m_v[i*(m+1) + k] = computePi(k, pipi, xpipi, x[i], pm1, pm2);
			
			std::cout<<"\n p"<<k<<"(x)["<<i<<"] "<<m_v[i*(m+1) + k];
			
		}
		
		pipi[k] = integratePipi(m_v, k, m, n);
		//xpipi[k] = integrateXpipi(m_v, m_x, k, m, n);
		
		std::cout<<"\n <p"<<k<<",p"<<k<<"> "<<pipi[k]
				<<"\n <xp"<<k<<",p"<<k<<"> "<<xpipi[k];
		
	}
}

float LegendreTest::computePi(int k, const float * pipi, const float * xpipi, const float & x,
								const float & pm1, const float & pm2) const
{
	if(k==0) 
		return 1.f;
		
	if(k==1)
		return x - xpipi[0] / pipi[0];
/// orthogonal polynomial sequence		
	return (x - xpipi[k-1] / pipi[k-1] ) * pm1 - pipi[k-1] / pipi[k-2] * pm2;
	
}

float LegendreTest::integratePipi(const float * v, int k, int m, int n) const
{
	float c = 0.f;
	int i = 0;
	for(;i<n;++i)
		c += v[i*(m+1) + k] * v[i*(m+1) + k];
		
	return c;
}

float LegendreTest::integrateXpipi(const float * v, const float * x, int k, int m, int n) const
{
	float c = 0.f;
	int i = 0;
	for(;i<n;++i)
		c += x[i] * v[i*(m+1) + k] * v[i*(m+1) + k];
		
	return c;
}

void LegendreTest::equalSpacingNodes(int n, float * x) const
{
	float dx = (INTERVAL_B - INTERVAL_A) / (float)(n-1);
	int i=0;
	for(;i<n;++i)
		x[i] = INTERVAL_A + dx * i;
}

void LegendreTest::draw(GeoDrawer * dr)
{
	glColor3f(0.f, 0.f, 0.f);
	drawEvaluate(m_exactEvaluate, dr);
	
	glColor3f(0.33f, 1.f, 0.33f);
	drawEvaluate(m_approximateEvaluate, dr);

#if 1
	Vector3F mcol;
	int i=0;
	for(;i<=POLY_MAX_DEG;++i) {
		sdb::gdt::GetCellColor(mcol, i);
		glColor3fv((const float *)&mcol);
		drawLegendrePoly(i, dr);
	}
#endif
}

void LegendreTest::printValues(const char * h, int n, const float * y) const
{
	std::cout<<"\n "<<std::setw(8)<<h;
	int i=0;
	for(;i<n;++i)
		std::cout<<" "<<std::setw(12) <<std::setprecision(8)<<y[i];
}

float LegendreTest::discreteIntegral(int n, const float * y, const float * v) const
{
	float dx = 1.f / (float)n;
	int i=0;
	float c = 0.f;
	for(;i<n;++i)
		c += dx * y[i];
		
	return c;
}

void LegendreTest::drawEvaluate(const float * y, GeoDrawer * dr)
{
	glPushMatrix();
	glScaled(15.0, 10.0, 10.0);
	glBegin(GL_LINES);
	int i;
	for(i=0;i<M_NUM_EVAL-1;++i) {
		glVertex3f(INTERVAL_A + DX_SAMPLE * i, y[i], 0.f);
		glVertex3f(INTERVAL_A + DX_SAMPLE * (i+1), y[i+1], 0.f);
	}
	glEnd();
	glPopMatrix();
}

void LegendreTest::drawLegendrePoly(int m, GeoDrawer * dr)
{
	glPushMatrix();
	glScaled(15.0, 10.0, 10.0);
	glBegin(GL_LINES);
	int i;
	for(i=0;i<M_NUM_EVAL-1;++i) {
		glVertex3f(INTERVAL_A + DX_SAMPLE * i, m_v[M_NUM_EVAL*m + i ], 0.f);
		glVertex3f(INTERVAL_A + DX_SAMPLE * (i+1), m_v[M_NUM_EVAL*m + i +1], 0.f);
	}
	glEnd();
	glPopMatrix();
}

}