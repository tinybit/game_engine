#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "ValidateDlg.h"


CValidateDlg::CValidateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CValidateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CValidateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  if (Create(CValidateDlg::IDD, pParent))
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


void CValidateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CValidateDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_pbProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CValidateDlg, CDialog)
	//{{AFX_MSG_MAP(CValidateDlg)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CValidateDlg message handlers

int CValidateDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CValidateDlg::Step()
{
	CProgressCtrl* progr =  (CProgressCtrl*)GetDlgItem( IDC_PROGRESS );
	progr->StepIt();
}

void CValidateDlg::SetPos( int iPos )
{ 
	CProgressCtrl* progr =  (CProgressCtrl*)GetDlgItem( IDC_PROGRESS );
	progr->SetPos( iPos ); 
}

void CValidateDlg::SetProgressRange(int iMin, int iMax)
{
	CProgressCtrl* progr =  (CProgressCtrl*)GetDlgItem( IDC_PROGRESS );
	progr->SetRange(iMin, iMax);
	progr->SetPos(0);
	progr->SetStep(1);
}

void CValidateDlg::OnTimer(UINT nIDEvent) 
{
	m_pbProgress.StepIt();
	this->UpdateWindow();
 	
	CDialog::OnTimer(nIDEvent);
}

void CValidateDlg::OnClose() 
{
	KillTimer(timer);	
	CDialog::OnClose();
}

void CValidateDlg::SetMessageStr(CString message)
{
	GetDlgItem(IDC_MESSAGE)->SetWindowText(message);
	UpdateWindow();
}

void CValidateDlg::SetProgressMessageStrA(CString message)
{
	GetDlgItem(IDC_MESSAGE_PROGRESS)->SetWindowText(message);
	UpdateWindow();
}

void CValidateDlg::SetProgressMessageStrB(CString message)
{
	GetDlgItem(IDC_MESSAGE_PROGRESS2)->SetWindowText(message);
	UpdateWindow();
}

void CValidateDlg::SetProgressMessageStrC(CString message)
{
	GetDlgItem(IDC_MESSAGE_PROGRESS3)->SetWindowText(message);
	UpdateWindow();
}

void CValidateDlg::SetProgressMessageStrD(CString message)
{
	GetDlgItem(IDC_MESSAGE_PROGRESS4)->SetWindowText(message);
	UpdateWindow();
}

void CValidateDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
}
