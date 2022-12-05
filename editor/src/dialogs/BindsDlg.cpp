#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "BindsDlg.h"
#include "CookieMachineDlg.h"

IMPLEMENT_DYNAMIC(CBindsDlg, CDialog)
CBindsDlg::CBindsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBindsDlg::IDD, pParent)
{
	m_pSelItem		= NULL;
	m_pSceneTypes	= NULL;
	m_bChangesMade	= false;
	m_pBinds		= NULL;
}

CBindsDlg::~CBindsDlg()
{

}

void CBindsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BINDS, m_lstBinds);
	DDX_Control(pDX, IDC_CMB_FILTER_PAR_TYPE, m_cmbParentType);
	DDX_Control(pDX, IDC_CMB_FILTER_PAR_ID, m_cmbParentID);
}


BEGIN_MESSAGE_MAP(CBindsDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_EDIT, OnBnClickedBtnEdit)
	ON_CBN_SELCHANGE(IDC_CMB_FILTER_PAR_TYPE, OnCbnSelchangeCmbFilterParType)
	ON_CBN_SELCHANGE(IDC_CMB_FILTER_PAR_ID, OnCbnSelchangeCmbFilterParId)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHdnItemdblclickListBinds)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BINDS, OnNMDblclkListBinds)
	
	//accelerators
	ON_COMMAND( ID_ACCL_SAVE, SaveAll )

	ON_BN_CLICKED(IDC_BTN_UP, OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBnClickedBtnDown)
END_MESSAGE_MAP()


void CBindsDlg::SaveAll()
{
	((CCookieMachineDlg*)m_pParent)->SaveGlob();
}

BOOL CBindsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lstBinds.SetExtendedStyle( m_lstBinds.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_lstBinds.DeleteAllItems();
	m_lstBinds.InsertColumn( 0,	"ID",					LVCFMT_LEFT,	25, -1 );
	m_lstBinds.InsertColumn( 1,	"Parent ID",		LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 2,	"Parent Type",	LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 3,	"Child ID",			LVCFMT_LEFT,	120, -1 );
	m_lstBinds.InsertColumn( 4,	"Child Type",		LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 5,	"Param1",			LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 6,	"Param2",			LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 7,	"Param3",			LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 8,	"Param4",			LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 9,	"Param5",			LVCFMT_LEFT,	75, -1 );
	m_lstBinds.InsertColumn( 10, "Param6",			LVCFMT_LEFT,	75, -1 );


	// load up data to parent types filter
	m_cmbParentType.ResetContent();

	m_cmbParentType.AddString( "---" );


	if( m_pSceneTypes )
	{
		for( unsigned int i  = 0; i < m_pSceneTypes->size(); i++ )
			m_cmbParentType.AddString( (*m_pSceneTypes)[i].data() );

		m_cmbParentType.SetCurSel( 0 );
	}


	// load up data to parent ID filter
	m_cmbParentID.ResetContent();

	m_cmbParentID.AddString( "---" );

	for( unsigned int i  = 0; i < (*m_pSceneData).size(); i++ )
		m_cmbParentID.AddString( (*m_pSceneData)[i].data() );

	m_cmbParentID.SetCurSel( 0 );



    m_lstBinds.SetFocus();

	UpdateGrid();

	//set selection (if assigned)
	if( m_pSelItem )
	{
		for( int i = 0; i < m_lstBinds.GetItemCount(); i++ )
		{
			if( (*m_pSelItem)[0] == m_lstBinds.GetItemText( i, 1 ).GetBuffer() && 
				(*m_pSelItem)[1] == m_lstBinds.GetItemText( i, 2 ).GetBuffer() && 
				(*m_pSelItem)[2] == m_lstBinds.GetItemText( i, 3 ).GetBuffer() &&
				(*m_pSelItem)[3] == m_lstBinds.GetItemText( i, 4 ).GetBuffer() )
			{
				m_lstBinds.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
				m_lstBinds.EnsureVisible( i, true );
				break;
			}
		}
	}
	else
	{
		m_lstBinds.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
		m_lstBinds.EnsureVisible( 0, true );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

static int CALLBACK MyCompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
   // lParamSort contains a pointer to the list view control.
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1	= pListCtrl->GetItemText( lParam1, 0 );
   CString    strItem2	= pListCtrl->GetItemText( lParam2, 0 );

   return strcmp( strItem2, strItem1 );
}

void CBindsDlg::UpdateGrid()
{
	/*** process filtering options ***/
	//get parent type string
	CString	str;
	str_t	filtType; 
	str_t	filtID;
	int		sel	= m_cmbParentType.GetCurSel();
	int		n	= m_cmbParentType.GetLBTextLen( sel );
	m_cmbParentType.GetLBText( sel, str.GetBuffer(n) );
	filtType = str.GetBuffer();

	//get parent ID string
	str		= "";
	sel		= m_cmbParentID.GetCurSel();
	n		= m_cmbParentID.GetLBTextLen( sel );
	m_cmbParentID.GetLBText( sel, str.GetBuffer(n) );
	filtID	= str.GetBuffer();

	StripTypeHeader( filtID );
	/*** END process filtering options ***/

	m_lstBinds.DeleteAllItems();

	//sort items
	ListStrVec		m_SortedBinds;
	list<int>		ids;
	list<int>::iterator ids_iter;
	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		int u = 0;
		u = atoi( (*iter)[0].data() );

		ids.push_back( u );

		iter++;
	}

	ids.sort();


	ids_iter = ids.begin();

	while( ids_iter != ids.end() )
	{
		iter = m_pBinds->begin();

		while( iter != m_pBinds->end() )
		{
			if( atoi( (*iter)[0].data() ) == *ids_iter )
				m_SortedBinds.push_back( (*iter) );

			iter++;
		}

		ids_iter++;
	}

	m_pBinds->swap( m_SortedBinds );

	iter = m_pBinds->begin();

	int row	= 0;
	int col	= 0;

	while( iter != m_pBinds->end() )
	{
		StrVec		str_vec = *iter;
		StrVec_iter	str_iter = str_vec.begin();

		if( ( filtType == str_vec[2] || filtType == "---" ) && ( filtID == str_vec[1] || filtID == "---") )
		{
			m_lstBinds.InsertItem( row, (*str_iter).data() );

			col = 1;

			str_iter++;

			while( str_iter != str_vec.end() )
			{
				str_t ff = (*str_iter).data();
				m_lstBinds.SetItemText(	 row, col, (*str_iter).data() );

				col++;
				str_iter++;
			}
			
			row++;
		}

		iter++;
	}

	//m_lstBinds.SortItems( MyCompareProc, (LPARAM)&m_lstBinds );
}

void CBindsDlg::OnBnClickedBtnClose()
{
	OnOK();
}

void CBindsDlg::OnBnClickedBtnAdd()
{
	CBindDlg* dlg = new CBindDlg( this );
	dlg->SetParent( m_pParent );
	dlg->SetCaption( "Добавить новый бинд..." );
	dlg->LinkBinds( m_pBinds );
	dlg->LinkSceneData( m_pSceneData );
	dlg->LinkSceneTypes( m_pSceneTypes );
	dlg->SetAccel( m_hAccelerator );
	dlg->DoModal();
	UpdateGrid();

	m_bChangesMade = true;

	SAFE_DELETE( dlg );
}

void CBindsDlg::OnBnClickedBtnRemove()
{
	int		sel_cnt	= (int)m_lstBinds.GetSelectedCount();
	int		nItem	= -1;

	if( sel_cnt > 0 )
	{
		for( int i = 0; i < sel_cnt; i++ )
		{
			nItem = m_lstBinds.GetNextItem( nItem, LVNI_SELECTED );
			ASSERT( nItem != -1 );
			RemoveBindFromTheList( nItem );
		}
	}

	m_bChangesMade = true;
	UpdateGrid();
}

void CBindsDlg::RemoveBindFromTheList( int iListViewIndex )
{
	CString temp = "";
	CString item = "";

	for( int i = 0; i < 11; i++ )
	{
		CString temp_item = m_lstBinds.GetItemText( iListViewIndex, i );
		temp += m_lstBinds.GetItemText( iListViewIndex, i );
	}

	ListStrVec_iter iter = m_pBinds->begin();


	while( iter != m_pBinds->end() )
	{
		StrVec		str_vec		= *iter;
		StrVec_iter	str_iter	= str_vec.begin();

		item = "";

		while( str_iter != str_vec.end() )
		{
			item += (*str_iter).data();
			str_iter++;
		}

		if( item == temp )
		{
			m_pBinds->erase( iter );

			DeleteFuncEntriesByKey( m_pFuncs, atoi(str_vec[0].data()) );

			break;
		}

		iter++;
	}
}

void CBindsDlg::OnBnClickedBtnEdit()
{
	CBindDlg* dlg = new CBindDlg( this );
	dlg->SetAccel( m_hAccelerator );
	dlg->SetCaption( "Редактировать бинд..." );
	dlg->LinkBinds( m_pBinds );
	dlg->LinkSceneData( m_pSceneData );
	dlg->LinkSceneTypes( m_pSceneTypes );
	dlg->SetParent( m_pParent );

	int	sel_cnt	= (int)m_lstBinds.GetSelectedCount();
	int	nItem	= -1;

	if( sel_cnt == 1 )
	{
		nItem = m_lstBinds.GetNextItem( nItem, LVNI_SELECTED );
		ASSERT( nItem != -1 );

		CString item = "";

		//check whether bind is editable
		str_t par_ID		= m_lstBinds.GetItemText( nItem, 1 ).GetBuffer();
		str_t par_Type		= m_lstBinds.GetItemText( nItem, 2 ).GetBuffer();
		str_t child_ID		= m_lstBinds.GetItemText( nItem, 3 ).GetBuffer();
		str_t child_Type	= m_lstBinds.GetItemText( nItem, 4 ).GetBuffer();

		bool pIDFound = false, cIDFound = false, pTypeFound = false, cTypeFound = false;

		for( unsigned int i = 0; i < m_pSceneTypes->size(); i++ )
		{
			if( (*m_pSceneTypes)[i] == par_Type )
				pTypeFound = true;

			if( (*m_pSceneTypes)[i] == child_Type )
				cTypeFound = true;
		}

		for( unsigned int i = 0; i < m_pSceneData->size(); i++ )
		{
			str_t data_item = (*m_pSceneData)[i].data();

			StripTypeHeader( data_item );

			if( data_item == par_ID )
				pIDFound = true;

			if( data_item == child_ID )
				cIDFound = true;
		}

		if( !pIDFound || !cIDFound || !pTypeFound || !cTypeFound )
		{
			AfxMessageBox( "Item is not editable!" );
			return;
		}


		StrVec listTypes;

		GetTypesList( listTypes );

		for( UINT i = 0; i < listTypes.size(); i++ )
		{
			CString temp_item = m_lstBinds.GetItemText( nItem, i );
			item += m_lstBinds.GetItemText( nItem, i );
		}

		dlg->SetItem2Edit( item );

		dlg->DoModal();
		UpdateGrid();

		m_lstBinds.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );
		m_lstBinds.EnsureVisible( nItem, true );

		m_bChangesMade = true;
	}
	
	SAFE_DELETE( dlg );
}

void CBindsDlg::OnCbnSelchangeCmbFilterParType()
{
	UpdateGrid();
}

void CBindsDlg::OnCbnSelchangeCmbFilterParId()
{
	UpdateGrid();
}

void CBindsDlg::OnHdnItemdblclickListBinds(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	*pResult = 0;
}

void CBindsDlg::OnNMDblclkListBinds(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedBtnEdit();
	*pResult = 0;
}

BOOL CBindsDlg::PreTranslateMessage( MSG* pMsg )
{
	if( TranslateAccelerator( this->GetSafeHwnd(), m_hAccelerator, pMsg ) )
		return true;

	switch( pMsg->wParam )
	{
		case VK_ESCAPE:
			OnBnClickedBtnClose();
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CBindsDlg::OnBnClickedBtnUp()
{
	int		sel_cnt		= (int)m_lstBinds.GetSelectedCount();
	int		nItem		= -1;
	int		nPrevItem	= -1;
	str_t	item_id, prev_item_id;

	if( sel_cnt == 1 )
	{
		nItem			= m_lstBinds.GetNextItem( nItem, LVNI_SELECTED );
		item_id			= m_lstBinds.GetItemText( nItem, 0 ).GetBuffer();
		nPrevItem		= m_lstBinds.GetNextItem( nItem, LVNI_ABOVE );
		prev_item_id	= m_lstBinds.GetItemText( nPrevItem, 0 ).GetBuffer();
	}
	

	if( nPrevItem == -1 )
		return;


	ListStrVec_iter iter = m_pBinds->begin();


	while( iter != m_pBinds->end() )
	{	
		if( (*iter)[0] == prev_item_id )
		{
			(*iter)[0] = item_id;
		}
		else if( (*iter)[0] == item_id )
		{
			(*iter)[0] = prev_item_id;
		}

		iter++;
	}

	UpdateGrid();

	m_lstBinds.SetFocus();
	m_lstBinds.SetItemState( nPrevItem, LVIS_SELECTED, LVIS_SELECTED );
	m_lstBinds.EnsureVisible( nPrevItem, true );
}

void CBindsDlg::OnBnClickedBtnDown()
{
	int		sel_cnt		= (int)m_lstBinds.GetSelectedCount();
	int		nItem		= -1;
	int		nNextItem	= -1;
	str_t	item_id, next_item_id;

	if( sel_cnt == 1 )
	{
		nItem			= m_lstBinds.GetNextItem( nItem, LVNI_SELECTED );
		item_id			= m_lstBinds.GetItemText( nItem, 0 ).GetBuffer();
		nNextItem		= m_lstBinds.GetNextItem( nItem, LVNI_BELOW );
		next_item_id	= m_lstBinds.GetItemText( nNextItem, 0 ).GetBuffer();
	}
	

	if( nNextItem == -1 )
		return;


	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{	
		if( (*iter)[0] == next_item_id )
		{
			(*iter)[0] = item_id;
		}
		else if( (*iter)[0] == item_id )
		{
			(*iter)[0] = next_item_id;
		}

		iter++;
	}

	UpdateGrid();

	m_lstBinds.SetFocus();
	m_lstBinds.SetItemState( nNextItem, LVIS_SELECTED, LVIS_SELECTED );
	m_lstBinds.EnsureVisible( nNextItem, true );
}
