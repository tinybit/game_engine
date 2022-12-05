#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "AnimScriptingDlg.h"




IMPLEMENT_DYNAMIC(CAnimScriptingDlg, CDialog)
CAnimScriptingDlg::CAnimScriptingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimScriptingDlg::IDD, pParent)
{
	m_pEfnAnimation = NULL;
}

CAnimScriptingDlg::~CAnimScriptingDlg()
{
}

void CAnimScriptingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RCH_SCRIPTSRC, m_rchScriptSrc);
	DDX_Control(pDX, IDC_LST_FRAMES, m_lstFrames);
}


BEGIN_MESSAGE_MAP(CAnimScriptingDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_FRAMES, OnLvnItemchangedLstFrames)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAnimScriptingDlg message handlers

BOOL CAnimScriptingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString temp;

	/* setup rich edit ctrl */
	//set richedit parent wnd
	m_rchScriptSrc.SetParent( this );

	//prepare char format for script control
	CHARFORMAT2 chf;
	ZeroMemory( &chf, sizeof(CHARFORMAT2) );
	chf.dwMask	= CFM_FACE | CFM_SIZE;
	chf.cbSize		= sizeof(CHARFORMAT2);
	chf.yHeight	= 250;
	//chf.bPitchAndFamily = FF_MODERN;

	sprintf( chf.szFaceName, "%s", "Courier New" );
	m_rchScriptSrc.SetDefaultCharFormat( chf );

	// Modify the paragraph format
	PARAFORMAT2 pf;
	pf.dwMask = PFM_TABSTOPS;
	pf.cTabCount = 1;
	pf.rgxTabs[0] = 0x222;
	m_rchScriptSrc.SetParaFormat( pf );



	m_lstFrames.DeleteAllItems();
	m_lstFrames.InsertColumn( 0, "Frame #", LVCFMT_LEFT, 67, -1 );
	m_lstFrames.SetExtendedStyle( m_lstFrames.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	//prepare imagelist
	CBitmap bitmap;
	m_ImageList.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );
	
	//0 script
	bitmap.LoadBitmap( IDB_SCRIPT );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();


	//1 empty
	bitmap.LoadBitmap( IDB_EMPTY );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();

	m_lstFrames.SetImageList( &m_ImageList, LVSIL_SMALL );

	if( !m_pEfnAnimation )
		return TRUE;

	int frm_cnt = m_pEfnAnimation->GetFramesCount();

	for( int i = 0; i < frm_cnt; i++ )
	{
		temp.Format( "%i", i );
		
		if( m_pEfnAnimation->GetFrameScript( i ) == "" )
			m_lstFrames.InsertItem( i, temp, 1 );
		else
			m_lstFrames.InsertItem( i, temp, 0 );
	}
	
	//load first item script
	m_rchScriptSrc.SetSel( 0, -1 );
	m_rchScriptSrc.ReplaceSel( m_pEfnAnimation->GetFrameScript( 0 ).data() , false );

	m_lstFrames.SetFocus();
	m_lstFrames.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
	m_lstFrames.EnsureVisible( 0, true );

	return TRUE;
}

void CAnimScriptingDlg::SetEfn( CEfnCharacter* pEfnAnimation )
{
	m_pEfnAnimation = pEfnAnimation;
}

void CAnimScriptingDlg::OnBnClickedBtnClose()
{
	this->SendMessage( WM_CLOSE, 0, 0 );
}

void CAnimScriptingDlg::OnLvnItemchangedLstFrames( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	 
	static int oldItem = -1;

	// select complete script, see rich text ctrl docs
	m_rchScriptSrc.SetSel( 0, -1 );  

	if( oldItem != -1 )
	{
		CString script_text = m_rchScriptSrc.GetSelText();

		LV_ITEM list_item;
		ZeroMemory( &list_item, sizeof( LV_ITEM ) );
		list_item.mask = LVIF_IMAGE;
		list_item.iItem = oldItem;
		list_item.iSubItem = 0;

		script_text.Trim();

		if( script_text == "" )
            list_item.iImage = 1;
		else
			list_item.iImage = 0;

		m_lstFrames.SetItem( &list_item );

		m_pEfnAnimation->SetFrameScript( oldItem, script_text.GetBuffer(0) );
	}

	m_rchScriptSrc.SetSel( 0, -1 );

	//load new item script
	m_rchScriptSrc.ReplaceSel( m_pEfnAnimation->GetFrameScript( pNMLV->iItem ).data() , false );

	oldItem = pNMLV->iItem;

	*pResult = 0;
}

void CAnimScriptingDlg::OnClose()
{
	// select complete script, see rich text ctrl docs
	m_rchScriptSrc.SetSel( 0, -1 );

	POSITION pos = m_lstFrames.GetFirstSelectedItemPosition();
	int nItem = m_lstFrames.GetNextSelectedItem( pos );

	m_pEfnAnimation->SetFrameScript( nItem, m_rchScriptSrc.GetSelText().GetBuffer(0) );
	
	m_lstFrames.SetFocus();
	m_lstFrames.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
	m_lstFrames.EnsureVisible( 0, true );

	CDialog::OnClose();
}

void CAnimScriptingDlg::OnMenuXSIMeshDisconnectAllTris()
{
	// TODO: Add your command handler code here
}
