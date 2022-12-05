#if !defined(VALIDATEDLG_H_)
#define VALIDATEDLG_H_

#define IDD_VALIDATING 131

class CValidateDlg : public CDialog
{
// Construction
public:
	void SetMessageStr(CString message);
	void SetProgressMessageStrA(CString message); 
	void SetProgressMessageStrB(CString message); 
	void SetProgressMessageStrC(CString message); 
	void SetProgressMessageStrD(CString message);
	void SetProgressRange(int iMin, int iMax);

	int timer;
	CValidateDlg(CWnd* pParent = NULL);   // standard constructor
	void Step();
	void SetPos( int iPos );

// Dialog Data
	//{{AFX_DATA(CValidateDlg)
	enum { IDD = IDD_VALIDATING };
	CProgressCtrl	m_pbProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValidateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CValidateDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(VALIDATEDLG_H_)
