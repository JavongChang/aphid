#include <bvh_common.h>
#include "bvh_math.cuh"
__global__ void computeMass_kernel(float * dst,
                float * mass0,
                uint * anchored,
                float scale,
                uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	float m0 = mass0[ind];
	if(anchored[ind] == 0) dst[ind] = m0 * scale;
}

__global__ void integrate_kernel(float3 * pos, 
								float3 * vel,
                                float3 * vela,
								float dt, 
								uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
    float3 anchoredVel = vela[ind];
    vel[ind] = anchoredVel;
	float3_add_inplace(pos[ind], scale_float3_by(anchoredVel, dt));
}

__global__ void integrate_kernel1(float3 * pos, 
								float3 * vel,
                                float3 * anchoredVel,
								uint * anchor,
								float dt, 
								uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
    float3 va = anchoredVel[ind];
    if(anchor[ind] > 0) vel[ind] = va;
	else va = vel[ind];
	float3_add_inplace(pos[ind], scale_float3_by(va, dt));
}

namespace masssystem {
void computeMass(float * dst,
                float * mass0,
                uint * anchored,
                float scale,
                uint maxInd)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(maxInd, 512);
    dim3 grid(nblk, 1, 1);
    
    computeMass_kernel<<< grid, block >>>(dst,
        mass0,
        anchored,
        scale,
        maxInd);
}

void integrate(float3 * pos, 
								float3 * vel, 
                                float3 * anchoredVel,
								uint * anchor,
								float dt, 
								uint maxInd)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(maxInd, 512);
    dim3 grid(nblk, 1, 1);
    
    integrate_kernel1<<< grid, block >>>(pos,
        vel,
        anchoredVel,
        anchor,
        dt,
        maxInd);
}

void integrateAllAnchored(float3 * pos,
                    float3 * vel,
                    float3 * vela,
                    float dt,
                    uint maxInd)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(maxInd, 512);
    dim3 grid(nblk, 1, 1);
    
    integrate_kernel<<< grid, block >>>(pos,
        vel,
        vela,
        dt,
        maxInd);
}

}
