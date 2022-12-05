#include "../sys/stdafx.h"

#include "RichEditEx.h"
#include "../dialogs/ScriptingDlg.h"



BEGIN_MESSAGE_MAP(RichEditEx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRulerRichEdit)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

UINT RichEditEx::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;	
}

void RichEditEx::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	//if( nChar == VK_ESCAPE )

	//	m_pParentWnd->OnOK();

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void RichEditEx::OnRButtonUp(UINT nFlags, CPoint point)
{
	//((CScriptingDlg*)m_pParentWnd)->RButtonUp( point );

	CRichEditCtrl::OnRButtonUp(nFlags, point);
}

void RichEditEx::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void RichEditEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRichEditCtrl::OnLButtonUp(nFlags, point);
}
