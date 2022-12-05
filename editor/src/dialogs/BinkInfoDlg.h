#pragma once

#include "../utils/binkinf.h"

class CBinkInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CBinkInfoDlg)

	public:
		CBinkInfoDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CBinkInfoDlg();
		
		CString file;
		CString appPath;

	// Dialog Data
		enum { IDD = IDD_BINKINFO };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	

	DECLARE_MESSAGE_MAP()

	public:
		virtual BOOL OnInitDialog();
		afx_msg void OnBnClickedBtnClose();
		afx_msg void OnBnClickedBtnPreview();
};
