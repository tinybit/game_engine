#pragma once

#include <d3dx9math.h>
#include "../VECTOR.H"
#include "../Plane.h"

#define EPSILON 0.00001

struct CSPHERE
{
	CSPHERE() {};
	CSPHERE( D3DXVECTOR3 vPos, float fRadius ) { pos = vPos; radius = fRadius; };

	float		radius;
	D3DXVECTOR3	pos;
};

void		ConvertVec2LHCoords( D3DXVECTOR3* vec );
void		ConvertVec2LHCoordsDeepEx( D3DXVECTOR3* vec );
void		ClampAngle( float& fAngle );
float		AngleDif( float NewAngle, float OldAngle );
D3DXMATRIX	InterpolateMatrices( D3DXMATRIX* mat1, D3DXMATRIX* mat2, float fScale );

float		CalcRaySphereIntersection( const CSPHERE& sphere, const D3DXVECTOR3* pos, const D3DXVECTOR3* dir );
float		CalcRayTriangleIntersection( D3DXVECTOR3* A, D3DXVECTOR3* B, D3DXVECTOR3* C, D3DXVECTOR3* pos, D3DXVECTOR3* dir );
bool		CalcLineLineIntersection( const D3DXVECTOR2* begin, const D3DXVECTOR2*  end, const D3DXVECTOR2* AAbegin, const D3DXVECTOR2* AAend, D3DXVECTOR2* isect );

//returns angle from -PI to PI, based on vBase
float		GetAngle( D3DXVECTOR3* vBase, D3DXVECTOR3* vSecond, D3DXVECTOR3* vAxis );

inline float ClampFloat2( float& fValue, float fMin = -1.0f, float fMax = 1.0f )
{
	if( fValue < fMin )
	{
		fValue = fMin;
	}
	else if( fValue > fMax )
	{
		fValue = fMax;
	}

	return fValue;
}

inline float ClampFloat( float fValue, float fMin = -1.0f, float fMax = 1.0f )
{
	ClampFloat2( fValue, fMin, fMax );

	return fValue;
}