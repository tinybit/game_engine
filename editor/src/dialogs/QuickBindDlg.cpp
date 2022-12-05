#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "QuickBindDlg.h"
#include "CookieMachineDlg.h"




IMPLEMENT_DYNAMIC(CQuickBindDlg, CDialog)
CQuickBindDlg::CQuickBindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickBindDlg::IDD, pParent)
{
	m_pBinds		= NULL;
	m_QuickBind		= NULL;
	m_strSceneID	= "";
	m_pParent		= pParent;
}

CQuickBindDlg::~CQuickBindDlg()
{
}

BEGIN_MESSAGE_MAP(CQuickBindDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


void CQuickBindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CQuickBindDlg::BindTexture( str_t strTextureID )
{
	m_strWindowCaption	= "Bind texture to camera";
	m_strObjectID		= strTextureID;
	m_BindMode			= BM_TEXTURE;
}

void CQuickBindDlg::BindMesh2Camera( str_t strMeshID )
{
	m_strWindowCaption	= "Bind mesh to camera";
	m_strObjectID		= strMeshID;
	m_BindMode			= BM_MESH2CAM;
}

void CQuickBindDlg::BindMeshes2Camera( StrVec* strMeshesID )
{
	m_strWindowCaption	= "Bind meshes to camera";
	m_BindMode			= BM_MESHES2CAM;

	m_ObjectsID.assign( strMeshesID->begin(), strMeshesID->end() );
}

void CQuickBindDlg::BindMeshes2Texture( StrVec* strMeshesID )
{
	m_strWindowCaption	= "Bind meshes to texture";
	m_BindMode			= BM_MESHES2TEX;

	m_ObjectsID.assign( strMeshesID->begin(), strMeshesID->end() );
}

void CQuickBindDlg::BindCustomObj( str_t strCustObjID )
{
	m_strWindowCaption	= "Bind custom object to camera";
	m_strObjectID		= strCustObjID;
	m_BindMode			= BM_CUST_OBJECT;
}

void CQuickBindDlg::BindMesh2Texture( str_t strMeshID )
{
	m_strWindowCaption	= "Bind mesh to texture";
	m_strObjectID		= strMeshID;
	m_BindMode			= BM_MESH2TEX;
}

void CQuickBindDlg::BindLight( str_t strLightID )
{
	m_strWindowCaption	= "Bind light to camera";
	m_strObjectID		= strLightID;
	m_BindMode			= BM_LIGHT;
}

void CQuickBindDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CComboBox*	pChildCmb	= (CComboBox*)GetDlgItem( IDC_CMB_CHILD_TYPE );
	CComboBox*	pParentCmb	= (CComboBox*)GetDlgItem( IDC_CMB_PARENT_TYPE );
	CComboBox*	pParentID	= (CComboBox*)GetDlgItem( IDC_CMB_PARENT_ID );
	CTreeCtrl*	pParentIDTr	= (CTreeCtrl*)GetDlgItem( IDC_TREE_PARENT_ID );
		
	CWnd*		pWnd		= GetDlgItem( IDC_EDIT_OBJECT_ID );
    StrVec		types;
	int			iTemp		= 0;
	ListStrVec_iter iter	= m_pBinds->begin();

	pWnd->SetWindowText( m_strObjectID.data() );

	if( !m_pBinds || m_strSceneID == "" )
	{
		MessageBox( "Initialization error! Check params.", "Внимание!", MB_ICONERROR | MB_OK );
		OnOK();
	}

	this->SetWindowText( m_strWindowCaption.data() );

	//fill type combos
	GetTypesList( types );

	switch( m_BindMode )
	{
		case BM_CUST_OBJECT:
			types.clear();
			types.push_back( "Bird" );
			types.push_back( "Car" );
			types.push_back( "Free" );
			types.push_back( "Man" );
			types.push_back( "ParticlesMgr" );
			types.push_back( "Video3D" );
			break;

		case BM_MESH2CAM:
			types.clear();
			types.push_back( "Action" );
			types.push_back( "CameraSelect" );
			types.push_back( "DynamicFloor" );
			types.push_back( "Floor" );
			types.push_back( "OcclusionGeom" );
			types.push_back( "ShadowCaster" );
			types.push_back( "ShadowReceiver" );
			types.push_back( "ShadowVolume" );
			types.push_back( "SndOcclusionGeom" );
			break;
	}

	//get custom types
	CBaseScene* pBaseScene	= ((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	if( !pBaseScene )
		return;


	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

	for( unsigned int i = 0; i < m_CustomTypes->size(); i++ )
		types.push_back( (*m_CustomTypes)[i] );


	for( unsigned int i = 0; i < types.size(); i++ )
	{
		pChildCmb->AddString( types[i].data() );
		pParentCmb->AddString( types[i].data() );
	}

	switch( m_BindMode )
	{
		case BM_TEXTURE:

			iTemp = pChildCmb->FindString( 0, "Background" );
			pChildCmb->SetCurSel( iTemp );
			pChildCmb->EnableWindow( false );

			iTemp = pChildCmb->FindString( 0, "Camera" );
			pParentCmb->SetCurSel( iTemp );
			pParentCmb->EnableWindow( false );

			while( iter != m_pBinds->end() )
			{
				StrVec vec = *iter;
				
				if( vec[1] == m_strSceneID && vec[2] == "Scene" && vec[4] == "Camera" )
				{
					pParentID->AddString( vec[3].data() );
					pParentIDTr->InsertItem( vec[3].data(), TVI_ROOT, TVI_SORT );
				}

				iter++;
			}

			pParentID->SetCurSel( 0 );

			break;

		case BM_MESH2CAM:
			pChildCmb->SetCurSel( 0 );
			
			iTemp = pChildCmb->FindString( 0, "Camera" );
			pParentCmb->SetCurSel( iTemp );
			pParentCmb->EnableWindow( false );

			while( iter != m_pBinds->end() )
			{
				StrVec vec = *iter;
				
				if( vec[1] == m_strSceneID && vec[2] == "Scene" && vec[4] == "Camera" )
				{
					pParentID->AddString( vec[3].data() );
					pParentIDTr->InsertItem( vec[3].data(), TVI_ROOT, TVI_SORT );
				}

				iter++;
			}

			pParentID->AddString( ALLCAMERAS );
			pParentID->SetCurSel( 0 );

			break;

		case BM_CUST_OBJECT:
			pChildCmb->SetCurSel( 0 );
			
			iTemp = pChildCmb->FindString( 0, "Camera" );
			pParentCmb->SetCurSel( iTemp );
			pParentCmb->EnableWindow( false );

			while( iter != m_pBinds->end() )
			{
				StrVec vec = *iter;
				
				if( vec[1] == m_strSceneID && vec[2] == "Scene" && vec[4] == "Camera" )
				{
					pParentID->AddString( vec[3].data() );
					pParentIDTr->InsertItem( vec[3].data(), TVI_ROOT, TVI_SORT );
				}

				iter++;
			}

			pParentID->AddString( ALLCAMERAS );
			pParentID->SetCurSel( 0 );

			break;

		case BM_MESH2TEX:

			iTemp = pChildCmb->FindString( 0, "LayerGeom" );
			pChildCmb->SetCurSel( iTemp );
			pChildCmb->EnableWindow( false );
			
			iTemp = pChildCmb->FindString( 0, "Background" );
			pParentCmb->SetCurSel( iTemp );
			pParentCmb->EnableWindow( false );

			while( iter != m_pBinds->end() )
			{
				StrVec vec = *iter;
				
				if( vec[1] == m_strSceneID && vec[2] == "Scene" && vec[4] == "Background" )
				{
					pParentID->AddString( vec[3].data() );
					pParentIDTr->InsertItem( vec[3].data(), TVI_ROOT, TVI_SORT );
				}

				iter++;
			}

			pParentID->SetCurSel( 0 );

			break;

		case BM_LIGHT:

			iTemp = pChildCmb->FindString( 0, "Light" );
			pChildCmb->SetCurSel( iTemp );
			pChildCmb->EnableWindow( false );

			iTemp = pChildCmb->FindString( 0, "Camera" );
			pParentCmb->SetCurSel( iTemp );
			pParentCmb->EnableWindow( false );

			while( iter != m_pBinds->end() )
			{
				StrVec vec = *iter;
				
				if( vec[1] == m_strSceneID && vec[2] == "Scene" && vec[4] == "Camera" )
				{
					pParentID->AddString( vec[3].data() );
					pParentIDTr->InsertItem( vec[3].data(), TVI_ROOT, TVI_SORT );
				}

				iter++;
			}

			pParentID->AddString( ALLCAMERAS );
			pParentID->SetCurSel( 0 );

			break;
	}

	str_t strOT;

	switch( m_BindMode )
	{
		case BM_LIGHT:
			strOT = "Light";
			break;

		case BM_MESH2CAM:
			strOT = "MeshCam";
			break;

		case BM_MESH2TEX:
			strOT = "MeshTex";
			break;

		case BM_CUST_OBJECT:
			strOT = "CustObj";
			break;

		case BM_TEXTURE:
			strOT = "Texture";
			break;
	}

	//set previous quick bind values
	if( strOT == m_QuickBind->m_strChildObjType )
	{
		//make sure both values are ok
		int idxA = pChildCmb->FindString( -1, m_QuickBind->m_strChildType.data() );
		int idxB = pParentID->FindString( -1, m_QuickBind->m_strParentID.data() );

		if( idxA != CB_ERR )
		{
			pChildCmb->SetCurSel( idxA );
		}

		if( idxB != CB_ERR )
		{
			pParentID->SetCurSel( idxB );
		}
	}

	m_QuickBind->m_strChildObjType = strOT;

	CString strTemp;

	pChildCmb->GetLBText( pChildCmb->GetCurSel(), strTemp );
	m_QuickBind->m_strChildType	= strTemp;

	pParentID->GetLBText( pParentID->GetCurSel(), strTemp );
	m_QuickBind->m_strParentID = strTemp;
}

bool CQuickBindDlg::BindExistsOnTheList( str_t ParType, str_t ParID, str_t ChildType, str_t ChildID )
{
	if( !m_pBinds )
		return false;

	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		StrVec vec = *iter;
		
		if( vec[1] == ParID && vec[2] == ParType && vec[3] == ChildID && vec[4] == ChildType )
			return true;

		iter++;
	}

	return false;
}

void CQuickBindDlg::AddBindToTheList( str_t ParType, str_t ParID, str_t ChildType, str_t ChildID )
{
	if( !m_pBinds )
		return;

	StrVec	bind;
	CString strIndex;
	int		iMaxIndex = GetMaxIndexFromBinds();

	iMaxIndex++;

	if( iMaxIndex < 10 )
	{
		strIndex.Format( "00%i", iMaxIndex );
	}
	else if( iMaxIndex >= 10 && iMaxIndex < 100 )
	{
		strIndex.Format( "0%i", iMaxIndex );
	}
	else
	{
		strIndex.Format( "%i", iMaxIndex );
	}

	bind.push_back( strIndex.GetBuffer() );
	bind.push_back( ParID.data() );
	bind.push_back( ParType.data() );
	bind.push_back( ChildID.data() );
	bind.push_back( ChildType.data() );

	m_pBinds->push_back( bind );
}

int CQuickBindDlg::GetMaxIndexFromBinds()
{
	int idx = -1;

	if( !m_pBinds )
		return -1;

	ListStrVec_iter iter = m_pBinds->begin();

	while( iter != m_pBinds->end() )
	{
		StrVec vec = *iter;
		
		if( atoi( vec[0].data() ) > idx )
			idx = atoi( vec[0].data() );

		iter++;
	}

	return idx;
}

void CQuickBindDlg::OnBnClickedOk()
{
	CComboBox*	pChildCmb	= (CComboBox*)GetDlgItem( IDC_CMB_CHILD_TYPE );
	CComboBox*	pParentCmb	= (CComboBox*)GetDlgItem( IDC_CMB_PARENT_TYPE );
	CComboBox*	pParentID	= (CComboBox*)GetDlgItem( IDC_CMB_PARENT_ID );


	CString strTemp;

	switch( m_BindMode )
	{
		case BM_TEXTURE:
			m_ParType	= "Scene";
			m_ParID		= m_strSceneID;
			m_ChildType = "Background";
			m_ChildID	= m_strObjectID;
			break;

		case BM_MESH2CAM:
			pChildCmb->GetLBText( pChildCmb->GetCurSel(), strTemp );

			m_ParType	= "Scene";
			m_ParID		= m_strSceneID;
			m_ChildType = strTemp.GetBuffer();
			m_ChildID	= m_strObjectID;
			break;

		case BM_MESH2TEX:
			m_ParType	= "Scene";
			m_ParID		= m_strSceneID;
			m_ChildType = "LayerGeom";
			m_ChildID	= m_strObjectID;
			break;

		case BM_LIGHT:
			m_ParType	= "Scene";
			m_ParID		= m_strSceneID;
			m_ChildType = "Light";
			m_ChildID	= m_strObjectID;
			break;
	}
	

	if( !BindExistsOnTheList( m_ParType, m_ParID, m_ChildType, m_ChildID ) )
		AddBindToTheList( m_ParType, m_ParID, m_ChildType, m_ChildID );


	switch( m_BindMode )
	{
		case BM_CUST_OBJECT:
			pParentID->GetLBText( pParentID->GetCurSel(), strTemp );
			m_ParType	= "Camera";
			m_ParID		= strTemp.GetBuffer();

			pChildCmb->GetLBText( pChildCmb->GetCurSel(), strTemp );
			m_ChildType = strTemp.GetBuffer();
			m_ChildID	= m_strObjectID;
			break;

		case BM_TEXTURE:
			pParentID->GetLBText( pParentID->GetCurSel(), strTemp );
			m_ParType	= "Camera";
			m_ParID		= strTemp.GetBuffer();
			m_ChildType = "Background";
			m_ChildID	= m_strObjectID;
			break;

		case BM_MESH2CAM:
			pParentID->GetLBText( pParentID->GetCurSel(), strTemp );
			m_ParType	= "Camera";
			m_ParID		= strTemp.GetBuffer();

			pChildCmb->GetLBText( pChildCmb->GetCurSel(), strTemp );
			m_ChildType = strTemp.GetBuffer();
			m_ChildID	= m_strObjectID;
			break;

		case BM_MESH2TEX:
			pParentID->GetLBText( pParentID->GetCurSel(), strTemp );
            m_ParType	= "Background";
			m_ParID		= strTemp.GetBuffer();
			m_ChildType = "LayerGeom";
			m_ChildID	= m_strObjectID;
			break;

		case BM_LIGHT:
			pParentID->GetLBText( pParentID->GetCurSel(), strTemp );
			m_ParType	= "Camera";
			m_ParID		= strTemp.GetBuffer();
			m_ChildType = "Light";
			m_ChildID	= m_strObjectID;
			break;
	}

	if( m_ParType == "Camera" && m_ParID == ALLCAMERAS )
	{
			ListStrVec_iter iter = m_pBinds->begin();

			while( iter != m_pBinds->end() )
			{
				StrVec vec = *iter;
				
				if( vec[1] == m_strSceneID && vec[2] == "Scene" && vec[4] == "Camera" )
				{
					if( !BindExistsOnTheList( m_ParType, vec[3], m_ChildType, m_ChildID ) )
						AddBindToTheList( m_ParType, vec[3], m_ChildType, m_ChildID );
				}

				iter++;
			}
	}
	else
	{
		if( !BindExistsOnTheList( m_ParType, m_ParID, m_ChildType, m_ChildID ) )
		 AddBindToTheList( m_ParType, m_ParID, m_ChildType, m_ChildID );
	}

	m_QuickBind->m_strChildType	= m_ChildType;
	m_QuickBind->m_strParentID	= m_ParID;

	OnOK();
}
