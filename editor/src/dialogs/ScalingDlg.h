#pragma once

#include "afxwin.h"

#define IDD_SCALING 158

class CScalingDlg : public CDialog
{
	DECLARE_DYNAMIC(CScalingDlg)

public:
	CScalingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScalingDlg();
  void SetHeight(float height);

// Dialog Data
	enum { IDD = IDD_SCALING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	CWnd *g_pParent;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnCancel();

private:
	float m_curr_height;
public:
	afx_msg void OnBnClickedBtnOk();
	float m_curr_val;
	CEdit m_edit_curr;
	float GetNewHeight(void);
	CEdit m_new_height;
	float m_fNewHeight;
	bool m_bOK;
};
