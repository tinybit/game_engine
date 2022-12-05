#pragma once

#include <vector>

using namespace std;

class CFramesCropToolDlg : public CDialog
{
	DECLARE_DYNAMIC(CFramesCropToolDlg)

	int			m_iFramesCount;
	vector<int>	ranges;

	public:
		CFramesCropToolDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CFramesCropToolDlg();

		void SetFramesCount( int iValue ) { m_iFramesCount = iValue; };
		void SetRangesDel( vector<int>* pVector ) { ranges_del = pVector; };
		void UpdateUI();

		enum { IDD = IDD_CROP_FRAMES_TOOL };

		vector<int>* ranges_del;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddRange();
	afx_msg void OnBnClickedDelRange();
	afx_msg void OnBnClickedCrop();
};
