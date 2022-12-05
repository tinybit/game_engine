#pragma once

class CBaseDialog : public CDialog
{
	DECLARE_DYNAMIC(CBaseDialog)

	public:
		CBaseDialog( UINT nIDTemplate, CWnd* pParent = NULL );   // standard constructor
		virtual ~CBaseDialog();
		virtual BOOL OnInitDialog();
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		void SetAccelerator( HACCEL	hAccelerator ) { m_hAccelerator = hAccelerator; };
		virtual void Populate( int iFrameIndex, int iStartFrame, int iEndFrame ) {};
		virtual void Populate( int iFrameIndex )	{ Populate( iFrameIndex, -1, -1 ); };
		virtual void Populate()						{ Populate( -1 ); };
		virtual void ReleaseLinkedData() {};
		virtual bool IsEdited() { return false; };
		afx_msg void OnDestroy();
		

		enum { IDD = IDD_BASEDIALOG };

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

		virtual void DoDataExchange(CDataExchange* pDX);

		CBrush	m_brush;
		CBrush	m_brWhite;

		int m_iFrameIndex;
		int m_iStartFrame;
		int m_iEndFrame;

		HACCEL	m_hAccelerator;
};
