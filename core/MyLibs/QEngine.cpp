#define STRICT

#include "QEngine.h"

#include "QSubSysLog.h"

#include "QMgrGC.h"
#include "QMgrResFiles.h"
#include "QMgrResTextures.h"
#include "QMgrResBinks.h"
#include "QMgrResEfnModels.h"
#include "QMgrResFonts.h"

QEngine::QEngine()
{
	ZeroTime();

	bExitFlag					= true;

	m_pTimers					= new QMgrTimers();
	m_pStates					= new QMgrResStates();
	
	m_pGraph					= new QSubSysGraphics();
	m_pInput					= new QSubSysInput();

	m_pFileMan					= new QMgrResFiles();
	m_pScripts					= new QMgrScripts();
	m_pTex						= new QMgrResTextures();
	m_pBinks					= new QMgrResBinks();
	m_pFonts					= new QMgrResFonts();
	m_pEfn						= new QMgrResEfnModels();
	m_pShad						= new QMgrResShaders();
	m_pSnd						= new QMgrResSounds();

	m_pGC						= new QMgrGC();

	m_pGames					= new QMgrMiniGames();
}

QEngine::~QEngine()
{
	SAFE_DELETE(m_pGames);

	SAFE_RELEASE(m_pGC);

	SAFE_DELETE(m_pSnd);
	SAFE_DELETE(m_pShad);
	SAFE_RELEASE(m_pEfn);
	SAFE_RELEASE(m_pFonts);
	SAFE_RELEASE(m_pBinks);
	SAFE_RELEASE(m_pTex);
	SAFE_DELETE(m_pScripts);
	SAFE_RELEASE(m_pFileMan);

	SAFE_DELETE(m_pInput);
	SAFE_DELETE(m_pGraph);
	
	SAFE_DELETE(m_pStates);
	SAFE_DELETE(m_pTimers);
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void QEngine::Init( const std::wstring& sName )
{
	m_sName = sName;

	g_Script.RegisterVariables();
	g_Script.RegisterVariables2();

	g_Script.RegisterEvents(m_sName);

	g_Script.ExecFunc( m_sName+L"_OnInit" );   //QEngine_OnInit

	if ( g_Graph.GetFlagGraphInitOK() )
		bExitFlag = false;
}

//--------------------------------------------------------------------------------------
// Close the app 
//--------------------------------------------------------------------------------------
void QEngine::Kill()
{
	g_Script.ExecFunc( m_sName+L"_OnKill" );
}

HRESULT CALLBACK QEngine::OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
	g_Graph.OnCreateDevice();

	g_TexMgr.OnCreateDevice();
	g_EfnMgr.OnCreateDevice();
	g_ShadMgr.OnCreateDevice();
	g_FontMgr.OnCreateDevice();

	g_Input.InitInput();

	g_GamesMgr.OnCreateDevice();

	return S_OK;
}

HRESULT CALLBACK QEngine::OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
	g_Graph.OnResetDevice();

	g_TexMgr.OnResetDevice();
	g_EfnMgr.OnResetDevice();
	g_ShadMgr.OnResetDevice();
	g_FontMgr.OnResetDevice();

	g_GamesMgr.OnResetDevice();
	g_SndMgr.OnResetDevice();

	return S_OK;
}

void CALLBACK QEngine::OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
	if ( fElapsedTime > 0.1f )
		return;

	g_Engine.IncTime( fElapsedTime );

	g_Input.UpdateInput();

	g_StatesMgr.EvStackProc();

	g_GC.Collect( false );

	g_GamesMgr.OnFrameMove( g_Engine.GetTime(), g_Engine.GetElapsedTime() );

	g_Graph.OnFrameMove( g_Engine.GetTime(), g_Engine.GetElapsedTime() );
}

void CALLBACK QEngine::OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
	if ( fElapsedTime > 0.1f )
		return;

	if( SUCCEEDED( g_Graph.dev()->BeginScene() ) )
    {
		g_GamesMgr.OnFrameRender( g_Engine.GetTime(), g_Engine.GetElapsedTime() );

		g_Graph.OnFrameRender( g_Engine.GetTime(), g_Engine.GetElapsedTime() );
		
        g_Graph.dev()->EndScene();
    }
}

void CALLBACK QEngine::OnLostDevice()
{
	g_Graph.OnLostDevice();

	g_SndMgr.OnLostDevice();
	g_TexMgr.OnLostDevice();
	g_EfnMgr.OnLostDevice();
	g_ShadMgr.OnLostDevice();
	g_FontMgr.OnLostDevice();

	g_GamesMgr.OnLostDevice();
}

void CALLBACK QEngine::OnDestroyDevice()
{
	g_Graph.OnDestroyDevice();

	g_Input.ReleaseInput();

	g_TexMgr.OnDestroyDevice();
	g_EfnMgr.OnDestroyDevice();
	g_ShadMgr.OnDestroyDevice();
	g_FontMgr.OnDestroyDevice();

	g_GamesMgr.OnDestroyDevice();
}

LRESULT CALLBACK QEngine::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
//	g_Graph.MsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing );
	
	return 0;
}

void CALLBACK QEngine::KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  )
{
	g_Input.KeyboardProc( nChar, bKeyDown, bAltDown );
}

void CALLBACK QEngine::MouseProc(	bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
								bool bSideButton2Down, INT nMouseWheelDelta, INT xPos, INT yPos)
{
	g_Input.MouseProc(bLeftButtonDown, bRightButtonDown, bMiddleButtonDown, bSideButton1Down,
				      bSideButton2Down, nMouseWheelDelta, xPos, yPos);
}

void QEngine::Exit()
{
	DXUTPause( true, true ); 
	bExitFlag = true;
}

bool ParseCmdLine( const std::wstring& sCmd )
{
	listWStr slo;
	mSplit2( slo, sCmd, L" " );

	vecWStr sl;

	sl.reserve( slo.size() );
	for( listWStr_iter i = slo.begin(); i != slo.end(); i++ )
	{
		sl.push_back( str2wstr(TrimStr( wstr2str(*i) )) );
	}

	for( int i = 0; i < sl.size(); )
	{
		if ( sl[i] == L"-w1024" )
			g_Graph.SetForceRezMode(3);
		else if ( sl[i] == L"-w800" )
			g_Graph.SetForceRezMode(2);
		else if ( sl[i] == L"-w640" )
			g_Graph.SetForceRezMode(1);

		else if ( sl[i] == L"-disp0" )
			g_Graph.SetForceDisp(0);
		else if ( sl[i] == L"-disp1" )
			g_Graph.SetForceDisp(1);

		else if ( sl[i] == L"-bpp16" )
			g_Graph.SetForceBits(16);
		else if ( sl[i] == L"-bpp32" )
			g_Graph.SetForceBits(32);

		else if ( sl[i] == L"-texbpp16" )
			g_Graph.SetForceTexBits(16);
		else if ( sl[i] == L"-texbpp32" )
			g_Graph.SetForceTexBits(32);

		else if ( sl[i] == L"-forcesw" )
			g_Graph.SetForceSW( true );

		else if ( sl[i] == L"-fs" )
			g_Graph.SetForceFS( true );

		else if ( sl[i] == L"-debug" )
			g_Graph.SetForceDebug( true );

		else if ( sl[i] == L"-scn" )
		{
			g_Graph.SetForceStartScene( sl[i+1] );
			i += 2;
			continue;
		}

		else if ( sl[i] == L"-null" )
		{
			g_Graph.SetForceStartNull( sl[i+1] );
			i += 2;
			continue;
		}

		else if ( sl[i] == L"-help" || sl[i] == L"-h" || sl[i] == L"?" || sl[i] == L"/?" || sl[i] == L"-?" )
		{
			std::wstring s = L"resolution:\n";
			s += L"            -w1024\n";
			s += L"            -w800\n";
			s += L"            -w640\n\n";
			s += L"display:\n";
			s += L"            -disp0\n";
			s += L"            -disp1\n\n";
			s += L"bits per pixel:\n";
			s += L"            -bpp16\n";
			s += L"            -bpp32\n\n";
			s += L"bits per texel:\n";
			s += L"            -texbpp16\n";
			s += L"            -texbpp32\n\n";
			s += L"TnL:\n";
			s += L"            -forcesw\n\n";
			s += L"run in fullscreen:\n";
			s += L"            -fs\n\n";
			s += L"debug mode:\n";
			s += L"            -debug\n\n";
			s += L"start scene: (works only on boot)\n";
			s += L"            -scn scene_id -null null_id\n\n";
			ShowMessage( s );
			return false;
		}
		else
			g_Log.WriteError( L"unspecified command in command line = \"" + sl[i] + L"\"" );

		i++;
	}
	return true;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR pStrArr, int iPrmCount )
{
	QSubSysLog* m_pLog = NULL;

	try	{ m_pLog = new QSubSysLog(L"log.html");	} catch(...) {	SAFE_DELETE( m_pLog ); }

	if ( !m_pLog )
	{
		ShowMessage(L"QEngine log sub system start failed!!!");
		return 1;
	}

	QEngine* m_pEngine = NULL;

	try { m_pEngine = new QEngine(); } catch(...) { SAFE_DELETE( m_pEngine ); }

	if ( !m_pEngine )
	{
		g_Log.WriteError( L"QEngine start failed!!!" );
		SAFE_DELETE(m_pLog);
		return 1;
	}

	if ( ParseCmdLine( str2wstr(pStrArr) ) )
	{
		DXUTSetCallbackDeviceCreated( &QEngine::OnCreateDevice );
		DXUTSetCallbackDeviceReset( &QEngine::OnResetDevice );
		DXUTSetCallbackDeviceLost( &QEngine::OnLostDevice );
		DXUTSetCallbackDeviceDestroyed( &QEngine::OnDestroyDevice );
		DXUTSetCallbackMsgProc( &QEngine::MsgProc );
		DXUTSetCallbackKeyboard( &QEngine::KeyboardProc );
		DXUTSetCallbackMouse( &QEngine::MouseProc, true );
		DXUTSetCallbackFrameRender( &QEngine::OnFrameRender );
		DXUTSetCallbackFrameMove( &QEngine::OnFrameMove );

		m_pEngine->Init();

		//------------------------------------------------------------------------------------------
	    
		bool bGotMsg;
		HWND hWnd = DXUTGetHWND();
		MSG  msg;
		msg.message = WM_NULL;
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

		while( WM_QUIT != msg.message && !m_pEngine->bExitFlag )
		{
			// Use PeekMessage() so we can use idle time to render the scene. 
			bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

			if( bGotMsg )
			{
				// Translate and dispatch the message
				if( hWnd == NULL || 0 == TranslateAccelerator( hWnd, NULL, &msg ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
			else
			{
				// Render a frame during idle time (no messages are waiting)
				DXUTRender3DEnvironment();
			}
		}

		//------------------------------------------------------------------------------------------
		m_pEngine->Kill();

		DXUTShutdown();
	}

	SAFE_DELETE(m_pEngine);

	int exit_code = DXUTGetExitCode();

	if ( exit_code > 0 )
		g_Log.WriteError( L"game exit with code = \"" + int2wstr(exit_code) + L"\"" );

	SAFE_DELETE(m_pLog);

    return exit_code;
}