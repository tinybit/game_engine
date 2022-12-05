#pragma once


// CPositionRotationScaleDlg dialog

class CPositionRotationScaleDlg : public CDialog
{
	DECLARE_DYNAMIC(CPositionRotationScaleDlg)

	public:
		CPositionRotationScaleDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CPositionRotationScaleDlg();

	// Dialog Data
		enum { IDD = IDD_POSITION_ROTATION_SCALE };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnBnClickedOk();
		afx_msg void OnBnClickedCancel();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

		float m_fPosX;
		float m_fPosY;
		float m_fPosZ;

		float m_fRotX;
		float m_fRotY;
		float m_fRotZ;

		float m_fScaleX;
		float m_fScaleY;
		float m_fScaleZ;
};
