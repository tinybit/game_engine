// BinkInfoDlg.cpp : implementation file
//

#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "BinkInfoDlg.h"
#include ".\binkinfodlg.h"


// CBinkInfoDlg dialog

IMPLEMENT_DYNAMIC(CBinkInfoDlg, CDialog)
CBinkInfoDlg::CBinkInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBinkInfoDlg::IDD, pParent)
{
}

CBinkInfoDlg::~CBinkInfoDlg()
{
}

void CBinkInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBinkInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, OnBnClickedBtnPreview)
END_MESSAGE_MAP()


// CBinkInfoDlg message handlers

BOOL CBinkInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetWindowText( "Bink info" );

	SBINKINF pInf;

	if( GetBinkInfo( file.GetBuffer(), &pInf ) == false )
		OnOK();

	CString	strTemp;
	CEdit*	pWidth	= (CEdit*)GetDlgItem( IDC_EDIT_WIDTH );
	CEdit*	pHeight = (CEdit*)GetDlgItem( IDC_EDIT_HEIGHT );
	CEdit*	pFPS	= (CEdit*)GetDlgItem( IDC_EDIT_FRAMERATE );
	CEdit*	pFrames	= (CEdit*)GetDlgItem( IDC_EDIT_FRAMESCOUNT );
	CEdit*	pAlpha	= (CEdit*)GetDlgItem( IDC_EDIT_ALPHA_PRESENT );


	CEdit*	pResWidth	= (CEdit*)GetDlgItem( IDC_EDIT_RES_WIDTH );
	CEdit*	pResHeight	= (CEdit*)GetDlgItem( IDC_EDIT_RES_HEIGHT );
	CEdit*	pPixCount	= (CEdit*)GetDlgItem( IDC_EDIT_RES_PIXCOUNT );


	strTemp.Format( "%d", pInf.iWidth );
	pWidth->SetWindowText( strTemp );

	strTemp.Format( "%d", pInf.iHeight );
	pHeight->SetWindowText( strTemp );

	strTemp.Format( "%.4f", pInf.fFrameRate );
	pFPS->SetWindowText( strTemp );

	strTemp.Format( "%d", pInf.iFramesCount );
	pFrames->SetWindowText( strTemp );

	if( pInf.bHaveAlpha )
		pAlpha->SetWindowText( "YES" );
	else
		pAlpha->SetWindowText( "NO" );

	strTemp.Format( "%d", pInf.iWidthPow2 );
	pResWidth->SetWindowText( strTemp );

	strTemp.Format( "%d", pInf.iHeightPow2 );
	pResHeight->SetWindowText( strTemp );

	strTemp.Format( "%d", pInf.iPixelCount );
	pPixCount->SetWindowText( strTemp );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBinkInfoDlg::OnBnClickedBtnClose()
{
	OnOK();
}

void CBinkInfoDlg::OnBnClickedBtnPreview()
{
	WinExec( appPath + "\\utils\\binkplay.exe " + file + " /L /Q /B2 /J", 1 );
}
