#ifndef CUFEMTETRAHEDRON_IMPLEMENT_H
#define CUFEMTETRAHEDRON_IMPLEMENT_H

#include "bvh_common.h"
#include <radixsort_implement.h>
extern "C" {
    
void cuFemTetrahedron_resetRe(mat33 * d, uint maxInd);

void cuFemTetrahedron_calculateRe(mat33 * dst, 
                                    float3 * pos, 
                                    float3 * pos0,
                                    uint4 * indices,
                                    uint maxInd);

void cuFemTetrahedron_resetStiffnessMatrix(mat33 * dst,
                                    uint maxInd);

void cuFemTetrahedron_stiffnessAssembly(mat33 * dst,
                                        mat33 * orientation,
                                        KeyValuePair * tetraInd,
                                        uint * bufferIndices,
                                        uint maxBufferInd,
                                        uint maxInd);
}
#endif        //  #ifndef CUFEMTETRAHEDRON_IMPLEMENT_H

