#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "PropCustomParamDlg.h"

IMPLEMENT_DYNAMIC(CPropCustomParamDlg, CDialog)
CPropCustomParamDlg::CPropCustomParamDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CPropCustomParamDlg::IDD, pParent),
	m_iFrameIndex(0)
{
	m_pParam	= NULL;
}

CPropCustomParamDlg::~CPropCustomParamDlg()
{
}

void CPropCustomParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropCustomParamDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPropCustomParamDlg message handlers

HBRUSH CPropCustomParamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);


	return hbr;
}

BOOL CPropCustomParamDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CBaseDialog::PreTranslateMessage(pMsg);
}

void CPropCustomParamDlg::ReleaseLinkedData()
{
	m_pParam = NULL;
}

void CPropCustomParamDlg::Populate( int iFrameIndex )
{
	if( !m_pParam || ( iFrameIndex != -1 && iFrameIndex < 0 ) )
		return;

	if( iFrameIndex != -1 )
		m_iFrameIndex = iFrameIndex;

	CString strTemp;

	if( iFrameIndex != -1 )
		strTemp.Format( "%0.4f", m_pParam->data[iFrameIndex] );
	else
		strTemp.Format( "%0.4f", m_pParam->data[m_iFrameIndex] );

	m_pID->SetWindowText( m_pParam->id.data() );
	m_pValue->SetWindowText( strTemp );

	UpdateWindow();
}

BOOL CPropCustomParamDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	m_pID		= GetDlgItem( IDC_EDIT_ID );
	m_pValue	= GetDlgItem( IDC_EDIT_VALUE );

	return TRUE;
}
