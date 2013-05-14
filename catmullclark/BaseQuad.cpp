#include <Vector3F.h>
#include "BaseQuad.h"

BaseQuad::BaseQuad()
{
    _corners = new Vector3F[4];
}

BaseQuad::~BaseQuad()
{
    delete[] _corners;
}

void BaseQuad::setCorner(Vector3F p, int i)
{
	_corners[i] = p;
}

Vector3F BaseQuad::getCorner(int i) const
{
	return _corners[i];
}

Vector3F BaseQuad::interpolate(float u, float v) const
{
    Vector3F lo = _corners[0] * (1.f - u) + _corners[1] * u;
    Vector3F hi = _corners[3] * (1.f - u) + _corners[2] * u;
    return lo * (1.f - v) + hi * v;
}
