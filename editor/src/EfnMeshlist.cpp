#include "sys/stdafx.h"
#include "sys/CookieMachine.h"

#include "EfnMeshlist.h"
#include "dialogs/CookieMachineDlg.h"


void CEfnMeshlist::Initialize()
{
	efn_type			= EF_MESHLIST;
	FileType			= FT_EFN_MESHLIST;
	m_pParentWnd		= NULL;
	m_pTexture			= NULL;
	m_bAllNullsSelected	= false;
	m_bRenderNormals	= false;
	m_dwFileVersion		= EFN_MESHLIST_VERSION_02;
}

int CEfnMeshlist::GetMeshesCount()
{
	if( m_vecMeshes.empty() )
		return 0;

	return m_vecMeshes.size();
}

CMesh* CEfnMeshlist::GetFirstSelectedMesh()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() )
			return m_vecMeshes[i];
	}

	return NULL;
}

float CEfnMeshlist::IntersectByRay( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation, enum OBJECT_TYPES& objType, str_t& strObjID )
{
	StrVec			meshes, nulls;
	vector< float >	distances;
	vector< float >	distances_nl;
	float			temp	= 999999.0f;
	int				idx		= -1;
	float			temp_nl	= 999999.0f;
	int				idx_nl	= -1;
	int				isected_object_type = 0;


	/*****************************/
	/*		intersect meshes	 */
	/*****************************/
	// oobb and bsphere intersections
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetVisible() )
		{
			if( m_vecMeshes[i]->IntersectOOBB( pFrom, pTo, mTransformation ) && m_vecMeshes[i]->IntersectBSphere( pFrom, pTo, mTransformation ) )
				meshes.push_back( m_vecMeshes[i]->GetName() );
		}
	}

	// poly intersections
	for( UINT i = 0; i < meshes.size(); i++ )
	{
		float dist = GetMeshByName( meshes[i] )->IntersectPolys( pFrom, pTo, mTransformation );
		distances.push_back( dist );
	}

	// closest intersection
	for( UINT i = 0; i < distances.size(); i++ )
	{
		if( distances[i] != -1.0f && distances[i] < temp )
		{
			temp	= distances[i];
			idx		= i;
		}
	}

	/*****************************/
	/*		intersect nulls		 */
	/*****************************/
	CMesh* pNullsMesh = GetMeshByName( "Nulls" );

	if( pNullsMesh && pNullsMesh->GetAlternateNullIcon() )
	{
		CEfnCharacter*	pAnimIcon;
		CEfnMeshlist*	pMlistIcon;
		CMesh*			pIconMesh;
		CBaseScene*		pIcon = pNullsMesh->GetAlternateNullIcon();

		if( pIcon->GetType() == FT_EFN )
		{
			pAnimIcon = (CEfnCharacter*)pIcon;
		}
		else if( pIcon->GetType() == FT_EFN_MESHLIST )
		{
			pMlistIcon	= (CEfnMeshlist*)pIcon;
			pIconMesh	= pMlistIcon->GetMeshByIndex( 0 );
		}

		// oobb and bsphere intersections
		for( int i = 0; i < pNullsMesh->GetNullsCount(); i++ )
		{
			CNULL*		pNull = pNullsMesh->GetNullByIndex( i );
			D3DXMATRIX	mNullTransformation, transl, rotX, rotY, rotZ, scale;

			float** matrices = pNull->matrices;

			D3DXMatrixTranslation( &transl, matrices[0][0], matrices[0][1], matrices[0][2] );

			D3DXMatrixRotationX( &rotX,	D3DXToRadian( matrices[0][3] ) );
			D3DXMatrixRotationY( &rotY,	D3DXToRadian( matrices[0][4] ) );
			D3DXMatrixRotationZ( &rotZ,	D3DXToRadian( matrices[0][5] ) );

			float icon_scale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();

			D3DXMatrixScaling( &scale, icon_scale, icon_scale, icon_scale );

			mNullTransformation = scale * rotX * rotY * rotZ * transl * mTransformation;

			if( pNull )
			{
				if( pIcon->GetType() == FT_EFN )
				{
					if( pAnimIcon->IntersectOOBB( pFrom, pTo, mNullTransformation ) && pAnimIcon->IntersectBSphere( pFrom, pTo, mNullTransformation ) )
						nulls.push_back( pNull->id.data() );
				}
				else if( pIcon->GetType() == FT_EFN_MESHLIST )
				{
					if( pIconMesh->IntersectOOBB( pFrom, pTo, mNullTransformation ) && pIconMesh->IntersectBSphere( pFrom, pTo, mNullTransformation ) )
						nulls.push_back( pNull->id.data() );
				}
				else
				{}
			}
		}

		// poly intersections
		for( unsigned int i = 0; i < nulls.size(); i++ )
		{
			CNULL*		pNull = pNullsMesh->GetNullByName( nulls[i] );
			D3DXMATRIX	mNullTransformation, transl, rotX, rotY, rotZ, scale;

			float** matrices = pNull->matrices;

			D3DXMatrixTranslation( &transl, matrices[0][0], matrices[0][1], matrices[0][2] );

			D3DXMatrixRotationX( &rotX,	D3DXToRadian( matrices[0][3] ) );
			D3DXMatrixRotationY( &rotY,	D3DXToRadian( matrices[0][4] ) );
			D3DXMatrixRotationZ( &rotZ,	D3DXToRadian( matrices[0][5] ) );

			float icon_scale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();

			D3DXMatrixScaling( &scale, icon_scale, icon_scale, icon_scale );

			mNullTransformation = scale * rotX * rotY * rotZ * transl * mTransformation;

			if( pNull )
			{
				if( pIcon->GetType() == FT_EFN )
				{
					float dist = pAnimIcon->IntersectPolys( pFrom, pTo, mNullTransformation );
					distances_nl.push_back( dist );
				}
				else if( pIcon->GetType() == FT_EFN_MESHLIST )
				{
					float dist = pIconMesh->IntersectPolys( pFrom, pTo, mNullTransformation );
					distances_nl.push_back( dist );
				}
				else
				{}
			}
		}

		// closest intersection
		for( unsigned int i = 0; i < distances_nl.size(); i++ )
		{
			if( distances_nl[i] != -1.0f && distances_nl[i] < temp_nl )
			{
				temp_nl	= distances_nl[i];
				idx_nl	= i;
			}
		}
	}

	//get closest intersection
	bool mesh_isected = false;
	bool null_isected = false;

	if( meshes.size() > 0 && idx != -1 )
		mesh_isected = true;

	if( nulls.size() > 0 && idx_nl != -1 )
		null_isected = true;

	if( mesh_isected && null_isected )
	{
		if( temp < temp_nl )
		{
			strObjID = meshes[idx];
			objType = OBJ_MESH;
			return temp;
		}
		else
		{
			strObjID = nulls[idx_nl];
			objType = OBJ_NULL;
			return temp_nl;
		}
	}
	else if( mesh_isected )
	{
		strObjID = meshes[idx];
		objType = OBJ_MESH;
		return temp;
	}
	else if( null_isected )
	{
		strObjID = nulls[idx_nl];
		objType = OBJ_NULL;
		return temp_nl;
	}
	else
	{
		strObjID = "";
		objType = OBJ_UNDEFINED;
		return -1.0f;
	}
}

CMesh* CEfnMeshlist::AddMesh( str_t strName )
{
	CMesh* mesh = new CMesh;

	mesh->SetName( strName );

	m_vecMeshes.push_back( mesh );

	return mesh;
}

void CEfnMeshlist::SetNullIcon( CBaseScene* pBaseScene )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		m_vecMeshes[i]->SetNullIcon( pBaseScene );
	}
}

bool CEfnMeshlist::IsParentMeshSet()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsParentMesh() )
			return true;
	}

	return false;
}

void CEfnMeshlist::SetParentMesh( str_t meshID )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		m_vecMeshes[i]->SetParentMesh( false );
			
	GetMeshByName( meshID )->SetParentMesh( true );
}

void CEfnMeshlist::MakeSureParentIsSet()
{
	if( m_vecMeshes.size() <= 0 )
		return;

	if( IsParentMeshSet() == false )
		m_vecMeshes[0]->SetParentMesh( true );
}

void CEfnMeshlist::MoveNull( str_t strSrcMesh, str_t strSrcNull, str_t strDestMesh )
{
	CNULL* nl = GetMeshByName( strSrcMesh )->GetNullByName( strSrcNull );
	GetMeshByName( strDestMesh )->AddNull( nl );
	GetMeshByName( strSrcMesh )->RemoveNull( nl );
}

bool CEfnMeshlist::InitFromXSICharacter( CXSIFile* xsi_file )
{
	if( !xsi_file )
		return false;

	// count visible meshes
	for( int i = 0; i < xsi_file->GetMeshesCount(); i++ )
	{
		if( xsi_file->GetMeshByIndex( i )->IsVisible() == false )
			continue;

		CMesh* mesh = new CMesh;
		mesh->InitFromXSIMesh( xsi_file->GetMeshByIndex( i ) );
		m_vecMeshes.push_back( mesh );
	}

	CMesh* mesh = new CMesh;
	mesh->SetName( "Nulls" );
	mesh->InitNullsFromXSICharacter( xsi_file );
	m_vecMeshes.push_back( mesh );

	m_strSceneTag = "";

	return true;
}

void CEfnMeshlist::Destroy()
{
	for( int i = 0; i < GetMeshByName( "Nulls" )->GetNullsCount(); i++ )
		SAFE_DELETE( GetMeshByName( "Nulls" )->GetNullByIndex( i )->attachment );

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		SAFE_DELETE( m_vecMeshes[i] );

	m_vecMeshes.clear();
	
	SAFE_RELEASE( m_pTexture );
}

bool CEfnMeshlist::SaveToFile( str_t Path, bool bSilent )
{
	if( Path == "" || m_vecMeshes.size() == 0 )
	{
		MessageBox( NULL, "Couldn't save meshlist file!", "Ошибка!", MB_OK );
		return false;
	}

	if( bSilent == false )
	{
		if( m_dwFileVersion != EFN_MESHLIST_VERSION_02 )
		{
			str_t curr_ver	= GetEfnMeshlistVersionAsString( EFN_MESHLIST_VERSION_02 );
			str_t file_ver	= GetEfnMeshlistVersionAsString( m_dwFileVersion );
			str_t err_msg	= "Файл имеет версию " + file_ver + ", он может быть сохранён только в\n\r версию " + curr_ver + ". Сохранить файл?";
		
			if( MessageBox( NULL, err_msg.data(), "Внимание!", MB_YESNO ) != IDYES )
				return false;
		}
	}

	streampos pos;

	ofstream file( Path.data(), ios::binary );
	file.seekp( pos );

	//store efn file version
	DWORD efn_file_version = EFN_MESHLIST_VERSION_02;
	file.write( (char *)&efn_file_version, sizeof(DWORD) );

	//store efn filetype bit
	int et = efn_type;
	file.write( (char *)&et, sizeof(int) );

	WR2Strm( file, m_strSceneTag );
	WR2Strm( file, (int)m_vecMeshes.size() );

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( !m_vecMeshes[i]->SaveToFile( file ) )
		{
			MessageBox( NULL, "Ошибка сохрания файла", "Ошибка!", MB_OK );
			file.close();
			return false;
		}
	}

	file.close();

	return true;
}

void CEfnMeshlist::EnableMaterials( bool bValue )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		m_vecMeshes[i]->EnableMaterials( bValue );
}

bool CEfnMeshlist::IsSupportedVersion( DWORD dwVersion )
{
	if( dwVersion == EFN_MESHLIST_VERSION_01 || dwVersion == EFN_MESHLIST_VERSION_02 )
		return true;

	return false;
}

int CEfnMeshlist::GetTotalPolycount()
{
	int res = 0;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		res += m_vecMeshes[i]->GetIndicesCount() / 3;

	return res;
}

bool CEfnMeshlist::IsVertexEditingMode()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsVertexEditingMode() )
			return true;
	}

	return false;
}

void CEfnMeshlist::TranslateWholeSceneHack( D3DXVECTOR3* pTranslate )
{
	for( int i = 0; i < GetMeshesCount(); i++ )
	{
		GetMeshByIndex(i)->TranslateWholeSceneHack( pTranslate );
	}
}

bool CEfnMeshlist::LoadFromFile( str_t Path )
{
	streampos pos;

	m_dwFileVersion = GetEfnMeshlistVersion( Path );

	if( IsSupportedVersion( m_dwFileVersion ) == false )
	{
		CString err_str;
		//str_t curr_vers_str		= GetEfnVersionAsString( curr_version );
		//str_t file_vers_str		= GetEfnVersionAsString( file_version );

		err_str.Format( "File can not be opened. Unknown Format." );

		MessageBox( NULL, err_str, "Warning!", MB_OK | MB_ICONEXCLAMATION );

		return false;
	}

	if( !FileExists( Path ) )
		return false;


	ifstream file( Path.data(), ios::binary );
	file.seekg( pos );

	/************************************ version ****************************************/
	DWORD vers;
	file.read( (char *)&vers, sizeof(DWORD) );
	/********************************END* version **************************************/

	/************************************ type ********************************************/
	int et;
	file.read( (char *)&et, sizeof(int) );
	/********************************END* type *****************************************/

	ReadStrm( file, &m_strSceneTag );

	int iMeshesCount = 0;

	ReadStrm( file, iMeshesCount );

	for( int i = 0; i < iMeshesCount; i++ )
	{
		CMesh* mesh = new CMesh;
		mesh->LoadFromFile( file );

		if( m_dwFileVersion == EFN_MESHLIST_VERSION_02 )
		{
			mesh->LoadMaterialFromFile( file );
		}
		else if( m_dwFileVersion == EFN_MESHLIST_VERSION_01 )
		{
			mesh->InitMaterialFromVColor();
		}
		else
		{}

		m_vecMeshes.push_back( mesh );
	}

	file.close();

	return true;
}

void CEfnMeshlist::AssignMaterialToSelectedMeshes( D3DMATERIAL9 mat )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() )
			m_vecMeshes[i]->SetMaterial( mat ); 
	}
}

void CEfnMeshlist::FlipNormalsInSelectedMeshes()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() )
			m_vecMeshes[i]->InverseNormals();
	}
}

void CEfnMeshlist::ConnectTrisInSelectedMeshes()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() )
			m_vecMeshes[i]->ConnectPolys();
	}
}

void CEfnMeshlist::DisconnectTrisInSelectedMeshes()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() )
			m_vecMeshes[i]->DisconnectPolys();
	}
}

CMesh* CEfnMeshlist::GetMeshByName( str_t strName )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetName() == strName )
			return m_vecMeshes[i];
	}

	return NULL;
}

CMesh* CEfnMeshlist::GetMeshByName2( str_t strName )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		str_t temp = m_vecMeshes[i]->GetName();

		if( _2Upper( temp ) == _2Upper( strName ) )
			return m_vecMeshes[i];
	}

	return NULL;
}

void CEfnMeshlist::SelectMesh( str_t strMeshName )
{
	CMesh* pMesh = NULL;

	pMesh = GetMeshByName( strMeshName );

	if( !pMesh )
		return;

	pMesh->SetSelected( true );
}

void CEfnMeshlist::DeselectMesh( str_t strMeshName )
{
	CMesh* pMesh = NULL;

	pMesh = GetMeshByName( strMeshName );

	if( !pMesh )
		return;

	pMesh->SetSelected( false );
}

void CEfnMeshlist::GetSelectedMeshesOOBB( D3DXVECTOR3* vMin, D3DXVECTOR3* vMax )
{
	*vMin = D3DXVECTOR3( 9999999, 9999999, 9999999 );
	*vMax = D3DXVECTOR3( -9999999, -9999999, -9999999 );

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		CMesh* pMesh = m_vecMeshes[i];

		if( pMesh->IsSelected() == false )
			continue;
		
		D3DXVECTOR3* pOOBB = pMesh->GetOOBB();

		if( pOOBB[1].x > vMax->x )
			vMax->x = pOOBB[1].x;

		if( pOOBB[1].y > vMax->y )
			vMax->y = pOOBB[1].y;

		if( pOOBB[1].z > vMax->z )
			vMax->z = pOOBB[1].z;

		if( pOOBB[7].x < vMin->x )
			vMin->x = pOOBB[7].x;

		if( pOOBB[7].y < vMin->y )
			vMin->y = pOOBB[7].y;

		if( pOOBB[7].z < vMin->z )
			vMin->z = pOOBB[7].z;
	}
}

D3DXVECTOR3 CEfnMeshlist::GetSelectedMeshesOOBBCenter()
{
	D3DXVECTOR3 res, min, max;

	GetSelectedMeshesOOBB( &min, &max );

	res = max - min;
	res /= 2.0f;
	res += min;

	return res;
}

CSPHERE CEfnMeshlist::GetSelectedMeshesBSphere()
{
	CSPHERE sphere;

	if( GetSelectedMeshesCount() == 1 )
		return *GetSelectedMesh()->GetBSphere();

	D3DXVECTOR3 res, min, max;

	GetSelectedMeshesOOBB( &min, &max );

	res = max - min;
	res /= 2.0f;

	sphere.pos		= GetSelectedMeshesOOBBCenter();
	sphere.radius	= D3DXVec3Length( &res );

	return sphere;
}

void CEfnMeshlist::DeselectAllMeshes()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetName() != "Nulls" )
			m_vecMeshes[i]->SetSelected( false );
	}
}

int CEfnMeshlist::GetSelectedMeshesCount()
{
	int res = 0;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() )
			res++;
	}
	
	return res;
}

void CEfnMeshlist::DeleteAllMeshes()
{
	int i = 0;

	while( m_vecMeshes.size() > 1 )
	{
		if( m_vecMeshes[0]->GetName() == "Nulls" && i == 0 )
			i++;

		SAFE_DELETE( m_vecMeshes[i] );
		m_vecMeshes.erase( m_vecMeshes.begin() + i );
	}
}

void CEfnMeshlist::DeleteAllNulls()
{
	CMesh* pMesh = GetMeshByName( "Nulls" );

	if( !pMesh )
		return;

	pMesh->KillNulls();
}

void CEfnMeshlist::DeleteSelectedMeshes()
{
	vector<str_t> sel;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() && m_vecMeshes[i]->GetName() != "Nulls" )
		{
			SAFE_DELETE( m_vecMeshes[i] );
			m_vecMeshes.erase( m_vecMeshes.begin() + i );
			i = 0;
		}
	}
}

void CEfnMeshlist::Reset(LPDIRECT3DDEVICE9 pd3dDevice)
{

}

bool CEfnMeshlist::Prepare( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_vecMeshes.size() < 1 )
		return false;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		m_vecMeshes[i]->Prepare( pd3dDevice );

	//D3DXCreateTextureFromFile( pd3dDevice, "c:\\debug_tex.jpg", &m_pTexture );

	return true;
}

void CEfnMeshlist::SetSelectedNull( str_t strMeshID, str_t strNullID )
{
	DeselectAllMeshes();
	SelectMesh( strMeshID );

	DeselectAllNulls();

	m_bAllNullsSelected = true;

	// select needed null
	GetMeshByName( strMeshID )->SetSelectedNull( strNullID );
}

CNULL* CEfnMeshlist::GetSelectedNull()
{
	return GetMeshByName( "Nulls" )->GetSelectedNull();
}

CMesh* CEfnMeshlist::GetParentMesh()
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsParentMesh() )
			return m_vecMeshes[i];
	}

	return NULL;
}

void CEfnMeshlist::MergeSelectedMeshes( str_t* strPar )
{
	StrVec	chiln;
	str_t	par;
	CMesh*	pMesh = NULL;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() && !m_vecMeshes[i]->IsParentMesh() )
			chiln.push_back( m_vecMeshes[i]->GetName() );
		else if( m_vecMeshes[i]->IsSelected() && m_vecMeshes[i]->IsParentMesh() )
			par = m_vecMeshes[i]->GetName();
		else
		{}
	}

	pMesh = GetMeshByName( par );

	if( !pMesh )
	{
		if( chiln.size() > 1 )
		{
			par = chiln[0];
			pMesh = GetMeshByName( par );
			chiln.erase( chiln.begin() );
		}

		if( !pMesh )
			return;
	}

	*strPar = par;

	pMesh->KillReverseIndex();

	for( UINT i = 0; i < chiln.size(); i++ )
		pMesh->MergeWithCMesh( GetMeshByName( chiln[i] ) );

	for( UINT i = 0; i < chiln.size(); i++ )
		DeleteMesh( chiln[i] );

	pMesh->RebuildEditBuffers();
	pMesh->CalcReverseIndex();	
	pMesh->PrecalculateNormals();

	//pMesh->ConnectPolys();

	pMesh = GetMeshByName( par );
	pMesh->CalcOOBB();
	pMesh->CalcBoundingSphere();
}

void CEfnMeshlist::Scale( D3DXMATRIX matx_scale )
{
	D3DXVECTOR3 tmp;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		for( int j = 0; j < m_vecMeshes[i]->GetVerteciesCount(); j++ )
		{
			CVERTEX* pVert = m_vecMeshes[i]->GetVertecies();
			tmp = D3DXVECTOR3( pVert[j].x, pVert[j].y, pVert[j].z );

			D3DXVec3TransformCoord( &tmp, &tmp, &matx_scale );
			
			pVert[j].x = tmp.x;
			pVert[j].y = tmp.y;
			pVert[j].z = tmp.z;
		}

		for( int j = 0; j < m_vecMeshes[i]->GetNullsCount(); j++ )
		{
			CNULL* pNull = m_vecMeshes[i]->GetNullByIndex(j);
			tmp = D3DXVECTOR3( pNull->matrices[0][0], pNull->matrices[0][1], pNull->matrices[0][2] );

			D3DXVec3TransformCoord( &tmp, &tmp, &matx_scale );
			
			pNull->matrices[0][0] = tmp.x;
			pNull->matrices[0][1] = tmp.y;
			pNull->matrices[0][2] = tmp.z;
		}
		
		m_vecMeshes[i]->CalcOOBB();
		m_vecMeshes[i]->CalcBoundingSphere();
		m_vecMeshes[i]->PrecalculateNormals();
	}
}

void CEfnMeshlist::DeselectAllNulls()
{
	// deselect nulls in all meshes
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		m_vecMeshes[i]->DeselectAllNulls();

	if( GetMeshByName( "Nulls" ) )
		GetMeshByName( "Nulls" )->SetSelected( false );

	m_bAllNullsSelected = false;
}

CMesh* CEfnMeshlist::GetSelectedMesh()
{ 
	if( GetSelectedMeshesCount() != 1 )
		return NULL; 
	
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->IsSelected() && m_vecMeshes[i]->GetName() != "Nulls" )
			return m_vecMeshes[i];
	}
};

void CEfnMeshlist::SelectAllMeshes()
{
	//CMesh* pMesh = GetMeshByName( "Nulls" );

	//if( !pMesh )
	//	return;

	//pMesh->SetSelected( false );

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetName() != "Nulls" )
			m_vecMeshes[i]->SetSelected( true );
	}
}

void CEfnMeshlist::Render( LPDIRECT3DDEVICE9 pd3dDevice, bool bUseMaterial )
{
	if( m_vecMeshes.size() < 1 )
		return;

	float fIconScale = 1.0f;

	if( m_pParentWnd )
		fIconScale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();


	//pd3dDevice->SetTexture( 0, m_pTexture );

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetVisible() && m_vecMeshes[i]->GetMaterial().Diffuse.a == 1.0f )
			m_vecMeshes[i]->Render( pd3dDevice, fIconScale, bUseMaterial );

		if( m_bRenderNormals )
			m_vecMeshes[i]->RenderNormals( pd3dDevice );
	}

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetVisible() && m_vecMeshes[i]->GetMaterial().Diffuse.a != 1.0f )
			m_vecMeshes[i]->Render( pd3dDevice, fIconScale, bUseMaterial );

		if( m_bRenderNormals )
			m_vecMeshes[i]->RenderNormals( pd3dDevice );
	}
}

void CEfnMeshlist::ShowAllMeshes( bool bShow )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetName() != "Nulls" )
			m_vecMeshes[i]->SetVisible( bShow );
	}
}

void CEfnMeshlist::ShowAllNulls( bool bShow )
{
	GetMeshByName( "Nulls" )->SetVisible( bShow );
}

void CEfnMeshlist::ShowMeshes( str_t strName, bool bShow )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetName() == strName )
			m_vecMeshes[i]->SetVisible( bShow );
	}
}

void CEfnMeshlist::ShowOOBBs( bool bShow )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		m_vecMeshes[i]->SetOOBBVisible( bShow );
}

bool CEfnMeshlist::GetOOBBVisible()
{
	bool visible = false;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if(	m_vecMeshes[i]->GetOOBBVisible() )
		{
			visible = true;
			break;
		}
	}

	return visible;
}

void CEfnMeshlist::ShowBSpheres( bool bShow )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
		m_vecMeshes[i]->SetBSphereVisible( bShow );
}

bool CEfnMeshlist::GetBSpheresVisible()
{
	bool visible = false;

	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if(	m_vecMeshes[i]->GetBSphereVisible() )
		{
			visible = true;
			break;
		}
	}

	return visible;
}

void CEfnMeshlist::DeleteMesh( str_t strName )
{
	for( UINT i = 0; i < m_vecMeshes.size(); i++ )
	{
		if( m_vecMeshes[i]->GetName() == strName )
		{
			SAFE_DELETE( m_vecMeshes[i] );
			m_vecMeshes.erase( m_vecMeshes.begin() + i );
			break;
		}
	}
}

void CEfnMeshlist::RotateXBy90()
{
	D3DXVECTOR3 tmp;
	D3DXMATRIX rot;

	D3DXMatrixRotationX( &rot, D3DX_PI / 2.0f );

	for( int i = 0; i < GetMeshesCount(); i++ )
	{
		CMesh* pMesh = GetMeshByIndex( i );

		for( int j = 0; j < pMesh->GetVerteciesCount(); j++ )
		{
			D3DXVECTOR3 vecTemp;

			vecTemp.x = pMesh->GetVertecies()[j].x;
			vecTemp.y = pMesh->GetVertecies()[j].y;
			vecTemp.z = pMesh->GetVertecies()[j].z;

			D3DXVec3TransformCoord( &vecTemp, &vecTemp, &rot );

			pMesh->GetVertecies()[j].x = vecTemp.x;
			pMesh->GetVertecies()[j].y = vecTemp.y;
			pMesh->GetVertecies()[j].z = vecTemp.z;

			vecTemp.x = pMesh->GetVertecies()[j].nx;
			vecTemp.y = pMesh->GetVertecies()[j].ny;
			vecTemp.z = pMesh->GetVertecies()[j].nz;

			D3DXVec3TransformCoord( &vecTemp, &vecTemp, &rot );

			pMesh->GetVertecies()[j].nx = vecTemp.x;
			pMesh->GetVertecies()[j].ny = vecTemp.y;
			pMesh->GetVertecies()[j].nz = vecTemp.z;
		}
		
		//recalc aabbs and bspheres
		pMesh->CalcOOBB();
		pMesh->CalcBoundingSphere();
	}
}

void CEfnMeshlist::RotateYBy90()
{
	D3DXVECTOR3 tmp;
	D3DXMATRIX rot;

	D3DXMatrixRotationY( &rot, D3DX_PI / 2.0f );

	for( int i = 0; i < GetMeshesCount(); i++ )
	{
		CMesh* pMesh = GetMeshByIndex( i );

		for( int j = 0; j < pMesh->GetVerteciesCount(); j++ )
		{
			D3DXVECTOR3 vecTemp;

			vecTemp.x = pMesh->GetVertecies()[j].x;
			vecTemp.y = pMesh->GetVertecies()[j].y;
			vecTemp.z = pMesh->GetVertecies()[j].z;

			D3DXVec3TransformCoord( &vecTemp, &vecTemp, &rot );

			pMesh->GetVertecies()[j].x = vecTemp.x;
			pMesh->GetVertecies()[j].y = vecTemp.y;
			pMesh->GetVertecies()[j].z = vecTemp.z;

			vecTemp.x = pMesh->GetVertecies()[j].nx;
			vecTemp.y = pMesh->GetVertecies()[j].ny;
			vecTemp.z = pMesh->GetVertecies()[j].nz;

			D3DXVec3TransformCoord( &vecTemp, &vecTemp, &rot );

			pMesh->GetVertecies()[j].nx = vecTemp.x;
			pMesh->GetVertecies()[j].ny = vecTemp.y;
			pMesh->GetVertecies()[j].nz = vecTemp.z;
		}
		
		//recalc aabbs and bspheres
		pMesh->CalcOOBB();
		pMesh->CalcBoundingSphere();
	}
}

void CEfnMeshlist::RotateZBy90()
{
	D3DXVECTOR3 tmp;
	D3DXMATRIX rot;

	D3DXMatrixRotationZ( &rot, D3DX_PI / 2.0f );

	for( int i = 0; i < GetMeshesCount(); i++ )
	{
		CMesh* pMesh = GetMeshByIndex( i );

		for( int j = 0; j < pMesh->GetVerteciesCount(); j++ )
		{
			D3DXVECTOR3 vecTemp;

			vecTemp.x = pMesh->GetVertecies()[j].x;
			vecTemp.y = pMesh->GetVertecies()[j].y;
			vecTemp.z = pMesh->GetVertecies()[j].z;

			D3DXVec3TransformCoord( &vecTemp, &vecTemp, &rot );

			pMesh->GetVertecies()[j].x = vecTemp.x;
			pMesh->GetVertecies()[j].y = vecTemp.y;
			pMesh->GetVertecies()[j].z = vecTemp.z;

			vecTemp.x = pMesh->GetVertecies()[j].nx;
			vecTemp.y = pMesh->GetVertecies()[j].ny;
			vecTemp.z = pMesh->GetVertecies()[j].nz;

			D3DXVec3TransformCoord( &vecTemp, &vecTemp, &rot );

			pMesh->GetVertecies()[j].nx = vecTemp.x;
			pMesh->GetVertecies()[j].ny = vecTemp.y;
			pMesh->GetVertecies()[j].nz = vecTemp.z;
		}
		
		//recalc aabbs and bspheres
		pMesh->CalcOOBB();
		pMesh->CalcBoundingSphere();
	}
}