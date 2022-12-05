#pragma once

#include "QInterfaces.h"

enum RENDER_TYPES
{
	RT_POS,
	RT_POS_TEX,
	RT_POS_NORM,
	RT_POS_NORM_TEX,
	RT_POS_NORM_POS_NORM_TEX
};

struct QEfnDataListItem
{
	std::wstring				efn_id;
	std::wstring				strTexturePath;
	bool						needs;

	bool						bMorf_BaseWrited;

	EFN_CHARACTERDATA*			pcdData;
	LPDIRECT3DVERTEXBUFFER9*	pPosBuff;
	DWORD						iPosBuffUsage;
	LPDIRECT3DVERTEXBUFFER9*	pNormBuff;
	DWORD						iNormBuffUsage;
	LPDIRECT3DVERTEXBUFFER9		pUvBuff;
	DWORD						iUvBuffUsage;
	LPDIRECT3DVERTEXBUFFER9		pVColorBuff;
	DWORD						iVColorBuffUsage;
	LPDIRECT3DINDEXBUFFER9		pIB;
	DWORD						iIBUsage;

	LPDIRECT3DVERTEXBUFFER9*	pOOBBPosBuff;
	LPDIRECT3DINDEXBUFFER9		pOOBB_IB;

	D3DMATERIAL9				stMtrl;
};

struct SMorfKoefs
{
	std::wstring	name;
	D3DXVECTOR3*	posList;						
	D3DXVECTOR3*	normList;
	float			value;
};

#define g_EfnMgr IMgrResEfnModels::GetSingleton()

struct IMgrResEfnModels : public IMyD3DUnknown, Singleton <IMgrResEfnModels>
{
	virtual void					LoadEfn( const std::wstring& efn_id ) = 0;
	virtual void					FreeEfnByID( const std::wstring& efn_id ) = 0;
	virtual void					FreeEfn( QEfnDataListItem* EfnDataListItem ) = 0;

	virtual void					CloneEfn( const std::wstring& efn_id, const std::wstring& src_efn_id, DWORD iIBUsage, DWORD iPosBuffUsage, DWORD iNormBuffUsage, DWORD iUvBuffUsage, DWORD iVColorBuffUsage ) = 0;

	virtual int						LoadData( const std::wstring& efn_id, EFN_CHARACTERDATA* (&data) ) = 0;
	virtual bool					LoadDataAnimation(EFN_CHARACTERDATA* (&m_cdData), ifstream& stream) = 0;
	virtual bool					LoadDataMeshlist(EFN_CHARACTERDATA* (&m_cdData), ifstream& stream, int& count) = 0;
	virtual void					FreeData(EFN_CHARACTERDATA* m_cdData) = 0;
	virtual void					FreeData(EFN_CHARACTERDATA* m_cdData, bool bDelete ) = 0;
	virtual void					CopyCData(EFN_CHARACTERDATA* dest, EFN_CHARACTERDATA* src) = 0;
	virtual void					PrecalculateNormals( EFN_CHARACTERDATA* m_cdData ) = 0;

	virtual bool					InitBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage ) = 0;
	virtual void					FreeBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage ) = 0;
	virtual bool					FillBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage ) = 0;

	virtual bool					FillIndexBuffer( QEfnDataListItem* EfnDataListItem ) = 0;
	virtual bool					FillPosBuffer( QEfnDataListItem* EfnDataListItem ) = 0;
	virtual bool					FillNormBuffer( QEfnDataListItem* EfnDataListItem ) = 0;
	virtual bool					FillUVBuffer( QEfnDataListItem* EfnDataListItem ) = 0;
	virtual bool					FillColorBuffer( QEfnDataListItem* EfnDataListItem ) = 0;

	virtual std::wstring			GetFileFromID( const std::wstring& efn_id ) = 0;

	virtual std::wstring			GetFrameScript( const std::wstring& efn_id, int iFrame ) = 0;
	virtual float					GetZOffset( const std::wstring& efn_id, int iFrame ) = 0;
	virtual float					GetZOffset( const QEfnDataListItem*, int iFrame ) = 0;

	virtual D3DMATERIAL9*			GetEfnMtrl( const std::wstring& efn_id ) = 0;

	virtual void					MakeMorf( const std::wstring& efn_id, const std::wstring& nulls_efn_id, const std::wstring& meshs_efn_id, int iLeftFrame, int iRightFrame, float fTweenFactor ) = 0;

	virtual void					Render( const std::wstring& efn_id, RENDER_TYPES r_type ) = 0;
	virtual void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, RENDER_TYPES r_type ) = 0;
	virtual void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame, RENDER_TYPES r_type ) = 0;
	virtual void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, const std::wstring& anim_name, float fFrameIndex, RENDER_TYPES r_type ) = 0;
	virtual void					Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, int iLeftFrame, int iRightFrame, float fTweenFactor, RENDER_TYPES r_type ) = 0;
	virtual void					Render( const QEfnDataListItem *EfnDataListItem, const D3DXMATRIX* pmatWorld, int iLeftFrame, int iRightFrame, float fTweenFactor, RENDER_TYPES r_type ) = 0;

	virtual void					RenderOOBB( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame ) = 0;
	virtual void					RenderOOBB( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame, const D3DXCOLOR* pstColor ) = 0;

	virtual void					GetEfnNullList( const std::wstring& efn_id, listWStr* plstWstr ) = 0;

	virtual bool					GetEfnNull( const std::wstring& efn_id, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, SNULL* pstNull ) = 0;
	virtual bool					GetEfnNull( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, SNULL* pstNull ) = 0;
	virtual bool					GetEfnNullMatrix( const std::wstring& efn_id, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, D3DXMATRIX* pMat ) = 0;
	virtual bool					GetEfnNullForMeshMorf( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, float* pfRotY ) = 0;
	virtual bool					GetEfnNullFor3DVideo( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, int* piLeftFrame, int* piRightFrame ) = 0;

	virtual CSPHERE*				GetEfnSphere( const std::wstring& efn_id, float fFrame) = 0;
	virtual D3DXVECTOR3*			GetEfnOOBB( const std::wstring& efn_id, float fFrame) = 0;
	virtual float					GetGlobalCylinderRadius( const std::wstring& efn_id ) = 0;

	virtual int						GetAnimCount( const std::wstring& efn_id ) = 0;
	virtual int						GetAnimFramesCount( const std::wstring& efn_id, const std::wstring& anim_name) = 0;
	virtual bool					GetAnimFrame( const std::wstring& efn_id, const std::wstring& anim_name, const double fFrameIndex, int* iLeftFrame, int* iRightFrame, float* fTweenFactor) = 0;
	virtual EFN_CHARACTERANIMATION*	GetAnim( const std::wstring& efn_id, const std::wstring& anim_name ) = 0;
	virtual float					GetAnimTimeLength( const std::wstring& efn_id, const std::wstring& anim_name ) = 0;
	virtual float					GetAnimFrameRate( const std::wstring& efn_id ) = 0;
	virtual float					GetAnimZLength( const std::wstring& efn_id, const std::wstring& anim_name ) = 0;

	virtual int						GetTriCount( const std::wstring& efn_id ) = 0;
	virtual QEfnDataListItem*		GetEfn( const std::wstring& efn_id ) = 0;

	virtual void					LogEnabled( bool ) = 0;
};
typedef IMgrResEfnModels* LPMGRRESEFNMODELS;