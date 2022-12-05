#define STRICT

#include "QSubSysInput.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QEngine.h"

#include "QSubSysGraphics.h"

QSubSysInput::QSubSysInput()
{
	m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

	m_iDblClickInterval			= GetDoubleClickTime();

	m_fLastTimeMouseOver		= g_Engine.GetTime();
	m_fMouseOverTickTime		= 0.1f;

	iLeftClickTimerID			= 0;
	iRightClickTimerID			= 0;

	iLeftClickTime				= 0;
	iRightClickTime				= 0;

	iLeftClickCounter			= 0;
	iRightClickCounter			= 0;

    ZeroMemory( &m_stNewKeybState, sizeof(m_stNewKeybState) );
	ZeroMemory( &m_stNewMouseState, sizeof(m_stNewMouseState) );
    ZeroMemory( &keyb, sizeof(KeybInput) );
	ZeroMemory( &mouse, sizeof(MouseInput) );

	g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", -1);
	g_StatesMgr.SetState( L"sys", L"sys.mouseLeftDown", 0);
	
	g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", -1);
	g_StatesMgr.SetState( L"sys", L"sys.mouseRightDown", 0);

	g_StatesMgr.SetState( L"sys", L"sys.mouseOverTick", -1);
	
	g_StatesMgr.SetState( L"sys", L"sys.keybKeyCode", -1);
}

QSubSysInput::~QSubSysInput()
{
	ReleaseInput();
}

//-----------------------------------------------------------------------------
// Name: InitInput()
// Desc: Initialize DirectInput objects
//-----------------------------------------------------------------------------
void QSubSysInput::InitInput()
{
    // Create a IDirectInput8*
    if( FAILED( DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
	{
        return;
	}
    
    // Create a IDirectInputDevice8* for the keyboard
    if( FAILED( m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
	{
        return;
	}
    
    // Set the keyboard data format
    if( FAILED( m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
	{
        return;
	}
    
    // Set the cooperative level on the keyboard
    if( FAILED( m_pKeyboard->SetCooperativeLevel( DXUTGetHWND(), 
                                            DISCL_NONEXCLUSIVE | 
                                            DISCL_FOREGROUND ) ) )
	{
        return;
	}

    // Acquire the keyboard
    m_pKeyboard->Acquire();
}

//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void QSubSysInput::UpdateInput()
{
	if ( GetEnabled() == false )
		return;

	// Get the input's device state, and put the state in dims
    ZeroMemory( &m_stNewKeybState.diks, sizeof(m_stNewKeybState.diks) );

	if( FAILED(m_pKeyboard->GetDeviceState( sizeof(m_stNewKeybState.diks), &m_stNewKeybState.diks )) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

	m_stNewKeybState.bEscape	= ( (m_stNewKeybState.diks[DIK_ESCAPE] & 0x80)  == 0x80 );
	m_stNewKeybState.bEnter		= ( (m_stNewKeybState.diks[DIK_RETURN] & 0x80)  == 0x80 );
	m_stNewKeybState.bBackspace	= ( (m_stNewKeybState.diks[DIK_BACKSPACE] & 0x80)  == 0x80 );	
	m_stNewKeybState.bSubtract	= ( (m_stNewKeybState.diks[DIK_SUBTRACT] & 0x80)  == 0x80 );
	m_stNewKeybState.bAdd		= ( (m_stNewKeybState.diks[DIK_ADD] & 0x80)  == 0x80 );	
    m_stNewKeybState.bLeft		= ( (m_stNewKeybState.diks[DIK_LEFT] & 0x80)  == 0x80 );
	m_stNewKeybState.bLBracket	= ( (m_stNewKeybState.diks[DIK_LBRACKET] & 0x80)  == 0x80 );	
	m_stNewKeybState.bRBracket	= ( (m_stNewKeybState.diks[DIK_RBRACKET] & 0x80)  == 0x80 );
	m_stNewKeybState.bBackSlash	= ( (m_stNewKeybState.diks[DIK_BACKSLASH] & 0x80)  == 0x80 );	
	m_stNewKeybState.bLeftCtrl	= ( (m_stNewKeybState.diks[DIK_LCONTROL] & 0x80)  == 0x80 );
	m_stNewKeybState.bRightCtrl	= ( (m_stNewKeybState.diks[DIK_RCONTROL] & 0x80)  == 0x80 );
	m_stNewKeybState.bLeftAlt	= ( (m_stNewKeybState.diks[DIK_LALT] & 0x80)  == 0x80 );
	m_stNewKeybState.bRightAlt	= ( (m_stNewKeybState.diks[DIK_RALT] & 0x80)  == 0x80 );
	m_stNewKeybState.bLeftShift	= ( (m_stNewKeybState.diks[DIK_LSHIFT] & 0x80)  == 0x80 );
	m_stNewKeybState.bRightShift= ( (m_stNewKeybState.diks[DIK_RSHIFT] & 0x80)  == 0x80 );
	m_stNewKeybState.bTab		= ( (m_stNewKeybState.diks[DIK_TAB] & 0x80)  == 0x80 );
	m_stNewKeybState.bSpace		= ( (m_stNewKeybState.diks[DIK_SPACE] & 0x80)  == 0x80 );
    m_stNewKeybState.bRight		= ( (m_stNewKeybState.diks[DIK_RIGHT] & 0x80) == 0x80 );
    m_stNewKeybState.bUp		= ( (m_stNewKeybState.diks[DIK_UP] & 0x80)    == 0x80 );
    m_stNewKeybState.bDown		= ( (m_stNewKeybState.diks[DIK_DOWN] & 0x80)  == 0x80 );
	m_stNewKeybState.bDown		= ( (m_stNewKeybState.diks[DIK_DOWN] & 0x80)  == 0x80 );
	m_stNewKeybState.bDown		= ( (m_stNewKeybState.diks[DIK_DOWN] & 0x80)  == 0x80 );
	m_stNewKeybState.b0			= ( (m_stNewKeybState.diks[DIK_0] & 0x80)  == 0x80 );
	m_stNewKeybState.b1			= ( (m_stNewKeybState.diks[DIK_1] & 0x80)  == 0x80 );
	m_stNewKeybState.b2			= ( (m_stNewKeybState.diks[DIK_2] & 0x80)  == 0x80 );
	m_stNewKeybState.b3			= ( (m_stNewKeybState.diks[DIK_3] & 0x80)  == 0x80 );
	m_stNewKeybState.b4			= ( (m_stNewKeybState.diks[DIK_4] & 0x80)  == 0x80 );
	m_stNewKeybState.b5			= ( (m_stNewKeybState.diks[DIK_5] & 0x80)  == 0x80 );
	m_stNewKeybState.b6			= ( (m_stNewKeybState.diks[DIK_6] & 0x80)  == 0x80 );
	m_stNewKeybState.b7			= ( (m_stNewKeybState.diks[DIK_7] & 0x80)  == 0x80 );
	m_stNewKeybState.b8			= ( (m_stNewKeybState.diks[DIK_8] & 0x80)  == 0x80 );
	m_stNewKeybState.b9			= ( (m_stNewKeybState.diks[DIK_9] & 0x80)  == 0x80 );
	m_stNewKeybState.bA			= ( (m_stNewKeybState.diks[DIK_A] & 0x80)  == 0x80 );
	m_stNewKeybState.bB			= ( (m_stNewKeybState.diks[DIK_B] & 0x80)  == 0x80 );
	m_stNewKeybState.bC			= ( (m_stNewKeybState.diks[DIK_C] & 0x80)  == 0x80 );
	m_stNewKeybState.bD			= ( (m_stNewKeybState.diks[DIK_D] & 0x80)  == 0x80 );
	m_stNewKeybState.bE			= ( (m_stNewKeybState.diks[DIK_E] & 0x80)  == 0x80 );
	m_stNewKeybState.bF			= ( (m_stNewKeybState.diks[DIK_F] & 0x80)  == 0x80 );
	m_stNewKeybState.bG			= ( (m_stNewKeybState.diks[DIK_G] & 0x80)  == 0x80 );
	m_stNewKeybState.bH			= ( (m_stNewKeybState.diks[DIK_H] & 0x80)  == 0x80 );
	m_stNewKeybState.bI			= ( (m_stNewKeybState.diks[DIK_I] & 0x80)  == 0x80 );
	m_stNewKeybState.bJ			= ( (m_stNewKeybState.diks[DIK_J] & 0x80)  == 0x80 );
	m_stNewKeybState.bK			= ( (m_stNewKeybState.diks[DIK_K] & 0x80)  == 0x80 );
	m_stNewKeybState.bL			= ( (m_stNewKeybState.diks[DIK_L] & 0x80)  == 0x80 );
	m_stNewKeybState.bM			= ( (m_stNewKeybState.diks[DIK_M] & 0x80)  == 0x80 );
	m_stNewKeybState.bN			= ( (m_stNewKeybState.diks[DIK_N] & 0x80)  == 0x80 );
	m_stNewKeybState.bO			= ( (m_stNewKeybState.diks[DIK_O] & 0x80)  == 0x80 );
	m_stNewKeybState.bP			= ( (m_stNewKeybState.diks[DIK_P] & 0x80)  == 0x80 );
	m_stNewKeybState.bQ			= ( (m_stNewKeybState.diks[DIK_Q] & 0x80)  == 0x80 );
	m_stNewKeybState.bR			= ( (m_stNewKeybState.diks[DIK_R] & 0x80)  == 0x80 );
	m_stNewKeybState.bS			= ( (m_stNewKeybState.diks[DIK_S] & 0x80)  == 0x80 );
	m_stNewKeybState.bT			= ( (m_stNewKeybState.diks[DIK_T] & 0x80)  == 0x80 );
	m_stNewKeybState.bU			= ( (m_stNewKeybState.diks[DIK_U] & 0x80)  == 0x80 );
	m_stNewKeybState.bV			= ( (m_stNewKeybState.diks[DIK_V] & 0x80)  == 0x80 );
	m_stNewKeybState.bW			= ( (m_stNewKeybState.diks[DIK_W] & 0x80)  == 0x80 );
	m_stNewKeybState.bX			= ( (m_stNewKeybState.diks[DIK_X] & 0x80)  == 0x80 );
	m_stNewKeybState.bY			= ( (m_stNewKeybState.diks[DIK_Y] & 0x80)  == 0x80 );
	m_stNewKeybState.bZ			= ( (m_stNewKeybState.diks[DIK_Z] & 0x80)  == 0x80 );

	keyb = m_stNewKeybState;

	if ( g_Engine.GetTime() - m_fLastTimeMouseOver > m_fMouseOverTickTime )
	{
		m_fLastTimeMouseOver = g_Engine.GetTime();

		g_StatesMgr.SetState( L"sys", L"sys.mouseOverTick", 1);
		g_StatesMgr.SetState( L"sys", L"sys.mouseOverTick", -1);	
	}
}

void QSubSysInput::UpdateMouse()
{
	tagPOINT temp;
	if ( GetCursorPos( &temp ) )
	{
		if ( ScreenToClient( DXUTGetHWND(), &temp ) )
		{
			MouseProc( mouse.LeftDown, mouse.RightDown, mouse.MidlDown, false, false, 0, temp.x, temp.y );
		}
	}
}

void QSubSysInput::KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  )
{
	if ( GetEnabled() == false )
		return;

	if(bKeyDown)
		g_StatesMgr.SetState( L"sys", L"sys.keybKeyCode", nChar);
	else
		g_StatesMgr.SetState( L"sys", L"sys.keybKeyCode", 1000+nChar);
	g_StatesMgr.SetState( L"sys", L"sys.keybKeyCode", -1);
}

void QSubSysInput::MouseProc(	bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
								bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos)
{
	if ( !g_Graph.GetFlagShowCursor() )
		SetCursor(NULL);

	if ( GetEnabled() == false )
		return;

	m_stNewMouseState.LeftDown	= bLeftButtonDown;
	m_stNewMouseState.RightDown	= bRightButtonDown;
	m_stNewMouseState.MidlDown	= bMiddleButtonDown;

	m_stNewMouseState.x = xPos;
	m_stNewMouseState.y = yPos;
	m_stNewMouseState.fx = (float)xPos;
	m_stNewMouseState.fy = (float)yPos;
	
	//-----------------left-----------------------
	g_StatesMgr.SetState( L"sys", L"sys.mouseLeftDown", ( m_stNewMouseState.LeftDown ? 1 : 0 ) );

	if ( (mouse.LeftDown != 1) && (m_stNewMouseState.LeftDown == 1) )
	{
		if ( iLeftClickCounter==0 )
		{
			iLeftClickTime = ttime();
			iLeftClickTimerID = SetTimer( NULL, 0, m_iDblClickInterval, OnClickTimerProc );
			iLeftClickCounter++;
			m_stNewMouseState.left_x = xPos;
			m_stNewMouseState.left_y = yPos;
			g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", 1);
			g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", -1);
		}
		else if ( ttime() - iLeftClickTime < m_iDblClickInterval )
		{
			KillTimer( NULL, iLeftClickTimerID );
			iLeftClickCounter = 0;
			iLeftClickTimerID = 0;

			g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", 3);
			g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", -1);
		}
	}
	else if ( (mouse.LeftDown == 1) && (m_stNewMouseState.LeftDown!=1) )
	{
		g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", 0);
		g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", -1);	
	}
	//---------------right---------------------------------------
	g_StatesMgr.SetState( L"sys", L"sys.mouseRightDown", ( m_stNewMouseState.RightDown ? 1 : 0 ));

	if ( (mouse.RightDown != 1) && (m_stNewMouseState.RightDown == 1) )
	{
		if ( iRightClickCounter==0 )
		{
			iRightClickTime = ttime();
			iRightClickTimerID = SetTimer( NULL, 0, m_iDblClickInterval, OnClickTimerProc );
			iRightClickCounter++;
			m_stNewMouseState.right_x = xPos;
			m_stNewMouseState.right_y = yPos;
			g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", 1);
			g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", -1);
		}
		else if ( ttime() - iRightClickTime < m_iDblClickInterval )
		{
			iRightClickCounter = 0;
			KillTimer( NULL, iRightClickTimerID );
			iRightClickTimerID = 0;

			g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", 3);
			g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", -1);	
		}
	}
	else if ( (mouse.RightDown == 1) && (m_stNewMouseState.RightDown!=1) )
	{
		g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", 0);
		g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", -1);	
	}

	if ( g_Graph.GetDebug() )
	{
		if ( (mouse.x!=m_stNewMouseState.x) || (mouse.y!=m_stNewMouseState.y) )
		{
			g_StatesMgr.SetStateV2( L"sys", L"sys.mouse", &D3DXVECTOR2( m_stNewMouseState.fx, m_stNewMouseState.fy ) );	
		}
	}

	mouse = m_stNewMouseState;
}

void CALLBACK OnClickTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	if ( g_Input.iLeftClickTimerID==idEvent )
	{
		KillTimer( NULL, g_Input.iLeftClickTimerID );
		g_Input.iLeftClickTimerID = 0;

		g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", 2);
		g_StatesMgr.SetState( L"sys", L"sys.mouseLeftClick", -1);	
		g_Input.iLeftClickCounter = 0;
	}
	else if ( g_Input.iRightClickTimerID==idEvent )
	{
		KillTimer( NULL, g_Input.iRightClickTimerID );
		g_Input.iRightClickTimerID = 0;

		g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", 2);
		g_StatesMgr.SetState( L"sys", L"sys.mouseRightClick", -1);	
		g_Input.iRightClickCounter = 0;
	}
}


void QSubSysInput::ReleaseInput()
{
	if ( iLeftClickTimerID!=0 )
	{
		KillTimer( NULL, iLeftClickTimerID );
		iLeftClickTimerID = 0;
	}

	if ( iRightClickTimerID!=0 )
	{
		KillTimer( NULL, iRightClickTimerID );
		iRightClickTimerID = 0;
	}

   	SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );
}

void QSubSysInput::SetEnabled( bool bEnabled )
{
	if ( bEnabled == false )
	{
		g_Input.iLeftClickCounter = 0;
		g_Input.iRightClickCounter = 0;
		if ( g_Input.iLeftClickTimerID != 0 )
			KillTimer( NULL, g_Input.iLeftClickTimerID );
		if ( g_Input.iRightClickTimerID != 0 )
		KillTimer( NULL, g_Input.iRightClickTimerID );
	}
	g_StatesMgr.SetStateB( L"sys", L"sys.inputEnabled", bEnabled );
}