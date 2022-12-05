// CModelNameDlg dialog
#pragma once

#define IDD_MODEL_NAME 154

class CModelNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CModelNameDlg)

	public:
		CModelNameDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CModelNameDlg();

		CString GetString();
		void	SetString( CString strName );
		void	SetCaption( CString strCaption );
		void	SetWidth( int iWidth ) { m_iWidth = iWidth; };

	private:
		CWnd*	m_pParent;
		CString	m_string;
		CString	m_caption;
		int		m_iWidth;

	// Dialog Data
		enum { IDD = IDD_MODEL_NAME };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		afx_msg void OnClose();
		afx_msg void OnDestroy();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedClose();
		afx_msg void OnEnChangeEdit();

		DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
