#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "../utils/common.h"
#include "EngineMessagingDlg.h"
#include "../MemMappingsMgr.h"

IMPLEMENT_DYNAMIC(CEngineMessagingDlg, CDialog)
CEngineMessagingDlg::CEngineMessagingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEngineMessagingDlg::IDD, pParent)
{
}

CEngineMessagingDlg::~CEngineMessagingDlg()
{
}

void CEngineMessagingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEngineMessagingDlg, CDialog)
	ON_BN_CLICKED(IDC_SEND, OnBnClickedSend)
	ON_COMMAND(ID_ACCL_SEND, OnBnClickedSend)
END_MESSAGE_MAP()


BOOL CEngineMessagingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hAccelerators = LoadAccelerators( AfxGetResourceHandle(), MAKEINTRESOURCE( IDR_ACCELERATOR_ENGINE_MSG ) );

	CEdit* edit	= (CEdit*)GetDlgItem( IDC_TEXT );
	edit->LimitText( 65530 );

	return TRUE;
}

void CEngineMessagingDlg::OnBnClickedSend()
{
	CEdit* edit			= (CEdit*)GetDlgItem( IDC_TEXT );
	CEdit* edit_history	= (CEdit*)GetDlgItem( IDC_HISTORY );

	CString test;
	CString history;

	edit->GetWindowText( test );

	//ßיצא װאבונזו
	CWnd* pWnd = FindWindow( NULL, "ßיצא װאבונזו" );

	if( !pWnd )
		AfxMessageBox( "Engine window was not found!" );
	else
	{
		g_MemMappingsMgr->PostScriptToEngine( test );

		edit_history->GetWindowText( history );

		history += "--------------------------------------\r\n";
		history += test + "\r\n";

		edit_history->SetWindowText( history );
	}
}

BOOL CEngineMessagingDlg::PreTranslateMessage(MSG* pMsg)
{
	if( TranslateAccelerator( this->GetSafeHwnd(), m_hAccelerators, pMsg ) )
		return true;

	switch( pMsg->wParam )
	{
		case VK_F1:
			return false;
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
