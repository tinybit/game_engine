#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "../utils/common.h"
#include "AboutBoxDlg.h"




IMPLEMENT_DYNAMIC(CAboutBoxDlg, CDialog)
CAboutBoxDlg::CAboutBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutBoxDlg::IDD, pParent)
{
	if (Create(CAboutBoxDlg::IDD, pParent))
	{
		ShowWindow(SW_SHOW);

		CRect parent_wnd_rect, wnd_rect, wnd_res;

		//move validation window to the center of the modal window
		this->GetWindowRect(&wnd_rect);
		pParent->GetWindowRect(&parent_wnd_rect);

		wnd_res.left = parent_wnd_rect.left + 
                   (parent_wnd_rect.right - parent_wnd_rect.left) / 2 - 
                   (wnd_rect.right - wnd_rect.left) / 2;
  
		wnd_res.top = parent_wnd_rect.top + 
                   (parent_wnd_rect.bottom - parent_wnd_rect.top) / 2 - 
                   (wnd_rect.bottom - wnd_rect.top) / 2;

    wnd_res.right = wnd_res.left + (wnd_rect.right - wnd_rect.left);
     
    wnd_res.bottom = wnd_res.top + (wnd_rect.bottom - wnd_rect.top);

    this->MoveWindow(wnd_res, true);
  }
}

CAboutBoxDlg::~CAboutBoxDlg()
{
}

void CAboutBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutBoxDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_STN_CLICKED(IDC_BUILD, OnStnClickedBuild)
END_MESSAGE_MAP()


// CAboutBoxDlg message handlers

void CAboutBoxDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CEdit* version_edit = (CEdit*)GetDlgItem( IDC_VERSION ); 
	CEdit* build_edit	= (CEdit*)GetDlgItem( IDC_BUILD );

	CString strEx;

	#if defined(DEBUG) | defined(_DEBUG)
		strEx = " (debug)";
	#endif

	version_edit->SetWindowText( "version: " + CString( EDITOR_VERSION ) + strEx );
	build_edit->SetWindowText( "build: " + CString( EDITOR_BUILD ) + ", " + CString( EDITOR_DATA ) );
}

void CAboutBoxDlg::OnStnClickedBuild()
{
	// TODO: Add your control notification handler code here
}
