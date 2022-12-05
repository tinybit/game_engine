#pragma once
#pragma warning( disable : 4267 4800 )
#pragma inline_recursion(on)

#include "UtilsDefines.h"

int				float2int( float fPrm );

std::string		int2str( int iNum );
std::string		int2str( int iNum, int iPosCount );
std::wstring	int2wstr( int iNum );
std::wstring	int2wstr( int iNum, int iPosCount );
std::string		float2str( float n );
std::wstring	float2wstr( float n );
std::string		dbl2str( double n );
std::wstring	dbl2wstr( double n );
std::wstring	bool2wstr( bool b );

int				str2int( const std::string& sStr );
int				wstr2int( const std::wstring& s );
float			str2float( const std::string& s );
float			wstr2float( const std::wstring& s );
double			str2dbl( const std::string& s );
double			wstr2dbl( const std::wstring& s );
bool			wstr2bool( const std::wstring& s );

std::wstring	str2wstr( const std::string& s );
std::string		wstr2str( const std::wstring& ws );

std::wstring	vec3_2wstr( const D3DXVECTOR3* pv );

D3DMATERIAL9	GetMtrlDef();
D3DMATERIAL9	GetMtrl( const D3DXCOLOR* pDiffuse );
D3DMATERIAL9	GetMtrl2( const D3DXCOLOR* pDiffuse, const D3DXCOLOR* pAmbient, const D3DXCOLOR* pSpecular, const D3DXCOLOR* pEmissive, float fPower );

SNULL			GetNull( const D3DXVECTOR3* pvPos );
SNULL			GetNull2( const D3DXVECTOR3* pvPos, const D3DXVECTOR3* pvScale, const D3DXVECTOR3* pvRot );

//-----------------------------------------------------------
std::string		TrimStr( std::string strValue );

void			mSplit( mapWStrWStr& sl, const std::wstring& sStr, const std::wstring& sSplitter );
void			mSplit2( listWStr& sl, const std::wstring& sStr, const std::wstring& sSplitter );
std::wstring	mConCat( mapWStrWStr& sl, const std::wstring& l );

void			ShowMessage( const std::wstring& s );

iVECTOR2		dot( const iVECTOR2* v );
D3DXVECTOR2		dot( const D3DXVECTOR2* v );

DWORD			ttime();

float			FovX2FovY( float fFovX, float fAspect );
float			FovY2FovX( float fFovY, float fAspect );

float			AngleDif( float NewAngle, float OldAngle );