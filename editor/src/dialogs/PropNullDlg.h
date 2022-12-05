#pragma once

#include "BaseDialog.h"
#include "../utils/Common.h"

class CPropNullDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CPropNullDlg)

	public:
		CPropNullDlg(CWnd* pParent = NULL);
		virtual ~CPropNullDlg();

		enum { IDD = IDD_PROP_NULL };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);

		DECLARE_MESSAGE_MAP()
	public:
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		virtual BOOL PreTranslateMessage(MSG* pMsg);

		void ReleaseLinkedData();
		void SetNull( CNULL2* pNull2 )	{ m_pNull2 = pNull2; };
		void SetNull( CNULL* pNull )	{ m_pNull = pNull; };
		void Populate( int iFrameIndex = -1, int iStartFrame = -1, int iEndFrame = -1 );
		void UpdateDataFromControl();
		void UpdateDataFromControl( UINT id );
		bool IsEdited();
		bool IsEditedBox( int id );

		CNULL2* m_pNull2;
		CNULL*	m_pNull;

		CWnd*	m_pID;
		CWnd*	m_pScaleX;
		CWnd*	m_pScaleY;
		CWnd*	m_pScaleZ;
		CWnd*	m_pRotX;
		CWnd*	m_pRotY;
		CWnd*	m_pRotZ;
		CWnd*	m_pTranslX;
		CWnd*	m_pTranslY;
		CWnd*	m_pTranslZ;

		virtual BOOL OnInitDialog();
		afx_msg void OnEnKillfocusScaleX();
		afx_msg void OnEnKillfocusScaleY();
		afx_msg void OnEnKillfocusScaleZ();
		afx_msg void OnEnKillfocusRotX();
		afx_msg void OnEnKillfocusRotY();
		afx_msg void OnEnKillfocusRotZ();
		afx_msg void OnEnKillfocusTransX();
		afx_msg void OnEnKillfocusTransY();
		afx_msg void OnEnKillfocusTransZ();
		afx_msg void OnEnSetfocusScaleX();
		afx_msg void OnEnSetfocusScaleY();
		afx_msg void OnEnSetfocusScaleZ();
		afx_msg void OnEnSetfocusRotX();
		afx_msg void OnEnSetfocusRotY();
		afx_msg void OnEnSetfocusRotZ();
		afx_msg void OnEnSetfocusTransX();
		afx_msg void OnEnSetfocusTransY();
		afx_msg void OnEnSetfocusTransZ();
};
