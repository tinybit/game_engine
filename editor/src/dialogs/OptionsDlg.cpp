// OptionsDlg.cpp : implementation file

#pragma warning( disable: 4800 )

#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "CookieMachineDlg.h"
#include "OptionsDlg.h"
#include "shlobj.h"

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

/*-- COptionsDlg message mapping ------ */
BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_OK, OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_BRWS_PATH, OnBnClickedBtnBrwsPath)
	ON_BN_CLICKED(IDC_BTN_LIGHT_AMBIENT_COL, OnBnClickedBtnAmblightCol)
	ON_BN_CLICKED(IDC_CHK_DIS_LIGHTING, OnBnClickedChkDisLighting)
	ON_BN_CLICKED(IDC_CHK_ATTACH_LIGHT_TOCAMERA, OnBnClickedChkAttachLightTocamera)
	ON_BN_CLICKED(IDC_BTN_LIGHT_DIFFUSE_COL, OnBnClickedBtnDiffuseCol)
	ON_BN_CLICKED(IDC_BTN_AMBIENT_COL, OnBnClickedBtnAmbientCol)
	ON_CBN_SELCHANGE(IDC_CMB_DEF_FILETYPE, OnCbnSelchangeCmbDefFiletype)
	ON_BN_CLICKED(IDC_CHK_CAMERA_TO_CURRENT, OnBnClickedChkCameraToCurrent)
	ON_BN_CLICKED(IDC_CHK_RESET_CAMERA_ON_FILEOPEN, OnBnClickedChkResetCameraOnFileopen)
	ON_BN_CLICKED(IDC_CHK_SHOW_GRID, OnBnClickedChkShowGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRID_SIZE, OnDeltaposSpinGridSize)
	ON_EN_KILLFOCUS(IDC_EDIT_GRID_SIZE, OnEnKillfocusEditGridSize)
	ON_EN_CHANGE(IDC_EDIT_GRID_SIZE, OnEnChangeEditGridSize)
	ON_EN_KILLFOCUS(IDC_EDIT_GRID_CELLSIZE, OnEnKillfocusEditGridCellsize)
	ON_EN_CHANGE(IDC_EDIT_GRID_CELLSIZE, OnEnChangeEditGridCellsize)
	ON_BN_CLICKED(IDC_BTN_GRIDWIRE_COL, OnBnClickedBtnGridwireCol)
	ON_BN_CLICKED(IDC_BTN_GRIDCROSS_COL, OnBnClickedBtnGridcrossCol)
	ON_BN_CLICKED(IDC_BTN_VIEW_BACKCOLOR, OnBnClickedBtnViewBackcolor)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CELL_SIZE, OnDeltaposSpinCellSize)
	ON_EN_CHANGE(IDC_EDIT_ICON_SIZE, OnEnChangeEditIconSize)
	ON_BN_CLICKED(IDC_CHK_CLEAR_RECENT_FILES_LIST, OnBnClickedChkClearRecentFilesList)
	ON_BN_CLICKED(IDC_CHK_DONT_REPAIR_XSI, OnBnClickedChkDontRepairXsi)
	ON_BN_CLICKED(IDC_CHK_BINDS_FILTER_OBJS_BYPREFX, OnBnClickedChkBindsFilterObjsByprefx)
	ON_BN_CLICKED(IDC_CHK_SCN_FILES_ALT_NULL_ICO, OnBnClickedChkScnFilesAltNullIco)
	ON_BN_CLICKED(IDC_CHK_BINDS_SHOW_CUST_OBJS, OnBnClickedChkBindsShowCustObjs)
	ON_BN_CLICKED(IDC_CHK_ASK_FOR_FILE_SAVING, OnBnClickedChkAskForFileSaving)
	ON_BN_CLICKED(IDC_CHK_BACKFACE_CULL, OnBnClickedChkBackfaceCull)
	ON_EN_CHANGE(IDC_EDIT_ENGINE_EXEC, OnEnChangeEditEngineExec)
	ON_BN_CLICKED(IDC_CHK_PRESERVE4_3, OnBnClickedChkPreserve43)
	ON_BN_CLICKED(IDC_CHK_1ST_MONITOR, OnBnClickedChk1stMonitor)
	ON_BN_CLICKED(IDC_BTN_BRWS_ENGINE_DIR, OnBnClickedBtnBrwsEngineDir)
	ON_EN_CHANGE(IDC_EDIT_ENGINE_DIR, OnEnChangeEditEngineDir)
	ON_BN_CLICKED(IDC_EDIT_CUSTBTN_PATHBRW_A, OnBnClickedEditCustbtnPathbrwA)
	ON_BN_CLICKED(IDC_EDIT_CUSTBTN_PATHBRW_B, OnBnClickedEditCustbtnPathbrwB)
	ON_EN_CHANGE(IDC_EDIT_CUSTBTN_CAPTION_A, OnEnChangeEditCustbtnCaptionA)
	ON_EN_CHANGE(IDC_EDIT_CUSTBTN_CAPTION_B, OnEnChangeEditCustbtnCaptionB)
	ON_EN_CHANGE(IDC_EDIT_CUSTBTN_PATH_A, OnEnChangeEditCustbtnPathA)
	ON_EN_CHANGE(IDC_EDIT_CUSTBTN_PATH_B, OnEnChangeEditCustbtnPathB)
	ON_EN_CHANGE(IDC_EDIT_DEFAULT_CAM_FOV, OnEnChangeEditDefaultCamFov)
	ON_BN_CLICKED(IDC_CHK_RUN_ENGINE_DEBUG, OnBnClickedChkRunEngineDebug)
	ON_BN_CLICKED(IDC_CHK_SWCH_ENGINE_DEBUG, OnBnClickedChkSwchEngineDebug)
END_MESSAGE_MAP()
/*-- COptionsDlg message mapping end -- */

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/) : CDialog(COptionsDlg::IDD, pParent) , m_iGridSize(0)
, m_fCellSize(0)
, m_fIconSize(0)
{
	m_pParent					= pParent;
	m_bOK						= false;
	m_bSettingsChanged	= false;
}

COptionsDlg::~COptionsDlg()
{
	m_pParent = NULL;
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GRID_SIZE, m_iGridSize);
	DDX_Text(pDX, IDC_EDIT_GRID_CELLSIZE, m_fCellSize);
	DDX_Text(pDX, IDC_EDIT_ICON_SIZE, m_fIconSize);
	DDX_Control(pDX, IDC_CHK_CLEAR_RECENT_FILES_LIST, m_chkClearRecentFilesList);
}

void COptionsDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CRect parent_wnd_rect, wnd_rect, wnd_res;

	//move validation window to the center of the modal window
	this->GetWindowRect(&wnd_rect);
	m_pParent->GetWindowRect(&parent_wnd_rect);

	wnd_res.left		= parent_wnd_rect.left + (parent_wnd_rect.right - parent_wnd_rect.left) / 2		- (wnd_rect.right - wnd_rect.left) / 2;
	wnd_res.top		= parent_wnd_rect.top + (parent_wnd_rect.bottom - parent_wnd_rect.top) / 2	- (wnd_rect.bottom - wnd_rect.top) / 2;
	wnd_res.right		= wnd_res.left + (wnd_rect.right - wnd_rect.left); 
	wnd_res.bottom	= wnd_res.top + (wnd_rect.bottom - wnd_rect.top);

	this->MoveWindow(wnd_res, true);

	VERIFY(cbAmbLight.Attach(IDC_BTN_LIGHT_AMBIENT_COL, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)));
	VERIFY(cbDiffLight.Attach(IDC_BTN_LIGHT_DIFFUSE_COL, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)));
	VERIFY(cbAmbient.Attach(IDC_BTN_AMBIENT_COL, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)));

	VERIFY(cbGridCross.Attach(IDC_BTN_GRIDCROSS_COL, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)));
	VERIFY(cbGridWires.Attach(IDC_BTN_GRIDWIRE_COL, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)));
	VERIFY(cbViewportBackcolor.Attach(IDC_BTN_VIEW_BACKCOLOR, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)));

	((CCookieMachineDlg*)m_pParent)->StartRendering();

	CSpinButtonCtrl* sp	= (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_GRID_SIZE );
	CSpinButtonCtrl* sp2	= (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_CELL_SIZE );
	sp->SetRange( 1, 5000 );
	sp2->SetRange( 2, 5000 );

	UpdateUI();
}

bool COptionsDlg::CheckForChanges()
{
	UpdateData( true );

	m_appSettings.GridSize		= m_iGridSize;
	m_appSettings.GridCellSize	= m_fCellSize;
	m_appSettings.IconsScaling	= m_fIconSize;

	if( m_appSettings.FilesPath != m_appSettingsOriginal.FilesPath || m_appSettings.DefaultFileType != m_appSettingsOriginal.DefaultFileType )
		return true;

	if( m_appSettings.LightAmbientColor != m_appSettingsOriginal.LightAmbientColor || m_appSettings.LightingEnabled != m_appSettingsOriginal.LightingEnabled)
		return true;

	if( m_appSettings.LightDiffuseColor != m_appSettingsOriginal.LightDiffuseColor || m_appSettings.AttachToCamera != m_appSettingsOriginal.AttachToCamera)
		return true;

	if( m_appSettings.AmbientColor != m_appSettingsOriginal.AmbientColor )
		return true;

	CButton* chk_def_cam = (CButton*)GetDlgItem( IDC_CHK_CAMERA_TO_CURRENT );

	if( chk_def_cam->GetCheck() == 1 )
		return true;

	if( m_appSettings.ResetCameraOnFileOpen != m_appSettingsOriginal.ResetCameraOnFileOpen || m_appSettings.GridVisible != m_appSettingsOriginal.GridVisible )
		return true;

	if( m_appSettings.GridSize != m_appSettingsOriginal.GridSize ||  m_appSettings.GridCellSize != m_appSettingsOriginal.GridCellSize )
		return true;

	if( m_appSettings.GridCrossColor != m_appSettingsOriginal.GridCrossColor || m_appSettings.GridWiresColor != m_appSettingsOriginal.GridWiresColor )
		return true;

	if( m_appSettings.IconsScaling != m_appSettingsOriginal.IconsScaling )
		return true;

	if( m_appSettings.RecentFilesClear != m_appSettingsOriginal.RecentFilesClear )
		return true;

	if( m_appSettings.EngineDebugModeOn != m_appSettingsOriginal.EngineDebugModeOn )
		return true;

	if( m_appSettings.ViewportBackColor != m_appSettingsOriginal.ViewportBackColor )
		return true;

	if( m_appSettings.RepairXSIFiles != m_appSettingsOriginal.RepairXSIFiles )
		return true;

	if( m_appSettings.FilterObjsByPrefix != m_appSettingsOriginal.FilterObjsByPrefix )
		return true;

	if( m_appSettings.ScnAlternateNullIco != m_appSettingsOriginal.ScnAlternateNullIco )
		return true;

	if( m_appSettings.StartParams != m_appSettingsOriginal.StartParams )
		return true;

	if( m_appSettings.ExpertMode != m_appSettingsOriginal.ExpertMode )
		return true;

	if( m_appSettings.AskForSavingOnAppQuit != m_appSettingsOriginal.AskForSavingOnAppQuit )
		return true;

	if( m_appSettings.ViewportBackfaceCull != m_appSettingsOriginal.ViewportBackfaceCull )
		return true;

	if( m_appSettings.ViewportPreserve43 != m_appSettingsOriginal.ViewportPreserve43 )
		return true;

	if( m_appSettings.FirstMonitorOnly != m_appSettingsOriginal.FirstMonitorOnly )
		return true;

	if( m_appSettings.EngineFolder != m_appSettingsOriginal.EngineFolder )
		return true;

	if( m_appSettings.Button_A_Capt != m_appSettingsOriginal.Button_A_Capt )
		return true;

	if( m_appSettings.Button_A_Path != m_appSettingsOriginal.Button_A_Path )
		return true;

	if( m_appSettings.Button_B_Capt != m_appSettingsOriginal.Button_B_Capt )
		return true;

	if( m_appSettings.Button_B_Path != m_appSettingsOriginal.Button_B_Path )
		return true;

	if( m_appSettings.DefaultCamera.hfov != m_appSettingsOriginal.DefaultCamera.hfov )
		return true;

	if( m_appSettings.SwitchBackOnDebug != m_appSettingsOriginal.SwitchBackOnDebug )
		return true;

	return false;
}

void COptionsDlg::OnBnClickedBtnOk()
{
	m_bSettingsChanged = CheckForChanges();

	//refresh settings

	// general params
	CWnd*		pWnd			= GetDlgItem(IDC_ED_DEF_FILES_DIR);
	CComboBox*	pCmbFiletype	= (CComboBox*)GetDlgItem(IDC_CMB_DEF_FILETYPE);

	pWnd->GetWindowText( m_appSettings.FilesPath );

	switch( pCmbFiletype->GetCurSel() )
	{
		case 0:
			m_appSettings.DefaultFileType = FT_XSI;
		break;

		case 1:
			m_appSettings.DefaultFileType = FT_EFN;
		break;

		case 2:
			m_appSettings.DefaultFileType = FT_SCN;
		break;
	}

	m_bOK = true;

	OnOK();
}

void COptionsDlg::OnBnClickedBtnClose()
{
	OnOK();
}

void COptionsDlg::SetValues(AppSettingsData &appSettings)
{
	m_appSettingsOriginal = m_appSettings = appSettings;
}

AppSettingsData COptionsDlg::GetValues()
{
	return m_appSettings;
}

void COptionsDlg::UpdateUI()
{
	//set general params
	CWnd* pWnd = GetDlgItem(IDC_ED_DEF_FILES_DIR);
	pWnd->SetWindowText( m_appSettings.FilesPath );

	
	CComboBox* pCmbFiletype					= (CComboBox*)GetDlgItem( IDC_CMB_DEF_FILETYPE );
	CButton*		pCmbShowgrid			= (CButton*)GetDlgItem( IDC_CHK_SHOW_GRID );
	CButton*		btnEnable				= (CButton*)GetDlgItem( IDC_CHK_DIS_LIGHTING );
	CButton*		btnAttach2Cam			= (CButton*)GetDlgItem( IDC_CHK_ATTACH_LIGHT_TOCAMERA );
	CButton*		btnResCamOnFileopen		= (CButton*)GetDlgItem( IDC_CHK_RESET_CAMERA_ON_FILEOPEN );
	CButton*		btnXSIFilesRepair		= (CButton*)GetDlgItem( IDC_CHK_DONT_REPAIR_XSI );
	CButton*		btnFilterObjByPrefx		= (CButton*)GetDlgItem( IDC_CHK_BINDS_FILTER_OBJS_BYPREFX );
	CButton*		btnScnAltNullIcons		= (CButton*)GetDlgItem( IDC_CHK_SCN_FILES_ALT_NULL_ICO );
	CButton*		btnShowCustObjs			= (CButton*)GetDlgItem( IDC_CHK_BINDS_SHOW_CUST_OBJS );
	CButton*		btnAskForSavingOnQuit	= (CButton*)GetDlgItem( IDC_CHK_ASK_FOR_FILE_SAVING );
	CButton*		btn1stMonitorOnly		= (CButton*)GetDlgItem( IDC_CHK_1ST_MONITOR );
	CButton*		btnIsBackfaceCullingON	= (CButton*)GetDlgItem( IDC_CHK_BACKFACE_CULL );
	CButton*		btnPreserve43			= (CButton*)GetDlgItem( IDC_CHK_PRESERVE4_3 );
	CEdit*			editEngExePath			= (CEdit*)GetDlgItem( IDC_EDIT_ENGINE_EXEC );
	CEdit*			editEngDir				= (CEdit*)GetDlgItem( IDC_EDIT_ENGINE_DIR );
	CSpinButtonCtrl* sp						= (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_GRID_SIZE );
	CSpinButtonCtrl* sp2					= (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_CELL_SIZE );
	CButton*		chkIsEngineDebugMode	= (CButton*)GetDlgItem( IDC_CHK_RUN_ENGINE_DEBUG );
	CButton*		chkSwitchBackOnDebug	= (CButton*)GetDlgItem( IDC_CHK_SWCH_ENGINE_DEBUG );

	CEdit*			editBtn_A_Capt			= (CEdit*)GetDlgItem( IDC_EDIT_CUSTBTN_CAPTION_A );
	CEdit*			editBtn_A_Path			= (CEdit*)GetDlgItem( IDC_EDIT_CUSTBTN_PATH_A );
	CEdit*			editBtn_B_Capt			= (CEdit*)GetDlgItem( IDC_EDIT_CUSTBTN_CAPTION_B );
	CEdit*			editBtn_B_Path			= (CEdit*)GetDlgItem( IDC_EDIT_CUSTBTN_PATH_B );

	CEdit*			editDefCameraFOV		= (CEdit*)GetDlgItem( IDC_EDIT_DEFAULT_CAM_FOV );

	editEngExePath->SetWindowText( m_appSettings.StartParams );
	editEngDir->SetWindowText( m_appSettings.EngineFolder );

	editBtn_A_Capt->SetWindowText( m_appSettings.Button_A_Capt );
	editBtn_A_Path->SetWindowText( m_appSettings.Button_A_Path );
	editBtn_B_Capt->SetWindowText( m_appSettings.Button_B_Capt );
	editBtn_B_Path->SetWindowText( m_appSettings.Button_B_Path );

	CString strTemp;
	strTemp.Format( "%.2f", m_appSettings.DefaultCamera.hfov );
	editDefCameraFOV->SetWindowText( strTemp );

	m_iGridSize	= m_appSettings.GridSize;
	m_fCellSize	= m_appSettings.GridCellSize;
	m_fIconSize	= m_appSettings.IconsScaling;


	//default filetype combobox
	pCmbFiletype->ResetContent();
	pCmbFiletype->AddString("dotXSI Files (*.xsi)");
	pCmbFiletype->AddString("EFN Files (*.efn)");
	pCmbFiletype->AddString("Scene Files (*.scn)");
	pCmbFiletype->AddString("All Supported Files (*.scn, *.efn, *.xsi)");


	if( m_appSettings.DefaultFileType == FT_XSI )
		pCmbFiletype->SetCurSel( 0 );

	if( m_appSettings.DefaultFileType == FT_EFN )
		pCmbFiletype->SetCurSel( 1 );

	if( m_appSettings.DefaultFileType == FT_SCN )
		pCmbFiletype->SetCurSel( 2 );

	if( m_appSettings.DefaultFileType == FT_ALLSUPPORTED )
		pCmbFiletype->SetCurSel( 3 );
	

	//set light params
	cbAmbLight.SetColor( m_appSettings.LightAmbientColor );
	cbDiffLight.SetColor( m_appSettings.LightDiffuseColor ); 
	cbAmbient.SetColor( m_appSettings.AmbientColor );

	cbGridCross.SetColor( m_appSettings.GridCrossColor );
	cbGridWires.SetColor( m_appSettings.GridWiresColor ); 
	cbViewportBackcolor.SetColor( m_appSettings.ViewportBackColor );


	btnEnable->SetCheck( !m_appSettings.LightingEnabled );
	btnAttach2Cam->SetCheck( m_appSettings.AttachToCamera );

	if( m_appSettingsOriginal.ResetCameraOnFileOpen )
		btnResCamOnFileopen->SetCheck(1);
	else
		btnResCamOnFileopen->SetCheck(0);

	if( m_appSettingsOriginal.EngineDebugModeOn )
		chkIsEngineDebugMode->SetCheck(1);
	else
		chkIsEngineDebugMode->SetCheck(0);

	
	if( m_appSettingsOriginal.RepairXSIFiles )
		btnXSIFilesRepair->SetCheck(0);
	else
		btnXSIFilesRepair->SetCheck(1);


	if( m_appSettingsOriginal.SwitchBackOnDebug )
		chkSwitchBackOnDebug->SetCheck(1);
	else
		chkSwitchBackOnDebug->SetCheck(0);


	if( m_appSettingsOriginal.FilterObjsByPrefix )
		btnFilterObjByPrefx->SetCheck(1);
	else
		btnFilterObjByPrefx->SetCheck(0);


	if( m_appSettingsOriginal.ScnAlternateNullIco )
		btnScnAltNullIcons->SetCheck(1);
	else
		btnScnAltNullIcons->SetCheck(0);


	if( m_appSettingsOriginal.ExpertMode )
		btnShowCustObjs->SetCheck(1);
	else
		btnShowCustObjs->SetCheck(0);


	if( m_appSettingsOriginal.ViewportPreserve43 )
		btnPreserve43->SetCheck(1);
	else
		btnPreserve43->SetCheck(0);


	if( m_appSettingsOriginal.AskForSavingOnAppQuit )
		btnAskForSavingOnQuit->SetCheck(1);
	else
		btnAskForSavingOnQuit->SetCheck(0);


	if( m_appSettingsOriginal.FirstMonitorOnly )
		btn1stMonitorOnly->SetCheck(1);
	else
		btn1stMonitorOnly->SetCheck(0);


	if( m_appSettingsOriginal.GridVisible )
		pCmbShowgrid->SetCheck(1);
	else
		pCmbShowgrid->SetCheck(0);


	if( m_appSettingsOriginal.ViewportBackfaceCull )
		btnIsBackfaceCullingON->SetCheck(1);
	else
		btnIsBackfaceCullingON->SetCheck(0);

	UpdateData(false);

	sp->SetPos( m_iGridSize / 2 );
	sp2->SetPos( (int)(m_fCellSize * 10.0f) );

	UpdateData(false);

	Invalidate(true);
}

void COptionsDlg::OnBnClickedBtnBrwsPath()
{
	LPITEMIDLIST	pidl = NULL;
	BROWSEINFO	bi;

	ZeroMemory( &bi, sizeof(BROWSEINFO) );
	bi.lpszTitle = __TEXT("Выберите папку:");
	bi.hwndOwner = this->GetSafeHwnd();

	SHGetSpecialFolderLocation( this->GetSafeHwnd(), CSIDL_DRIVES , &pidl );

	bi.pidlRoot = pidl;

	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;

	pidl = SHBrowseForFolder(&bi);

    if ( pidl != NULL )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if( SHGetPathFromIDList ( pidl, path ) )
        {
            m_appSettings.FilesPath = path;
        }

        // free memory used
		IMalloc* imalloc = NULL;
        if( SUCCEEDED(SHGetMalloc(&imalloc)) )
        {
            imalloc->Free( pidl );
            imalloc->Release();
        }
    }

	UpdateUI();
}

void COptionsDlg::OnBnClickedBtnAmblightCol()
{
	CColorDialog dlg(m_appSettings.LightAmbientColor, CC_FULLOPEN);
	dlg.DoModal();

	m_appSettings.LightAmbientColor = dlg.GetColor();
	
	UpdateUI();
}

void COptionsDlg::OnBnClickedChkDisLighting()
{
	m_appSettings.LightingEnabled = !m_appSettings.LightingEnabled;
}

void COptionsDlg::OnBnClickedChkAttachLightTocamera()
{
	m_appSettings.AttachToCamera = !m_appSettings.AttachToCamera;
}

void COptionsDlg::OnBnClickedBtnDiffuseCol()
{
	CColorDialog dlg(m_appSettings.LightDiffuseColor, CC_FULLOPEN);

	dlg.DoModal();

	m_appSettings.LightDiffuseColor = dlg.GetColor();
	
	UpdateUI();

	((CCookieMachineDlg*)m_pParent)->SettingsApplyToApp( m_appSettings );
}

void COptionsDlg::OnBnClickedBtnAmbientCol()
{
	CColorDialog dlg(m_appSettings.AmbientColor, CC_FULLOPEN);
	dlg.DoModal();

	m_appSettings.AmbientColor = dlg.GetColor();
	
	UpdateUI();

	((CCookieMachineDlg*)m_pParent)->SettingsApplyToApp( m_appSettings );
}


void COptionsDlg::OnCbnSelchangeCmbDefFiletype()
{
	CComboBox* pCmbFiletype = (CComboBox*)GetDlgItem(IDC_CMB_DEF_FILETYPE);

	if( pCmbFiletype->GetCurSel() == 0 )
		m_appSettings.DefaultFileType = FT_XSI;

	if( pCmbFiletype->GetCurSel() == 1 )
		m_appSettings.DefaultFileType = FT_EFN;

	if( pCmbFiletype->GetCurSel() == 2 )
		m_appSettings.DefaultFileType = FT_SCN;

	if( pCmbFiletype->GetCurSel() == 3 )
		m_appSettings.DefaultFileType = FT_ALLSUPPORTED;
}

void COptionsDlg::OnBnClickedChkCameraToCurrent()
{
	m_appSettings.CameraToCurrent = !m_appSettings.CameraToCurrent;
}

void COptionsDlg::OnBnClickedChkResetCameraOnFileopen()
{
	m_appSettings.ResetCameraOnFileOpen = !m_appSettings.ResetCameraOnFileOpen;
}

void COptionsDlg::OnBnClickedChkShowGrid()
{
	m_appSettings.GridVisible = !m_appSettings.GridVisible;
}

void COptionsDlg::OnDeltaposSpinGridSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CSpinButtonCtrl* sp = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_GRID_SIZE );

	m_iGridSize = ( sp->GetPos() - 65537 ) * 2;

	if( m_iGridSize < 2 )
	{
		m_iGridSize = 2;
		sp->SetPos( 2 );
	}

	if( m_iGridSize > 10000 )
	{
		m_iGridSize = 10000;
		sp->SetPos( 5000 );
	}

	UpdateData( false );

	*pResult = 0;
}

void COptionsDlg::OnEnKillfocusEditGridSize()
{	
	CSpinButtonCtrl* sp = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_GRID_SIZE );
	
	UpdateData( true );

	if( m_iGridSize < 2 )
	{
		m_iGridSize = 2;
		sp->SetPos( 2 );
	}

	if( m_iGridSize > 10000 )
	{
		m_iGridSize = 9998;
		sp->SetPos( 5000 );
	}


	sp->SetPos( m_iGridSize / 2 );

	UpdateData( false );
}

void COptionsDlg::OnEnChangeEditGridSize()
{
	UpdateData( true );

	CSpinButtonCtrl* sp = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_GRID_SIZE );
	sp->SetPos( m_iGridSize / 2 );
}

void COptionsDlg::OnEnKillfocusEditGridCellsize()
{
	UpdateData( true );
}

void COptionsDlg::OnEnChangeEditGridCellsize()
{
	UpdateData( true );
}

void COptionsDlg::OnBnClickedBtnGridwireCol()
{
	CColorDialog dlg(m_appSettings.LightDiffuseColor, CC_FULLOPEN);

	dlg.DoModal();

	m_appSettings.GridWiresColor = dlg.GetColor();
	UpdateUI();
}

void COptionsDlg::OnBnClickedBtnGridcrossCol()
{
	CColorDialog dlg(m_appSettings.LightDiffuseColor, CC_FULLOPEN);

	dlg.DoModal();

	m_appSettings.GridCrossColor = dlg.GetColor();
	UpdateUI();
}

void COptionsDlg::OnBnClickedBtnViewBackcolor()
{
	CColorDialog dlg(m_appSettings.LightDiffuseColor, CC_FULLOPEN);

	dlg.DoModal();

	m_appSettings.ViewportBackColor = dlg.GetColor();
	UpdateUI();
}

void COptionsDlg::OnDeltaposSpinCellSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CSpinButtonCtrl* sp = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_CELL_SIZE );

	m_fCellSize = (float)( sp->GetPos() - 65537 ) / 10.0f;

	UpdateData( false );

	*pResult = 0;
}

void COptionsDlg::OnEnChangeEditIconSize()
{
	UpdateData( true );
}

void COptionsDlg::OnBnClickedChkClearRecentFilesList()
{
	m_appSettings.RecentFilesClear = (bool)(m_chkClearRecentFilesList.GetCheck());
}

void COptionsDlg::OnBnClickedChkDontRepairXsi()
{
	m_appSettings.RepairXSIFiles = !m_appSettings.RepairXSIFiles;
}

void COptionsDlg::OnBnClickedChkBindsFilterObjsByprefx()
{
	m_appSettings.FilterObjsByPrefix = !m_appSettings.FilterObjsByPrefix;
}

void COptionsDlg::OnBnClickedChkScnFilesAltNullIco()
{
	m_appSettings.ScnAlternateNullIco = !m_appSettings.ScnAlternateNullIco;
}

void COptionsDlg::OnBnClickedChkBindsShowCustObjs()
{
	m_appSettings.ExpertMode = !m_appSettings.ExpertMode;
}

void COptionsDlg::OnBnClickedChkAskForFileSaving()
{
	m_appSettings.AskForSavingOnAppQuit = !m_appSettings.AskForSavingOnAppQuit;
}

void COptionsDlg::OnBnClickedChkBackfaceCull()
{
	m_appSettings.ViewportBackfaceCull = !m_appSettings.ViewportBackfaceCull;
}

void COptionsDlg::OnEnChangeEditEngineExec()
{
	CString StartParams;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_ENGINE_EXEC );
	pWnd->GetWindowText( StartParams );

	m_appSettings.StartParams = StartParams;
}

void COptionsDlg::OnBnClickedChkPreserve43()
{
	m_appSettings.ViewportPreserve43 = !m_appSettings.ViewportPreserve43;
}

void COptionsDlg::OnBnClickedChk1stMonitor()
{
	m_appSettings.FirstMonitorOnly = !m_appSettings.FirstMonitorOnly;
}

void COptionsDlg::OnBnClickedBtnBrwsEngineDir()
{
	LPITEMIDLIST	pidl = NULL;
	BROWSEINFO		bi;

	ZeroMemory( &bi, sizeof(BROWSEINFO) );
	bi.lpszTitle = __TEXT("Выберите папку:");
	bi.hwndOwner = this->GetSafeHwnd();

	SHGetSpecialFolderLocation( this->GetSafeHwnd(), CSIDL_DRIVES , &pidl );

	bi.pidlRoot = pidl;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;

	pidl = SHBrowseForFolder(&bi);

    if( pidl != NULL )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if( SHGetPathFromIDList ( pidl, path ) )
        {
            m_appSettings.EngineFolder = path;
        }

        // free memory used
		IMalloc* imalloc = NULL;
        if( SUCCEEDED(SHGetMalloc(&imalloc)) )
        {
            imalloc->Free( pidl );
            imalloc->Release();
        }
    }

	UpdateUI();
}

void COptionsDlg::OnEnChangeEditEngineDir()
{
	CString strEngineDir;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_ENGINE_DIR );
	pWnd->GetWindowText( strEngineDir );

	m_appSettings.EngineFolder = strEngineDir;
}

void COptionsDlg::OnBnClickedEditCustbtnPathbrwA()
{
	LPITEMIDLIST	pidl = NULL;
	BROWSEINFO	bi;

	ZeroMemory( &bi, sizeof(BROWSEINFO) );
	bi.lpszTitle = __TEXT("Выберите папку:");
	bi.hwndOwner = this->GetSafeHwnd();

	SHGetSpecialFolderLocation( this->GetSafeHwnd(), CSIDL_DRIVES , &pidl );

	bi.pidlRoot = pidl;

	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;

	pidl = SHBrowseForFolder(&bi);

    if ( pidl != NULL )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if( SHGetPathFromIDList ( pidl, path ) )
        {
            m_appSettings.Button_A_Path = path;
        }

        // free memory used
		IMalloc* imalloc = NULL;
        if( SUCCEEDED(SHGetMalloc(&imalloc)) )
        {
            imalloc->Free( pidl );
            imalloc->Release();
        }
    }

	UpdateUI();
}

void COptionsDlg::OnBnClickedEditCustbtnPathbrwB()
{
	LPITEMIDLIST	pidl = NULL;
	BROWSEINFO	bi;

	ZeroMemory( &bi, sizeof(BROWSEINFO) );
	bi.lpszTitle = __TEXT("Выберите папку:");
	bi.hwndOwner = this->GetSafeHwnd();

	SHGetSpecialFolderLocation( this->GetSafeHwnd(), CSIDL_DRIVES , &pidl );

	bi.pidlRoot = pidl;

	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;

	pidl = SHBrowseForFolder(&bi);

    if ( pidl != NULL )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if( SHGetPathFromIDList ( pidl, path ) )
        {
            m_appSettings.Button_B_Path = path;
        }

        // free memory used
		IMalloc* imalloc = NULL;
        if( SUCCEEDED(SHGetMalloc(&imalloc)) )
        {
            imalloc->Free( pidl );
            imalloc->Release();
        }
    }

	UpdateUI();
}

void COptionsDlg::OnEnChangeEditCustbtnCaptionA()
{
	CString strTemp;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_CUSTBTN_CAPTION_A );
	pWnd->GetWindowText( strTemp );

	m_appSettings.Button_A_Capt = strTemp;
}

void COptionsDlg::OnEnChangeEditCustbtnCaptionB()
{
	CString strTemp;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_CUSTBTN_CAPTION_B );
	pWnd->GetWindowText( strTemp );

	m_appSettings.Button_B_Capt = strTemp;
}

void COptionsDlg::OnEnChangeEditCustbtnPathA()
{
	CString strTemp;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_CUSTBTN_PATH_A );
	pWnd->GetWindowText( strTemp );

	m_appSettings.Button_A_Path = strTemp;
}

void COptionsDlg::OnEnChangeEditCustbtnPathB()
{
	CString strTemp;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_CUSTBTN_PATH_B );
	pWnd->GetWindowText( strTemp );

	m_appSettings.Button_B_Path = strTemp;
}

void COptionsDlg::OnEnChangeEditDefaultCamFov()
{
	CEdit* editDefCameraFOV = (CEdit*)GetDlgItem( IDC_EDIT_DEFAULT_CAM_FOV );

	CString strTemp;
	editDefCameraFOV->GetWindowText( strTemp );

	if( atof( strTemp.GetBuffer() ) != 0 )
		m_appSettings.DefaultCamera.hfov = (float)atof( strTemp.GetBuffer() );
}

void COptionsDlg::OnBnClickedChkRunEngineDebug()
{
	m_appSettings.EngineDebugModeOn = !m_appSettings.EngineDebugModeOn;
}

void COptionsDlg::OnBnClickedChkSwchEngineDebug()
{
	m_appSettings.SwitchBackOnDebug = !m_appSettings.SwitchBackOnDebug;
}
