#ifndef BVH_COMMON_H
#define BVH_COMMON_H

/*
 *  bvh_common.h
 *  
 *
 *  Created by jian zhang on 1/9/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <cuda_runtime_api.h>
typedef unsigned int uint;
#ifdef __APPLE__
typedef unsigned long long uint64;
#else
typedef unsigned long uint64;
#endif

static uint iDivUp(uint dividend, uint divisor)
{
    return ( (dividend % divisor) == 0 ) ? (dividend / divisor) : (dividend / divisor + 1);
}

static uint nextPow2( uint x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

static bool isPow2(unsigned int x)
{
    return ((x&(x-1))==0);
}

struct Aabb {
    float3 low;
    float3 high;
};

struct EdgeContact {
    uint v[4];   
};

#define MAX_INDEX 999999999
#define TINY_VALUE 10e-6

#endif        //  #ifndef BVH_COMMON_H

