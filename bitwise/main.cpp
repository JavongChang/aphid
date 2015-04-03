/*
 *  main.cpp
 *  
 *
 *  Created by jian zhang on 1/11/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <map>
#include <boost/format.hpp>
#ifdef __APPLE__
typedef unsigned long long uint64;
#else
typedef unsigned long uint64;
#endif

typedef unsigned int uint;

#define min(a, b) (a < b?a: b)
#define max(a, b) (a > b?a: b)

uint64 upsample(uint a, uint b) 
{ return ((uint64)a << 32) | (uint64)b; }

uint expandBits(uint v) 
{ 
    v = (v * 0x00010001u) & 0xFF0000FFu; 
    v = (v * 0x00000101u) & 0x0F00F00Fu; 
    v = (v * 0x00000011u) & 0xC30C30C3u; 
    v = (v * 0x00000005u) & 0x49249249u; 
    return v; 
}

// Calculates a 30-bit Morton code for the 
// given 3D point located within the unit cube [0,1].
uint morton3D(uint x, uint y, uint z) 
{ 
    x = min(max(x, 0.0f), 1023); 
    y = min(max(y, 0.0f), 1023); 
    z = min(max(z, 0.0f), 1023); 
    uint xx = expandBits((uint)x); 
    uint yy = expandBits((uint)y); 
    uint zz = expandBits((uint)z); 
    return xx * 4 + yy * 2 + zz; 
}

const char *byte_to_binary(uint x)
{
    static char b[33];
    b[32] = '\0';

    for (int z = 0; z < 32; z++) {
        b[31-z] = ((x>>z) & 0x1) ? '1' : '0';
    }

    return b;
}

const char *byte_to_binary64(uint64 x)
{
    static char b[65];
    b[64] = '\0';

    for (int z = 0; z < 64; z++) {
        b[63-z] = ((x>>z) & 0x1) ? '1' : '0';
    }

    return b;
}

// the number, between 0 and 64 inclusive, of consecutive zero bits 
// starting at the most significant bit (i.e. bit 63) of 64-bit integer parameter x. 
int clz(uint64 x)
{
	static char b[64];
	int z;
    for (z = 0; z < 64; z++) {
        b[63-z] = ((x>>z) & 0x1) ? '1' : '0';
    }

	int nzero = 0;
	for (z = 0; z < 64; z++) {
		if(b[z] == '0') nzero++;
		else break;
    }
	return nzero;
}

struct Stripe {
    char rgba[2048]; // 256 * 4 * 2
    float z[256];
};

// decoding morton code to cartesian coordinate
// https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/

uint Compact1By2(uint x)
{
  x &= 0x09249249;                  // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
  x = (x ^ (x >>  2)) & 0x030c30c3; // x = ---- --98 ---- 76-- --54 ---- 32-- --10
  x = (x ^ (x >>  4)) & 0x0300f00f; // x = ---- --98 ---- ---- 7654 ---- ---- 3210
  x = (x ^ (x >>  8)) & 0xff0000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
  x = (x ^ (x >> 16)) & 0x000003ff; // x = ---- ---- ---- ---- ---- --98 7654 3210
  return x;
}

uint DecodeMorton3Z(uint code)
{
  return Compact1By2(code >> 0);
}

uint DecodeMorton3Y(uint code)
{
  return Compact1By2(code >> 1);
}

uint DecodeMorton3X(uint code)
{
  return Compact1By2(code >> 2);
}

int main(int argc, char * const argv[])
{
	std::cout<<"bitwise test\n";
	std::cout<<"bit size of uint:   "<<sizeof(uint) * 8<<"\n";
	std::cout<<"bit size of uint64: "<<sizeof(uint64) * 8<<"\n";
	
	int x0 = 1023, y0 = 0, z0 = 8;
	int x1 = 900, y1 = 116, z1 = 7;
	uint m0 = morton3D(x0, y0, z0);
	uint m1 = morton3D(x1, y1, z1);
	uint64 u0 = upsample(m0, 8);
	uint64 u1 = upsample(m1, 9);
	std::cout<<boost::format("morton code of (%1%, %2%, %3%): %4%\n") % x0 % y0 % z0 % byte_to_binary(m0);
	std::cout<<boost::format("morton code of (%1%, %2%, %3%): %4%\n") % x1 % y1 % z1 % byte_to_binary(m1);
	std::cout<<boost::format("upsample m0: %1%\n") % byte_to_binary64(u0);
	std::cout<<boost::format("upsample m1: %1%\n") % byte_to_binary64(u1);
	std::cout<<boost::format("upsample m0 ^ upsample m1: %1%\n") % byte_to_binary64(u0 ^ u1);
	std::cout<<boost::format("common prefix length: %1%\n") % clz(u0 ^ u1);
	uint64 bitMask = ((uint64)(~0)) << (64 - clz(u0 ^ u1));
	std::cout<<boost::format("bit mask: %1%\n") % byte_to_binary64(bitMask);
	uint64 sharedBits = u0 & u1;
	std::cout<<boost::format("shared bits: %1%\n") % byte_to_binary64(sharedBits);
	std::cout<<boost::format("common prefix: %1%\n") % byte_to_binary64(sharedBits & bitMask);
	std::cout<<boost::format("mask 0x80000000: %1%\n") % byte_to_binary(0x80000000);
	std::cout<<boost::format("~mask ~0x80000000: %1%\n") % byte_to_binary(~0x80000000);
	std::cout<<boost::format("8 masked: %1%\n") % byte_to_binary(8 | 0x80000000);
	std::cout<<boost::format("8 unmasked: %1%\n") % byte_to_binary(8 & (~0x80000000));
	std::cout<<boost::format("12: %1%\n") % byte_to_binary(12);
	std::cout<<boost::format("12 left 24 masked: %1%\n") % byte_to_binary(12<<24 | 0x80000000);
	std::cout<<boost::format("12 right 24 unmasked: %1%\n") % byte_to_binary(((12<<24 | 0x80000000)&(~0x80000000))>>24);
	
	std::cout<<boost::format("12 left 24 with 1923 masked: %1%\n") % byte_to_binary((12<<24 | 1923) | 0x80000000);
	std::cout<<boost::format("12 right 24 with 1923 unmasked: %1%\n") % byte_to_binary((((12<<24 | 1923) | 0x80000000)&(~0x80000000))>>24);
	std::cout<<boost::format("id masked: %1%\n") % byte_to_binary(~0x800000);
	std::cout<<boost::format("1923: %1%\n") % byte_to_binary(1923);
	std::cout<<boost::format("<<7 >>7: %1%\n") % byte_to_binary((((12<<24 | 1923) | 0x80000000)<<7)>>7);
	
	std::cout<<boost::format("sizeof Stripe: %1%\n") % sizeof(std::map<int, Stripe *>);
	
	int a[8] = {99, 83, -73, -32, 48, 1, 53, -192};
	int b[4];
	memcpy(b, &a[4], 4 * 4);

	int i;
	for(i=0; i<4; i++) {
	    std::cout<<boost::format("b[%1%] %2%\n") % i % b[i]; 
	}
	
	float c[4];
	for(i=0; i<4; i++) {
	    std::cout<<boost::format("c[%1%] %2%\n") % i % c[i]; 
	}
// If n is a power of 2, 
// (i/n) is equivalent to (i>>log2(n)) 
// and (i%n) is equivalent to (i&(n-1)); 	
	std::cout<<boost::format("1023 & 255 %1%\n") % (1023 & 255);
	std::cout<<boost::format("1024 & 255 %1%\n") % (1024 & 255);
	std::cout<<boost::format("1023>>10 %1%\n") % (1023>>10);
	std::cout<<boost::format("3<<2 %1%\n") % (3<<2);
	std::cout<<boost::format("5>>2 %1%\n") % (5>>2);
	std::cout<<boost::format("513 / 256 %1%\n") % ((513>>8) + ((513 & 255) != 0));
	
	x1 = 99, y1 = 736, z1 = 121;
	m1 = morton3D(x1, y1, z1);
	
	std::cout<<boost::format("morton code of (%1%, %2%, %3%): %4%\n") % x1 % y1 % z1 % byte_to_binary(m1);	
	std::cout<<boost::format("decode morton code to xyz: (%1%, %2%, %3%)\n") % DecodeMorton3X(m1) % DecodeMorton3Y(m1) % DecodeMorton3Z(m1);
		
	std::cout<<"end of test\n";
	return 0;
}