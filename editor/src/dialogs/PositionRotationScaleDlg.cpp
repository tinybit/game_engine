// PositionRotationDlg.cpp : implementation file
//

#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "PositionRotationScaleDlg.h"

#pragma warning( disable: 4244 )

// CPositionRotationScaleDlg dialog

IMPLEMENT_DYNAMIC(CPositionRotationScaleDlg, CDialog)
CPositionRotationScaleDlg::CPositionRotationScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPositionRotationScaleDlg::IDD, pParent)
{
}

CPositionRotationScaleDlg::~CPositionRotationScaleDlg()
{
}

void CPositionRotationScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPositionRotationScaleDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CPositionRotationScaleDlg message handlers

void CPositionRotationScaleDlg::OnBnClickedOk()
{
	CString temp;
	CWnd*	pWnd;

	pWnd = GetDlgItem( IDC_EDIT_POS_X );
	pWnd->GetWindowText( temp );
	m_fPosX = atof( temp.GetBuffer(0) );

	pWnd = GetDlgItem( IDC_EDIT_POS_Y );
	pWnd->GetWindowText( temp );
	m_fPosY = atof( temp.GetBuffer(0) );

	pWnd = GetDlgItem( IDC_EDIT_POS_Z );
	pWnd->GetWindowText( temp );
	m_fPosZ = atof( temp.GetBuffer(0) );


	pWnd = GetDlgItem( IDC_EDIT_ROT_X );
	pWnd->GetWindowText( temp );
	m_fRotX = atof( temp.GetBuffer(0) );

	pWnd = GetDlgItem( IDC_EDIT_ROT_Y );
	pWnd->GetWindowText( temp );
	m_fRotY = atof( temp.GetBuffer(0) );

	pWnd = GetDlgItem( IDC_EDIT_ROT_Z );
	pWnd->GetWindowText( temp );
	m_fRotZ = atof( temp.GetBuffer(0) );


	pWnd = GetDlgItem( IDC_EDIT_SCALE_X );
	pWnd->GetWindowText( temp );
	m_fScaleX = atof( temp.GetBuffer(0) );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_Y );
	pWnd->GetWindowText( temp );
	m_fScaleY = atof( temp.GetBuffer(0) );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_Z );
	pWnd->GetWindowText( temp );
	m_fScaleZ = atof( temp.GetBuffer(0) );

	OnOK();
}

void CPositionRotationScaleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CPositionRotationScaleDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CString temp;
	CWnd*	pWnd;

	pWnd = GetDlgItem( IDC_EDIT_POS_X );
	temp.Format( "%.6f", m_fPosX ); 
	pWnd->SetWindowText( temp );

	pWnd = GetDlgItem( IDC_EDIT_POS_Y );
	temp.Format( "%.6f", m_fPosY ); 
	pWnd->SetWindowText( temp );

	pWnd = GetDlgItem( IDC_EDIT_POS_Z );
	temp.Format( "%.6f", m_fPosZ ); 
	pWnd->SetWindowText( temp );


	pWnd = GetDlgItem( IDC_EDIT_ROT_X );
	temp.Format( "%.6f", m_fRotX ); 
	pWnd->SetWindowText( temp );

	pWnd = GetDlgItem( IDC_EDIT_ROT_Y );
	temp.Format( "%.6f", m_fRotY ); 
	pWnd->SetWindowText( temp );

	pWnd = GetDlgItem( IDC_EDIT_ROT_Z );
	temp.Format( "%.6f", m_fRotZ ); 
	pWnd->SetWindowText( temp );


	pWnd = GetDlgItem( IDC_EDIT_SCALE_X );
	temp.Format( "%.6f", m_fScaleX ); 
	pWnd->SetWindowText( temp );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_Y );
	temp.Format( "%.6f", m_fScaleY ); 
	pWnd->SetWindowText( temp );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_Z );
	temp.Format( "%.6f", m_fScaleZ ); 
	pWnd->SetWindowText( temp );
}
