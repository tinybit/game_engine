// Common.h
// Common utilities, enums and defines
// NB: compile with: /EHsc
#pragma once

#include "../sys/StdAfx.h"
#include "../BaseScene.h"
#include "../utils/enums.h"
#include <locale>
#include <vector>
#include <list>
#include <map>
#include <hash_map>
#include <algorithm>
#include <assert.h>
#include <d3d9.h>
#include <d3dx9math.h>
#include "commondefines.h"
#include "io_utils.h"
#include "math_utils.h"

using namespace std;

//////////////////////////
//		VERSIONING		//
//////////////////////////

// editor versions
#ifndef	EDITOR_VERSION
#define	EDITOR_VERSION "1.3"
#endif

#ifndef	EDITOR_BUILD
#define	EDITOR_BUILD "0034"
#endif

#ifndef	EDITOR_DATA
#define	EDITOR_DATA "17.july.2006"
#endif

#ifndef	DEFAULT_CAMERA_ID
#define	DEFAULT_CAMERA_ID " -- Editor camera -- "
#endif

//defines
#ifndef D3DFVF_VERTEX_DESCRIPTOR
#define D3DFVF_VERTEX_DESCRIPTOR ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#endif

// | D3DFVF_TEXCOORDSIZE2(0)

//typedef hash_map<str_t, long>			hmapSrtLong;
//typedef hmapSrtLong::iterator			hmapSrtLong_iter;

struct VERTEX2D_RHW
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex colorc
};

#define D3DFVF_VERTEX2D_RHW ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )
#define D3DFVF_VERTEX3D ( D3DFVF_XYZ )


// efn file versions
#define EFN_VERSION_01		0xFFFFFF01
#define EFN_VERSION_02		0xFFFFFF02
#define EFN_VERSION_03		0xFFFFFF03
#define EFN_VERSION_04		0xFFFFFF04
#define EFN_VERSION_05		0xFFFFFF05
#define EFN_VERSION_06		0xFFFFFF06
#define EFN_VERSION_07		0xFFFFFF07
#define EFN_VERSION_08		0xFFFFFF08
#define EFN_VERSION_09		0xFFFFFF09
#define EFN_VERSION_10		0xFFFFFF10
#define EFN_VERSION_11		0xFFFFFF11
#define EFN_VERSION_12		0xFFFFFF12
#define EFN_VERSION_13		0xFFFFFF13
#define EFN_VERSION_14		0xFFFFFF14
#define EFN_VERSION_15		0xFFFFFF15
#define EFN_VERSION_16		0xFFFFFF16
#define EFN_VERSION_17		0xFFFFFF17
#define EFN_VERSION_18		0xFFFFFF18
#define EFN_VERSION_19		0xFFFFFF19
#define EFN_VERSION_20		0xFFFFFF20
#define EFN_VERSION_21		0xFFFFFF21
#define EFN_VERSION_22		0xFFFFFF22
#define EFN_VERSION_23		0xFFFFFF23


#define EFN_SUPP_VERSION_01	EFN_VERSION_18
#define EFN_SUPP_VERSION_02	EFN_VERSION_19
#define EFN_SUPP_VERSION_03 EFN_VERSION_20
#define EFN_SUPP_VERSION_04 EFN_VERSION_21
#define EFN_SUPP_VERSION_05 EFN_VERSION_22
#define EFN_SUPP_VERSION_06 EFN_VERSION_23

#define EFN_CURR_VERSION EFN_SUPP_VERSION_06

enum EFN_COMPRESSION_TYPE
{
	ECT_NONE		= 2,
	ECT_SHORT		= 4,
	ECT_SHORT_RLE	= 8
};


#define EFN_MESHLIST_VERSION_01	EFN_VERSION_18
#define EFN_MESHLIST_VERSION_02	0x11111111

#define CANTWALK			4294967295	//2^32-1
#define CANWALK				4294967294	//2^32-2

// scn file versions
#define SCN_VERSION_01		0xFFFFFF01
#define SCN_VERSION_02		0xFFFFFF02
#define SCN_VERSION_03		0xFFFFFF03
#define SCN_VERSION_04		0xFFFFFF04
#define SCN_VERSION_05		0xFFFFFF05

// scn file versions
#define SCN_CURR_VERSION SCN_VERSION_05

#define BEGIN( funcname ) funcname { string func_name = #funcname; try { 
#define END } catch(...) { g_ExeptLog->__data.push_back( func_name ); throw; }  }

//////////////////////////////////////
//		structures and classes		//
//////////////////////////////////////

struct ModelStates
{
	bool        IsCullingOn;
	RENDER_MODE RenderMode;
};

struct ModelWalkStates
{
	bool        IsCullingOn;
	RENDER_MODE RenderMode;
};

struct CurveItem
{
	CurveItem() : type(LINEAR), id(""), length(0), visible(true), selected(false) {}; 
	CurveItem( str_t ID ) : type(LINEAR), id(ID), length(0), visible(true), selected(false) {}; 

	str_t			id;
	enum CURVE_TYPE type;
	vector<str_t>	nulls;
	float			length;
	bool			visible;
	bool			selected;
};

struct TextResItem
{
	TextResItem() {};
	TextResItem( str_t _id, str_t _comments, str_t _data )
	{ 
		id			= _id;
		comments	= _comments;
		data		= _data;
	};

	str_t	id;
	str_t	comments;
	str_t	data;
};

struct CTRANSFORM
{
	CTRANSFORM()
	{
		rot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		scl = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

	D3DXVECTOR3 pos;
	D3DXVECTOR3	rot;
	D3DXVECTOR3	scl;
};

struct CTRANSFORMEX : public CTRANSFORM
{
	CTRANSFORMEX()
	{
		D3DXMatrixIdentity( &pre_transform );
	};

	D3DXMATRIX	pre_transform;

	void operator=( const CTRANSFORM& tr )
	{
		pos = tr.pos;
		rot = tr.rot;
		scl = tr.scl;
	}
};

struct CBASENULL
{
	str_t					id;
	vector<CTRANSFORMEX>	matrices;
};

struct CNULL
{
	str_t				id;
	float**				matrices; //frame, values ( tr,rot,scale )
	CBaseScene*			attachment;
};

struct CNULL2 : public CBASENULL
{
	CNULL2() 
	{
	};

	~CNULL2() 
	{ 
		for( UINT i = 0; i < attaches.size(); i++ )
			SAFE_DELETE( attaches[i] ); 
	};

	vector<CBaseScene*> attaches;
};

struct CCUSTPARAM
{
	str_t			id;
	vector<float>	data;
};

struct CCOLOR
{
	float a;
	float r;
	float g;
	float b;

	CCOLOR() {};

	CCOLOR( float _a, float _r, float _g, float _b )
	{
		a = _a;
		r = _r;
		g = _g;
		b = _b;
	};
};

struct CLIGHT
{
	D3DXVECTOR3	pos;
	D3DXVECTOR3	targ;
	LIGHT_TYPE	type;

	str_t		name;
	CCOLOR		diffuse;
	CCOLOR		specular;
	CCOLOR		ambient;
	float		range;
	float		falloff;
	float		attenuation;
	float		multiplier;
	float		phi;

	bool		visible;

	void Reset()
	{
		pos		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		targ	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		type	= LT_POINT;
		name	= "";
		diffuse = CCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
		specular	= CCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
		attenuation	= 1.0f;
		multiplier	= 1.0f;
		ambient = CCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
		range	= 0.0f;
		falloff = 1.0f;
		phi		= 45.0f;
		visible	= true;	
	}

	CLIGHT() { Reset(); };
	CLIGHT( str_t strID ) { Reset(); name = strID; };
};

struct CCAMERA
{
	D3DXVECTOR3	pos;
	D3DXVECTOR3 dir;
	D3DXVECTOR3 upvector;
	str_t		name;
	float		hfov;
	float		vfov;

	bool		visible;
};


//struct SCAMERA_LOAD;
struct CCAMERA2;

struct SCAMERA_LOAD
{
	SCAMERA_LOAD() {};
	SCAMERA_LOAD( CCAMERA2& cam );

	D3DXVECTOR3				vPerPos;
	D3DXVECTOR3				vPerTarg;
	D3DXVECTOR3				vPerUpVec;
	D3DXVECTOR2				vPerFov;
	float					fPerNearPlane;
	float					fPerFarPlane;

	float					fZoomNear;
	float					fZoomFar;

	float					fFogStart;
	float					fFogEnd;
	float					fFogDensity;
	D3DCOLOR				iFogColor;
	UINT					iFogType;

	bool					bApplyEffectsOnShadow;
};

struct CCAMERA2
{
	int			id;
	str_t		name;

	D3DXVECTOR3	pos;
	D3DXVECTOR3 targ;
	D3DXVECTOR3 upvec;
	D3DXVECTOR2	fov;

	D3DXVECTOR2	back_dim;

	float		z_near;
	float		z_far;
	float		zoom_near;
	float		zoom_far;

	bool		visible;

	float		fFogStart;
	float		fFogEnd;
	float		fFogDensity;
	D3DCOLOR	iFogColor;
	UINT		iFogType;

	bool		bApplyEffectsOnShadow;

	CCAMERA2() { Init(); };

	CCAMERA2( str_t strID ) 
	{
		Init();

		name	= strID;
	};

	void Init()
	{
		id			= -1;
		name		= "";
		pos			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		targ		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		upvec		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		fov			= D3DXVECTOR2( 0.0f, 0.0f );
		back_dim	= D3DXVECTOR2( 0.0f, 0.0f );

		z_far		= 100.0f;
		z_near		= 0.1f;
		zoom_far	= 0.0f;
		zoom_near	= 0.0f;

		visible		= true;

		fFogEnd		= 0.0f;
		fFogStart	= 0.0f;
		iFogColor	= 0;
		iFogType	= 0;
		fFogDensity = 0.0f;
		bApplyEffectsOnShadow = true;
	}

	void operator=( const CCAMERA& cam )
	{
		name		= cam.name;
		pos			= cam.pos;
		targ		= cam.dir;
		fov.x		= cam.hfov;
		fov.y		= cam.vfov;
		upvec		= cam.upvector;
		visible		= cam.visible;
	};

	void operator=( const SCAMERA_LOAD& cam )
	{
		pos			= cam.vPerPos;
		targ		= cam.vPerTarg;		
		upvec		= cam.vPerUpVec;		
		fov			= cam.vPerFov;			
		z_near		= cam.fPerNearPlane;
		z_far		= cam.fPerFarPlane;	
							
		zoom_near	= cam.fZoomNear;	
		zoom_far	= cam.fZoomFar;		
							
		fFogStart	= cam.fFogStart;		
		fFogEnd		= cam.fFogEnd;			
		fFogDensity	= cam.fFogDensity;
		iFogColor	= cam.iFogColor;		
		iFogType	= cam.iFogType;		
							
		bApplyEffectsOnShadow = cam.bApplyEffectsOnShadow;
	};
};


struct CVERTEX
{
	float x,  y,  z;	// position
	float nx, ny, nz;	// normal	
	long  color;		// color
	float u,  v;		// texture coords set1

	bool operator==( const CVERTEX& vertex )
	{
		if( x == vertex.x && y == vertex.y && z == vertex.z && u == vertex.u && v == vertex.v )
			return true;
		else
			return false;
	};

	bool operator^( const CVERTEX& vertex )
	{
		if( (fabs(x - vertex.x) < EPSILON) && (fabs( y - vertex.y ) < EPSILON) && (fabs( z - vertex.z ) < EPSILON) )
			return true;
		else
			return false;
	};

	bool operator%=( const CVERTEX& vertex )
	{
		if( x == vertex.x && y == vertex.y && z == vertex.z )
			return true;
		else
			return false;
	};

	bool operator|=( const CVERTEX& vertex )
	{
		if( ( x == vertex.x && y == vertex.y && z == vertex.z ) && (u != vertex.u || v != vertex.v) )
			return true;
		else
			return false;
	};

	bool PosComp( const CVERTEX& vertex )
	{
		if( x == vertex.x && y == vertex.y && z == vertex.z )
			return true;
		else
			return false;
	};
};

struct CBASEVERTEX
{
	float x, y, z;		// position

	bool operator==( const CBASEVERTEX& vertex )
	{
		if( x == vertex.x && y == vertex.y && z == vertex.z )
			return true;
		else
			return false;
	};
};

struct CVERTEX_XYZ_COL
{
	float x, y, z;		// position
	long  color;		// color
};

struct CVERTEX_XYZ
{
	float x, y, z;		// position

	bool operator==( const CVERTEX_XYZ& vertex )
	{
		if( x == vertex.x && y == vertex.y && z == vertex.z )
			return true;
		else
			return false;
	};

	bool operator^( const CVERTEX_XYZ& vertex )
	{
		if( (fabs(x - vertex.x) < 0.000001f ) && (fabs( y - vertex.y ) < 0.000001f ) && (fabs( z - vertex.z ) < 0.000001f ) )
			return true;
		else
			return false;
	};

	bool operator!=( const CVERTEX_XYZ& vertex )
	{
		if( x != vertex.x || y != vertex.y || z != vertex.z )
			return true;
		else
			return false;
	};
};

struct CVERTEX_UV
{
	float u, v;		// uv map

	bool operator==( const CVERTEX_UV& vertex )
	{
		if( u == vertex.u && v == vertex.v )
			return true;
		else
			return false;
	};

	bool operator!=( const CVERTEX_UV& vertex )
	{
		if( u != vertex.u || v != vertex.v )
			return true;
		else
			return false;
	};
};

struct GridStates
{
	int		Size;
	float	CellSize;
	int		LinesCount;
};

// storage for quickbind operation
struct QuickBind
{
	QuickBind()
	{
		m_strChildObjType	= "";
		m_strChildType		= "";
		m_strParentID		= "";
	}

	// physical obj type, e.g. light, mesh, texture, etc.
    str_t			m_strChildObjType;

	// self-explanatory
	str_t			m_strChildType;
	str_t			m_strParentID;
};

struct AppSettingsData
{
	// general
	CString		FilesPath;
	CString		StartParams;
	CString		EngineFolder;
	FILE_TYPE	DefaultFileType;
	bool		RepairXSIFiles;		// repair xsi files ( originator err in deep exploration )
	bool		FilterObjsByPrefix;	// filter objects by prefixes in binds editor
	bool		ExpertMode;			
	bool		AskForSavingOnAppQuit;
	bool		FirstMonitorOnly;
	bool		EngineDebugModeOn;
	bool		SwitchBackOnDebug;

	// lighting
	bool		LightingEnabled;
	bool		AttachToCamera;
	COLORREF	LightAmbientColor;
	COLORREF	LightDiffuseColor;
	COLORREF	AmbientColor;

	//camera
	CCAMERA		DefaultCamera;
	CTRANSFORM	DefaultWorldStates;
	bool		ResetCameraOnFileOpen;
	bool		CameraToCurrent;

	//grid
	bool		GridVisible;
	int			GridSize;
	float		GridCellSize;
	COLORREF	GridWiresColor;
	COLORREF	GridCrossColor;

	//viewport
	COLORREF	ViewportBackColor;
	bool		ViewportBackfaceCull;
	bool		ViewportPreserve43;

	//cam light icons
	float		IconsScaling;

	//recent files&dirs entries
	vector<CString>		RecentDirs;
	vector<CString>		RecentFiles;

	bool		RecentFilesClear;

	//alternate null icons in SCN files
	bool		ScnAlternateNullIco;

	CString		Button_A_Capt;
	CString		Button_A_Path;
	CString		Button_B_Capt;
	CString		Button_B_Path;
};

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

/////////////////////////
// function prototypes //
/////////////////////////	

// 1) string utils
char*			_2Upper( char* string );
char*			_2Lower( char* string );
str_t			_2Upper( str_t& string );
str_t			_2Lower( str_t& string );

std::string		TrimStr( std::string strValue );

// 2) math utils
CVector*		ToCVec(const D3DXVECTOR3 vec);
CVector*		ToCVec(const D3DXVECTOR4 vec);
D3DXVECTOR3&	ToDX3Vec(const CVector vec);
float			FovX2FovY(float fFovX, float fAspect);
float			FovY2FovX(float fFovY, float fAspect);

// 3) misc utils
void			EnableMenuItem( CMenu* menu, int ID, bool enable );
DWORD			GetEfnVersion( str_t file );
str_t			GetEfnVersionAsString( DWORD version );
enum EFN_FILETYPE GetEfnFiletype( str_t path );
DWORD			GetScnVersion( str_t file_path );
str_t			GetScnVersionAsString( DWORD version );
void			GetTypesList( StrVec &listTypes );
DWORD			GetEfnMeshlistVersion( str_t file_path );
str_t			GetEfnMeshlistVersionAsString( DWORD version );

void	RemoveDupesFromVector( std::vector <int>* vec );
void	GetARGBFromDWORD( int& a, int& r, int& g, int& b, DWORD dwColor );

str_t	GetObjTypeStr( enum ObjTypes obj_t);
void	CorrectXSIFile( str_t path);
str_t	StripTypeHeader( str_t &strValue );
CString CSTRim( CString str );
str_t	GetPrexifFromType( str_t strType );

inline DWORD FtoDW( float f )
{
	return *((DWORD*)(&f));
}

void DeleteFuncEntriesByKey( IFuncMap* pFuncs, int iKey );