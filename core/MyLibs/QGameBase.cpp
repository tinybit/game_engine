#define STRICT

#include "QGameBase.h"

#include "QSubSysLog.h"

#include "QMgrScripts.h"

void QGameBase::DoEvent(QEngineEvents e)
{
	switch(e)
	{
		case ON_INIT:
			g_Script.ExecFunc(m_sName+L"_OnInit");
			break;

		case ON_KILL:
			g_Script.ExecFunc(m_sName+L"_OnKill");
			break;

		default:
			g_Log.WriteError(L"undefined QEngine event is basegame");
			break;
	}
}

std::wstring QGameBase::GetGameTypeName(GAMETYPE t)
{
	switch(t)
	{
		case GT_QUEST:
			return L"GT_QUEST";
			break;

		case GT_MENU:
			return L"GT_MENU";
			break;

		case GT_CONSOLE:
			return L"GT_CONSOLE";
			break;

		case GT_VIDEO:
			return L"GT_VIDEO";
			break;

		case GT_STOP:
			return L"GT_STOP";
			break;

		default:
			return L"";
			break;
	}
}

std::wstring QGameBase::GetActiveObjsStrList()
{
	if ( m_mActiveObjs.size() > 0 )
	{
		std::wstring s = L"";
		for( mapWStrQObjBase_iter i = m_mActiveObjs.begin(); i != m_mActiveObjs.end(); i++ )
		{
			s += i->first + L"\n";
		}
		return s;
	}
	return L"";
}

void QGameBase::LogEnabled(bool bLogEnabled)
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage( L"MiniGame \"" + GetName() + L"\" logging enabled");
	}
	else
	{
		g_Log.WriteMessage( L"MiniGame \"" + GetName() + L"\" logging disabled");
	}
}