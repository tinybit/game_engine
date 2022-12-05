#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "AnimationDlg.h"
#include ".\animationdlg.h"



IMPLEMENT_DYNAMIC(CAnimationDlg, CDialog)
CAnimationDlg::CAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationDlg::IDD, pParent)
	, m_iStart(0)
	, m_iEnd(0)
	, m_strName("")
	, m_bIsTransition(false)
{
	g_pParent    = pParent;
	m_bOkPressed = false;
}

CAnimationDlg::~CAnimationDlg()
{
	g_pParent = NULL;
}

void CAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ANIMATION_START, m_iStart);
	DDX_Text(pDX, IDC_ANIMATION_END, m_iEnd);
	DDX_Text(pDX, IDC_ANIMATION_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CAnimationDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedBtnOk)
END_MESSAGE_MAP()


// CAnimationDlg message handlers

void CAnimationDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	UpdateData(false);

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


	CButton* btnTransition = (CButton*)GetDlgItem( IDC_CHK_TRANSITION );

	if( m_bIsTransition )
		btnTransition->SetCheck( 1 );

	this->MoveWindow(wnd_res, true);

	m_bOkPressed = false;
}

void CAnimationDlg::OnBnClickedBtnOk()
{
	UpdateData(true);

	if( m_iStart < 0 || m_iEnd < 0 || m_iStart > m_iMax || m_iEnd > m_iMax )
	{
		MessageBox("Кадр за пределами анимации!", "Внимание!", MB_OK | MB_ICONHAND);
		return;
	}

	if( m_iStart > m_iEnd )
	{
		MessageBox("Начальный кадр не может быть меньше конечного!", "Внимание!", MB_OK | MB_ICONHAND);		
		return;	
	}

	m_strName = m_strName.Trim();

	if( m_strName == "" )
	{
		MessageBox("Задайте имя анимации!", "Внимание!", MB_OK | MB_ICONHAND);		
		return;	
	}

	UpdateData(false);

	CButton* btnTransition = (CButton*)GetDlgItem( IDC_CHK_TRANSITION );

	if( btnTransition->GetCheck() == 0 )
		m_bIsTransition = false;
	else
		m_bIsTransition = true;

	m_bOkPressed = true;

	OnOK();
}

BOOL CAnimationDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->wParam )
	{
		case VK_RETURN:
			OnBnClickedBtnOk();	
			return false;
			break;

		case VK_F1:
			return false;
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
