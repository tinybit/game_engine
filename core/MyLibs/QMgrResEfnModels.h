#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "IMgrResEfnModels.h"

using namespace std;

typedef std::map< wstring, QEfnDataListItem*, std::less<wstring> > efnmap;
typedef efnmap::const_iterator efnmap_iter;

typedef std::vector<SMorfKoefs> vecKoefs;
typedef std::vector<SMorfKoefs>::iterator vecKoefsIter;

class QMgrResEfnModels : public IMgrResEfnModels
{
public:

	QMgrResEfnModels();
	~QMgrResEfnModels();

	void					LogEnabled( bool bLogEnabled );

	void					LoadEfn( const std::wstring& efn_id );
	void					FreeEfnByID( const std::wstring& efn_id );
	void					FreeEfn( QEfnDataListItem* EfnDataListItem );

	void					CloneEfn( const std::wstring& efn_id, const std::wstring& src_efn_id, DWORD iIBUsage, DWORD iPosBuffUsage, DWORD iNormBuffUsage, DWORD iUvBuffUsage, DWORD iVColorBuffUsage );

	int						LoadData( const std::wstring& efn_id, EFN_CHARACTERDATA* (&data));
	bool					LoadDataAnimation(EFN_CHARACTERDATA* (&m_cdData), ifstream& stream);
	bool					LoadDataMeshlist(EFN_CHARACTERDATA* (&m_cdData), ifstream& stream, int& count);
	void					FreeData(EFN_CHARACTERDATA* m_cdData);
	void					FreeData(EFN_CHARACTERDATA* m_cdData, bool bDelete);
	void					CopyCData(EFN_CHARACTERDATA* dest, EFN_CHARACTERDATA* src);
	void					PrecalculateNormals(EFN_CHARACTERDATA* m_cdData);

	bool					InitBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage );
	void					FreeBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage );
	bool					FillBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage );

	bool					FillIndexBuffer( QEfnDataListItem* EfnDataListItem );
	bool					FillPosBuffer( QEfnDataListItem* EfnDataListItem );
	bool					FillNormBuffer( QEfnDataListItem* EfnDataListItem );
	bool					FillUVBuffer( QEfnDataListItem* EfnDataListItem );
	bool					FillColorBuffer( QEfnDataListItem* EfnDataListItem );

	std::wstring			GetFileFromID( const std::wstring& efn_id );

	std::wstring			GetFrameScript( const std::wstring& efn_id, int iFrame );
	float					GetZOffset( const std::wstring& efn_id, int iFrame );
	float					GetZOffset( const QEfnDataListItem* EfnDataListItem, int iFrame );

	D3DMATERIAL9*			GetEfnMtrl( const std::wstring& efn_id );

	void					MakeMorf( const std::wstring& efn_id, const std::wstring& nulls_efn_id, const std::wstring& meshs_efn_id, int iLeftFrame, int iRightFrame, float fTweenFactor );

	void					Render( const std::wstring& efn_id, RENDER_TYPES r_type );
	void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, RENDER_TYPES r_type );
	void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame, RENDER_TYPES r_type );
	void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, const std::wstring& anim_name, float fFrameIndex, RENDER_TYPES r_type );
	void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, int iLeftFrame, int iRightFrame, float fTweenFactor, RENDER_TYPES r_type );
	void					Render( const QEfnDataListItem *EfnDataListItem, const D3DXMATRIX* pmatWorld, int iLeftFrame, int iRightFrame, float fTweenFactor, RENDER_TYPES r_type );

	void					RenderOOBB( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame );
	void					RenderOOBB( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame, const D3DXCOLOR* pstColor );

	void					GetEfnNullList( const std::wstring& efn_id, listWStr* plstWstr );

	bool					GetEfnNull( const std::wstring& efn_id, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, SNULL* pstNull );
	bool					GetEfnNull( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, SNULL* pstNull );
	bool					GetEfnNullMatrix( const std::wstring& efn_id, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, D3DXMATRIX* pMat );
	bool					GetEfnNullForMeshMorf( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, float* pfRotY );
	bool					GetEfnNullFor3DVideo( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, int* piLeftFrame, int* piRightFrame );

	CSPHERE*				GetEfnSphere( const std::wstring& efn_id, float fFrame);
	D3DXVECTOR3*			GetEfnOOBB( const std::wstring& efn_id, float fFrame);
	float					GetGlobalCylinderRadius( const std::wstring& efn_id);

	int						GetAnimCount( const std::wstring& efn_id );
	int						GetAnimFramesCount( const std::wstring& efn_id, const std::wstring& anim_name );
	bool					GetAnimFrame( const std::wstring& efn_id, const std::wstring& anim_name, const double fFrameIndex, int* iLeftFrame, int* iRightFrame, float* fTweenFactor );
	EFN_CHARACTERANIMATION*	GetAnim( const std::wstring& efn_id, const std::wstring& anim_name );
	float					GetAnimTimeLength( const std::wstring& efn_id, const std::wstring& anim_name );
	float					GetAnimFrameRate( const std::wstring& efn_id );
	float					GetAnimZLength( const std::wstring& efn_id,  const std::wstring& anim_name );

	int						GetTriCount( const std::wstring& efn_id );
	QEfnDataListItem*		GetEfn( const std::wstring& efn_id );
	efnmap_iter				GetEfnIter( const std::wstring& efn_id );
	void					PurgeCache() { m_sEfnCache = L""; }

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnLostDevice();
	void					OnDestroyDevice();

	void					Release() { delete this; }

private:

	efnmap_iter				m_itEfnCache;
	std::wstring			m_sEfnCache;

	efnmap					m_mEfn;

	bool					m_bLogEnabled;

	LPDIRECT3DVERTEXDECLARATION9 m_pVD_POS;
	LPDIRECT3DVERTEXDECLARATION9 m_pVD_POS_NORM;
	LPDIRECT3DVERTEXDECLARATION9 m_pVD_POS_TEX;
	LPDIRECT3DVERTEXDECLARATION9 m_pVD_POS_COL_TEX;
	LPDIRECT3DVERTEXDECLARATION9 m_pVD_POS_NORM_TEX;
	LPDIRECT3DVERTEXDECLARATION9 m_pVD_POS_NORM_COL;
	LPDIRECT3DVERTEXDECLARATION9 m_pVD_POS_NORM_POS_NORM_TEX;
};