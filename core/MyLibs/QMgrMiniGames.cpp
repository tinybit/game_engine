#define STRICT

#include "QMgrMiniGames.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "QMgrScripts.h"


QMgrMiniGames::QMgrMiniGames()
{
	m_bLogEnabled = false;
}

QMgrMiniGames::~QMgrMiniGames()
{
	DelGames();
}

void QMgrMiniGames::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage( L"QMgrMiniGames logging enabled");
	}
	else
	{
		g_Log.WriteMessage( L"QMgrMiniGames logging disabled");
	}
}

QGameVideo* QMgrMiniGames::AddGameVideo( int iZ, const std::wstring& game_id, const std::wstring& bink_id ) 
{
	return (QGameVideo*)AddGame( iZ, new QGameVideo( bink_id ), game_id, GT_VIDEO ); 
}

QGameQuest* QMgrMiniGames::AddGameQuest( int iZ, const std::wstring& game_id )
{
	return (QGameQuest*)AddGame( iZ, new QGameQuest(), game_id, GT_QUEST );
}

QGameConsole* QMgrMiniGames::AddGameConsole( int iZ, const std::wstring& game_id )
{
	return (QGameConsole*)AddGame( iZ, new QGameConsole(), game_id, GT_CONSOLE );
}

QGameMenu* QMgrMiniGames::AddGameMenu( int iZ, const std::wstring& game_id)
{
	return (QGameMenu*)AddGame( iZ, new QGameMenu(), game_id, GT_MENU );
}

QGameStop* QMgrMiniGames::AddGameStop( int iZ, const std::wstring& game_id)
{
	return (QGameStop*)AddGame( iZ, new QGameStop(), game_id, GT_STOP );
}

QGameBase* QMgrMiniGames::AddGame( int iZ, QGameBase* pGame, const std::wstring& game_id, GAMETYPE t )
{
	if ( pGame == NULL )
	{
		g_Log.WriteError( L"pGame is null in \"QBaseGame* QMgrMiniGames::AddGame( " + int2wstr(iZ) + L", pGame, " + game_id + /*L", " + QBaseGame::GetGameTypeName(t) +*/ L" );\"" );
		return NULL;
	}

	DelGame( game_id );

	zmap_iter i = m_mZ.find(iZ);

	if ( i != m_mZ.end() )
	{
		g_Log.WriteError( L"game = \"" + i->second->GetName() + L"\" already exists in Z = " + int2wstr(iZ) + L", new game_id = \"" + game_id + L"\"" );
		DelGame( i->second->GetName() );
	}

	pGame->Pause();
	pGame->SetName(game_id);
	pGame->SetGameType(t);
	pGame->SetZ( iZ );
	pGame->ZeroCam();
	pGame->ZeroGameTime();
	pGame->GetObjMgr()->SetGame( pGame );
	pGame->LogEnabled( m_bLogEnabled );

	m_mGames[game_id] = pGame;
	m_mZ[iZ] = pGame;

	switch( t )
	{
		case GT_VIDEO:
			g_Script.ExecScript( game_id + L" = g_GamesMgr:GetGameVideo( L(\"" + pGame->GetName() + L"\") );" );
			break;

		case GT_QUEST:
			g_Script.ExecScript( game_id + L" = g_GamesMgr:GetGameQuest( L(\"" + pGame->GetName() + L"\") );" );
			break;

		case GT_CONSOLE:
			g_Script.ExecScript( game_id + L" = g_GamesMgr:GetGameConsole( L(\"" + pGame->GetName() + L"\") );" );
			break;

		case GT_MENU:
			g_Script.ExecScript( game_id + L" = g_GamesMgr:GetGameMenu( L(\"" + pGame->GetName() + L"\") );" );
			break;

		case GT_STOP:
			g_Script.ExecScript( game_id + L" = g_GamesMgr:GetGameStop( L(\"" + pGame->GetName() + L"\") );" );
			break;
	}
	g_Script.ExecScript( pGame->GetName() + L"ObjMgr = " + game_id + L":GetObjMgr();" );
	g_Script.ExecScript( pGame->GetName() + L"OMgr = LuaClass();" );

	if ( m_bLogEnabled )
	{
		g_Log.WriteWarning( L"game added, game_id = \"" + game_id + L"\"" );
	}

	return pGame;
}

QGameBase* QMgrMiniGames::GetGame( const std::wstring& game_id )
{
	gamesmap_iter i = m_mGames.find(game_id);

	return ( ( i != m_mGames.end() ) ? i->second : NULL );
}

QGameBase* QMgrMiniGames::GetGameZ( int iZ )
{
	zmap_iter i = m_mZ.find(iZ);

	return ( ( i != m_mZ.end() ) ? i->second : NULL );
}

QMgrResObjects* QMgrMiniGames::GetObjMgr( const std::wstring& game_id )
{
	QGameBase* temp = GetGame( game_id );

	if ( !temp )
		return NULL;

	return temp->GetObjMgr();
}

QMgrResObjects* QMgrMiniGames::GetObjMgrZ( int iZ )
{
	QGameBase* temp = GetGameZ( iZ );

	if ( !temp )
		return NULL;

	return temp->GetObjMgr();
}

void QMgrMiniGames::DelGames()
{
	while(m_mGames.size()>0)
	{
		DelGame( m_mGames.begin()->first );
	}
}

void QMgrMiniGames::DelGame( const std::wstring game_id )
{
	if ( m_mGames.size() > 0 )
	{
		gamesmap_iter i = m_mGames.find(game_id);

		if ( i != m_mGames.end() )
		{
			QGameBase* temp = i->second;
			int m_iZ = -1;
			if(temp)
			{
				m_iZ = temp->GetZ();
				temp->Kill();
				SAFE_DELETE( temp );
			}

			m_mGames.erase( game_id );
			m_mZ.erase( m_iZ );

			g_Script.ExecScript( game_id + L" = nil;" );
			g_Script.ExecScript( game_id + L"ObjMgr = nil;" );
			g_Script.ExecScript( game_id + L"OMgr = nil;" );

			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning( L"game deleted, game_id = \"" + game_id + L"\"" );
			}
		}
	}
}

void QMgrMiniGames::InitGame( const std::wstring& game_id)
{
	QGameBase* pGame = GetGame( game_id );
	
	if (pGame) 
	{
		try
		{
			pGame->Init();
		}
		catch(...)
		{
			g_Log.WriteError( L"exception in \"void QMgrMiniGames::InitGame()\", game_id = \"" + game_id + L"\"" );
		}
	}
}

void QMgrMiniGames::FreezeAll()
{
	if( m_mGames.size()>0 )
	{
		QGameBase* game = NULL;
		for( gamesmap_iter i = m_mGames.begin(); i != m_mGames.end(); ++i )
		{
			game = i->second;
			if (game) 
				game->Freeze();
		}
	}
}

void QMgrMiniGames::UnFreezeAll()
{
	if( m_mGames.size()>0 )
	{
		QGameBase* game = NULL;
		for( gamesmap_iter i = m_mGames.begin(); i != m_mGames.end(); ++i )
		{
			game = i->second;
			if (game) 
				game->UnFreeze();
		}
	}
}

void QMgrMiniGames::PauseAll()
{
	if( m_mGames.size()>0 )
	{
		for( gamesmap_iter i = m_mGames.begin(); i != m_mGames.end(); ++i )
		{
			Pause( i->first );
		}
	}
}

void QMgrMiniGames::Pause( const std::wstring& game_id )
{
	QGameBase* game = GetGame( game_id );
	
	if (game) 
		game->Pause();
}

void QMgrMiniGames::PauseMove( const std::wstring& game_id )
{
	QGameBase* game = GetGame( game_id );
	
	if (game) 
		game->PauseMove();
}

void QMgrMiniGames::PauseRender( const std::wstring& game_id )
{
	QGameBase* game = GetGame( game_id );
	
	if (game) 
		game->PauseRender();
}

void QMgrMiniGames::ContinueAll()
{
	if ( m_mGames.size()>0 )
	{
		for( gamesmap_iter i = m_mGames.begin(); i != m_mGames.end(); ++i)
		{
			Continue( i->first );
		}
	}
}

void QMgrMiniGames::Continue( const std::wstring& game_id )
{
	QGameBase* game = GetGame(game_id);
	
	if (game) 
		game->Continue();
}

void QMgrMiniGames::ContinueMove( const std::wstring& game_id )
{
	QGameBase* game = GetGame(game_id);
	
	if (game) 
		game->ContinueMove();
}

void QMgrMiniGames::ContinueRender( const std::wstring& game_id )
{
	QGameBase* game = GetGame(game_id);
	
	if (game) 
		game->ContinueRender();
}

void QMgrMiniGames::OnCreateDevice()
{
	if ( m_mZ.size()>0 )
	{
		QGameBase* game = NULL;
		for( zmap_iter i = m_mZ.begin(); i != m_mZ.end(); i++ )
		{
			game = i->second;
			if ( game )
				game->OnCreateDevice();
		}
	}
}

void QMgrMiniGames::OnResetDevice()
{
	if ( m_mZ.size()>0 )
	{
		QGameBase* game = NULL;
		
		for( zmap_iter i = m_mZ.begin(); i != m_mZ.end(); i++ )
		{
			game = i->second;
			if ( game )
				game->OnResetDevice();
		}
	}
}

void QMgrMiniGames::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( m_mZ.size()>0 )
	{
		QGameBase* game = NULL;

		zmap m_mZTemp = m_mZ;
		for( zmap_iter i = m_mZTemp.begin(); i != m_mZTemp.end(); i++ )
		{
			game = i->second;
			
			if ( game )
			{
				if ( !game->isMovePaused() && !game->isFreezed() )
				{
					game->IncGameTime( fElapsedTime );
					game->OnFrameMove( game->GetGameTime(), fElapsedTime );
				}
			}
		}
	}
}

void QMgrMiniGames::OnFrameRender( double fTime, float fElapsedTime )
{
	if ( m_mZ.size()>0 )
	{
		QGameBase* game = NULL;
		//игры отрисовываем снизу вверх
		zmap_iter i = m_mZ.end();
		while( i != m_mZ.begin() )
		{
			i--;

			game = i->second;
			
			if ( game )
			{
				if ( !game->isRenderPaused() && !game->isFreezed() )
					game->OnFrameRender( game->GetGameTime(), fElapsedTime );
			}
		}
	}
	else
	{
		g_Graph.Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
		g_Graph.RenderText5( 0, 0, L"ни одна игра не запущена", 0xffff0000, 0, L"", 0 );
	}
}

void QMgrMiniGames::OnLostDevice()
{
	if ( m_mZ.size()>0 )
	{
		QGameBase* game = NULL;
		for( zmap_iter i = m_mZ.begin(); i != m_mZ.end(); i++ )
		{
			game = i->second;
			
			if ( game )
				game->OnLostDevice();
		}
	}
}

void QMgrMiniGames::OnDestroyDevice()
{
	if ( m_mZ.size()>0 )
	{
		QGameBase* game = NULL;
		for( zmap_iter i = m_mZ.begin(); i != m_mZ.end(); i++ )
		{
			game = i->second;
			
			if ( game )
				game->OnDestroyDevice();
		}
	}
}
