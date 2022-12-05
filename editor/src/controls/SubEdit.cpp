#include "../sys/stdafx.h"
#include "SubEdit.h"

BEGIN_MESSAGE_MAP(CSubEdit, CEdit)
	//{{AFX_MSG_MAP(CSubEdit)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CSubEdit::OnWindowPosChanging( WINDOWPOS FAR* lpwndpos ) 
{
	lpwndpos->x = m_x;

	CEdit::OnWindowPosChanging( lpwndpos );	
}

void CSubEdit::OnKillFocus( CWnd* pNewWnd )
{
	CEdit::OnKillFocus( pNewWnd );	
	m_pParent->SendMessage( WM_SIZE, 0, 0 );
}
