#define STRICT

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"

int float2int( float fPrm )
{ 
	return (int)fPrm; 
}

inline std::string int2str( int iNum )
{
	char b[48];
	_itoa( iNum, b, 10 );
	return b;
}

std::string int2str( int iNum, int iPosCount )
{
	std::string temp = int2str(iNum);
	int iDiff = iPosCount - temp.length();
	if ( iDiff > 0 )
	{
		std::string temp2;
		temp2.reserve( iPosCount );
		for( short i = 0; i < iDiff; i++ )
		{
			temp2.append("0");
		}
		return temp2 + temp;
	}
	else
	{
		return temp;
	}
}

inline std::wstring int2wstr( int iNum )
{
	wchar_t b[48];
	_itow( iNum, b, 10 );
	return std::wstring(b);
}

std::wstring int2wstr( int iNum, int iPosCount )
{
	std::wstring temp = int2wstr(iNum);
	int iDiff = iPosCount - temp.length();
	if ( iDiff > 0 )
	{
		std::wstring temp2;
		temp2.reserve( iPosCount );
		for( short i = 0; i < iDiff; i++ )
		{
			temp2.append( L"0" );
		}
		return temp2 + temp;
	}
	else
	{
		return temp;
	}
}

std::wstring bool2wstr( bool b )
{
	return ( b ? L"true" : L"false" );
}

std::string float2str( float n )
{
	if ( n == 0.0f )
	{
		return "0.0";
	}
	else
	{
		char b[256];
		_snprintf( b, 255, "%f", n);
		return std::string(b);
	}
}

std::wstring float2wstr( float n )
{
	if ( n == 0.0f )
	{
		return L"0.0";
	}
	else
	{
		wchar_t b[256];
		_snwprintf( b, 255, L"%f", n);
		return std::wstring(b);
	}
}

std::string dbl2str( double n )
{
	if ( n == 0.0f )
	{
		return "0.0";
	}
	else
	{
		char b[256];
		_snprintf( b, 255, "%f", n);
		return std::string(b);
	}
}

std::wstring dbl2wstr( double n )
{
	if ( n == 0.0f )
	{
		return L"0.0";
	}
	else
	{
		wchar_t b[256];
		_snwprintf( b, 255, L"%f", n);
		return std::wstring(b);
	}
}


int str2int( const std::string& s )
{
	return atoi( s.c_str() );
}

int wstr2int( const std::wstring& s )
{
	return _wtoi( s.c_str() );
}

bool wstr2bool( const std::wstring& s )
{
	return ( s==L"true" );
}

float str2float( const std::string& s )
{
	return (float)atof( s.c_str() );
}

float wstr2float( const std::wstring& s )
{
	return (float)_wtof( s.c_str() );
}

double str2dbl( const std::string& s )
{
	return atof( s.c_str() );
}

double wstr2dbl( const std::wstring& s )
{
	return _wtof( s.c_str() );
}


std::wstring str2wstr( const std::string& s )
{
	int l = s.length();
	wchar_t* ws_t = new wchar_t[l];
	MultiByteToWideChar(1251, MB_PRECOMPOSED, s.c_str(), l, ws_t, l ); 
	std::wstring ws;
	ws.reserve(l);
	ws.append( ws_t, l );
	SAFE_DELETE(ws_t);
	return ws;
}

std::string wstr2str( const std::wstring& ws )
{
	int l = ws.length();
	char* s_t = new char[l];
	WideCharToMultiByte(1251, WC_COMPOSITECHECK, ws.c_str(), l, s_t, l, NULL, NULL );
	std::string s;
	s.reserve(l);
	s.append( s_t, l );
	SAFE_DELETE(s_t);
	return s;
}

std::wstring vec3_2wstr( const D3DXVECTOR3* pv )
{
	return L"( " + float2wstr(pv->x) + L", " + float2wstr(pv->y) + L", " + float2wstr(pv->z) + L" )";
}

D3DMATERIAL9 GetMtrlDef()
{
	return GetMtrl2( &D3DXCOLOR(1,1,1,1), &D3DXCOLOR(1,1,1,1), NULL, NULL, 0 );
}

D3DMATERIAL9 GetMtrl( const D3DXCOLOR* pDiffuse )
{
	return GetMtrl2( pDiffuse, pDiffuse, NULL, NULL, 0 );
}

D3DMATERIAL9 GetMtrl2( const D3DXCOLOR* pDiffuse, const D3DXCOLOR* pAmbient, const D3DXCOLOR* pSpecular, const D3DXCOLOR* pEmissive, float fPower )
{
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	
	if ( pDiffuse != NULL )
		mtrl.Diffuse = *pDiffuse;

	if ( pAmbient != NULL )
		mtrl.Ambient = *pAmbient;

	if ( pSpecular != NULL )
		mtrl.Specular = *pSpecular;

	if ( pEmissive != NULL )
		mtrl.Emissive = *pEmissive;

	mtrl.Power = fPower;

	return mtrl;
}

SNULL GetNull( const D3DXVECTOR3* pvPos )
{
	return GetNull2( pvPos, NULL, NULL );
}

SNULL GetNull2( const D3DXVECTOR3* pvPos, const D3DXVECTOR3* pvScale, const D3DXVECTOR3* pvRot )
{
	SNULL temp;
	ZeroMemory( &temp, sizeof(SNULL) );
	if ( pvPos ) temp.vPos = *pvPos;
	if ( pvScale ) temp.vScale = *pvScale;
	if ( pvRot ) temp.vRot = *pvRot;
	return temp;
}

//---------------------------------------

void mSplit( mapWStrWStr& sl, const std::wstring& sStr, const std::wstring& sSplitter )
{
	int i=0;
	int j=0;
	int n=0;
	sl.clear();

	std::wstring s;
    while ( j != std::wstring::npos )
    {
		i = sStr.find_first_not_of(sSplitter, j);
		if( i==std::string::npos ) break;
		j = sStr.find_first_of(sSplitter, i);
		s = sStr.substr(i, j-i);
		//todo make s.trim
		sl[s] = L"ok";
		n++;
    }
}

void mSplit2( listWStr& sl, const std::wstring& sStr, const std::wstring& sSplitter )
{
	int i=0;
	int j=0;
	int n=0;
	sl.clear();

	std::wstring s;
    while ( j != std::wstring::npos )
    {
		i = sStr.find_first_not_of(sSplitter, j);
		if( i==std::string::npos ) break;
		j = sStr.find_first_of(sSplitter, i);
		s = sStr.substr(i, j-i);
		//todo make s.trim
		sl.push_back(s);
		n++;
    }
}

std::wstring mConCat( mapWStrWStr& sl, const std::wstring& l )
{
	if ( sl.size() > 0 )
	{
		std::wstring s = L"";
		for( mapWStrWStr_iter i = sl.begin(); i != sl.end(); i++)
		{
			s += i->first;
			if ( i != sl.end() )
			{
				s += l;
			}
		}
		return s;
	}
	else
	{
		return L"";
	}
}

std::string TrimStr( std::string strValue )
{
	int beg = 0;
	int end = 0;
	int counter = 0;
	
	//trim leading spaces
	while( strValue[counter] == ' ' && counter != strValue.length() )
		counter++;

	beg = counter;

	counter = strValue.length() - 1;

	//trim back spaces
	while( strValue[counter] == ' ' && counter >= 0 )
	{
		counter--;
		end++;
	}

	return strValue.substr( beg, strValue.length() - ( beg + end ) );
}

//-----------------------------------------------

void ShowMessage( const std::wstring& s )
{
	MessageBox(NULL, s.c_str(), L"Error", MB_OK);
}

iVECTOR2 dot( const iVECTOR2* v )
{
	iVECTOR2 temp;
	temp.x = -v->y;
	temp.y = v->x;
	return temp;
}

D3DXVECTOR2 dot( const D3DXVECTOR2* v )
{
	return D3DXVECTOR2( -v->y, v->x );
}

DWORD ttime()
{
	return timeGetTime();
}

float FovX2FovY( float fFovX, float fAspect )
{
	float xScale = 1 / tanf(fFovX / 2);
	float yScale = xScale * fAspect;
	return 2 * atanf(1/yScale);
};

float FovY2FovX( float fFovY, float fAspect )
{
	float yScale = 1 / tanf(fFovY / 2);
	float xScale = yScale / fAspect;
	return 2 * atanf(1/xScale);
};

float AngleDif( float NewAngle, float OldAngle )
{
	if ( NewAngle < OldAngle )
		NewAngle = NewAngle + D3DX_2PI;

	float temp =  fabs(NewAngle - OldAngle);

	if ( temp >= D3DX_PI ) temp = D3DX_2PI - temp;

	return ( ( fabs( cosf(NewAngle)-cosf(OldAngle+temp) )<0.0001f && 
				fabs( sinf(NewAngle)-sinf(OldAngle+temp) )<0.0001f ) ? temp : -temp );

	//float temp = NewAngle + D3DX_2PI - OldAngle;
	//if(temp > D3DX_2PI) temp -= D3DX_2PI;

	//float diff = abs(NewAngle - OldAngle);

	//if ( diff > D3DX_PI ) 
	//{
	//	diff = D3DX_2PI - diff; 
	//}

	//if ( temp<D3DX_PI )
	//{
	//	return diff;	
	//}
	//else 
	//{
	//	return -diff;	
	//}
}