// Plane.cpp: implementation of the CPlane class.
//////////////////////////////////////////////////////////////////////

#pragma warning(disable: 75)
#pragma warning(disable: 4244)

#include "Plane.h"

CPlane::CPlane()
{

}

CPlane::CPlane(float a, float b, float c, float d)
{
  N = CVector(a, b, c);
  D = d;
}

CPlane::CPlane(const CVector& normal, float d)
{
  N = normal;
  D = d;
}

CPlane::CPlane(const CPlane& plane)
{
  N = plane.N;
  D = plane.D;
}

CPlane::CPlane(const CVector& vertexA, const CVector& vertexB, const CVector& vertexC)
{
  CVector normalA((vertexC - vertexA) | 1.0);
  CVector normalB((vertexC - vertexB) | 1.0);
  N = (normalA ^ normalB) | 1.0;
  D = -vertexA % N;
}

CPlane::CPlane(const D3DXVECTOR3* vertexA, const D3DXVECTOR3* vertexB, const D3DXVECTOR3* vertexC)
{
	CVector a( vertexA->x, vertexA->y, vertexA->z );
	CVector b( vertexB->x, vertexB->y, vertexB->z );
	CVector c( vertexC->x, vertexC->y, vertexC->z );

	CVector normalA((c - a) | 1.0);
	CVector normalB((c - b) | 1.0);
	N = (normalA ^ normalB) | 1.0;
	D = -a % N; 
}

const CPlane& CPlane::operator=(const CPlane &plane)
{
	N = plane.N;
	D = plane.D;

	return *this;
}

const bool CPlane::operator==(const CPlane &plane) const
{
	return N == plane.N && D == plane.D;
}

const bool CPlane::operator!=(const CPlane &plane) const
{
	return !(*this == plane);
}

const double inline CPlane::DistanceToPlane(const CVector& point) const
{
	return N % point + D;
}

const bool inline CPlane::IsPointOnPlane(const CVector& point) const
{
	return DistanceToPlane(point) == 0;
}

const CVector CPlane::RayIntersection(const CVector& rayPos, const CVector& rayDir) const
{
	const float a = N % rayDir;

	if (a == 0)
		return rayPos;

	return rayPos - rayDir * (DistanceToPlane(rayPos) / a);
}

const D3DXVECTOR3 CPlane::RayIntersection(const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir) const
{
	D3DXVECTOR3 res;
	CVector isect, pos, dir;

	pos = *rayPos;
	dir = *rayDir;
	
	isect = RayIntersection( pos, dir );

	res = D3DXVECTOR3( isect.x, isect.y, isect.z );

	return res;
}
