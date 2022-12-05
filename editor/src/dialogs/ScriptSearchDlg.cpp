#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "ScriptSearchDlg.h"

#pragma warning(disable: 4800)

IMPLEMENT_DYNAMIC(CScriptSearchDlg, CDialog)
CScriptSearchDlg::CScriptSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptSearchDlg::IDD, pParent)
{
	m_searchMode	= SS_SEARCH;
	m_pBinds		= NULL;
}

CScriptSearchDlg::~CScriptSearchDlg()
{
}

void CScriptSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScriptSearchDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CScriptSearchDlg message handlers

void CScriptSearchDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( m_searchMode == SS_SEARCH )
	{
		CWnd* pWnd = GetDlgItem( IDC_EDIT_REPLACE );
		pWnd->EnableWindow( false );
	}

	CButton* pBtnCurrBind	= (CButton*)GetDlgItem( IDC_CHK_CURR_BIND );
	CButton* pBtnCaseSens	= (CButton*)GetDlgItem( IDC_CHK_CASE );

	//pBtnCurrBind->SetCheck( true );
	//pBtnCaseSens->SetCheck( true );

	
	//get data from clipboard and set it to searchbox
	/*str_t strClipboardText;

	::OpenClipboard( this->GetSafeHwnd() );

	HGLOBAL hglb = GetClipboardData( CF_TEXT );

	if( hglb )
	{
		strClipboardText = (char *)GlobalLock( hglb );
		GlobalUnlock( hglb );
	}

	::CloseClipboard();*/

	if( m_bCurrBind )
		pBtnCurrBind->SetCheck( true );

	if( m_bCaseSens )
		pBtnCaseSens->SetCheck( true );

	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_FIND );
	pEdit->SetWindowText( m_strSearchStr );
}

void CScriptSearchDlg::SearchScripts()
{
	CButton*	pBtnCurrBind	= (CButton*)GetDlgItem( IDC_CHK_CURR_BIND );
	CButton*	pBtnCaseSens	= (CButton*)GetDlgItem( IDC_CHK_CASE );

	m_bCurrBind	= (bool)pBtnCurrBind->GetCheck();
	m_bCaseSens	= (bool)pBtnCaseSens->GetCheck();

	OnOK();
}

void CScriptSearchDlg::SearchAndReplaceScripts()
{
	CButton*	pBtnCurrBind	= (CButton*)GetDlgItem( IDC_CHK_CURR_BIND );
	CButton*	pBtnCaseSens	= (CButton*)GetDlgItem( IDC_CHK_CASE );
	
	int			counter = 0;
	CString		strTemp;

	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		if( (*iter)[2] == "Scene" )
		{
			CString script_str = "";
			CString script_str_original;

			if( (*iter).size() > 5 )
			{
				script_str = (*iter)[5].data();
				script_str = script_str.Trim();
			}

			script_str_original = script_str;

			if( !pBtnCaseSens->GetCheck() )
			{
				script_str.MakeUpper();
				m_strSearchStr.MakeUpper();
			}


			if( script_str != "" )
			{
				if( pBtnCurrBind->GetCheck() )
				{
					if( m_strCurrSelObj == ( (*iter)[4] + "." + (*iter)[3] ) )
					{
						int		start_idx = 0;

						while( start_idx < script_str.GetLength() && start_idx != -1 )
						{
							start_idx = script_str.Find( m_strSearchStr.GetBuffer(), start_idx );

							if( start_idx == -1 )
								break;

							CString find_what = script_str_original.Mid( start_idx, m_strSearchStr.GetLength() );
							script_str_original.Replace( find_what, m_strReplaceStr );

							start_idx += m_strSearchStr.GetLength();
							counter++;
						}

						(*iter)[5] = script_str_original.GetBuffer();
					}
				}
				else
				{
					int	start_idx = 0;

					while( start_idx < script_str.GetLength() && start_idx != -1 )
					{
						start_idx = script_str.Find( m_strSearchStr.GetBuffer(), start_idx );

						if( start_idx == -1 )
							break;

						CString find_what = script_str_original.Mid( start_idx, m_strSearchStr.GetLength() );
						script_str_original.Replace( find_what, m_strReplaceStr );

						start_idx += m_strSearchStr.GetLength();

						counter++;
					}

					(*iter)[5] = script_str_original.GetBuffer();
				}
			}
		}

		iter++;
	}

	strTemp.Format( "%i replacements made.", counter );
	
	MessageBox( strTemp, "Информация", MB_OK | MB_ICONINFORMATION );
}

void CScriptSearchDlg::OnBnClickedOk()
{
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_FIND );
	pEdit->GetWindowText( m_strSearchStr );

	CEdit* pEdit2 = (CEdit*)GetDlgItem( IDC_EDIT_REPLACE );
	pEdit2->GetWindowText( m_strReplaceStr );

	//m_strSearchStr.Trim();

	if( m_strSearchStr == "" )
		return;

	if( !m_pBinds )
		OnOK();	



	if( m_searchMode == SS_SEARCH )
	{
		SearchScripts();
	}
	else if( m_searchMode == SS_SEARCH_REPLACE )
	{
		SearchAndReplaceScripts();
	}
	else
	{}

	OnOK();
}

BOOL CScriptSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
		case WM_KEYDOWN:

			if( pMsg->wParam == VK_ESCAPE )
				return false;
			break;

		case WM_KEYUP:

			if( pMsg->wParam == VK_ESCAPE )
			{
				OnBnClickedCancel();
				return true;
			}
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CScriptSearchDlg::OnBnClickedCancel()
{
	OnCancel();
}
