#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "AnimDegrageToolDlg.h"
#include ".\animdegragetooldlg.h"


IMPLEMENT_DYNAMIC(CAnimDegrageToolDlg, CDialog)
CAnimDegrageToolDlg::CAnimDegrageToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimDegrageToolDlg::IDD, pParent)
{
}

CAnimDegrageToolDlg::~CAnimDegrageToolDlg()
{
}

void CAnimDegrageToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAnimDegrageToolDlg, CDialog)
END_MESSAGE_MAP()


// CAnimDegrageToolDlg message handlers

BOOL CAnimDegrageToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_PERCENTAGE );

	pEdit->SetWindowText( "100%" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
