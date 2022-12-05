// CAboutBoxDlg dialog

#pragma once

#include "resource.h"

class CAboutBoxDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutBoxDlg)

	public:
		CAboutBoxDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CAboutBoxDlg();

	// Dialog Data
		enum { IDD = IDD_ABOUT };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnStnClickedBuild();
};
