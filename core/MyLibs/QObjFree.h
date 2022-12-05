#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjBase.h"
#include "QObjEfnAnim.h"

#include "QGameQuestSceneData.h"

using namespace std;

class QObjFree : public QObjBase
{
public:

	QObjFree( const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, bool bFillGopa );
	~QObjFree();

	void					Init();
	void					Kill();

	void					Go( double fTime, int seq_id );
	void					Stop( bool bOK );

	void					SetPos( const SNULL* stNull );
	void					Teleport( const SNULL* stNull );

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) {	return ( GetEfnAnim() ? GetEfnAnim()->CheckForRayIntersect( pstRay, pfDist, pvIntrPos ) : false ); };

	void					ObjRender( DWORD rt );

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort ) {};
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

private:

	bool					m_bFillGopa;
	bool*					m_pGopa;
	int						m_iGopaSize;

	QGameQuestSceneData*	m_pQuestScn;

};