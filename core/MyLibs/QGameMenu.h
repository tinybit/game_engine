#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QGameBase.h"

struct SMouseAnimSeq
{
	std::wstring			m_sEfnID;
	std::wstring			m_sAnimName;
	float					m_fAnimFrameRate;
	float					m_fAnimGoTime;
	float					m_fAnimRotSpeed;
	int						m_iAnimFramesCount;
	int						m_iAnimRepeatTimes;
	float					m_fAnimTimeLength;
	float					m_fAnimTotalTimeLength;
	float					m_fAnimZLength;
	float					m_fAnimTotalZLength;
	float					m_fAnimGoZ;
};

typedef std::vector<SMouseAnimSeq> vecMouseAnim;

class QGameMenu : public QGameBase
{
public:
	QGameMenu() {};
	~QGameMenu() {};

	void					Init();
	void					Kill();

	void					SetCam( const std::wstring& sActiveCamera );

	bool					CollisionTest( QObjBase* pObj, const D3DXVECTOR3* pvNewPos, std::wstring* pCollisedObjID, D3DXVECTOR3* pvCollisionPos ) { return true; };

	void					RenderCursor();

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime );
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

protected:

	QObjMenuRukzak*			m_pRukzak;
	QObjMenuCursor*			m_pCursor;
	QObjMenuMenu*			m_pMenu;

	D3DXVECTOR3				pos, pos_s, pos_e;
	D3DXPLANE				plane;

};