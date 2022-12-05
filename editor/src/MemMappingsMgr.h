#pragma once

#include "sys/StdAfx.h"

#include "utils/SharedStruct.h"
#include "utils/common.h"

#include <mmsystem.h>

#define g_MemMappingsMgr CMemMappingsMgr::GetSingletonPtr()

struct MyStrData
{
	char text[65536];
};

struct MyMeshData
{
	struct CCUSTVERTEX
	{
		float x,  y,  z;	// position
		float nx, ny, nz;	// normal	
		long  color;		// color
		float u,  v;		// texture coords set1
	};

	MyMeshData() {};

	void Reset()
	{
		fvf			= D3DFVF_VERTEX_DESCRIPTOR;
		//tricount	= 0;
		//vertcount	= 0;
		wireframe	= true;
		//ZeroMemory( indices, sizeof( indices ) );
		//ZeroMemory( vertecies, sizeof( vertecies ) );	

		ZeroMemory( &material, sizeof( D3DMATERIAL9 ) );
		material.Diffuse = D3DXCOLOR( 0xffffffff );
		material.Ambient = D3DXCOLOR( 0xffffffff );
		culing = D3DCULL_NONE;
	};

	int			tricount;
	int			vertcount;
	int			indices[65536];
	CCUSTVERTEX	vertecies[65536];
	DWORD		fvf;
	bool		wireframe;

	D3DMATERIAL9	material;
	DWORD			culing;
	char			name[256];
};

struct STEXMTLR
{
	char			sTexID[512];
	DWORD			iMinFilter;
	DWORD			iMagFilter;
	DWORD			iMipFilter;
	DWORD			iTEXCOORDINDEX;
	DWORD			iColorOp;
	DWORD			iAlphaOp;
	DWORD			iAddressU;
	DWORD			iAddressV;
};

struct SEXTMTRL
{
	char			sMtrlID[512];
	D3DMATERIAL9	stMtrl;
	DWORD			iCulling;
	DWORD			iTexFactor;
	float			fTexFactor;
	DWORD			iShading;
	bool			bLightsEnabled;
	bool			bAlphaBlend;
	bool			bAlphaTest;
	DWORD			iFillMode;
	DWORD			iEfnRenderType;
	
	STEXMTLR		astMtrl[4];
};

class CMemMappingsMgr : public Singleton<CMemMappingsMgr>
{
	public:
		CMemMappingsMgr(void);
		~CMemMappingsMgr(void);

		CWnd*	m_pParentWnd;
		UINT	QEM_SEND_FIXED_STRING_64K;
		UINT	QEM_QUEST_UPDATE_CUSTOM_MESH;
		UINT	QEM_QUEST_UPDATE_MTRL;

		void	ReaquireMappings();

		CSharedStruct<MyMeshData>	m_MeshData;
		CSharedStruct<MyStrData>	m_StrData;
		CSharedStruct<MyStrData>	m_ExactSceneData;
		CSharedStruct<SEXTMTRL>		m_MaterialData;

		void SetParent( CWnd* pParent ) { m_pParentWnd = pParent; };
		void PostScriptToEngine( CString strScript, bool bCheckExactScene = false );
		void PostScriptToEngine( str_t strScript, bool bCheckExactScene = false ) { PostScriptToEngine( (CString)strScript.data(), bCheckExactScene ); };
		void PostScriptToEngine( char* strScript, bool bCheckExactScene = false ) { PostScriptToEngine( (CString)strScript, bCheckExactScene ); };
		void PostMessageToEngine( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );

		bool IsExactScene();
};
