#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "afxtempl.h"
#include "BindDlg.h"
#include "CookieMachineDlg.h"


#pragma warning( disable:4800 )

IMPLEMENT_DYNAMIC(CBindDlg, CDialog)
CBindDlg::CBindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBindDlg::IDD, pParent)
{
	// item to edit ( all params joined in one string )
	m_strItem2Edit = "";
	m_pSceneTypes = NULL;
}

CBindDlg::~CBindDlg()
{
}

void CBindDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_CMB_PAR_ID, m_cmbParentID );
    DDX_Control( pDX, IDC_CMB_PAR_TYPE, m_cmbParentType );
    DDX_Control( pDX, IDC_CMB_CHILD_TYPE, m_cmbChildType );
    DDX_Control( pDX, IDC_LST_LOG, m_lstLog );
    DDX_Control(pDX, IDC_LST_CHILD_ID, m_lstChildID);
}


BEGIN_MESSAGE_MAP(CBindDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_ADD_ANOTHER, OnBnClickedBtnAddAnother)
	ON_BN_CLICKED(IDC_CHK_PAR1, OnBnClickedChkPar1)
	ON_LBN_DBLCLK(IDC_LST_CHILD_ID, OnLbnDblclkLstChildId)

	//accelerators
	ON_COMMAND( ID_ACCL_SAVE, SaveAll )
	ON_COMMAND( ID_ACCL_ADD_ANOTHER_BIND, OnLbnDblclkLstChildId )

	ON_CBN_SELCHANGE(IDC_CMB_CHILD_TYPE, OnCbnSelchangeCmbChildType)
END_MESSAGE_MAP()

void CBindDlg::SaveAll()
{
	((CCookieMachineDlg*)m_pParent)->SaveGlob();
}

void CBindDlg::OnBnClickedBtnCancel()
{
	// in case we were editing item
	if( m_strItem2Edit != "" )
	{
		if( !AddNewBind() )
			return;
	}

	OnOK();
}

void CBindDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow( bShow, nStatus );

	this->SetWindowText( m_strCaption );
}

BOOL CBindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//init types
	m_cmbParentType.ResetContent();
	m_cmbChildType.ResetContent();

	if( m_pSceneTypes )
	{
		for( unsigned int i = 0; i < m_pSceneTypes->size(); i++ )
		{
			m_cmbParentType.AddString( (*m_pSceneTypes)[i].data() );
			m_cmbChildType.AddString( (*m_pSceneTypes)[i].data() );
		}
	}

	m_cmbParentType.SetCurSel( 0 );
	m_cmbChildType.SetCurSel( 0 );

	//init scene data
	m_cmbParentID.ResetContent();
	m_lstChildID.ResetContent();

	StrVec_iter iter = m_pSceneData->begin();

	while( iter != m_pSceneData->end() )
	{
		m_cmbParentID.AddString( (*iter).data() );
        m_lstChildID.AddString( (*iter).data() ); 
		iter++;
	}

	m_cmbParentID.SetCurSel( 0 );
	m_lstChildID.SetCurSel( 0 );
    m_lstChildID.SetSel( 0, true );

	// in case we want to edit item
	if( m_strItem2Edit != "" )
	{
		m_lstChildID.SetCurSel( -1 );
		m_lstChildID.SetSel( -1, false );

		m_lstChildID.ModifyStyleEx( 0, LBS_STANDARD );

		GetDlgItem( IDC_BTN_ADD_ANOTHER )->ShowWindow( false );

		//find bind
		StrVec needed_bind;

		CString temp = m_strItem2Edit;
		CString item = "";

		ListStrVec_iter iter = m_pBinds->begin();

		while( iter != m_pBinds->end() )
		{
			StrVec			str_vec = *iter;
			StrVec_iter	str_iter = str_vec.begin();

			item = "";

			while( str_iter != str_vec.end() )
			{
				item += (*str_iter).data();
				str_iter++;
			}

			if( item == temp )
			{
				needed_bind = *iter;
				break;
			}

			iter++;
		}

		// set combo boxes to correct values
		//set types
		StrVec_iter str_iter = needed_bind.begin();

		m_iEditedItemID = atoi( (*str_iter).data() );

		str_iter += 2;

		for( int i = 0; i < m_cmbParentType.GetCount(); i++ )
		{
			CString	str;
			str_t res_str;
			str_t src_str = (*str_iter);
			int			n = m_cmbParentType.GetLBTextLen( i );
			m_cmbParentType.GetLBText( i, str.GetBuffer(n) );
			res_str = str.GetBuffer();

			if( res_str == src_str )
			{
				m_cmbParentType.SetCurSel( i );
				break;
			}
		}

		str_iter += 2;

		for( int i = 0; i < m_cmbChildType.GetCount(); i++ )
		{
			CString	str;
			str_t res_str;
			str_t src_str = (*str_iter);
			int			n = m_cmbChildType.GetLBTextLen( i );
			m_cmbChildType.GetLBText( i, str.GetBuffer(n) );
			res_str = str.GetBuffer();

			if( res_str == src_str )
			{
				m_cmbChildType.SetCurSel( i );
				break;
			}
		}

		str_iter -= 3;

		for( int i = 0; i < m_cmbParentID.GetCount(); i++ )
		{
			CString	str;
			int iIdx;
			str_t res_str;
			str_t src_str = (*str_iter);
			int			n = m_cmbParentID.GetLBTextLen( i );
			m_cmbParentID.GetLBText( i, str.GetBuffer(n) );
			res_str = str.GetBuffer();
			iIdx = res_str.find( ".", 0);
			res_str = res_str.substr( iIdx + 1, res_str.length() );

			if( res_str == src_str )
			{
				m_cmbParentID.SetCurSel( i );
				break;
			}
		}

		str_iter += 2;

		for( int i = 0; i < m_lstChildID.GetCount(); i++ )
		{
			CString	str;
			int iIdx;
			str_t res_str;
			str_t src_str = (*str_iter);
			int			n = m_lstChildID.GetTextLen( i );
			m_lstChildID.GetText( i, str.GetBuffer(n) );
			res_str = str.GetBuffer();
			iIdx = res_str.find( ".", 0);
			res_str = res_str.substr( iIdx + 1, res_str.length() );

			if( res_str == src_str )
			{
				m_lstChildID.SetCurSel( i );
                m_lstChildID.SetSel( i, true );
                m_lstChildID.SetTopIndex( i );
				break;
			}
		}

		if( needed_bind.size() > 5 )
		{
			str_iter +=2;

			//par1
			CButton*	pBtn		= (CButton*)GetDlgItem( IDC_CHK_PAR1 );
			CRichEditCtrl*	pEdit		= (CRichEditCtrl*)GetDlgItem( IDC_EDIT_PAR1 );
			pEdit->SetSel( 0, -1 );

			pBtn->SetCheck( true );
			pEdit->SetReadOnly( false );
			pEdit->ReplaceSel( (*str_iter).data() , false );
		}

		//delete entry from the list
		m_pBinds->erase( iter );
	}

    m_lstChildID.SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

bool CBindDlg::AddNewBind()
{
	//find largest bind id
	int largest_id = 0;

	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		StrVec temp = *iter;

		if( largest_id < atoi(temp[0].data()) )
			largest_id = atoi( temp[0].data() );

		iter++;
	}

    int		binds_count = 0;

	//prepare parent name string
	CString	str;
	str_t strParName, strChildName, strParType, strChildType, strPar1; 
	int			sel	= m_cmbParentID.GetCurSel();
	int			n		= m_cmbParentID.GetLBTextLen( sel );
	m_cmbParentID.GetLBText( sel, str.GetBuffer(n) );
	strParName = str.GetBuffer();
	StripTypeHeader( strParName );


	//prepare parent type string
	sel	= m_cmbParentType.GetCurSel();
	n		= m_cmbParentType.GetLBTextLen( sel );
	m_cmbParentType.GetLBText( sel, str.GetBuffer(n) );
	strParType = str.GetBuffer();


    //prepare child type string
	sel	= m_cmbChildType.GetCurSel();
	n		= m_cmbChildType.GetLBTextLen( sel );
	m_cmbChildType.GetLBText( sel, str.GetBuffer(n) );
	strChildType = str.GetBuffer();


	//create list of child ids
	// Get the indexes of all the selected items.
    binds_count = m_lstChildID.GetSelCount();

	CArray< int, int > aryListBoxSel;
	aryListBoxSel.SetSize( binds_count );
	m_lstChildID.GetSelItems( binds_count, aryListBoxSel.GetData() );

	StrVec vec;

	for( int i = 0; i < binds_count; i++ )
	{	
		/* generate bind id */
		CString tmp_id;
		str_t	res_id;

		if( m_strItem2Edit == "" )	
		{
			//increment id counter
			if( largest_id + i + 1 < 10 )
				tmp_id.Format( "00%i", largest_id + i + 1);

			if( largest_id + i + 1 >= 10 && largest_id + i + 1 < 100 )
				tmp_id.Format( "0%i", largest_id + i + 1);

			if( largest_id + i + 1 > 99 )
				tmp_id.Format( "%i", largest_id + i + 1);

			res_id = tmp_id.GetBuffer();
			/* END generate bind id */
		}
		else
		{
			if( m_iEditedItemID < 10 )
				tmp_id.Format( "00%i", m_iEditedItemID );

			if( m_iEditedItemID >= 10 && m_iEditedItemID < 100 )
				tmp_id.Format( "0%i", m_iEditedItemID );

			if( m_iEditedItemID > 99 )
				tmp_id.Format( "%i", m_iEditedItemID );

			
			res_id = tmp_id.GetBuffer();
		}


		vec.clear();

		vec.push_back( res_id );
		vec.push_back( strParName );
		vec.push_back( strParType );

		
		sel	= aryListBoxSel[i];
		n	= m_lstChildID.GetTextLen( sel );
		m_lstChildID.GetText( sel, str.GetBuffer(n) );
		strChildName = str.GetBuffer();
		StripTypeHeader( strChildName );


		vec.push_back( strChildName );
		vec.push_back( strChildType );

		//par1
		CButton*			pBtn		= (CButton*)GetDlgItem( IDC_CHK_PAR1 );
		CRichEditCtrl*	pEdit		= (CRichEditCtrl*)GetDlgItem( IDC_EDIT_PAR1 );

		if( (bool)pBtn->GetCheck() )
		{
			pEdit->SetSel( 0, -1 );
			strPar1 = pEdit->GetSelText().GetBuffer();
			vec.push_back( strPar1 );
		}

		if( IsAlreadyOnTheList( &vec ) )
		{
			str_t res_vec_text = "";

			for( unsigned int i = 0; i < vec.size(); i++ )
				res_vec_text += " - " + vec[i];

			res_vec_text = "Ѕинд: \n\r" + res_vec_text + "\n\rне будет добавлен! ”казанный бинд уже существует в списке.";

			AfxMessageBox( res_vec_text.data() );

			if( m_strItem2Edit != "" )
				return false;
		}
		else
		{
			m_pBinds->push_back( vec );
			m_lstLog.AddString( (strParName + " - " + strParType + " - " + strChildName + " - " + strChildType).data() );
		}
	}

	m_pBinds->sort();

	return true;
}

void CBindDlg::OnBnClickedBtnAddAnother()
{
	AddNewBind();
}

bool CBindDlg::IsAlreadyOnTheList( StrVec* vec )
{
	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		bool found = true;

		StrVec		str_vec		= *iter;
		StrVec_iter	str_iter	= str_vec.begin();
		StrVec_iter	str_iter2	= vec->begin();

		str_iter++;
		str_iter2++;

		if( str_vec.size() == vec->size() )
		{
			while( str_iter != str_vec.end() )
			{
				str_t one = *str_iter2;
				str_t two = *str_iter;

				if( one != two )
					found = false;

				str_iter++;
				str_iter2++;
			}

			if( found )
				return true;
		}

		iter++;
	}

	return false;
}

void CBindDlg::OnBnClickedChkPar1()
{
	CEdit*		pEdit		= (CEdit*)GetDlgItem( IDC_EDIT_PAR1 );
	CButton*	pBtn		= (CButton*)GetDlgItem( IDC_CHK_PAR1 );

	if( (bool)pBtn->GetCheck() )
		pEdit->SetReadOnly( false );
	else
		pEdit->SetReadOnly( true );
}

void CBindDlg::OnLbnDblclkLstChildId()
{
	if( m_strItem2Edit == "" )
		AddNewBind();
	else
		OnBnClickedBtnCancel();
}

BOOL CBindDlg::PreTranslateMessage(MSG* pMsg)
{
	if( TranslateAccelerator( this->GetSafeHwnd(), m_hAccelerator, pMsg ) )
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}

void CBindDlg::OnCbnSelchangeCmbChildType()
{
	if( ((CCookieMachineDlg*)m_pParent)->FilterObjectsByPrefix() == false )
		return;

	CString str;
	m_cmbChildType.GetLBText( m_cmbChildType.GetCurSel(), str );
	str_t strParType = str.GetBuffer();

	str_t prefix = GetPrexifFromType( strParType );

	StrVec filtered_objects;

	for( unsigned int i = 0; i < m_pSceneData->size(); i++ )
	{
		str_t obj_item = (*m_pSceneData)[i];
		StripTypeHeader( obj_item );

		if( obj_item.substr( 0, 2 ) == prefix )
			filtered_objects.push_back( (*m_pSceneData)[i] );
	}

	m_lstChildID.ResetContent();

	for( unsigned int i = 0; i < filtered_objects.size(); i++ )
		m_lstChildID.AddString( filtered_objects[i].data() );
}
