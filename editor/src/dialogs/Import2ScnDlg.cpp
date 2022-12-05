#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "Import2ScnDlg.h"




IMPLEMENT_DYNAMIC(CImport2ScnDlg, CDialog)
CImport2ScnDlg::CImport2ScnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImport2ScnDlg::IDD, pParent)
{
	m_TargetType = FT_UNDEFINED;
}

CImport2ScnDlg::~CImport2ScnDlg()
{
}

void CImport2ScnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_IMPORT_DATA, m_lstImportData);
	DDX_Control(pDX, IDC_LST_CHOSEN_DATA, m_lstChosenData);
}


BEGIN_MESSAGE_MAP(CImport2ScnDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_OK, OnBnClickedBtnOk)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_LBN_DBLCLK(IDC_LST_IMPORT_DATA, OnLbnDblclkLstImportData)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBnClickedBtnRemove)
	ON_LBN_DBLCLK(IDC_LST_CHOSEN_DATA, OnLbnDblclkLstChosenData)
	ON_BN_CLICKED(IDC_BTN_ADD_ALL, OnBnClickedBtnAddAll)
	ON_BN_CLICKED(IDC_BTN_REMOVE_ALL, OnBnClickedBtnRemoveAll)
END_MESSAGE_MAP()


// CImport2ScnDlg message handlers

void CImport2ScnDlg::OnBnClickedBtnCancel()
{
	OnOK();
}

void CImport2ScnDlg::DoImportFromSCN2SCN()
{
	StrVec			vecCameras, vecLights, vecMeshes, vecNulls;
	bool			bMeshReplaceAll		= false;
	bool			bMeshReplaceOne		= false;
	CSceneFile*		scene				= (CSceneFile*)m_pSceneData;
	CEfnMeshlist*	meshlist			= (CEfnMeshlist*)scene->GetMeshlist();
	CSceneFile*		pImpScn				= (CSceneFile*)m_pImportData;
	CEfnMeshlist*	pImpMeshlist		= (CEfnMeshlist*)pImpScn->GetMeshlist();

	//select all
	m_lstChosenData.SetSel( -1, true );

	/**********  build list of objects ***************/
	int					objs_count = m_lstChosenData.GetSelCount();
	CArray< int, int >	aryListBoxSel;
	CString				temp;
	StrVec				vec;
	
	aryListBoxSel.SetSize( objs_count );
	m_lstChosenData.GetSelItems( objs_count, aryListBoxSel.GetData() );

	for( int i = 0; i < objs_count; i++ )
	{
		str_t str_temp = GetListItemText( &m_lstChosenData, aryListBoxSel[i] ).GetBuffer();

		if( str_temp[0] == '+' )
			str_temp = str_temp.substr( 1, str_temp.length() - 1 );

		str_t obj_text = str_temp;
		str_t obj_type = StripTypeHeader( obj_text );

		if( obj_type == "Camera" )
			vecCameras.push_back( obj_text );

		if( obj_type == "Light" )
			vecLights.push_back( obj_text );

		if( obj_type == "Mesh" )
			vecMeshes.push_back( obj_text );

		if( obj_type == "Null" )
			vecNulls.push_back( obj_text );
	}

	/*************** import meshes ******************/
	for( unsigned int i = 0; i < vecMeshes.size(); i++ )
	{
		bMeshReplaceOne = false;

		//check whether mesh already exists in scene
		if( !bMeshReplaceAll && meshlist->GetMeshByName( vecMeshes[i] ) )
		{
			CString err_string = "Mesh with ID \"";
			err_string += vecMeshes[i].data();
			err_string += "\" already exists! ";

			CReplaceWrnDlg dlg;
			dlg.SetWarningMessage( err_string );
			dlg.DoModal();

			if( dlg.GetRetCode() == IDCANCEL )
				continue;

			if( dlg.GetRetCode() == IDRETRY )
				bMeshReplaceAll = true;

			if( dlg.GetRetCode() == IDOK )
				bMeshReplaceOne = true;
		}

		if( bMeshReplaceAll || bMeshReplaceOne )
		{
			if( meshlist->GetMeshByName( vecMeshes[i] ) )
				meshlist->DeleteMesh( vecMeshes[i] );
		}

		meshlist->AddMesh( vecMeshes[i] );
		meshlist->GetMeshByName( vecMeshes[i] )->InitFromCMesh( pImpMeshlist->GetMeshByName( vecMeshes[i] ) );
	}	

	/*********** import nulls ***************/
	for( unsigned int i = 0; i < vecNulls.size(); i++ )
	{
		bMeshReplaceOne = false;

		//check whether null already exists in scene
		if( !bMeshReplaceAll && meshlist->GetMeshByName( "Nulls" )->GetNullByName( vecNulls[i] ) )
		{
			CString err_string = "Null with ID \"";
			err_string += vecNulls[i].data();
			err_string += "\" already exists! ";

			CReplaceWrnDlg dlg;
			dlg.SetWarningMessage( err_string );
			dlg.DoModal();

			if( dlg.GetRetCode() == IDCANCEL )
				continue;

			if( dlg.GetRetCode() == IDRETRY )
				bMeshReplaceAll = true;

			if( dlg.GetRetCode() == IDOK )
				bMeshReplaceOne = true;
		}

		if( bMeshReplaceAll || bMeshReplaceOne )
		{
			if( meshlist->GetMeshByName( "Nulls" )->GetNullByName( vecNulls[i] ) )
				meshlist->GetMeshByName( "Nulls" )->RemoveNull( vecNulls[i] );
		}

		CNULL* null = pImpMeshlist->GetMeshByName( "Nulls" )->GetNullByName( vecNulls[i] );
		null->attachment = NULL;

		meshlist->GetMeshByName( "Nulls" )->AddNull( null );
	}

	/*********** import cameras ***************/
	for( unsigned int i = 0; i < vecCameras.size(); i++ )
	{
		CCAMERA2* pCam	= NULL;
		bMeshReplaceOne = false;

		//check whether cam already exists in scene
		if( !bMeshReplaceAll && scene->GetCameraByID( vecCameras[i] ) )
		{
			CString err_string = "Camera with ID \"";
			err_string += vecCameras[i].data();
			err_string += "\" already exists! ";

			CReplaceWrnDlg dlg;
			dlg.SetWarningMessage( err_string );
			dlg.DoModal();

			if( dlg.GetRetCode() == IDCANCEL )
				continue;

			if( dlg.GetRetCode() == IDRETRY )
				bMeshReplaceAll = true;

			if( dlg.GetRetCode() == IDOK )
				bMeshReplaceOne = true;
		}

		if( bMeshReplaceAll || bMeshReplaceOne )
		{
			pCam = scene->GetCameraByID( vecCameras[i] );
		}
		else
		{
			//add cam
			pCam = scene->AddCamera( vecCameras[i] );
		}
	
		//init cam data
		if( pCam )
		{
			*pCam = *(pImpScn->GetCameraByID( vecCameras[i] ));
		}
	}


	/*********** import lights ***************/
	for( unsigned int i = 0; i < vecLights.size(); i++ )
	{
		CLIGHT* pLight	= NULL;
		bMeshReplaceOne	= false;

		//check whether cam already exists in scene
		if( !bMeshReplaceAll && scene->GetLightByID( vecLights[i] ) )
		{
			CString err_string = "Light with ID \"";
			err_string += vecLights[i].data();
			err_string += "\" already exists! ";

			CReplaceWrnDlg dlg;
			dlg.SetWarningMessage( err_string );
			dlg.DoModal();

			if( dlg.GetRetCode() == IDCANCEL )
				continue;

			if( dlg.GetRetCode() == IDRETRY )
				bMeshReplaceAll = true;

			if( dlg.GetRetCode() == IDOK )
				bMeshReplaceOne = true;
		}

		if( bMeshReplaceAll || bMeshReplaceOne )
		{
			pLight = scene->GetLightByID( vecLights[i] );
		}
		else
		{
			//add cam
			pLight = scene->AddLight( vecLights[i] );
		}
	
		//init cam data
		if( pLight )
		{
			*pLight = *pImpScn->GetLightByID( vecLights[i] );
		}
	}
}

void CImport2ScnDlg::DoImportFromXSI2SCN()
{
	StrVec			vecCameras, vecLights, vecMeshes, vecNulls;
	bool			bMeshReplaceAll		= false;
	bool			bMeshReplaceOne		= false;
	CSceneFile*		scene				= (CSceneFile*)m_pSceneData;
	CEfnMeshlist*	meshlist			= (CEfnMeshlist*)scene->GetMeshlist();
	CXSIFile*		xsi_char			= (CXSIFile*)m_pImportData;

	//select all
	m_lstChosenData.SetSel( -1, true );

	/**********  build list of objects ***************/
	int					objs_count = m_lstChosenData.GetSelCount();
	CArray< int, int >	aryListBoxSel;
	CString				temp;
	StrVec				vec;
	
	aryListBoxSel.SetSize( objs_count );
	m_lstChosenData.GetSelItems( objs_count, aryListBoxSel.GetData() );

	for( int i = 0; i < objs_count; i++ )
	{
		str_t str_temp = GetListItemText( &m_lstChosenData, aryListBoxSel[i] ).GetBuffer();

		if( str_temp[0] == '+' )
			str_temp = str_temp.substr( 1, str_temp.length() - 1 );

		str_t obj_text = str_temp;
		str_t obj_type = StripTypeHeader( obj_text );

		if( obj_type == "Camera" )
			vecCameras.push_back( obj_text );

		if( obj_type == "Light" )
			vecLights.push_back( obj_text );

		if( obj_type == "Mesh" )
			vecMeshes.push_back( obj_text );

		if( obj_type == "Null" )
			vecNulls.push_back( obj_text );
	}

	/*************** import meshes ******************/
	for( unsigned int i = 0; i < vecMeshes.size(); i++ )
	{
		bMeshReplaceOne = false;

		//check whether mesh already exists in scene
		if( !bMeshReplaceAll && meshlist->GetMeshByName( vecMeshes[i] ) )
		{
			CString err_string = "Mesh with ID \"";
			err_string += vecMeshes[i].data();
			err_string += "\" already exists! ";

			CReplaceWrnDlg dlg;
			dlg.SetWarningMessage( err_string );
			dlg.DoModal();

			if( dlg.GetRetCode() == IDCANCEL )
				continue;

			if( dlg.GetRetCode() == IDRETRY )
				bMeshReplaceAll = true;

			if( dlg.GetRetCode() == IDOK )
				bMeshReplaceOne = true;
		}

		if( bMeshReplaceAll || bMeshReplaceOne )
		{
			if( meshlist->GetMeshByName( vecMeshes[i] ) )
				meshlist->DeleteMesh( vecMeshes[i] );
		}

		meshlist->AddMesh( vecMeshes[i] );
		meshlist->GetMeshByName( vecMeshes[i] )->InitFromXSIMesh( xsi_char->GetMeshByName( vecMeshes[i] ) );
	}	

	///*********** import nulls ***************/
	//for( unsigned int i = 0; i < vecNulls.size(); i++ )
	//{
	//	bMeshReplaceOne = false;

	//	//check whether null already exists in scene
	//	if( !bMeshReplaceAll && meshlist->GetMeshByName( "Nulls" )->GetNullByName( vecNulls[i] ) )
	//	{
	//		CString err_string = "Null with ID \"";
	//		err_string += vecNulls[i].data();
	//		err_string += "\" already exists! ";

	//		CReplaceWrnDlg dlg;
	//		dlg.SetWarningMessage( err_string );
	//		dlg.DoModal();

	//		if( dlg.GetRetCode() == IDCANCEL )
	//			continue;

	//		if( dlg.GetRetCode() == IDRETRY )
	//			bMeshReplaceAll = true;

	//		if( dlg.GetRetCode() == IDOK )
	//			bMeshReplaceOne = true;
	//	}

	//	if( bMeshReplaceAll || bMeshReplaceOne )
	//	{
	//		if( meshlist->GetMeshByName( "Nulls" )->GetNullByName( vecNulls[i] ) )
	//			meshlist->GetMeshByName( "Nulls" )->RemoveNull( vecNulls[i] );
	//	}

	//	CNULL2* null = xsi_char->GetNullByName( vecNulls[i] );
	//	null->attachment = NULL;

	//	meshlist->GetMeshByName( "Nulls" )->AddNull( null );
	//}

	/*********** import cameras ***************/
	for( unsigned int i = 0; i < vecCameras.size(); i++ )
	{
		CCAMERA2* pCam	= NULL;
		bMeshReplaceOne = false;

		//check whether cam already exists in scene
		if( !bMeshReplaceAll && scene->GetCameraByID( vecCameras[i] ) )
		{
			CString err_string = "Camera with ID \"";
			err_string += vecCameras[i].data();
			err_string += "\" already exists! ";

			CReplaceWrnDlg dlg;
			dlg.SetWarningMessage( err_string );
			dlg.DoModal();

			if( dlg.GetRetCode() == IDCANCEL )
				continue;

			if( dlg.GetRetCode() == IDRETRY )
				bMeshReplaceAll = true;

			if( dlg.GetRetCode() == IDOK )
				bMeshReplaceOne = true;
		}

		if( bMeshReplaceAll || bMeshReplaceOne )
		{
			pCam = scene->GetCameraByID( vecCameras[i] );
		}
		else
		{
			//add cam
			pCam = scene->AddCamera( vecCameras[i] );
		}
	
		//init cam data
		if( pCam )
		{
			int		cam_idx = xsi_char->GetCameraIndexByName( vecCameras[i] );
			CCAMERA* cam_imp = xsi_char->GetCameraByIndex( cam_idx );

			*pCam = *cam_imp;
		}
	}


	/*********** import lights ***************/
	for( unsigned int i = 0; i < vecLights.size(); i++ )
	{
		CLIGHT* pLight	= NULL;
		bMeshReplaceOne	= false;

		//check whether cam already exists in scene
		if( !bMeshReplaceAll && scene->GetLightByID( vecLights[i] ) )
		{
			CString err_string = "Light with ID \"";
			err_string += vecLights[i].data();
			err_string += "\" already exists! ";

			CReplaceWrnDlg dlg;
			dlg.SetWarningMessage( err_string );
			dlg.DoModal();

			if( dlg.GetRetCode() == IDCANCEL )
				continue;

			if( dlg.GetRetCode() == IDRETRY )
				bMeshReplaceAll = true;

			if( dlg.GetRetCode() == IDOK )
				bMeshReplaceOne = true;
		}

		if( bMeshReplaceAll || bMeshReplaceOne )
		{
			pLight = scene->GetLightByID( vecLights[i] );
		}
		else
		{
			//add cam
			pLight = scene->AddLight( vecLights[i] );
		}
	
		//init cam data
		if( pLight )
		{
			int		 light_idx	= xsi_char->GetLightIndexByName( vecLights[i] );
			CLIGHT*  light_imp	= xsi_char->GetLightByIndex( light_idx );
			memcpy( pLight, light_imp, sizeof( CLIGHT ) );
		}
	}
}

void CImport2ScnDlg::OnBnClickedBtnOk()
{
	if( m_TargetType == FT_SCN )
	{
		if( m_pImportData->GetType() == FT_XSI )
		{
			DoImportFromXSI2SCN();
		}
		else if( m_pImportData->GetType() == FT_SCN )
		{
			DoImportFromSCN2SCN();
		}
		else
		{}
	}
	else if( m_TargetType == FT_EFN_MESHLIST )
	{
		StrVec			vecMeshes;
		bool			bMeshReplaceAll		= false;
		bool			bMeshReplaceOne		= false;
		CEfnMeshlist*	meshlist			= (CEfnMeshlist*)m_pSceneData;
		CXSIFile*		xsi_char			= (CXSIFile*)m_pImportData;

		//select all
		m_lstChosenData.SetSel( -1, true );

		/**********  build list of objects ***************/
		int					objs_count = m_lstChosenData.GetSelCount();
		CArray< int, int >	aryListBoxSel;
		CString				temp;
		StrVec				vec;
		
		aryListBoxSel.SetSize( objs_count );
		m_lstChosenData.GetSelItems( objs_count, aryListBoxSel.GetData() );

		for( int i = 0; i < objs_count; i++ )
		{
			str_t str_temp = GetListItemText( &m_lstChosenData, aryListBoxSel[i] ).GetBuffer();

			if( str_temp[0] == '+' )
				str_temp = str_temp.substr( 1, str_temp.length() - 1 );

			str_t obj_text = str_temp;
			str_t obj_type = StripTypeHeader( obj_text );

			if( obj_type == "Mesh" )
				vecMeshes.push_back( obj_text );
		}

		/*************** import meshes ******************/
		for( unsigned int i = 0; i < vecMeshes.size(); i++ )
		{
			bMeshReplaceOne = false;

			//check whether mesh already exists in scene
			if( !bMeshReplaceAll && meshlist->GetMeshByName( vecMeshes[i] ) )
			{
				CString err_string = "Mesh with ID \"";
				err_string += vecMeshes[i].data();
				err_string += "\" already exists! ";

				CReplaceWrnDlg dlg;
				dlg.SetWarningMessage( err_string );
				dlg.DoModal();

				if( dlg.GetRetCode() == IDCANCEL )
					continue;

				if( dlg.GetRetCode() == IDRETRY )
					bMeshReplaceAll = true;

				if( dlg.GetRetCode() == IDOK )
					bMeshReplaceOne = true;
			}

			if( bMeshReplaceAll || bMeshReplaceOne )
			{
				if( meshlist->GetMeshByName( vecMeshes[i] ) )
					meshlist->DeleteMesh( vecMeshes[i] );
			}

			meshlist->AddMesh( vecMeshes[i] );
			meshlist->GetMeshByName( vecMeshes[i] )->InitFromXSIMesh( xsi_char->GetMeshByName( vecMeshes[i] ) );
		}	
	}
	else
	{}

	OnOK();
}

void CImport2ScnDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( m_pImportData->GetType() == FT_XSI )
	{
		CString		temp;
		CXSIFile*	xsi_char = (CXSIFile*)m_pImportData;

		for( int i = 0; i < xsi_char->GetCamerasCount(); i++ )
		{
			CCAMERA* pCam = xsi_char->GetCameraByIndex( i );
			temp = "Camera.";
			temp += (char*)pCam->name.data();

			m_lstImportData.AddString( temp );
		}

		for( int i = 0; i < xsi_char->GetLightsCount(); i++ )
		{
			CLIGHT* light = xsi_char->GetLightByIndex( i );
			temp = "Light.";
			temp += (char*)light->name.data();

			m_lstImportData.AddString( temp );
		}

		for( int i = 0; i < xsi_char->GetMeshesCount(); i++ )
		{
			CXSIMesh* mesh = xsi_char->GetMeshByIndex( i );
			temp = "Mesh.";
			temp += mesh->GetName();

			m_lstImportData.AddString( temp );
		}

		for( int i = 0; i < xsi_char->GetNullsCount(); i++ )
		{
			CNULL2* null = xsi_char->GetNullByIndex( i );
			temp = "Null.";
			temp += null->id.data();

			m_lstImportData.AddString( temp );
		}
	}
	if( m_pImportData->GetType() == FT_SCN )
	{
		CString			temp;
		CSceneFile*		pScene		= (CSceneFile*)m_pImportData;
		CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

		for( int i = 0; i < pScene->GetCamerasCount(); i++ )
		{
			CCAMERA2* pCam = pScene->GetCameraByIndex( i );
			temp = "Camera.";
			temp += (char*)pCam->name.data();

			m_lstImportData.AddString( temp );
		}

		for( int i = 0; i < pScene->GetLightsCount(); i++ )
		{
			CLIGHT* light = pScene->GetLightByIndex( i );
			temp = "Light.";
			temp += (char*)light->name.data();

			m_lstImportData.AddString( temp );
		}

		for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
		{
			CMesh*	pMesh = pMeshlist->GetMeshByIndex( i );
			str_t	temp;

			if( pMesh->GetName() != "Nulls" )
			{
				temp = "Mesh.";
				temp += pMesh->GetName();

				m_lstImportData.AddString( temp.data() );
			}
			else
			{
				for( int i = 0; i < pMesh->GetNullsCount(); i++ )
				{
					CNULL* null = pMesh->GetNullByIndex( i );
					temp = "Null.";
					temp += null->id.data();

					m_lstImportData.AddString( temp.data() );
				}			
			}
		}
	}
}

__inline CString CImport2ScnDlg::GetListItemText( CListBox* m_lstBox, int iIndex )
{
	CString temp;
	int text_len		= m_lstBox->GetTextLen( iIndex );
	m_lstBox->GetText( iIndex, temp.GetBuffer( text_len ) );

	return temp;
}

void CImport2ScnDlg::OnBnClickedBtnAdd()
{
	if( m_TargetType == FT_SCN )
	{
		int					objs_count = m_lstImportData.GetSelCount();
		CArray< int, int >	aryListBoxSel;
		CString				temp;
		StrVec				vec;
		CSceneFile*			scene		= (CSceneFile*)m_pSceneData;
		CEfnMeshlist*		meshlist	= (CEfnMeshlist*)scene->GetMeshlist();

		aryListBoxSel.SetSize( objs_count );
		m_lstImportData.GetSelItems( objs_count, aryListBoxSel.GetData() );

		for( int i = 0; i < objs_count; i++ )
			vec.push_back( GetListItemText( &m_lstImportData, aryListBoxSel[i] ).GetBuffer() );

		for( unsigned int i = 0; i < vec.size(); i++ )
		{
			m_lstImportData.DeleteString( m_lstImportData.FindString( 0, vec[i].data() ) );

			str_t obj_text = vec[i];
			str_t obj_type = StripTypeHeader( obj_text );


			if( obj_type == "Camera" )
			{
				CCAMERA2* cam = scene->GetCameraByID( obj_text );

				if( cam )
					obj_text = "+" +  vec[i];
				else
					obj_text = vec[i];
			}

			if( obj_type == "Light" )
			{
				CLIGHT* light = scene->GetLightByID( obj_text );

				if( light )
					obj_text = "+" +  vec[i];
				else
					obj_text = vec[i];
			}

			if( obj_type == "Mesh" )
			{
				CMesh* mesh = meshlist->GetMeshByName( obj_text );

				if( mesh )
					obj_text = "+" +  vec[i];
				else
					obj_text = vec[i];
			}

			if( obj_type == "Null" )
			{
				CNULL* null = meshlist->GetMeshByName( "Nulls" )->GetNullByName( obj_text );

				if( null )
					obj_text = "+" +  vec[i];
				else
					obj_text = vec[i];
			}

			m_lstChosenData.AddString( obj_text.data() );
		}
	}
	else if( m_TargetType == FT_EFN_MESHLIST )
	{
		int					objs_count = m_lstImportData.GetSelCount();
		CArray< int, int >	aryListBoxSel;
		CString				temp;
		StrVec				vec;
		CEfnMeshlist*		meshlist	= (CEfnMeshlist*)m_pSceneData;

		aryListBoxSel.SetSize( objs_count );
		m_lstImportData.GetSelItems( objs_count, aryListBoxSel.GetData() );

		for( int i = 0; i < objs_count; i++ )
			vec.push_back( GetListItemText( &m_lstImportData, aryListBoxSel[i] ).GetBuffer() );

		for( unsigned int i = 0; i < vec.size(); i++ )
		{
			m_lstImportData.DeleteString( m_lstImportData.FindString( 0, vec[i].data() ) );

			str_t obj_text = vec[i];
			str_t obj_type = StripTypeHeader( obj_text );


			if( obj_type == "Mesh" )
			{
				CMesh* mesh = meshlist->GetMeshByName( obj_text );

				if( mesh )
					obj_text = "+" +  vec[i];
				else
					obj_text = vec[i];
			}

			m_lstChosenData.AddString( obj_text.data() );
		}
	}
	else
	{}
}

void CImport2ScnDlg::OnLbnDblclkLstImportData()
{
	OnBnClickedBtnAdd();
}

void CImport2ScnDlg::OnBnClickedBtnRemove()
{
	int					objs_count = m_lstChosenData.GetSelCount();
	CArray< int, int >	aryListBoxSel;
	CString				temp;
	StrVec				vec;
	
	aryListBoxSel.SetSize( objs_count );
	m_lstChosenData.GetSelItems( objs_count, aryListBoxSel.GetData() );

	for( int i = 0; i < objs_count; i++ )
		vec.push_back( GetListItemText( &m_lstChosenData, aryListBoxSel[i] ).GetBuffer() );

	for( unsigned int i = 0; i < vec.size(); i++ )
	{
		m_lstChosenData.DeleteString( m_lstChosenData.FindString( 0, vec[i].data() ) );

		str_t item_text = vec[i];

		if( item_text[0] == '+' )
			item_text = item_text.substr( 1, item_text.length() - 1 );

		m_lstImportData.AddString( item_text.data() );
	}
}

void CImport2ScnDlg::OnLbnDblclkLstChosenData()
{
	OnBnClickedBtnRemove();
}

void CImport2ScnDlg::OnBnClickedBtnAddAll()
{
	m_lstImportData.SetSel( -1, true );
	OnBnClickedBtnAdd();
}

void CImport2ScnDlg::OnBnClickedBtnRemoveAll()
{
	m_lstChosenData.SetSel( -1, true );
	OnBnClickedBtnRemove();
}
