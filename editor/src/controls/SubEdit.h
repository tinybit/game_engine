#pragma once


class CSubEdit : public CEdit
{
	public:
		CSubEdit(){};
		virtual ~CSubEdit() {};
		

		int		m_x;
		CWnd*	m_pParent;
		void	SetParent( CWnd* pParent ){ m_pParent = pParent; };

	protected:
		//{{AFX_MSG(CSubEdit)
		afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
		afx_msg void OnKillFocus( CWnd* pNewWnd );
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
};