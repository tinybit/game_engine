#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjBase.h"
#include "QObjEfnAnim.h"

using namespace std;

class QObjMenuMenu : public QObjBase
{
public:

	QObjMenuMenu( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	~QObjMenuMenu();

	void					Init();
	void					Kill();

	bool					IsOpened() { return m_bOpened; };
	void					Open();
	void					Close();

	void					SetActiveLine( int iLine );

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos );
	
	void					ObjRender( DWORD rt ) {	if ( GetEfnAnim() )	GetEfnAnim()->ObjRender( rt ); };

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

private:

	bool					m_bOpened;

	int						m_iActiveLine;

};