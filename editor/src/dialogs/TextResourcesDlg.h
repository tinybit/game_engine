#pragma once

#include "../resource.h"

#include "ModelNameDlg.h"

//extra controls
#include "../controls/SplitterControl.h"
#include "../controls/ScriptLexer.h"
#include "../controls/ListViewCtrlEx.h"


class CTextResourcesDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextResourcesDlg)

	CSplitterControl	m_wndSplitter;
	CScriptLexer		m_lexer;
	CImageList			m_ImageList;
	CListCtrlEx			m_lstObjects;
	CBaseScene*			m_pScene;
	CModelNameDlg*		m_dlgVal;
	str_t				m_strInitialItem;
	int					m_iErrLine;

	CWnd*	m_pParent;
	int		m_iMinWindowSizeX;
	int		m_iMinWindowSizeY;
	int		m_nID;
	str_t	m_iOldItemID;
	HACCEL	m_hAccelerator;
	bool	m_bFirstTime;

	void	UpdateUISizing();
	void	DeleteSelectedItems();
	int		FindItemByName( str_t ID );
	void	RunEngine();
	void	RunEngineCurr();
	void	ShowScriptsEdWnd();
	void	SaveFile();
	void	UpdateCurrentResource();
	void	DeselectAllResItems();
	void	SetCurrResource( str_t strID = "", int iErrLine = -1 ) { m_strInitialItem = strID; m_iErrLine = iErrLine; };
	void	DeleteKeyPressed();

	static int	CALLBACK CompareCallback( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

	enum SORT_BY { ID = 1, COMMENTS };
	SORT_BY	m_sortedBy;

	public:
		CTextResourcesDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CTextResourcesDlg();

		BOOL	Create();
		void	DoResize( int delta );
		void	SetScene( CBaseScene* pScene ) { m_pScene = pScene; };
		void	Init();	
		void	Kill();

		// Dialog Data
		enum { IDD = IDD_TEXT_RESOURCES };

	protected:
		virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
		virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnBnClickedBtnClose();
		virtual BOOL OnInitDialog();
		virtual BOOL DestroyWindow();
		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual void OnOK();
		afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
		afx_msg void OnSize( UINT nType, int cx, int cy );
		afx_msg void OnLvnItemchangedLstObject(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnLvnColumnclickLstObject(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnLvnEndlabeleditLstObject(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMDblclkLstObject(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnBnClickedBtnDel();
		afx_msg void OnBnClickedBtnAdd();
		afx_msg void OnNMClickLstObject(NMHDR *pNMHDR, LRESULT *pResult);
};
