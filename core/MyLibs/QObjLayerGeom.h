#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"
#include "IMgrResEfnModels.h"
#include "IMgrResTextures.h"

#include "QObjBase.h"

class QObjLayerGeom : public QObjBase
{
public:

	QObjLayerGeom( const std::wstring& efn_id, const std::wstring& state_id );
	~QObjLayerGeom() {};

	void					Init();
	void					Kill();

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) { return false; };
	void					CalcUV( void* pCam, const QTextureListItem* temp_tex );

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime ) {};
	inline void				OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt ) { g_EfnMgr.Render( GetEfnID(), (RENDER_TYPES)rt ); };

private:

};