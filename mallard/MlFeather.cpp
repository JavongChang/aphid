#include "MlFeather.h"
#include "MlRachis.h"
#include <CollisionRegion.h>
MlFeather::MlFeather() : m_quilly(0), m_vaneVertices(0), m_worldP(0) 
{
	m_rachis = new MlRachis;
}

MlFeather::~MlFeather() 
{
    if(m_quilly) delete[] m_quilly;
    if(m_vaneVertices) delete[] m_vaneVertices;
	if(m_worldP) delete[] m_worldP;
	delete m_rachis;
}

void MlFeather::createNumSegment(short x)
{
    m_numSeg = x;
    m_quilly = new float[m_numSeg];
    m_vaneVertices = new Vector2F[(m_numSeg + 1) * 6];
	m_worldP = new Vector3F[(m_numSeg + 1) * 7];
	m_rachis->create(x);
}

void MlFeather::computeLength()
{
	m_length = 0.f;
	for(short i=0; i < m_numSeg; i++)
		m_length += m_quilly[i];
	m_rachis->computeAngles(m_quilly, m_length);
}

short MlFeather::numSegment() const
{
	return m_numSeg;
}

float * MlFeather::quilly()
{
    return m_quilly;
}

float * MlFeather::getQuilly() const
{
     return m_quilly;
}

Vector2F * MlFeather::vaneAt(short seg, short side)
{
    return &m_vaneVertices[seg * 6 + 3 * side];
}

Vector2F * MlFeather::getVaneAt(short seg, short side) const
{
    return &m_vaneVertices[seg * 6 + 3 * side];
}

float MlFeather::getLength() const
{
	return m_length;
}

float MlFeather::getWidth(short seg) const
{
	Vector2F * vane = getVaneAt(seg, 0);
	float r = vane->x;
	vane++;
	r += vane->x;
	vane++;
	r += vane->x;

	vane = getVaneAt(seg, 1);
	r -= vane->x;
	vane++;
	r -= vane->x;
	vane++;
	r -= vane->x;
	return r;
}

void MlFeather::computeWorldP(const Vector3F & oriPos, const Matrix33F & oriRot, const float& pitch, const float & scale)
{
	
	m_rachis->update(oriPos, oriRot, getLength() * scale, m_skin, pitch);
	
	Vector3F segOrigin = oriPos;
	Matrix33F segSpace = oriRot;
	for(short i = 0; i < m_numSeg; i++) {
		Matrix33F mat = m_rachis->getSpace(i);
		mat.multiply(segSpace);
		
		*segmentOriginWP(i) = segOrigin;
		computeVaneWP(segOrigin, mat, i, 0, scale);
		computeVaneWP(segOrigin, mat, i, 1, scale);
		
		Vector3F d(0.f, 0.f, m_quilly[i] * scale);
		d = mat.transform(d);
		
		segOrigin += d;
		segSpace = mat;
	}
	
	*segmentOriginWP(m_numSeg) = segOrigin;
	computeVaneWP(segOrigin, segSpace, m_numSeg, 0, scale);
	computeVaneWP(segOrigin, segSpace, m_numSeg, 1, scale);
}

Vector3F * MlFeather::segmentOriginWP(short seg)
{
	return &m_worldP[seg * 7];
}

Vector3F * MlFeather::segmentVaneWP(short seg, short side, short idx)
{
	return &m_worldP[seg * 7 + 1 + side * 3 + idx];
}

Vector3F MlFeather::getSegmentOriginWP(short seg) const
{
	return m_worldP[seg * 7];
}

Vector3F MlFeather::getSegmentVaneWP(short seg, short side, short idx) const
{
	return m_worldP[seg * 7 + 1 + side * 3 + idx];
}

void MlFeather::computeVaneWP(const Vector3F & origin, const Matrix33F& space, short seg, short side, float scale)
{
	Vector3F p = origin;
	Vector2F * vane = getVaneAt(seg, side);
	
	const float tapper = getWidth(seg) * -.03f;
	for(short i = 0; i < 3; i++) {
		Vector3F d(tapper * (i + 1), vane->x, vane->y);
		d *= scale;
		d = space.transform(d);
		
		p += d;
		*segmentVaneWP(seg, side, i) = p;
		
		vane++;
	}
}

void MlFeather::setCollision(CollisionRegion * skin)
{
	m_skin = skin;
}
