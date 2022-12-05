#pragma once
#pragma warning ( disable: 4267 )

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QGameBase.h"
#include "QGameQuest.h"
#include "QGameVideo.h"
#include "QGameConsole.h"
#include "QGameMenu.h"
#include "QGameStop.h"

using namespace std;

typedef map< wstring, QGameBase*, std::less<wstring> > gamesmap;
typedef gamesmap::iterator gamesmap_iter;

typedef map< int, QGameBase*, std::less<int> > zmap;
typedef zmap::iterator zmap_iter;

#define g_GamesMgr QMgrMiniGames::GetSingleton()
//#define g_GameObjMgr(s) g_GamesMgr.GetObjMgr(s)
//#define g_GameObjMgrZ(i) g_GamesMgr.GetObjMgrZ(i)

//#define ConsoleObjMgr		g_GameObjMgrZ( 0 ) //Console
//#define MenuObjMgr		g_GameObjMgrZ( 10 ) //Menu
//#define VideoObjMgr		g_GameObjMgrZ( 90 ) //Video
//#define QuestObjMgr		g_GameObjMgrZ( 100 ) //Quest
//#define StopObjMgr		g_GameObjMgrZ( 9999 ) //Stop

class QMgrMiniGames : public Singleton <QMgrMiniGames>
{										   
public:
	QMgrMiniGames();
	~QMgrMiniGames();

	void					LogEnabled(bool);

	QGameVideo*				AddGameVideo( int iZ, const std::wstring& game_id, const std::wstring& bink_id );
	QGameQuest*				AddGameQuest( int iZ, const std::wstring& game_id );
	QGameConsole*			AddGameConsole( int iZ, const std::wstring& game_id );
	QGameMenu*				AddGameMenu( int iZ, const std::wstring& game_id );
	QGameStop*				AddGameStop( int iZ, const std::wstring& game_id );
	QGameBase*				AddGame( int iZ, QGameBase* pGame, const std::wstring& game_id, GAMETYPE t );
	
	void					InitGame( const std::wstring& game_id );

	QGameVideo*				GetGameVideo( const std::wstring& game_id ) { return (QGameVideo*)GetGame(game_id); };
	QGameQuest*				GetGameQuest( const std::wstring& game_id ) { return (QGameQuest*)GetGame(game_id); };
	QGameConsole*			GetGameConsole( const std::wstring& game_id ) { return (QGameConsole*)GetGame(game_id); };
	QGameMenu*				GetGameMenu( const std::wstring& game_id ) { return (QGameMenu*)GetGame(game_id); };
	QGameStop*				GetGameStop( const std::wstring& game_id ) { return (QGameStop*)GetGame(game_id); };
	QGameBase*				GetGame( const std::wstring& game_id );
	QGameBase*				GetGameZ( int iZ );

	QMgrResObjects*			GetObjMgr( const std::wstring& game_id );
	QMgrResObjects*			GetObjMgrZ( int iZ );

	zmap					GetSortedGameList() { return m_mZ; };
	UINT					GetGamesCount() { return m_mGames.size(); };
	
	void					DelGames();
	void					DelGame( const std::wstring game_id );
	
	void					PauseAll();
	void					Pause( const std::wstring& game_id );
	void					PauseMove( const std::wstring& game_id );
	void					PauseRender( const std::wstring& game_id );
	void					ContinueAll();
	void					Continue( const std::wstring& game_id );
	void					ContinueMove( const std::wstring& game_id );
	void					ContinueRender( const std::wstring& game_id );
	void					FreezeAll();
	void					UnFreezeAll();

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime );
	void					OnLostDevice();
	void					OnDestroyDevice();

private:

	gamesmap				m_mGames;
	zmap					m_mZ;

	bool					m_bLogEnabled;

};