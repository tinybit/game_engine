#pragma once

#include "XSIFile.h"
#include "Mesh.h"
#include "utils/common.h"
#include "EfnCharacter.h"
#include <d3d9.h>


#define CURVE_STEP 20

enum TESS_MESH_DLG_MODE
{
	TMDM_UNDEFINED	= 0,
	TMDM_BOX		= 1,
	TMDM_CONE		= 2,
	TMDM_CYLINDER	= 3,
	TMDM_SPHERE		= 4,
	TMDM_QUAD		= 5
};

class CSceneFile : public CBaseScene
{
	public:
		/* CONSTRUCTORS */
		CSceneFile(void)	{ Initialize(); };
		~CSceneFile(void)	{ Destroy(); };

		/* INTERFACE */
		void			Initialize();
		void			Destroy();
		void			SafeReleaseFuncsBuffer();
		
		bool			IsSupportedVersion( DWORD dwVersion );
		bool			LoadFromFile( str_t path );
		bool			SaveToFile( str_t path );

		bool			Prepare( LPDIRECT3DDEVICE9 pd3dDevice );
		void			Render( LPDIRECT3DDEVICE9 pd3dDevice );
		void			RenderShadowMesh( LPDIRECT3DDEVICE9 pd3dDevice );
		void			Reset(LPDIRECT3DDEVICE9 pd3dDevice);
		bool			InitFromXSICharacter( CXSIFile* xsi_file );
		
		void			SetLevelID( str_t strID )	{ m_strSceneID = strID; };
		str_t			GetLevelID()				{ return m_strSceneID; };
		str_t			GetSceneID();

		//shadow mesh funcs
		void			BeginShadowMeshBoxDrawing( D3DXVECTOR3* pos );
		void			SetShadowMeshParams( D3DXVECTOR3* size, D3DXVECTOR3* tesselation );
		void			EndShadowMeshDrawing();
		void			CreateMeshFromShadowMesh( CString ID, D3DXVECTOR3* pos, D3DXVECTOR3* size, D3DXVECTOR3* tesselation, LPDIRECT3DDEVICE9 pd3dDevice );

		void			PrepareCurves();
		int				GetCamerasCount()		{ return m_vecCameras.size(); };
		int				GetLightsCount()		{ return m_vecLights.size(); };
		bool			IsGridVisible()			{ return m_bGridVisible; };
		void			ShowGrid( bool bShow );
		bool			IsGridSelectorsVisible(){ return m_bGridSelectorsVisible; };
		void			ShowGridSelectors( bool bShow );
		void			RecalcGrid();
		void			RecalcGridUV();
		void			SetLightBulbCone( CBaseScene* pBulb, CBaseScene* pCone ) { m_pLightBulb = pBulb; m_pLightCone = pCone; };
		void			SetCamBox( CBaseScene* pBaseScene ) { m_pCamBox = pBaseScene; };
		void			SetSelectedCamera( str_t strCamID );
		CCAMERA2*		GetSelectedCamera() { if( m_iSelectedCamera != -1) return m_vecCameras[m_iSelectedCamera]; return NULL; };
		void			SetNoCameraSelected() { m_iSelectedCamera = -1; };
		void			SetSelectedLight( str_t strLightID );
		void			SetNoLightSelected() { m_iSelectedLight = -1; };
		void			DeleteAllLights();
		void			DeleteAllCameras();
		void			DeleteLight( str_t strID );
		void			DeleteCamera( str_t strID );
		void			TranslateWholeSceneHack( D3DXVECTOR3* pTranslate );

		CCAMERA2*		GetCameraByIndex( int iIndex )		{ if( iIndex >= 0 && iIndex < (int)m_vecCameras.size() ) return m_vecCameras[iIndex]; return NULL; };
		CCAMERA2*		GetCameraByID( str_t strCamID );
		CCAMERA2*		AddCamera( str_t strCamID );
		CLIGHT*			GetLightByIndex( int iIndex )			{ if( iIndex >= 0 && iIndex < (int)m_vecLights.size() ) return m_vecLights[iIndex]; return NULL; };
		CLIGHT*			GetLightByID( str_t strLightID );
		CLIGHT*			AddLight( str_t strLightID );
		CBaseScene*		GetMeshlist()									{ return m_pMeshes; };
		ListStrVec*		GetBinds()										{ return &m_lBinds; };
		IFuncMap*		GetFuncs()										{ return &m_mapFuncs; };
        StrVec*			GetTextures()									{ return &m_vecTextures; };
		void			SetProgressWnd( CWnd* pWnd )	{ m_pProgressWnd	= pWnd; };
		void			SetParent( CWnd* pWnd );
		void			ShowAllCameras( bool bShow );
		void			ShowAllLights( bool bShow );
		void			PrepareCamsLines( LPDIRECT3DDEVICE9 pd3dDevice );
		void			PrepareLightsLines( LPDIRECT3DDEVICE9 pd3dDevice );
		void			SetCellSize( float fCellSize )		{ m_fCellSize = fCellSize; };
		float			GetCellSize()						{ return m_fCellSize; };
		void			SetCamZoomNear( bool bIsActive )	{ m_bIsCamZoomNearActive  = bIsActive; m_bIsCamZoomFarActive = false; };
		void			SetCamZoomFar( bool bIsActive )		{ m_bIsCamZoomFarActive  = bIsActive; m_bIsCamZoomNearActive = false; };
		bool			GetCamZoomNear() { return m_bIsCamZoomNearActive; };
		bool			GetCamZoomFar() { return m_bIsCamZoomFarActive; };
		void			SetVColorFromMaterial( bool	bValue, LPDIRECT3DDEVICE9 pd3dDevice );
		float			IntersectByRay( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation, enum OBJECT_TYPES& objType, str_t& strObjID );
		void			ShowEverything( bool bValue, LPDIRECT3DDEVICE9 pd3dDevice );
		void			ShowIsLightRangeVisible( bool bValue ) { m_bIsLightRangeVisible = bValue; };
		bool			GetIsLightRangeVisible() { return m_bIsLightRangeVisible; };
		StrVec*			GetCustObjList()	{ return &m_vecCustomObjects; };
		StrVec*			GetCustTypesList()	{ return &m_vecCustomTypes; };
		bool			AddTextResource( str_t ID );
		TextResItem*	GetTextResourceByName( str_t ID );
		TextResItem*	GetTextResourceByIndex( int index );
		int				GetTextResourceCount() { return m_vecTextRes.size(); };
		void			DeleteTextResource( TextResItem* item );
		TextResItem*	AddNewTextResource();
		CurveItem*		GetCurveByName( str_t strID );
		void			DeleteCurve( str_t strID );
		CurveItem*		GetCurveByIndex( int iIndex );
		int				GetCurvesCount() { return m_vecCurves.size(); };
		void			AddNewCurve( str_t ID );
		void			DeselectAllCurves();
		void			DeleteAllCurves();
		void			ShowAllCurves( bool bValue );
		void			RemoveNullFromCurves( str_t strNullID );
		void			RenameNullInCurves( str_t strCurrNullID, str_t strNewNullID );
		str_t			GetVersion() { return GetScnVersionAsString( m_dwFileVersion ); };
		void			SetEditedObject( str_t strEditedObject ) { m_strEditedObject = strEditedObject; }; 
		str_t			GetEditedObject() { return m_strEditedObject; }; 
		void			Scale( D3DXMATRIX mScale );
		void			BuildFuncsList();

		// HACK!!! ( remembers last edited object in scripts dialog )
		str_t			m_strLastEditedObj;
		str_t			m_strLastEditedObj_FilterParType;
		str_t			m_strLastEditedObj_FilterParID;

	private:

		/*************** util functions ******************/
		bool			LoadMeshlist();
		D3DXVECTOR3		CalcCatmullSpline3D( D3DXVECTOR3* a, D3DXVECTOR3* b, D3DXVECTOR3* c, D3DXVECTOR3* d, float u );
		void			PrepareGridTexture( LPDIRECT3DDEVICE9 pd3dDevice );

		void			RenderCams( LPDIRECT3DDEVICE9 pd3dDevice );
		void			RenderLights( LPDIRECT3DDEVICE9 pd3dDevice );
		void			RenderCurves( LPDIRECT3DDEVICE9 pd3dDevice );
		void			RenderLightCone( LPDIRECT3DDEVICE9 pd3dDevice, CLIGHT* light );

		void			IntersectFloorMeshesWithGrid( StrVec* FloorMeshes, vector<int>** pGrid, D3DXVECTOR2* pvMin, float fCellSize );
		void			IntersectCamSelectorsWithGrid( int** pCamSelGrid, StrVec* pSelectorNames, D3DXVECTOR2* pvMin, float fCellSize, int iGridMaxX, int iGridMaxZ );
		void			DeleteGrid();
		bool			PrepareGridDataFloors( vector<int>* MeshIndexes, vector<float>* TriMeshDims, vector<bool>* TriMeshEdges, vector<float>*	MeshDims, StrVec* FloorMeshes, float fCellSize );
		bool			PrepareGridDataSelectors( vector<float>* TriMeshDims, vector<float>* MeshDims, StrVec* FloorMeshes, float fCellSize );
		void			CalcAzimuthAnglesFromVec( D3DXVECTOR3* from, D3DXVECTOR3* to, float& angX, float& angY, bool bLightHack = false );

		/*********************** data ********************/
		str_t		m_strSceneID;
		DWORD		m_dwFileVersion;
		CBaseScene*	m_pMeshes;
		ListStrVec	m_lBinds;
		StrVec		m_vecTextures;
		StrVec		m_vecCustomObjects;
		StrVec		m_vecCustomTypes;

		vector<TextResItem*>	m_vecTextRes;
		vector<CCAMERA2*>		m_vecCameras;
		//vector<SCAMERA_LOAD*>	m_vecCameras;
		vector<CLIGHT*>			m_vecLights;
		vector<CurveItem*>		m_vecCurves;
		D3DXVECTOR3*			m_pCurves;


		/** FLOORS **/
		D3DXVECTOR2	m_vMin, m_vMax;				// min, max of floor geometry
		float		m_fCellSize;				// size of single cell in grid (square)
		int			m_iXGridSize, m_iZGridSize;	// size of grid in cells count
		int***		m_pppGrid;					// grid
		bool		m_bGridVisible;
		bool		m_bGridSelectorsVisible;
		D3DXVECTOR3* m_pFloorsEdge;
		int			m_iEdgesCount;

		/*********************** misc data ********************/
		CWnd*		m_pProgressWnd;
		CWnd*		m_pParentWnd;

		LPDIRECT3DTEXTURE9		m_pTexture;				// floor texture

		//lights vis data
		CBaseScene*				m_pLightBulb;
		CBaseScene*				m_pLightCone;
		LPDIRECT3DVERTEXBUFFER9	m_pVBLightsLines;
		int						m_iSelectedLight;
		int						m_iVisibleLightsCount;
		
		//cams vis data
		CBaseScene*				m_pCamBox;
		LPDIRECT3DVERTEXBUFFER9	m_pVBCamLines;
		int						m_iVisibleCamsCount;
		int						m_iSelectedCamera;
		

		//target sphere for light and cam
		LPD3DXMESH				m_pSphere;
		LPD3DXMESH				m_pCone;
		
		//red light
		D3DLIGHT9				m_SelectionLight;
		bool					m_bIsCamZoomNearActive;
		bool					m_bIsCamZoomFarActive;
		bool					m_bIsLightRangeVisible;

		//shadow mesh
		D3DXVECTOR3*			m_pShadowMesh;
		D3DXVECTOR3				m_pShadowMeshPos;
		int						m_iShadowMeshEdgesCount;
		int						m_iShadowMeshVisibilityMode;


		D3DMATERIAL9		m_RedMtrl;
		D3DMATERIAL9		m_BlueMtrl;
		D3DMATERIAL9		m_YellowMtrl;
		D3DMATERIAL9		m_WhiteMtrl;
		D3DMATERIAL9		m_BlackMtrl;
		D3DMATERIAL9		m_DarkYellow;

		str_t				m_strEditedObject;

		//list of function locations in scripts
		IFuncMap			m_mapFuncs;
};
