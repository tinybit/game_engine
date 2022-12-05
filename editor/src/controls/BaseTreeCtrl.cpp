#include "../sys/stdafx.h"

#include "BaseTreeCtrl.h"

IMPLEMENT_DYNAMIC( CBaseTreeCtrl, CTreeCtrl )
CBaseTreeCtrl::CBaseTreeCtrl()
{
	m_clrBgColor = RGB( 171, 168, 166 );
}

CBaseTreeCtrl::~CBaseTreeCtrl()
{
}

BEGIN_MESSAGE_MAP( CBaseTreeCtrl, CTreeCtrl )
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnTvnDeleteitem)
END_MESSAGE_MAP()


void CBaseTreeCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	HTREEITEM item = this->HitTest( point, 0 );
	this->SelectItem( item );
}

BOOL CBaseTreeCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	if( message == 307 || message == 312 )
		return true;

	LPNMHDR pNmhdr = (LPNMHDR)lParam;
	
	switch( pNmhdr->code )
	{
		case NM_CUSTOMDRAW:
			LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)lParam;

			switch( pCustomDraw->nmcd.dwDrawStage )
			{
				case CDDS_PREPAINT:

					// Need to process this case and set pResult to CDRF_NOTIFYITEMDRAW,
					// otherwise parent will never receive CDDS_ITEMPREPAINT notification. (GGH)
					*pLResult = CDRF_NOTIFYITEMDRAW;
					return true;

				case CDDS_ITEMPREPAINT:

					// painting all 0-level items blue,
					// and all 1-level items red (GGH)
					switch (pCustomDraw->iLevel)
					{
						case 0:

							if( pCustomDraw->nmcd.uItemState == (CDIS_FOCUS | CDIS_SELECTED) ) // selected
							{
								pCustomDraw->clrText = RGB(0, 0, 0);
								pCustomDraw->clrTextBk = RGB(255, 204, 0);
							}
							else
							{
								pCustomDraw->clrText = RGB(0, 0, 255);
								pCustomDraw->clrTextBk = RGB( 171, 168, 166 );
							}

							break;


						default:

							if( pCustomDraw->nmcd.uItemState == (CDIS_FOCUS | CDIS_SELECTED) ) // selected
							{
								pCustomDraw->clrText = RGB(0, 0, 0);
								pCustomDraw->clrTextBk = RGB(255, 204, 0);
							}
							else if( pCustomDraw->nmcd.uItemState == CDIS_SELECTED ) // selected
							{
								pCustomDraw->clrText = RGB(0, 0, 0);
								pCustomDraw->clrTextBk = RGB(185, 134, 0);
							}
							else
							{
								pCustomDraw->clrText = RGB(0, 0, 0);
								pCustomDraw->clrTextBk = RGB( 171, 168, 166 );
							}

							break;
					}
					

					*pLResult = CDRF_SKIPDEFAULT;
					return false;
			}

			break;
	}

	return CTreeCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

void CBaseTreeCtrl::OnPaint()
{
	SetBkColor( RGB( 171, 168, 166 ) );
	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	CTreeCtrl::OnPaint();
}

void CBaseTreeCtrl::DeselectAllNodes()
{
	DeselectAllNodesRecurse( this->GetRootItem() );
}

HTREEITEM CBaseTreeCtrl::FindTreeItemByName( CString strItemName, HTREEITEM hRootItem, bool bRecursive )
{
	return FindTreeItemByName( (str_t)strItemName.GetBuffer(), hRootItem, bRecursive );
}

HTREEITEM CBaseTreeCtrl::FindTreeItemByName( char* strItemName, HTREEITEM hRootItem, bool bRecursive )
{
	return FindTreeItemByName( (str_t)strItemName, hRootItem, bRecursive );	
}

HTREEITEM CBaseTreeCtrl::FindTreeItemByName( const char* strItemName, HTREEITEM hRootItem, bool bRecursive )
{
	return FindTreeItemByName( (str_t)strItemName, hRootItem, bRecursive );	
}

HTREEITEM CBaseTreeCtrl::FindTreeItemByName( str_t strItemName, HTREEITEM hRootItem, bool bRecursive )
{
	str_t item_name = GetItemText( hRootItem ).GetBuffer();

	if( strItemName == item_name )
		return hRootItem;
	else
	{
		if( bRecursive )
		{
			//recurse
			HTREEITEM tr_item = GetNextItem( hRootItem, TVGN_CHILD );

			while( tr_item != NULL )
			{
				HTREEITEM res = FindTreeItemByName( strItemName, tr_item );

				if( res != NULL )
					return res;
				
				tr_item = GetNextItem( tr_item, TVGN_NEXT );
			}
		}
		else
		{
			HTREEITEM tr_item = GetNextItem( hRootItem, TVGN_CHILD );

			while( tr_item != NULL )
			{
				str_t item_name = GetItemText( tr_item ).GetBuffer();

				if( strItemName == item_name )
					return tr_item;

				tr_item = GetNextItem( tr_item, TVGN_NEXT );
			}
		}
	}

	return NULL;
}

void CBaseTreeCtrl::DeselectAllNodesRecurse( HTREEITEM hRoot )
{
	while( hRoot )
	{
		this->SetItemState( hRoot, 0, TVIS_SELECTED );

		HTREEITEM hChild = this->GetNextItem( hRoot, TVGN_CHILD );

		if( hChild != NULL )
			DeselectAllNodesRecurse( hChild );

		hRoot = this->GetNextItem( hRoot, TVGN_NEXT );
	}
}

CString	CBaseTreeCtrl::GetCurrNodeText()
{
	return GetItemText( GetSelectedItem() );
}

void CBaseTreeCtrl::SetCurrNodeText( CString strNewID )
{
	SetItemText( GetSelectedItem(), strNewID );
}

void CBaseTreeCtrl::SetCurrNodeImage( int iImgIdx )
{
	SetItemImage( GetSelectedItem(), iImgIdx, iImgIdx );
}

void CBaseTreeCtrl::SetCurrNodeData( void* pData )
{
	void* pOldData = (void*)GetItemData( GetSelectedItem() );
	
	SAFE_DELETE( pOldData );

	SetItemData( GetSelectedItem(), (DWORD)pData );
}

CString	CBaseTreeCtrl::GetParNodeText()
{
	HTREEITEM hParent = GetParentItem( GetSelectedItem() );

	return GetItemText( hParent );
}

CString	CBaseTreeCtrl::GetPar2NodeText()
{
	HTREEITEM hParent = GetParentItem( GetSelectedItem() );
	HTREEITEM hParent2 = GetParentItem( hParent );
	
	return GetItemText( hParent2 );
}

HTREEITEM CBaseTreeCtrl::GetParNode()
{
	HTREEITEM hItem = GetSelectedItem();

	if( !hItem )
		return NULL;

	return GetParentItem( hItem );
}

HTREEITEM CBaseTreeCtrl::GetPar2Node()
{
	HTREEITEM hItem = GetSelectedItem();

	if( !hItem )
		return NULL;

	HTREEITEM hParent = GetParentItem( hItem );

	if( !hParent )
		return NULL;

	return GetParentItem( hParent );
}

int CBaseTreeCtrl::GetCurrNodeImage()
{
	int iImage = -1;

	GetItemImage( GetSelectedItem(), iImage, iImage );

	return iImage;
}

void CBaseTreeCtrl::ReleaseTreeData()
{
	HTREEITEM hItem = GetRootItem();

	while( hItem != NULL && hItem != GetRootItem() )
	{
		ReleaseTreeItem( hItem );
		hItem = GetNextSiblingItem( hItem );
	} 
}

void* CBaseTreeCtrl::GetCurrNodeData()
{
	HTREEITEM hItem = GetSelectedItem();

	if( !hItem )
		return NULL;

	return (void*)GetItemData( hItem );
}

HTREEITEM CBaseTreeCtrl::InsertItem2( const char* szText, int iImage, void* nodeData, HTREEITEM hParent, HTREEITEM hInsertAfter )
{
	DWORD uiMask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	if( nodeData )
	{
		uiMask |= TVIF_PARAM;

		return InsertItem( uiMask, szText, iImage, iImage, 0, 0, (LPARAM)nodeData, hParent, hInsertAfter );
	}

	return InsertItem( uiMask, szText, iImage, iImage, 0, 0, NULL, hParent, hInsertAfter );
}

HTREEITEM CBaseTreeCtrl::InsertItem2( const char* szText, int iImage, HTREEITEM hParent, HTREEITEM hInsertAfter )
{
	return InsertItem2( szText, iImage, NULL, hParent, hInsertAfter );
}

void CBaseTreeCtrl::ReleaseTreeItem( const HTREEITEM hti )
{
	if( hti == NULL )
		return;

	//CString sss = GetItemText( hti );

	TNODE* pNode = (TNODE*)GetItemData( hti );

	if( pNode != NULL )
		int u = 0;

	SAFE_DELETE( pNode );
	//SetItemData( hti, (LPARAM)0 );

	HTREEITEM hti_ch = GetChildItem( hti );

	while( hti_ch != NULL )
	{
		ReleaseTreeItem( hti_ch );
		hti_ch = GetNextSiblingItem( hti_ch );
	}
};

void CBaseTreeCtrl::OnTvnDeleteitem( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	ReleaseTreeItem( pNMTreeView->itemOld.hItem );

	*pResult = 0;
}
