#define STRICT

#include "QMath.h"

inline Wm3::Vector3f D3DX2WmVector( const D3DXVECTOR3* v )
{
	return Vector3f( v->x, v->y, v->z ); 
}

inline D3DXVECTOR3 WmVector2D3DX( const Wm3::Vector3f* v )
{
	return D3DXVECTOR3(v->X(), v->Y(), v->Z()); 
}

bool IsNumThePow2( UINT a )
{
	return ( GetHigherPow2(a)==a );
}

UINT GetHigherPow2( UINT a )
{
	UINT i = 0;
	for( i = 1; i < a; i = i * 2 ) {}
	return i;
}

UINT GetLowerPow2( UINT a )
{
	UINT i = 0;
	for( i = 1; i < a; i = i * 2 ) {}
	return i / 2;
}

float GetMin( float a, float b )
{
	return ( (a<b) ? a : b ); 
}

float GetMax( float a, float b )
{
	return ( (a>b) ? a : b ); 
}

bool IntersectRay2Sphere( const SRay* pstRay, const D3DXVECTOR3* pSpherePos, float fSphereRad, const D3DXMATRIX* pmatWorld )
{
	if ( pmatWorld != NULL )
	{
		D3DXVECTOR3 temp, qwe;
		float f;
		
		D3DXVec3TransformCoord( &temp, pSpherePos, pmatWorld );
		D3DXVec3TransformCoord( &qwe, &(*pSpherePos + D3DXVECTOR3(fSphereRad,0,0)), pmatWorld );
		f = D3DXVec3LengthSq( &(temp-qwe) );
		if ( fabs(fSphereRad*fSphereRad-f) > 0.001f )
			fSphereRad = sqrtf(f);
		
		IntrRay3Sphere3f intr_ray_sphere( 
			Ray3f(	D3DX2WmVector( &pstRay->vPos ), D3DX2WmVector( &pstRay->vDir ) ), 
			Sphere3f( D3DX2WmVector( &temp ), fSphereRad )
			);

		return intr_ray_sphere.Test();
	}
	else
	{
		IntrRay3Sphere3f intr_ray_sphere( 
			Ray3f(	D3DX2WmVector( &pstRay->vPos ), D3DX2WmVector( &pstRay->vDir ) ), 
			Sphere3f( D3DX2WmVector( pSpherePos ), fSphereRad )
			);

		return intr_ray_sphere.Test();
	}
}

inline void InlIntersectTri( const D3DXVECTOR3 *pVertArr,
								int p0, int p1, int p2, const SRay* pstRay,
								float *pU, float *pV, float *pDist,	bool* bIntersected,
								int* pv0, int* pv1,	int* pv2 )
{
	float m_fTempU, m_fTempV, m_fDist;
	if ( D3DXIntersectTri( &pVertArr[p0], &pVertArr[p1], &pVertArr[p2], &pstRay->vPos, &pstRay->vDir, &m_fTempU, &m_fTempV, &m_fDist ) )
	{
		*bIntersected = true;
		if ( m_fDist < *pDist )
		{
			*pDist	= m_fDist;
			*pU		= m_fTempU;
			*pV		= m_fTempV;
			*pv0	= p0;
			*pv1	= p1;
			*pv2	= p2;
		}
	}	
}

inline void InlIntersectTri2( const D3DXVECTOR3 *pVertArr,
								int p0, int p1, int p2, const SRay* pstRay,	
								bool* bIntersected )
{
	if ( *bIntersected )
	{
		return;
	}
	else
	{
		*bIntersected = D3DXIntersectTri( &pVertArr[p0], &pVertArr[p1], &pVertArr[p2], &pstRay->vPos, &pstRay->vDir, NULL, NULL, NULL );
	}
}

bool IntersectRay2Box( const SRay* pstRay, const D3DXVECTOR3* pVertArr, const D3DXMATRIX* pmatWorld, float* pfDist, D3DXVECTOR3* pvIntrPos )
{
	if(!pVertArr)
	{
		return false;		
	}

	D3DXVECTOR3* oobb = new D3DXVECTOR3[8];

	D3DXVec3TransformCoordArray( oobb, 1, pVertArr, 1, pmatWorld, 8 );

	bool m_bIntersected = false;

	if ( pfDist || pvIntrPos )
	{
		float m_fTempU, m_fTempV, m_fDist = 999999999.0f;
		int m_iTempV0, m_iTempV1, m_iTempV2;

		InlIntersectTri( oobb, 0, 1, 2, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 0, 2, 3, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 4, 6, 5, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 4, 7, 6, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 4, 5, 1, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 4, 1, 0, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 7, 4, 0, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 7, 0, 3, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 7, 2, 6, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 7, 3, 2, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 6, 1, 5, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );
		InlIntersectTri( oobb, 6, 2, 1, pstRay, &m_fTempU, &m_fTempV, &m_fDist, &m_bIntersected, &m_iTempV0, &m_iTempV1, &m_iTempV2 );

		if (m_bIntersected)
		{
			if ( pfDist )
			{
				*pfDist = m_fDist;
			}
			if ( pvIntrPos )
			{
				D3DXVec3BaryCentric( pvIntrPos, &oobb[m_iTempV0], &oobb[m_iTempV1], &oobb[m_iTempV2], m_fTempU, m_fTempV );
			}
		}
	}
	else
	{
		InlIntersectTri2( oobb, 0, 1, 2, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 0, 2, 3, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 4, 6, 5, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 4, 7, 6, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 4, 5, 1, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 4, 1, 0, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 7, 4, 0, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 7, 0, 3, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 7, 2, 6, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 7, 3, 2, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 6, 1, 5, pstRay, &m_bIntersected );
		InlIntersectTri2( oobb, 6, 2, 1, pstRay, &m_bIntersected );
	}

	SAFE_DELETE_ARRAY( oobb );
	return m_bIntersected;
}