#pragma once

#include "dxstdafx.h"

#include "QGameBase.h"

class QGameConsole : public QGameBase
{
public:

	void					Init();
	void					Kill();

	void					SetCam( const std::wstring& sActiveCamera );

	bool					CollisionTest( QObjBase* pObj, const D3DXVECTOR3* pvNewPos, std::wstring* pCollisedObjID, D3DXVECTOR3* pvCollisionPos ) { return true; };

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime );
	void					OnLostDevice();
	void					OnDestroyDevice();
	
protected:
  
};