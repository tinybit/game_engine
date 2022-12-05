#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "TextResourcesDlg.h"
#include "CookieMachineDlg.h"
#include ".\textresourcesdlg.h"


IMPLEMENT_DYNAMIC(CTextResourcesDlg, CDialog)
CTextResourcesDlg::CTextResourcesDlg(CWnd* pParent /*=NULL*/) : CDialog(CTextResourcesDlg::IDD, pParent)
{
	m_pParent			= pParent;
	m_nID				= CTextResourcesDlg::IDD;
	m_iMinWindowSizeX	= 340;
	m_iMinWindowSizeY	= 500;
	m_hAccelerator		= LoadAccelerators( AfxGetResourceHandle(), MAKEINTRESOURCE( IDR_ACCELERATOR_TEXT_RES_DLG ) );
	m_dlgVal			= NULL;
	m_strInitialItem	= "";
	m_iErrLine			= -1;

	Kill();
}

CTextResourcesDlg::~CTextResourcesDlg()
{

}

BOOL CTextResourcesDlg::Create()
{
	if( !m_pScene )
   		return false;

	return CDialog::Create( m_nID, m_pParent );
}

void CTextResourcesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_OBJECT, m_lstObjects);
}

BEGIN_MESSAGE_MAP(CTextResourcesDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()

	//accelerators
	ON_COMMAND(ID_ACCL_SAVE, SaveFile)
	ON_COMMAND(ID_ACCL_ENGINE, RunEngine)
	ON_COMMAND(ID_ACCL_ENGINE_CURR, RunEngineCurr)
	ON_COMMAND(ID_ACCL_SRIPTS_ED, ShowScriptsEdWnd)
	ON_COMMAND(ID_ACCL_DEL, DeleteKeyPressed )
	

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_OBJECT, OnLvnItemchangedLstObject)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LST_OBJECT, OnLvnColumnclickLstObject)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LST_OBJECT, OnLvnEndlabeleditLstObject)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_OBJECT, OnNMDblclkLstObject)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_NOTIFY(NM_CLICK, IDC_LST_OBJECT, OnNMClickLstObject)
END_MESSAGE_MAP()


void CTextResourcesDlg::DeleteSelectedItems()
{
	POSITION pos = m_lstObjects.GetFirstSelectedItemPosition();
	char	buff[512];
	int		iFirstSelPos = -1;

	if( pos != NULL )
	{
		iFirstSelPos = m_lstObjects.GetNextSelectedItem( pos );
		pos = m_lstObjects.GetFirstSelectedItemPosition();

		while( pos )
		{
			pos = m_lstObjects.GetFirstSelectedItemPosition();
			int nItem = m_lstObjects.GetNextSelectedItem( pos );
			m_lstObjects.GetItemText( nItem, 0, buff, 512 );

			((CSceneFile*)m_pScene)->DeleteTextResource( ((CSceneFile*)m_pScene)->GetTextResourceByName( buff ) );

			m_lstObjects.DeleteItem( nItem );
		}
	}

	if( iFirstSelPos != -1 && m_lstObjects.GetItemCount() > 0 )
	{
		if( iFirstSelPos - 1 >= 0 )
			iFirstSelPos--;

		m_lstObjects.SetItemState( iFirstSelPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED  );
	}

	m_lstObjects.SetFocus();

	if( m_lstObjects.GetItemCount() == 0 )
	{
		m_lexer.SendEditor( SCI_SETTEXT, 0, (LPARAM)"" );
		m_lexer.SendEditor( SCI_SETXOFFSET, 0 );
		m_lexer.SendEditor( SCI_EMPTYUNDOBUFFER );
	}
}

void CTextResourcesDlg::DeleteKeyPressed()
{
	CWnd* pWnd = GetFocus();

	if( pWnd && pWnd->GetDlgCtrlID() != IDC_LST_OBJECT && pWnd->GetDlgCtrlID() != IDC_BTN_CLOSE && 
		pWnd->GetDlgCtrlID() != IDC_BTN_ADD && pWnd->GetDlgCtrlID() != IDC_BTN_DEL )
	{
		m_lexer.SendEditor( WM_KEYDOWN, VK_DELETE );
	}
	else if( pWnd && pWnd->GetDlgCtrlID() == IDC_LST_OBJECT )
	{
		DeleteSelectedItems();
	}
}

void CTextResourcesDlg::ShowScriptsEdWnd()
{
	((CCookieMachineDlg*)m_pParent)->ShowScriptsEditorGlob();
}

void CTextResourcesDlg::RunEngine()
{
	((CCookieMachineDlg*)m_pParent)->RunEngineGlob();
}

void CTextResourcesDlg::RunEngineCurr()
{
	((CCookieMachineDlg*)m_pParent)->RunEngineCurrGlob();
}

void CTextResourcesDlg::SaveFile()
{
	UpdateCurrentResource();
	((CCookieMachineDlg*)m_pParent)->SaveGlob();
}

void CTextResourcesDlg::Kill()
{
	m_pScene			= NULL;
	m_iOldItemID		= "";
	m_bFirstTime		= true;

	SAFE_DELETE( m_dlgVal );
}

void CTextResourcesDlg::Init()
{
	m_lstObjects.DeleteAllItems();
	m_lstObjects.SetExtendedStyle( m_lstObjects.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP );

	//m_lstObjects.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP );

	m_lstObjects.InsertColumn( 0, "Object ID", LVCFMT_LEFT, 270 );
	m_lstObjects.InsertColumn( 1, "Comment", LVCFMT_LEFT, 500 );

	for( int i = 0; i < ((CSceneFile*)m_pScene)->GetTextResourceCount(); i++ )
	{
		TextResItem* item = ((CSceneFile*)m_pScene)->GetTextResourceByIndex( i );

		if( !item )
			continue;

		//CString* pstr = new CString;

		//*pstr = item->id.data();

		if( item->data == "" )
		{
			m_lstObjects.InsertItem(	LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, 
										i,
										item->id.data(), 
										INDEXTOSTATEIMAGEMASK(1), 
										LVIS_STATEIMAGEMASK, 
										0,  
										(LPARAM)item	);
		}
		else
		{
			m_lstObjects.InsertItem(	LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, 
										i,
										item->id.data(), 
										INDEXTOSTATEIMAGEMASK(1), 
										LVIS_STATEIMAGEMASK, 
										1,  
										(LPARAM)item	);
		}

		m_lstObjects.SetItemText( i, 1, item->comments.data() );
	}

	if( ((CSceneFile*)m_pScene)->GetTextResourceCount() > 0 )
		m_lstObjects.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED  );


	m_lstObjects.EnableSortIcon   ( true );
	m_lstObjects.ColorSortColumn  ( true );
	m_lstObjects.SetSortColumn    ( 1 );
	m_lstObjects.KeepLabelLeft    ();
	m_lstObjects.EnableSubItemTips();
	m_lstObjects.RestoreState     ( _T("Settings"), _T("Overview") );
	m_sortedBy = static_cast<SORT_BY>( m_lstObjects.GetSortColumn() );
	m_lstObjects.SortItems( CompareCallback, m_sortedBy );

	m_lstObjects.SetFocus();

	UpdateData();
}

void CTextResourcesDlg::UpdateCurrentResource()
{
	if( m_lstObjects.GetSelectedCount() != 1 )
		return;

	int pos = m_lstObjects.GetNextItem( -1, LVNI_SELECTED );

	m_lstObjects.SetItemState( pos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED  );

	if( pos != -1 )
	{
		TextResItem*	item	= (TextResItem*)m_lstObjects.GetItemData( pos );
		int				len		= m_lexer.SendEditor( SCI_GETLENGTH );
		char*			src		= new char[len + 1];
		CString			script_text;

		m_lexer.SendEditor( SCI_GETTEXT, len + 1, (LPARAM)src );

		script_text = src;
		script_text.Trim();

		if( script_text == "" )
			m_lstObjects.SetItemState( pos, 0, LVIF_IMAGE );
		else
			m_lstObjects.SetItemState( pos, 1, LVIF_IMAGE );

		m_lstObjects.SetItemState( pos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED  );

		item->data = script_text.GetBuffer();

		SAFE_DELETE_ARRAY( src );
	}
}

void CTextResourcesDlg::OnBnClickedBtnClose()
{
	UpdateCurrentResource();
	ShowWindow( SW_HIDE );
}

void CTextResourcesDlg::DoResize( int delta )
{
	CWnd* pObjectsList	= GetDlgItem( IDC_LST_OBJECT );
	CWnd* pScriptSrcFrm	= GetDlgItem( IDC_LEXER_FRM );

	CSplitterControl::ChangeHeight( pObjectsList, delta, CW_TOPALIGN );
	CSplitterControl::ChangeHeight( pScriptSrcFrm, -delta, CW_BOTTOMALIGN );


	CRect rect;

	pScriptSrcFrm->GetWindowRect( &rect );
	ScreenToClient( &rect );
	m_lexer.MoveEditor( rect.left, rect.top, rect.Width(), rect.Height() );


	Invalidate();
	UpdateWindow();
}

BOOL CTextResourcesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	SetIcon( m_hIcon, TRUE );

	m_lexer.Load( this->GetSafeHwnd(), ((CCookieMachineDlg*)m_pParent)->GetAppDir(), "TextRes" );
	m_lexer.SetFocus();

	//setup splitter
	CRect rc;
	CWnd* pWnd = GetDlgItem( IDC_SLIDER );

	pWnd->GetWindowRect( &rc );
	ScreenToClient( &rc );
	m_wndSplitter.Create( WS_CHILD | WS_VISIBLE, rc, this, IDC_SLIDER );
	m_wndSplitter.SetRange( 100, 400 );

	
	/* LISTBOX CONTROL */
	//prepare imagelist
	CBitmap bitmap;
	m_ImageList.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );

	//0 empty
	bitmap.LoadBitmap( IDB_EMPTY );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();	
	
	//1 script
	bitmap.LoadBitmap( IDB_TEXT_RESOURCE );
	m_ImageList.Add( &bitmap, RGB(255, 255, 0) );
	bitmap.DeleteObject();

	m_lstObjects.SetImageList( &m_ImageList, LVSIL_SMALL );

	Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/*** Compare window data by sort criterion ***********************************/
int CALLBACK CTextResourcesDlg::CompareCallback( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	if( lParam1 == 0 || lParam2 == 0 )
		return 0;

	TextResItem* psz1 = (TextResItem*)lParam1;
	TextResItem* psz2 = (TextResItem*)lParam2;


	bool    bComplementResult;
	int     result = 0;

	if( lParamSort < 0 )
	{
		lParamSort = -lParamSort;
		bComplementResult = true;
	}
	else 
		bComplementResult = false;

	// Sort window data by predefined sort criterion
	// (secondary sort criterion is always the window handle)
	if( lParamSort == ID )
	{
		if( psz1->id > psz2->id )
			result = 1;
		else if( psz1->id < psz2->id )
			result = -1;
		else
			result = 0;
	}
	else if( lParamSort == COMMENTS )
	{
		if( psz1->comments > psz2->comments )
			result = 1;
		else if( psz1->comments < psz2->comments )
			result = -1;
		else
			result = 0;
	}
	else
	{}

	if( bComplementResult ) result = -result;
		return result;
}

void CTextResourcesDlg::UpdateUISizing()
{
	CListBox*	pListBox	= (CListBox*)GetDlgItem( IDC_LST_OBJECT );
	CButton*	pButtClose	= (CButton*)GetDlgItem( IDC_BTN_CLOSE );
	CStatic*	pLexerFrm	= (CStatic*)GetDlgItem( IDC_LEXER_FRM );
	bool		bFirstTime	= true;


	CRect rc;
	CRect par_rc;
	CRect rc_self;
	this->GetWindowRect( &rc );
	m_pParent->GetWindowRect( &par_rc );

	
	if( m_bFirstTime )
	{
		MoveWindow( par_rc.Width() / 2 - rc.Width() / 2, par_rc.Height() / 2 - rc.Height() / 2, rc.Width(), rc.Height(), false );	
		m_bFirstTime = false;
	}


	int width_incr	= 0;
	int height_incr	= 0;

	if( pLexerFrm )
	{
		pLexerFrm->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );

		width_incr		= rc.Width() - rc_self.right;
		height_incr		= rc.Height() - rc_self.bottom;
		rc_self.right	+= width_incr - 8;
		rc_self.bottom	+= height_incr - 60;

		pLexerFrm->MoveWindow( &rc_self, 0 );
		
		CWnd* pWnd = pLexerFrm;
		CRect rect;

		pWnd->GetWindowRect( &rect );
		ScreenToClient( &rect );
		m_lexer.MoveEditor( rect.left, rect.top, rect.Width(), rect.Height() );
	}

	//move close button
	if( pButtClose )
	{
		pButtClose->GetWindowRect( &rc_self );
		rc_self.MoveToXY( rc.Width() - 87, rc.Height() - 55 );
		pButtClose->MoveWindow( &rc_self, 1 );
	}

	//move list box
	if( pListBox )
	{
		pListBox->GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );
		rc_self.right	+= width_incr - 8;
		pListBox->MoveWindow( &rc_self, 1 );
	}

	if( m_wndSplitter )
	{
		//move slider
		m_wndSplitter.GetWindowRect( &rc_self );
		ScreenToClient( &rc_self );
		rc_self.right += width_incr - 8;
		m_wndSplitter.MoveWindow( &rc_self, 1 );	

		m_wndSplitter.Invalidate( true );
	}
}

void CTextResourcesDlg::OnShowWindow( BOOL bShow, UINT nStatus )
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_lstObjects.SetFocus();
	m_lstObjects.SortItems( CompareCallback, m_sortedBy );

	if( m_strInitialItem != "" && m_strInitialItem.length() < 64 )
	{
		DeselectAllResItems();

		//select script
		int objsel = -1;

		LVFINDINFO info;
		info.flags	= LVFI_STRING;
		info.psz	= m_strInitialItem.data();

		objsel = m_lstObjects.FindItem( &info );

		if( objsel != -1 )
		{
			m_lstObjects.SetFocus();
			m_lstObjects.SetItemState( objsel, LVIS_SELECTED, LVIS_SELECTED );
			m_lstObjects.EnsureVisible( objsel, true );
			m_strInitialItem = "";

			if( m_iErrLine != -1 )
			{
				int err_pos_st	= m_lexer.SendEditor( SCI_POSITIONFROMLINE, m_iErrLine );
				int err_pos_en	= m_lexer.SendEditor( SCI_GETLINEENDPOSITION, m_iErrLine );
				m_lexer.SendEditor( SCI_SETSEL, err_pos_st, err_pos_en );
				m_lexer.SetFocus();

				m_iErrLine = -1;
			}
		}
	}

	m_lexer.SendEditor( SCI_GRABFOCUS );

	UpdateUISizing();
}

void CTextResourcesDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UpdateUISizing();
}

LRESULT CTextResourcesDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	WINDOWPOS* lpwp;

	switch(message)
	{ 
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE )
				UpdateCurrentResource();
			break;

		case WM_CLOSE:
			OnBnClickedBtnClose();
			return false;
			break;

		case WM_NOTIFY:
			if( wParam == IDC_SLIDER )
			{	
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize( pHdr->delta );
			}
			break;

		case WM_WINDOWPOSCHANGING:
			lpwp = (LPWINDOWPOS) lParam;
            
			if( lpwp->cx < m_iMinWindowSizeX )
				lpwp->cx = m_iMinWindowSizeX;

			if( lpwp->cy < m_iMinWindowSizeY )
				lpwp->cy = m_iMinWindowSizeY;
			break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CTextResourcesDlg::OnLvnItemchangedLstObject( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	TextResItem* ritem = NULL;

	if( m_lstObjects.GetSelectedCount() > 1 )
		return;

	char buff[256];
	m_lstObjects.GetItemText( pNMLV->iItem, 0, buff, 256 );


	CString strTemp, script_text;

	if( m_iOldItemID != "" )
	{
		LV_ITEM list_item;
		ZeroMemory( &list_item, sizeof( LV_ITEM ) ); 
		list_item.mask		= LVIF_IMAGE;
		list_item.iItem		= FindItemByName( m_iOldItemID );

		//list_item.lParam	= pNMLV->lParam;
	
		int len = m_lexer.SendEditor( SCI_GETLENGTH );
		char* src = new char[len + 1];
		m_lexer.SendEditor( SCI_GETTEXT, len + 1, (LPARAM)src );
		
		CString fff = src;

		script_text = src;
		script_text.Trim();

		if( script_text == "" )
            list_item.iImage = 0;
		else
			list_item.iImage = 1;

		m_lstObjects.SetItem( &list_item );

		
		//save old item script
		strTemp = m_lstObjects.GetItemText( FindItemByName( m_iOldItemID ), 0 );

		ritem = ((CSceneFile*)m_pScene)->GetTextResourceByName( strTemp.GetBuffer() );

		if( ritem )
			ritem->data = fff.GetBuffer();

		SAFE_DELETE_ARRAY( src );
	}

	strTemp = m_lstObjects.GetItemText( pNMLV->iItem, 0 );

	TextResItem* item = ((CSceneFile*)m_pScene)->GetTextResourceByName( strTemp.GetBuffer() );

	if( !item )
		return;

	m_lexer.SendEditor( SCI_SETTEXT, 0, (LPARAM)item->data.data() );
	m_lexer.SendEditor( SCI_SETXOFFSET, 0 );

	m_lexer.SendEditor( SCI_EMPTYUNDOBUFFER );
	m_lexer.SendEditor( SCI_GRABFOCUS );

	m_iOldItemID = buff;

	*pResult = 0;
}

void CTextResourcesDlg::OnLvnColumnclickLstObject( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	SORT_BY sortBy;

	switch( pNMListView->iSubItem )
	{
		default: ASSERT( false );

		case 0:
			sortBy = ID;
			break;

		case 1:
			sortBy = COMMENTS;
			break;
	}

	if( m_sortedBy == sortBy )
		m_sortedBy = static_cast<SORT_BY>( -sortBy );
	else
		m_sortedBy = sortBy;

	m_lstObjects.SortItems( CompareCallback, m_sortedBy );
	
	*pResult = 0;
}

void CTextResourcesDlg::OnLvnEndlabeleditLstObject( NMHDR *pNMHDR, LRESULT *pResult )
{
	/*NMLVDISPINFO*	pDispInfo	= reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_DISPINFO*	plvDispInfo	= (LV_DISPINFO*)pNMHDR;
	LV_ITEM*		plvItem		= &plvDispInfo->item;

	TextResItem*	item		= (TextResItem*)plvItem->lParam;

	m_lstObjects.OnEndLabelEdit( pNMHDR, pResult );

	AfxMessageBox( "!" );*/

	*pResult = 0;
}

void CTextResourcesDlg::OnNMDblclkLstObject( NMHDR *pNMHDR, LRESULT *pResult )
{
	UpdateCurrentResource();

	CRect rc;
	m_lstObjects.GetWindowRect( &rc );
	ScreenToClient( &rc );

	CPoint pt;
	GetCursorPos( &pt );
	ScreenToClient( &pt );

	pt.x -= rc.left;
	pt.y -= rc.top;

	LVHITTESTINFO lvhit;
	lvhit.pt = pt;


	int item = m_lstObjects.SubItemHitTest( &lvhit );
	

	if( (m_lstObjects.GetSelectedCount() == 1 && item != -1 && ( lvhit.flags & LVHT_ONITEM )) == false )
		return;


	CString test;
	test.Format( "%i %i", item, lvhit.iSubItem );

	char hh[512];

	m_lstObjects.GetItemText( item, lvhit.iSubItem, hh, 512 ); 

	m_dlgVal = new CModelNameDlg(this);
	m_dlgVal->SetString( hh );

	if( lvhit.iSubItem == 0 )
	{
		m_dlgVal->SetCaption( "ID:" );
		m_dlgVal->SetWidth( 250 );
	}
	else
	{
		m_dlgVal->SetCaption( "Comment:" );
		m_dlgVal->SetWidth( 600 );
	}

	m_dlgVal->DoModal();

	//dlg->GetString();
	//in case ID changed -> check for dupes, 
	//in case found -> show err
	//else - change ID

	TextResItem* it = (TextResItem*)m_lstObjects.GetItemData( item );
	CString new_val = m_dlgVal->GetString();
	new_val.Trim();

	if( lvhit.iSubItem == 0 )
	{
		bool ok = true;
		
		//check whether id already used
		for( int i = 0; i < ((CSceneFile*)m_pScene)->GetTextResourceCount(); i++ )
		{
			TextResItem* t_it = ((CSceneFile*)m_pScene)->GetTextResourceByIndex( i );

			if( it != t_it && t_it->id == new_val.GetBuffer() )
			{
				ok = false;
				break;
			}
		}

		if( ok )
		{
			it->id = new_val.GetBuffer();

			m_iOldItemID = it->id;

			m_lstObjects.SetItemText( item, lvhit.iSubItem, new_val.GetBuffer() );
			m_lstObjects.SortItems( CompareCallback, m_sortedBy );
		}
		else
			AfxMessageBox( "Resource with ID \"" + new_val + "\" already exists!" );
	}
	else
	{
		it->comments = new_val.GetBuffer();
		m_lstObjects.SetItemText( item, lvhit.iSubItem, new_val.GetBuffer() );	
		m_lstObjects.SortItems( CompareCallback, m_sortedBy );
	}

	SAFE_DELETE( m_dlgVal );

	m_lstObjects.SetFocus();

	*pResult = 0;
}

void CTextResourcesDlg::OnBnClickedBtnDel()
{
	DeleteSelectedItems();
}

int CTextResourcesDlg::FindItemByName( str_t ID )
{
	LVFINDINFO info;

	info.flags	= LVFI_STRING;
	info.psz	= ID.data();
	
	return m_lstObjects.FindItem( &info );
}

void CTextResourcesDlg::DeselectAllResItems()
{
	ListView_SetItemState( m_lstObjects.GetSafeHwnd(), -1, 0, LVIS_SELECTED | LVIS_FOCUSED );
}

void CTextResourcesDlg::OnBnClickedBtnAdd()
{
	TextResItem* item = ((CSceneFile*)m_pScene)->AddNewTextResource();

	if( !item )
	{
		return;
	}

	UpdateCurrentResource();
	DeselectAllResItems();

	int i = m_lstObjects.InsertItem( LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, m_lstObjects.GetItemCount(), item->id.data(), INDEXTOSTATEIMAGEMASK(1), 
							 LVIS_STATEIMAGEMASK, 0, (LPARAM)item );

	m_lstObjects.SetItemState( i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED  );
	m_lstObjects.SetFocus();

	m_lstObjects.SortItems( CompareCallback, m_sortedBy );

	m_lexer.SendEditor( SCI_SETTEXT, 0, (LPARAM)"" );
	m_lexer.SendEditor( SCI_SETXOFFSET, 0 );
	m_lexer.SendEditor( SCI_EMPTYUNDOBUFFER );
}

void CTextResourcesDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

BOOL CTextResourcesDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::DestroyWindow();
}

void CTextResourcesDlg::OnNMClickLstObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	OnLvnItemchangedLstObject( pNMHDR, pResult );

	*pResult = 0;
}

BOOL CTextResourcesDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	SCNotification* ntfMsg = (SCNotification*)lParam;
	CString			strTemp;

	//check whether first line consists of  "--LUA" part
	if( ntfMsg->nmhdr.code == SCN_MODIFIED )
	{
		char buff[512];

		ZeroMemory( buff, sizeof(char) * 512 );

		m_lexer.SendEditor( SCI_GETLINE, 0, (WPARAM)buff );
	

		strTemp = buff;
		strTemp = strTemp.Left( 5 );


		if( strTemp == "--LUA" )
		{
			m_lexer.SetLUALex();
		}
		else
		{
			m_lexer.SetNoLex();
		}

	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CTextResourcesDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_CLOSE || pMsg->message == WM_DESTROY )
		int i = 0;

	if( TranslateAccelerator( this->GetSafeHwnd(), m_hAccelerator, pMsg ) )
		return true;

	switch( pMsg->message )
	{
		case WM_KEYDOWN:

			if( pMsg->wParam == VK_ESCAPE )
			{
				return false;
			}
			//else if( pMsg->wParam == VK_DELETE )
			//{
			//	CWnd* pWnd = GetFocus();

			//	if( pWnd && pWnd->GetDlgCtrlID() != IDC_LST_OBJECT && pWnd->GetDlgCtrlID() != IDC_BTN_CLOSE && 
			//		pWnd->GetDlgCtrlID() != IDC_BTN_ADD && pWnd->GetDlgCtrlID() != IDC_BTN_DEL )
			//	{
			//		m_lexer.SendEditor( WM_KEYDOWN, VK_DELETE );
			//	}
			//	else if( pWnd && pWnd->GetDlgCtrlID() == IDC_LST_OBJECT )
			//	{
			//		DeleteSelectedItems();
			//	}

			//	return false;
			//}

			break;

		case WM_KEYUP:

			if( pMsg->wParam == VK_ESCAPE )
			{
				OnBnClickedBtnClose();
				return true;
			}
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
