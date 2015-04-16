#ifndef FEMTETRAHEDRONSYSTEM_H
#define FEMTETRAHEDRONSYSTEM_H

#include <CudaTetrahedronSystem.h>
class CUDABuffer;
class CudaCSRMatrix;
class FEMTetrahedronSystem : public CudaTetrahedronSystem {
public:
    FEMTetrahedronSystem();
    virtual ~FEMTetrahedronSystem();
    virtual void initOnDevice();
    
    void resetOrientation();
    void updateOrientation();
protected:
    void createStiffnessMatrix();
    
private:
    CUDABuffer * m_Re;
    CudaCSRMatrix * m_stiffnessMatrix;
};

#endif        //  #ifndef FEMTETRAHEDRONSYSTEM_H

