#pragma once


class CAnimDegrageToolDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnimDegrageToolDlg)

public:
	CAnimDegrageToolDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnimDegrageToolDlg();

// Dialog Data
	enum { IDD = IDD_ANIMATION_DEGRADE_TOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
