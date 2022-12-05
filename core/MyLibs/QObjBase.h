#pragma once

#include "dxstdafx.h"

#include "UtilsDefines.h"

#include "QMgrResObjects.h"

class QGameBase;
class QObjEfnAnim;

class QObjBase
{
public:

	virtual void			Init() {};
	virtual void			Kill() {};

	void					GoEfnAnim( double fTime, double fPathLength, int seq_id );
	void					StopEfnAnim( bool bOK );

	std::wstring			GetObjID() { return m_sObjID; };
	void					SetObjID( const std::wstring& obj_id ) { m_sObjID = obj_id; m_sEfnAnimObjID = obj_id + L"_EfnAnim"; };

	void					SetType( ObjTypes t ) { m_eType = t; };
	ObjTypes				GetType() { return m_eType; };
	std::wstring			GetTypeStr();

	void					SetScript( const std::wstring& sScript ) { m_sScript = sScript; }
	std::wstring			GetScript() { return m_sScript; }

	void					SetEnabled( bool bEnabled );
	bool					GetEnabled() { return m_bEnabled; }

	void					SetVisible( bool bVisible );
	bool					GetVisible() { return m_bVisible; }

	void					SetObjMgr( QMgrResObjects* pObjMgr ) { m_pObjMgr = pObjMgr; };
	QMgrResObjects*			GetObjMgr() { return m_pObjMgr; };

	void					SetGame( QGameBase* pGame ) { m_pGame = pGame; };
	QGameBase*				GetGame() { return m_pGame; };

	void					SetNullPos( const SNULL* stNull ) { m_stPos = *stNull; };
	SNULL*					GetNullPos() { return &m_stPos; };
	bool					isinNull( const SNULL* pstNull ) { return ( D3DXVec2Length( &(D3DXVECTOR2( pstNull->vPos.x, pstNull->vPos.z ) - D3DXVECTOR2( GetNullPos()->vPos.x, GetNullPos()->vPos.z )) ) <= GetGopaRadius() ? true : false ); };

	void					SetEfnAnim( QObjEfnAnim* pEfnAnim ) { m_pEfnAnim = pEfnAnim; };
	QObjEfnAnim*			GetEfnAnim() { return m_pEfnAnim; };
	
	std::wstring			GetEfnAnimObjID() { return m_sEfnAnimObjID; };

	bool					IsAnimStarted();

	void					SetMtrl( const D3DMATERIAL9* pstMtrl ) { if ( pstMtrl ) { m_stMtrl = *pstMtrl; } else { ZeroMemory( &m_stMtrl, sizeof(D3DMATERIAL9) ); m_stMtrl.Diffuse = m_stMtrl.Ambient = D3DXCOLOR(1,1,1,1); } };
	void					SetMtrlAlpha( float fAlpha ) { m_stMtrl.Diffuse.a = m_stMtrl.Ambient.a = fAlpha; };
	void					SetMtrlColor( const D3DXCOLOR* pstColor )	{ m_stMtrl.Diffuse = m_stMtrl.Ambient = *pstColor; };
	void					SetMtrlEmissive( const D3DXCOLOR* pstColor ) { m_stMtrl.Emissive = *pstColor; };
	void					SetMtrlSpecular( const D3DXCOLOR* pstColor, float fPower ) { m_stMtrl.Specular = *pstColor; m_stMtrl.Power = fPower; };

	D3DMATERIAL9*			GetMtrl() { return &m_stMtrl; };

	void					SetTexID( const std::wstring& tex_id ) { m_sTexID = tex_id; };
	std::wstring			GetTexID() { return m_sTexID; };
	
	void					SetTexID2( const std::wstring& tex_id ) { m_sTexID2 = tex_id; };
	std::wstring			GetTexID2() { return m_sTexID2; };

	void					SetEfnID( const std::wstring& efn_id ) { m_sEfnID = efn_id; };
	std::wstring			GetEfnID() { return m_sEfnID; };
	
	void					SetEfnID2( const std::wstring& efn_id ) { m_sEfnID2 = efn_id; };
	std::wstring			GetEfnID2() { return m_sEfnID2; };

	void					SetGopaRadius( float fGopaRadius ) { m_fGopaRadius = fGopaRadius; };
	float					GetGopaRadius() { return m_fGopaRadius; };

	void					AddObjToCollisionList();
	void					RemoveObjFromCollisionList();

	virtual bool			CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) { return false; };

	virtual void			OnCreateDevice() {};
	virtual void			OnResetDevice() {};
	virtual void			OnFrameMove( double fTime, float fElapsedTime ) {};
	virtual void			OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort ) {};
	virtual void			OnLostDevice() {};
	virtual void			OnDestroyDevice() {};

	virtual void			ObjRender( DWORD rt ) {};

private:

	ObjTypes				m_eType;

	SNULL					m_stPos;
	
	std::wstring			m_sObjID;
	std::wstring			m_sScript;

	std::wstring			m_sEfnAnimObjID;
	QObjEfnAnim*			m_pEfnAnim;

	D3DMATERIAL9			m_stMtrl;

	std::wstring			m_sEfnID;
	std::wstring			m_sEfnID2;

	std::wstring			m_sTexID;
	std::wstring			m_sTexID2;

	float					m_fGopaRadius;

	bool					m_bEnabled;
	bool					m_bVisible;

	bool					m_bRenderNeeds;

	QMgrResObjects*			m_pObjMgr;
	QGameBase*				m_pGame;

};