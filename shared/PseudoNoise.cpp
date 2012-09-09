/*
 *  PseudoNoise.cpp
 *  hair
 *
 *  Created by jian zhang on 5/13/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "PseudoNoise.h"
#include <math.h>

unsigned int PseudoNoise::xRand;
unsigned int PseudoNoise::yRand;
unsigned int PseudoNoise::zRand;

void PseudoNoise::seedd(unsigned int nx, unsigned int ny, unsigned int nz)
{
	xRand = nx;
    yRand = ny;
    zRand = nz;
}

unsigned PseudoNoise::randomize()
{
	unsigned a = xRand/1393137, b = xRand%1393137;
    unsigned c = yRand/1393149, d = yRand%1393149;
    unsigned e = zRand/1393151, f = zRand%1393151;

    xRand = (103 * b + 149 * a) % 1395253;
    yRand = (161 * d + 123  * c) % 1396313;
    zRand = (109 * f + 157 * e) % 1395329;
	
	return xRand + yRand + zRand;
}

float PseudoNoise::rfloat( int i )
{
	unsigned int seed = i;
	seedd( seed, seed * 3, seed * 131 );
	randomize();
	randomize();
	randomize();	
	float result;
    result = (double) xRand/390253.0 + (double) yRand/390299.0 + (double) zRand/390313.0;
    return result - ((int) result);
}

int PseudoNoise::rint( int i )
{
	unsigned int seed = i;
		
	seedd( seed, seed * 3, seed * 131 ); 
	randomize();
	
	return xRand + yRand + zRand;
}

void PseudoNoise::sphereRand(float& x, float& y, float& z, float r, unsigned int &i)
{
	unsigned int seed = i;
		
	seedd( seed, seed * 13, seed * 61 );
	randomize();
	randomize();
	randomize();
	
	float rd = (double) xRand/390253.0 + (double) yRand/390299.0 + (double) zRand/390313.0;
	rd = rd - ((int)rd);
	
	float theta = rd * 2.f * 3.1415927f;
	
	seedd( seed, seed * 23, seed * 41 );
	randomize();
	randomize();
	
	rd = (double) xRand/390253.0 + (double) yRand/390299.0 + (double) zRand/390313.0;
	rd = rd - ((int)rd);
	
	float u = rd * 2.f - 1.0f;
	
	x = cos(theta) * sqrt(1.0-(u*u)) * r;
	y = sin(theta) * sqrt(1.0-(u*u)) * r;
	z = u * r;
	
	seedd( seed, seed * 13, seed * 29 );
	randomize();
	randomize();
	
	rd = (double) xRand/390253.0 + (double) yRand/390299.0 + (double) zRand/390313.0;
	rd = rd - ((int)rd);
	
	x *= rd; y *= rd; z *= rd;
}
