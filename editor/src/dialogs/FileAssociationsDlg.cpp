#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "FileAssociationsDlg.h"



IMPLEMENT_DYNAMIC(CFileAssociationsDlg, CDialog)
CFileAssociationsDlg::CFileAssociationsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileAssociationsDlg::IDD, pParent)
{
}

CFileAssociationsDlg::~CFileAssociationsDlg()
{
	// cleanup
	DeleteFile( "c:\\341345154464asdgfafbg.reg" );
}

void CFileAssociationsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
}


BEGIN_MESSAGE_MAP(CFileAssociationsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CFileAssociationsDlg message handlers

BOOL CFileAssociationsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_tree.ModifyStyle( TVS_CHECKBOXES, 0 );
	m_tree.ModifyStyle( 0, TVS_CHECKBOXES );

	xsi = m_tree.InsertItem( "dotXSI Files", NULL, NULL );
	efn = m_tree.InsertItem( "EFN Files", NULL, NULL );
	scn = m_tree.InsertItem( "SCN Files", NULL, NULL );

	m_tree.SetCheck( xsi );
	m_tree.SetCheck( efn );
	m_tree.SetCheck( scn );

	return TRUE;
}

void CFileAssociationsDlg::SetAppPath( CString strPath )
{
	m_strappPath = strPath;
}

void CFileAssociationsDlg::OnBnClickedOk()
{
	//remove associations
	m_strappPath.Replace( "\\", "\\\\" );

	//remove file if exists
	if( FileExists( "c:\\341345154464asdgfafbg.reg" ) )
		DeleteFile( "c:\\341345154464asdgfafbg.reg" );

	if( !m_tree.GetCheck( xsi ) && !m_tree.GetCheck( efn ) && !m_tree.GetCheck( scn ) )
		return;

	FILE* file;

	file = fopen( "c:\\341345154464asdgfafbg.reg", "wc" );

	fprintf( file, "%s\n\n", "Windows Registry Editor Version 5.00" );
	
	if( m_tree.GetCheck( efn ) )
	{
		fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\.efn]" );
		fprintf( file, "%s\n\n\n", "@=\"CookieMachine.File\"" );
	}

	if( m_tree.GetCheck( scn ) )
	{
		fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\.scn]" );
		fprintf( file, "%s\n\n\n", "@=\"CookieMachine.File\"" );
	}

	if( m_tree.GetCheck( xsi ) )
	{
		fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\.xsi]" );
		fprintf( file, "%s\n\n\n", "@=\"CookieMachine.File\"" );
	}

	fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\CookieMachine.File]" );
	fprintf( file, "%s\n", "@=\"Cookie Machine file\"" );
	fprintf( file, "%s\n", "\"EditFlags\"=dword:00000000" );
	fprintf( file, "%s\n\n", "\"BrowserFlags\"=dword:00000008" );

	fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\CookieMachine.File\\DefaultIcon]" );
	fprintf( file, "%s%s%s\n\n", "@=\"", m_strappPath.GetBuffer(), "\\\\Cookie Machine.exe,0\"" );

	fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\CookieMachine.File\\shell]" );
	fprintf( file, "%s\n\n", "@=\"\"" );

	fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\CookieMachine.File\\shell\\open]" );
	fprintf( file, "%s\n\n", "@=\"\"" );

	fprintf( file, "%s\n", "[HKEY_CLASSES_ROOT\\CookieMachine.File\\shell\\open\\command]" );
	fprintf( file, "%s%s%s\n\n", "@=\"\\\"", m_strappPath.GetBuffer(), "\\\\Cookie Machine.exe\\\" \\\"%1\\\"\"" );

	fclose( file );
	
	// exec reg file in silent mode
	WinExec( "regedit -s c:\\341345154464asdgfafbg.reg", 0 );

	OnOK();
}
