#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjCamera.h"

#include "QMgrResObjects.h"

enum GAMETYPE
{
	GT_QUEST,
	GT_VIDEO,
	GT_CONSOLE,
	GT_MENU,
	GT_STOP
};

typedef map< wstring, QObjBase*, std::less< wstring > > mapWStrQObjBase;
typedef mapWStrQObjBase::iterator mapWStrQObjBase_iter;

class QGameBase
{
public:

	virtual void			Init() {};
	virtual void			Kill() {};

	void					DoEvent(QEngineEvents e);

	void					Continue() { m_bMovePaused = false; m_bRenderPaused = false; UnFreeze(); }
	void					ContinueMove() { m_bMovePaused = false; }
	void					ContinueRender() { m_bRenderPaused = false; }
	void					Pause() { m_bMovePaused = true; m_bRenderPaused = true; }
	void					PauseMove() { m_bMovePaused = true; }
	void					PauseRender() { m_bRenderPaused = true; }
	void					Freeze() { m_bFreezed = true; }
	void					UnFreeze() { m_bFreezed = false; }
	bool					isMovePaused() { return m_bMovePaused; }
	bool					isRenderPaused() { return m_bRenderPaused; }
	bool					isFreezed() { return m_bFreezed; }
	
	void					ZeroGameTime() { m_fGameTime = 0; };
	double					GetGameTime() { return m_fGameTime; }
	float					GetGameElapsedTime() { return m_fGameElapsedTime; }
	void					IncGameTime( float fElapsedTime ) { m_fGameElapsedTime = fElapsedTime; m_fGameTime+=m_fGameElapsedTime; }

	void					SetZ( int iZ ) { m_iZ = iZ; }; 
	int						GetZ() { return m_iZ; }; 

	void					SetName( const std::wstring& sName ) { m_sName = sName; };
	std::wstring			GetName() { return m_sName; };

	void					SetGameType(GAMETYPE t) { m_eGameType = t; }
	GAMETYPE				GetGameType() { return m_eGameType; }
	std::wstring			GetGameTypeName() { return GetGameTypeName(m_eGameType); };
	static std::wstring		GetGameTypeName(GAMETYPE t);

	std::wstring			GetActiveObjsStrList();
	mapWStrQObjBase*		GetActiveObjsList() { return &m_mActiveObjs; };
	void					SetActiveObj( const std::wstring& obj_id ) { m_mActiveObjs[obj_id] = GetObjMgr()->GetObj( obj_id ); };
	void					DelActiveObj( const std::wstring& obj_id ) { if ( m_mActiveObjs.size() > 0 ) m_mActiveObjs.erase( obj_id ); };
	virtual bool			CollisionTest( QObjBase* pObj, const D3DXVECTOR3* pvNewPos, std::wstring* psCollisedObjID, D3DXVECTOR3* pvCollisionPos ) { return true; };

	void					ZeroCam() { m_pActiveCamera = NULL; };
	QObjCamera*				Cam() { return m_pActiveCamera; };
	std::wstring			CamStr() { return m_sActiveCamera; };
	virtual void			SetCam( const std::wstring& sActiveCamera ) {};

	SRay*					GetMouseRay() { return ( Cam() ? Cam()->GetMouseRay() : NULL ); };
	SRay*					GetLeftClickRay() {	return ( Cam() ? Cam()->GetLeftClickRay() : NULL ); };
	SRay*					GetRightClickRay() { return ( Cam() ? Cam()->GetRightClickRay() : NULL ); };

	SRay*					GetOrthoMouseRay() { return ( Cam() ? Cam()->GetOrthoMouseRay() : NULL ); };
	SRay*					GetOrthoLeftClickRay() {	return ( Cam() ? Cam()->GetOrthoLeftClickRay() : NULL ); };
	SRay*					GetOrthoRightClickRay() { return ( Cam() ? Cam()->GetOrthoRightClickRay() : NULL ); };

	QMgrResObjects*			GetObjMgr() { return &m_pObjMgr; };

	void					LogEnabled(bool bLogEnabled);
	bool					GetLogEnabled() { return m_bLogEnabled; };

	virtual void			OnCreateDevice() {};
	virtual void			OnResetDevice() {};
	virtual void			OnFrameMove( double fTime, float fElapsedTime ) {};
	virtual void			OnFrameRender( double fTime, float fElapsedTime ) {};
	virtual void			OnLostDevice() {};
	virtual void			OnDestroyDevice() {};
	
protected:

	int						m_iZ;

	mapWStrQObjBase			m_mActiveObjs;

	std::wstring			m_sName;
	std::wstring			m_sFinishedState;

	GAMETYPE				m_eGameType;

	bool					m_bMovePaused;
	bool					m_bRenderPaused;

	bool					m_bFreezed;

	double					m_fGameTime;
	float					m_fGameElapsedTime;

	QMgrResObjects			m_pObjMgr;

	std::wstring			m_sActiveCamera;
	QObjCamera*				m_pActiveCamera;

	bool					m_bLogEnabled;
  
};