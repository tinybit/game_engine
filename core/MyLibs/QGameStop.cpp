#define STRICT

#include "QGameStop.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "QMgrMiniGames.h"
#include "QMgrScripts.h"

void QGameStop::Init()
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

void QGameStop::Kill()
{
	Pause();

	OnLostDevice();
	OnDestroyDevice();

	DoEvent(ON_KILL);
}

void QGameStop::SetCam( const std::wstring& sActiveCamera )
{
	if(m_sActiveCamera==sActiveCamera)
		return;

	m_pActiveCamera = GetObjMgr()->GetObjCam(sActiveCamera);

	if ( m_pActiveCamera )
	{
		m_sActiveCamera = sActiveCamera;
	}
}