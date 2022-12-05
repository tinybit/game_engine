#pragma once
#pragma warning(disable: 4311 4312)

#include "dxstdafx.h"
#include "stl_inc.h"

using namespace std;

enum ObjTypes
{
	OT_MAN,
	OT_CAR,
	OT_BIRD,
	OT_FREE,
	OT_CAMERA,
	OT_LIGHT,
	OT_FLOOR,
	OT_SHADOWVOLUME,
	OT_LAYERGEOM,
	OT_ACTION,
	OT_BACK,
	OT_SCENE,
	OT_REFLECTION,
	OT_CAMSELECT,
	OT_EFNANIM,
	OT_VIDEOANIM,
	OT_3DVIDEO,
	OT_MENURUKZAK,
	OT_MENUCURSOR,
	OT_MENUMENU
};

struct SWStrWStr
{
	std::wstring s1;
	std::wstring s2;
};

typedef std::vector<SWStrWStr>	vecWStrWStr;
typedef vecWStrWStr::iterator	vecWStrWStr_iter;

typedef list<std::wstring>		listWStr;
typedef listWStr::iterator		listWStr_iter;

typedef list<std::wstring>		listStr;
typedef listStr::iterator		listStr_iter;

typedef list<float>				listFloat;
typedef listFloat::iterator		listFloat_iter;

typedef list<D3DXVECTOR3>		listVec3;
typedef listVec3::iterator		listVec3_iter;

typedef	vector< std::string >	vecStr;
typedef	vecStr::iterator		vecStr_iter;

typedef	vector< std::wstring >	vecWStr;
typedef	vecWStr::iterator		vecWStr_iter;

typedef	map< std::wstring, std::wstring, std::less<wstring> > mapWStrWStr;
typedef	mapWStrWStr::iterator	mapWStrWStr_iter;

typedef map< std::wstring, bool, std::less<wstring> > mapWStrBool;
typedef mapWStrBool::iterator	mapWStrBool_iter;

typedef map< std::wstring, int, std::less<wstring> > mapWStrInt;
typedef mapWStrInt::iterator	mapWStrInt_iter;

typedef map< int, std::wstring, std::less<int> > mapIntWStr;
typedef mapIntWStr::iterator	mapIntWStr_iter;

typedef	multimap< std::wstring, std::wstring, std::less<wstring> > mmapWStrWStr;
typedef	mmapWStrWStr::iterator	mmapWStrWStr_iter;

typedef multimap< std::wstring, int, std::less<wstring> > mmapWStrInt;
typedef mmapWStrInt::iterator	mmapWStrInt_iter;

template <typename T> class Singleton
{
	static T* ms_Singleton;

public:
	Singleton( void )
	{
		assert( !ms_Singleton );
		int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
		ms_Singleton = (T*)((int)this + offset);
	}
	~Singleton( void)
	{
		assert( ms_Singleton ); ms_Singleton = 0; 
	}
	static T& GetSingleton( void )
	{
		assert( ms_Singleton ); return ( *ms_Singleton );
	}
	static T* GetSingletonPtr( void )
	{
		return ( ms_Singleton ); 
	}
};

template <typename T> T* Singleton <T>::ms_Singleton = 0;