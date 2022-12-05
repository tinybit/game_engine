#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QObjBase.h"

class QObjShadowVolume : public QObjBase
{
public:
	QObjShadowVolume( const std::wstring& efn_id );
	~QObjShadowVolume();

	void					Init();
	void					Kill();

    void					RenderShadow();
    void					CalcShadow();

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) { return false; };

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime ) {};
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt );

private:

};