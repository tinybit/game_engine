#pragma once

#include "../utils/Common.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "../sys/resource.h"

//extra controls
#include "../controls/SplitterControl.h"
#include "../controls/ScriptLexer.h"
#include "../controls/BaseTreeCtrl.h"
#include "ScriptSearchDlg.h"

#include <algorithm>
#include <functional>

class CScriptingDlg : public CDialog
{
	DECLARE_DYNAMIC(CScriptingDlg)

	int oldItem;
	int	m_iMinWindowSizeX;
	int	m_iMinWindowSizeY;
	bool m_bIsRefreshingFuncsBindings;

	IFuncMap*	m_pFuncs;
	ListStrVec*	m_pBinds;

	void ShowLastSelObject();

	public:
		CScriptingDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CScriptingDlg();	

		enum { IDD = IDD_SCRIPTING };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		str_t				m_strLastSelObj;
		StrVec				m_pObjects;
		StrVec*				m_pNulls;
		//RichEditEx		m_rchScriptSrc;
		CSplitterControl	m_wndSplitter;
		CSplitterControl	m_wndSplitterH;
		CComboBox			m_cmbFilterParentType;
		CComboBox			m_cmbFilterParentID;
		CComboBox			m_cmbExFuncs;
		CComboBoxEx			m_cmbExClasses;
		int					m_iOldSelection;
		bool				m_bJustOpened;
		HACCEL				m_hAccelerator;
		CWnd*				m_pParent;
		CImageList			m_ImageList;
		str_t				m_strOldClipboard;
		//CListCtrl			m_lstObjects;
		CBaseTreeCtrl		m_tvObjects;
		vector<int>			m_SearchResults;
		int					m_iCurrSearchResult;
		str_t				m_strFilePath;
		CString				m_strSearchString;
		bool				m_bSearchCurrBindOnly;
		bool				m_bSearchCaseSensitive;
		bool				m_bFirstSearch;
		int					m_iCurrentlySearchedBind;
		CString				m_strFilterParType;
		CString				m_strFilterParID;
		CString				m_strFilterParID_full;
		int					m_WndHeight;
		bool				m_bIsDialogClosing;

		CScriptLexer		m_lexer;
		CScriptLexer		m_lexer_out;
		map< int, std::string > scripts_map;
		map< int, std::string > txtres_map;

		void Go2FuncDefinition();
		void SetBinds( ListStrVec*	pBinds )	{ m_pBinds = pBinds; };
		void SetFuncs( IFuncMap*	pFuncs )	{ m_pFuncs = pFuncs; };
		void SetNulls( StrVec* pNulls )		{ m_pNulls = pNulls; };
		void DoResize( int delta );
		void DoResizeH( int delta );
		str_t GetItemScript( str_t strItem );
		void SetItemScript( str_t strItem, str_t strValue );
		void OnCloseDlg();
		void ShowLuaBinds();
		void SaveAll();
		void RunEngine();
		void RunEngineCurr();
		void String2Clipboard( str_t strValue );
		void StorOldClipboard();
		void RestorOldClipboard();
		void RButtonUp( CPoint point );
		void MaximizeWindow();
		void ShowNextSearchResult();
		void UpdateUISizing();
		void CommentSelectedBlock();
		void UncommentSelectedBlock();
		void ShowTextResEditor();
		bool FindInScript( str_t strObject, int& iOffset, int iStartingPos );
		void SelectSearchResult( int iItem, int iOffset );
		void UpdateCurrentScript();
		void ParseScripts();
		void ClearTempFolder();
		void ScrollOutput();
		void JumpToToScript();
		void FillFilterCombos();
		void PopulateObjectsList();
		void PopulateClassesCombo();
		void PopulateFuncsCombo();
		void RefreshFuncsBindings();
		int  GetCurrentBindCounter();
		void UpdateParentChildFilterFromBindCounter( int iBindCounter );
		void ShowCurrentBind( CString strParentID, CString strParentType, CString strChildID, CString strChildType );
		bool FindNextResultInScripts( int iCurrentBindCounter, int iScriptOffset, CString strSearch, int& iResBindCounter, int& iResScriptOffset );
		bool IsValidFuncChar( char chChar );
		CString ExecuteExternalFile( CString csExeName, CString csArguments );
		int	 SearchComboboxEx( CComboBoxEx* pComboBoxEx, CString strSearchString );
		int	 SearchCombobox( CComboBox* pComboBox, CString strSearchString );

		virtual BOOL OnInitDialog();
		virtual BOOL PreTranslateMessage( MSG* pMsg );
		virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
		virtual void OnOK();
		afx_msg void OnSize( UINT nType, int cx, int cy );
		afx_msg void OnBnClickedBtnClose();
		afx_msg void OnLvnItemchangedLstObject( NMHDR *pNMHDR, LRESULT *pResult );
		afx_msg void OnBnClickedBtnCreateHandler();
		afx_msg void OnBnClickedBtnCreateSet();
		afx_msg void OnBnClickedBtnCreateFunction();
		afx_msg void OnBnClickedBtnSearch();
		afx_msg void OnBnClickedBtnSearchAndReplace();
		afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
		afx_msg void OnMarginClick( NMHDR* nmhdr, LRESULT* result );
		afx_msg void OnLexerDataModify( NMHDR* nmhdr, LRESULT* result );
		afx_msg void OnOutputDblClick( NMHDR* nmhdr, LRESULT* result );
		afx_msg void OnBnClickedButton3();
		afx_msg void OnBnClickedButton4();
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnCbnSelchangeCmbFilterType();
		afx_msg void OnCbnSelchangeCmbFilterId();
		afx_msg void OnTvnSelchangedTreeObjects(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnTvnSelchangingTreeObjects(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnDestroy();
		afx_msg void OnCbnSelchangeComboboxexClasses();
		afx_msg void OnCbnSelchangeComboboxexFuncs();
		afx_msg void OnCbnCloseupComboboxexFuncs();
		afx_msg void OnCbnCloseupCmbFilterType();
		afx_msg void OnCbnCloseupCmbFilterId();
		UINT GetFileSize( const string& sFileName );
};
