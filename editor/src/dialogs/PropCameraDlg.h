#pragma once

#include "BaseDialog.h"
#include "../utils/Common.h"

class CPropCameraDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CPropCameraDlg)

	CCAMERA2* m_pCamera;
	
	CWnd* m_pParent;

	CWnd* m_pID;
	CWnd* m_pPosX;
	CWnd* m_pPosY;
	CWnd* m_pPosZ;
	CWnd* m_pDirX;
	CWnd* m_pDirY;
	CWnd* m_pDirZ;
	CWnd* m_pUpvecX;
	CWnd* m_pUpvecY;
	CWnd* m_pUpvecZ;
	CWnd* m_pFovV;
	CWnd* m_pFovH;
	CWnd* m_pZoomNear;
	CWnd* m_pZoomFar;
	CWnd* m_pClipNear;
	CWnd* m_pClipFar;

	public:
		CPropCameraDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CPropCameraDlg();

		void SetCamera( CCAMERA2* pCamera ) { m_pCamera = pCamera; Reset(); };
		void ReleaseLinkedData() { /*m_pCamera = NULL;*/ };
		void Populate( int iFrameIndex = -1 );
		void Reset();


		enum { IDD = IDD_PROP_CAMERA };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		virtual BOOL OnInitDialog();
		afx_msg void OnBnClickedChkZoomNear();
		afx_msg void OnBnClickedChkZoomFar();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnDestroy();
};
