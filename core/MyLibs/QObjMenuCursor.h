#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjBase.h"
#include "QObjEfnAnim.h"

using namespace std;

class QObjMenuCursor : public QObjBase
{
public:

	QObjMenuCursor( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& rukzak_obj_id );
	~QObjMenuCursor();

	void					Init();
	void					Kill();

	void					SetItem( const std::wstring& item_id );
	std::wstring			GetItem() { return m_sItemID; };
	void					Clear();

	bool					CursorAttach( QObjEfnAnim* efn, const std::wstring& null_id, const std::wstring& obj_id, void* pSRukzakItem );

	void					SetCursorAnim( const std::wstring& sAnim );
	void					SetCursorAnim2( const std::wstring efn_id, const std::wstring& sAnim );
	std::wstring			GetCursorAnim() { return m_sCursorAnim; };
	int						GetCursorType() { return m_iCursorType; };

	void					SetCursorHint( const std::wstring& sHint ) { m_sHint = sHint; };
									   
	void					MouseClick( int iMouseEvType );

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) {	return ( GetEfnAnim() ? GetEfnAnim()->CheckForRayIntersect( pstRay, pfDist, pvIntrPos ) : false ); };
	
	void					ObjRender( DWORD rt ) {	if ( GetEfnAnim() )	GetEfnAnim()->ObjRender( rt ); };

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

private:

	std::wstring			m_sItemID;

	int						m_iCursorType;
	std::wstring			m_sCursorAnim;

	std::wstring			m_sHint;

	std::wstring			m_sRukzakObjID;

};