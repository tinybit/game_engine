#define STRICT

#include "QObjFree.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "QMgrResSounds.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"

QObjFree::QObjFree( const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, bool bFillGopa )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );

	m_bFillGopa		= bFillGopa;
	m_pGopa			= NULL;

	m_pQuestScn		= NULL;

	SetEfnID( efn_id );
	SetEfnID2( meshes_efn_id );		
	SetTexID( tex_id );
	SetTexID2( tex_id2 );
}

QObjFree::~QObjFree()
{
    Stop( false );
}

void QObjFree::Init()
{
	if ( m_bFillGopa )
	{
		if ( GetGame() )
		{
			if ( GetGame()->GetGameType() == GT_QUEST )
			{
				m_pQuestScn = ((QGameQuest*)GetGame())->GetScene();
			}
			else
			{
				g_Log.WriteError( L"FreeObj with gopa fill can only be created in game Quest, obj_id = \"" + GetObjID() + L"\"" );
			}
		}
	}

	if ( GetObjMgr()->AddObjEfnAnimE( GetEfnAnimObjID(), GetEfnID(), GetEfnID2(), GetTexID(), GetTexID2() ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
		GetEfnAnim()->SetRenderPrms( true, false, true );
	}

	SetGopaRadius( g_EfnMgr.GetGlobalCylinderRadius( GetEfnID() ) );

	if ( m_bFillGopa )
		m_pQuestScn->MakeGopa( &m_pGopa, &m_iGopaSize, GetGopaRadius() );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjFree::Kill()
{
	if ( m_bFillGopa )
	{
		int x, z;
		m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
		m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );
	}

	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();

	SAFE_DELETE_ARRAY(m_pGopa);
}

void QObjFree::SetPos( const SNULL* pstNull )
{
	SetNullPos( pstNull );

	if ( m_bFillGopa )
	{
		int x, z;
		m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
		m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANTWALK );
	}

	if ( GetEfnAnim() )
		GetEfnAnim()->SetPos( GetNullPos() );
}

void QObjFree::Teleport( const SNULL* pstNull )
{
	if ( m_bFillGopa )
	{
		int x, z;
		m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
		m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );
	}

	SetPos( pstNull );
}


void QObjFree::ObjRender( DWORD rt )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->ObjRender( rt );
}

void QObjFree::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->OnFrameMove( fTime, fElapsedTime );
}

void QObjFree::Go( double fTime, int seq_id )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->Go( fTime, 0, seq_id );
}

void QObjFree::Stop(bool bOK)
{
	if ( GetEfnAnim() )
		GetEfnAnim()->Stop( bOK );
}