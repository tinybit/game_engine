#pragma once

#include <string>
using namespace std;

class CEngineMessagingDlg : public CDialog
{
	DECLARE_DYNAMIC(CEngineMessagingDlg)

	virtual void DoDataExchange(CDataExchange* pDX);

	HACCEL	m_hAccelerators;

	public:
		CEngineMessagingDlg(CWnd* pParent = NULL);
		virtual ~CEngineMessagingDlg();

		enum { IDD = IDD_ENGINE_MESSAGING };


	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSend();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
