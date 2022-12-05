#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjBase.h"

class QObjAction : public QObjBase
{
public:

	QObjAction( const std::wstring& efn_id, const std::wstring& sOnClickScript );
	~QObjAction() {};

	void					Init();
	void					Kill();

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos );

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime ) {};
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort ) {};
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt );

private:

};