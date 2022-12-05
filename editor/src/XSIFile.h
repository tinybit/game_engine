#pragma warning(disable: 4715)

#pragma once

#include "XSIMesh.h"
#include "BaseObject.h"
#include <d3d9.h>
#include <d3dx9tex.h>
#include <string.h>
#include <SemanticLayer.h>
#include "utils/Common.h"

enum XSI_FILE_STAT
{
	XFS_OK,
	XFS_UNREADABLE,
	XFS_POLYLIST_DETECTED
};

class CXSIFile : public CBaseScene
{
	public:                   

		/* constructors */
		CXSIFile( CWnd* pParent, CWnd* pProgress ); 
		~CXSIFile()					{ Destroy(); };

		/* interface */
		CXSIMesh*	GetMeshByIndex(int index);       
		CXSIMesh*	GetMeshByName(str_t mesh_name);
		int			GetMeshIndexByName(str_t mesh_name);

		void		SetParent( CWnd* pParent )	{ m_pParent = pParent; };

		void		SetLightData( CLIGHT light, int index );
		CLIGHT*		GetLightByIndex( UINT index )	{ if( index < m_vecLights.size() )	return &m_vecLights[index];	};
		CCAMERA*	GetCameraByIndex( UINT index )	{ if( index < m_vecCameras.size() )	return &m_vecCameras[index];};

		int			GetLightIndexByName( str_t light_name );
		int			GetCameraIndexByName( str_t camera_name );
		CNULL2*		GetNullByIndex( int index );
		CNULL2*		GetNullByName( str_t strNullID );

		void		Initialize();
		void		Destroy();
		int			GetMeshesCount()		{ return m_iMeshesNum;	};
		int			GetLightsCount()		{ return m_vecLights.size();	};
		int			GetCamerasCount()		{ return m_vecCameras.size(); };
		int			GetNullsCount()			{ return m_vecNulls.size();	};
		int			GetFramesCount()		{ return m_iFramesNum; };
		str_t		GetTexturePath(void)	{ return m_szTextureFilename; };
		D3DXMATRIX	GetTransformation( CSLModel* pModel, int iFrame, CTRANSFORMEX* pTransf = NULL );

		void		SetDirectory(str_t strPath);
		void		SetTexturePath(str_t strPath);
		
		bool		Prepare(LPDIRECT3DDEVICE9 pd3dDevice);
		bool		PrepareMeshData(LPDIRECT3DDEVICE9 pd3dDevice);
		bool		PrepareSelectedMeshData(LPDIRECT3DDEVICE9 pd3dDevice);
		bool		PrepareLightData(LPDIRECT3DDEVICE9 pd3dDevice);
		bool		PrepareNullsData(LPDIRECT3DDEVICE9 pd3dDevice);

		void		Render(LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXMESH ppMesh, D3DXMATRIX mat_grid);
		void		RenderSelection(LPDIRECT3DDEVICE9 pd3dDevice);
		void		Reset(LPDIRECT3DDEVICE9 pd3dDevice) {};

		void		DeleteBuffers();
		void		SetCurrentFrame( int iFrameIndex );
		void		SwapYZCoords();
		void		SwapYZCoordsNULLS();
		void		ResetTexture( LPDIRECT3DDEVICE9 pd3dDevice );
		void		InverseMeshNormals( str_t mesh_id );
		void		ConvertToLHCoords();
		bool		LoadFromFile( str_t strDirName, str_t strFileName, bool bRepair );

		//selection stuff
		void		SetSelectedMesh( str_t mesh_id );
		void		SetSelectedLight( str_t mesh_id );
		void		SetSelectedCamera( str_t camera_id );
		void		SetNoMeshesSelected()	{	m_iSelectedMesh		= -1; };
		void		SetNoLightsSelected()	{	m_iSelectedLight	= -1; };
		void		SetNoCameraSelected()	{	m_iSelectedCamera	= -1; };
		CCAMERA*	GetSelectedCamera()		{ if( m_iSelectedCamera != -1 ) return &m_vecCameras[m_iSelectedCamera]; };

		enum XSI_ORIGINATOR	GetOriginator() { return originator; };
		int					GetCustParamsCount() { return m_vecCustParams.size(); };
		CCUSTPARAM*			GetCustParamByIndex( int iIndex ) { return &m_vecCustParams[iIndex]; };
		CCUSTPARAM*			GetCustParamByName( str_t strName );
		vector<CCUSTPARAM>* GetCustParamsList() { return &m_vecCustParams; };
		bool				IsCustParamAnimated( str_t strCustParamID );
		bool				IsCustParamAnimated( int iIndex );
		bool				m_bUVExists;

	private:
		void				Init( int iMeshesNum );
		enum XSI_FILE_STAT	Validate( str_t strPath );
		bool				DetectPolyLists( CSLModel* pModel );
		bool				DetectShapeAnimation( CSLModel* pModel );
		bool				DetectCurveAnimation( CSLModel* pModel );
		int					CountModels( CSLModel* pModel );
		int					CountNulls( CSLModel* pModel );
		int					CountLights( CSLModel* pModel );
		int					CountCameras( CSLModel* pModel );
		long				CountTriangles( CSLMesh* pMesh );
		long				CountPolygons( CSLMesh* pModel );
		void				StripScene( CSLModel* pModel );
		void				StripModel( CSLModel* pModel, CXSIMesh* mesh );
		//void				StripModelNorm( CSLModel* pModel, CXSIMesh* pMesh );
		void				StripLight( CSLModel* pModel );
		void				StripNull( CSLModel* pModel );
		void				CalcGlobalTransform( float& x, float& y, float& z, CSLModel* pModel );
		void				StripCamera( CSLModel* pModel );
		void				StripCustomParameter( CSLModel* pModel );
		void				SetVColorFromMaterial( bool	bValue, LPDIRECT3DDEVICE9 pd3dDevice );

		LPDIRECT3DVERTEXBUFFER9*	m_pVB;
		LPDIRECT3DINDEXBUFFER9		m_pIB;


		LPDIRECT3DVERTEXBUFFER9*	m_pVBSelection;
		LPDIRECT3DVERTEXBUFFER9*	m_pVBSelection2;
		LPDIRECT3DINDEXBUFFER9		m_pIBSelection;

		LPDIRECT3DVERTEXBUFFER9		m_pVBLines;
		LPDIRECT3DVERTEXBUFFER9		m_pVBNull;
		LPDIRECT3DTEXTURE9			m_pTexture;

		// MESHES
		CXSIMesh*					m_objMeshes;
		int							m_iMeshesNum;
		int							m_iSelectedMesh;

		// LIGHTS
		vector<CLIGHT>				m_vecLights;
		int							m_iDirectedLightsCount;
		int							m_iSelectedLight;
		int							m_iVisibleDirectedLightsCount;

		// NULLS
		vector<CNULL2>				m_vecNulls;

		// CAMERAS
		vector<CCAMERA>				m_vecCameras;
		int							m_iSelectedCamera;
		int							m_iVisibleCamsCount;

		// CUSTOM PARAMS
		vector< CCUSTPARAM >		m_vecCustParams;

		int							m_iFramesNum;
		int							m_iVertNum;
		int							m_iPolyNum; 
		int							m_iCurrentFrame;

		str_t						m_szTextureFilename;
		str_t						m_szFolder;  
		CWnd*						m_pParent;

		CSLScene					m_XsiScene;


		int							m_iProcessedModels;
		CWnd*						m_pDlgProgress;
		int							m_iXSIFramesOffset;
		int							m_iAmountModels;
		bool						m_bUseVertexNormals;

		// ORIGINATOR
		enum XSI_ORIGINATOR			originator;
};
