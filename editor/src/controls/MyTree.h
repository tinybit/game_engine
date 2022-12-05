#pragma once

#include "../utils/common.h"
#include "BaseTreeCtrl.h"

using namespace TREENODE;

enum TREE_NODE_TYPE
{
	TNT_UNDEFINED,
	TNT_MESHES,
	TNT_MESH,
	TNT_LIGHTS,
	TNT_LIGHT,
	TNT_CAMERAS,
	TNT_CAMERA,
	TNT_NULLS,
	TNT_NULL,
	TNT_CUST_PARAM,
	TNT_TEXTURE,
	TNT_MATERIAL,

	TNT_SCN_CUST_TYPES,
	TNT_SCN_CUST_TYPE,
	TNT_SCN_CUST_OBJECTS,
	TNT_SCN_CUST_OBJECT,
	TNT_SCN_CURVES,
	TNT_SCN_CURVE,
	TNT_SCN_CURVE_NULL,
	TNT_SCN_TEXTURES,
	TNT_SCN_CAMERA_BIND,
	TNT_SCN_CAMERA_MISC_BIND,
	TNT_SCN_LEVEL
};

class CMyTree : public CBaseTreeCtrl
{
	DECLARE_DYNAMIC(CMyTree)

	CWnd*	m_pParent;
	bool	m_bIsLabelEdited;
	CEdit*	m_editLabel;
	enum TREE_NODE_TYPE eCurrNodeType;

	public:
		CMyTree();
		virtual ~CMyTree();

		afx_msg void	OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void	OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void	OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void	OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
		void			SetParent( CWnd* pParent ) { m_pParent = pParent; };

		enum TREE_NODE_TYPE	GetCurrNodeType() { return eCurrNodeType; };

		void			EndLabelEditing();
		bool			IsLabelEdited() { return m_bIsLabelEdited; };
		void			StartEditingLabel();

		DECLARE_MESSAGE_MAP()
		afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


