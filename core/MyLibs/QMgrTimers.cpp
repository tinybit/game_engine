#define STRICT

#include "QMgrTimers.h"

#include "time.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysLog.h"

#include "QEngine.h"
#include "QMgrScripts.h"

QMgrTimers::QMgrTimers()
{
	m_bLogEnabled = false;
}

QMgrTimers::~QMgrTimers()
{
	TimerKillAll();
}

void QMgrTimers::LogEnabled(bool bLogEnabled)
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"timers mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"timers mgr logging disabled");
	}
}

std::wstring QMgrTimers::TimersList()
{
	std::wstring s = L"-------------TIMERS LIST-------------\n\n";

	if ( m_mTimers.size() > 0 )
	{
		STimer* temp = NULL;
		for( timersmap_iter i = m_mTimers.begin(); i != m_mTimers.end(); i++ )
		{
			temp = &i->second;

			if ( temp )
				s += L"timer = \"" + temp->sTimerID + L"\", intrvl = " + int2wstr( temp->iInterval ) + L", Repeat = " + int2wstr( temp->iRepeatCount ) + L", paused = " + bool2wstr( temp->bPaused ) + L"\n";
		}
	}
	return s;
}

void QMgrTimers::TimerSet( const std::wstring& timer_id, int iInterval, const std::wstring& sScript )
{
	TimerSet2( timer_id, iInterval, sScript, -1 );
}

void QMgrTimers::TimerSet2( const std::wstring& timer_id, int iInterval, const std::wstring& sScript, int iRepeatCount )
{
	if ( m_mTimers.size() > 0 )
	{
		timersmap_iter i = m_mTimers.find(timer_id);

		if ( i != m_mTimers.end() )
		{
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning( L"Timer already exists, timer_id = \"" + timer_id + L"\"" );
				return;
			}
		}
	}

	STimer temp; 
	temp.sTimerID		= timer_id;
	temp.iRepeatCount	= iRepeatCount;
	temp.iInterval		= (int) ( (float)iInterval / g_Engine.GetGameSpeed() );
	if ( temp.iInterval == 0 )
		temp.iInterval = 1;
	temp.sScript		= sScript;
	temp.bPaused		= false;
	temp.iTimerID		= SetTimer( NULL, 0, temp.iInterval, OnTimerProc );

	if( temp.iTimerID!=0 )
	{
		m_mTimers2[temp.iTimerID] = temp.sTimerID;
		m_mTimers[temp.sTimerID] = temp;
	}
	else
	{
		g_Log.WriteError( L"Error create a timer, timer_id = \"" + timer_id + L"\"" );
	}
}

void QMgrTimers::TimerKill( const std::wstring& timer_id )
{
	if ( m_mTimers.size() > 0 )
	{
		timersmap_iter i = m_mTimers.find(timer_id);

		if ( i != m_mTimers.end() )
		{
			KillTimer( NULL, i->second.iTimerID );
			m_mTimers2.erase( i->second.iTimerID );
			m_mTimers.erase( timer_id );
		}
	}
}

void QMgrTimers::TimerKillAll()
{
	if ( m_mTimers.size() > 0 )
	{
		for( timersmap_iter i = m_mTimers.begin(); i != m_mTimers.end(); i++ )
		{
			m_mTimers2.erase(i->second.iTimerID);

			KillTimer( NULL, i->second.iTimerID );
		}
		m_mTimers.clear();
	}
}

void QMgrTimers::TimerPause( const std::wstring& timer_id )
{
	if ( m_mTimers.size() > 0 )
	{
		timersmap_iter i = m_mTimers.find(timer_id);

		if ( i != m_mTimers.end() )
		{
			i->second.bPaused = true;
		}
	}
}

void QMgrTimers::TimerContinue( const std::wstring& timer_id )
{
	if ( m_mTimers.size() > 0 )
	{
		timersmap_iter i = m_mTimers.find(timer_id);

		if ( i != m_mTimers.end() )
		{
			i->second.bPaused = false;
		}
	}
}

void CALLBACK OnTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	if ( g_Timers.m_mTimers2.size() > 0 )
	{
		timersmap2_iter j = g_Timers.m_mTimers2.find(idEvent);

		if ( j != g_Timers.m_mTimers2.end() )
		{
			timersmap_iter i = g_Timers.m_mTimers.find( j->second );

			if ( i != g_Timers.m_mTimers.end() )
			{
				if ( !i->second.bPaused )
				{
					if ( i->second.iRepeatCount>0 || i->second.iRepeatCount==-1 )
					{
						if(i->second.iRepeatCount!=-1)
						{
							i->second.iRepeatCount--;
						}

						g_Script.ExecScript( i->second.sScript );
					}
					else
					{
						g_Timers.TimerKill( j->second );
					}
				}
			}
		}
	}
}