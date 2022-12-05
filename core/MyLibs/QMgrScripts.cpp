#define STRICT

#include "QMgrScripts.h"

#include "QEngine.h"

#include "QSubSysLog.h"

QMgrScripts::QMgrScripts()
{
	m_bLogEnabled = false;
	bExpressionResult = false;

	luaVM = lua_open();
  
	if (luaVM == NULL) 
	{
		ShowMessage(L"Error initializing lua");
	} 
	else
	{
		// инициализация стандартных библиотечных функции lua
		lua_baselibopen(luaVM);
		lua_iolibopen(luaVM); 
		lua_strlibopen(luaVM); 
		lua_mathlibopen(luaVM); 

		luabind::open(luaVM);

		lua_register(luaVM, "_ALERT", lua_error_func);

		lua_atpanic(luaVM, lua_error_func);
	}
}

QMgrScripts::~QMgrScripts()
{
	lua_close(luaVM);
}

void QMgrScripts::LogEnabled(bool bLogEnabled)
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"script mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"script mgr logging disabled");
	}
}

void QMgrScripts::RegisterEvents( const std::wstring& s )
{
	g_Script.ExecFile(s+L"_Events.lua");
}

int lua_error_func(lua_State* l)
{
	try
	{
		const char* c = lua_tostring(l, -1);
		if(c!=NULL)
		{
			g_Log.WriteError(L"lua_error rised, error = \"" + str2wstr(c) + L"\"");
		}
		else
		{
			g_Log.WriteError(L"lua_error rised, but it is unpossible get the error text");	
		}
	}
	catch(...)
	{
		g_Log.WriteError(L"lua_error rised, but it is unpossible get the error text");	
	}
	return 1;
}

void QMgrScripts::ExecFunc( const std::wstring& f )
{
	using namespace luabind;

	if ( !Exists(f) )
	{
		g_Log.WriteError( L"failed to exec function = \"" + f + L"\", function not exists" );
		return;
	}
	if ( m_bLogEnabled )
	{
		g_Log.Write(L"#7777ff", f+L"();");
	}

	try
	{
		call_function<int>(luaVM, wstr2str(f).c_str());
	}
	catch(luabind::error& e)
	{
		try
		{
			const char* c = lua_tostring(e.state(), -1);
			if(c!=NULL)
			{
				g_Log.WriteError(L"lua exec func error, func=\"" + f + L"\" error=\"" + str2wstr(c) + L"\"");
			}
			else
			{
				g_Log.WriteError(L"lua exec func error, func=\"" + f + L"\" but it is unposible to get error text");			
			}
		}
		catch(...)
		{
			g_Log.WriteError(L"lua exec func error, func=\"" + f + L"\" but it is unposible to get error text");			
		}
	}
}

bool QMgrScripts::ExecFile( const std::wstring& file )
{
	if (file.length()==0)
		return false;

	if(luaVM)
	{
		std::wstring fullfile = L"data\\scripts\\" + file;

		if(g_FileMgr.FileExists(fullfile))
		{

			if(m_bLogEnabled)
			{
				g_Log.WriteMessage(L"exec file = \""+fullfile+L"\"");
			}

			g_Log.SetPrefix(file);

			if(lua_dofile(luaVM, wstr2str(fullfile).c_str())!=0)
			{
				g_Log.WriteError(L"failed exec file = \""+fullfile+L"\"");
				lua_error(luaVM);
			}

			g_Log.SetPrefix(L"");
			return true;
		}
		else
		{
			g_Log.WriteError(L"failed exec file = \""+fullfile+L"\", file not exists");
			return false;
		}
	}
	else
	{
		ShowMessage(L"script machine not started");
		return false;
	}
}

bool QMgrScripts::ExecScript( const std::wstring& sScript )
{
	if (sScript.length()==0)
	{
		if(m_bLogEnabled)
		{
			g_Log.WriteWarning(L"trying to execute empty script");
		}
		return false;
	}

	if(luaVM)
	{
		if(m_bLogEnabled)
		{
			g_Log.WriteMessage(L"--exec script<br>\n>---------------------------------<br>\n"+sScript+L"");
		}

		if( lua_dostring(luaVM, wstr2str(sScript).c_str()) !=0 )
		{
			g_Log.WriteError(L"failed exec script = \""+sScript+L"\"");
			lua_error(luaVM);
		}

		return true;
	}
	else
	{
		ShowMessage(L"script machine not started");
		return false;
	}
}

bool QMgrScripts::ExecBool( const std::wstring& sBoolExpression )
{
	if ( sBoolExpression == L"" )
	{
		return false;
	}
	else
	{
		ExecScript( L"g_Script.bExpressionResult = ( " + sBoolExpression + L" );" ); 
		return bExpressionResult;
	}
}