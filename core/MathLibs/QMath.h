#pragma once
#pragma warning( disable : 4267 4800 )

#include "UtilsDefines.h"

#include "Wm3IntrRay3Sphere3.h"

using namespace Wm3;

Wm3::Vector3f	D3DX2WmVector( const D3DXVECTOR3* v );
D3DXVECTOR3	WmVector2D3DX( const Wm3::Vector3f* v );

bool		IsNumThePow2( UINT a );
UINT		GetHigherPow2( UINT a );
UINT		GetLowerPow2( UINT a );

float		GetMin( float a, float b );
float		GetMax( float a, float b );

bool		IntersectRay2Sphere( const SRay* pstRay, const D3DXVECTOR3* pSpherePos, float fSphereRad, const D3DXMATRIX* pmatWorld );
bool		IntersectRay2Box( const SRay* pstRay, const D3DXVECTOR3* pVertArr, const D3DXMATRIX* pmatWorld, float* pfDist, D3DXVECTOR3* pvIntrPos );