#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "../EfnCharacter.h"
#include "../utils/Common.h"
#include "../sys/resource.h"

//extra controls
#include "../controls/SplitterControl.h"
#include "../controls/RichEditEx.h"



class CAnimScriptingDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnimScriptingDlg)
	
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	public:
		CAnimScriptingDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CAnimScriptingDlg();

		enum { IDD = IDD_ANIM_SCRIPTING };

		DECLARE_MESSAGE_MAP()
		virtual	BOOL OnInitDialog();
		afx_msg void OnBnClickedBtnClose();
		afx_msg void OnLvnItemchangedLstFrames(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnClose();

	public:
		void SetEfn( CEfnCharacter* pEfnAnimation );

	private:
		CListCtrl		m_lstFrames;
		CImageList		m_ImageList;
		RichEditEx		m_rchScriptSrc;
		CEfnCharacter*	m_pEfnAnimation;
public:
	afx_msg void OnMenuXSIMeshDisconnectAllTris();
};
