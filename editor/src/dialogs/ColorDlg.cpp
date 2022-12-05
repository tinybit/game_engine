#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "ColorDlg.h"



IMPLEMENT_DYNAMIC(CColorDlg, CDialog)
CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
	, m_iAlpha(0)
{
}

CColorDlg::~CColorDlg()
{
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ALPHA, m_iAlpha);
	DDV_MinMaxInt(pDX, m_iAlpha, 0, 255);
}


BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_COLOR, OnBnClickedBtnColor)
END_MESSAGE_MAP()


// CColorDlg message handlers

void CColorDlg::OnBnClickedBtnClose()
{
	UpdateData( true );

	color.a = (float)m_iAlpha;

	OnOK();
}

void CColorDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	this->SetWindowText( caption );

	if( !alpha_enabled )
	{
		CEdit* edit_alpha = (CEdit*)GetDlgItem( IDC_EDIT_ALPHA );
		edit_alpha->SetReadOnly( true );
		edit_alpha->EnableWindow( false );
	}
	else
	{
		m_iAlpha = (int)color.a;
		UpdateData( false );
	}

	VERIFY( cbColor.Attach( IDC_BTN_COLOR, this, RGB( color.r, color.g, color.b ), RGB( color.r, color.g, color.b ), RGB( color.r, color.g, color.b )) );
}

void CColorDlg::OnBnClickedBtnColor()
{
	COLORREF col = RGB( color.r , color.g, color.b );
	CColorDialog dlg( col, CC_FULLOPEN );

	dlg.DoModal();

	col = dlg.GetColor();

	cbColor.SetColor( col );
	cbColor.Invalidate( true );

	color.r = GetRValue( col );
	color.g = GetGValue( col );
	color.b = GetBValue( col );
}

BOOL CColorDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->wParam )
	{
		case VK_ESCAPE:
			return false;
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
