#define STRICT

#include "QGameMenu.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysInput.h"
#include "QSubSysLog.h"

#include "QMgrScripts.h"
#include "QMgrMiniGames.h"

#include "QObjBase.h"
#include "QObjMan.h"
#include "QObjCamera.h"
#include "QObjLight.h"
#include "QObjShadowVolume.h"
#include "QObjLayerGeom.h"
#include "QObjAction.h"
#include "QObjEfnAnim.h"
#include "QObjVideoAnim.h"
#include "QObjMenuRukzak.h"
#include "QObjMenuCursor.h"
#include "QObjMenuMenu.h"


void QGameMenu::Init()
{
	g_Script.RegisterEvents( GetName() );

	DoEvent(ON_INIT);

	D3DLIGHT9 temp_light;
	ZeroMemory( &temp_light, sizeof(D3DLIGHT9) );
	temp_light.Type			= D3DLIGHT_DIRECTIONAL;
	temp_light.Position		= D3DXVECTOR3( -10.0f, 40.0f, -5.0f );
	temp_light.Direction	= D3DXVECTOR3(   1.0f, -1.0f,  5.0f );
	temp_light.Ambient		= D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	temp_light.Diffuse		= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	temp_light.Specular		= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	temp_light.Attenuation1	= 1.0f;
	temp_light.Range		= 100.0f;

	GetObjMgr()->AddObjLight( L"LightFromLeftUp", &temp_light );

	//plane
	D3DXPlaneFromPointNormal( &plane, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,-1) );

	m_pRukzak	= GetObjMgr()->GetObjMenuRukzak( L"rukzak" );
	m_pCursor	= GetObjMgr()->GetObjMenuCursor( L"cursor" );
	m_pMenu		= GetObjMgr()->GetObjMenuMenu( L"menu" );
	
	Continue();

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QGameMenu::Kill()
{
	Pause();

	OnLostDevice();
	OnDestroyDevice();

	DoEvent(ON_KILL);
}

void QGameMenu::SetCam( const std::wstring& sActiveCamera )
{
	if(m_sActiveCamera==sActiveCamera)
		return;

	m_pActiveCamera = GetObjMgr()->GetObjCam(sActiveCamera);

	if ( m_pActiveCamera )
	{
		m_sActiveCamera = sActiveCamera;
	}
}

void QGameMenu::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( m_pRukzak )
		m_pRukzak->OnFrameMove( fTime, fElapsedTime );
	if ( m_pMenu )
		m_pMenu->OnFrameMove( fTime, fElapsedTime );
	if ( m_pCursor )
		m_pCursor->OnFrameMove( fTime, fElapsedTime );
}

void QGameMenu::OnFrameRender( double fTime, float fElapsedTime )
{
	if ( g_GamesMgr.GetGamesCount() == 1 )
	{
		g_Graph.Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00222255, 1.0f, 0);
	}
	else
	{
		g_Graph.Clear(0, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	}

	g_Graph.SetLightsFromGroup( GetObjMgr(), L"Lights" );

	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );

	g_Graph.SetLightingEnabled( true );
	g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD );

	g_Graph.dev()->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, true );
	
	Cam()->SetOrthoCam();
	
	if ( m_pRukzak )
		m_pRukzak->OnFrameRender( GetGameTime(), GetGameElapsedTime(), ORT_NONTRANSPARENT );

	Cam()->SetPerCam();

	if ( m_pMenu )
		m_pMenu->OnFrameRender( GetGameTime(), GetGameElapsedTime(), ORT_NONTRANSPARENT );

	RenderCursor();
	
	g_Graph.dev()->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,	false );

	//Render obj list
	if ( g_StatesMgr.GetStateB( L"Menu.rp_ShowObjList" ) && g_Graph.GetDebug() )
	{
		int a = g_StatesMgr.GetState(L"Menu.ObjListStartY");
		if ( a == 99999 )
		{
			g_Graph.RenderText5( -1, 1, GetObjMgr()->GetObjList(), 0xff00ff00, 0x80000000, L"small", 5 );
		}
		else if ( a == -99999 )
		{
			g_Graph.RenderText5( -1, -1, GetObjMgr()->GetObjList(), 0xff00ff00, 0x80000000, L"small", 5 );
		}
		else
		{
			g_Graph.RenderText4( 5, a, GetObjMgr()->GetObjList(), 0xff00ff00, 0x80000000, L"small", 5 );
		}
	}

	return;
}

void QGameMenu::RenderCursor()
{
	if ( g_Input.GetEnabled() == false )
		return;

	if ( m_pCursor )
	{
		g_Input.UpdateMouse();

		D3DXVec3Unproject( &pos_s, &D3DXVECTOR3(g_Mouse.fx, g_Mouse.fy, 0.0f), &g_Graph.vp, &Cam()->matProjPer, &Cam()->matViewPer, &Cam()->matIdentity );
		D3DXVec3Unproject( &pos_e, &D3DXVECTOR3(g_Mouse.fx, g_Mouse.fy, 5000.0f), &g_Graph.vp, &Cam()->matProjPer, &Cam()->matViewPer, &Cam()->matIdentity );

		D3DXPlaneIntersectLine( &pos, &plane, &pos_s, &pos_e );

		m_pCursor->GetNullPos()->vPos = pos;
		m_pCursor->GetEfnAnim()->SetPosition( &pos );
		m_pCursor->OnFrameRender( GetGameTime(), GetGameElapsedTime(), ORT_NONTRANSPARENT );
	}
}