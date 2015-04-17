#include "cuFemTetrahedron_implement.h"
#include "cuFemMath.cu"

__global__ void internalForce_kernel(float3 * dst,
    float d16, float d17, float d18,
                                    float3 * pos,
                                    uint4 * tetvert,
                                    mat33 * orientation,
                                    KeyValuePair * tetraInd,
                                    uint * bufferIndices,
                                    uint maxBufferInd,
                                    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	float volume;
	float3 B[4];
	float3 pj, force, sum;
	mat33 Ke, Re;
	uint iTet, i, j;
	uint cur = bufferIndices[ind];
	uint lastTet = 94967295;
	for(;;) {
	    if(tetraInd[cur].key != ind) break;
	    
	    extractTetij(tetraInd[cur].value, iTet, i, j);
	    
	    if(lastTet != iTet) {
	        if(lastTet != 94967295) {
	            mat33_float3_prod(force, Re, sum);
	            float3_minus_inplace(dst[ind], force);
	        }
	        Re = orientation[iTet];
	        calculateBandVolume(B, volume, pos, tetvert[iTet]);
	        float3_set_zero(sum);
	        lastTet = iTet;
	    }

	    calculateKe(Ke, B, d16, d17, d18, volume, i, j);	    
	    
	    uint * tetv = &tetvert[iTet].x;
	    pj = pos[tetv[j]];
		
	    mat33_float3_prod(force, Ke, pj);	    
	    float3_add_inplace(sum, force);

	    cur++;
	    if(cur >= maxBufferInd) break;
	}
	
	mat33_float3_prod(force, Re, sum);
	float3_minus_inplace(dst[ind], force);
}

__global__ void resetForce_kernel(float3 * dst,
    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	float3_set_zero(dst[ind]);
}

__global__ void resetStiffnessMatrix_kernel(mat33* dst, 
                    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	set_mat33_zero(dst[ind]);
}

__global__ void stiffnessAssembly_kernel(mat33 * dst,
                                        float d16, float d17, float d18,
                                        float3 * pos,
                                        uint4 * tetv,
                                        mat33 * orientation,
                                        KeyValuePair * tetraInd,
                                        uint * bufferIndices,
                                        uint maxBufferInd,
                                        uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	float volume;
	float3 B[4];
	mat33 Ke, Re, ReT, tmp, tmpT;
	uint iTet, i, j;
	uint cur = bufferIndices[ind];
	for(;;) {
	    if(tetraInd[cur].key != ind) break;
	    
	    extractTetij(tetraInd[cur].value, iTet, i, j);
	    
	    calculateBandVolume(B, volume, pos, tetv[iTet]);
		
	    calculateKe(Ke, B, d16, d17, d18, volume, i, j);

	    Re = orientation[iTet];
	    mat33_transpose(ReT, Re);
	    
	    mat33_cpy(tmp, Re);
	    mat33_mult(tmp, Ke);
	    mat33_mult(tmp, ReT);
	        
	    mat33_add(dst[ind], tmp);
	    
	    if(j>i) {
	        mat33_transpose(tmpT, tmp);
	        mat33_add(dst[ind], tmpT);
	    }
	    
	    cur++;
	    if(cur >= maxBufferInd) break;
	}
}

__global__ void resetRe_kernel(mat33* dst, 
                    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	set_mat33_identity(dst[ind]);
}

__global__ void calculateRe_kernel(mat33 * dst, 
                                    float3 * pos, 
                                    float3 * pos0,
                                    uint4 * indices,
                                    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	uint4 & t = indices[ind];
	
	float3 pnt[4];
	tetrahedronP(pnt, pos0, t);
	float3 e01, e02, e03;
	tetrahedronEdge(e01, e02, e03, pnt); 
	
	float div6V = 1.f / tetrahedronVolume(e01, e02, e03) * 6.f;

	tetrahedronP(pnt, pos, t);
	float3 e1, e2, e3;
	tetrahedronEdge(e1, e2, e3, pnt); 
	float3 n1 = scale_float3_by(float3_cross(e2, e3), div6V);
	float3 n2 = scale_float3_by(float3_cross(e3, e1), div6V);
	float3 n3 = scale_float3_by(float3_cross(e1, e3), div6V);
	
	mat33 & Re = dst[ind];
	Re.v[0].x = e01.x * n1.x + e02.x * n2.x + e03.x * n3.x;  
	Re.v[1].x = e01.x * n1.y + e02.x * n2.y + e03.x * n3.y;   
	Re.v[2].x = e01.x * n1.z + e02.x * n2.z + e03.x * n3.z;

    Re.v[0].y = e01.y * n1.x + e02.y * n2.x + e03.y * n3.x;  
	Re.v[1].y = e01.y * n1.y + e02.y * n2.y + e03.y * n3.y;   
	Re.v[2].y = e01.y * n1.z + e02.y * n2.z + e03.y * n3.z;

    Re.v[0].z = e01.z * n1.x + e02.z * n2.x + e03.z * n3.x;  
	Re.v[1].z = e01.z * n1.y + e02.z * n2.y + e03.z * n3.y;  
	Re.v[2].z = e01.z * n1.z + e02.z * n2.z + e03.z * n3.z;
	
	mat33_orthoNormalize(Re);
}

extern "C" {
void cuFemTetrahedron_resetRe(mat33 * d, uint maxInd)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(maxInd, 512);
    dim3 grid(nblk, 1, 1);
    
    resetRe_kernel<<< grid, block >>>(d, maxInd);
}

void cuFemTetrahedron_calculateRe(mat33 * dst, 
                                    float3 * pos, 
                                    float3 * pos0,
                                    uint4 * indices,
                                    uint maxInd)
{
    int tpb = CudaBase::LimitNThreadPerBlock(24, 50);
    dim3 block(tpb, 1, 1);
    unsigned nblk = iDivUp(maxInd, tpb);
    dim3 grid(nblk, 1, 1);
    
    calculateRe_kernel<<< grid, block >>>(dst, 
                                       pos, 
                                       pos0,
                                       indices,
                                       maxInd);
}

void cuFemTetrahedron_resetStiffnessMatrix(mat33 * dst,
                                    uint maxInd)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(maxInd, 512);
    dim3 grid(nblk, 1, 1);
    
    resetStiffnessMatrix_kernel<<< grid, block >>>(dst, 
                                        maxInd);
}

void cuFemTetrahedron_stiffnessAssembly(mat33 * dst,
                                        float3 * pos,
                                        uint4 * vert,
                                        mat33 * orientation,
                                        KeyValuePair * tetraInd,
                                        uint * bufferIndices,
                                        uint maxBufferInd,
                                        uint maxInd)
{
    int tpb = CudaBase::LimitNThreadPerBlock(24, 50);
    dim3 block(tpb, 1, 1);
    unsigned nblk = iDivUp(maxInd, tpb);
    dim3 grid(nblk, 1, 1);
    
    stiffnessAssembly_kernel<<< grid, block >>>(dst,
        1.f, 2.f, 3.f,
                                            pos,
                                            vert,
                                            orientation,
                                            tetraInd,
                                            bufferIndices,
                                            maxBufferInd,
                                            maxInd);
}

void cuFemTetrahedron_resetForce(float3 * dst,
                                    uint maxInd)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(maxInd, 512);
    dim3 grid(nblk, 1, 1);
    
    resetForce_kernel<<< grid, block >>>(dst, maxInd);
}

void cuFemTetrahedron_internalForce(float3 * dst,
                                    float3 * pos,
                                    uint4 * tetvert,
                                    mat33 * orientation,
                                    KeyValuePair * tetraInd,
                                    uint * bufferIndices,
                                    uint maxBufferInd,
                                    uint maxInd)
{
    int tpb = CudaBase::LimitNThreadPerBlock(34, 50);
    dim3 block(tpb, 1, 1);
    unsigned nblk = iDivUp(maxInd, tpb);
    dim3 grid(nblk, 1, 1);
    
    internalForce_kernel<<< grid, block >>>(dst,
        1.f, 2.f, 3.f,
                                            pos,
                                            tetvert,
                                            orientation,
                                            tetraInd,
                                            bufferIndices,
                                            maxBufferInd,
                                            maxInd);
}

}
