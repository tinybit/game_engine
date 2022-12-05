#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "BaseDialog.h"


IMPLEMENT_DYNAMIC(CBaseDialog, CDialog)
CBaseDialog::CBaseDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent),
	m_iFrameIndex(0),
	m_iStartFrame(-1),
	m_iEndFrame(-1)
{
	m_hAccelerator = NULL;
}

CBaseDialog::~CBaseDialog()
{
}

void CBaseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CBaseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_brush.CreateSolidBrush( RGB( 171, 168, 166 ) );
	m_brWhite.CreateSolidBrush( RGB( 255, 255, 255 ) );

	return TRUE;
}

HBRUSH CBaseDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetTextColor( RGB(0, 0, 0) );
	pDC->SetBkMode( TRANSPARENT );

	hbr = m_brush;

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CBaseDialog::PreTranslateMessage(MSG* pMsg)
{
	if( IsEdited() == false )
	{
		if( m_hAccelerator && TranslateAccelerator( this->GetSafeHwnd(), m_hAccelerator, pMsg ) ) 
			return true;
	}

	switch( pMsg->wParam )
	{
		case VK_ESCAPE:
			return false;
			break;

		case VK_RETURN:
			return false;
			break;

		case VK_F1:
			return false;
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CBaseDialog::OnDestroy()
{
	CDialog::OnDestroy();

	m_brush.DeleteObject();
}
