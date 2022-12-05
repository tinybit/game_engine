#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QGameBase.h"

class QGameVideo : public QGameBase
{
public:

	QGameVideo( const std::wstring& bink_id );
	
	void					SetPrm( bool bCanStop, bool bStretchToScreen, int iSeqID );

	void					Init();
	void					Kill();

	void					SetCam( const std::wstring& sActiveCamera );

	bool					CollisionTest( QObjBase* pObj, const D3DXVECTOR3* pvNewPos, std::wstring* pCollisedObjID, D3DXVECTOR3* pvCollisionPos ) { return true; };
	
	void					OnCreateDevice();
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime ) {};
	void					OnFrameRender( double fTime, float fElapsedTime );
	void					OnLostDevice() {};
	void					OnDestroyDevice();

private:

	float					m_fFrame;

	LPDIRECT3DVERTEXBUFFER9 m_bsVB;

	bool					m_bCanStop;
	bool					m_bStretchToScreen;
	int						m_iSeqID;

	std::wstring			m_sBinkID;
	
	D3DXVECTOR2				m_vPos;
	D3DXVECTOR2				m_vSize;
	
	bool					m_bClearScr;

};