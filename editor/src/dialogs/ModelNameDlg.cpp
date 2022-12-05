// ModelName.cpp : implementation file

#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "ModelNameDlg.h"

IMPLEMENT_DYNAMIC(CModelNameDlg, CDialog)
CModelNameDlg::CModelNameDlg(CWnd* pParent) : CDialog(CModelNameDlg::IDD, pParent), m_string(""), m_iWidth(-1)
{
	m_pParent = pParent;
}

CModelNameDlg::~CModelNameDlg()
{
	m_pParent = NULL;
}

void CModelNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_string);
}


BEGIN_MESSAGE_MAP(CModelNameDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedClose)
	ON_EN_CHANGE(IDC_EDIT, OnEnChangeEdit)
END_MESSAGE_MAP()


// CModelNameDlg message handlers

void CModelNameDlg::OnClose()
{
	CDialog::OnClose();
}

void CModelNameDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CModelNameDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CRect parent_wnd_rect, wnd_rect, wnd_res, wnd_res2;

	CDialog::OnShowWindow(bShow, nStatus);

	this->SetWindowText( m_caption );

	UpdateData(false);


	if( m_iWidth != -1 )
	{
		this->GetWindowRect( &wnd_rect );

		if( wnd_rect.Width() < m_iWidth )
			wnd_rect.right = wnd_rect.left + m_iWidth;

		this->MoveWindow( wnd_rect, false );

		CRect rect;
		GetClientRect( rect );


		CWnd* pButt = GetDlgItem( IDOK );

		pButt->GetWindowRect( &wnd_res );
		ScreenToClient( &wnd_res );

		wnd_res.left	= rect.right - 1 - wnd_res.Width();
		wnd_res.right	= rect.right - 1;

		pButt->MoveWindow( wnd_res, false );



		CWnd* pEdit = GetDlgItem( IDC_EDIT );
		
		pEdit->GetWindowRect( &wnd_res2 );
		ScreenToClient( &wnd_res2 );

		wnd_res2.right	= rect.right - 5 - wnd_res.Width();
		pEdit->MoveWindow( wnd_res2, false );
	}
	

	//move validation window to the center of the modal window
	this->GetWindowRect(&wnd_rect);
	m_pParent->GetWindowRect(&parent_wnd_rect);

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

CString CModelNameDlg::GetString(void)
{
	return m_string;
}

void CModelNameDlg::OnBnClickedClose()
{
	OnOK();
}

void CModelNameDlg::OnEnChangeEdit()
{
	UpdateData(true);
}

void CModelNameDlg::SetCaption(CString strCaption)
{
	m_caption = strCaption;
}

void CModelNameDlg::SetString(CString strName)
{
	m_string = strName;
}

BOOL CModelNameDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
		case WM_KEYDOWN:

			if( pMsg->wParam == VK_ESCAPE )
				return false;
			break;

		case WM_KEYUP:

			if( pMsg->wParam == VK_ESCAPE )
			{
				OnBnClickedClose();
				return true;
			}
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
