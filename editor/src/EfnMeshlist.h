#pragma once

#include "XSIFile.h"
#include "Mesh.h"
#include "utils/common.h"
#include <d3d9.h>


class CEfnMeshlist : public CBaseScene
{
	public:
		/* CONSTRUCTORS */
		CEfnMeshlist(void) { Initialize(); };
		~CEfnMeshlist(void) { Destroy(); };

		/* INTERFACE */
		void			Initialize();
		void			Destroy();
		
		bool			Prepare(LPDIRECT3DDEVICE9 pd3dDevice);
		void			Render(LPDIRECT3DDEVICE9 pd3dDevice ) { Render( pd3dDevice, true ); };
		void			Render(LPDIRECT3DDEVICE9 pd3dDevice, bool bUseMaterial = true );
		void			Reset(LPDIRECT3DDEVICE9 pd3dDevice);
		static bool		IsSupportedVersion( DWORD dwVersion );
		bool			InitFromXSICharacter(CXSIFile* xsi_file);

		void			SetRenderNormals( bool bValue ) { m_bRenderNormals = bValue; };
		CMesh*			GetMeshByIndex( int iIndex )	{ if( iIndex >= 0 && iIndex < (int)m_vecMeshes.size() ) return m_vecMeshes[iIndex]; };
		CMesh*			GetFirstSelectedMesh();
		CMesh*			GetMeshByName( str_t strName );
		CMesh*			GetMeshByName2( str_t strName );
		CMesh*			GetSelectedMesh();
		int				GetMeshesCount();
		bool			SaveToFile( str_t Path, bool bSilent = false );
		bool			LoadFromFile( str_t Path );
		str_t			GetSceneTag()				{ return m_strSceneTag; };
		void			SetSceneTag( str_t strTag )	{ m_strSceneTag = strTag; };
		void			DeselectAllMeshes();
		int				GetSelectedMeshesCount();
		void			SelectMesh( str_t strMeshName );
		void			DeselectMesh( str_t strMeshName );
		void			ShowAllMeshes( bool bShow );
		void			ShowAllNulls( bool bShow );
		void			ShowMeshes( str_t strName, bool bShow );
		void			DeleteMesh( str_t strName );
		CMesh*			AddMesh( str_t strName );
		void			MoveNull( str_t strSrcMesh, str_t strSrcNull, str_t strDestMesh );
		void			MakeSureParentIsSet();
		bool			IsParentMeshSet();
		CMesh*			GetParentMesh();
		void			SetParentMesh( str_t meshID );
		void			Scale( D3DXMATRIX matx_scale );
		int				GetTotalPolycount();
		bool			IsVertexEditingMode();
		void			TranslateWholeSceneHack( D3DXVECTOR3* pTranslate );

		void			ShowOOBBs( bool bShow );
		bool			GetOOBBVisible();
		void			ShowBSpheres( bool bShow );
		bool			GetBSpheresVisible();
		void			SetParent( CWnd* parent )		{ m_pParentWnd = parent; };
		void			SetSelectedNull( str_t strMeshID, str_t strNullID );
		CNULL*			GetSelectedNull();
		void			DeselectAllNulls();
		void			SelectAllNulls() { m_bAllNullsSelected = true; };
		void			SelectAllMeshes();
		void			SetNullIcon( CBaseScene* pBaseScene );
		float			IntersectByRay( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation, enum OBJECT_TYPES& objType, str_t& strObjID );
		void			EnableMaterials( bool bValue );// { m_bMaterialsEnabled = bValue; };
		void			DeleteAllMeshes();
		void			DeleteAllNulls();
		void			DeleteSelectedMeshes();
		void			MergeSelectedMeshes( str_t* strPar );
		bool			IsAllNullsSelected() { return m_bAllNullsSelected; };
		void			AssignMaterialToSelectedMeshes( D3DMATERIAL9 mat );
		void			FlipNormalsInSelectedMeshes();
		void			ConnectTrisInSelectedMeshes();
		void			DisconnectTrisInSelectedMeshes();
		str_t			GetVersion() { return GetEfnMeshlistVersionAsString( m_dwFileVersion ); };
		
		void			GetSelectedMeshesOOBB( D3DXVECTOR3* vMin, D3DXVECTOR3* vMax );
		D3DXVECTOR3		GetSelectedMeshesOOBBCenter();
		CSPHERE			GetSelectedMeshesBSphere();
		void			RotateXBy90();
		void			RotateYBy90();
		void			RotateZBy90();

	private:
		vector<CMesh*>	m_vecMeshes;
		bool			m_bRenderNormals;
		DWORD			m_dwFileVersion;
		CWnd*			m_pParentWnd;
		str_t			m_strSceneTag;
		bool			m_bAllNullsSelected;
		

		enum EFN_FILETYPE	efn_type;

		LPDIRECT3DTEXTURE9	m_pTexture;		// debug texture
};
