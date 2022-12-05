#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "ReplaceWrnDlg.h"



IMPLEMENT_DYNAMIC(CReplaceWrnDlg, CDialog)
CReplaceWrnDlg::CReplaceWrnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReplaceWrnDlg::IDD, pParent)
{
	m_iRetCode = -1;
}

CReplaceWrnDlg::~CReplaceWrnDlg()
{
}

void CReplaceWrnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CReplaceWrnDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_REPLACE, OnBnClickedBtnReplace)
	ON_BN_CLICKED(IDC_BTN_REPLACE_ALL, OnBnClickedBtnReplaceAll)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CReplaceWrnDlg message handlers
void CReplaceWrnDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CWnd* pWnd = GetDlgItem( IDC_WARNING_MSG );
	pWnd->SetWindowText( m_strMessage );
}

void CReplaceWrnDlg::OnBnClickedBtnReplace()
{
	m_iRetCode = IDOK;
	OnOK();
}

void CReplaceWrnDlg::OnBnClickedBtnReplaceAll()
{
	m_iRetCode = IDRETRY;
	OnOK();
}

void CReplaceWrnDlg::OnBnClickedBtnCancel()
{
	m_iRetCode = IDCANCEL;
	OnOK();
}
