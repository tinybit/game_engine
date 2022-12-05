#pragma once

#include "../utils/Common.h"
#include "afxwin.h"

class CBindDlg : public CDialog
{
	DECLARE_DYNAMIC(CBindDlg)

	CString			m_strCaption;
	CString			m_strItem2Edit;
	StrVec*			m_pSceneData;
	StrVec*			m_pSceneTypes;
	ListStrVec*	m_pBinds;
	CComboBox	m_cmbParentID;
	CComboBox	m_cmbParentType;
    CListBox		m_lstChildID;
	CComboBox	m_cmbChildType;
	CListBox		m_lstLog;
	int					m_iEditedItemID;
	CWnd*			m_pParent;
	HACCEL		m_hAccelerator;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		CBindDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CBindDlg();

		enum { IDD = IDD_BIND };


		afx_msg void OnBnClickedBtnCancel();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedBtnAddAnother();
		afx_msg void OnBnClickedChkPar1();
		afx_msg void OnLbnDblclkLstChildId();

		virtual BOOL OnInitDialog();
		void	LinkSceneData( StrVec* pSceneData )	{ m_pSceneData = pSceneData; };
		void	LinkSceneTypes( StrVec* pSceneTypes ){ m_pSceneTypes = pSceneTypes; };
		void	SetCaption( CString strCaption )				{ m_strCaption = strCaption; };
		void	LinkBinds( ListStrVec* binds )					{ m_pBinds = binds; };
		bool	IsAlreadyOnTheList( StrVec* vec );
		void	SetItem2Edit( CString	strItem2Edit ) { m_strItem2Edit = strItem2Edit; };
		bool	AddNewBind();
		void	SetAccel( HACCEL hAccel ) { m_hAccelerator = hAccel; };
		void	SaveAll();
		void	SetParent( CWnd* pParent ) { m_pParent = pParent; };
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		afx_msg void OnCbnSelchangeCmbChildType();
};
