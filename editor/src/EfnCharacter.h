#pragma once

#include "EfnCharacterData.cpp"
#include "XSIFile.h"  
#include "atlstr.h"
#include <d3d9.h>
#include <d3dx9tex.h>
#include <d3dx9math.h>
#include "utils/DXUtil.h"
#include "BaseObject.h"
#include "utils/Common.h"
#include "ViewMgr.h"
#include "dialogs/ValidateDlg.h"

typedef std::list<str_t>				strings_list_t;
typedef strings_list_t::iterator		str_list_iterator_t;

using namespace TREENODE;

class CEfnCharacter : public CBaseScene
{
	public:
		/* constructors */
		CEfnCharacter(void)  { Init(); };   
		~CEfnCharacter(void) { Kill(); };  

		/* utility functions */
		void			Init();                      
		void			Kill();	
		void			InitData();
		void			KillData();
		void			SetParentWnd( CWnd* pParent ) { m_pParent = pParent; };
		void			SetValidationDlg( CWnd* pDlg ) { m_dlgValidate = (CValidateDlg*)pDlg; };
		void			GetMinMaxDimentions( D3DXVECTOR3& vecMin, D3DXVECTOR3& vecMax );

		bool			LoadFromFile( str_t path );
		bool			SaveToFile( str_t path, bool bSilent = false );
		bool			SaveToFile();
		static bool		IsSupportedVersion( DWORD dwVersion );
		bool			InitFromXSICharacter( CXSIFile* character );
		bool			Prepare( LPDIRECT3DDEVICE9 pd3dDevice );
		bool			PrepareNullsData( LPDIRECT3DDEVICE9 pd3dDevice );
		void			SetCurrNextFrame( int iCurrFrameIndex, int iNextFrameIndex );
		int				GetFramesCount();
		int				CreateSequence();
		float			GetZOffset( int iIndex );
		void			SetZOffset( int iIndex, float fValue );
		int				GetTrianglesCount();
		int				GetTrianglesCount2();
		int				GetVerteciesCount();
		int				GetVerteciesCount2();
		const char*		GetModelName();
		void			SetVeteciesColor( DWORD dwColor );
		void			CreateLod( int iIndeciesCount, int iVerteciesCount );
		void			ResetTexture( LPDIRECT3DDEVICE9 pd3dDevice );
		str_t			GetTexturePath() { return m_strTexturePath; };
		void			SetTexturePath( str_t& strPath ) { m_strTexturePath = strPath; };

		void			ConnectAllTris();
		void			DisconnectAllTris();
		void			SetModelName( char* strName );
		void			DicardVerteciesColor();
		int				GetAnimationsCount();
		void			AddAnimToSequence( char * szName );
		int				GetSequenceLength();
		int*			GetSequence();
		void			SetFrameScript( int iFrame, str_t strScript ) { m_cdData->anim_frame_scripts[iFrame] = strScript; };
		str_t			GetFrameScript( int iFrame ) { if( !m_cdData || !m_cdData->anim_frame_scripts ) return "";  return m_cdData->anim_frame_scripts[iFrame]; };
		void			KillSequence();
		float			GetAnimationVecLength( char* szAnimationName );
		int				ContinueSequence( float vec_length, bool walk_to_run );
		void			DeleteFrames( int iIndexBegin, int iIndexEnd );
		void			DrawModel( LPDIRECT3DDEVICE9 pd3dDevice );
		void			Render( LPDIRECT3DDEVICE9 pd3dDevice, bool bWireframeMode, float fTweenFactor, bool bUseMaterial = true );
		void			RenderNormals( LPDIRECT3DDEVICE9 pd3dDevice );
		void			SetWorldTransformation( D3DXMATRIX matxWorld );
		void			RecalcXZPos();
		void			RecalcWorldPos( const D3DXMATRIX matxChar );
		void			ProcessClick( bool& bDoubleClicked, CVector& isect );
		void			CalcCharacterRotation( CVector isect );
		void			ConvertToLHCoords();
		float			GetModelHeight();
		void			Scale( D3DXMATRIX matx_scale );
		void			SetVColorFromMaterial( bool bValue, LPDIRECT3DDEVICE9 pd3dDevice );
		void			DeleteMeshData();

		void			DeselectedAllNulls()				{ m_iSelectedNull = -1; };
		int				GetNullsCount()						{ return m_cdData->vecNulls.size();	};
		CNULL2*			GetNullByIndex( int index );
		CNULL2*			GetNullByName( str_t name );
		void			DeleteNullByName( str_t name );
		void			DeleteAllNulls();
		void			CreateNewNull( str_t name );
		void			SetSelectedNull( str_t name );
		str_t			GetVersion() { return GetEfnVersionAsString( m_FileVersion ); };
		void			RotateBy180DegAboutY();

		void			CalcReverseIndex();
		void			PrecalculateNormals();
		void			KillReverseIndex();

		void			CalcAABBs();
		void			CalcBoundingSpheres();
		void			ShowBoundingSphere( bool bShow );
		bool			IsBoundingSphereVisible()		{ return m_bIsBoundingSphereVisible; };
		void			ShowAABB( bool bShow );
		bool			IsAABBVisible()					{ return m_bIsAABBVisible; };
		void			GetSelectedMeshesOOBB( D3DXVECTOR3* vMin, D3DXVECTOR3* vMax );
		D3DXVECTOR3		GetSelectedMeshesOOBBCenter();
		void			RotateZBy90();
		void			ScaleZOffsets( float fScaleFactor );

		CSPHERE*		GetCurrentBSphere()				{ if( !m_pBoundingSpheres ) return NULL; return &m_pBoundingSpheres[ m_iCurrentFrame ]; };
		CVector			GetWorldPos()					{ return m_vCharXZPos; };
		float			GetWorldRot()					{ return m_fCurrentAngle; };
		float			GetGlobalRadius()				{ return m_cdData->global_radius; };
		float			GetFPS()						{ return m_cdData->fFPS; };
		void			SetFPS( float fFPS )			{ m_cdData->fFPS = fFPS; };
		void			SetGlobalRadius( float fRad )	{ m_cdData->global_radius = fRad; };
		void			RecalcGlobalRadius();
		D3DXVECTOR3*	GetCurrentAABB()				{ if( !m_aabbList ) return NULL; return m_aabbList[ m_iCurrentFrame ]; };
		CSPHERE			GetSelectedMeshesBSphere();

		bool			IntersectOOBB( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation );
		bool			IntersectBSphere( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation );
		float			IntersectPolys( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation );

		CCUSTPARAM*		GetCustParamByIndex( int iIndex ) { return &m_cdData->vecCustomParams[iIndex]; };
		CCUSTPARAM*		GetCustParamByName( str_t strName );
		int				GetCustParamsCount() { return m_cdData->vecCustomParams.size(); };
		bool			IsCustParamAnimated( str_t strCustParamID );
		bool			IsCustParamAnimated( int iIndex );
		void			SetRenderNormals( bool bValue );

		CVERTEX_XYZ*	GetVerteciesList()				{ return m_cdData->posList; };
		int*			GetIndicesList()				{ return m_cdData->vertex_indices_list; };
		void			CreateTexture( LPDIRECT3DTEXTURE9 tex, LPDIRECT3DDEVICE9 dev );
		D3DXMATRIX		CalcCharacterMatrix();

		int*			GetIndicesList2()				{ return m_cdData->vertex_indices_list2; };

		D3DMATERIAL9	GetMaterial()	{ return m_cdData->material; };
		void			SetMaterial( D3DMATERIAL9& material )	{ m_cdData->material = material; };

		void				AddAnimation( EFN_ANIMATION* pAnimation );
		void				DeleteAnimationByName( str_t strName );
		void				DeleteAllAnimations();
		EFN_ANIMATION*		GetAnimationByName( str_t strName );
		EFN_ANIMATION*		GetAnimationByIndex( int iIndex );
		bool			IsUV1Used() { return m_bIsUV1Used; };
		void			SetUV1Used( bool bIsUV1Used ) { m_bIsUV1Used = bIsUV1Used; };
		void			AddLodFromAnimEfn( CEfnCharacter* pEfnLOD, LPDIRECT3DDEVICE9 pd3dDevice );
		bool			IsAnimsRangeOK();
		void			DegradeAnimation( float fPercentage );
		void			SetCompressionType( enum EFN_COMPRESSION_TYPE tCompessionType );
		enum EFN_COMPRESSION_TYPE GetCompressionType() { return m_tCompessionType; };

	private:
		/* data */
		EFN_CHARACTERDATA*				m_cdData;

		LPDIRECT3DVERTEXBUFFER9*		m_pPosBuff;
		LPDIRECT3DVERTEXBUFFER9*		m_pNormBuff;
		LPDIRECT3DVERTEXBUFFER9			m_pUvBuff;
		LPDIRECT3DVERTEXBUFFER9			m_pUvBuff2;
		LPDIRECT3DVERTEXBUFFER9			m_pVColorBuff;
		LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;

		ID3DXEffect*					m_pEffect;       // D3DX effect interface
		LPDIRECT3DVERTEXBUFFER9*		m_pVBLines;
		LPDIRECT3DVERTEXBUFFER9			m_pVBNull;
		LPDIRECT3DVERTEXBUFFER9			m_pVBSelectedNull;
		LPDIRECT3DINDEXBUFFER9			m_pIB;
		LPDIRECT3DINDEXBUFFER9			m_pIB2;
		LPDIRECT3DTEXTURE9				m_pTexture;
		EFN_FILETYPE					efn_type;
		DWORD							m_FileVersion;
		LPD3DXMESH*						ppMesh;
		str_t							m_strTexturePath;

		D3DMATERIAL9		m_SelectionMtrl;
		D3DMATERIAL9		m_Material;
		D3DMATERIAL9		m_BlueMtrl;
		D3DMATERIAL9		m_GreenMtrl;
		D3DMATERIAL9		m_YellowMtrl;
		D3DMATERIAL9		m_BlackMtrl;
		D3DMATERIAL9		m_WhiteMtrl;

		//bounding boxes & spheres
		D3DXVECTOR3**	m_aabbList;
		CSPHERE*		m_pBoundingSpheres;
		
		CVERTEX_XYZ*	m_pNormalsCache;

		int				m_iCurrentFrame;
		int				m_iNextFrame;
		//CString		m_strTexturePath;
		int*			m_iSequenceFrames;
		int				m_iSequenceFramesCount;
		strings_list_t	m_strSequenceAnims;

		/* transforms data */
		float			m_fCurrentAngle;  
		D3DXMATRIX		m_matxWorld;
		CVector			m_vCharXZPos;
		CVector			m_vCharWorldPos;
		float			m_fZOffset;

		/* transforms flags and secondary data */
		bool			m_bIsUV1Used;
		bool			m_bIsRotationFinished; 
		bool			m_bIsRotationWFinished;
		bool			m_bIsBoundingSphereVisible;
		bool			m_bIsAABBVisible;
		float			m_fRotationIncrement;
		bool			m_bIsSequencePlaying;
		int				m_iCurrentSequenceFrame;
		int				m_iZOffsetFrame;
		int				m_iSelectedNull;

		CWnd*			m_pParent;
		CValidateDlg*	m_dlgValidate;
		bool			m_bRenderNormals;

		enum EFN_COMPRESSION_TYPE m_tCompessionType;

		// HACK!
		bool			m_bLodExists;
		// HACK END
};
