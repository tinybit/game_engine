#pragma once

#include "../utils/commondefines.h"

using namespace TREENODE;

class CBaseTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CBaseTreeCtrl)

	COLORREF m_clrBgColor;

	public:
		CBaseTreeCtrl();
		virtual ~CBaseTreeCtrl();

		afx_msg void	OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void	OnPaint();
		afx_msg void	OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
		virtual BOOL	OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

		void			DeselectAllNodes();
		void			DeselectAllNodesRecurse( HTREEITEM hRoot );
		void			ReleaseTreeItem( const HTREEITEM hti );

		HTREEITEM		FindTreeItemByName( CString strItemName, HTREEITEM hRootItem = NULL, bool bRecursive = true );
		HTREEITEM		FindTreeItemByName( str_t strItemName, HTREEITEM hRootItem = NULL, bool bRecursive = true );
		HTREEITEM		FindTreeItemByName( char* strItemName, HTREEITEM hRootItem = NULL, bool bRecursive = true );
		HTREEITEM		FindTreeItemByName( const char* strItemName, HTREEITEM hRootItem = NULL, bool bRecursive = true );

		void*			GetCurrNodeData();
		int				GetCurrNodeImage();
		CString			GetCurrNodeText();

		void			SetCurrNodeData( void* pData );
		void			SetCurrNodeImage( int iImgIdx );
		void			SetCurrNodeText( CString strNewID );

		CString			GetParNodeText();
		CString			GetPar2NodeText();
		HTREEITEM 		GetParNode();
		HTREEITEM 		GetPar2Node();
		void			ReleaseTreeData();

		HTREEITEM		InsertItem2( const char* szText, int iImage, void* nodeData = NULL, HTREEITEM hParent = NULL, HTREEITEM hInsertAfter = NULL );
		HTREEITEM		InsertItem2( const char* szText, int iImage, HTREEITEM hParent, HTREEITEM hInsertAfter = NULL );

		DECLARE_MESSAGE_MAP()
};


