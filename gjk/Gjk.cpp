#include <Gjk.h>

float determinantTetrahedron(Matrix44F & mat, const Vector3F & v1, const Vector3F & v2, const Vector3F & v3, const Vector3F & v4)
{
    * mat.m(0, 0) = v1.x;
    * mat.m(0, 1) = v1.y;
    * mat.m(0, 2) = v1.z;
    * mat.m(0, 3) = 1.f;
    
    * mat.m(1, 0) = v2.x;
    * mat.m(1, 1) = v2.y;
    * mat.m(1, 2) = v2.z;
    * mat.m(1, 3) = 1.f;
    
    * mat.m(2, 0) = v3.x;
    * mat.m(2, 1) = v3.y;
    * mat.m(2, 2) = v3.z;
    * mat.m(2, 3) = 1.f;
    
    * mat.m(3, 0) = v4.x;
    * mat.m(3, 1) = v4.y;
    * mat.m(3, 2) = v4.z;
    * mat.m(3, 3) = 1.f;
    
    return mat.determinant();
}

char isTetrahedronDegenerate(const Vector3F * v)
{
    Matrix44F mat;
    float D0 = determinantTetrahedron(mat, v[0], v[1], v[2], v[3]);
    return (D0 == 0.f);
}

char isTriangleDegenerate(const Vector3F * v)
{
	Vector3F n = Vector3F(v[1], v[0]).cross( Vector3F(v[2], v[0]) );
	
    float D0 = n.length2();
    return (D0 == 0.f);
}

BarycentricCoordinate getBarycentricCoordinate2(const Vector3F & p, const Vector3F * v)
{
	BarycentricCoordinate coord;
	Vector3F dv = v[1] - v[0];
	float D0 = dv.length();
	if(D0 == 0.f) {
        std::cout<<"line is degenerate\n";
        coord.x = coord.y = coord.z = coord.w = -1.f;
        return coord;
    }  
	Vector3F dp = p - v[0];
	coord.x = dp.length() / D0;
	coord.y = 1.f - coord.x;
	
	return coord;
}

BarycentricCoordinate getBarycentricCoordinate3(const Vector3F & p, const Vector3F * v)
{
    Matrix33F mat;
    
    BarycentricCoordinate coord;
    
    Vector3F n = Vector3F(v[1] - v[0]).cross( Vector3F(v[2] - v[0]) );
	
    float D0 = n.length2();
    if(D0 == 0.f) {
        std::cout<<"tiangle is degenerate\n";
        coord.x = coord.y = coord.z = coord.w = -1.f;
        return coord;
    }  
    
	Vector3F na = Vector3F(v[2] - v[1]).cross( Vector3F(p - v[1]) );
    float D1 = n.dot(na);
	Vector3F nb = Vector3F(v[0] - v[2]).cross( Vector3F(p - v[2]) );  
    float D2 = n.dot(nb);
	Vector3F nc = Vector3F(v[1] - v[0]).cross( Vector3F(p - v[0]) );
    float D3 = n.dot(nc);
    
    coord.x = D1/D0;
    coord.y = D2/D0;
    coord.z = D3/D0;
    coord.w = 1.f;
    
    return coord;
}

BarycentricCoordinate getBarycentricCoordinate4(const Vector3F & p, const Vector3F * v)
{
    Matrix44F mat;
    
    BarycentricCoordinate coord;
    
    float D0 = determinantTetrahedron(mat, v[0], v[1], v[2], v[3]);
    if(D0 == 0.f) {
        std::cout<<"tetrahedron is degenerate\n";
        coord.x = coord.y = coord.z = coord.w = -1.f;
        return coord;
    }  
    
    float D1 = determinantTetrahedron(mat, p, v[1], v[2], v[3]);
    float D2 = determinantTetrahedron(mat, v[0], p, v[2], v[3]);
    float D3 = determinantTetrahedron(mat, v[0], v[1], p, v[3]);
    float D4 = determinantTetrahedron(mat, v[0], v[1], v[2], p);
    
    coord.x = D1/D0;
    coord.y = D2/D0;
    coord.z = D3/D0;
    coord.w = D4/D0;
    
    return coord;
}

Vector3F closestToOriginOnLine(const Vector3F & p0, const Vector3F & p1)
{
    const float d0 = p0.length();
    if(d0 < TINY_VALUE) return p0;
	
	const float d1 = p1.length();
	if(d1 < TINY_VALUE) return p1;
	
	Vector3F r = p0;
    Vector3F dir = p1 - p0;
	float d = d0;
	
	const float l01 = dir.length();
    if(l01 < TINY_VALUE) return r;
	
	if(d1 < d0) {
		r = p1;
		dir = p0 - p1;
		d = d1;
	}
    
    if(dir.dot(r) > 0.f) return r;
    
    r /= d;
    dir /= l01;
    
    const float factor = dir.dot(r);
    
    return r * d - dir * d * factor;
}

char closestPointToOriginInsideTriangle(Vector3F & onplane, const Vector3F & p0, const Vector3F & p1, const Vector3F & p2)
{
    Vector3F ab = p1 - p0;
    Vector3F ac = p2 - p0;
    Vector3F nor = ab.cross(ac);
    nor.normalize();
    
    float t = p0.dot(nor);
    onplane = nor * t;
    
    Vector3F e01 = p1 - p0;
	Vector3F x0 = onplane - p0;
	if(e01.cross(x0).dot(nor) < 0.f) return 0;
	
	Vector3F e12 = p2 - p1;
	Vector3F x1 = onplane - p1;
	if(e12.cross(x1).dot(nor) < 0.f) return 0;
	
	Vector3F e20 = p0 - p2;
	Vector3F x2 = onplane - p2;
	if(e20.cross(x2).dot(nor) < 0.f) return 0;
	
	return 1;
}

Vector3F closestToOriginOnTriangle(const Vector3F & a, const Vector3F & b, const Vector3F & c)
{
    // std::cout<<" closest on triangle "<<a.str()<<" "<<b.str()<<" "<<c.str()<<"\n";
    Vector3F onplane;
    if(closestPointToOriginInsideTriangle(onplane, a, b, c)) {
        // std::cout<<" inside tri ";
        return onplane;
    }
    
    Vector3F online = closestToOriginOnLine(a, b);
    float minDistance = online.length();
    Vector3F l = closestToOriginOnLine(b, c);
    float ll = l.length();
    if(ll < minDistance) {
        online = l;
        minDistance = ll;
    }
    l = closestToOriginOnLine(c, a);
    ll = l.length();
    if(ll < minDistance) {
        online = l;
        minDistance = ll;
    }
    return online;
}

Vector3F closestToOriginOnTetrahedron(const Vector3F * p)
{
    Vector3F q = closestToOriginOnTriangle(p[0], p[1], p[2]);
    float d = q.length();
    float minD = d;
    Vector3F r = q;
    
    q = closestToOriginOnTriangle(p[0], p[1], p[3]);
    d = q.length();
    if(d < minD) {
        minD = d;
        r = q;
    }
    
    q = closestToOriginOnTriangle(p[0], p[2], p[3]);
    d = q.length();
    if(d < minD) {
        minD = d;
        r = q;
    }
    
    q = closestToOriginOnTriangle(p[1], p[2], p[3]);
    d = q.length();
    if(d < minD) {
        minD = d;
        r = q;
    }
    
    return r;
}

bool checkCoplanar(const Vector3F & a, const Vector3F & b, const Vector3F & c)
{
    Vector3F ab = b - a;
   // std::cout<<"ab length "<<ab.length();
    if(ab.length() < 0.00001f) {
        std::cout<<" too close";
        return 1;
    }
    
    Vector3F bc = c - b;
    // std::cout<<"cb length "<<bc.length();
    if(bc.length() < 0.00001f) {
        std::cout<<" too close";
        return 1;
    }
    
    ab.normalize();
    bc.normalize();
    
    if(ab.dot(bc) > 0.999999f || ab.dot(bc) < -0.999999f) {
        // std::cout<<" dot "<<ab.dot(bc)<<" is coplanar\n";
        return 1;
    }
    
    return 0;
}

int farthestP(const Vector3F * v, const Vector3F & p)
{
    int ind = 0;
    Vector3F d = p - v[0];
    float dist = d.length();
    float minDist = dist;
    for(int i = 1; i < 3; i++) {
        d = p - v[i];
        dist = d.length();
        if(dist < minDist) {
            minDist = dist;
            ind = i;
        }
    }
    return ind;
}

void resetSimplex(Simplex & s)
{
    s.d = 0;
}

void addToSimplex(Simplex & s, const Vector3F & p)
{
    if(s.d < 1) {
        s.p[0] = p;
        s.d = 1;
    }
    else if(s.d < 2) {
        s.p[1] = p;
        s.d = 2;
    }
    else if(s.d < 3) {
		s.p[2] = p;
		s.d = 3;
		if(isTriangleDegenerate(s.p)) {
			std::cout<<" tri is degenerate";
			std::cout<<" ("<<s.p[0].str()<<","<<s.p[1].str()<<","<<s.p[2].str()<<")\n";
		}
    }
    else {
        s.p[3] = p;
        s.d = 4;
		if(isTetrahedronDegenerate(s.p))
			std::cout<<" tet is degenerate";
    }
}

void removeFromSimplex(Simplex & s, BarycentricCoordinate coord)
{
	float * bar = &coord.x;
    for(int i = 0; i < s.d; i++) {
		if(fabs(bar[i]) < TINY_VALUE) {
			// std::cout<<" zero "<<bar[i]<<" remove vertex "<<i<<"\n";
			for(int j = i; j < s.d - 1; j++) {
				s.p[j] = s.p[j+1];
				bar[j] = bar[j+1];
			}
			i--;
			s.d--;
		}
    }
	// std::cout<<"s.d "<<s.d<<"\n";
}

char pointInsideTetrahedronTest(const Vector3F & p, const Vector3F * v)
{
    BarycentricCoordinate coord = getBarycentricCoordinate4(p, v);
    // std::cout<<"sum "<<coord.x + coord.y + coord.z + coord.w<<"\n";
    
    //Vector3F proof = v[0] * coord.x + v[1] * coord.y + v[2] * coord.z + v[3] * coord.w;
    //std::cout<<"proof "<<proof.str()<<"\n";
    
    if(coord.x < 0.f || coord.y < 0.f || coord.z < 0.f || coord.w < 0.f)
        return 0;
    
    if(coord.x > 1.f || coord.y > 1.f || coord.z > 1.f || coord.w > 1.f)
        return 0;
    
    return 1;
}

char isOriginInsideSimplex(const Simplex & s)
{
    if(s.d < 4) return 0;
    return pointInsideTetrahedronTest(Vector3F::Zero, s.p);
}

Vector3F closestToOriginOnLine(Simplex & s)
{
	Vector3F p = closestToOriginOnLine(s.p[0], s.p[1]);
		
	BarycentricCoordinate bar = getBarycentricCoordinate2(p, s.p);
	// std::cout<<" line bar "<<bar.x<<" "<<bar.y<<"\n";
    removeFromSimplex(s, bar);
    return p;
}

Vector3F closestToOriginOnTriangle(Simplex & s)
{
	Vector3F p = closestToOriginOnTriangle(s.p[0], s.p[1], s.p[2]);
		
	BarycentricCoordinate bar = getBarycentricCoordinate3(p, s.p);
	// std::cout<<" tri bar "<<bar.x<<" "<<bar.y<<" "<<bar.z<<"\n";
    removeFromSimplex(s, bar);
    return p;
}

Vector3F closestToOriginOnTetrahedron(Simplex & s)
{
    Vector3F p = closestToOriginOnTetrahedron(s.p);
	
	BarycentricCoordinate bar = getBarycentricCoordinate4(p, s.p);
	// std::cout<<" tet bar "<<bar.x<<" "<<bar.y<<" "<<bar.z<<" "<<bar.w<<"\n";
    removeFromSimplex(s, bar);
    return p;
}

Vector3F closestToOriginWithinSimplex(Simplex & s)
{
    if(s.d < 2) {
        return s.p[0];
    }
    else if(s.d < 3) {
        return closestToOriginOnLine(s);
    }
    else if(s.d < 4) {
		return closestToOriginOnTriangle(s);
    }
    
    return closestToOriginOnTetrahedron(s);
}
