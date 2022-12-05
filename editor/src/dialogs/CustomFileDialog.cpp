// CustomFileDialog.cpp : implementation file
//

#include "..\sys\stdafx.h"
#include "..\sys\CookieMachine.h"
#include "CustomFileDialog.h"
#include ".\customfiledialog.h"
#include "CookieMachineDlg.h"


// CCustomFileDialog


IMPLEMENT_DYNAMIC(CCustomFileDialog, CFileDialog)
CCustomFileDialog::CCustomFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

CCustomFileDialog::~CCustomFileDialog()
{
}


BEGIN_MESSAGE_MAP(CCustomFileDialog, CFileDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
	ON_WM_TIMER()
	ON_COMMAND(IDC_BTN_MODELS,Models)
	ON_COMMAND(IDC_BTN_SCENES,Scenes)
	ON_BN_CLICKED(IDC_BTN_A, OnBnClickedBtnA)
	ON_BN_CLICKED(IDC_BTN_B, OnBnClickedBtnB)
END_MESSAGE_MAP()



// CCustomFileDialog message handlers
void CCustomFileDialog::Models()
{
	CWnd*	pMainWnd		= ((CCookieMachineApp*)AfxGetApp())->m_pMainWnd;
	CString EngineFolder	= ((CCookieMachineDlg*)pMainWnd)->GetAppSettingsGlob()->EngineFolder;

	CWnd* pWnd = GetParent()->GetDlgItem( 0x047C );

	CEdit* cb = (CEdit*)pWnd;

	if( cb )
	{
		if( PathFileExists( (EngineFolder + "\\data\\models").GetBuffer() ) )
		{
			cb->SetWindowText( (EngineFolder + "\\data\\models").GetBuffer() );
			cb->SendMessage( WM_COMMAND, IDOK, 0 );
			cb->SetWindowText( "" );
		}
		else
		{
			MessageBox( "Models folder was not found!\r\n\r\nPlease make sure that your engine directory is set correctly!", "Warning!", MB_OK|MB_ICONEXCLAMATION );
		}
	}
}

void CCustomFileDialog::Scenes()
{
	CWnd*	pMainWnd		= ((CCookieMachineApp*)AfxGetApp())->m_pMainWnd;
	CString EngineFolder	= ((CCookieMachineDlg*)pMainWnd)->GetAppSettingsGlob()->EngineFolder;

	CWnd* pWnd = GetParent()->GetDlgItem( 0x047C );

	CEdit* cb = (CEdit*)pWnd;

	if( cb )
	{
		if( PathFileExists( (EngineFolder + "\\data\\models\\scenes").GetBuffer() ) )
		{
			cb->SetWindowText( (EngineFolder + "\\data\\models\\scenes").GetBuffer() );
			cb->SendMessage( WM_COMMAND, IDOK, 0 );
			cb->SetWindowText( "" );
		}
		else
		{
			MessageBox( "Scenes folder was not found!\r\n\r\nPlease make sure that your engine directory is set correctly!", "Warning!", MB_OK|MB_ICONEXCLAMATION );
		}
	}
}

BOOL CCustomFileDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	CWnd*	pMainWnd		= ((CCookieMachineApp*)AfxGetApp())->m_pMainWnd;
	
	CWnd*	pBtnA			= GetDlgItem( IDC_BTN_A );
	CWnd*	pBtnB			= GetDlgItem( IDC_BTN_B );

	CString strTemp = ((CCookieMachineDlg*)pMainWnd)->GetAppSettingsGlob()->Button_A_Capt;
	strTemp.Trim();

	if( strTemp != "" )
		pBtnA->SetWindowText( strTemp );
	else
		pBtnA->ShowWindow( false );


	strTemp = ((CCookieMachineDlg*)pMainWnd)->GetAppSettingsGlob()->Button_B_Capt;
	strTemp.Trim();

	if( strTemp != "" )
		pBtnB->SetWindowText( strTemp );
	else
		pBtnB->ShowWindow( false );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCustomFileDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
//	SetTimer( 888, 1000, NULL );
	CFileDialog::OnShowWindow(bShow, nStatus);
}

void CCustomFileDialog::OnInitDone()
{
	//CWnd* pWndTB = GetParent()->GetDlgItem( 0x04A0 );
	//CToolBarCtrl* __tb = (CToolBarCtrl*)pWndTB;

	//while( __tb->GetButtonCount() > 0 )
	//{
	//	__tb->DeleteButton( 0 );
	//}

	
	CFileDialog::OnInitDone();
}

void CCustomFileDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFileDialog::OnActivate(nState, pWndOther, bMinimized);

	//CWnd* pWndTB = GetParent()->GetDlgItem( 0x04A0 );
	//CToolBarCtrl* __tb = (CToolBarCtrl*)pWndTB;

	//while( __tb->GetButtonCount() > 0 )
	//{ 
	//	__tb->DeleteButton( 0 );
	//}
}


void CCustomFileDialog::OnTimer(UINT nIDEvent)
{
	//if( nIDEvent == 889 )
	//{
	//	CWnd* pWnd = GetParent()->GetDlgItem( 0x047C );

	//	CEdit* cb = (CEdit*)pWnd;

	//	if( cb )
	//	{
	//		CWnd* pWndTB = GetParent()->GetDlgItem( 0x04A0 );
	//		CToolBarCtrl* __tb = (CToolBarCtrl*)pWndTB;
	//		__tb->SetOwner( pOwner );
	//		cb->SendMessage( WM_COMMAND, IDOK, 0 );
	//		cb->SetWindowText( "" );
	//	}

	//	KillTimer(889);
	//}

	//if( nIDEvent == 888 )
	//{
	//	KillTimer(888);

	//	CWnd* pWndTB = GetParent()->GetDlgItem( 0x04A0 );
	//	CToolBarCtrl* __tb = (CToolBarCtrl*)pWndTB;

	//	/*while( __tb->GetButtonCount() > 0 )
	//	{
	//		__tb->DeleteButton( 0 );
	//	}*/

	//	TBBUTTON btns;

	//	btns.fsState = TBSTATE_ENABLED;
	//	btns.fsStyle = TBSTYLE_BUTTON;
	//	btns.iBitmap = 10;
	//	btns.idCommand = 666;
	//	btns.iString	= 0;

	//	//btns[1].fsState = TBSTATE_ENABLED;
	//	//btns[1].fsStyle = TBSTYLE_BUTTON;
	//	//btns[1].iBitmap = 10;
	//	//btns[1].idCommand = 667;
	//	//btns[1].iString	= 1;

	//	__tb->InsertButton( 0, &btns );
	//	//__tb->AddStrings( "Scenes\0Models\0" );
	//	pOwner = __tb->GetOwner();
	//	__tb->SetOwner( this );
	//}

	CFileDialog::OnTimer( nIDEvent );
}

LRESULT CCustomFileDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	//if( message == WM_COMMAND )
	//{
	//	int u = 0;
	//}

	return CFileDialog::WindowProc(message, wParam, lParam);
}

void CCustomFileDialog::OnBnClickedBtnA()
{
	CWnd*	pMainWnd	= ((CCookieMachineApp*)AfxGetApp())->m_pMainWnd;
	CString path		= ((CCookieMachineDlg*)pMainWnd)->GetAppSettingsGlob()->Button_A_Path;

	CWnd* pWnd = GetParent()->GetDlgItem( 0x047C );

	CEdit* cb = (CEdit*)pWnd;

	if( cb )
	{
		if( PathFileExists( path.GetBuffer() ) )
		{
			cb->SetWindowText( path.GetBuffer() );
			cb->SendMessage( WM_COMMAND, IDOK, 0 );
			cb->SetWindowText( "" );
		}
	}
}

void CCustomFileDialog::OnBnClickedBtnB()
{
	CWnd*	pMainWnd	= ((CCookieMachineApp*)AfxGetApp())->m_pMainWnd;
	CString path		= ((CCookieMachineDlg*)pMainWnd)->GetAppSettingsGlob()->Button_B_Path;

	CWnd* pWnd = GetParent()->GetDlgItem( 0x047C );

	CEdit* cb = (CEdit*)pWnd;

	if( cb )
	{
		if( PathFileExists( path.GetBuffer() ) )
		{
			cb->SetWindowText( path.GetBuffer() );
			cb->SendMessage( WM_COMMAND, IDOK, 0 );
			cb->SetWindowText( "" );
		}
	}
}
