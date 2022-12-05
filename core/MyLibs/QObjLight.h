#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjBase.h"

class QObjLight : public QObjBase
{
public:

	QObjLight( const D3DLIGHT9* pLight );
	~QObjLight() {};

	void					Init();
	void					Kill();

	D3DLIGHT9*				GetLight() { return &m_stLight; };
	void					SetPosition( D3DXVECTOR3* pvPos ) { m_stLight.Position = *pvPos; };

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) { return false; };

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime ) {};
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort ) {};
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt );

private:

	D3DLIGHT9				m_stLight;

	D3DXMATRIX				matWorld;

};