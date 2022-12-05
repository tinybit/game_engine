#pragma once

#include "afxcmn.h"
#include "BindDlg.h"
#include "afxwin.h"

#include "../utils/Common.h"

class CBindsDlg : public CDialog
{
	DECLARE_DYNAMIC(CBindsDlg)

	public:
		CBindsDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CBindsDlg();

		void LinkBinds( ListStrVec* binds )			{ m_pBinds		= binds; };
		void LinkFuncs( IFuncMap* funcs )			{ m_pFuncs		= funcs; };
		void LinkSceneData( StrVec* pSceneData )	{ m_pSceneData	= pSceneData; };
		void LinkSceneTypes( StrVec* pTypes )		{ m_pSceneTypes	= pTypes; };
		void SetParent( CWnd* pParent )				{ m_pParent		= pParent; };
		void UpdateGrid();
		void RemoveBindFromTheList( int iListViewIndex );
		void SetSelection( StrVec* pSelItem )		{ m_pSelItem	= pSelItem; };
		void SetAccel( HACCEL hAccel )				{ m_hAccelerator = hAccel; };
		void SaveAll();
		bool ChangesMade()							{ return m_bChangesMade; }; 

		enum { IDD = IDD_BINDS };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual	BOOL OnInitDialog();
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		afx_msg void OnBnClickedBtnClose();
		afx_msg void OnBnClickedBtnAdd();
		afx_msg void OnBnClickedBtnRemove();
		afx_msg void OnBnClickedBtnEdit();
		afx_msg void OnCbnSelchangeCmbFilterParType();
		afx_msg void OnCbnSelchangeCmbFilterParId();
		afx_msg void OnHdnItemdblclickListBinds(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMDblclkListBinds(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnBnClickedBtnUp();
		afx_msg void OnBnClickedBtnDown();
		DECLARE_MESSAGE_MAP()

	private:
		CListCtrl	m_lstBinds;
		CComboBox	m_cmbParentType;
		CComboBox	m_cmbParentID;

		ListStrVec*	m_pBinds;
		IFuncMap*	m_pFuncs;
		StrVec*		m_pSceneData;
		StrVec*		m_pSceneTypes;
		StrVec*		m_pSelItem;
		HACCEL		m_hAccelerator;
		CWnd*		m_pParent;
		bool		m_bChangesMade;
};
