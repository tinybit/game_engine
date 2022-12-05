// CAnimationDlg dialog

#pragma once

#define IDD_ANIMATION 155

class CAnimationDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnimationDlg)

	public:
		CAnimationDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CAnimationDlg();

		enum { IDD = IDD_ANIMATION };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		DECLARE_MESSAGE_MAP()
		CWnd *g_pParent;

	public:
		int		m_iStart;
		int		m_iEnd;
		int		m_iMax;
		bool	m_bOkPressed;
		bool	m_bIsTransition;

		CString	m_strName;
		afx_msg void OnBnClickedBtnOk();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
