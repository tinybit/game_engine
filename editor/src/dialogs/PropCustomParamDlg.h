#pragma once

#include "BaseDialog.h"
#include "../utils/Common.h"

class CPropCustomParamDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CPropCustomParamDlg)
	int		m_iFrameIndex;

	public:
		CPropCustomParamDlg( CWnd* pParent = NULL );   // standard constructor
		virtual ~CPropCustomParamDlg();

	// Dialog Data
		enum { IDD = IDD_PROP_CUSTOMPARAM };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		void SetCustomParam( CCUSTPARAM* pParam ) { m_pParam = pParam; };
		void Populate( int iFrameIndex = -1 );
		void Populate( int iFrameIndex, int iStartFrame, int iEndFrame ) { Populate( iFrameIndex ); };
		void ReleaseLinkedData();

		CCUSTPARAM* m_pParam;
		CWnd* m_pID;
		CWnd* m_pValue;
		virtual BOOL OnInitDialog();
};
