#ifndef FEMTETRAHEDRONSYSTEM_H
#define FEMTETRAHEDRONSYSTEM_H

#include <CudaTetrahedronSystem.h>
#include <CudaConjugateGradientSolver.h>
class BaseBuffer;
class CUDABuffer;
class CudaCSRMatrix;
class FEMTetrahedronSystem : public CudaTetrahedronSystem, 
                                public CudaConjugateGradientSolver
{
public:
    FEMTetrahedronSystem();
    FEMTetrahedronSystem(ATetrahedronMesh * md);
    virtual ~FEMTetrahedronSystem();
	virtual void initOnDevice();
	virtual void update();
    virtual void integrate(float dt);
	
    void verbose();
protected:
    
private:
    void createStiffnessMatrix();
	void createVertexTetraHash();
    void resetOrientation();
    void updateOrientation();
    void resetStiffnessMatrix();
    void updateStiffnessMatrix();
    void resetForce();
    void updateForce();
    void dynamicsAssembly(float dt);
    void updateExternalForce();
	void solveConjugateGradient();
private:
    CUDABuffer * m_Re;
    CudaCSRMatrix * m_stiffnessMatrix;
    BaseBuffer * m_stiffnessTetraHash;
    BaseBuffer * m_stiffnessInd;
    BaseBuffer * m_vertexTetraHash;
    BaseBuffer * m_vertexInd;
    CUDABuffer * m_deviceStiffnessTetraHash;
    CUDABuffer * m_deviceStiffnessInd;
    CUDABuffer * m_deviceVertexTetraHash;
    CUDABuffer * m_deviceVertexInd;
    CUDABuffer * m_F0;
    CUDABuffer * m_Fe;
};

#endif        //  #ifndef FEMTETRAHEDRONSYSTEM_H

