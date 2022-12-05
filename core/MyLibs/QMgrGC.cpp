#define STRICT

#include "QMgrGC.h"

#include "QEngine.h"

#include "QSubSysLog.h"

#include "IMgrResTextures.h"
#include "IMgrResEfnModels.h"
#include "IMgrResBinks.h"
#include "IMgrResFiles.h"
#include "QMgrResSounds.h"
#include "QMgrResStates.h"

QMgrGC::QMgrGC()
{
	m_bLogEnabled			= false;

	m_fLastCollectTime		= g_Engine.GetTime();
	m_fCollectionInterval	= 3*60;
}

QMgrGC::~QMgrGC()
{
	Collect( true );
}

void QMgrGC::LogEnabled(bool bLogEnabled)
{
	m_bLogEnabled = bLogEnabled;

	if ( m_bLogEnabled )
	{
		g_Log.WriteMessage(L"GC mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"GC mgr logging disabled");
	}
}

void QMgrGC::Collect( bool bForce )
{
	if ( GetTimeFromLastCollect() > GetCollectionInterval() || bForce )
	{
		m_fLastCollectTime = g_Engine.GetTime();

		if ( m_bLogEnabled )
		{
			g_Log.WriteWarning( L"Collect happends" );
		}
		g_StatesMgr.Collect();
		//g_TexMgr.Collect();
		//g_BinkMgr.Collect();
		//g_EfnMgr.Collect();
		//g_SndMgr.Collect();
		//g_FileMgr.Collect();
	}
}

float QMgrGC::GetTimeFromLastCollect()
{
	return ( g_Engine.GetTime() - m_fLastCollectTime );
}