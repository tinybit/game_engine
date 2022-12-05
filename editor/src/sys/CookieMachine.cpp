#pragma warning(disable: 4098)

#include "stdafx.h"
#include "../utils/common.h"
#include "CookieMachine.h"
#include "../ExeptionLogger.h"
#include "../dialogs/CookieMachineDlg.h"
#include "../dialogs/HelpDlg.h"

CExeptionLogger exLogger;


BEGIN_MESSAGE_MAP(CCookieMachineApp, CWinApp)
	//{{AFX_MSG_MAP(CCookieMachineApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCookieMachineApp construction

CCookieMachineApp::CCookieMachineApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCookieMachineApp object

CCookieMachineApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCookieMachineApp initialization

BOOL CCookieMachineApp::InitInstance()
{
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.

		#ifdef _AFXDLL
			Enable3dControls();			// Call this when using MFC in a shared DLL
		#else
			Enable3dControlsStatic();	// Call this when linking to MFC statically
		#endif

		AfxEnableControlContainer();

		AfxInitRichEdit();

		CCookieMachineDlg dlg;

		m_pMainWnd = &dlg;
		
		int nResponse = dlg.DoModal();
		
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			// dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			// dismissed with Cancel
		}

		// Since the dialog has been closed, return FALSE so that we exit the
		// application, rather than start the application's message pump.

	/*}
	catch(...)
	{*/
		//g_ExeptLog->__data.push_back( "BOOL CCookieMachineApp::InitInstance()" );

		//MessageBox( NULL, g_ExeptLog->GetExeptionLog().data(), "Critical error, application will exit!", MB_OK | MB_ICONERROR );

		//MSG msg;

		//for( int t = 0; t < 100; t++)
		//{
		//	if( PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE) )
		//	{
 	//			TranslateMessage(&msg);
		//		DispatchMessage(&msg);
		//	}
		//}		
	//}

	return FALSE;
}

//BOOL CCookieMachineApp::OnIdle( LONG )
//{
//  //dlg.Render();
//  //((CCookieMachineDlg *)m_pMainWnd)->Render();
//  // Keep requesting more idle time
//  return TRUE;
//}

void CCookieMachineApp::WinHelp(DWORD dwData, UINT nCmd)
{
	CWinApp::WinHelp(dwData, nCmd);
}
