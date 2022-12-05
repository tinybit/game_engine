#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ScalingDlg.h"

#include "../controls/colorbtn.h"
#include "../utils/Common.h"
#include "../sys/Resource.h"

class COptionsDlg : public CDialog
{
	public:
		/* constructors */
		COptionsDlg( CWnd* pParent = NULL );
		~COptionsDlg();

		/* interface */
		void				SetValues(AppSettingsData &appSettings);
		AppSettingsData		GetValues();
		void				UpdateUI();
		bool				CheckForChanges();
		bool				m_bOK;
		bool				m_bSettingsChanged;

	private:
		CWnd*					m_pParent;
		CColorButton			cbAmbLight;
		CColorButton			cbDiffLight;
		CColorButton			cbAmbient;
		CColorButton			cbGridCross;
		CColorButton			cbGridWires;
		CColorButton			cbViewportBackcolor;

		AppSettingsData		m_appSettings;
		AppSettingsData		m_appSettingsOriginal;

		/* messages handlers */
		DECLARE_MESSAGE_MAP()

		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedBtnOk();
		afx_msg void OnBnClickedBtnClose();

		enum { IDD = IDD_OPTIONS };
		
		DECLARE_DYNAMIC(COptionsDlg)

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
public:
	afx_msg void OnBnClickedBtnBrwsPath();
	afx_msg void OnBnClickedBtnAmblightCol();
	afx_msg void OnBnClickedChkDisLighting();
	afx_msg void OnBnClickedChkAttachLightTocamera();
	afx_msg void OnBnClickedBtnDiffuseCol();
	afx_msg void OnBnClickedBtnAmbientCol();
	afx_msg void OnCbnSelchangeCmbDefFiletype();
	afx_msg void OnBnClickedChkCameraToCurrent();
	afx_msg void OnBnClickedChkResetCameraOnFileopen();
	afx_msg void OnBnClickedChkShowGrid();
	int m_iGridSize;
	afx_msg void OnDeltaposSpinGridSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEditGridSize();
	afx_msg void OnEnChangeEditGridSize();
	float m_fCellSize;
	afx_msg void OnEnKillfocusEditGridCellsize();
	afx_msg void OnEnChangeEditGridCellsize();
	afx_msg void OnBnClickedBtnGridwireCol();
	afx_msg void OnBnClickedBtnGridcrossCol();
	afx_msg void OnBnClickedBtnViewBackcolor();
	afx_msg void OnDeltaposSpinCellSize(NMHDR *pNMHDR, LRESULT *pResult);
	float m_fIconSize;
	afx_msg void OnEnChangeEditIconSize();
	CButton m_chkClearRecentFilesList;
	afx_msg void OnBnClickedChkClearRecentFilesList();
	afx_msg void OnBnClickedChkDontRepairXsi();
	afx_msg void OnBnClickedChkBindsFilterObjsByprefx();
	afx_msg void OnBnClickedChkScnFilesAltNullIco();
	afx_msg void OnBnClickedChkBindsShowCustObjs();
	afx_msg void OnBnClickedChkAskForFileSaving();
	afx_msg void OnBnClickedChkBackfaceCull();
	afx_msg void OnEnChangeEditEngineExec();
	afx_msg void OnBnClickedChkPreserve43();
	afx_msg void OnBnClickedChk1stMonitor();
	afx_msg void OnBnClickedBtnBrwsEngineDir();
	afx_msg void OnEnChangeEditEngineDir();
	afx_msg void OnBnClickedEditCustbtnPathbrwA();
	afx_msg void OnBnClickedEditCustbtnPathbrwB();
	afx_msg void OnEnChangeEditCustbtnCaptionA();
	afx_msg void OnEnChangeEditCustbtnCaptionB();
	afx_msg void OnEnChangeEditCustbtnPathA();
	afx_msg void OnEnChangeEditCustbtnPathB();
	afx_msg void OnEnChangeEditDefaultCamFov();
	afx_msg void OnBnClickedChkRunEngineDebug();
	afx_msg void OnBnClickedChkSwchEngineDebug();
};
