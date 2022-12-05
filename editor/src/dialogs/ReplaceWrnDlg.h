#pragma once


// CReplaceWrnDlg dialog

class CReplaceWrnDlg : public CDialog
{
	DECLARE_DYNAMIC(CReplaceWrnDlg)

	CString m_strMessage;
	int m_iRetCode;

public:
	CReplaceWrnDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReplaceWrnDlg();
	void	SetWarningMessage( CString strMessage ) { m_strMessage = strMessage; };
	int		GetRetCode() { return m_iRetCode; };

// Dialog Data
	enum { IDD = IDD_REPLACE_WRN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnReplace();
	afx_msg void OnBnClickedBtnReplaceAll();
	afx_msg void OnBnClickedBtnCancel();
};
