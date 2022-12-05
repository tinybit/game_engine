#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "CommonDefines.h"

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
} 
#include <luabind/luabind.hpp>

int lua_error_func(lua_State*);

#define g_Script QMgrScripts::GetSingleton()

class QMgrScripts : public Singleton <QMgrScripts>
{
public:
	QMgrScripts();
	~QMgrScripts();

	void					LogEnabled(bool);	

	bool					RegisterVariables();
	bool					RegisterVariables2();

	void					RegisterEvents( const std::wstring& s );

	bool					ExecFile( const std::wstring& sFile );
	bool					ExecScript( const std::wstring& sScript );
	void					ExecFunc( const std::wstring& f );
	bool					ExecBool( const std::wstring& sBoolExpression );

	bool					Exists( const std::wstring& s ) { return ExecBool( L" not ( " + s + L" == nil ) " ); };
	bool					Exists2( const std::wstring& class_name, const std::wstring& s ) { return ( Exists(class_name) ? Exists( class_name + L"." + s ) : false ); };
	
	bool					bExpressionResult;

private:
	
	lua_State*				luaVM;

	bool					m_bLogEnabled;

};