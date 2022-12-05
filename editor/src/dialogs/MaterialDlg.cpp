// MaterialDlg.cpp : implementation file
//

#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "MaterialDlg.h"
#include ".\materialdlg.h"


// CMaterialDlg dialog

IMPLEMENT_DYNAMIC(CMaterialDlg, CDialog)
CMaterialDlg::CMaterialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMaterialDlg::IDD, pParent)
{
	ZeroMemory( &m_Material, sizeof( m_Material ) );

	m_Material.Ambient.r	= 1.0f;
	m_Material.Ambient.g	= 0.0f;
	m_Material.Ambient.b	= 1.0f;
	m_Material.Ambient.a	= 0.4f;

	m_Material.Emissive.r	= 0.0f;
	m_Material.Emissive.g	= 0.0f;
	m_Material.Emissive.b	= 1.0f;
	m_Material.Emissive.a	= 0.5f;

	m_Material.Diffuse.r	= 0.0f;
	m_Material.Diffuse.g	= 1.0f;
	m_Material.Diffuse.b	= 0.0f;
	m_Material.Diffuse.a	= 0.6f;

	m_Material.Specular.r	= 0.0f;
	m_Material.Specular.g	= 1.0f;
	m_Material.Specular.b	= 1.0f;
	m_Material.Specular.a	= 0.7f;

	m_Material.Power		= 2.3f;
}

CMaterialDlg::~CMaterialDlg()
{
}

void CMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMaterialDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedBtnOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedBtnCancel)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_AMB_COLOR, OnBnClickedBtnAmbColor)
	ON_BN_CLICKED(IDC_BTN_EMM_COLOR, OnBnClickedBtnEmmColor)
	ON_BN_CLICKED(IDC_BTN_DIFF_COLOR, OnBnClickedBtnDiffColor)
	ON_BN_CLICKED(IDC_BTN_SPEC_COLOR, OnBnClickedBtnSpecColor)
END_MESSAGE_MAP()


// CMaterialDlg message handlers

void CMaterialDlg::OnBnClickedBtnOk()
{
	CString temp;
	CWnd* pWndAmb		= GetDlgItem( IDC_EDIT_AMB_ALPHA );
	CWnd* pWndEmm		= GetDlgItem( IDC_EDIT_EMM_ALPHA );
	CWnd* pWndDiff		= GetDlgItem( IDC_EDIT_DIFF_ALPHA );
	CWnd* pWndSpec		= GetDlgItem( IDC_EDIT_SPEC_ALPHA );
	CWnd* pWndPower		= GetDlgItem( IDC_EDIT_POWER );

	pWndAmb->GetWindowText( temp );
	m_Material.Ambient.a = (float)atof( temp.GetBuffer( 0 ) );

	pWndEmm->GetWindowText( temp );
	m_Material.Emissive.a = (float)atof( temp.GetBuffer( 0 ) );
	
	pWndDiff->GetWindowText( temp );
	m_Material.Diffuse.a = (float)atof( temp.GetBuffer( 0 ) );

	pWndSpec->GetWindowText( temp );
	m_Material.Specular.a = (float)atof( temp.GetBuffer( 0 ) );
	
	pWndPower->GetWindowText( temp );
	m_Material.Power = (float)atof( temp.GetBuffer( 0 ) );

	OnOK();
}

void CMaterialDlg::OnBnClickedBtnCancel()
{
	OnCancel();
}

void CMaterialDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	CString temp;

	COLORREF ambCol		= RGB( 255 * m_Material.Ambient.r, 255 * m_Material.Ambient.g, 255 * m_Material.Ambient.b );
	COLORREF emmCol		= RGB( 255 * m_Material.Emissive.r, 255 * m_Material.Emissive.g, 255 * m_Material.Emissive.b );
	COLORREF diffCol	= RGB( 255 * m_Material.Diffuse.r, 255 * m_Material.Diffuse.g, 255 * m_Material.Diffuse.b );
	COLORREF specCol	= RGB( 255 * m_Material.Specular.r, 255 * m_Material.Specular.g, 255 * m_Material.Specular.b );

	CWnd* pWndAmb		= GetDlgItem( IDC_EDIT_AMB_ALPHA );
	CWnd* pWndEmm		= GetDlgItem( IDC_EDIT_EMM_ALPHA );
	CWnd* pWndDiff		= GetDlgItem( IDC_EDIT_DIFF_ALPHA );
	CWnd* pWndSpec		= GetDlgItem( IDC_EDIT_SPEC_ALPHA );
	CWnd* pWndPower		= GetDlgItem( IDC_EDIT_POWER );

	temp.Format( "%.2f", m_Material.Ambient.a );
	pWndAmb->SetWindowText( temp );

	temp.Format( "%.2f", m_Material.Emissive.a );
	pWndEmm->SetWindowText( temp );

	temp.Format( "%.2f", m_Material.Diffuse.a );
	pWndDiff->SetWindowText( temp );

	temp.Format( "%.2f", m_Material.Specular.a );
	pWndSpec->SetWindowText( temp );
	
	temp.Format( "%.2f", m_Material.Power );
	pWndPower->SetWindowText( temp );


	VERIFY( cbAmbColor.Attach( IDC_BTN_AMB_COLOR, this, ambCol, ambCol, ambCol) );
	VERIFY( cbEmmColor.Attach( IDC_BTN_EMM_COLOR, this, emmCol, emmCol, emmCol) );
	VERIFY( cbDiffColor.Attach( IDC_BTN_DIFF_COLOR, this, diffCol, diffCol, diffCol) );
	VERIFY( cbSpecColor.Attach( IDC_BTN_SPEC_COLOR, this, specCol, specCol, specCol) );
}

void CMaterialDlg::OnBnClickedBtnAmbColor()
{
	COLORREF col = RGB( 255 * m_Material.Ambient.r, 255 * m_Material.Ambient.g, 255 * m_Material.Ambient.b );
	CColorDialog dlg( col, CC_FULLOPEN );

	dlg.DoModal();

	col = dlg.GetColor();

	cbAmbColor.SetColor( col );
	cbAmbColor.Invalidate( true );

	m_Material.Ambient.r = (float)GetRValue( col ) / 255;
	m_Material.Ambient.g = (float)GetGValue( col ) / 255;
	m_Material.Ambient.b = (float)GetBValue( col ) / 255;

	if( btnLock->GetCheck() == 1 )
	{
		cbDiffColor.SetColor( col );
		cbDiffColor.Invalidate( true );

		m_Material.Diffuse.r = (float)GetRValue( col ) / 255;
		m_Material.Diffuse.g = (float)GetGValue( col ) / 255;
		m_Material.Diffuse.b = (float)GetBValue( col ) / 255;
	}
}

void CMaterialDlg::OnBnClickedBtnEmmColor()
{
	COLORREF col = RGB( 255 * m_Material.Emissive.r, 255 * m_Material.Emissive.g, 255 * m_Material.Emissive.b );
	CColorDialog dlg( col, CC_FULLOPEN );

	dlg.DoModal();

	col = dlg.GetColor();

	cbEmmColor.SetColor( col );
	cbEmmColor.Invalidate( true );

	m_Material.Emissive.r = (float)GetRValue( col ) / 255;
	m_Material.Emissive.g = (float)GetGValue( col ) / 255;
	m_Material.Emissive.b = (float)GetBValue( col ) / 255;
}

void CMaterialDlg::OnBnClickedBtnDiffColor()
{
	COLORREF col = RGB( 255 * m_Material.Diffuse.r, 255 * m_Material.Diffuse.g, 255 * m_Material.Diffuse.b );
	CColorDialog dlg( col, CC_FULLOPEN );

	dlg.DoModal();

	col = dlg.GetColor();

	cbDiffColor.SetColor( col );
	cbDiffColor.Invalidate( true );

	m_Material.Diffuse.r = (float)GetRValue( col ) / 255;
	m_Material.Diffuse.g = (float)GetGValue( col ) / 255;
	m_Material.Diffuse.b = (float)GetBValue( col ) / 255;

	if( btnLock->GetCheck() == 1 )
	{
		cbAmbColor.SetColor( col );
		cbAmbColor.Invalidate( true );

		m_Material.Ambient.r = (float)GetRValue( col ) / 255;
		m_Material.Ambient.g = (float)GetGValue( col ) / 255;
		m_Material.Ambient.b = (float)GetBValue( col ) / 255;
	}
}

void CMaterialDlg::OnBnClickedBtnSpecColor()
{
	COLORREF col = RGB( 255 * m_Material.Specular.r, 255 * m_Material.Specular.g, 255 * m_Material.Specular.b );
	CColorDialog dlg( col, CC_FULLOPEN );

	dlg.DoModal();

	col = dlg.GetColor();

	cbSpecColor.SetColor( col );
	cbSpecColor.Invalidate( true );

	m_Material.Specular.r = (float)GetRValue( col ) / 255;
	m_Material.Specular.g = (float)GetGValue( col ) / 255;
	m_Material.Specular.b = (float)GetBValue( col ) / 255;
}

BOOL CMaterialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	btnLock = (CButton*)GetDlgItem( IDC_CHK_AMB_DIFF );
	btnLock->SetCheck( true );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
