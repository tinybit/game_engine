#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "ScriptingDlg.h"
#include "../SceneFile.h"
#include "CookieMachineDlg.h"
#include "TextResourcesDlg.h"
#include ".\scriptingdlg.h"
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/Common.h"


IMPLEMENT_DYNAMIC(CScriptingDlg, CDialog)
CScriptingDlg::CScriptingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptingDlg::IDD, pParent)
{
	m_iOldSelection		= 0;
	m_bJustOpened		= true;
	m_bFirstSearch		= true;
	m_pParent			= pParent;
	m_hAccelerator		= LoadAccelerators( AfxGetResourceHandle(), MAKEINTRESOURCE( IDR_ACCELERATOR_SCRIPT_DLG ) );
	oldItem				= -1;
	m_iMinWindowSizeX	= 947;
	m_iMinWindowSizeY	= 650;
	m_strFilterParType	= "";
	m_strFilterParID	= "";
	m_WndHeight			= -1;
	m_bIsDialogClosing	= false;
	m_bIsRefreshingFuncsBindings = false;

	m_iCurrentlySearchedBind = 0;
	m_strSearchString		 = "";
	m_bSearchCurrBindOnly	 = false;
	m_bSearchCaseSensitive   = false;
}

CScriptingDlg::~CScriptingDlg()
{
}

void CScriptingDlg::SaveAll()
{
	UpdateCurrentScript();
	((CCookieMachineDlg*)m_pParent)->SaveGlob();
}

void CScriptingDlg::RunEngine()
{
	UpdateCurrentScript();
	((CCookieMachineDlg*)m_pParent)->RunEngineGlob();
}

void CScriptingDlg::ShowTextResEditor()
{
	int st	= m_lexer.SendEditor( SCI_GETSELECTIONSTART );
	int end	= m_lexer.SendEditor( SCI_GETSELECTIONEND );

	if( end - st > 0 )
	{
		char* text = new char[end - st + 1];
		m_lexer.SendEditor( SCI_GETSELTEXT, 0, (LPARAM)text );
		str_t buff = text;
		SAFE_DELETE_ARRAY( text );

		CTextResourcesDlg* pTextResEd = (CTextResourcesDlg*)((CCookieMachineDlg*)m_pParent)->GetTextResEditorGlob();

		if( pTextResEd )
			pTextResEd->SetCurrResource( buff );	
	}
	
	((CCookieMachineDlg*)m_pParent)->ShowTextResEditorGlob();
}

void CScriptingDlg::RunEngineCurr()
{
	UpdateCurrentScript();
	((CCookieMachineDlg*)m_pParent)->RunEngineCurrGlob();
}

void CScriptingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LST_OBJECT, m_lstObjects);
	DDX_Control(pDX, IDC_CMB_FILTER_TYPE, m_cmbFilterParentType);
	DDX_Control(pDX, IDC_CMB_FILTER_ID, m_cmbFilterParentID);
	DDX_Control(pDX, IDC_TREE_OBJECTS, m_tvObjects);
	DDX_Control(pDX, IDC_COMBOBOXEX_FUNCS, m_cmbExFuncs);
	DDX_Control(pDX, IDC_COMBOBOXEX_CLASSES, m_cmbExClasses);
}


BEGIN_MESSAGE_MAP(CScriptingDlg, CDialog)
	
	//accelerators
	ON_COMMAND( ID_ACCL_SAVE, SaveAll )
	ON_COMMAND( ID_ACCL_MAXIMIZE, MaximizeWindow )
	ON_COMMAND( ID_ACCL_ENGINE, RunEngine )
	ON_COMMAND( ID_ACCL_FIND, OnBnClickedBtnSearch )
	ON_COMMAND( ID_ACCL_REPLACE, OnBnClickedBtnSearchAndReplace )
	ON_COMMAND( ID_ACCL_FIND_NEXT, ShowNextSearchResult )	
	ON_COMMAND( ID_ACCL_ENGINE_CURR, RunEngineCurr )
	ON_COMMAND( ID_ACCL_COMMENT_BLOCK, CommentSelectedBlock )
	ON_COMMAND( ID_ACCL_UNCOMMENT_BLOCK, UncommentSelectedBlock )
	ON_COMMAND( ID_ACCL_TEXT_RES, ShowTextResEditor )
	ON_COMMAND( ID_ACCL_PARSE_SCRIPTS, ParseScripts )
	ON_COMMAND( ID_ACCELERATOR_LUA_BINDS, ShowLuaBinds )
	ON_COMMAND( ID_ACCL_REFRESH_FUNC_BINDINGS, RefreshFuncsBindings )
	ON_COMMAND( ID_ACCL_GO2DEF, Go2FuncDefinition )

	ON_NOTIFY( SCN_MARGINCLICK, 0, OnMarginClick )
	ON_NOTIFY( SCN_DOUBLECLICK, 0, OnOutputDblClick )

	ON_NOTIFY( SCN_MODIFIED, 0, OnLexerDataModify )
	ON_NOTIFY( LVN_ITEMCHANGED, IDC_LST_OBJECT, OnLvnItemchangedLstObject )
	ON_BN_CLICKED( IDC_BTN_CLOSE, OnBnClickedBtnClose )
	ON_WM_SIZE()

	ON_BN_CLICKED(IDC_BTN_CREATE_HANDLER, OnBnClickedBtnCreateHandler)
	ON_BN_CLICKED(IDC_BTN_CREATE_SET, OnBnClickedBtnCreateSet)
	ON_BN_CLICKED(IDC_BTN_CREATE_FUNCTION, OnBnClickedBtnCreateFunction)
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_SEARCH_AND_REPLACE, OnBnClickedBtnSearchAndReplace)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_CMB_FILTER_TYPE, OnCbnSelchangeCmbFilterType)
	ON_CBN_SELCHANGE(IDC_CMB_FILTER_ID, OnCbnSelchangeCmbFilterId)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OBJECTS, OnTvnSelchangedTreeObjects)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_OBJECTS, OnTvnSelchangingTreeObjects)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_CLASSES, OnCbnSelchangeComboboxexClasses)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_FUNCS, OnCbnSelchangeComboboxexFuncs)
	ON_CBN_CLOSEUP(IDC_COMBOBOXEX_FUNCS, OnCbnCloseupComboboxexFuncs)
	ON_CBN_CLOSEUP(IDC_CMB_FILTER_TYPE, OnCbnCloseupCmbFilterType)
	ON_CBN_CLOSEUP(IDC_CMB_FILTER_ID, OnCbnCloseupCmbFilterId)
END_MESSAGE_MAP()


void CScriptingDlg::MaximizeWindow()
{
	this->ShowWindow( SW_MAXIMIZE );
}

void CScriptingDlg::Go2FuncDefinition()
{
	int		len			= m_lexer.SendEditor( SCI_GETLENGTH );
	char*	src			= new char[len + 1];
	int		iCursorPos	= m_lexer.SendEditor( SCI_GETCURRENTPOS );
	str_t	strScript;

	m_lexer.SendEditor( SCI_GETTEXT, len + 1, (LPARAM)src );

	strScript = src;

	//strScript = strScript.substr( iCursorPos - 3,  );

	int	iCounter = 0;
	while( iCursorPos - iCounter > 0 && IsValidFuncChar( strScript[iCursorPos - iCounter] ) )
		iCounter++;

	int	iCounter2 = 0;
	while( iCursorPos + iCounter2 < (len + 1) && IsValidFuncChar( strScript[iCursorPos + iCounter2] ) )
		iCounter2++;

	if( strScript[iCursorPos + iCounter2] == '(' )
	{
		str_t strFunction = strScript.substr( (iCursorPos - iCounter) + 1, iCounter + iCounter2 - 1 );

		str_t strClass;
		str_t strClassMethod;

		int iOff = -1;
		for( int i = strFunction.size(); i >= 0; i-- )
		{
			if( strFunction[i] == 58 )
			{
				iOff = i;
			}
		}

		if( iOff == -1 )
		{
			strClassMethod = strFunction;
			//AfxMessageBox( strClassMethod.data() );
		}
		else
		{
			strClass		= strFunction.substr( 0, iOff );
			strClassMethod	= strFunction.substr( iOff + 1, strFunction.size() );

			//AfxMessageBox( (strClass + "\r\n" + strClassMethod).data() );
		}

		//go to definition
		if( strClass == "" )
		{
			int iGlobIdx = SearchComboboxEx( &m_cmbExClasses, "(Globals)" );
			m_cmbExClasses.SetCurSel( iGlobIdx );
			PopulateFuncsCombo();
		}
		else
		{
			int iGlobIdx = SearchComboboxEx( &m_cmbExClasses, strClass.data() );

			if( iGlobIdx != -1 )
			{
				m_cmbExClasses.SetCurSel( iGlobIdx );
				PopulateFuncsCombo();
			}
		}

		int iGlobIdx = SearchCombobox( &m_cmbExFuncs, strClassMethod.data() );

		if( iGlobIdx != -1 )
		{
			m_cmbExFuncs.SetCurSel( iGlobIdx );
			OnCbnCloseupComboboxexFuncs();
		}
	}

	SAFE_DELETE_ARRAY( src )
}

bool CScriptingDlg::IsValidFuncChar( char chChar )
{
	if( chChar >= 48 && chChar <= 58 )
		return true;

	if( chChar >= 65 && chChar <= 90 )
		return true;

	if( chChar >= 97 && chChar <= 122 )
		return true;

	if( chChar == 95 || chChar == 46 )
		return true;

	return false;
}

void CScriptingDlg::RefreshFuncsBindings()
{
	if( !m_pParent )
		return;

	UpdateCurrentScript();

	CSceneFile* pScene = (CSceneFile*)((CCookieMachineDlg*)m_pParent)->GetSceneGlob();
	pScene->BuildFuncsList();

	if( !pScene )
		return;

	m_pFuncs = pScene->GetFuncs();

	if( !m_pFuncs )
		return;

	m_bIsRefreshingFuncsBindings = true;

	PopulateObjectsList();
	PopulateFuncsCombo();
	PopulateClassesCombo();

	//restore last bind
	ShowLastSelObject();

	m_bIsRefreshingFuncsBindings = false;
}

UINT CScriptingDlg::GetFileSize( const string& sFileName )
{
	struct _stat kStat;
		
	if ( _stat( sFileName.c_str(), &kStat ) != 0 )
		return 0;

	return kStat.st_size;
}

void CScriptingDlg::ShowLuaBinds()
{
	if( !m_pParent )
		return;

	const AppSettingsData* pSettings = ((CCookieMachineDlg*)m_pParent)->GetAppSettingsGlob();

	if( !pSettings )
		return;

	WIN32_FIND_DATA	findData;
	HANDLE			hFind;
	bool			bMoreFiles = true;
	str_t			strAppPath = ((CCookieMachineDlg*)m_pParent)->GetAppDir();

	hFind = FindFirstFile( pSettings->EngineFolder + "\\data\\Scripts\\lua_binds\\*.*", &findData );
	string resBuff;

	while( hFind != INVALID_HANDLE_VALUE && bMoreFiles )
	{
		if( !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			str_t strTemp = pSettings->EngineFolder;
			//strTemp.assign( , pSettings->EngineFolder.GetLength() ); 
			strTemp += "\\data\\Scripts\\lua_binds\\";
			strTemp += findData.cFileName;

			FILE* pFile = fopen( strTemp.data(), "rb" );

			if( !pFile )
				continue;
		
			
			int iFileSize = GetFileSize( strTemp );

			if( iFileSize <= 0 )
				continue;

			char* pFileBuff = new char[iFileSize];

			if( !pFileBuff )
				continue;

			fread( pFileBuff, iFileSize, 1, pFile );
			fclose( pFile );

			resBuff.append( pFileBuff, iFileSize );

			if( pFileBuff )
				delete []pFileBuff;
		}

		if( FindNextFile( hFind, &findData ) )
			bMoreFiles = true;
		else
			bMoreFiles = false;
	}

	FindClose( hFind );

	str_t editor_dir = ((CCookieMachineDlg*)m_pParent)->GetAppDir();

	editor_dir += "\\temp\\luabinds.cpp";

	FILE* pFile = fopen( editor_dir.data(), "wb" );
	fwrite( resBuff.data(), resBuff.size(), 1, pFile );
	fclose( pFile );

	WinExec( (((CCookieMachineDlg*)m_pParent)->GetAppDir() + "\\utils\\notepad.exe " + editor_dir).data(), SW_SHOWMAXIMIZED );	
}

void CScriptingDlg::JumpToToScript()
{
	//int output_focused = m_lexer_out.SendEditor( SCI_GETFOCUS );

	//if( output_focused != 1 )
	//	return;

	//CString strAppTmpPath = ( ((CCookieMachineDlg*)m_pParent)->GetAppDir() + "\\temp\\" ).data();
	//int		pos		= m_lexer_out.SendEditor( SCI_GETCURRENTPOS );
	//int		line	= m_lexer_out.SendEditor( SCI_LINEFROMPOSITION, pos );
	//int		end		= m_lexer_out.SendEditor( SCI_GETLINEENDPOSITION, line );
	//int		start	= m_lexer_out.SendEditor( SCI_POSITIONFROMLINE, line );
	//CString	output_line, strTemp;

	//if( end - start <= 0 )
	//	return;

	//if( scripts_map[line] == "" && txtres_map[line] == "" )
	//	return;

	//char* buff = new char[end - start];
	//m_lexer_out.SendEditor( SCI_GETCURLINE, end - start, (LPARAM)buff );
	//output_line = buff;
	//delete []buff;

	////go to error in script
	//if( scripts_map[line] != "" )
	//{
	//	//select script
	//	int objsel = -1;
	//	LVFINDINFO info;
	//	info.flags = LVFI_STRING;
	//	info.psz = scripts_map[line].data();

	//	strTemp = scripts_map[line].data();

	//	objsel = m_lstObjects.FindItem( &info );

	//	if( objsel == -1 )
	//		return;

	//	m_lstObjects.SetFocus();
	//	m_lstObjects.SetItemState( objsel, LVIS_SELECTED, LVIS_SELECTED );
	//	m_lstObjects.EnsureVisible( objsel, true );

	//	//go to err line
	//	strTemp = output_line.Right( output_line.GetLength() - strTemp.GetLength() );

	//	if( strTemp[0] != ':' )
	//		return;

	//	strTemp = strTemp.Right( strTemp.GetLength() - 1 );
	//	int sec = strTemp.Find( ":", 0 );

	//	if( sec == -1 )
	//		return;

	//	strTemp = strTemp.Left( sec );

	//	int err_line	= atoi( strTemp.GetBuffer() ) - 1;
	//	int err_pos_st	= m_lexer.SendEditor( SCI_POSITIONFROMLINE, err_line );
	//	int err_pos_en	= m_lexer.SendEditor( SCI_GETLINEENDPOSITION, err_line );
	//	m_lexer.SendEditor( SCI_SETSEL, err_pos_st, err_pos_en );
	//	m_lexer.SetFocus();
	//}
	//else if( txtres_map[line] != "" )
	//{
	//	CTextResourcesDlg* pTextResEd = (CTextResourcesDlg*)((CCookieMachineDlg*)m_pParent)->GetTextResEditorGlob();

	//	//go to err line
	//	strTemp = txtres_map[line].data();
	//	strTemp = output_line.Right( output_line.GetLength() - strTemp.GetLength() );

	//	int err_line = -1;

	//	if( strTemp[0] == ':' )
	//	{
	//		strTemp = strTemp.Right( strTemp.GetLength() - 1 );
	//		int sec = strTemp.Find( ":", 0 );

	//		if( sec != -1 )
	//		{
	//			strTemp = strTemp.Left( sec );
	//			err_line = atoi( strTemp.GetBuffer() ) - 1;		
	//		}
	//	}

	//	if( pTextResEd )
	//		pTextResEd->SetCurrResource( txtres_map[line], err_line );
	//		
	//	((CCookieMachineDlg*)m_pParent)->ShowTextResEditorGlob( txtres_map[line], err_line );
	//}
}

afx_msg void CScriptingDlg::OnOutputDblClick( NMHDR* nmhdr, LRESULT* result )
{
	SetTimer( ID_SCRIPTING_TIMER, 100, NULL );
}

afx_msg void CScriptingDlg::OnMarginClick( NMHDR* nmhdr, LRESULT* result )
{
	SCNotification* notify = (SCNotification*)nmhdr;

	const int modifiers		= notify->modifiers;
	const int position		= notify->position;
	const int margin		= notify->margin;
	const int line_number	= (const int)m_lexer.SendEditor( SCI_LINEFROMPOSITION, position, 0 );

	switch( margin )
	{
		case 1:
			m_lexer.SendEditor( SCI_TOGGLEFOLD, line_number, 0 );
			break;
	}
}

void CScriptingDlg::OnBnClickedBtnClose()
{
	OnOK();
}

void CScriptingDlg::ClearTempFolder()
{
	WIN32_FIND_DATA	findData;
	HANDLE			hFind;
	bool			bMoreFiles = true;
	str_t			strAppPath = ((CCookieMachineDlg*)m_pParent)->GetAppDir();

	hFind = FindFirstFile( "temp\\*.*", &findData );
	
	while( hFind != INVALID_HANDLE_VALUE && bMoreFiles )
	{
		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( strcmp( findData.cFileName, "." ) )
				if( strcmp( findData.cFileName, ".." ) )
				{
					//AfxMessageBox( findData.cFileName );
				}
		}
		else
		{
			str_t compl_path = strAppPath + "\\temp\\" + findData.cFileName;
			DeleteFile( compl_path.data() );
		}

		if( FindNextFile( hFind, &findData ) )
			bMoreFiles = true;
		else
			bMoreFiles = false;
	}

	FindClose( hFind );
}

void CScriptingDlg::ScrollOutput()
{
	//POINT pt;
	//pt.y = 999;

	//RichEditEx* pWnd = (RichEditEx*)GetDlgItem( IDC_RICHEDIT_OUT );
	//pWnd->SendMessage( EM_SETSCROLLPOS, 0, (LPARAM)&pt );
	//pWnd->SendMessage( WM_VSCROLL, 0, 0 );
	//pWnd->SendMessage( WM_LBUTTONDOWN, 0, MAKELPARAM( 16, 16 ) );
	//pWnd->SendMessage( WM_LBUTTONUP, 0, MAKELPARAM( 16, 16 ) );
}

void CScriptingDlg::ParseScripts()
{
	CString out = "";
	int		iScriptsErrorsCount	= 0;
	int		iTxtResErrorsCount	= 0;
	str_t	strTemp;
	int		iLine = 6;

	m_lexer_out.SendEditor( SCI_SETREADONLY, false );

	m_lexer_out.SendEditor( SCI_SETTEXT, 0, (LPARAM)"" );
	UpdateCurrentScript();

	scripts_map.clear();
	txtres_map.clear();

	out = "Deleting intermediate files...\r\n\r\n";
	m_lexer_out.SendEditor( SCI_APPENDTEXT, out.GetLength(), (LPARAM)out.GetBuffer() );
	m_lexer_out.SendEditor( SCI_LINESCROLL, 0, 10 );

	ClearTempFolder();

	out = "------------------------ Parsing started: ------------------------\r\n\r\n";
	out += "Scripts...\r\n\r\n";

	m_lexer_out.SendEditor( SCI_APPENDTEXT, out.GetLength(), (LPARAM)out.GetBuffer() );
	m_lexer_out.SendEditor( SCI_LINESCROLL, 0, 10 );


	m_lexer_out.SendEditor( SCI_SETREADONLY, true );

	StrVec objects;

	//for( int i = 0; i < m_lstObjects.GetItemCount(); i++ )
	//{
	//	CString strItem	= m_lstObjects.GetItemText( i, 0 );
	//	CString	script	= GetItemScript( strItem.GetBuffer() ).data();
	//
	//	if( script.Trim() == "" )
	//		continue;

	//	strTemp = strItem.GetBuffer();
	//	if( StripTypeHeader( strTemp ) == "LayerGeom" )
	//		script = "LayerGeom_variable = (" + script + ")";

	//	script.Replace( "\n", "" );
	//	script.Replace( "\r", "\r\n" );

	//	CString strFilePath = ( ((CCookieMachineDlg*)m_pParent)->GetAppDir() + "\\temp\\" + strItem.GetBuffer() ).data();

	//	FILE*	file = fopen( strFilePath, "w" );
	//	fprintf( file, "%s", script.GetBuffer() );
	//	fclose( file );

	//	CString appDir = ((CCookieMachineDlg*)m_pParent)->GetAppDir().data();
	//	CString strParseRes = ExecuteExternalFile( appDir + "\\utils\\luac.exe", "\"" + strFilePath + "\"" );
	//	strParseRes.Trim();

	//	if( strParseRes != "" )
	//	{
	//		strParseRes = strParseRes.Right( strParseRes.GetLength() - 15 - ((CCookieMachineDlg*)m_pParent)->GetAppDir().length() - 1 );
	//		out = strParseRes + "\r\n";
	//		iScriptsErrorsCount++;
	//	}
	//	else
	//		out = strItem + " -- OK" + "\r\n";

	//	scripts_map[iLine] = strItem.GetBuffer();
	//	iLine++;

	//	m_lexer_out.SendEditor( SCI_SETREADONLY, false );
	//	m_lexer_out.SendEditor( SCI_APPENDTEXT, out.GetLength(), (LPARAM)out.GetBuffer() );
	//	m_lexer_out.SendEditor( SCI_LINESCROLL, 0, 10 );
	//	m_lexer_out.SendEditor( SCI_SETREADONLY, true );
	//}

	//m_lexer_out.SendEditor( SCI_SETREADONLY, false );
	//out = "\r\nText resources...\r\n\r\n";
	//m_lexer_out.SendEditor( SCI_APPENDTEXT, out.GetLength(), (LPARAM)out.GetBuffer() );
	//m_lexer_out.SendEditor( SCI_LINESCROLL, 0, 10 );
	//m_lexer_out.SendEditor( SCI_SETREADONLY, true );

	//ClearTempFolder();

	//CSceneFile* scene = (CSceneFile*)((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	//iLine += 3;

	//for( int i = 0; i < scene->GetTextResourceCount(); i++ )
	//{
	//	TextResItem* tResItem = scene->GetTextResourceByIndex( i );
	//	
	//	CString strItem	= tResItem->id.data();
	//	CString script	= tResItem->data.data();
	//	script.Trim();

	//	CString strFilePath = ( ((CCookieMachineDlg*)m_pParent)->GetAppDir() + "\\temp\\" + strItem.GetBuffer() ).data();

	//	if( script.Left( 5 ) == "--LUA" )
	//	{
	//		script.Replace( "\n", "" );
	//		script.Replace( "\r", "\r\n" );

	//		FILE*	file = fopen( strFilePath, "w" );
	//		fprintf( file, "%s", script.GetBuffer() );
	//		fclose( file );
	//	}
	//	else
	//		continue;

	//	CString appDir = ((CCookieMachineDlg*)m_pParent)->GetAppDir().data();

	//	CString strParseRes = ExecuteExternalFile( appDir + "\\utils\\luac.exe", "\"" + strFilePath + "\"" );
	//	strParseRes.Trim();

	//	if( strParseRes != "" )
	//	{
	//		strParseRes = strParseRes.Right( strParseRes.GetLength() - 15 - ((CCookieMachineDlg*)m_pParent)->GetAppDir().length() - 1 );
	//		out = strParseRes + "\r\n";
	//		iTxtResErrorsCount++;
	//	}
	//	else
	//		out = strItem + " -- OK" + "\r\n";

	//	txtres_map[iLine] = strItem.GetBuffer();
	//	iLine++;

	//	m_lexer_out.SendEditor( SCI_SETREADONLY, false );
	//	m_lexer_out.SendEditor( SCI_APPENDTEXT, out.GetLength(), (LPARAM)out.GetBuffer() );
	//	m_lexer_out.SendEditor( SCI_LINESCROLL, 0, 10 );
	//	m_lexer_out.SendEditor( SCI_SETREADONLY, true );
	//}


	//m_lexer_out.SendEditor( SCI_SETREADONLY, false );
	//out = "\r\n------------------------------ Done ------------------------------\r\n\r\n";

	//CString err;
	//err.Format( "      Parsing: %d scripts failed, %d text resources failed\r\n ", iScriptsErrorsCount, iTxtResErrorsCount );
	//out += err;

	//m_lexer_out.SendEditor( SCI_APPENDTEXT, out.GetLength(), (LPARAM)out.GetBuffer() );
	//m_lexer_out.SendEditor( SCI_LINESCROLL, 0, 10 );
	//m_lexer_out.SendEditor( SCI_SETREADONLY, true );
}

CString CScriptingDlg::ExecuteExternalFile( CString csExeName, CString csArguments )
{
	CString csExecute = csExeName + " " + csArguments;
  
	SECURITY_ATTRIBUTES secattr; 
	ZeroMemory( &secattr, sizeof(secattr) );
	secattr.nLength			= sizeof( secattr );
	secattr.bInheritHandle	= TRUE;

	HANDLE rPipe, wPipe;

	//Create pipes to write and read data
	CreatePipe( &rPipe, &wPipe, &secattr, 0 );

	STARTUPINFO			sInfo;
	PROCESS_INFORMATION	pInfo; 
	ZeroMemory( &sInfo, sizeof(sInfo) );  
	ZeroMemory( &pInfo, sizeof(pInfo) );
	sInfo.cb		=	sizeof(sInfo);
	sInfo.dwFlags	=	STARTF_USESTDHANDLES;
	sInfo.hStdInput	=	NULL; 
	sInfo.hStdOutput=	wPipe; 
	sInfo.hStdError	=	wPipe;

	char command[1024];
	strcpy( command, csExecute.GetBuffer(csExecute.GetLength()) );

	//Create the process here.
	CreateProcess( 0, command, 0, 0, TRUE, NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW, 0, 0, &sInfo, &pInfo );
	CloseHandle( wPipe );

	//now read the output pipe here.
	char	buf[100];
	DWORD	reDword; 
	CString	m_csOutput;
	CString	csTemp;
	BOOL	res;

	do
	{
		res			=	::ReadFile( rPipe, buf, 100, &reDword, 0 );
		csTemp		=	buf;
		m_csOutput	+=	csTemp.Left( reDword );
	} while( res );

	return m_csOutput;
}

//void CScriptingDlg::ClearFuncsList()
//{
//	if( m_vecFuncs.empty() )
//		return;
//
//	for( UINT i = 0; i < m_vecFuncs.size(); i++ )
//	{
//		SAFE_DELETE( m_vecFuncs[i] );
//	}
//
//	m_vecFuncs.clear();
//}

//void CScriptingDlg::BuildFuncsList()
//{
//	if( !m_pBinds )
//		return;
//
//	ListStrVec_iter iter = m_pBinds->begin();
//
//	while( iter != m_pBinds->end() )
//	{
//		CString script_str = "";
//
//		if( (*iter).size() <= 5 )
//		{
//			iter++;
//			continue;
//		}
//
//		script_str = (*iter)[5].data();
//		script_str = script_str.Trim();
//
//		if( script_str.GetLength() == 0 )
//		{
//			iter++;
//			continue;
//		}
//		
//		int offset = script_str.Find( "function", 0 );
//
//		while( offset != -1 )
//		{
//			int BaseOffset = offset;
//
//			offset += 8;
//
//			bool found = false;
//
//			for( int g = offset; g < script_str.GetLength(); g++ )
//			{
//				if( script_str[g] != ' ' )
//				{
//					found = true;
//					offset = g;
//					break;
//				}
//			}
//
//			int		start	= offset;
//			int		end		= 0;
//			CString strFuncName;
//
//			if( found && script_str.Find( "()", offset ) != -1 )
//			{
//				strFuncName = script_str.Mid( offset, script_str.Find( "()", offset ) );
//
//				strFuncName.Trim();
//				strFuncName = strFuncName.Left( strFuncName.Find( '(', 0 ) );
//				
//				FuncEntry* pFuncEntry	= new FuncEntry;
//
//				pFuncEntry->strClassName	= "";
//				pFuncEntry->strFuncName		= "";
//
//				int iClOffset = strFuncName.Find( ':', 0 );
//
//				if( iClOffset != -1 )
//				{
//					pFuncEntry->strClassName	= strFuncName.Left( iClOffset );
//					pFuncEntry->strFuncName		= strFuncName.Right( strFuncName.GetLength() - iClOffset );
//				}
//				else
//				{
//					pFuncEntry->strFuncName = strFuncName;
//				}
//
//				pFuncEntry->iBind		= atoi((*iter)[0].data());
//				pFuncEntry->iOffset		= BaseOffset;
//
//				m_mapFuncs.insert( funcsmap_pair( pFuncEntry->iBind, pFuncEntry ) );
//
//				offset = script_str.Find( "function", offset + strFuncName.GetLength() );
//			}
//			else
//				offset = -1;
//		} // while( offset != -1 )
//
//	} // while( iter != m_pBinds->end() )
//}

void CScriptingDlg::PopulateClassesCombo()
{
	m_cmbExClasses.ResetContent();

	int iCounter = 0;

	COMBOBOXEXITEM     cbi;
	CString            str;

	cbi.mask = CBEIF_TEXT;
	cbi.iItem = iCounter;
	cbi.pszText = (LPTSTR)(LPCTSTR)"(Globals)";
	cbi.cchTextMax = 9;

	iCounter = m_cmbExClasses.InsertItem( &cbi );

	CStringList strClasses;

	for( IFuncMap_iter i = m_pFuncs->begin(); i != m_pFuncs->end(); ++i )
	{
		if( strClasses.GetCount() == 0 )
			strClasses.AddTail( i->second->strClassName );
		else
		{
			bool found = false;

			for( int j = 0; j < strClasses.GetCount(); j++ )
			{
				POSITION pos = strClasses.FindIndex( j );

				if( strClasses.GetAt( pos ) == i->second->strClassName || i->second->strClassName == "" )
				{
					found = true;
					break;
				}
			}

			if( !found )
				strClasses.AddTail( i->second->strClassName );
		}
	}
	
	for( int i = 0; i < (int)strClasses.GetCount(); i++ )
	{
		POSITION pos = strClasses.FindIndex( i );

		cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
		cbi.iItem = iCounter;
		cbi.pszText = (LPTSTR)(LPCTSTR)strClasses.GetAt( pos );
		cbi.cchTextMax = strClasses.GetAt( pos ).GetLength();
		cbi.iImage = 4;
		cbi.iSelectedImage = 4;
		cbi.iOverlay = 4;
		cbi.iIndent = (iCounter & 0x03);   //Set indentation according to item position

		iCounter = m_cmbExClasses.InsertItem( &cbi );
	}

	m_cmbExClasses.SetCurSel(0);
}

int	CScriptingDlg::SearchCombobox( CComboBox* pComboBox, CString strSearchString )
{
	CString strSS = strSearchString;
	//strSS = strSS.MakeUpper();

	for( int i = 0; i < pComboBox->GetCount(); i++ )
	{
		CString strItem;
		int n = pComboBox->GetLBTextLen( i );
		pComboBox->GetLBText( i, strItem.GetBuffer(n) );

		//strItem	= strItem.MakeUpper();

		if( strSS == strItem )
			return i;
	}

	return -1;
}

int CScriptingDlg::SearchComboboxEx( CComboBoxEx* pComboBoxEx, CString strSearchString )
{
	CString strSS = strSearchString;
	//strSS = strSS.MakeUpper();

	for( int i = 0; i < pComboBoxEx->GetCount(); i++ )
	{
		CString strItem;
		int n = pComboBoxEx->GetLBTextLen( i );
		pComboBoxEx->GetLBText( i, strItem.GetBuffer(n) );

		//strItem	= strItem.MakeUpper();

		if( strSS == strItem )
			return i;
	}

	return -1;
}

void CScriptingDlg::PopulateFuncsCombo()
{
	m_cmbExFuncs.ResetContent();

	int iCounter = 0;

	COMBOBOXEXITEM     cbi;
	CString            str;

	cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
	cbi.iImage = 3;
	cbi.iSelectedImage = 3;
	cbi.iOverlay = 3;

	int n = m_cmbExClasses.GetLBTextLen( m_cmbExClasses.GetCurSel() );
	m_cmbExClasses.GetLBText( m_cmbExClasses.GetCurSel(), str.GetBuffer(n) );

	if( str == "(Globals)" )
	{
		str.ReleaseBuffer();
		str = "";
	}

	//vector<string> v1;

	for( IFuncMap_iter i = m_pFuncs->begin(); i != m_pFuncs->end(); ++i )
	{
		if( i->second->strClassName == str )
		{
			m_cmbExFuncs.AddString( i->second->strFuncName );
			//v1.push_back( i->second->strFuncName.GetBuffer() );
		}
	}

	//sort( v1.begin(), v1.end(), greater<string>() );

	//if( v1.empty() == false )
	//{
	//	for( UINT i = 0; i < v1.size(); i++ )
	//	{
	//		CString f = v1[i].data();

	//		cbi.iItem = iCounter;
	//		cbi.pszText = (LPTSTR)(LPCTSTR)f;
	//		cbi.cchTextMax = f.GetLength();
	//		cbi.iIndent = (iCounter & 0x03);   //Set indentation according to item position

	//		iCounter = m_cmbExFuncs.InsertItem( &cbi );
	//	}
	//}

	str.ReleaseBuffer();

	m_cmbExFuncs.SetCurSel(0);
}

void CScriptingDlg::OnCbnSelchangeComboboxexClasses()
{
	PopulateFuncsCombo();
}

void CScriptingDlg::OnCbnSelchangeComboboxexFuncs()
{

}

void CScriptingDlg::PopulateObjectsList()
{
	if( !m_pBinds )
		return;

	m_tvObjects.ReleaseTreeData();
	m_tvObjects.DeleteAllItems();

	//ClearFuncsList();

	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		bool ok = ((*iter)[2] == (str_t)m_strFilterParType.GetBuffer()) && ((*iter)[1] == (str_t)m_strFilterParID.GetBuffer());
		
		if( ok == false )
		{
			iter++;
			continue;
		}

		CString script_str = "";

		if( (*iter).size() > 5 )
		{
			script_str = (*iter)[5].data();
			script_str = script_str.Trim();
		}

		//if( script_str == "" )
		//{
		HTREEITEM hPar = m_tvObjects.FindTreeItemByName( (*iter)[4].data() );

		if( hPar == NULL )
			hPar = m_tvObjects.InsertItem2( (*iter)[4].data(), 2, NULL, NULL, TVI_SORT );

		if( script_str == "" )
			m_tvObjects.InsertItem2( (*iter)[3].data(), 0, NULL, hPar, TVI_SORT );
		else
		{
			HTREEITEM hObj = m_tvObjects.InsertItem2( (*iter)[3].data(), 1, NULL, hPar, TVI_SORT );

			int u = atoi((*iter)[0].data());

			//insert functions
			pair<IFuncMap_iter, IFuncMap_iter> func_entries;
			func_entries = m_pFuncs->equal_range( atoi((*iter)[0].data()) );

			while( func_entries.first != func_entries.second )
			{
				m_tvObjects.InsertItem2( (*func_entries.first->second).strFuncName, 3, new TNODE(func_entries.first->second), hObj, TVI_SORT );	
				func_entries.first++;
			}
		}

		iter++;
	}
}

BOOL CScriptingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	SetIcon( m_hIcon, TRUE );

	CWnd* pWnd = GetDlgItem( IDC_OUTPUT_FRM );

	m_lexer.Load( this->GetSafeHwnd(), ((CCookieMachineDlg*)m_pParent)->GetAppDir(), "Script Source" );
	m_lexer.SetFocus();

	m_lexer_out.Load( this->GetSafeHwnd(), ((CCookieMachineDlg*)m_pParent)->GetAppDir(), "Output" );
	m_lexer_out.SetOutputLex();

	/* LISTBOX CONTROL */
	//prepare imagelist
	CBitmap bitmap;
	m_ImageList.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );
	

	//0 empty
	bitmap.LoadBitmap( IDB_EMPTY );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();	
	
	//1 script
	bitmap.LoadBitmap( IDB_SCRIPT );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();

	//2 group
	bitmap.LoadBitmap( IDB_GROUP );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();

	//3 function
	bitmap.LoadBitmap( IDB_FUNCTION );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();

	//4 class
	bitmap.LoadBitmap( IDB_CLASS );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();

	m_tvObjects.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_tvObjects.DeleteAllItems();

	m_cmbExFuncs.ResetContent();
	m_cmbExClasses.SetImageList( &m_ImageList );
	m_cmbExClasses.ResetContent();

	if( !m_pBinds )
		return TRUE;

	FillFilterCombos();
	PopulateObjectsList();

	//setup V splitter
	CRect rcSliderV;
	CWnd* pWndSliderV = GetDlgItem( IDC_SLIDER );

	pWndSliderV->GetWindowRect( &rcSliderV );
	ScreenToClient( &rcSliderV );
	m_wndSplitter.Create( WS_CHILD | WS_VISIBLE, rcSliderV, this, IDC_SLIDER );
	m_wndSplitter.SetRange( 0, 200, -1 );

	//setup H splitter
	CRect rcSliderH;
	CWnd* pWndSliderH = GetDlgItem( IDC_SLIDERH );

	pWndSliderH->GetWindowRect( &rcSliderH );
	ScreenToClient( &rcSliderH );
	m_wndSplitterH.Create( WS_CHILD | WS_VISIBLE, rcSliderH, this, IDC_SLIDERH );
	m_wndSplitterH.SetRange( 300, 50, -1 );

	//m_wndSplitterH.Create( WS_CHILD | WS_VISIBLE, rc, this, IDC_SLIDER_H );
	//m_wndSplitterH.SetRange( 0, 200, -1 );


	//init event handlers combo
	CComboBox* pCmbHandlers = (CComboBox*)GetDlgItem( IDC_CMB_HANDLERS_COMBO );

	pCmbHandlers->AddString( "OnLeftClick" );
	pCmbHandlers->AddString( "OnRightClick" );
	pCmbHandlers->AddString( "OnTimer" );
	pCmbHandlers->AddString( "OnSetCam" );
	pCmbHandlers->AddString( "OnMouseOver" );
	pCmbHandlers->AddString( "OnCollision" );
	pCmbHandlers->AddString( "OnCollision2" );
	pCmbHandlers->AddString( "OnOuterCollision" );

	pCmbHandlers->SetCurSel( 0 );


	//init functions combo
	CComboBox* pCmbFuncs = (CComboBox*)GetDlgItem( IDC_CMB_FUNCTIONS );

	pCmbFuncs->AddString( "LoadTex4" );
	pCmbFuncs->AddString( "LoadVideo2" );
	
	pCmbFuncs->AddString( "ObjGoSeq" );
	pCmbFuncs->AddString( "ObjGoPredSeq" );
	pCmbFuncs->AddString( "ObjGoSeq2" );
	pCmbFuncs->AddString( "ObjGo0" );
	pCmbFuncs->AddString( "ObjGo1" );
	pCmbFuncs->AddString( "ObjGo2" );
	pCmbFuncs->AddString( "ObjGo3" );
	pCmbFuncs->AddString( "ObjGo3Pred" );
	pCmbFuncs->AddString( "ObjGo4" );
	pCmbFuncs->AddString( "ObjGo4Pred" );
	pCmbFuncs->AddString( "ObjStop" );
	pCmbFuncs->AddString( "ObjSetPos" );
	pCmbFuncs->AddString( "ObjGetPos" );
	pCmbFuncs->AddString( "ObjSetPosSeq" );
	pCmbFuncs->AddString( "ObjInNull2D" );

	pCmbFuncs->AddString( "inputEnabled" );
	pCmbFuncs->AddString( "inputEnabledSeq" );
	pCmbFuncs->AddString( "IntersectPortal" );
	
	pCmbFuncs->AddString( "SetCam" );
	pCmbFuncs->AddString( "SetCam2" );

	pCmbFuncs->AddString( "Quest_Scene2Scene" );
	pCmbFuncs->AddString( "Quest_Scene2SceneSeq" );
	pCmbFuncs->AddString( "Quest_LoadSceneSeq" );
	pCmbFuncs->AddString( "Quest_LoadScene" );

	pCmbFuncs->AddString( "AddObjVideoAnim" );
	pCmbFuncs->AddString( "SetEnabled" );
	
	pCmbFuncs->AddString( "PlaySmpl" );
	pCmbFuncs->AddString( "PlaySnd" );
	pCmbFuncs->AddString( "PlaySnd2" );
	pCmbFuncs->AddString( "PlaySnd3" );
	pCmbFuncs->AddString( "PlaySnd4" );
	pCmbFuncs->AddString( "StopSnd" );

	pCmbFuncs->AddString( "AddFadeDownZone" );
	pCmbFuncs->AddString( "ssSeq" );
	pCmbFuncs->AddString( "ssbSeq" );
	pCmbFuncs->AddString( "SeqInit" );
	pCmbFuncs->AddString( "SeqAddAct" );
	pCmbFuncs->AddString( "SeqAddActInc" );
	pCmbFuncs->AddString( "SeqPause" );

	pCmbFuncs->AddString( "TimerSet" );
	pCmbFuncs->AddString( "TimerKill" );
	pCmbFuncs->AddString( "SetCursorHint" );
	pCmbFuncs->AddString( "LogErr" );
	pCmbFuncs->AddString( "GoFadeDown" );
	pCmbFuncs->AddString( "GoFadeDown2" );
	pCmbFuncs->AddString( "GoFadeUp" );
	pCmbFuncs->AddString( "GoFadeUp2" );


	pCmbFuncs->SetCurSel( 0 );

	//init predefined sets combo
	CComboBox* pCmbSets = (CComboBox*)GetDlgItem( IDC_CMB_PREDEFINED_SETS );

	pCmbSets->AddString( "Sequence" );
	pCmbSets->AddString( "Video animation" );

	pCmbSets->SetCurSel( 0 );

	PopulateClassesCombo();
	PopulateFuncsCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CScriptingDlg::FillFilterCombos()
{
	if( !m_pParent )
		return;

	CBaseScene* pBaseScene = ((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	if( !pBaseScene )
		return;


	StrVec vecSceneData;

	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*	scene = ((CSceneFile*)pBaseScene);
	CString		temp;

	//collect data from the scene
	for( int i = 0 ; i < scene->GetLightsCount(); i++ )
	{
		temp.Format( "Light.%s", scene->GetLightByIndex( i )->name.data() );
		vecSceneData.push_back( temp.GetBuffer() );
	}

	for( int i = 0 ; i < scene->GetCamerasCount(); i++ )
	{
		temp.Format( "Camera.%s", scene->GetCameraByIndex( i )->name.data() );
		vecSceneData.push_back( temp.GetBuffer() );
	}

	CEfnMeshlist* meshlist = (CEfnMeshlist*)scene->GetMeshlist();

	if( meshlist )
	{
		for( int i = 0 ; i < meshlist->GetMeshesCount(); i++ )
		{
			temp.Format( "Mesh.%s", meshlist->GetMeshByIndex( i )->GetName().data() );
			vecSceneData.push_back( temp.GetBuffer() );
		}
	}

	StrVec tex_vec = *(scene->GetTextures());
	for( unsigned int i = 0; i < tex_vec.size(); i++ )
	{
		temp.Format( "Texture.%s", tex_vec[i].data() );
		vecSceneData.push_back( temp.GetBuffer() );
	}

	temp.Format( "Scene.%s", scene->GetSceneID().data() );
	vecSceneData.push_back( temp.GetBuffer() );

	StrVec scnTypes;
	GetTypesList( scnTypes );

	StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();
	
	for( unsigned int i = 0; i < m_CustomObjects->size(); i++ )
	{
		temp.Format( "Object.%s", (*m_CustomObjects)[i].data() );
		vecSceneData.push_back( temp.GetBuffer() );
	}

	for( unsigned int i = 0; i < m_CustomTypes->size(); i++ )
	{
		scnTypes.push_back( (*m_CustomTypes)[i] );
	}

	m_cmbFilterParentType.ResetContent();
	m_cmbFilterParentID.ResetContent();

	for( unsigned int i = 0; i < scnTypes.size(); i++ )
		m_cmbFilterParentType.AddString( scnTypes[i].data() );

	for( unsigned int i = 0; i < vecSceneData.size(); i++ )
		m_cmbFilterParentID.AddString( vecSceneData[i].data() );


	if( m_strFilterParType == "" || m_strFilterParID_full == "" )
	{
		m_strFilterParType	= "Scene";
		m_strFilterParID	= scene->GetSceneID().data();
		m_strFilterParID_full = m_strFilterParType + "." + m_strFilterParID;
	}

	m_cmbFilterParentType.SetCurSel( m_cmbFilterParentType.FindString( 0, m_strFilterParType ) );
	m_cmbFilterParentID.SetCurSel( m_cmbFilterParentID.FindString( 0, m_strFilterParID_full ) );

	str_t strTemp = m_strFilterParID_full.GetBuffer();
	StripTypeHeader( strTemp );
	m_strFilterParID = strTemp.data();
}

str_t CScriptingDlg::GetItemScript( str_t strItem )
{
	// item_type   -- type of child to get script from
	// item_name -- name of child to get script from
	str_t item_type	= StripTypeHeader( strItem );
	str_t script;

	//find corresponding bind
	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		bool ok = false;

		if( (*iter)[2] == (str_t)m_strFilterParType.GetBuffer() && (*iter)[1] == (str_t)m_strFilterParID.GetBuffer() && (*iter)[3] == strItem && (*iter)[4] == item_type )
			ok = true;

		if( ok == false )
		{
			iter++;
			continue;
		}

		if( (*iter).size() < 6 )
			script = "";
		else
			script = (*iter)[5];

		break;
	}

	return script;
}

void CScriptingDlg::SetItemScript( str_t strItem, str_t strValue )
{
	// item_type   -- type of child to get script from
	// item_name -- name of child to get script from

	str_t item_type	= StripTypeHeader( strItem );
	str_t script;

	//find corresponding bind
	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		bool ok = false;

		if( (*iter)[2] == (str_t)m_strFilterParType.GetBuffer() && (*iter)[1] == (str_t)m_strFilterParID.GetBuffer() && (*iter)[3] == strItem && (*iter)[4] == item_type )
			ok = true;

		if( ok == false )
		{
			iter++;
			continue;
		}

		if( (*iter).size() < 6 )
		{
			(*iter).push_back( strValue );
		}

		(*iter)[5] = strValue;

		break;
	}
}

void CScriptingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UpdateUISizing();

	CRect rc;
	this->GetWindowRect( &rc );
	ScreenToClient( &rc );

	m_WndHeight = rc.Height();

	UpdateUISizing();
}

void CScriptingDlg::UpdateUISizing()
{
	CButton*	pButtClose	= (CButton*)GetDlgItem( IDC_BTN_CLOSE );
	//CListBox*	pListBox	= (CListBox*)GetDlgItem( IDC_LST_OBJECT );
	CStatic*	pLexerFrm	= (CStatic*)GetDlgItem( IDC_LEXER_FRM );
	CStatic*	pOutputFrm	= (CStatic*)GetDlgItem( IDC_OUTPUT_FRM );
	//CWnd*		pOutput		= GetDlgItem( IDC_RICHEDIT_OUT );
	CWnd*		pOutputHdr	= GetDlgItem( IDC_OUT_HDR );

	CComboBoxEx* pClassesCombo		= (CComboBoxEx*)GetDlgItem( IDC_COMBOBOXEX_CLASSES );
	CComboBoxEx* pFunctionsCombo	= (CComboBoxEx*)GetDlgItem( IDC_COMBOBOXEX_FUNCS );

	int top = 0;

	CRect rc;
	CRect rc_self;
	this->GetWindowRect( &rc );


	ScreenToClient( &rc );

	if( !m_wndSplitterH )
		return;

	if( m_wndSplitterH )
	{
		m_wndSplitterH.GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );
		top = rc_self.top;
	}

	int width_incr	= 0;
	int height_incr	= 0;

	if( pLexerFrm )
	{
		pLexerFrm->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		width_incr		= rc.Width() - rc_self.right;
		height_incr		= rc.Height() - rc_self.bottom;
		rc_self.right	+= width_incr - 10;
		rc_self.bottom	= top;

		pLexerFrm->MoveWindow( &rc_self, 0 );

		m_lexer.MoveEditor( rc_self );
	}

	int iXOff;

	if( pClassesCombo )
	{
		pClassesCombo->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		width_incr		= rc.Width() - rc_self.right;
		rc_self.right	+= width_incr - 10;

		rc_self.right = rc_self.left + rc_self.Width() / 2 - 6;

		iXOff = rc_self.right + 6;

		pClassesCombo->MoveWindow( &rc_self, 0 );
	}

	if( pFunctionsCombo )
	{
		pFunctionsCombo->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		rc_self.left = iXOff;
		width_incr		= rc.Width() - rc_self.right;
		rc_self.right	+= width_incr - 10;

		pFunctionsCombo->MoveWindow( &rc_self, 0 );
	}

	if( m_wndSplitterH )
	{
		m_wndSplitterH.GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		rc_self.right += width_incr - 10;

		m_wndSplitterH.MoveWindow( &rc_self, 1 );
		m_wndSplitterH.Invalidate( true );
	}

	//move close button
	if( pButtClose )
	{
		pButtClose->GetWindowRect( &rc_self );
		rc_self.MoveToXY( rc.Width() - 87, rc.Height() - 55 );
		pButtClose->MoveWindow( &rc_self, 1 );
	}

	//move list box
	/*if( pListBox )
	{
		pListBox->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );
		rc_self.bottom	= top;
		pListBox->MoveWindow( &rc_self, 1 );
	}*/

	m_tvObjects.GetWindowRect( &rc_self );
	ScreenToClient( &rc_self );
	rc_self.bottom	= top;
	m_tvObjects.MoveWindow( &rc_self, 1 );

	if( m_wndSplitter )
	{
		//move slider
		m_wndSplitter.GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );
		rc_self.bottom = top;
		m_wndSplitter.MoveWindow( &rc_self, 1 );	

		m_wndSplitter.Invalidate( true );
	}


	if( pOutputHdr )
	{
		pOutputHdr->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );
		rc_self.right	= rc.right - 6;
		pOutputHdr->MoveWindow( &rc_self, 1 );
	}

	if( pOutputFrm )
	{
		pOutputFrm->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		rc_self.bottom	= rc.bottom - 38;
		rc_self.right	= rc.right - 6;
		pOutputFrm->MoveWindow( &rc_self, 1 );
		
		m_lexer_out.MoveEditor( rc_self );
	}

	if( m_WndHeight != -1 )
	{
		int delta = rc.Height() - m_WndHeight;
		m_wndSplitterH.ChangePos( (CWnd*)&m_wndSplitterH, 0, delta );
		DoResizeH( delta );
	}
}

LRESULT CScriptingDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	WINDOWPOS *lpwp;

	switch(message)
	{
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE )
				UpdateCurrentScript();
			break;

		case WM_NOTIFY:
			{
				if( wParam == IDC_SLIDER )
				{
					SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
					DoResize( pHdr->delta );
				}
				else if( wParam == IDC_SLIDERH )
				{
					SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
					DoResizeH( pHdr->delta );
				}

				NMHDR *lpnmhdr = (LPNMHDR)lParam;

				if( lpnmhdr->hwndFrom == m_lexer.m_hEditor )
				{
					switch( lpnmhdr->code )
					{
						case SCEN_CHANGE:
							//AfxMessageBox("!");
						break;
					}
				}
			}
			break;

		case WM_CLOSE:
			OnCloseDlg();
			break;

		case WM_WINDOWPOSCHANGING:
			lpwp = (LPWINDOWPOS) lParam;
            
			if( lpwp->cx < m_iMinWindowSizeX )
				lpwp->cx = m_iMinWindowSizeX;

			if( lpwp->cy < m_iMinWindowSizeY )
				lpwp->cy = m_iMinWindowSizeY;
			break;

		case WM_EXITSIZEMOVE:
			break;

		case WM_ENTERSIZEMOVE:
			{

			}
			break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CScriptingDlg::OnCloseDlg()
{
	UpdateCurrentScript();

	m_bIsDialogClosing = true;

	m_tvObjects.ReleaseTreeData();
	m_tvObjects.DeleteAllItems();
	
	//ClearFuncsList();
}

void CScriptingDlg::DoResize( int delta )
{
	CWnd* pScriptSrcFrm		= GetDlgItem( IDC_LEXER_FRM );
	CWnd* pObjFilterFrm		= GetDlgItem( IDC_FILTER_FRM );
	CWnd* pObjFilterType	= GetDlgItem( IDC_CMB_FILTER_TYPE );
	CWnd* pObjFilterID		= GetDlgItem( IDC_CMB_FILTER_ID );
	CWnd* pFuncsCombo		= GetDlgItem( IDC_COMBOBOXEX_CLASSES );

	CSplitterControl::ChangeWidth( &m_tvObjects, delta, CW_LEFTALIGN );
	CSplitterControl::ChangeWidth( pObjFilterFrm, delta, CW_LEFTALIGN );
	CSplitterControl::ChangeWidth( pObjFilterType, delta, CW_LEFTALIGN );
	CSplitterControl::ChangeWidth( pObjFilterID, delta, CW_LEFTALIGN );

	CSplitterControl::ChangeWidth( pScriptSrcFrm, -delta, CW_RIGHTALIGN );
	CSplitterControl::ChangeWidth( pFuncsCombo, -delta, CW_RIGHTALIGN );
	
	CRect rect;

	pScriptSrcFrm->GetWindowRect( &rect );
	ScreenToClient( &rect );
	m_lexer.MoveEditor( rect.left, rect.top, rect.Width(), rect.Height() );

	CRect rc;
	CRect rc_self;
	this->GetWindowRect( &rc );
	ScreenToClient( &rc );

	int width_incr	= 0;
	int height_incr	= 0;

	CComboBoxEx* pClassesCombo		= (CComboBoxEx*)GetDlgItem( IDC_COMBOBOXEX_CLASSES );
	CComboBoxEx* pFunctionsCombo	= (CComboBoxEx*)GetDlgItem( IDC_COMBOBOXEX_FUNCS );

	int iXOff;

	if( pClassesCombo )
	{
		pClassesCombo->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		width_incr		= rc.Width() - rc_self.right;
		rc_self.right	+= width_incr - 10;

		rc_self.right = rc_self.left + rc_self.Width() / 2 - 6;

		iXOff = rc_self.right + 6;

		pClassesCombo->MoveWindow( &rc_self, 0 );
	}

	if( pFunctionsCombo )
	{
		pFunctionsCombo->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		rc_self.left = iXOff;
		width_incr		= rc.Width() - rc_self.right;
		rc_self.right	+= width_incr - 10;

		pFunctionsCombo->MoveWindow( &rc_self, 0 );
	}

	Invalidate();
	UpdateWindow();
}

void CScriptingDlg::DoResizeH( int delta )
{
	//CWnd* pObjectsList	= GetDlgItem( IDC_LST_OBJECT );
	CWnd* pScriptSrcFrm	= GetDlgItem( IDC_LEXER_FRM );
	CWnd* pOutputHdr	= GetDlgItem( IDC_OUT_HDR );
	CWnd* pOutputHdrLbl	= GetDlgItem( IDC_OUT_HDR_LBL );
	CWnd* pOutputFrm	= GetDlgItem( IDC_OUTPUT_FRM );

	//CSplitterControl::ChangeHeight( pObjectsList, delta, CW_TOPALIGN );
	CSplitterControl::ChangeHeight( &m_tvObjects, delta, CW_TOPALIGN );
	CSplitterControl::ChangeHeight( pScriptSrcFrm, delta, CW_TOPALIGN );
	CSplitterControl::ChangeHeight( &m_wndSplitter, delta, CW_TOPALIGN );
	CSplitterControl::ChangePos( pOutputHdr, 0, delta );
	CSplitterControl::ChangePos( pOutputHdrLbl, 0, delta );
	CSplitterControl::ChangeHeight( pOutputFrm, -delta, CW_BOTTOMALIGN );
	
	
	CRect rect;

	pScriptSrcFrm->GetWindowRect( &rect );
	ScreenToClient( &rect );
	m_lexer.MoveEditor( rect.left, rect.top, rect.Width(), rect.Height() );

	pOutputFrm->GetWindowRect( &rect );
	ScreenToClient( &rect );
	m_lexer_out.MoveEditor( rect );

	Invalidate();
	UpdateWindow();
}

void CScriptingDlg::OnOK()
{
	OnCloseDlg();

	CDialog::OnOK();
}

BOOL CScriptingDlg::PreTranslateMessage( MSG* pMsg )
{	
	if( TranslateAccelerator( this->GetSafeHwnd(), m_hAccelerator, pMsg ) )
		return true;

	switch( pMsg->message )
	{
		case WM_KEYDOWN:

			if( pMsg->wParam == VK_ESCAPE )
				return false;
			break;

		case WM_KEYUP:

			if( pMsg->wParam == VK_ESCAPE )
			{
				OnCloseDlg();
				OnOK();
				return true;
			}
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CScriptingDlg::StorOldClipboard()
{
	::OpenClipboard( this->GetSafeHwnd() );

	HGLOBAL hglb = GetClipboardData( CF_TEXT );

	if( hglb )
	{
		m_strOldClipboard = (char *)GlobalLock( hglb );
		GlobalUnlock( hglb );
	}

	::CloseClipboard();
}

void CScriptingDlg::RestorOldClipboard()
{
	String2Clipboard( m_strOldClipboard );
}

void CScriptingDlg::OnLvnItemchangedLstObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//if( oldItem == pNMLV->iItem )
	//	return;

	//CString strTemp, script_text;

	//if( oldItem != -1 )
	//{
	//	LV_ITEM list_item;
	//	ZeroMemory( &list_item, sizeof( LV_ITEM ) );
	//	list_item.mask		= LVIF_IMAGE;
	//	list_item.iItem		= oldItem;
	//
	//	int len = m_lexer.SendEditor( SCI_GETLENGTH );
	//	char* src = new char[len + 1];

	//	m_lexer.SendEditor( SCI_GETTEXT, len + 1, (LPARAM)src );

	//	script_text = src;
	//	script_text.Trim();

	//	if( script_text == "" )
 //           list_item.iImage = 0;
	//	else
	//		list_item.iImage = 1;

	//	m_lstObjects.SetItem( &list_item );

	//	//save old item script
	//	strTemp = m_lstObjects.GetItemText( oldItem, 0 );
	//	SetItemScript( strTemp.GetBuffer(), src );

	//	SAFE_DELETE_ARRAY( src );
	//}

	////load new item script
	//strTemp = m_lstObjects.GetItemText( pNMLV->iItem, 0 );
	//strTemp += "\0";
	//m_lexer.SendEditor( SCI_SETTEXT, 0, (LPARAM)GetItemScript( strTemp.GetBuffer() ).data() );
	//m_lexer.SendEditor( SCI_SETXOFFSET, 0 );

	//oldItem = pNMLV->iItem;

	//m_strLastSelObj = strTemp.GetBuffer();

	//m_lexer.SendEditor( SCI_EMPTYUNDOBUFFER );

	//*pResult = 0;
}

afx_msg void CScriptingDlg::OnLexerDataModify( NMHDR* nmhdr, LRESULT* result )
{
	bool res = true;

	SCNotification* notify = (SCNotification*)nmhdr;
	const int line_number	= (const int)m_lexer.SendEditor( SCI_LINEFROMPOSITION, notify->position, 0 );

	if( 0 != ( notify->modificationType & SC_MOD_DELETETEXT ) ||
		0 != ( notify->modificationType & SC_MOD_INSERTTEXT ) )
	{
		if( m_lexer.SendEditor( SCI_GETFOLDEXPANDED, line_number, 0 ) == false )
		{
			//deleting folded line
		}
	}
}

void CScriptingDlg::String2Clipboard( str_t strValue )
{
	LPTSTR		lptstrCopy; 
    HGLOBAL		hglbCopy; 
  
    // Open the clipboard, and empty it. 
	::OpenClipboard( this->GetSafeHwnd() );
	::EmptyClipboard();

	str_t str_dt = strValue;
 
    // Allocate a global memory object for the text. 
    hglbCopy = GlobalAlloc( GMEM_MOVEABLE, (str_dt.length() + 1) * sizeof(char) ); 

    // Lock the handle and copy the text to the buffer. 
    lptstrCopy = (LPTSTR)GlobalLock( hglbCopy ); 
    memcpy( lptstrCopy, str_dt.data(), str_dt.length() * sizeof(char) ); 
    lptstrCopy[str_dt.length()] = (char)0;    // null character 
    GlobalUnlock( hglbCopy );

    // Place the handle on the clipboard. 
    SetClipboardData( CF_TEXT, hglbCopy ); 

	::CloseClipboard();
}

void CScriptingDlg::RButtonUp( CPoint point )
{
	//CObjPopupDlg* popup = new CObjPopupDlg( this );

	//CRect rect;
	//m_rchScriptSrc.GetWindowRect( &rect );
	//ScreenToClient( &rect );

	//point.x += rect.left + 7;
	//point.y += rect.top + 23;

	//popup->point = point;


	//StrVec objs;
	//ListStrVec_iter iter = m_pBinds->begin();

	//while( iter != m_pBinds->end() )
	//{
	//	if( (*iter)[2] == "Scene" )
	//	{
	//		str_t tmp_obj = (*iter)[4] + "." + (*iter)[3];
	//		objs.push_back( tmp_obj );
	//	}

	//	iter++;
	//}

	//for( unsigned int i = 0; i < m_pNulls->size(); i++ )
	//	objs.push_back( (*m_pNulls)[i] );

	//popup->SetObjects( &objs );
	//popup->DoModal();

	////insert choice into script
	//str_t res = popup->sel_object;

	//if( res != "" )
	//{
	//	StripTypeHeader( res );

	//	String2Clipboard( res );

	//	m_rchScriptSrc.PasteSpecial( CF_TEXT );
	//	m_rchScriptSrc.SetFocus();
	//}

	//SAFE_DELETE( popup );
}

void CScriptingDlg::OnBnClickedBtnCreateHandler()
{
	//pCmbHandlers->AddString( "OnLeftClick" );
	//pCmbHandlers->AddString( "OnRightClick" );
	//pCmbHandlers->AddString( "OnTimer" );
	//pCmbHandlers->AddString( "OnSetCam" );
	//pCmbHandlers->AddString( "OnMouseOver" );
	//pCmbHandlers->AddString( "OnCollision" );
	//pCmbHandlers->AddString( "OnCollision2" );
	//pCmbHandlers->AddString( "OnOuterCollision" );

	if( m_tvObjects.GetParNode() == NULL )
		return;	

	CComboBox*	pCmb = (CComboBox*)GetDlgItem( IDC_CMB_HANDLERS_COMBO );
	CString		strTemp;
	str_t		item, item_type, func_string;

	pCmb->GetLBText( pCmb->GetCurSel(), strTemp );

	//get OBJECT TYPE AND HEADER
	if( m_tvObjects.GetParNode() == NULL )
		return;

	item		= m_tvObjects.GetCurrNodeText().GetBuffer();
	item_type	= m_tvObjects.GetParNodeText().GetBuffer();

	if( strTemp == "OnLeftClick" )
	{
		func_string = "function Scene:" + item_type + "_" + item + "_OnLeftClick(bDblClick)";
	}
	else if( strTemp == "OnSetCam" )
	{
		func_string = "function Scene:SetCam_" + item_type + "_" + item + "()\n\rQuest:SetCam(\"" + item_type + "\");";
	}
	else if( strTemp == "OnRightClick" )
	{
		func_string = "function Scene:" + item_type + "_" + item + "_OnRightClick(bDblClick)";
	}
	else if( strTemp == "OnTimer" )
	{
		func_string = "function Scene:" + item_type + "_" + item + "_OnTimer()";
	}
	else if( strTemp == "OnMouseOver" )
	{
		func_string = "function Scene:" + item_type + "_" + item + "_OnMouseOver()";
	}
	else if( strTemp == "OnCollision" )
	{
		func_string = "function Scene:" + item_type + "_" + item + "_OnCollision( obj_id )";
	}
	else if( strTemp == "OnCollision2" )
	{
		func_string = "function Scene:" + item_type + "_" + item + "_OnCollision2( obj_id, fElapsedTime )";
	}
	else if( strTemp == "OnOuterCollision" )
	{
		func_string = "function Scene:" + item_type + "_" + item + "_OnOuterCollision( obj_id )";
	}
	else
	{}

	//closing "end;"
	func_string += "\n\r\n\r\n\rend;\n\r";

	m_lexer.SendEditor( SCI_INSERTTEXT, -1, (LPARAM)func_string.data() );
}

void CScriptingDlg::OnBnClickedBtnCreateSet()
{
	if( m_tvObjects.GetParNode() == NULL )
		return;	

	CComboBox*	pCmb = (CComboBox*)GetDlgItem( IDC_CMB_PREDEFINED_SETS );
	CString		strTemp;
	str_t		func_string;

	pCmb->GetLBText( pCmb->GetCurSel(), strTemp );


	if( strTemp == "Sequence" )
	{
		func_string += "\tseq_id = SeqInit(\"\");\n";
		func_string += "\ts_seq_id = int2str(seq_id);\n\r\n\r";
		func_string += "\tm_sAction = \"\";\n";
		func_string += "\tSeqAddAct(seq_id, m_sAction);\n\r\n\r";
		func_string += "\tSeqGo(seq_id);";
	}
	else if( strTemp == "Video animation" )
	{
		func_string += "\tQuestObjMgr:AddObjVideoAnim( \"anim_ID\" );\n"; 
		func_string += "\tAnimObj = QuestObjMgr:GetObjVideoAnim( \"anim_ID\" );\n";
		func_string += "\tAnimObj:AnimAdd( \"Backgrounds\\\\filename\", -1, -1, -1 );\n";
		func_string += "\tAnimObj:Go( 0, -1 );\n";
	}
	else
	{
	
	}

	m_lexer.SendEditor( SCI_INSERTTEXT, -1, (LPARAM)func_string.data() );
}

void CScriptingDlg::OnBnClickedBtnCreateFunction()
{
	//pCmbFuncs->AddString( "LoadTex4" );
	//pCmbFuncs->AddString( "LoadVideo2" );
	
	//pCmbFuncs->AddString( "ObjGoSeq" );
	//pCmbFuncs->AddString( "ObjGoPredSeq" );
	//pCmbFuncs->AddString( "ObjGoSeq2" );
	//pCmbFuncs->AddString( "ObjGo0" );
	//pCmbFuncs->AddString( "ObjGo1" );
	//pCmbFuncs->AddString( "ObjGo2" );
	//pCmbFuncs->AddString( "ObjGo3" );
	//pCmbFuncs->AddString( "ObjGo3Pred" );
	//pCmbFuncs->AddString( "ObjGo4" );
	//pCmbFuncs->AddString( "ObjGo4Pred" );
	//pCmbFuncs->AddString( "ObjStop" );
	//pCmbFuncs->AddString( "ObjSetPos" );
	//pCmbFuncs->AddString( "ObjGetPos" );
	//pCmbFuncs->AddString( "ObjSetPosSeq" );
	//pCmbFuncs->AddString( "ObjInNull2D" );

	//pCmbFuncs->AddString( "inputEnabled" );
	//pCmbFuncs->AddString( "inputEnabledSeq" );
	//pCmbFuncs->AddString( "IntersectPortal" );
	
	//pCmbFuncs->AddString( "SetCam" );
	//pCmbFuncs->AddString( "SetCam2" );

	//pCmbFuncs->AddString( "Quest_Scene2Scene" );
	//pCmbFuncs->AddString( "Quest_Scene2SceneSeq" );
	//pCmbFuncs->AddString( "Quest_LoadSceneSeq" );
	//pCmbFuncs->AddString( "Quest_LoadScene" );

	//pCmbFuncs->AddString( "AddObjVideoAnim" );
	//pCmbFuncs->AddString( "SetEnabled" );
	
	//pCmbFuncs->AddString( "PlaySmpl" );
	//pCmbFuncs->AddString( "PlaySnd" );
	//pCmbFuncs->AddString( "PlaySnd2" );
	//pCmbFuncs->AddString( "PlaySnd3" );
	//pCmbFuncs->AddString( "PlaySnd4" );
	//pCmbFuncs->AddString( "StopSnd" );

	//pCmbFuncs->AddString( "AddFadeDownZone" );
	//pCmbFuncs->AddString( "ssSeq" );
	//pCmbFuncs->AddString( "ssbSeq" );
	//pCmbFuncs->AddString( "SeqInit" );
	//pCmbFuncs->AddString( "SeqAddAct" );
	//pCmbFuncs->AddString( "SeqAddActInc" );
	//pCmbFuncs->AddString( "SeqPause" );

	//pCmbFuncs->AddString( "TimerSet" );
	//pCmbFuncs->AddString( "TimerKill" );
	//pCmbFuncs->AddString( "SetCursorHint" );
	//pCmbFuncs->AddString( "LogErr" );
	//pCmbFuncs->AddString( "GoFadeDown" );
	//pCmbFuncs->AddString( "GoFadeDown2" );
	//pCmbFuncs->AddString( "GoFadeUp" );
	//pCmbFuncs->AddString( "GoFadeUp2" );

	if( m_tvObjects.GetParNode() == NULL )
		return;	

	CComboBox*	pCmb = (CComboBox*)GetDlgItem( IDC_CMB_FUNCTIONS );
	CString		strTemp;
	str_t		func_string;

	pCmb->GetLBText( pCmb->GetCurSel(), strTemp );

	if( strTemp == "LoadTex4" )
	{
		func_string += "LoadTex4( \"Backgrounds\\\\\", 1, 1, 0, 0, 0, 0 );";
	}
	else if( strTemp == "LoadVideo2" )
	{
		func_string += "LoadVideo2( \"Backgrounds\\\\\", 1, true, 0, 0 );";
	}
	else if( strTemp == "ObjGoSeq" )
	{
		func_string += "--ObjGoSeq функция для добавления перемещения в сиквенцию,\r\n";
		func_string += "--надо передавать название нуля из сцены\r\n";
		func_string += "ObjGoSeq( obj_id, null_id, bRun, bTakeNullPos, seq_id );";
	}
	else if( strTemp == "ObjGoPredSeq" )
	{
		func_string += "--ObjGoPredSeq функция для добавления перемещения в сиквенцию,\r\n";
		func_string += "--надо передавать название нуля из сцены, также указать растояние для правильного вхождения в ноль,\r\n";
		func_string += "--используется если bTakeNullPos == true, если fPredNull==0 то нуль для правильного вхождения\r\n";
		func_string += "--не достраивается\r\n";
		func_string += "ObjGoPredSeq( obj_id, null_id, bRun, bTakeNullPos, fPredNull, seq_id );";
	}
	else if( strTemp == "ObjGoSeq2" )
	{
		func_string += "--ObjGoSeq2 функция для добавления перемещения в сиквенцию, надо передавать нуль из сцены а не название\r\n";
		func_string += "ObjGoSeq2( obj_id, stNull, bRun, bTakeNullPos, seq_id );";
	}
	else if( strTemp == "ObjGo0" )
	{
		func_string += "--ObjGo0 без сиквенции, функция для отправки дуда в точку пересечения\r\n";
		func_string += "ObjGo0( bRun );";
	}
	else if( strTemp == "ObjGo1" )
	{
		func_string += "--ObjGo1 без сиквенции, функция для отправки людей в нуль, надо передавать нуль из сцены, а не его\r\n";
		func_string += "--название\r\n";
		func_string += "ObjGo1( obj_id, p, bRun )";
	}
	else if( strTemp == "ObjGo2" )
	{
		func_string += "--ObjGo2 функция без сиквенции, надо передавать название нуля из сцены\r\n";
		func_string += "ObjGo2( obj_id, null_id, bRun, bTakeNullPos );";
	}
	else if( strTemp == "ObjGo3" )
	{
		func_string += "--ObjGo3 функция с сиквенцией, надо передавать сам нуль из сцены, а не название\r\n";
		func_string += "ObjGo3( obj_id, p, bRun, bTakeNullPos, seq_id );";
	}
	else if( strTemp == "ObjGo3Pred" )
	{
		func_string += "--ObjGo3Pred функция с сиквенцией, надо передавать сам нуль из сцены, а не название, также нужно указать\r\n";
		func_string += "--растояние для правильного вхождения в ноль, используется если bTakeNullPos == true, если fPredNull==0\r\n";
		func_string += "--то нуль для правильного вхождения не достраивается\r\n";
		func_string += "ObjGo3Pred( obj_id, p, bRun, bTakeNullPos, fPredNull, seq_id );";
	}
	else if( strTemp == "ObjGo4" )
	{
		func_string += "--ObjGo4 функция с сиквенцией, надо передавать кривую для вей поинтов, а не название или нуль\r\n";
		func_string += "ObjGo4( obj_id, curve_id, bRun, bTakeNullPos, seq_id );";
	}
	else if( strTemp == "ObjGo4Pred" )
	{
		func_string += "--ObjGo4Pred функция с сиквенцией, надо передавать вей поинты, а не название или нуль, также нужно указать\r\n";
		func_string += "--растояние для правильного вхождения в ноль, используется если bTakeNullPos == true, если fPredNull==0\r\n";
		func_string += "--то нуль для правильного вхождения не достраивается\r\n";
		func_string += "ObjGo4Pred( obj_id, curve_id, bRun, bTakeNullPos, fPredNull, seq_id )";
	}
	else if( strTemp == "ObjStop" )
	{
		func_string += "ObjStop( obj_id, flag );";
	}
	else if( strTemp == "ObjSetPos" )
	{
		func_string += "ObjSetPos( obj_id, stNull );";
	}
	else if( strTemp == "ObjGetPos" )
	{
		func_string += "ObjGetPos( obj_id );";
	}
	else if( strTemp == "ObjSetPosSeq" )
	{
		func_string += "ObjSetPosSeq( obj_id, null_id, seq_id );";
	}
	else if( strTemp == "ObjInNull2D" )
	{
		func_string += "ObjInNull2D( obj_id, null_id );";
	}
	else if( strTemp == "inputEnabled" )
	{
		func_string += "inputEnabled( bEnabled );";
	}
	else if( strTemp == "inputEnabledSeq" )
	{
		func_string += "inputEnabledSeq( bEnabled, seq_id );";
	}
	else if( strTemp == "IntersectPortal" )
	{
		func_string += "IntersectPortal( cam_id, obj_id, bDblClick );";
	}
	else if( strTemp == "SetCam" )
	{
		func_string += "SetCam( cam_id );";
	}
	else if( strTemp == "SetCam2" )
	{
		func_string += "SetCam2( cam_id );";
	}
	else if( strTemp == "Quest_Scene2Scene" )
	{
		func_string += "Quest_Scene2Scene( null_id, scene_id, new_null_id, sVideo, bDblClick, bTakeNullPos );";
	}
	else if( strTemp == "Quest_Scene2SceneSeq" )
	{
		func_string += "Quest_Scene2SceneSeq( null_id, scene_id, new_null_id, sVideo, bDblClick, bTakeNullPos, seq_id );";
	}
	else if( strTemp == "Quest_LoadSceneSeq" )
	{
		func_string += "Quest_LoadSceneSeq( scene_id, null_id, seq_id );";
	}
	else if( strTemp == "Quest_LoadScene" )
	{
		func_string += "Quest_LoadScene( scene_id, null_id );";
	}
	else if( strTemp == "AddObjVideoAnim" )
	{
		func_string += "QuestObjMgr:AddObjVideoAnim( anim_id );";
	}
	else if( strTemp == "SetEnabled" )
	{
		func_string += "QuestObjMgr:SetEnabled( obj_id, bValue );";
	}
	else if( strTemp == "PlaySmpl" )
	{
		func_string += "PlaySmpl( snd_id );";
	}
	else if( strTemp == "PlaySnd" )
	{
		func_string += "PlaySnd( snd_id );";
	}
	else if( strTemp == "PlaySnd2" )
	{
		func_string += "PlaySnd2( snd_id, iRepeatCount );";
	}
	else if( strTemp == "PlaySnd3" )
	{
		func_string += "PlaySnd3( snd_id, bSync );";
	}
	else if( strTemp == "PlaySnd4" )
	{
		func_string += "PlaySnd4( snd_id, iRepeatCount, bSync );";
	}
	else if( strTemp == "StopSnd" )
	{
		func_string += "StopSnd( snd_id );";
	}
	else if( strTemp == "AddFadeDownZone" )
	{
		func_string += "AddFadeDownZone( null_id, fRadius, fFadeTime, seq_id );";
	}
	else if( strTemp == "ssSeq" )
	{
		func_string += "ssSeq( state_id, iValue, seq_id );";
	}
	else if( strTemp == "ssbSeq" )
	{
		func_string += "ssbSeq( state_id, bValue, seq_id );";
	}
	else if( strTemp == "SeqInit" )
	{
		func_string += "SeqInit( parent_id );";
	}
	else if( strTemp == "SeqAddAct" )
	{
		func_string += "SeqAddAct( seq_id, sAction )";
	}
	else if( strTemp == "SeqAddActInc" )
	{
		func_string += "SeqAddActInc( seq_id, sAction );";
	}
	else if( strTemp == "SeqPause" )
	{
		func_string += "SeqPause( seq_id, fSec );";
	}
	else if( strTemp == "TimerSet" )
	{
		func_string += "QuestTMgr:TimerSet( timer_id, fInterval, sScript );";
	}
	else if( strTemp == "TimerKill" )
	{
		func_string += "QuestTMgr:TimerKill( timer_id )";
	}
	else if( strTemp == "SetCursorHint" )
	{
		func_string += "cursor:SetCursorHint( text );";
	}
	else if( strTemp == "LogErr" )
	{
		func_string += "LogErr( text );";
	}
	else if( strTemp == "GoFadeDown" )
	{
		func_string += "GoFadeDown( fFadeTime );";
	}
	else if( strTemp == "GoFadeDown2" )
	{
		func_string += "GoFadeDown2( fFadeTime, iCurve );";
	}
	else if( strTemp == "GoFadeUp" )
	{
		func_string += "GoFadeUp( fFadeTime );";
	}
	else if( strTemp == "GoFadeUp2" )
	{
		func_string += "GoFadeUp2( fFadeTime, iCurve );";
	}

	//------------------------------------------------------------

	else if( strTemp == "ObjGo2" )
	{
		func_string += "ObjGo2( \"dude\", \"nl_ID\", bDblClick, false );";
	}
	else if( strTemp == "ObjGoSeq" )
	{
		func_string += "ObjGoSeq( \"dude\", \"nl_ID\", bDblClick, true, seq_id );";
	}
	else if( strTemp == "ManTeleportSeq" )
	{
		func_string += "ManTeleportSeq( \"dude\", \"nl_ID\", seq_id );";
	}
	else if( strTemp == "InputEnabledSeq" )
	{
		func_string += "inputEnabledSeq( true, seq_id );";
	}
	else if( strTemp == "IntersectPortal" )
	{
		func_string += "IntersectPortal( \"камера\", \"полы\", bDblClick );";
	}
	else if( strTemp == "SetCam" )
	{
		func_string += "SetCam( \"Camera\" );";
	}
	else if( strTemp == "Scene2Scene" )
	{
		func_string += "Quest_Scene2Scene( \"nl_GO2\", \"SCENE_ID\", \"nl_APPEAR\", L\"\", bDblClick, true );";
	}
	else if( strTemp == "AddObjVideoAnim" )
	{
		func_string += "QuestObjMgr:AddObjVideoAnim( \"anim_ID\" );";
	}
	else if( strTemp == "SetEnabled" )
	{
		func_string += "QuestObjMgr:SetEnabled( \"obj_ID\", false );";
	}
	else if( strTemp == "SetCam2" )
	{
		func_string += "SetCam2( \"Camera\" );";
	}
	else if( strTemp == "TimerSet" )
	{
		func_string += "QuestTMgr:TimerSet( \"timer_ID\", 0.5, \"function_call\" );";
	}
	else if( strTemp == "QuestTMgr:TimerKill" )
	{
		func_string += "QuestTMgr:TimerKill( \"timer_ID\" );";
	}
	else if( strTemp == "SeqAddActInc" )
	{
		func_string += "SeqAddActInc( seq_id, \"script_text\" );";
	}
	else if( strTemp == "SetCursorHint" )
	{
		func_string += "cursor:SetCursorHint( \"hint_text\" );";
	}
	else if( strTemp == "LogErr" )
	{
		func_string += "LogErr( \"\" );";
	}
	else if( strTemp == "Scene2Scene_2" )
	{
		func_string += "Quest_Scene2Scene_2( \"nl_GO2\", \"SCENE_ID\", \"nl_APPEAR\", \"\", bDblClick, true );";
	}
	else if( strTemp == "GoFadeDown" )
	{
		func_string += "GoFadeDown( val );";
	}
	else if( strTemp == "GoFadeDown2" )
	{
		func_string += "GoFadeDown2( val1, val2 );";
	}
	else if( strTemp == "GoFadeUp" )
	{
		func_string += "GoFadeUp( val );";
	}
	else if( strTemp == "GoFadeUp2" )
	{
		func_string += "GoFadeUp2( val1, val2 );";
	}
	else
	{}

	func_string += "\n";

	m_lexer.SendEditor( SCI_INSERTTEXT, -1, (LPARAM)func_string.data() );
}

void CScriptingDlg::OnBnClickedBtnSearch()
{
	UpdateCurrentScript();

	CScriptSearchDlg*	pDlg = new CScriptSearchDlg(this);

	//POSITION	pos		= m_lstObjects.GetFirstSelectedItemPosition();
	//int			nItem	= m_lstObjects.GetNextSelectedItem( pos );
	//str_t		strTemp	= m_lstObjects.GetItemText( nItem, 0 );


	//pDlg->SetCurrSelObject( strTemp );
	//pDlg->SetBinds( m_pBinds );

	pDlg->SetSearchMode( SS_SEARCH );
	pDlg->m_strSearchStr = m_strSearchString;
	pDlg->m_bCurrBind	 = m_bSearchCurrBindOnly;
	pDlg->m_bCaseSens	 = m_bSearchCaseSensitive;

	if( pDlg->DoModal() == IDOK )
	{
		m_strSearchString		= pDlg->m_strSearchStr;
		m_bSearchCurrBindOnly	= pDlg->m_bCurrBind;
		m_bSearchCaseSensitive	= pDlg->m_bCaseSens;
		//m_bFirstSearch			= false;

		//if( !m_bSearchCaseSensitive )
		//	m_strSearchString.MakeUpper();

		ShowNextSearchResult();
	}

	SAFE_DELETE( pDlg );
}

void CScriptingDlg::OnBnClickedBtnSearchAndReplace()
{
	//OnCloseDlg();

	//CScriptSearchDlg*	pDlg = new CScriptSearchDlg(this);

	//POSITION	pos		= m_lstObjects.GetFirstSelectedItemPosition();
	//int			nItem	= m_lstObjects.GetNextSelectedItem( pos );
	//str_t		strTemp	= m_lstObjects.GetItemText( nItem, 0 );
	//
	//pDlg->SetCurrSelObject( strTemp );
	//pDlg->SetBinds( m_pBinds );
	//pDlg->SetSearchMode( SS_SEARCH_REPLACE );
	//pDlg->DoModal();

	//SAFE_DELETE( pDlg );

	////update script window from binds
	//str_t script = GetItemScript( strTemp );

	//script += "\0";

	//m_lexer.SendEditor( SCI_SETTEXT, 0, (LPARAM)script.data() );
	//m_lexer.SetFocus();
}

bool CScriptingDlg::FindInScript( str_t strObject, int& iOffset, int iStartingPos )
{
	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		if( (*iter)[2] == "Scene" && ((*iter)[4] + "." + (*iter)[3]) == strObject )
		{
			if( (*iter).size() > 5 )
			{
				CString script = (*iter)[5].data();

				if( !m_bSearchCaseSensitive )
					script.MakeUpper();
				
				int res = script.Find( m_strSearchString, iStartingPos );

				if( res != -1 )
				{
					iOffset = res;
					return true;
				}
			}
		}

		iter++;
	}

	iOffset = -1;
	return false;
}

void CScriptingDlg::SelectSearchResult( int iItem, int iOffset )
{
	////set focus on obj list
	//m_lstObjects.SetFocus();
	//m_lstObjects.SetItemState( iItem, LVIS_SELECTED, LVIS_SELECTED );
	//m_lstObjects.EnsureVisible( iItem, true );

	////highlight search result
	//m_lexer.SendEditor( SCI_SETSELECTIONSTART, iOffset );
	//m_lexer.SendEditor( SCI_SETSELECTIONEND, iOffset + m_strSearchString.GetLength() );

	//int iLine = m_lexer.SendEditor( SCI_LINEFROMPOSITION, iOffset );

	//m_lexer.SendEditor( SCI_ENSUREVISIBLE, iLine );
	//m_lexer.SendEditor( SCI_ENSUREVISIBLEENFORCEPOLICY, iLine );
	//m_lexer.SendEditor( SCI_SCROLLCARET );

	//m_lexer.SetFocus();
}

void CScriptingDlg::UpdateCurrentScript()
{
	if( m_tvObjects.GetParNode() == NULL )	
		return;	

	CString strItem;

	if( m_tvObjects.GetCurrNodeImage() == 3 )
	{
		strItem = m_tvObjects.GetPar2NodeText() + "." + m_tvObjects.GetParNodeText();
	}
	else
	{
		strItem = m_tvObjects.GetParNodeText() + "." + m_tvObjects.GetCurrNodeText();
	}

	CString	strScript;

	int		len = m_lexer.SendEditor( SCI_GETLENGTH );
	char*	src = new char[len + 1];

	m_lexer.SendEditor( SCI_GETTEXT, len + 1, (LPARAM)src );

	strScript = src;
	strScript.Trim();

	if( strScript == "" )
	{
		if( m_tvObjects.GetCurrNodeImage() == 3 )
			m_tvObjects.SetItemImage( m_tvObjects.GetParNode(), 0, 0 );
		else
			m_tvObjects.SetCurrNodeImage( 0 );
	}
	else
	{
		if( m_tvObjects.GetCurrNodeImage() == 3 )
			m_tvObjects.SetItemImage( m_tvObjects.GetParNode(), 1, 1 );
		else
			m_tvObjects.SetCurrNodeImage( 1 );	
	}

	SetItemScript( strItem.GetBuffer(), src );

	SAFE_DELETE_ARRAY( src );
}

void CScriptingDlg::ShowCurrentBind( CString strParentID, CString strParentType, CString strChildID, CString strChildType )
{
	//update filters
	bool bFilterUpdateNeeded = true;

	if( m_cmbFilterParentType.GetCurSel() == m_cmbFilterParentType.FindString( 0, strParentType ) )
	{
		CString strID2;

		int n2 = m_cmbFilterParentID.GetLBTextLen( m_cmbFilterParentID.GetCurSel() );
		m_cmbFilterParentID.GetLBText( m_cmbFilterParentID.GetCurSel(), strID2 );

		strID2.GetBuffer();
		int iOff = strID2.ReverseFind('.');

		if( iOff != -1 )
		{
			CString strID3;
			strID3.Append( strID2.Right( strID2.GetLength() - iOff - 1 ) );

			if( strID3 == strParentID )
				bFilterUpdateNeeded = false;
		}
	}

	if( bFilterUpdateNeeded )
	{
		int idx		= m_cmbFilterParentType.FindString( 0, strParentType );
		int idx2	= CB_ERR;

		for( int i = 0; i < m_cmbFilterParentID.GetCount(); i++ )
		{
			CString strID;
			str_t strTemp;
			int n = m_cmbFilterParentID.GetLBTextLen( i );
			m_cmbFilterParentID.GetLBText( i, strID.GetBuffer(n) );

			strTemp = strID.GetBuffer();
			StripTypeHeader( strTemp );
			strID = strTemp.data();

			if( strID == strParentID )
			{
				idx2 = i;
				strID.ReleaseBuffer();
				break;
			}

			strID.ReleaseBuffer();
		}

		if( idx == CB_ERR || idx2 == CB_ERR )
		{
			return;
		}

		m_cmbFilterParentType.SetCurSel( idx );
		OnCbnCloseupCmbFilterType();

		m_cmbFilterParentID.SetCurSel( idx2 );
		OnCbnCloseupCmbFilterId();
	}

	//update treeview
	HTREEITEM hGroup = m_tvObjects.FindTreeItemByName( strChildType.GetBuffer(), NULL, false );
	m_tvObjects.Expand( hGroup, TVE_EXPAND );
	m_tvObjects.EnsureVisible( hGroup );

	HTREEITEM hObj = m_tvObjects.FindTreeItemByName( strChildID.GetBuffer(), hGroup, false );
	m_tvObjects.Expand( hObj, TVE_EXPAND );
	m_tvObjects.EnsureVisible( hObj );

	m_tvObjects.SelectItem( hObj );
	m_tvObjects.UpdateWindow();
}

void CScriptingDlg::UpdateParentChildFilterFromBindCounter( int iBindCounter )
{
	int iCounter = 0;

	CString sPI, sPT, sCI, sCT;

	ListStrVec_iter iter = m_pBinds->begin();
	while( iter != m_pBinds->end() )
	{
		if( iCounter == iBindCounter )
		{
			sPI = (*iter)[1].data();
			sPT = (*iter)[2].data();
			sCI = (*iter)[3].data();
			sCT = (*iter)[4].data();
		}

		iter++;
		iCounter++;
	}

	ShowCurrentBind( sPI, sPT, sCI, sCT );
}

void CScriptingDlg::ShowNextSearchResult()
{
	if( m_strSearchString == "" )
		return;

	int	iCursorPos	= m_lexer.SendEditor( SCI_GETCURRENTPOS );
	int iResBindCounter;
	int iResScriptOffset;

	if( FindNextResultInScripts( m_iCurrentlySearchedBind, iCursorPos, m_strSearchString, iResBindCounter, iResScriptOffset ) )
	{
		if( m_iCurrentlySearchedBind != iResBindCounter )
			UpdateParentChildFilterFromBindCounter( iResBindCounter );

		m_iCurrentlySearchedBind = iResBindCounter;

		m_lexer.SendEditor( SCI_SETSELECTIONSTART, iResScriptOffset );
		m_lexer.SendEditor( SCI_SETSELECTIONEND, iResScriptOffset + m_strSearchString.GetLength() );

		int iLine = m_lexer.SendEditor( SCI_LINEFROMPOSITION, iResScriptOffset );

		m_lexer.SendEditor( SCI_ENSUREVISIBLE, iLine );
		m_lexer.SendEditor( SCI_ENSUREVISIBLEENFORCEPOLICY, iLine );
		m_lexer.SendEditor( SCI_SCROLLCARET );

		m_lexer.SetFocus();
		m_lexer.SendEditor( SCI_GRABFOCUS );
	}
	else
	{
		MessageBox( "Search produced no results!", "Information", MB_OK | MB_ICONINFORMATION );
	}

	//int iCounter = 0;

	//ListStrVec_iter iter = m_pBinds->begin();

	//while( iter != m_pBinds->end() )
	//{
	//	if( iCounter == m_iCurrentlySearchedBind && (*iter).size() > 5 )
	//	{
	//		AfxMessageBox( (*iter)[5].data() );
	//		
	//	}

	//	/*if( (*iter)[1] == (str_t)m_strFilterParID.GetBuffer() &&
	//		(*iter)[2] == (str_t)m_strFilterParType.GetBuffer() && 
	//		(*iter)[3] == (str_t)m_tvObjects.GetParNodeText().GetBuffer() &&
	//		(*iter)[4] == (str_t)m_tvObjects.GetParNodeText().GetBuffer() )
	//	{
	//		break;
	//	}*/

	//	iCounter++;
	//}

	//m_iCurrentlySearchedBind = iCounter;

	//ListStrVec_iter iter = m_pBinds->begin();

	//while( iter != m_pBinds->end() )
	//{
	//	if( (*iter)[2] == "Scene" && ((*iter)[4] + "." + (*iter)[3]) == strObject )
	//	{
	//		if( (*iter).size() > 5 )
	//		{}
	//	}
	//}

	//m_iCurrentlySearchedBind

	//if( m_bFirstSearch )
	//	return;

	//UpdateCurrentScript();

	//POSITION	pos		= m_lstObjects.GetFirstSelectedItemPosition();
	//int			nItem	= m_lstObjects.GetNextSelectedItem( pos );
	//int			iStart	= nItem;
	//int			iEnd	= nItem + 1;
	//bool		bFound	= false;
	//int			iCursorPos = m_lexer.SendEditor( SCI_GETCURRENTPOS );
	//int			iOffset	= 0;


	//if( !m_bSearchCurrBindOnly )
	//{
	//	iStart	= nItem;
	//	iEnd	= m_lstObjects.GetItemCount();
	//}

	//for( int i = iStart; i < iEnd; i++ )
	//{
	//	CString item = m_lstObjects.GetItemText( i, 0 ); 
	//
	//	if( i > iStart )
	//		iCursorPos = 0;

	//	if( FindInScript( item.GetBuffer(), iOffset, iCursorPos ) )
	//	{
	//		bFound = true;

	//		SelectSearchResult( i, iOffset );
	//		
	//		break;
	//	}
	//}

	//if( bFound )
	//	return;

	//if( !m_bSearchCurrBindOnly )
	//{
	//	iStart	= 0;
	//	iEnd	= nItem + 1;
	//}

	//for( int i = iStart; i < iEnd; i++ )
	//{
	//	CString item = m_lstObjects.GetItemText( i, 0 ); 

	//	if( iStart + 1 == iEnd )
	//		iCursorPos = 0;

	//	if( FindInScript( item.GetBuffer(), iOffset, iCursorPos ) )
	//	{
	//		bFound = true;

	//		SelectSearchResult( i, iOffset );

	//		break;
	//	}
	//}

	//if( !bFound )
	//{
	//	MessageBox( "No entries found." );
	//}
}

void CScriptingDlg::ShowLastSelObject()
{
	str_t item = m_strLastSelObj;
	str_t type = StripTypeHeader( item );
	
	::EnableWindow( m_lexer.m_hEditor, false );
	m_lexer.Disable();

	HTREEITEM hRoot = m_tvObjects.FindTreeItemByName( type );

	if( hRoot == NULL )
		return;

	HTREEITEM hItem = m_tvObjects.FindTreeItemByName( item, hRoot );

	if( hItem == NULL )
		return;
	
	m_tvObjects.SelectItem( hItem );
	m_tvObjects.SetFocus();

	::EnableWindow( m_lexer.m_hEditor, true );
	m_lexer.SetLUALex();
}

void CScriptingDlg::OnShowWindow( BOOL bShow, UINT nStatus )
{
	CDialog::OnShowWindow(bShow, nStatus);

	this->SetWindowText( m_strFilePath.data() );
	UpdateUISizing();

	ShowLastSelObject();
}

void CScriptingDlg::CommentSelectedBlock()
{
	m_lexer.CommentSelectedBlock();
}

void CScriptingDlg::UncommentSelectedBlock()
{
	m_lexer.UncommentSelectedBlock();
}

void CScriptingDlg::OnTimer(UINT nIDEvent)
{
	switch( nIDEvent )
	{
		case ID_SCRIPTING_TIMER:
			JumpToToScript();
			KillTimer( ID_SCRIPTING_TIMER );
			break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CScriptingDlg::OnCbnSelchangeCmbFilterType()
{

}

void CScriptingDlg::OnCbnSelchangeCmbFilterId()
{

}

void CScriptingDlg::OnTvnSelchangingTreeObjects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	*pResult = 0;

	if( m_bIsDialogClosing || m_bIsRefreshingFuncsBindings )
		return;

	UpdateCurrentScript();
}

void CScriptingDlg::OnTvnSelchangedTreeObjects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	*pResult = 0;

	if( m_bIsDialogClosing || m_bIsRefreshingFuncsBindings )
		return;

	if( m_tvObjects.GetParNode() == NULL )
	{
		m_strLastSelObj = "";
		::EnableWindow( m_lexer.m_hEditor, false );
		m_lexer.Disable();

		m_lexer.SendEditor( SCI_SETTEXT, 0, (LPARAM)"" );
		m_lexer.SendEditor( SCI_SETXOFFSET, 0 );
		m_lexer.SendEditor( SCI_EMPTYUNDOBUFFER );

		return;
	}

	CString strItem;

	if( m_tvObjects.GetCurrNodeImage() == 3 )
	{
		strItem = m_tvObjects.GetPar2NodeText() + "." + m_tvObjects.GetParNodeText();
	}
	else
	{
		strItem = m_tvObjects.GetParNodeText() + "." + m_tvObjects.GetCurrNodeText();
	}

	m_lexer.SendEditor( SCI_SETTEXT, 0, (LPARAM)GetItemScript( strItem.GetBuffer() ).data() );
	m_lexer.SendEditor( SCI_SETXOFFSET, 0 );
	m_lexer.SendEditor( SCI_EMPTYUNDOBUFFER );

	m_strLastSelObj = strItem;

	CEdit* lex = (CEdit*)CWnd::FromHandle( m_lexer.m_hEditor );
	lex->EnableWindow( true );
	m_lexer.SetLUALex();

	if( m_tvObjects.GetCurrNodeImage() == 3 )
	{
		TNODE* pNodeData = (TNODE*)m_tvObjects.GetCurrNodeData();

		if( pNodeData && pNodeData->_data )
		{
			FuncEntry* pEntry = (FuncEntry*)pNodeData->_data;
			//m_lexer.SetFocus();
			//m_lexer.SendEditor( SCI_GRABFOCUS );
			m_lexer.SendEditor( SCI_GOTOPOS, pEntry->iOffset );
			m_lexer.SendEditor( SCI_SETSELECTIONSTART, pEntry->iOffset );
			
			int iLine = m_lexer.SendEditor( SCI_LINEFROMPOSITION, pEntry->iOffset );
			m_lexer.SendEditor( SCI_SETSELECTIONEND, m_lexer.SendEditor( SCI_GETLINEENDPOSITION, iLine ) );


			int iClass = -1;

			if( pEntry->strClassName != "" )
				iClass = m_cmbExClasses.FindStringExact( 0, pEntry->strClassName );
			else
				iClass = m_cmbExClasses.FindStringExact( 0, "(Globals)" );

			if( iClass != -1 )
				m_cmbExClasses.SetCurSel( iClass );

			int iFunc = m_cmbExFuncs.FindStringExact( 0, pEntry->strFuncName );

			if( iFunc != -1 )
				m_cmbExFuncs.SetCurSel( iFunc );
		}
	}

	m_tvObjects.SetFocus();
	m_tvObjects.UpdateWindow();

	m_iCurrentlySearchedBind = GetCurrentBindCounter();
}

//////////////////////////////////////////////////////////////////////
// function:
// FindNextResultinScripts
//
// params:
// int iCurrentBindCounter	-- sequential number of current bind item
// int iScriptOffset		-- cursor offset in binds script
// CString strSearch		-- search string
// int& iResBindCounter		-- passes back resulting sequential number of bind where the search result was found, -1 otherwise
// int& iResScriptOffset	-- passes back resulting cursor offset in binds script where the search result was found, -1 otherwise
//
// return value: true if search string was found, false otherwise
//////////////////////////////////////////////////////////////////////
bool CScriptingDlg::FindNextResultInScripts( int iCurrentBindCounter, int iScriptOffset, CString strSearch, int& iResBindCounter, int& iResScriptOffset )
{
	if( !m_bSearchCaseSensitive )
		strSearch = strSearch.MakeUpper();

	int iCounter = 0;
	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		if( iCounter >= iCurrentBindCounter && (*iter).size() > 5 )
		{
			CString strScript = (*iter)[5].data();

			int iResOff = -1;

			if( m_bSearchCaseSensitive )
            	iResOff = strScript.Find( strSearch, iScriptOffset );
			else
			{
				strScript = strScript.MakeUpper();
				iResOff = strScript.Find( strSearch, iScriptOffset );
			}

			if( iResOff != -1 )
			{
				iResBindCounter	 = iCounter;
				iResScriptOffset = iResOff;

				return true;
			}

			//only first script takes cursor position into account
			if( iScriptOffset != 0 )
				iScriptOffset = 0;

			if( m_bSearchCurrBindOnly )
			{
				iResOff = -1;

				if( m_bSearchCaseSensitive )
            		iResOff = strScript.Find( strSearch, iScriptOffset );
				else
				{
					strScript = strScript.MakeUpper();
					iResOff = strScript.Find( strSearch, iScriptOffset );
				}

				if( iResOff != -1 )
				{
					iResBindCounter	 = iCounter;
					iResScriptOffset = iResOff;

					return true;
				}
				else
					return false;
			}
		}

		iter++;
		iCounter++;
	}

	iter = m_pBinds->begin();
	iCounter = 0;
	while( iter != m_pBinds->end() )
	{
		if( iCounter <= iCurrentBindCounter && (*iter).size() > 5 )
		{
			CString strScript = (*iter)[5].data();

			int iResOff = -1;

			if( m_bSearchCaseSensitive )
            	iResOff = strScript.Find( strSearch, 0 );
			else
			{
				strScript = strScript.MakeUpper();
				iResOff = strScript.Find( strSearch, 0 );
			}

			if( iResOff != -1 )
			{
				iResBindCounter	 = iCounter;
				iResScriptOffset = iResOff;

				return true;
			}
		}

		iter++;
		iCounter++;
	}

	iResBindCounter	 = -1;
	iResScriptOffset = -1;

	return false;
}

int CScriptingDlg::GetCurrentBindCounter()
{
	if( m_tvObjects.GetCurrNodeImage() == 3 || m_tvObjects.GetCurrNodeImage() == 1 )
	{
		int iCounter = 0;

		str_t sPI, sPT, sCI, sCT;

		if( m_tvObjects.GetCurrNodeImage() == 3 )
		{
			sPI = (str_t)m_strFilterParID.GetBuffer();
			sPT = (str_t)m_strFilterParType.GetBuffer();
			sCI = (str_t)m_tvObjects.GetParNodeText().GetBuffer();
			sCT = (str_t)m_tvObjects.GetPar2NodeText().GetBuffer();
		}
		else
		{
			sPI = (str_t)m_strFilterParID.GetBuffer();
			sPT = (str_t)m_strFilterParType.GetBuffer();
			sCI = (str_t)m_tvObjects.GetCurrNodeText().GetBuffer();
			sCT = (str_t)m_tvObjects.GetParNodeText().GetBuffer();		
		}

		ListStrVec_iter iter = m_pBinds->begin();

		while( iter != m_pBinds->end() )
		{
			if( (*iter)[1] == sPI && (*iter)[2] == sPT && (*iter)[3] == sCI && (*iter)[4] == sCT )
			{
				break;
			}

			iter++;
			iCounter++;
		}

		return iCounter;
	}

	return -1;
}

HBRUSH CScriptingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//if( nCtlColor == CTLCOLOR_EDIT )
	//pDC->SetDCBrushColor( RGB(255, 0, 0) );

	return hbr;
}

void CScriptingDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void CScriptingDlg::OnCbnCloseupComboboxexFuncs()
{
	CString strFunction, strClass;
	FuncEntry* pEntry = NULL;

	int nFuncsSel = m_cmbExFuncs.GetLBTextLen( m_cmbExFuncs.GetCurSel() );
	m_cmbExFuncs.GetLBText( m_cmbExFuncs.GetCurSel(), strFunction.GetBuffer( nFuncsSel ) );

	int nClassesSel = m_cmbExClasses.GetLBTextLen( m_cmbExClasses.GetCurSel() );
	m_cmbExClasses.GetLBText( m_cmbExClasses.GetCurSel(), strClass.GetBuffer( nClassesSel ) );

	if( strClass == "(Globals)" )
	{
		strClass.ReleaseBuffer();
		strClass = "";
	}

	for( IFuncMap_iter i = m_pFuncs->begin(); i != m_pFuncs->end(); ++i )
	{
		if( i->second->strClassName == strClass && i->second->strFuncName == strFunction )
		{
			pEntry = i->second;
			break;
		}
	}

	if( !pEntry )
	{
		strFunction.ReleaseBuffer();
		strClass.ReleaseBuffer();
		return;
	}

	//get parent type and id from binds
	CString strParentID, strParentType, strChildID, strChildType;
	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		int iBindID = atoi((*iter)[0].data());

		if( iBindID == pEntry->iBind )
		{
			strParentID		= (*iter)[1].data();
			strParentType	= (*iter)[2].data();
			strChildID		= (*iter)[3].data();
			strChildType	= (*iter)[4].data();
			break;
		}

		iter++;
	}

	ShowCurrentBind( strParentID, strParentType, strChildID, strChildType );

	//select needed obj group and obj
	HTREEITEM hGroup = m_tvObjects.FindTreeItemByName( strChildType.GetBuffer(), NULL, false );
	HTREEITEM hObj = m_tvObjects.FindTreeItemByName( strChildID.GetBuffer(), hGroup, false );
	HTREEITEM hFunc = m_tvObjects.FindTreeItemByName( pEntry->strFuncName, hObj, false );
	m_tvObjects.SelectItem( hFunc );
	m_tvObjects.UpdateWindow();

	strFunction.ReleaseBuffer();
	strClass.ReleaseBuffer();
}

void CScriptingDlg::OnCbnCloseupCmbFilterType()
{
	str_t func_string, tmp;

	UpdateCurrentScript();

	tmp = m_strFilterParID.GetBuffer();

	m_cmbFilterParentType.GetLBText( m_cmbFilterParentType.GetCurSel(), m_strFilterParType );
	m_cmbFilterParentID.GetLBText( m_cmbFilterParentID.GetCurSel(), m_strFilterParID );

	tmp = m_strFilterParID.GetBuffer();
	StripTypeHeader( tmp );
	m_strFilterParID = tmp.data();

	PopulateObjectsList();
}

void CScriptingDlg::OnCbnCloseupCmbFilterId()
{
	str_t func_string, tmp;

	UpdateCurrentScript();

	tmp = m_strFilterParID.GetBuffer();

	m_cmbFilterParentType.GetLBText( m_cmbFilterParentType.GetCurSel(), m_strFilterParType );
	m_cmbFilterParentID.GetLBText( m_cmbFilterParentID.GetCurSel(), m_strFilterParID );

	m_strFilterParID_full = m_strFilterParID;

	tmp = m_strFilterParID.GetBuffer();
	StripTypeHeader( tmp );
	m_strFilterParID = tmp.data();

	PopulateObjectsList();
}
