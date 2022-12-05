#pragma once

#include "../utils/Common.h"

enum BindMode
{
	BM_TEXTURE,
	BM_MESH2CAM,
	BM_MESH2TEX,
	BM_MESHES2CAM,
	BM_MESHES2TEX,
	BM_LIGHT,
	BM_CUST_OBJECT
};

#define ALLCAMERAS "-- All Cameras --"

class CQuickBindDlg : public CDialog
{
	DECLARE_DYNAMIC(CQuickBindDlg)

	StrVec			m_ObjectsID;
	str_t			m_strObjectID;
	str_t			m_strWindowCaption;
	enum BindMode	m_BindMode;
	ListStrVec*		m_pBinds;
	str_t			m_ParType, m_ParID, m_ChildType, m_ChildID;
	str_t			m_strSceneID;
	CWnd*			m_pParent;
	QuickBind*		m_QuickBind;

	public:
		CQuickBindDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CQuickBindDlg();

		void	BindTexture( str_t strTextureID );
		void	BindCustomObj( str_t strCustObjID );
		void	BindMesh2Camera( str_t strMeshID );
		void	BindMesh2Texture( str_t strMeshID );
		void	BindMeshes2Camera( StrVec* strMeshesID );
		void	BindMeshes2Texture( StrVec* strMeshesID );
		void	BindLight( str_t strLightID );
		void	SetQBindItem( QuickBind* qb ) { m_QuickBind =  qb; };
		void	SetBinds( ListStrVec* pBinds ) { m_pBinds = pBinds; };
		void	SetSceneID( str_t strSceneID ) { m_strSceneID = strSceneID; };
		bool	BindExistsOnTheList( str_t ParType, str_t ParID, str_t ChildType, str_t ChildID );
		void	AddBindToTheList( str_t ParType, str_t ParID, str_t ChildType, str_t ChildID );
		int		GetMaxIndexFromBinds();

		enum { IDD = IDD_QUICKBIND };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnBnClickedOk();
};
