#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../utils/common.h"


class CFileAssociationsDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileAssociationsDlg)

	public:
		CFileAssociationsDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CFileAssociationsDlg();

		enum { IDD = IDD_FILE_ASSOCIATIONS };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		afx_msg void OnBnClickedOk();

		DECLARE_MESSAGE_MAP()

	public:
		void		SetAppPath( CString strPath );
		CTreeCtrl	m_tree;
		HTREEITEM	xsi, efn, scn;
		CString		m_strappPath;
};
