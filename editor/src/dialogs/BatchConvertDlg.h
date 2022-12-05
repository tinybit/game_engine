#pragma once

#include "CustomFileDialog.h"
#include <string>
#include <vector>

using namespace std;

enum BATCH_CONVERT_TYPES
{
	BCT_XSI2EFN,
	BCT_XSI2MESHLIST,
	BCT_ERRORCHECK
};

class CBatchConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CBatchConvertDlg)

	CString m_strCaption;
	CString m_strSourcePath;
	CString m_strDestinationPath;
	bool	m_bRecursiveConvert;
	bool	m_bReplaceExistingFiles;
	vector<CString> m_vecSrcFiles;
	CWnd*	m_pParent;

	enum BATCH_CONVERT_TYPES m_bctType;

	void ConvertXSI2AnimEFN();
	void ConvertXSI2AnimEFN_File( CString strSrcPath, CString strDestFolder, bool bOverwrite, bool bScale );
	void UpdateLastLogString( CString strValue );
	void AddLogString( CString strValue );
	void SHPathToPidl( LPCTSTR szpath, LPITEMIDLIST* ppidl );
	void SrcPathErrorcheckBrws();
	void SrcPathConvertEfnBrws();
	void ParseFolder( CString path );
	void Errorcheck();
	void Errorcheck_FileSCN( CString strSrcPath, bool bCorrectNormals );
	void Errorcheck_FileEFN( CString strSrcPath, bool bCorrectNormals );
	void ProcessMessages();

	public:
		CBatchConvertDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CBatchConvertDlg();
		
		void	SetConvertType( BATCH_CONVERT_TYPES bctValue ) { m_bctType = bctValue; };
		void	UpdateUI();

	// Dialog Data
		enum { IDD = IDD_BATCH_CONVERT };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedBtnSrcPathBrws();
		afx_msg void OnBnClickedBtnDestPathBrws();
		afx_msg void OnEnChangeEditDestDir();
		afx_msg void OnBnClickedOk();
		afx_msg void OnBnClickedBtnScenes();
		afx_msg void OnBnClickedBtnA();
		afx_msg void OnBnClickedBtnB();
		afx_msg void OnBnClickedBtnModels();
};
