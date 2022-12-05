#pragma once

#include "BaseDialog.h"
#include "../utils/Common.h"
#include "../controls/colorbtn.h"
#include "afxcmn.h"

class CPropLightDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CPropLightDlg)

	CWnd*	m_pParent;

	CWnd*	m_pID;
	CWnd*	m_pType;

	CWnd*	m_pPosX;
	CWnd*	m_pPosY;
	CWnd*	m_pPosZ;
	CWnd*	m_pTargX;
	CWnd*	m_pTargY;
	CWnd*	m_pTargZ;
	CWnd*	m_pAttenuation;
	CWnd*	m_pMultiplier;
	CWnd*	m_pRange;
	CWnd*	m_pTheta;
	CWnd*	m_pPhi;

	CButton* m_btnRange;

	CLIGHT* m_pLight;

	CColorButton m_cbAmb;
	CColorButton m_cbDiff;
	CColorButton m_cbSpec;

	public:
		CPropLightDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CPropLightDlg();

		void SetLight( CLIGHT* pLight ) { m_pLight = pLight; Reset(); };
		void ReleaseLinkedData() { /*m_pCamera = NULL;*/ };
		void Populate( int iFrameIndex = -1 );
		void Reset();

		enum { IDD = IDD_PROP_LIGHT };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		CSpinButtonCtrl m_spinAtten;
		CSpinButtonCtrl m_spinMult;
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		virtual BOOL OnInitDialog();
		afx_msg void OnBnClickedChkRange();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedBtnAmb();
		afx_msg void OnBnClickedBtnDiff();
		afx_msg void OnBnClickedBtnSpec();
		afx_msg void OnDeltaposSpinAtten(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnDeltaposSpinMult(NMHDR *pNMHDR, LRESULT *pResult);
};
