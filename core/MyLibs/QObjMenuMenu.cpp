#define STRICT

#include "QObjMenuMenu.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "QMgrResSounds.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"

QObjMenuMenu::QObjMenuMenu( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	SetEfnID( efn_id );
	SetTexID( tex_id );
	SetTexID2( tex_id2 );
	
	m_bOpened = false;
	m_iActiveLine = -1;
}

QObjMenuMenu::~QObjMenuMenu()
{
}

void QObjMenuMenu::Init()
{
	if ( GetObjMgr()->AddObjEfnAnim( GetEfnAnimObjID(), GetEfnID(), GetTexID(), GetTexID2() ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
		GetEfnAnim()->SetRenderPrms( true, true, true );
	}

	Close();

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjMenuMenu::Kill()
{
	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();
}


void QObjMenuMenu::Open()
{
	m_bOpened = true;
	GetEfnAnim()->Stop( false );
	GetEfnAnim()->AnimAdd( L"rotate", 0, 1 );
	GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );
}

void QObjMenuMenu::Close()
{
	m_iActiveLine = -1;
	m_bOpened = false;
	GetEfnAnim()->Stop( false );
	GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );
}

void QObjMenuMenu::SetActiveLine( int iLine )
{
	if ( IsOpened() && m_iActiveLine != iLine )
	{
		m_iActiveLine = iLine;
		GetEfnAnim()->Stop( false );
		GetEfnAnim()->AnimAdd( int2wstr(iLine) + L" line", 0, 1 );
		GetEfnAnim()->AnimAdd( int2wstr(iLine) + L" line freeze", 0, -1 );
		GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );
	}
}

bool QObjMenuMenu::CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos )
{
	return ( GetEfnAnim() ? GetEfnAnim()->CheckForRayIntersect( pstRay, pfDist, pvIntrPos ) : false );
}

void QObjMenuMenu::OnFrameMove( double fTime, float fElapsedTime )
{
	//if ( GetEfnAnim()->GetAnimName() == L"opened" )
	//{
	//	GetEfnAnim()->GetNullPos()->vRot.y += fElapsedTime * D3DX_PI / 4;
	//}
	//else
	//{
	//	GetEfnAnim()->GetNullPos()->vRot.y = D3DX_PI;
	//}

	//GetEfnAnim()->SetPos( GetEfnAnim()->GetNullPos() );

	if ( GetEfnAnim() )
		GetEfnAnim()->OnFrameMove( fTime, fElapsedTime );
}

void QObjMenuMenu::OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	if ( GetEfnAnim() && IsOpened() )
	{
		GetEfnAnim()->OnFrameRender( fTime, fElapsedTime, ORT_NONTRANSPARENT );
		GetEfnAnim()->OnFrameRender( fTime, fElapsedTime, ORT_TRANSPARENT );
	}
}