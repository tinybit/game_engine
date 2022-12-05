#pragma once

#include "Vector.h"
#include <d3d9.h>
#include <d3dx9math.h>

class CPlane 
{
	public:
		CPlane();                                       /* constructors */
		CPlane(float a, float b, float c, float d);
		CPlane(const CVector& normal, float d);
		CPlane(const CPlane& plane);
		CPlane(const CVector& vertexA, const CVector& vertexB, const CVector& vertexC);
		CPlane(const D3DXVECTOR3* vertexA, const D3DXVECTOR3* vertexB, const D3DXVECTOR3* vertexC);

		CVector N;                                        /* data */
		float D;

		const CPlane& operator=(const CPlane &plane);     /* operators */

		const bool operator==(const CPlane &plane) const; /* boolean operators */
		const bool operator!=(const CPlane &plane) const;

		const double inline DistanceToPlane(const CVector& point) const;
		const bool inline IsPointOnPlane(const CVector& point) const;
		const CVector RayIntersection(const CVector& rayPos, const CVector& rayDir) const;
		const D3DXVECTOR3 RayIntersection(const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir) const;
};
