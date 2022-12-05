#define STRICT

#include "QGameVideo.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResTextures.h"
#include "QMgrScripts.h"
#include "QMgrResStates.h"
#include "QMgrMiniGames.h"

#include "QObjBase.h"
#include "QObjCamera.h"
#include "QObjLight.h"
#include "QObjVideoAnim.h"


QGameVideo::QGameVideo( const std::wstring& bink_id )
{
	m_bsVB		= NULL;

	m_fFrame	= 0.0f;

	m_sBinkID	= bink_id;

	m_bCanStop			= false;
	m_bStretchToScreen	= false;
	m_iSeqID			= -1;
}

void QGameVideo::SetPrm( bool bCanStop, bool bStretchToScreen, int iSeqID )
{
	m_bCanStop			= bCanStop;
	m_bStretchToScreen	= bStretchToScreen;
	m_iSeqID			= iSeqID;
}

void QGameVideo::Init()
{
	GetObjMgr()->AddObjCam( L"GameVideoCam" );
	SetCam( L"GameVideoCam" );

	if ( m_bCanStop )
	{
		g_StatesMgr.SetEvent( GetName(), GetName() + L".Click", L"sys.mouseLeftClick,sys.mouseRightClick,sys.keybKeyCode", L"SeqIncState(" + int2wstr(m_iSeqID) + L");");
	}

	g_TexMgr.LoadVideo( L"GameVideo", m_sBinkID, true, 0, false );

	Continue();

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QGameVideo::Kill()
{
	g_TexMgr.FreeTex( m_sBinkID );

	OnLostDevice();
	OnDestroyDevice();

	g_StatesMgr.DelEvents( GetName() );
}

void QGameVideo::SetCam( const std::wstring& sActiveCamera )
{
	if(m_sActiveCamera==sActiveCamera)
		return;

	m_pActiveCamera = GetObjMgr()->GetObjCam(sActiveCamera);

	if ( m_pActiveCamera )
	{
		m_sActiveCamera = sActiveCamera;
	}
}

void QGameVideo::OnCreateDevice()
{
	if( FAILED( g_Graph.dev()->CreateVertexBuffer(	4 * sizeof(POS_TEX_VERTEX),
													D3DUSAGE_WRITEONLY, POS_TEX_FVF,
													D3DPOOL_MANAGED, 
													&m_bsVB, NULL ) ) )
	{
		g_Log.WriteError( L"cant create vertex buffer in \"void QVideo::OnCreateDevice()\"" );
		return;
	}

	QTextureListItem* t = g_TexMgr.GetTexItem( m_sBinkID );

	if ( m_bStretchToScreen || 
		((float)t->ImgWidth / g_Graph.fAspect ) - (float)t->ImgHeight < 0.01 )
	{
		m_vPos		= D3DXVECTOR2( 0, 0 );
		m_vSize		= D3DXVECTOR2( g_Graph.fMaxX, g_Graph.fMaxY );
		m_bClearScr	= false;
	}
	else
	{
		if ( ((float)t->ImgWidth / g_Graph.fAspect ) >= (float)t->ImgHeight )
		{
			m_vPos.x	= 0;
			m_vSize.x	= g_Graph.fMaxX;

			m_vSize.y	= g_Graph.fHeight * ( (float)t->ImgHeight / ((float)t->ImgWidth / g_Graph.fAspect ) );
			m_vPos.y	= ( g_Graph.fHeight - m_vSize.y ) * 0.5f; 
		}
		else
		{
			m_vPos.y	= 0;
			m_vSize.y	= g_Graph.fMaxY;

			m_vSize.x	= g_Graph.fWidth * ( ((float)t->ImgWidth / g_Graph.fAspect ) / (float)t->ImgHeight );
			m_vPos.x	= ( g_Graph.fWidth - m_vSize.x ) * 0.5f;
		}
		m_bClearScr	= true;
	}

	D3DXVECTOR3 vOffset( -0.5f, 0.5f, 0.0f );

	POS_TEX_VERTEX* v = NULL;
	if ( FAILED( m_bsVB->Lock( 0, 0, (void**)&v, 0 ) ) )
		return;

		v[0].p = D3DXVECTOR3( m_vPos.x,				m_vPos.y + m_vSize.y,	0.0f ) + vOffset;
		v[1].p = D3DXVECTOR3( m_vPos.x + m_vSize.x,	m_vPos.y + m_vSize.y,	0.0f ) + vOffset;
		v[2].p = D3DXVECTOR3( m_vPos.x,				m_vPos.y,				0.0f ) + vOffset;
		v[3].p = D3DXVECTOR3( m_vPos.x + m_vSize.x,	m_vPos.y,				0.0f ) + vOffset;
		
		v[0].u = 0.0f;				v[0].v = 0.0f;
		v[1].u = t->fWidthAspect;	v[1].v = 0.0f;
		v[2].u = 0.0f;				v[2].v = t->fHeightAspect;
		v[3].u = t->fWidthAspect;	v[3].v = t->fHeightAspect;

    m_bsVB->Unlock();
}

void QGameVideo::OnFrameRender( double fTime, float fElapsedTime )
{
	QTextureListItem* temp = g_TexMgr.GetTexItem( m_sBinkID );

	if ( !temp )
	{
		g_Script.ExecScript(L"SeqIncState(" + int2wstr(m_iSeqID) + L");");
		return;
	}

	m_fFrame += fElapsedTime * temp->fFrameRate;
	int FramesCount = temp->iFramesCount;
	if ( (int)m_fFrame > FramesCount )
	{
		g_Script.ExecScript(L"SeqIncState(" + int2wstr(m_iSeqID) + L");");
		return;
	}

	g_Graph.SetLightingEnabled( false );
	g_Graph.SetAlphaBlendOff();

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_FALSE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		false );
	g_Graph.SetLightingEnabled( false );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE  );
	
	g_Graph.SetFiltering( 0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_POINT );

	if ( m_bClearScr )
	{
		g_Graph.Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	}

	g_Graph.dev()->SetTexture( 0, g_TexMgr.GetTex( m_sBinkID, m_fFrame ) );

	Cam()->SetOrthoCam();
	g_Graph.dev()->SetStreamSource( 0, m_bsVB, 0, sizeof(POS_TEX_VERTEX) );
	g_Graph.dev()->SetFVF( POS_TEX_FVF );
	g_Graph.dev()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void QGameVideo::OnDestroyDevice()
{
	SAFE_RELEASE(m_bsVB);
}