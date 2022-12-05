#pragma warning(disable: 4800)

#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "BatchConvertDlg.h"
#include "CookieMachineDlg.h"
#include ".\batchconvertdlg.h"

//float	max_coord	= 0.0f;
//string	Rfile		= "";

IMPLEMENT_DYNAMIC(CBatchConvertDlg, CDialog)
CBatchConvertDlg::CBatchConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchConvertDlg::IDD, pParent)
{
	m_strCaption			= "";
	m_strSourcePath			= "";
	m_strDestinationPath	= "";
	m_bRecursiveConvert		= true;
	m_bReplaceExistingFiles	= true;
	m_pParent				= pParent;
}

CBatchConvertDlg::~CBatchConvertDlg()
{
}

void CBatchConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBatchConvertDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_SRC_PATH_BRWS, OnBnClickedBtnSrcPathBrws)
	ON_BN_CLICKED(IDC_BTN_DEST_PATH_BRWS, OnBnClickedBtnDestPathBrws)
	ON_EN_CHANGE(IDC_EDIT_DEST_DIR, OnEnChangeEditDestDir)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SCENES, OnBnClickedBtnScenes)
	ON_BN_CLICKED(IDC_BTN_A, OnBnClickedBtnA)
	ON_BN_CLICKED(IDC_BTN_B, OnBnClickedBtnB)
	ON_BN_CLICKED(IDC_BTN_MODELS, OnBnClickedBtnModels)
END_MESSAGE_MAP()


void CBatchConvertDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( m_pParent )
	{
		CString		strTemp;
		CButton*	pBnA = (CButton*)GetDlgItem( IDC_BTN_A );
		CButton*	pBnB = (CButton*)GetDlgItem( IDC_BTN_B );

		const AppSettingsData* pAppSetts = ((CCookieMachineDlg*)m_pParent)->GetAppSettingsGlob();

		
		strTemp = pAppSetts->Button_A_Capt;
		strTemp.Trim();
		
		if( strTemp == "" )
			pBnA->ShowWindow( false );
		else
			pBnA->SetWindowText( strTemp );


		strTemp = pAppSetts->Button_B_Capt;
		strTemp.Trim();

		if( strTemp == "" )
			pBnB->ShowWindow( false );
		else
			pBnB->SetWindowText( strTemp );
	}

	switch( m_bctType )
	{
		case BCT_XSI2EFN:
			SetWindowText( "Batch files convert: XSI to animated EFN" );
			break;

		case BCT_XSI2MESHLIST:
			SetWindowText( "Batch files convert: XSI to meshlist EFN" );
			break;

		case BCT_ERRORCHECK:
			SetWindowText( "Batch files convert: animated EFN, meshlist EFN, SCN errorcheck" );
			break;
	};

	UpdateUI();
}

void CBatchConvertDlg::SrcPathConvertEfnBrws()
{
	string	szFilter;
	CString	temp;

	temp = "*.xsi";
	szFilter = "XSI Files (*.xsi)|*.xsi|";	

	char strFiles[65536];
	ZeroMemory( strFiles, sizeof(char) * 65536 );

	CCustomFileDialog* dlg = new CCustomFileDialog( TRUE, temp.GetBuffer(0), 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING | OFN_ALLOWMULTISELECT, szFilter.data(), NULL );
	dlg->m_pOFN->lpstrInitialDir	= "c:\\";
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);
	dlg->m_pOFN->lpstrFile			= strFiles;
	dlg->m_pOFN->nMaxFile			= 65535;

	//LPSTR

	string file_dir, file_name;

	if( dlg->DoModal() == IDOK )
	{
		//get file dir
		//file_dir = dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );

		//AfxMessageBox( ("dir: " + file_dir).data() );

		m_vecSrcFiles.clear();

		POSITION pos = dlg->GetStartPosition();

		while( pos )
			m_vecSrcFiles.push_back( dlg->GetNextPathName( pos ) );

		//get filename
		file_name	= dlg->GetFileName();
	}

	if( dlg )
		delete dlg;

	UpdateUI();
}

void CBatchConvertDlg::SrcPathErrorcheckBrws()
{
	CString			strPath;
	LPITEMIDLIST	pidl = NULL;
	BROWSEINFO		bi;

	ZeroMemory( &bi, sizeof(BROWSEINFO) );
	bi.lpszTitle = __TEXT( "Выберите папку:" );
	bi.hwndOwner = this->GetSafeHwnd();

	SHGetSpecialFolderLocation( this->GetSafeHwnd(), CSIDL_DRIVES, &pidl );

	bi.pidlRoot = pidl;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;

	pidl = SHBrowseForFolder( &bi );

    if( pidl != NULL )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if( SHGetPathFromIDList( pidl, path ) )
        {
            strPath = path;
        }

        // free memory used
		IMalloc* imalloc = NULL;
        if( SUCCEEDED( SHGetMalloc(&imalloc) ) )
        {
            imalloc->Free( pidl );
            imalloc->Release();
        }
    }

	ParseFolder( strPath );

	//UpdateUI();
}

void CBatchConvertDlg::ParseFolder( CString path )
{
	WIN32_FIND_DATA findData;
	bool			bMoreFiles	= true;
	HANDLE			hFind		= FindFirstFile( path + "\\*.*" , &findData );
	CString			strTemp;

	while( hFind != INVALID_HANDLE_VALUE && bMoreFiles )
	{
		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			strTemp = findData.cFileName;

			if( strTemp != "." && strTemp != ".." )
				ParseFolder( path + "\\" + strTemp );
		}

		if( FindNextFile( hFind, &findData ) )
			bMoreFiles = true;
		else
			bMoreFiles = false;
	}


	bMoreFiles	= true;

	if( m_bctType == BCT_ERRORCHECK )
		hFind = FindFirstFile( path + "\\*.scn" , &findData );
	else
		hFind = FindFirstFile( path + "\\*.efn" , &findData );

	while( hFind != INVALID_HANDLE_VALUE && bMoreFiles )
	{
		if( !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			CListBox* pSrcFilesBox = (CListBox*)GetDlgItem( IDC_LST_SRC_FILES );

			m_vecSrcFiles.push_back( path + "\\" + findData.cFileName );
			pSrcFilesBox->AddString( path + "\\" + findData.cFileName );

			int nCount = pSrcFilesBox->GetCount();

			if( nCount > 0 )
				pSrcFilesBox->SetCurSel( nCount - 1 );
		}

		if( FindNextFile( hFind, &findData ) )
			bMoreFiles = true;
		else
			bMoreFiles = false;
	}

	FindClose( hFind );
}

void CBatchConvertDlg::OnBnClickedBtnSrcPathBrws()
{
	if( m_bctType == BCT_ERRORCHECK )
	{
		SrcPathErrorcheckBrws();
	}
	else
	{
		SrcPathConvertEfnBrws();
	}
}

void CBatchConvertDlg::UpdateUI()
{
	CListBox*	pSrcFiles		= (CListBox*)GetDlgItem( IDC_LST_SRC_FILES );
	CWnd*		pDestBrws		= GetDlgItem( IDC_BTN_DEST_PATH_BRWS );
	CEdit*		pDestPath		= (CEdit*)GetDlgItem( IDC_EDIT_DEST_DIR );
	CButton*	pBtnRepl		= (CButton*)GetDlgItem( IDC_CHK_REPLACE_EXISTING );
	CButton*	pBtnScale		= (CButton*)GetDlgItem( IDC_CHK_SHULTZ_SCALING );
	CButton*	pBtnNormals		= (CButton*)GetDlgItem( IDC_CHK_NORMALS );
	CButton*	pBtnAnim		= (CButton*)GetDlgItem( IDC_CHK_ANIM_RANGE );
	CButton*	pBtnModels		= (CButton*)GetDlgItem( IDC_BTN_MODELS );
	CButton*	pBtnScenes		= (CButton*)GetDlgItem( IDC_BTN_SCENES );
	CButton*	pBtnA			= (CButton*)GetDlgItem( IDC_BTN_A );
	CButton*	pBtnB			= (CButton*)GetDlgItem( IDC_BTN_B );

	if( m_bctType == BCT_ERRORCHECK )
	{	
		pDestBrws->EnableWindow( false );
		pDestPath->EnableWindow( false );
		pDestPath->SetReadOnly( true );
		
		pBtnAnim->SetCheck( true );
		pBtnAnim->EnableWindow( false );

		pBtnRepl->ShowWindow( false );
		pBtnScale->ShowWindow( false );
		m_bReplaceExistingFiles = true;

		pBtnModels->EnableWindow( false );
		pBtnScenes->EnableWindow( false );
		pBtnA->EnableWindow( false );
		pBtnB->EnableWindow( false );
	}
	else
	{
		pBtnNormals->ShowWindow( false );
		pBtnAnim->ShowWindow( false );
	}

	pDestPath->SetWindowText( m_strDestinationPath );

	pSrcFiles->ResetContent();

	if( m_vecSrcFiles.empty() == false )
	{
		for( UINT i = 0; i < m_vecSrcFiles.size(); i++ )
		{
			pSrcFiles->AddString( m_vecSrcFiles[i] );
		}
	}
}

void CBatchConvertDlg::SHPathToPidl( LPCTSTR szpath, LPITEMIDLIST* ppidl )
{
	LPSHELLFOLDER	pShellFolder		= NULL;
	OLECHAR			wszPath[MAX_PATH]	= {0};
	ULONG			nCharsParsed		= 0;

	HRESULT hr = SHGetDesktopFolder( &pShellFolder );

	if( FAILED(hr) )
		return;

	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szpath, -1, wszPath, MAX_PATH );

	pShellFolder->ParseDisplayName( NULL, NULL, wszPath, &nCharsParsed, ppidl, NULL );
	pShellFolder->Release();
}

void CBatchConvertDlg::OnBnClickedBtnDestPathBrws()
{
	LPITEMIDLIST	pidl = NULL;
	BROWSEINFO		bi;

	ZeroMemory( &bi, sizeof(BROWSEINFO) );
	bi.lpszTitle = __TEXT( "Выберите папку:" );
	bi.hwndOwner = this->GetSafeHwnd();

	m_strDestinationPath.Trim();

	if( m_strDestinationPath.GetLength() > 0 )
	{
		if( !PathFileExists(m_strDestinationPath) )
			SHGetSpecialFolderLocation( this->GetSafeHwnd(), CSIDL_DRIVES, &pidl );
		else
			SHPathToPidl( m_strDestinationPath, &pidl );

	}
	else
	{
		SHGetSpecialFolderLocation( this->GetSafeHwnd(), CSIDL_DRIVES, &pidl );
	}

	bi.pidlRoot = pidl;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;

	pidl = SHBrowseForFolder( &bi );

    if( pidl != NULL )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if( SHGetPathFromIDList ( pidl, path ) )
        {
            m_strDestinationPath = path;
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

void CBatchConvertDlg::ProcessMessages(void)
{
	MSG msg;

	for( int t = 0; t < 20; t++ )
	{
		if(PeekMessage( &msg, this->GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
		{
 			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}		
}

void CBatchConvertDlg::OnEnChangeEditDestDir()
{
	CWnd* pWnd = GetDlgItem( IDC_EDIT_DEST_DIR );
	pWnd->GetWindowText( m_strDestinationPath );
}

void CBatchConvertDlg::AddLogString( CString strValue )
{
	((CCookieMachineDlg*)m_pParent)->ProcessMessagesGlob();
	ProcessMessages();

	CListBox* pLogBox = (CListBox*)GetDlgItem( IDC_LST_LOG );

	pLogBox->AddString( strValue );

	int nCount = pLogBox->GetCount();

	if( nCount > 0 )
		pLogBox->SetCurSel( nCount - 1 );
}

void CBatchConvertDlg::UpdateLastLogString( CString strValue )
{
	((CCookieMachineDlg*)m_pParent)->ProcessMessagesGlob();
	ProcessMessages();

	CListBox* pLogBox = (CListBox*)GetDlgItem( IDC_LST_LOG );

	int nCount = pLogBox->GetCount();

	pLogBox->DeleteString( nCount - 1 );

	AddLogString( strValue );
}

void CBatchConvertDlg::ConvertXSI2AnimEFN_File( CString strSrcPath, CString strDestFolder, bool bOverwrite, bool bScale )
{
	string	file_dir, file_name;
	bool	bFileSuccessfullyOpened = false;
	
	AddLogString( strSrcPath + " ... loading" );

	file_dir	= strSrcPath.Left( strSrcPath.ReverseFind('\\') + 1 ).GetBuffer();
	file_name	= strSrcPath.Right( strSrcPath.GetLength() - strSrcPath.ReverseFind('\\') - 1 ).GetBuffer();
	
	CString	strNewFileName = (file_name.substr( 0, file_name.length() - 3 ) + "efn").data();

	if( PathFileExists( strDestFolder + strNewFileName ) )
	{
		if( bOverwrite )
		{
			DeleteFile( strDestFolder + strNewFileName );
		}
		else
		{
			UpdateLastLogString( strSrcPath + " -- File Exists" );
			return;
		}
	}

	CXSIFile* pXsiFile = new CXSIFile( m_pParent, NULL );
	
	if( pXsiFile->LoadFromFile( file_dir, file_name, ((CCookieMachineDlg*)m_pParent)->GetAppSettingsGlob()->RepairXSIFiles ) )
	{
		pXsiFile->SetPaths( file_dir, file_name );

		bFileSuccessfullyOpened = true;

		UpdateLastLogString( strSrcPath + " ... converting" );

		if( m_bctType == BCT_XSI2EFN )
		{
			CEfnCharacter* efn_character = new CEfnCharacter;

			if( !efn_character->InitFromXSICharacter( pXsiFile ) )
			{
				UpdateLastLogString( strSrcPath + " -- Convertation Failure" );
			}
			else
			{
				if( bScale )
				{
					D3DXMATRIX	scale;
					float		fScaleCoef = 1.0f / 16.5289f;
					D3DXMatrixScaling( &scale, fScaleCoef, fScaleCoef, fScaleCoef );
					efn_character->Scale( scale );
				}

				efn_character->SaveToFile( (strDestFolder + strNewFileName).GetBuffer() );

				UpdateLastLogString( strSrcPath + " -- OK" );
			}

			SAFE_DELETE( efn_character );
		}
		else if( m_bctType == BCT_XSI2MESHLIST )
		{
			CEfnMeshlist* efn_meshlist = new CEfnMeshlist;

			if( !efn_meshlist->InitFromXSICharacter( pXsiFile ) )
			{
				UpdateLastLogString( strSrcPath + " -- Convertation Failure" );
			}
			else
			{
				if( bScale )
				{
					D3DXMATRIX	scale;
					float		fScaleCoef = 1.0f / 16.5289f;
					D3DXMatrixScaling( &scale, fScaleCoef, fScaleCoef, fScaleCoef );
					efn_meshlist->Scale( scale );
				}

				efn_meshlist->SaveToFile( (strDestFolder + strNewFileName).GetBuffer() );

				UpdateLastLogString( strSrcPath + " -- OK" );
			}

			SAFE_DELETE( efn_meshlist );
		}
	}
	else
	{
		UpdateLastLogString( strSrcPath + " -- Load Failure" );
	}

	SAFE_DELETE( pXsiFile );
}

void CBatchConvertDlg::Errorcheck_FileEFN( CString strSrcPath, bool bCorrectNormals )
{
	string	file_dir, file_name;
	
	AddLogString( strSrcPath + " ... loading" );

	file_dir	= strSrcPath.Left( strSrcPath.ReverseFind('\\') + 1 ).GetBuffer();
	file_name	= strSrcPath.Right( strSrcPath.GetLength() - strSrcPath.ReverseFind('\\') - 1 ).GetBuffer();

	if( GetEfnFiletype( strSrcPath.GetBuffer() ) == EF_ANIMATION )
	{
		CEfnCharacter* efn_character = new CEfnCharacter;

		if( efn_character->LoadFromFile( file_dir + file_name ) )
		{
			if( efn_character->GetFramesCount() > 0 )
			{
				DeleteFile( strSrcPath );

				efn_character->SetParentWnd( m_pParent );
				efn_character->SetPaths( file_dir, file_name );

				efn_character->SetCompressionType( ECT_SHORT );
				efn_character->SaveToFile( strSrcPath.GetBuffer(), true );

				UpdateLastLogString( strSrcPath + " -- OK" );
			}
		}
		else
		{
			UpdateLastLogString( strSrcPath + " -- Load Failure" );			
		}

		SAFE_DELETE( efn_character ); 
	}
	else
	{
		CEfnMeshlist* efn_meshlist = new CEfnMeshlist;	

		if( efn_meshlist->LoadFromFile( file_dir + file_name ) )
		{
			DeleteFile( strSrcPath );

			efn_meshlist->SetParent( m_pParent );
			efn_meshlist->SetPaths( file_dir, file_name );

			UpdateLastLogString( strSrcPath + " ... correcting" );

			for( int i = 0; i < efn_meshlist->GetMeshesCount(); i++ )
			{
				CMesh* pMesh = efn_meshlist->GetMeshByIndex(i);
				//pMesh->SetMaterial( mat );

				//normals
				if( bCorrectNormals )
					pMesh->CalcReverseIndex();
			}

			UpdateLastLogString( strSrcPath + " -- OK" );


			efn_meshlist->SaveToFile( strSrcPath.GetBuffer(), true );		
		}
		else
		{
			UpdateLastLogString( strSrcPath + " -- Load Failure" );
		}

		SAFE_DELETE( efn_meshlist ); 
	}
}

void CBatchConvertDlg::Errorcheck_FileSCN( CString strSrcPath, bool bCorrectNormals )
{
	string	file_dir, file_name;
	
	AddLogString( strSrcPath + " ... loading" );

	file_dir	= strSrcPath.Left( strSrcPath.ReverseFind('\\') + 1 ).GetBuffer();
	file_name	= strSrcPath.Right( strSrcPath.GetLength() - strSrcPath.ReverseFind('\\') - 1 ).GetBuffer();

	
	CSceneFile* scn_file = new CSceneFile;

	scn_file->SetPaths( file_dir, file_name );

	if( scn_file->LoadFromFile( file_dir + file_name ) )
	{
		DeleteFile( strSrcPath );

		UpdateLastLogString( strSrcPath + " ... correcting" );

		scn_file->SaveToFile( strSrcPath.GetBuffer() );

		UpdateLastLogString( strSrcPath + " -- OK" );
	}
	else
	{
		UpdateLastLogString( strSrcPath + " -- Load Failure" );			
	}

	SAFE_DELETE( scn_file ); 	


	////material to white
	//D3DMATERIAL9 mat;

	//ZeroMemory( &mat, sizeof(D3DMATERIAL9) );

	//mat.Ambient.a = 1.0f;
	//mat.Ambient.r = 1.0f;
	//mat.Ambient.g = 1.0f;
	//mat.Ambient.b = 1.0f;

	//mat.Diffuse.a = 1.0f;
	//mat.Diffuse.r = 1.0f;
	//mat.Diffuse.g = 1.0f;
	//mat.Diffuse.b = 1.0f;


	if( GetEfnFiletype( strSrcPath.GetBuffer() ) == EF_ANIMATION )
	{
		CEfnCharacter* efn_character = new CEfnCharacter;

		if( efn_character->LoadFromFile( file_dir + file_name ) )
		{
			if( efn_character->GetFramesCount() > 0 )
			{
				DeleteFile( strSrcPath );

				efn_character->SetParentWnd( m_pParent );
				efn_character->SetPaths( file_dir, file_name );

	//		/*	D3DXVECTOR3 MINV, MAXV;
	//			efn_character->GetMinMaxDimentions( MINV, MAXV );

	//			D3DXVECTOR3 resV = MAXV - MINV;

	//			resV.x = fabs( resV.x );
	//			resV.y = fabs( resV.y );
	//			resV.z = fabs( resV.z );

	//			float local_dim = resV.x;

	//			if( local_dim < resV.y )
	//				local_dim = resV.y;

	//			if( local_dim < resV.z )
	//				local_dim = resV.z;

	//			if( Rfile == "" )
	//			{
	//				Rfile		= file_dir + file_name;
	//				max_coord	= local_dim;
	//			}
	//			else
	//			{
	//				if( local_dim > max_coord )
	//				{
	//					max_coord	= local_dim;
	//					Rfile		= file_dir + file_name;
	//				}
	//			}*/

	//			UpdateLastLogString( strSrcPath + " ... correcting" );
	//			//efn_character->SetMaterial( mat );

	//			//normals
	//			//if( bCorrectNormals )
	//			//	efn_character->CalcReverseIndex();

	//			//if( efn_character->IsAnimsRangeOK() )
	//			//{
	//			//	UpdateLastLogString( strSrcPath + " -- OK" );
	//			//}
	//			//else
	//			//{
	//			//	UpdateLastLogString( strSrcPath + " -- OK (animations out of range)" );
	//			//}
				efn_character->SetCompressionType( ECT_SHORT );
				efn_character->SaveToFile( strSrcPath.GetBuffer(), true );

				UpdateLastLogString( strSrcPath + " -- OK" );
			}
		}
		else
		{
			UpdateLastLogString( strSrcPath + " -- Load Failure" );			
		}

		SAFE_DELETE( efn_character ); 
	}
	//else
	//{
	//	return;
	//}
	
	//	CEfnMeshlist* efn_meshlist = new CEfnMeshlist;	

	//	if( efn_meshlist->LoadFromFile( file_dir + file_name ) )
	//	{
	//		DeleteFile( strSrcPath );

	//		efn_meshlist->SetParent( m_pParent );
	//		efn_meshlist->SetPaths( file_dir, file_name );

	//		UpdateLastLogString( strSrcPath + " ... correcting" );

	//		for( int i = 0; i < efn_meshlist->GetMeshesCount(); i++ )
	//		{
	//			CMesh* pMesh = efn_meshlist->GetMeshByIndex(i);
	//			pMesh->SetMaterial( mat );

	//			//normals
	//			if( bCorrectNormals )
	//				pMesh->CalcReverseIndex();
	//		}

	//		UpdateLastLogString( strSrcPath + " -- OK" );


	//		efn_meshlist->SaveToFile( strSrcPath.GetBuffer(), true );		
	//	}
	//	else
	//	{
	//		UpdateLastLogString( strSrcPath + " -- Load Failure" );
	//	}

	//	SAFE_DELETE( efn_meshlist ); 
	//}
}

void CBatchConvertDlg::Errorcheck()
{
	CButton*	pBtnNormals = (CButton*)GetDlgItem( IDC_CHK_NORMALS );
	CListBox*	pLogBox		= (CListBox*)GetDlgItem( IDC_LST_LOG );

	if( m_vecSrcFiles.empty() )
	{
		pLogBox->AddString( "No files selected!" );
		return;
	}

	for( UINT i = 0; i < m_vecSrcFiles.size(); i++ )
	{
		if( m_vecSrcFiles[i].Right( 3 ).MakeUpper() == "SCN" )
		{
			Errorcheck_FileSCN( m_vecSrcFiles[i].GetBuffer(), (bool)pBtnNormals->GetCheck() );
		}
		else if( m_vecSrcFiles[i].Right( 3 ).MakeUpper() == "EFN" )
		{
			Errorcheck_FileEFN( m_vecSrcFiles[i].GetBuffer(), (bool)pBtnNormals->GetCheck() );
		}
	}

	pLogBox->AddString( "" );

	//CString strTemp;
	//strTemp.Format( "%s\r\n, %f ", Rfile, max_coord );

	//AfxMessageBox( strTemp );
}

void CBatchConvertDlg::ConvertXSI2AnimEFN()
{
	CButton*	pBtnRepl	= (CButton*)GetDlgItem( IDC_CHK_REPLACE_EXISTING );
	CButton*	pBtnScale	= (CButton*)GetDlgItem( IDC_CHK_SHULTZ_SCALING );
	CListBox*	pLogBox		= (CListBox*)GetDlgItem( IDC_LST_LOG );

	if( m_vecSrcFiles.empty() )
	{
		pLogBox->AddString( "No files selected!" );
		return;
	}

	CEdit*	pDestPath = (CEdit*)GetDlgItem( IDC_EDIT_DEST_DIR );
	CString	strDestpath;
	pDestPath->GetWindowText( strDestpath );

	strDestpath.Trim();

	if( strDestpath == "" )
	{
		pLogBox->AddString( "Destination path is not set!" );
		return;
	}

	if( PathFileExists( strDestpath ) == false )
	{
		pLogBox->AddString( "Destination path does not exist!" );
		return;
	}

	if( strDestpath.Right( 1 ) != '\\' )
		strDestpath += "\\";

	for( UINT i = 0; i < m_vecSrcFiles.size(); i++ )
	{
		ConvertXSI2AnimEFN_File( m_vecSrcFiles[i].GetBuffer(), strDestpath.GetBuffer(), (bool)pBtnRepl->GetCheck(), (bool)pBtnScale->GetCheck() );
	}

	pLogBox->AddString( "" );
}

void CBatchConvertDlg::OnBnClickedOk()
{
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	CListBox* pLogBox = (CListBox*)GetDlgItem( IDC_LST_LOG );
	pLogBox->AddString( "" );

	switch( m_bctType )
	{
		case BCT_XSI2EFN:
			pLogBox->AddString( "------------------ convertation ------------------" );
			ConvertXSI2AnimEFN();
			break;

		case BCT_XSI2MESHLIST:
			pLogBox->AddString( "------------------ convertation ------------------" );
			ConvertXSI2AnimEFN();
			break;

		case BCT_ERRORCHECK:
			pLogBox->AddString( "------------------ error check ------------------" );
			Errorcheck();
			break;
	};

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CBatchConvertDlg::OnBnClickedBtnModels()
{
	if( !m_pParent )
		return;

	const AppSettingsData* pAppSetts = ((CCookieMachineDlg*)m_pParent)->GetAppSettingsGlob();

	m_strDestinationPath = pAppSetts->EngineFolder + "\\data\\models\\";

	UpdateUI();
}

void CBatchConvertDlg::OnBnClickedBtnScenes()
{
	if( !m_pParent )
		return;

	const AppSettingsData* pAppSetts = ((CCookieMachineDlg*)m_pParent)->GetAppSettingsGlob();

	m_strDestinationPath = pAppSetts->EngineFolder + "\\data\\models\\scenes\\";

	UpdateUI();
}

void CBatchConvertDlg::OnBnClickedBtnA()
{
	if( !m_pParent )
		return;

	const AppSettingsData* pAppSetts = ((CCookieMachineDlg*)m_pParent)->GetAppSettingsGlob();

	m_strDestinationPath = pAppSetts->Button_A_Path;

	UpdateUI();
}

void CBatchConvertDlg::OnBnClickedBtnB()
{
	if( !m_pParent )
		return;

	const AppSettingsData* pAppSetts = ((CCookieMachineDlg*)m_pParent)->GetAppSettingsGlob();

	m_strDestinationPath = pAppSetts->Button_B_Path;

	UpdateUI();
}


