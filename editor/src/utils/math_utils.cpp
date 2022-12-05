#include "math_utils.h"

float CalcRayTriangleIntersection( D3DXVECTOR3* A, D3DXVECTOR3* B, D3DXVECTOR3* C, D3DXVECTOR3* pos, D3DXVECTOR3* dir )
{
	CPlane		plane( A, B, C );
	D3DXVECTOR3 temp, tempv1, tempv2, tempv3;

	float	num		= 0.0f;
    float	denum	= plane.N.x * dir->x + plane.N.y * dir->y + plane.N.z * dir->z;
	float	t;
	int		nc		= 0, sh, nsh;

	if( denum == 0 )
		return -1;      // ray is parallel to the plane
	else
	{
		num	= plane.N.x * pos->x + plane.N.y * pos->y + plane.N.z * pos->z + plane.D;
		t	= -num / denum;

		temp = D3DXVECTOR3( pos->x + dir->x * t, pos->y + dir->y * t, pos->z + dir->z * t);

		// project trimesh onto plane of dominant coordinate
		if( (fabs( plane.N.x ) > fabs( plane.N.y )) && (fabs ( plane.N.x ) > fabs( plane.N.z )) )
		{
			tempv1	= D3DXVECTOR3( A->y, A->z, 0 );
			tempv2	= D3DXVECTOR3( B->y, B->z, 0 );
			tempv3	= D3DXVECTOR3( C->y, C->z, 0 );
			temp	= D3DXVECTOR3( temp.y, temp.z, 0 );
		}
		else
		{
			if( fabs(plane.N.y) > fabs(plane.N.z) )
			{
				tempv1	= D3DXVECTOR3( A->x, A->z, 0 );
				tempv2	= D3DXVECTOR3( B->x, B->z, 0 );
				tempv3	= D3DXVECTOR3( C->x, C->z, 0 );
				temp	= D3DXVECTOR3( temp.x, temp.z, 0 );
			}
			else
			{
				tempv1	= D3DXVECTOR3( A->x, A->y, 0 );
				tempv2	= D3DXVECTOR3( B->x, B->y, 0 );
				tempv3	= D3DXVECTOR3( C->x, C->y, 0 );
				temp	= D3DXVECTOR3( temp.x, temp.y, 0 );
			}
		}

		// translate point of intersection to the origin 
		tempv1 = tempv1 - temp;
		tempv2 = tempv2 - temp;
		tempv3 = tempv3 - temp;

		// FIRST EDGE
		if( tempv1.y < 0 )  sh = -1; else  sh = 1;
		if( tempv2.y < 0 ) nsh = -1; else nsh = 1;
		if( sh != nsh )  // if y coordinates of an edge have different signs
		if ( (tempv1.x > 0) && (tempv2.x > 0) ) // if both x coordinates are positive
			nc = nc + 1;      // the edge crosses the ray
		else
			if( (tempv1.x > 0) || (tempv2.x > 0) )   // if one coordiante is positive, compute intersection
			if( tempv1.x - tempv1.y * (tempv2.x - tempv1.x) / (tempv2.y - tempv1.y) > 0 )
				nc = nc + 1;  // if the intersection is to the right from zero, the edge crosses the ray

		// SECOND EDGE 
		if( tempv2.y < 0 )  sh = -1; else  sh = 1;
		if( tempv3.y < 0 ) nsh = -1; else nsh = 1;
		if( sh != nsh )  // if y coordinates of an edge have different signs
		if ( (tempv2.x > 0)&&(tempv3.x > 0) ) // if both x coordinates are positive
			nc = nc + 1;      // the edge crosses the ray
		else
			if( (tempv2.x > 0) || (tempv3.x > 0) )   // if one coordiante is positive, compute intersection
			if( tempv2.x - tempv2.y * (tempv3.x - tempv2.x) / (tempv3.y - tempv2.y) > 0 )
				nc = nc + 1;  // if the intersection is to the right from zero, the edge crosses the ray

		// THIRD EDGE
		if( tempv3.y < 0 )  sh = -1; else  sh = 1;
		if( tempv1.y < 0 ) nsh = -1; else nsh = 1;
		if( sh != nsh )  // if y coordinates of an edge have different signs
		  if( (tempv3.x > 0) && (tempv1.x > 0) ) // if both x coordinates are positive
		    nc = nc + 1;      // the edge crosses the ray
		  else
		    if( (tempv3.x > 0) || (tempv1.x > 0) )   // if one coordiante is positive, compute intersection
		      if( tempv3.x - tempv3.y * (tempv1.x - tempv3.x) / (tempv1.y - tempv3.y) > 0)
		        nc = nc + 1;  // if the intersection is to the right from zero, the edge crosses the ray
	}

	if( nc == 1 )
		return t;
	else
		return -1.0f;
}

bool CalcLineLineIntersection(const D3DXVECTOR2* begin, const D3DXVECTOR2*  end, const D3DXVECTOR2* AAbegin, const D3DXVECTOR2* AAend, D3DXVECTOR2* isect )
{
    float k  = 0.0f, b  = 0.0f;
    float Ak = 0.0f, Ab = 0.0f;

    float tmp_x = 0.0f;
    float tmp_y = 0.0f;

    float res_x = 0.0f;
    float res_y = 0.0f;

    bool isy = false;
    
    //1st equation ------------------------------
    if( (begin->x - end->x) != 0.0f )
		k = (float)(begin->y - end->y) / (float)(begin->x - end->x);
    else
		k = 0.0f;

    b = begin->y - k * begin->x;
    //-------------------------------------------

    //aa line
    //2nd equation ------------------------------
    if( (AAbegin->x - AAend->x) != 0.0f )
    {
		Ak = (float)(AAbegin->y - AAend->y) / (float)(AAbegin->x - AAend->x);
		Ab = AAbegin->y - Ak * AAbegin->x;
		isy = true;
    }
    else
    {
		Ak = (float)(AAbegin->x - AAend->x) / (float)(AAbegin->y - AAend->y);
		Ab = AAbegin->x - Ak * AAbegin->y;
    }
    
    //-------------------------------------------
    if(isy)
    {
		tmp_y = Ak * AAbegin->x + Ab;
    }
    else
    {
		tmp_x = Ak * AAbegin->x + Ab;
    }

    if(isy)
    {
		res_x = (tmp_y - b) / k;
		res_y = tmp_y;  
    
		if( !(res_x >= AAbegin->x && res_x <= AAend->x) )
		{
			res_x = -1;
			res_y = -1;
		}
    }
    else
    {
		res_x = tmp_x;
		res_y = k * tmp_x + b;

		if( !(res_y >= AAbegin->y && res_y <= AAend->y) )
		{
			res_x = -1;
			res_y = -1;
		}
    }

    //check for the target line scope
    if( res_x != -1 && res_y != -1)
    {
		if(begin->x < end->x)
		{
			if( !(res_x >= begin->x && res_x <= end->x) )
			{
				res_x = -1;
				res_y = -1;
			}
		}

		if(begin->x >= end->x)
		{
			if( !(res_x >= end->x && res_x <= begin->x) )
			{
				res_x = -1;
				res_y = -1;
			}
		}
    }

    isect->x = res_x;
	isect->y = res_y;

    if( res_x == -1 && res_y == -1)
		return false;
    else
		return true;
}

float CalcRaySphereIntersection( const CSPHERE& sphere, const D3DXVECTOR3* pos, const D3DXVECTOR3* dir )
{
	float res, t1, t2;
	float b	= 2.0f * D3DXVec3Dot( dir, &D3DXVECTOR3( *pos - sphere.pos ) );											// second factor of quadratic equation
	float c	= D3DXVec3LengthSq( &D3DXVECTOR3( *pos - sphere.pos ) ) - sphere.radius * sphere.radius;		// third/free factor
	float det = b * b - 4.0f * c;																													// determinant of quadratic equation

	if( det < 0.0f )						// if line does not intersect the sphere
		res = -1.0f;						// return no intersection
	else
	{
		if ( det == 0.0f )				// line is tangent to the sphere
			res = -b / 2.0f;			// calculate line parameter t
		else								// line intersects the sphere (in two points) 
		{
			t1 = ( -b + sqrt(det) ) / 2.0f;
			t2 = ( -b - sqrt(det) ) / 2.0f;

			if( t1 <= EPSILON )
				res = t2;
			else 
			{
				if( t2 <= EPSILON )
					res = t1;
				else 
					res = ( (t1 < t2)?t1:t2 );
			}
		}
	}
	
	return res;
}

void ClampAngle( float& fAngle )
{
	int rot_count  = (int)( fabs( fAngle ) / 360.0f );

	if( fAngle > 0.0f )
		fAngle = fAngle - 360.0f * rot_count;
	else
	{
		fAngle = fAngle + 360.0f * rot_count;
		fAngle = 360 + fAngle;
	}

	if( fAngle == 360.0f )
		fAngle = 0.0f;
}

void ConvertVec2LHCoords( D3DXVECTOR3* vec )
{
	D3DXMATRIX rotateY;

	D3DXMatrixRotationY( &rotateY, D3DXToRadian( 180 ) );

	vec->z *= -1.0f;

	D3DXVec3TransformCoord( vec, vec, &rotateY );
}

void ConvertVec2LHCoordsDeepEx( D3DXVECTOR3* vec )
{
	D3DXMATRIX rotateX, rotateY;

	D3DXMatrixRotationX( &rotateX, D3DXToRadian( -90 ) );
	D3DXMatrixRotationY( &rotateY, D3DXToRadian( 180 ) );

	vec->z *= -1.0f;

	D3DXVec3TransformCoord( vec, vec, &( rotateY * rotateX ) );
}

D3DXMATRIX InterpolateMatrices( D3DXMATRIX* mat1, D3DXMATRIX* mat2, float fScale )
{
	D3DXQUATERNION pQuatA, pQuatB, resQuat;
	D3DXVECTOR3 vScaleA, vTranslationA, vScaleB, vTranslationB, vScaleRes, vTranslationRes;

	//decompose
	D3DXMatrixDecompose( &vScaleA, &pQuatA, &vTranslationA, mat1 );
	D3DXMatrixDecompose( &vScaleB, &pQuatB, &vTranslationB, mat2 );

	pQuatA.w *= -1.0f;
	pQuatB.w *= -1.0f;

	//interpolate
	D3DXVec3Lerp( &vScaleRes, &vScaleA, &vScaleB, fScale );
	D3DXVec3Lerp( &vTranslationRes, &vTranslationA, &vTranslationB, fScale );
	D3DXQuaternionSlerp( &resQuat, &pQuatA, &pQuatB, fScale );

	//build resulting matrix
	D3DXMATRIX mScaleIpol, mTranslIpol, mRotIpol;
	D3DXMatrixTranslation( &mTranslIpol, vTranslationRes.x, vTranslationRes.y, vTranslationRes.z );
	D3DXMatrixScaling( &mScaleIpol, vScaleRes.x, vScaleRes.y, vScaleRes.z );
	D3DXMatrixRotationQuaternion( &mRotIpol, &resQuat );

	return mScaleIpol * mRotIpol * mTranslIpol;
}

float GetAngle( D3DXVECTOR3* vBase, D3DXVECTOR3* vSecond, D3DXVECTOR3* vAxis )
{
	float			angle = 0.0f;
	D3DXVECTOR3		vTemp;

	if( *vBase == *vSecond )
		return 0.0f;

	D3DXVECTOR3 _vBase = *vBase, _vSecond = *vSecond;

	D3DXVec3Normalize( &_vBase, &_vBase );
	D3DXVec3Normalize( &_vSecond, &_vSecond );

	angle = acos( ClampFloat( D3DXVec3Dot( &_vBase, &_vSecond ) ) );

	D3DXMATRIX rotMtx;
	D3DXMatrixRotationAxis( &rotMtx, vAxis, angle );

	D3DXVec3TransformCoord( &vTemp, &_vBase, &rotMtx );
	
	if( fabs(D3DXVec3Length( &(_vSecond - vTemp))) > EPSILON )
	{
		angle *= -1.0f;
	}

	return angle;
}

float AngleDif( float NewAngle, float OldAngle )
{
	if(NewAngle<OldAngle)
		NewAngle += D3DX_PI*2;

	float temp =  abs(NewAngle - OldAngle);

	if ( temp >= D3DX_PI ) temp = 2* D3DX_PI - temp;

	if ( abs( cos(NewAngle)-cos(OldAngle+temp) )<0.0001f && 
		 abs( sin(NewAngle)-sin(OldAngle+temp) )<0.0001f )
	{
		return temp;
	}
	else
	{
		return -temp;
	}
}