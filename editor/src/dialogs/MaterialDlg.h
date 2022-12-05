#pragma once
#include "../controls/colorbtn.h"
#include "../utils/common.h"

class CMaterialDlg : public CDialog
{
	DECLARE_DYNAMIC(CMaterialDlg)

	CButton* btnLock;

	public:
		CMaterialDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CMaterialDlg();

	// Dialog Data
		enum { IDD = IDD_MATERIAL };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnBnClickedBtnOk();
		afx_msg void OnBnClickedBtnCancel();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

		D3DMATERIAL9 m_Material;
		CColorButton cbAmbColor;
		CColorButton cbEmmColor;
		CColorButton cbDiffColor;
		CColorButton cbSpecColor;
		afx_msg void OnBnClickedBtnAmbColor();
		afx_msg void OnBnClickedBtnEmmColor();
		afx_msg void OnBnClickedBtnDiffColor();
		afx_msg void OnBnClickedBtnSpecColor();
		virtual BOOL OnInitDialog();
};
