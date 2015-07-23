#include "IVelocityFile.h"
#include <BaseBuffer.h>
IVelocityFile::IVelocityFile() : APlaybackFile() 
{
    m_vel = new BaseBuffer;
    m_numV = 0;
}

IVelocityFile::IVelocityFile(const char * name) : APlaybackFile(name) 
{
    m_vel = new BaseBuffer;
    m_numV = 0;
}

IVelocityFile::~IVelocityFile() 
{ delete m_vel; }

const unsigned IVelocityFile::numPoints() const
{ return m_numV; }

void IVelocityFile::createPoints(unsigned n)
{ 
    m_vel->create(n*12);
    m_numV = n;
}

Vector3F * IVelocityFile::velocities() const
{ return (Vector3F *)m_vel->data(); }


