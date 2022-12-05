#pragma once

class RichEditEx : public CRichEditCtrl
{
	public:
		RichEditEx()				{ m_pParentWnd = NULL; };
		virtual ~RichEditEx()	{};

		void SetParent( CWnd* pWnd ) { m_pParentWnd = pWnd; };

	protected:
		//{{AFX_MSG( RichEditEx )
		afx_msg UINT OnGetDlgCode();
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	private:
		CWnd* m_pParentWnd;
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

