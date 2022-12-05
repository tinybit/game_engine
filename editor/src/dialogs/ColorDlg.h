#pragma once

#include "../controls/colorbtn.h"
#include "../utils/common.h"

class CColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorDlg)

	public:
		CColorDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CColorDlg();

		// Dialog Data
		enum { IDD = IDD_COLOR };

		CString caption;
		bool	alpha_enabled;

		CColorButton cbColor;

		CCOLOR color;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnBnClickedBtnClose();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedBtnColor();
		int m_iAlpha;
		virtual BOOL PreTranslateMessage(MSG* pMsg);
};
