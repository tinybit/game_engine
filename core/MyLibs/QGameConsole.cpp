#define STRICT

#include "QGameConsole.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "QMgrMiniGames.h"
#include "QMgrScripts.h"

void QGameConsole::Init()
{
	g_Script.RegisterEvents(GetName());

	DoEvent(ON_INIT);
	
	Continue();

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QGameConsole::Kill()
{
	Pause();

	OnLostDevice();
	OnDestroyDevice();

	DoEvent(ON_KILL);
}

void QGameConsole::SetCam( const std::wstring& sActiveCamera )
{
	if(m_sActiveCamera==sActiveCamera)
		return;

	m_pActiveCamera = GetObjMgr()->GetObjCam(sActiveCamera);

	if ( m_pActiveCamera )
	{
		m_sActiveCamera = sActiveCamera;
	}
}

void QGameConsole::OnCreateDevice()
{
}

void QGameConsole::OnResetDevice()
{
}

void QGameConsole::OnFrameMove( double fTime, float fElapsedTime )
{
}

void QGameConsole::OnFrameRender( double fTime, float fElapsedTime )
{
}
void QGameConsole::OnLostDevice()
{
}

void QGameConsole::OnDestroyDevice()
{
}