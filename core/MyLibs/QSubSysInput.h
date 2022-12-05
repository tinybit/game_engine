#pragma once

#include "dxstdafx.h"
#include <dinput.h>

#include "CommonDefines.h"
#include "QMgrResStates.h"

// Struct to store the current keyboard input state
struct KeybInput
{
    byte diks[256];

	bool bEscape;
	bool bEnter;
	bool bBackspace;
	bool bSubtract;
	bool bAdd;
	bool bLBracket;
	bool bRBracket;
	bool bBackSlash;
	bool bLeftCtrl;
	bool bRightCtrl;
	bool bLeftAlt;
	bool bRightAlt;
	bool bLeftShift;
	bool bRightShift;
	bool bTab;
	bool bSpace;
	bool bUp;
    bool bDown;
    bool bLeft;
    bool bRight;
	bool b0;
	bool b1;
	bool b2;
	bool b3;
	bool b4;
	bool b5;
	bool b6;
	bool b7;
	bool b8;
	bool b9;
	bool bA;
	bool bB;
	bool bC;
	bool bD;
	bool bE;
	bool bF;
	bool bG;
	bool bH;
	bool bI;
	bool bJ;
	bool bK;
	bool bL;
	bool bM;
	bool bN;
	bool bO;
	bool bP;
	bool bQ;
	bool bR;
	bool bS;
	bool bT;
	bool bU;
	bool bV;
	bool bW;
	bool bX;
	bool bY;
	bool bZ;
};

// Struct to store the current mouse input state
struct MouseInput
{
	int x;
	int y;
	float fx;
	float fy;
	int left_x;
	int left_y;
	int right_x;
	int right_y;
	bool LeftDown;
	bool MidlDown;
	bool RightDown;
};

void CALLBACK OnClickTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

#define g_Input QSubSysInput::GetSingleton()
#define g_Keyb g_Input.keyb
#define g_Mouse g_Input.mouse

class QSubSysInput : public Singleton <QSubSysInput>
{
public:

	QSubSysInput();
	~QSubSysInput();

    KeybInput               keyb;
	MouseInput				mouse;

	//LRESULT				MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    void					InitInput();
    void					UpdateInput();
	void					UpdateMouse();
	void					ReleaseInput();

	void					SetEnabled( bool bEnabled );
	bool					GetEnabled() { return g_StatesMgr.GetStateB(L"sys.inputEnabled"); };

	void					SetMouseOverTickTime( float fTime ) { m_fMouseOverTickTime = fTime; };
	
	void					KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  );
	void					MouseProc(	bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
										bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos);

	UINT_PTR				iLeftClickTimerID;
	UINT_PTR				iRightClickTimerID;

	UINT					iLeftClickTime;
	UINT					iRightClickTime;

	UINT					iLeftClickCounter;
	UINT					iRightClickCounter;

private:

	LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device

	MouseInput				m_stNewMouseState;
	KeybInput				m_stNewKeybState;

	double					m_fLastTimeMouseOver;
	float					m_fMouseOverTickTime;

	UINT					m_iDblClickInterval;

};