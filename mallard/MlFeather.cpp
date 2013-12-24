#include "MlFeather.h"
#include "MlRachis.h"
#include "MlVane.h"
#include <CollisionRegion.h>
MlFeather::MlFeather() : m_worldP(0)
{
	m_rachis = new MlRachis;
	m_vane = new MlVane[2];
	
	simpleCreate();
}

MlFeather::~MlFeather() 
{
	if(m_worldP) delete[] m_worldP;
	delete m_rachis;
	delete[] m_vane;
}

void MlFeather::createNumSegment(short x)
{
	BaseFeather::createNumSegment(x);
	if(m_worldP) delete[] m_worldP;
	
	m_worldP = new Vector3F[(numSegment() + 1) * 7];
	m_rachis->create(x);
}

void MlFeather::setupVane()
{
	m_vane[0].create(numSegment(), 3);
	m_vane[1].create(numSegment(), 3);
	Vector3F oriP(4.f, 0.f, 4.f);
	Matrix33F oriR; oriR.fill(Vector3F::ZAxis, Vector3F::XAxis, Vector3F::YAxis);
	float sc = 1.f;
	m_rachis->reset();
	computeWorldP(oriP, oriR, sc);
	m_vane[0].computeKnots();
	m_vane[1].computeKnots();
}

void MlFeather::updateVane()
{
	Vector3F oriP(4.f, 0.f, 4.f);
	Matrix33F oriR; oriR.fill(Vector3F::ZAxis, Vector3F::XAxis, Vector3F::YAxis);
	float sc = 1.f;
	computeWorldP(oriP, oriR, sc);
}

void MlFeather::bend()
{
	m_rachis->bend();
}

void MlFeather::bendAt(unsigned faceIdx, float patchU, float patchV, const Vector3F & oriPos, const Matrix33F & oriRot, const float & scale)
{
	m_rachis->bend(faceIdx, patchU, patchV, oriPos, oriRot, scale * shaftLength(), m_skin);
}

void MlFeather::curl(float val)
{
	m_rachis->curl(val);
}

void MlFeather::computeWorldP(const Vector3F & oriPos, const Matrix33F & oriRot, const float & scale)
{
	Vector3F segOrigin = oriPos;
	Matrix33F segSpace = oriRot;
	const short numSeg = numSegment();
	for(short i = 0; i < numSeg; i++) {
		Matrix33F mat = m_rachis->getSpace(i);
		mat.multiply(segSpace);
		
		*segmentOriginWP(i) = segOrigin;
		*segmentVaneWP1(i, 0, 0) = segOrigin;
		*segmentVaneWP1(i, 0, 1) = segOrigin;
		computeVaneWP(segOrigin, mat, i, 0, scale);
		computeVaneWP(segOrigin, mat, i, 1, scale);
		
		Vector3F d(0.f, 0.f, quilly()[i] * scale);
		d = mat.transform(d);
		
		segOrigin += d;
		segSpace = mat;
	}
	
	*segmentOriginWP(numSeg) = segOrigin;
	*segmentVaneWP1(numSeg, 0, 0) = segOrigin;
	*segmentVaneWP1(numSeg, 0, 1) = segOrigin;
	computeVaneWP(segOrigin, segSpace, numSeg, 0, scale);
	computeVaneWP(segOrigin, segSpace, numSeg, 1, scale);
}

Vector3F * MlFeather::worldP()
{
	return m_worldP;
}

Vector3F * MlFeather::segmentOriginWP(short seg)
{
	return &m_worldP[seg * 7];
}

Vector3F * MlFeather::segmentVaneWP(short seg, short side, short idx)
{
	return &m_worldP[seg * 7 + 1 + side * 3 + idx];
}

Vector3F * MlFeather::segmentVaneWP1(unsigned seg, unsigned end, short side)
{
	return m_vane[side].railCV(seg, end);
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
	Vector2F * vane = getUvDisplaceAt(seg, side);
	
	const float tapper = width() * -.005f;
	for(short i = 0; i < 3; i++) {
		Vector3F d(tapper, vane->x, vane->y);
		d *= scale;
		d = space.transform(d);
		
		p += d;
		*segmentVaneWP(seg, side, i) = p;
		*segmentVaneWP1(seg, i+1, side) = p;
		
		vane++;
	}
}

void MlFeather::setCollision(CollisionRegion * skin)
{
	m_skin = skin;
}

void MlFeather::setFeatherId(short x)
{
	m_id = x;
}
	
short MlFeather::featherId() const
{
	return m_id;
}

void MlFeather::simpleCreate(int ns)
{
	BaseFeather::simpleCreate(ns);
	setupVane();
}

void MlFeather::computeLength()
{
	BaseFeather::computeLength();
	m_rachis->computeLengths(quilly(), shaftLength());
}

void MlFeather::changeNumSegment(int d)
{
	BaseFeather::changeNumSegment(d);
	setupVane();
}

void MlFeather::getBoundingBox(BoundingBox & box) const
{
	for(unsigned i = 0; i < numWorldP(); i++)
		box.update(m_worldP[i]);
}

float * MlFeather::angles() const
{
	return m_rachis->angles();
}

float MlFeather::bendDirection() const
{
	return m_rachis->bendDirection();
}

MlVane * MlFeather::vane(short side) const
{
	return &m_vane[side];
}

void MlFeather::verbose()
{
	std::cout<<"feather index:\n id "<<featherId();
	std::cout<<"\n n segment "<<numSegment();
	std::cout<<"\n length "<<shaftLength();
	std::cout<<"\n base uv ("<<baseUV().x<<","<<baseUV().y<<")\n";
}

void MlFeather::samplePosition(unsigned gridU, unsigned gridV, Vector3F * dst)
{
	const float du = 1.f/(float)gridU;
	const float dv = 1.f/(float)gridV;
	
	
	unsigned acc = 0;
	for(unsigned i = 0; i <= gridU; i++) {
		m_vane[0].setU(du*i);
		for(unsigned j = 0; j <= gridV; j++) {
			m_vane[0].pointOnVane(dv * j, dst[acc]);
			acc++;
		}
		m_vane[0].modifyLength(du*i, gridV, &dst[acc - gridV - 1]);
	}
	
	
	for(unsigned i = 0; i <= gridU; i++) {
		m_vane[1].setU(du*i);
		for(unsigned j = 0; j <= gridV; j++) {
			m_vane[1].pointOnVane(dv * j, dst[acc]);
			acc++;
		}
		m_vane[1].modifyLength(du*i, gridV, &dst[acc - gridV - 1]);
	}
}

void MlFeather::setSeed(unsigned s)
{
	m_vane[0].setSeed(s);
	m_vane[1].setSeed(s + 64873);
}

void MlFeather::setNumSeparate(unsigned n)
{
	m_vane[0].separate(n);
	m_vane[1].separate(n);
}

void MlFeather::setSeparateStrength(float k)
{
	m_vane[0].setSeparateStrength(k);
	m_vane[1].setSeparateStrength(k);
}

void MlFeather::setFuzzy(float f)
{
	m_vane[0].setFuzzy(f);
	m_vane[1].setFuzzy(f);
}
