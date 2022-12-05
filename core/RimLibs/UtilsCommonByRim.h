#pragma once
#pragma warning(disable: 4311 4312)

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QVector.h"

#include "CommonDefines.h"

__inline DWORD FtoDW(float f)
{
	return *((DWORD*)(&f));
}

using namespace std;

////////////////
// defines //
////////////////

//////EFN FILE VERSIONS/////
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

#define EFN_CURRENT_VERSION		EFN_VERSION_20

#define EFN_MESHLIST_VERSION_01	EFN_VERSION_18
#define EFN_MESHLIST_VERSION_02	0x11111111

#define EFN_MESHLIST_CURRENT_VERSION EFN_MESHLIST_VERSION_02

#ifndef	SAFE_DELETE_ARRAY3D
#define	SAFE_DELETE_ARRAY3D( p, len )	{ if(p) { for( int i = 0; i < len; i++ ) (p)[i]->Release();	 SAFE_DELETE_ARRAY( (p) );  } }
#endif

/////////////////////
// structures //
/////////////////////

struct EFN_CHARACTERANIMATION
{
	std::wstring	name;		//animation name
	int*		frames_list;	//frames index list 
	int			frames_count;	//frames total count
	bool		bHuinya;
	float		m_fAnimTimeLength;
	float		m_fAnimZLength;
};

struct CVERTEX
{
	float x,  y,  z;			// position
	float nx, ny, nz;			// normal
	long  color;				// color
	float u,  v;				// texture coords set1
};

struct CVERTEX_UV
{
	float u,  v;		// uv map
};

enum LIGHT_TYPE
{
	LT_POINT,
    LT_SPOT,
	LT_DIRECTED
};

struct CCOLOR
{
	float a;
	float r;
	float g;
	float b;
};

struct CLIGHT
{
	D3DXVECTOR3	pos;
	D3DXVECTOR3	dir;
	LIGHT_TYPE	type;

	std::string	name;
	CCOLOR		diffuse;
	CCOLOR		specular;
	CCOLOR		ambient;
	float		range;
	float		falloff;
	float		attenuation;
	float		multiplier;
	float		phi;

	bool		visible;
};

struct CSPHERE
{
	float		radius;
	D3DXVECTOR3	pos;
};

enum EFN_FILETYPE
{
	EF_ANIMATION,
    EF_MESHLIST
};

struct SNULL
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vRot;
	D3DXVECTOR3 vScale;
};

typedef map< std::wstring, EFN_CHARACTERANIMATION, std::less<wstring> >  mapWStrEFNANIM;	//animations list
typedef mapWStrEFNANIM::iterator mapWStrEFNANIM_iter;

typedef vector<SNULL> vecSNulls;
typedef vecSNulls::iterator vecSNulls_iter;

typedef map< std::wstring, vecSNulls, std::less<wstring> > mapWStrVecSNulls;
typedef mapWStrVecSNulls::iterator mapWStrVecSNulls_iter;

struct EFN_CHARACTERDATA
{
	std::wstring		name;							//character name
	int*				vertex_indices_list;			//index list
	int					vertex_indices_count;			//index list items count
	int					vertecies_count;				//vertecies count
	int					anim_frames_count;				//frames count
	D3DXVECTOR3*		z_coord_offset_list;			//z coord offsets for each frame
	int					vertecies_per_frame_count;		//amount of vertecies per one animation frame
	mapWStrEFNANIM		anim_list;	//animations list
	int					animations_count;				//animations count
	std::wstring		texture_filename;				// - selfexplanatory -   
	D3DXVECTOR3**		aabbList;						// list of aabbs (for each frame)
	CSPHERE*			pBoundingSpheres;				// list of bounding spheres (for each frame)
	float				global_cylinder_radius;			// global cylinder radius of the model
	D3DXVECTOR3*		posList;						//
	D3DXVECTOR3*		normList;
	CVERTEX_UV*			uvList;
	long*				vcolList;
	int**				reverse_index;
	mapWStrVecSNulls	nulls_map;
	float				fFPS;
	vecWStr				frames_script;
	D3DMATERIAL9		material;
};


///////////////////////////////////
// functions prototypes //
///////////////////////////////////

std::wstring	GetEfnVersionAsString( DWORD version );
std::wstring	GetEfnMeshlistVersionAsString( DWORD version );
DWORD			GetEfnVersion( std::wstring file );
void			ReadStrm( ifstream& stream, std::string& strValue );
void			ReadStrm( ifstream& stream, int& iValue );
void			ReadStrm( ifstream& stream, DWORD& iValue );
void			ReadStrm( ifstream& stream, float& fValue );
void			ReadStrm( ifstream& stream, D3DXVECTOR3 &vecValue );
void			ReadStrm( ifstream& stream, D3DXVECTOR2 &vecValue );
void			ReadStrm( ifstream& stream, std::wstring& strValue );
void			ReadStrm( ifstream& stream, bool& bValue );


//typedef std::list<EFN_CHARACTERANIMATION>		char_anim_list_t;
//typedef char_anim_list_t::iterator				iterator_t;