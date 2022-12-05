#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "ScalingDlg.h"



IMPLEMENT_DYNAMIC(CScalingDlg, CDialog)
CScalingDlg::CScalingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScalingDlg::IDD, pParent)
	, m_fNewHeight(0)
	, m_bOK(false)
{
	g_pParent = pParent;
	m_bOK = false;
}

CScalingDlg::~CScalingDlg()
{
	g_pParent = NULL;
}

void CScalingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CURR_HEIGHT, m_edit_curr);
	DDX_Control(pDX, IDC_EDIT_NEW_HEIGHT, m_new_height);
}

BEGIN_MESSAGE_MAP(CScalingDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_BTN_CANCEL, OnBnClickedBtnCancel)
	ON_BN_CLICKED(ID_BTN_OK, OnBnClickedBtnOk)
END_MESSAGE_MAP()

void CScalingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CString tmp;
	tmp.Format("%f", m_curr_height);
	m_edit_curr.SetWindowText(tmp);
	m_new_height.SetWindowText(tmp);

	CRect parent_wnd_rect, wnd_rect, wnd_res;

  //move validation window to the center of the modal window
  this->GetWindowRect(&wnd_rect);
  g_pParent->GetWindowRect(&parent_wnd_rect);

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

void CScalingDlg::OnBnClickedBtnCancel()
{
  OnOK();
}

void CScalingDlg::SetHeight(float height)
{
  m_curr_height = height;
}

void CScalingDlg::OnBnClickedBtnOk()
{
	CString tmp;
	m_new_height.GetWindowText(tmp);
	m_fNewHeight = (float)atof(tmp.GetBuffer(0));

	m_bOK = true;

	OnOK();
}

float CScalingDlg::GetNewHeight(void)
{
	return m_fNewHeight;
}
