#pragma once
#pragma warning(disable: 4311 4312)

#include "dxstdafx.h"
#include "stl_inc.h"

#include "CommonDefines.h"

#include "UtilsCommonByRim.h"

using namespace std;

enum ObjRenderTypes
{
	ORT_NONTRANSPARENT,
	ORT_TRANSPARENT,
	ORT_FILLZFORTRANSPARENTWITHOUTZ
};

struct SResTmpl
{
	int		iUseCounter;
	double	fLastTimeUse;
};

struct SRay
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vTar;
};

struct CCAMERA
{
	int						id;
	std::string				name;

	D3DXVECTOR3				pos;
	D3DXVECTOR3				targ;
	D3DXVECTOR3				upvec;
	D3DXVECTOR2				fov;

	D3DXVECTOR2				back_dim;

	float					z_near;
	float					z_far;
	float					zoom_near;
	float					zoom_far;

	bool					visible;
};

struct SCAMERA
{
	D3DXVECTOR3				vPerPos;
	D3DXVECTOR3				vPerTarg;
	D3DXVECTOR3				vPerUpVec;
	D3DXVECTOR2				vPerBackSize;
	float					fPerAspect;
	D3DXVECTOR2				vPerFov;
	float					fPerNearPlane;
	float					fPerFarPlane;

	D3DXVECTOR3				vOrthoPos;
	D3DXVECTOR3				vOrthoTarg;
	D3DXVECTOR3				vOrthoUpVec;
	D3DXVECTOR2				vOrthoBackSize;
	float					fOrthoAspect;
	float					fOrthoNearPlane;
	float					fOrthoFarPlane;

	float					fZoomNear;
	float					fZoomFar;
};

struct iVECTOR2
{
	int x;
	int y;
};

struct SCheckPoint
{
	SNULL		p;
	iVECTOR2	g;
	double		l;
};

typedef vector<SCheckPoint> cpvector;
typedef cpvector::iterator cpvec_iter;

enum QEngineEvents
{
	ON_INIT,
	ON_KILL
};

struct POS_TEX_VERTEX
{
    D3DXVECTOR3 p;
    FLOAT       u, v;
};
#define POS_TEX_FVF ( D3DFVF_XYZ|D3DFVF_TEX1 )

struct POSRHW_COL_VERTEX
{
	D3DXVECTOR4	p;
	D3DCOLOR	c;
};
#define POSRHW_COL_FVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )