#include "../sys/stdafx.h"

#include "MyTree.h"
#include "../sys/CookieMachine.h"
#include "../dialogs/CookieMachineDlg.h"
#include ".\mytree.h"


IMPLEMENT_DYNAMIC( CMyTree, CBaseTreeCtrl )
CMyTree::CMyTree()
{
	eCurrNodeType		= TNT_UNDEFINED;
	m_pParent			= NULL;
	m_bIsLabelEdited	= false;
	m_editLabel			= NULL;
}

CMyTree::~CMyTree()
{
}

BEGIN_MESSAGE_MAP( CMyTree, CBaseTreeCtrl )
	ON_WM_RBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnTvnEndlabeledit)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnTvnSelchanging)
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CMyTree::OnRButtonUp( UINT nFlags, CPoint point )
{
	//calc menu position
	CRect par_rect, this_rect;
	GetParent()->GetWindowRect( &par_rect );
	GetWindowRect( &this_rect );

	point.x += par_rect.left - ( par_rect.left - this_rect.left );
	point.y += par_rect.top - ( par_rect.top - this_rect.top );

	GetParent()->SendMessage( WM_CONTEXTMENU, 0, ((point.y & 0xffff) << 16) + point.x );
}

void CMyTree::OnTvnSelchanged( NMHDR *pNMHDR, LRESULT *pResult )
{
	if( m_pParent )
	{
		LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
		
		CBaseScene* scene = ((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

		if( !scene )
		{
			*pResult = 1;
			return;
		}
		
		enum FILE_TYPE ft = scene->GetType();

		HTREEITEM hItem, hParent, hParent2, hParent3, hParent4, hParent5;
		CString strItem, strParent, strParent2, strParent3, strParent4, strParent5;

		hItem	= this->GetSelectedItem();
		strItem	= this->GetItemText( hItem );

		if( hItem >= 0 )
			hParent		= this->GetParentItem( hItem );

		if( hParent >= 0 )
		{
			hParent2	= this->GetParentItem( hParent );
			strParent	= this->GetItemText( hParent );
		}

		if( hParent2 >= 0 )
		{
			strParent2	= this->GetItemText( hParent2 );
			hParent3	= this->GetParentItem( hParent2 );
		}

		if( hParent3 >= 0 )
		{
			strParent3	= this->GetItemText( hParent3 );
			hParent4	= this->GetParentItem( hParent3 );
		}

		if( hParent4 >= 0 )
		{
			strParent4	= this->GetItemText( hParent4 );
			hParent5	= this->GetParentItem( hParent4 );
		}

		if( hParent5 >= 0 )
		{
			strParent5	= this->GetItemText( hParent5 );
		}


		switch( ft )
		{

			case FT_UNDEFINED:
			{
				*pResult = 0;
				return;
			}
			break;


			case FT_XSI:
			{
				if( strParent == "Meshes" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_MESH;
				}
				else if( strItem == "Meshes" && hParent == NULL )
				{
					eCurrNodeType = TNT_MESHES;
				}
				else if( strParent == "Lights" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_LIGHT;
				}
				else if( strItem == "Lights" && hParent == NULL )
				{
					eCurrNodeType = TNT_LIGHTS;
				}
				else if( strParent == "Cameras" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_CAMERA;
				}
				else if( strItem == "Cameras" && hParent == NULL )
				{
					eCurrNodeType = TNT_CAMERAS;
				}
				else if( strParent == "Nulls" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_NULL;
				}
				else if( strItem == "Nulls" && hParent == NULL )
				{
					eCurrNodeType = TNT_NULLS;
				}
				else if( strParent3 == "Nulls" && hParent5 == NULL && strParent == "Custom params" )
				{
					eCurrNodeType = TNT_CUST_PARAM;
				}
				else
				{
					eCurrNodeType = TNT_UNDEFINED;
				}
			}
			break;

			case FT_SCN:
			{
				if( strItem == "Meshes" && hParent == NULL )
				{
					eCurrNodeType = TNT_MESHES; 
				}
				else if( strParent == "Meshes" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_MESH; 
				}
				else if( strItem == "Nulls" && hParent == NULL )
				{
					eCurrNodeType = TNT_NULLS; 
				}
				else if( strParent == "Nulls" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_NULL; 
				}
				else if( strItem == "Textures" && hParent == NULL )
				{
					eCurrNodeType = TNT_SCN_TEXTURES; 
				}
				else if( strParent == "Textures" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_TEXTURE; 
				}
				else if( strItem == "Curves" && hParent == NULL )
				{
					eCurrNodeType = TNT_SCN_CURVES; 
				}
				else if( strParent == "Curves" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_SCN_CURVE; 
				}
				else if( strParent2 == "Curves" && hParent3 == NULL )
				{
					eCurrNodeType = TNT_SCN_CURVE_NULL; 
				}
				else if( strItem == "Lights" && hParent == NULL )
				{
					eCurrNodeType = TNT_LIGHTS; 
				}
				else if( strParent == "Lights" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_LIGHT; 
				}
				else if( strItem == "Cameras" && hParent == NULL )
				{
					eCurrNodeType = TNT_CAMERAS; 
				}
				else if( strParent == "Cameras" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_CAMERA; 
				}
				else if( strItem == "Custom objects" && hParent == NULL )
				{
					eCurrNodeType = TNT_SCN_CUST_OBJECTS; 
				}
				else if( strParent == "Custom objects" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_SCN_CUST_OBJECT; 
				}
				else if( strItem == "Custom types" && hParent == NULL )
				{
					eCurrNodeType = TNT_SCN_CUST_TYPES; 
				}
				else if( strParent == "Custom types" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_SCN_CUST_TYPE; 
				}
				else if( strParent2 == "Cameras" && strItem != "Binds" && hParent3 == NULL )
				{
					eCurrNodeType = TNT_SCN_CAMERA_BIND; 
				}
				else if( strParent3 == "Cameras" && strParent == "Binds" && hParent4 == NULL )
				{
					eCurrNodeType = TNT_SCN_CAMERA_MISC_BIND; 
				}
				else if( strParent3 == "Cameras" && strParent != "Binds" && hParent4 == NULL )
				{
					eCurrNodeType = TNT_SCN_CAMERA_BIND; 
				}
				else if( strItem.Left( 9 ) == "Level ID:" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_SCN_LEVEL;
				}
				else
				{
					eCurrNodeType = TNT_UNDEFINED;
				}
			}
			break;

			case FT_EFN_MESHLIST:
			{
				if( strItem == "Meshes" && hParent == NULL )
				{
					eCurrNodeType = TNT_MESHES; //9
				}
				else if( strParent == "Meshes" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_MESH; //10
				}
				else if( strItem == "Nulls" && hParent == NULL )
				{
					eCurrNodeType = TNT_NULLS; //52
				}
				else if( strParent == "Nulls" && hParent2 == NULL )
				{
					eCurrNodeType = TNT_NULL; //51
				}
				else
				{
					eCurrNodeType = TNT_UNDEFINED;
				}
			}
			break;
		} // switch( ft )

		((CCookieMachineDlg*)m_pParent)->SceneTreeItemChangeProc();
	}

	*pResult = 0;
}

void CMyTree::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	
	if( m_pParent )
	{
		CBaseScene* scene = ((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

		if( !scene )
		{
			*pResult = 1;
			return;
		}

		TNODE* data = (TNODE*)GetCurrNodeData();

		enum FILE_TYPE ft = scene->GetType();

		m_editLabel = GetEditControl();

		if( m_editLabel )
			m_editLabel->SetSel( 0, -1 );

		switch( ft )
		{

			case FT_UNDEFINED:
			{
				*pResult = 1;
				return;
			}
			break;

			case FT_EFN:
			{
				if( !data )
					return;

				if( data->_type != TREENODE::N_ANIMATION &&
					data->_type != TREENODE::N_NULL &&
					data->_type != TREENODE::N_MODEL_NAME &&
					data->_type != TREENODE::N_CUST_PARAM &&
					data->_type != TREENODE::N_FPS &&
					data->_type != TREENODE::N_GLOB_RADIUS )
				{
					*pResult = 1;
					return;
				}
				else if( data->_type == TREENODE::N_MODEL_NAME ) 
				{
					if( m_editLabel )
					{
						str_t tt = (char*)((CEfnCharacter*)scene)->GetModelName();
						m_editLabel->SetWindowText( tt.data() );
					}
				}
				else if( data->_type == TREENODE::N_FPS ) 
				{
					if( m_editLabel )
					{
						CString strTemp;
						strTemp.Format( "%.2f", ((CEfnCharacter*)scene)->GetFPS() );
						m_editLabel->SetWindowText( strTemp );
					}
				}
				else if( data->_type == TREENODE::N_GLOB_RADIUS ) 
				{
					if( m_editLabel )
					{
						CString strTemp;
						strTemp.Format( "%.4f", ((CEfnCharacter*)scene)->GetGlobalRadius() );
						m_editLabel->SetWindowText( strTemp );
					}
				}
			}
			break;

			case FT_EFN_MESHLIST:
			{
				if( eCurrNodeType != TNT_MESH &&
					eCurrNodeType != TNT_NULL )
				{
					*pResult = 1;
					return;
				}
			}
			break;

			case FT_SCN:
			{
				if( eCurrNodeType != TNT_CAMERA &&
					eCurrNodeType != TNT_LIGHT &&
					eCurrNodeType != TNT_MESH &&
					eCurrNodeType != TNT_NULL &&
					eCurrNodeType != TNT_TEXTURE &&
					eCurrNodeType != TNT_SCN_CURVE &&
					eCurrNodeType != TNT_SCN_CUST_OBJECT &&
					eCurrNodeType != TNT_SCN_CUST_TYPE &&
					eCurrNodeType != TNT_SCN_LEVEL )
				{
					*pResult = 1;
					return;
				}
				else if( eCurrNodeType == TNT_SCN_LEVEL ) 
				{
					if( m_editLabel )
					{
						str_t tt = ((CSceneFile*)scene)->GetLevelID();
						m_editLabel->SetWindowText( tt.data() );
					}
				}

			}
			break;
		}

		// TODO: Add your control notification handler code here
		m_bIsLabelEdited = true;

	}

	*pResult = 0;
}

void CMyTree::EndLabelEditing()
{
	//this->SendMessage( TVM_ENDEDITLABELNOW, 0, (LPARAM) false );
	//::SendMessage( this->GetSafeHwnd(), TVM_ENDEDITLABELNOW, 0, (LPARAM) true );
}

void CMyTree::StartEditingLabel()
{
	EditLabel( GetSelectedItem() );
}

void CMyTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if( m_pParent )
	{
		CString newID;

		if( pTVDispInfo->item.pszText )
		{
			newID = pTVDispInfo->item.pszText;
			newID = newID.Trim();
			((CCookieMachineDlg*)m_pParent)->RenameCurrTreeviewItemGlob( newID.GetBuffer() );
		}

		m_bIsLabelEdited = false;
	}

	*pResult = 0;
}

void CMyTree::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	DeselectAllNodes();

	*pResult = 0;
}

void CMyTree::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	//CBaseTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}
