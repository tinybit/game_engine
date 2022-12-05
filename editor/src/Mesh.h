#pragma once

#include "utils/common.h"
#include "XSIFile.h"
#include <d3d9.h>

class CMesh
{
	public:
		CMesh();
		~CMesh() { Kill(); };
		
		

		/* INTERFACE */
		bool	InitFromXSIMesh( CXSIMesh* xsi_mesh );		
		void	InitNullsFromXSICharacter(CXSIFile* xsi_file);
		bool	InitFromCMesh( CMesh* mesh );
		bool	MergeWithCMesh( CMesh* mesh );

		bool	Prepare( LPDIRECT3DDEVICE9 pd3dDevice );
		bool	PrepareNulls( LPDIRECT3DDEVICE9 pd3dDevice );
		void	Render( LPDIRECT3DDEVICE9 pd3dDevice, float icon_scale, bool bUseMaterial = true );
		void	RenderNormals( LPDIRECT3DDEVICE9 pd3dDevice );
		void	SetVisible( bool bVisible )			{ m_bIsVisible = bVisible; };
		bool	GetVisible()						{ return m_bIsVisible; };

		void	SetOOBBVisible( bool bVisible )		{ m_bIsOOBBVisible = bVisible; };
		bool	GetOOBBVisible() { return m_bIsOOBBVisible; };

		void	SetBSphereVisible( bool bVisible )	{ m_bIsBSphereVisible = bVisible; };
		bool	GetBSphereVisible()					{ return m_bIsBSphereVisible; };

		str_t	GetName()							{ return m_strName; };
		int		GetVerteciesCount()					{ return m_iVerteciesCount; };
		int		GetVerteciesEdCount()				{ return m_iVerteciesEdCount; };
		int		GetSelectedVerteciesCount();
		void	SetVerteciesCount( int iVerteciesCount ){ m_iVerteciesCount = iVerteciesCount; };
		int		GetIndicesCount()					{ return m_iIndicesCount; };
		void	SetIndicesCount( int iIndicesCount ){ m_iIndicesCount = iIndicesCount; };
		int*	GetIndices()						{ return m_pIndices; };
		int		GetNullsCount()						{ return m_iNullsCount; };
		void	AddNull( CNULL* pNull );
		void	RemoveNull( CNULL* pNull );
		void	RemoveNull( str_t strNullID );
		CNULL*	GetNullByIndex( int iIndex )		{ if( m_pNulls && iIndex < m_iNullsCount && m_iNullsCount >=0 ) return &m_pNulls[iIndex]; };
		CNULL*	GetNullByName( str_t name );
		CNULL*	GetNullByName2( str_t name );
		void	SetSelected( bool bValue )			{ m_bIsSelected = bValue; if( !bValue ) m_bIsParentMesh = false; };
		bool	IsSelected()						{ return m_bIsSelected; };

		void	SetNullIcon( CBaseScene* pBaseScene ){ m_pAlternateNullIcon = pBaseScene; };
        void	SetScript( str_t strScript ) { m_strScript = strScript; }; 
        str_t	GetScript() { return m_strScript; };
		void	SetName( str_t strName ) { m_strName = strName; };
		bool	SaveToFile( ofstream& file );
		void	LoadFromFile( ifstream& file );
		void	LoadMaterialFromFile( ifstream& file );
		void	CalcBoundingSphere();
		void	CalcOOBB();
		void	KillNulls();
		void	KillReverseIndex();
        void	CalcReverseIndex();
		void	DeselectAllNulls() { m_iSelectedNull = -1; }
		void	SetSelectedNull( str_t strNullID );
		CNULL*	GetSelectedNull();
		DWORD	GetVColor() { if( m_pVertecies) return m_pVertecies[0].color; return 0; };
		void	SetVColor( DWORD dwColor );
		int**	GetReverseIndex() { return m_ppReverseIndex; };
		void	CreateNull( str_t strID, D3DXVECTOR3* pos = NULL );
		bool	IntersectOOBB( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation );
		bool	IntersectBSphere( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation );
		float	IntersectPolys( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation );
		void	SetVertexEditingMode( bool bValue );
		bool	IsVertexEditingMode();
		void	CancelVertexEditingMode();
		void	SetRenderMode( enum RENDER_MODE RenderModeEnum ) { m_RenderModeEnum = RenderModeEnum; };
		void	SelectVerteciesByFrustrum( LPDIRECT3DDEVICE9 pd3dDevice, enum VERTEX_SELECTION_MODE SelectionMode, CPoint* vTopLeft, CPoint* vBottomRight );
		int		SelectNullsByFrustrum( LPDIRECT3DDEVICE9 pd3dDevice, CPoint* vTopLeft, CPoint* vBottomRight );
		void	SelectAllVertecies();
		void	MoveSelectedVerteciesXBegin( float x );
		void	MoveSelectedVerteciesYBegin( float y );
		void	MoveSelectedVerteciesZBegin( float z );
		void	MoveSelectedVerteciesXYBegin( float x, float y );
		void	MoveSelectedVerteciesXZBegin( float x, float z );
		void	MoveSelectedVerteciesYZBegin( float y, float z );
		void	MoveSelectedVerteciesEnd();
		void	PrecalculateNormals();
		void	SetVColorFromMaterial( bool	bValue, LPDIRECT3DDEVICE9 pd3dDevice );
		void	SetMaterial( D3DMATERIAL9 material );
		void	InitMaterialFromVColor();
		void	EnableMaterials( bool bValue )	{ m_bMaterialsEnabled = bValue; }; 
		bool	IsMaterialEnabled()				{ return m_bMaterialsEnabled; };
		void	SetParentMesh( bool bValue )	{ m_bIsParentMesh = bValue; }; 
		bool	IsParentMesh()					{ return m_bIsParentMesh; };
		void	RebuildEditBuffers();
		void	InverseNormals();
		void	ConnectPolys();
		void	DisconnectPolys();
		void	UpdateEngineTempMesh( bool force = false );
		void	AlignPoints();
		void	MergePoints();
		void	DeleteSelectedVertecies();
		void	TranslateWholeSceneHack( D3DXVECTOR3* pTranslate );

		D3DMATERIAL9 GetMaterial();

		CVERTEX*		GetVertecies()			{ return m_pVertecies; };
		void			SetVertecies( CVERTEX* pVertecies, int iVerteciesCount );
		void			SetIndices( int* pIndecies, int iIndeciesCount );
		CSPHERE*		GetBSphere()			{ return &m_BSphere; };
		D3DXVECTOR3*	GetOOBB()				{ return m_OOBB; };
		D3DXVECTOR3		GetOOBBCenter();
		CBaseScene*		GetAlternateNullIcon()	{ return m_pAlternateNullIcon; };

	private:
		void	Kill();
		void	RenderMeshData( LPDIRECT3DDEVICE9 pd3dDevice, bool bUseMaterial = true );

		LPDIRECT3DVERTEXBUFFER9	m_pOOBBVB;
		LPD3DXMESH				m_pBsphere;
		LPDIRECT3DVERTEXBUFFER9	m_pVBNull;
		LPD3DXMESH				m_pNullSphere;

		CBaseScene*			m_pAlternateNullIcon;

		str_t				m_strName;
		str_t				m_strScript;
		
		int					m_iVerteciesCount;
		CVERTEX*			m_pVertecies;

        int**				m_ppReverseIndex;
		int					m_iIndicesCount;
		int*				m_pIndices;
		CVERTEX_XYZ*		m_pNormalsCache;

		int					m_iVerteciesEdCount;
		CVERTEX*			m_pVertecies_Ed_Buff;
		CVERTEX*			m_pVertecies_Move_Buff;
		int					m_iIndicesEdCount;
		int*				m_pIndicesEd_Buff;
		
		CSPHERE				m_BSphere;
		D3DXVECTOR3			m_OOBB[8];

		int					m_iNullsCount;
		CNULL*				m_pNulls;
		int					m_iSelectedNull;

		bool				m_bIsVisible;
		bool				m_bIsParentMesh;
		bool				m_bIsSelected;
		bool				m_bIsOOBBVisible;
		bool				m_bIsBSphereVisible;
		bool				m_bIsVertexeditingMode;
		bool				m_bMaterialsEnabled;

		D3DMATERIAL9		m_SelectionMtrl;
		D3DMATERIAL9		m_Material;
		D3DMATERIAL9		m_BlueMtrl;
		D3DMATERIAL9		m_GreenMtrl;
		D3DMATERIAL9		m_YellowMtrl;
		D3DMATERIAL9		m_BlackMtrl;
		D3DMATERIAL9		m_WhiteMtrl;
		enum RENDER_MODE	m_RenderModeEnum;
		UINT				m_time;
};
