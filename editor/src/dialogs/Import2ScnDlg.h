#pragma once

#include "../XSIFile.h"
#include "../scenefile.h"
#include "../efnmeshlist.h"
#include "../utils/common.h"

#include "ReplaceWrnDlg.h"
#include "afxwin.h"

class CImport2ScnDlg : public CDialog
{
	DECLARE_DYNAMIC(CImport2ScnDlg)
	CBaseScene*	m_pImportData;
	CBaseScene*	m_pSceneData;

	public:
		CImport2ScnDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CImport2ScnDlg();

	// Dialog Data
		enum { IDD = IDD_IMPORT_2SCN };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnBnClickedBtnCancel();
		afx_msg void OnBnClickedBtnOk();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedBtnAdd();
		afx_msg void OnLbnDblclkLstImportData();
		afx_msg void OnBnClickedBtnRemove();
		afx_msg void OnLbnDblclkLstChosenData();
		afx_msg void OnBnClickedBtnAddAll();
		afx_msg void OnBnClickedBtnRemoveAll();

		void		SetImportFile( CBaseScene*	pImportData )	{ m_pImportData = pImportData; };
		void		SetSceneFile( CBaseScene*	pSceneFile )	{ m_pSceneData = pSceneFile; };
		void		SetTargetFileType( enum FILE_TYPE TargetType ) { m_TargetType = TargetType; }; 
		void		DoImportFromSCN2SCN();
		void		DoImportFromXSI2SCN();

		CString		GetListItemText( CListBox* m_lstBox, int iIndex );
		CListBox	m_lstImportData;
		CListBox	m_lstChosenData;

		enum FILE_TYPE m_TargetType;
};
