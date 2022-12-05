#pragma once

#include "../SceneFile.h"
#include "../EfnMeshlist.h"
#include "../utils/common.h"

//enum TESS_MESH_DLG_MODE
//{
//	TMDM_UNDEFINED	= 0,
//	TMDM_BOX		= 1,
//	TMDM_CONE		= 2,
//	TMDM_CYLINDER	= 3,
//	TMDM_SPHERE		= 4,
//	TMDM_QUAD		= 5
//};

class CTesselatedMeshDlg : public CDialog
{
	enum TESS_MESH_DLG_MODE m_mode;
	D3DXVECTOR3				m_objPos;
	CSceneFile*				m_pScene;
	DECLARE_DYNAMIC(CTesselatedMeshDlg)

	bool ValidateData();
	void UpdateShadowMesh();

	public:
		CTesselatedMeshDlg(CWnd* pParent = NULL);
		virtual ~CTesselatedMeshDlg();

		enum { IDD = IDD_CREATE_MESH_TESS };

		void SetMode( enum TESS_MESH_DLG_MODE mode ) { m_mode = mode; };
		void SetObjectPosition( D3DXVECTOR3* objPos ) { m_objPos = *objPos; };
		void SetScene( CSceneFile* pScene ) { m_pScene = pScene; };
		BOOL Create();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
