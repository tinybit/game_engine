#pragma once
#include "../utils/Common.h"
#include "../sys/resource.h"

enum SEARCH_MODE
{
	SS_SEARCH = 1,
	SS_SEARCH_REPLACE = 2
};

class CScriptSearchDlg : public CDialog
{
	DECLARE_DYNAMIC(CScriptSearchDlg)

	enum SEARCH_MODE	m_searchMode;
	ListStrVec*			m_pBinds;
	CString				m_strReplaceStr;
	str_t				m_strCurrSelObj;
	vector<int>			m_SearchResults;

	void SearchScripts();
	void SearchAndReplaceScripts();

	public:
		CScriptSearchDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CScriptSearchDlg();

		enum { IDD = IDD_SCRIPT_SEARCH };

		void		SetSearchMode( enum SEARCH_MODE searchMode ) { m_searchMode = searchMode; };
		void		SetBinds( ListStrVec* pBinds ) { m_pBinds = pBinds; };
		void		SetCurrSelObject( str_t strCurSelObj ) { m_strCurrSelObj = strCurSelObj; };
		vector<int> GetSearchResults() { return m_SearchResults; };
		CString		m_strSearchStr;
		bool		m_bCurrBind;
		bool		m_bCaseSens;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedOk();
		afx_msg void OnBnClickedCancel();

		DECLARE_MESSAGE_MAP()
};
