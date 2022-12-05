#include "sys/stdafx.h"

#include "XSIFile.h"
#include "EfnCharacter.h"
#include "ExeptionLogger.h"
#include "dialogs/CookieMachineDlg.h"

CXSIFile::CXSIFile( CWnd* pParent = NULL, CWnd* pProgress = NULL )
{
	m_pParent		= pParent;
	m_pDlgProgress	= pProgress;
	Initialize();
}

void CXSIFile::Init( int iMeshesNum )
{
	m_objMeshes		= new CXSIMesh[iMeshesNum];
	m_iMeshesNum	= iMeshesNum;

	for( int i = 0; i < iMeshesNum; i++ )
		m_objMeshes[i].SetParent(this);
}

void CXSIFile::SetLightData(CLIGHT light, int index)
{
	m_vecLights[index] = light;
}

CCUSTPARAM* CXSIFile::GetCustParamByName( str_t strName )
{
	for( UINT i = 0; i < m_vecCustParams.size(); i++ )
	{
		if( m_vecCustParams[i].id == strName )
			return &m_vecCustParams[i];
	}

	return NULL; 
}

CNULL2* CXSIFile::GetNullByIndex( int index )	
{ 
	if( index < (int)m_vecNulls.size() )			
	{
		return &m_vecNulls[index];		
	}

	return NULL;
};

CNULL2* CXSIFile::GetNullByName( str_t strNullID )
{
	for( UINT i = 0; i < m_vecNulls.size(); i++ )
	{
		if( m_vecNulls[i].id == strNullID )			
		{
			return &m_vecNulls[i];		
		}
	}

	return NULL;
}

CXSIMesh* CXSIFile::GetMeshByIndex(int index)
{
	if( index >= 0 && index < m_iMeshesNum )
	{
		if( m_objMeshes != NULL )
			return &m_objMeshes[index];
	}

	return NULL;
}

void CXSIFile::Destroy()
{	
	DeleteBuffers();

	SAFE_DELETE_ARRAY( m_objMeshes );

	m_iPolyNum			= 0;
	m_iVertNum			= 0;
	m_iCurrentFrame		= 0; 
	m_iMeshesNum		= 0;
	m_iFramesNum		= 0;
}

void CXSIFile::Initialize()
{
	m_objMeshes			= NULL;

	m_iMeshesNum		= 0;
	m_iDirectedLightsCount = 0;
	m_iFramesNum		= 0;
	m_iPolyNum			= 0;
	m_iVertNum			= 0;
	m_iCurrentFrame		= 0; 

	m_pVB				= NULL;
	m_pIB				= NULL;
	m_pVBSelection		= NULL;
	m_pVBSelection2		= NULL;
	m_pIBSelection		= NULL;
	m_pVBNull			= NULL;
	m_pVBLines			= NULL;

	m_iSelectedMesh		= -1;
	m_iSelectedLight	= -1;
	m_iSelectedCamera	= -1;
	m_pTexture			= NULL;
	FileType			= FT_XSI;

	m_iProcessedModels	= 0;
	m_iAmountModels		= 0;
	m_bUVExists			= true;
}

bool CXSIFile::IsCustParamAnimated( str_t strCustParamID )
{
	CCUSTPARAM*	par = GetCustParamByName( strCustParamID );

	for( int i = 1; i < m_iFramesNum; i++ )
	{
		if( par->data[0] != par->data[i] )
			return true;
	}

	return false;
}

bool CXSIFile::IsCustParamAnimated( int iIndex )
{
	CCUSTPARAM*	par = GetCustParamByIndex( iIndex );

	for( int i = 1; i < m_iFramesNum; i++ )
	{
		if( par->data[0] != par->data[i] )
			return true;
	}

	return false;
}

int CXSIFile::GetLightIndexByName( str_t light_name)
{
	for( UINT i = 0; i < m_vecLights.size(); i++ )
	{
		if( m_vecLights[i].name == light_name )
			return i;
	}

	return -1;
}

void CXSIFile::SetSelectedLight(str_t mesh_id)
{
	int iSelectedLightIndex = GetLightIndexByName( mesh_id.c_str() );

	if( iSelectedLightIndex != -1 )
		m_iSelectedLight = iSelectedLightIndex;
	else
		m_iSelectedLight = -1;
}

void CXSIFile::SetSelectedCamera( str_t camera_id )
{
	int iSelectedCameraIndex = GetCameraIndexByName( camera_id.c_str() );

	if( iSelectedCameraIndex != -1 )
		m_iSelectedCamera = iSelectedCameraIndex;
	else
		m_iSelectedCamera = -1;
}

int CXSIFile::GetCameraIndexByName( str_t camera_name )
{
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		if( m_vecCameras[i].name == camera_name )
			return i;
	}

	return -1;
}

void CXSIFile::SetSelectedMesh(str_t mesh_id)
{
	int iSelectedMeshIndex = GetMeshIndexByName( mesh_id.c_str() );

	if( iSelectedMeshIndex != -1 )
		m_iSelectedMesh = iSelectedMeshIndex;
	else
		m_iSelectedMesh = -1;
}

CXSIMesh* CXSIFile::GetMeshByName(str_t mesh_name)
{
	for(int i = 0; i < m_iMeshesNum; i++)
	{
		if( strcmp(m_objMeshes[i].GetName(), mesh_name.c_str() ) == 0)
			return &m_objMeshes[i];
	}

	return NULL;
}

int	CXSIFile::GetMeshIndexByName(str_t mesh_name)
{
	for(int i = 0; i < m_iMeshesNum; i++)
	{
		if( strcmp(m_objMeshes[i].GetName(), mesh_name.c_str() ) == 0)
			return i;
	}

	return -1;
}


/////////////////////////////////////////////////////////
// PREPARE SELECTION BUFFERS //
/////////////////////////////////////////////////////////
bool CXSIFile::PrepareSelectedMeshData(LPDIRECT3DDEVICE9 pd3dDevice)
{
	VOID *pVertices, *pIndices;

	// is any mesh selected?
	if( m_iSelectedMesh == -1 )
		return true;
	
	//delete existing selection buffers
	SAFE_RELEASE( m_pIBSelection );

	SAFE_DELETE_ARRAY3D( m_pVBSelection, m_iFramesNum );
	SAFE_DELETE_ARRAY3D( m_pVBSelection2, m_iFramesNum );

	//then build buffers
	/* ========================== Create the index buffer =========================== */
	pIndices = NULL;

	if( FAILED( pd3dDevice->CreateIndexBuffer( sizeof(int) * m_objMeshes[m_iSelectedMesh].GetTrianglesCount() * 3, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIBSelection, 	NULL)) ) 
		return false;

	m_pIBSelection->Lock(0, sizeof(int) * m_objMeshes[m_iSelectedMesh].GetTrianglesCount() * 3, (void**)&pIndices, 0);    //lock buffer

	int *indices = m_objMeshes[m_iSelectedMesh].GetTrianglesList();

	memcpy((int *)pIndices, indices, sizeof(int) * m_objMeshes[m_iSelectedMesh].GetTrianglesCount() * 3);
	m_pIBSelection->Unlock(); //unlock buffer
	/* ======================END= Create the index buffer =========================== */
     


	/* ========================== Create the vertex buffers ========================= */
	m_pVBSelection = new LPDIRECT3DVERTEXBUFFER9[m_iFramesNum];
	m_pVBSelection2 = new LPDIRECT3DVERTEXBUFFER9[m_iFramesNum];

	for( int framesCounter = 0; framesCounter < m_iFramesNum; framesCounter++ )
	{
		// get data amount and pointer
		long sz = sizeof(CVERTEX) * m_objMeshes[m_iSelectedMesh].GetVerteciesCount();
		CVERTEX *vlist = m_objMeshes[m_iSelectedMesh].GetVerteciesList()[framesCounter];

		pVertices = NULL;

		// ---allocate memory--- create temp array
		CVERTEX *vlist_temp = new CVERTEX[m_objMeshes[m_iSelectedMesh].GetVerteciesCount()];




		// create buffer
		if( FAILED( pd3dDevice->CreateVertexBuffer( sz, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBSelection[framesCounter], NULL ) ) ) 
			return false;
	  
		if( FAILED( m_pVBSelection[framesCounter]->Lock( 0, sz, (void**)&pVertices, 0 ) ) )
			return false;

		// copy data
		memcpy(vlist_temp, vlist, sz);

		for( int i = 0; i < m_objMeshes[m_iSelectedMesh].GetVerteciesCount(); i++)
			vlist_temp[i].color = 0x00FF0000;

		memcpy((CVERTEX *)pVertices, vlist_temp, sz);

		//unlock buffer
		m_pVBSelection[framesCounter]->Unlock();





		// create buffer
		if( FAILED( pd3dDevice->CreateVertexBuffer( sz, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBSelection2[framesCounter], NULL ) ) ) 
			return false;
	  
		if( FAILED( m_pVBSelection2[framesCounter]->Lock( 0, sz, (void**)&pVertices, 0 ) ) )
			return false;

		// copy data
		memcpy(vlist_temp, vlist, sz);

		for( int i = 0; i < m_objMeshes[m_iSelectedMesh].GetVerteciesCount(); i++)
			vlist_temp[i].color = 0x00000000;

		memcpy((CVERTEX *)pVertices, vlist_temp, sz);

		//unlock buffer
		m_pVBSelection2[framesCounter]->Unlock();




		// ---free memory---
		SAFE_DELETE_ARRAY( vlist_temp );
	}
	/* ======================END= Create the vertex buffers ========================= */



}

bool CXSIFile::PrepareMeshData(LPDIRECT3DDEVICE9 pd3dDevice)
{
	VOID* pVertices;
	VOID* pIndices;
	int i, j, iTemp = 0, iTemp2 = 0, iNumOfVert, iNumOfTri, offset, iPrevMeshIndex,indexOffset = 0;
	int framesCounter;

	//PREPARE COMPLETE MODEL BUFFERS
	//temp pointers
	CVERTEX**	vertList	= NULL;
	int*		triList		= NULL;
	int*		triListTemp	= NULL;

	//count total vertecies in visible meshes
	for(i = 0; i < m_iMeshesNum; i++)
	{
		if( m_objMeshes[i].IsVisible() )
		{
			iTemp  += m_objMeshes[i].GetVerteciesCount();
			iTemp2 += m_objMeshes[i].GetTrianglesCount();
		}
	}

	//num verts, num polys
	m_iVertNum = iTemp;
	m_iPolyNum = iTemp2;

	//in case any vertices exist, build mesh data
	if (m_iVertNum == 0 || m_iPolyNum == 0)
		return false;

	//index array size
	iTemp2 *= 3;

	m_pVB = new LPDIRECT3DVERTEXBUFFER9[m_iFramesNum];

	for( i = 0; i < m_iFramesNum; i++ )
		m_pVB[i] = NULL;

	/* ========================== Create the vertex buffers ========================= */
	for( framesCounter = 0; framesCounter < m_iFramesNum; framesCounter++ )
	{
		long sz = sizeof(CVERTEX) * m_iVertNum;
		if( FAILED( pd3dDevice->CreateVertexBuffer( sz, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVB[framesCounter], NULL ) ) ) 
			return false;
		
		//reset temp variables
		offset = 0;
		iPrevMeshIndex = -1;

		pVertices = NULL;

		//fill vertex buffer:
		//lock buffer
		if( FAILED( m_pVB[framesCounter]->Lock( 0, sizeof(CVERTEX) * m_iVertNum, (void**)&pVertices, 0 ) ) )
			return false;
		
		//copy data
		for( i = 0; i < m_iMeshesNum; i++ )
		{
			if( m_objMeshes[i].IsVisible() )
			{
				vertList   = m_objMeshes[i].GetVerteciesList();
				iNumOfVert = m_objMeshes[i].GetVerteciesCount(); 

				if (iPrevMeshIndex != -1)
					offset += m_objMeshes[iPrevMeshIndex].GetVerteciesCount();

				memcpy((CVERTEX *)pVertices + offset, vertList[framesCounter], sizeof(CVERTEX) * iNumOfVert);

				iPrevMeshIndex = i;
			}
		}

		//unlock buffer
		m_pVB[framesCounter]->Unlock();
	}
	/* ======================END= Create the vertex buffers ========================= */

	/* ========================== Create the index buffer =========================== */
	if( FAILED( pd3dDevice->CreateIndexBuffer( sizeof(int) * iTemp2, D3DUSAGE_WRITEONLY,	 D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIB,	NULL)) ) 
		return false;

	m_pIB->Lock(0, sizeof(int) * iTemp2, (void**)&pIndices, 0);    //lock buffer

	offset = 0;
	indexOffset = 0;
	iPrevMeshIndex =-1;

	for(i = 0; i < m_iMeshesNum; i++)
	{
		if( m_objMeshes[i].IsVisible() )
		{
			triList    = m_objMeshes[i].GetTrianglesList();
			iNumOfTri  = m_objMeshes[i].GetTrianglesCount() * 3;

			//temporary index buffer
			triListTemp = new int[iNumOfTri];
			for (j = 0; j < iNumOfTri; j++)
				triListTemp[j] = triList[j];

			if (iPrevMeshIndex != -1)
			{
				//calc buffer offset
				offset				+= m_objMeshes[iPrevMeshIndex].GetTrianglesCount() * 3;		
				indexOffset	+= m_objMeshes[iPrevMeshIndex].GetVerteciesCount();

				//move indices
				for(j = 0; j < iNumOfTri; j++)
					triListTemp[j] += indexOffset;
			}

			memcpy((int *)pIndices + offset, triListTemp, sizeof(int) * iNumOfTri);

			SAFE_DELETE_ARRAY( triListTemp );

			iPrevMeshIndex = i;
		}
	}

	m_pIB->Unlock(); //unlock buffer
	/* ======================END= Create the index buffer =========================== */

	/* ========================== Create texture  =============================== */
	CString temp, temp2;

	SAFE_RELEASE( m_pTexture );

	temp		= GetFolder().c_str();
	temp2	= m_szTextureFilename.data();
	temp		= temp + temp2;

	if( temp != "" )
		D3DXCreateTextureFromFile( pd3dDevice, temp, &m_pTexture );
	/* ======================END= Create texture =========================== ===== */

	//clean-up
	vertList	= NULL;
	triList	= NULL;

	SAFE_DELETE_ARRAY( triListTemp );
}


void CXSIFile::SetTexturePath(str_t strPath)
{
	bool slash_found = false;
	int counter = 0;

	for( int i = strPath.length(); i >= 0; i-- )
	{
		counter++;

		if( strPath[i] == '\\' )
		{
			slash_found = true;
			break;
		}
	}

	if( !slash_found )
		m_szTextureFilename = strPath;
	else
	{
		m_szTextureFilename = strPath.substr( strPath.length() - counter + 2,  counter );
	}
}

bool CXSIFile::PrepareNullsData(LPDIRECT3DDEVICE9 pd3dDevice)
{
	VOID*			pVertices;
	CVERTEX*	lines_vertecies	= new CVERTEX[6];

	//prepare lines for drawing Nulls
	lines_vertecies[0].x = -1.0;
	lines_vertecies[0].y =	0.0;
	lines_vertecies[0].z = 0.0;
	lines_vertecies[0].color = 0x0000FF00;

	lines_vertecies[1].x = 1.0;
	lines_vertecies[1].y =	0.0;
	lines_vertecies[1].z = 0.0;
	lines_vertecies[1].color = 0x0000FF00;

	lines_vertecies[2].x = 0.0;
	lines_vertecies[2].y =	-1.0;
	lines_vertecies[2].z = 0.0;
	lines_vertecies[2].color = 0x0000FF00;

	lines_vertecies[3].x = 0.0;
	lines_vertecies[3].y =	1.0;
	lines_vertecies[3].z = 0.0;
	lines_vertecies[3].color = 0x0000FF00;

	lines_vertecies[4].x = 0.0;
	lines_vertecies[4].y =	0.0;
	lines_vertecies[4].z = -1.0;
	lines_vertecies[4].color = 0x0000FF00;

	lines_vertecies[5].x = 0.0;
	lines_vertecies[5].y =	0.0;
	lines_vertecies[5].z = 1.0;
	lines_vertecies[5].color = 0x0000FF00;

	long size = sizeof( CVERTEX ) * 6;

	pd3dDevice->CreateVertexBuffer( size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBNull, NULL );
	m_pVBNull->Lock( 0, size, (void**)&pVertices, 0 );
	memcpy(pVertices, lines_vertecies, size);
	m_pVBNull->Unlock();
	SAFE_DELETE_ARRAY( lines_vertecies );

	return true;
}

////////////////////////////////////////////////////////
// PREPARE LIGHT LINES BUFFER //
////////////////////////////////////////////////////////
bool CXSIFile::PrepareLightData(LPDIRECT3DDEVICE9 pd3dDevice)
{
	VOID*	pVertices;
	int			visible_cams_count = 0;
	int			visible_lights_count = 0;


	//count directed lines
	for( UINT i = 0; i < m_vecLights.size(); i++)
	{
		if( m_vecLights[i].type == LT_SPOT )
			m_iDirectedLightsCount++;
	}

	//count visible lights
	for( UINT i = 0; i < m_vecLights.size(); i++ )
	{
		if( m_vecLights[i].visible && m_vecLights[i].type == LT_SPOT )
			visible_lights_count++;
	}

	//count visible cameras
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		if( m_vecCameras[i].visible )
			visible_cams_count++;
	}

	//prepare lines for drawing [directed and spot] lights
	if( (visible_lights_count + visible_cams_count) > 0 )
	{	
		int					vert_num			= (visible_lights_count + visible_cams_count) * 2;
		long				size					= sizeof(CVERTEX) * vert_num;
		CVERTEX*	lines_vertecies	= new CVERTEX[vert_num];

		int rr = 0;

		if( visible_lights_count > 0 )
		{	
			for( int i = 0; i < visible_lights_count; i++ )
			{
				for( UINT hh = rr; hh < m_vecLights.size(); hh++)
				{
					if( m_vecLights[hh].visible && m_vecLights[hh].type == LT_SPOT )
					{
						rr = hh;
						break;
					}
				}

				CLIGHT		tmp_light	= m_vecLights[rr];
				DWORD	col			= 0x00FFFF00;

				if( rr == m_iSelectedLight)
					col = 0x00FF0000;

				lines_vertecies[i * 2].x = tmp_light.pos.x;
				lines_vertecies[i * 2].y = tmp_light.pos.y;
				lines_vertecies[i * 2].z = tmp_light.pos.z;
				lines_vertecies[i * 2].color = col;

				lines_vertecies[i * 2 + 1].x = tmp_light.targ.x;
				lines_vertecies[i * 2 + 1].y = tmp_light.targ.y;
				lines_vertecies[i * 2 + 1].z = tmp_light.targ.z;
				lines_vertecies[i * 2 + 1].color = col;

				rr++;
			}
		}

		int rt = 0;

		if( m_vecCameras.size() > 0)
		{
			for( int i = visible_lights_count; i < visible_lights_count + visible_cams_count; i++)
			{
				for( UINT hh = rt; hh < m_vecCameras.size(); hh++)
				{
					if( m_vecCameras[hh].visible )
					{
						rt = hh;
						break;
					}
				}

				CCAMERA	tmp_cam	= m_vecCameras[rt];
				DWORD		col			= 0x000000FF;

				if( rt == m_iSelectedCamera)
					col = 0x00FF0000;

				lines_vertecies[i * 2].x				= tmp_cam.pos.x;
				lines_vertecies[i * 2].y				= tmp_cam.pos.y;
				lines_vertecies[i * 2].z				= tmp_cam.pos.z;
				lines_vertecies[i * 2].color		= col;

				lines_vertecies[i * 2 + 1].x		= tmp_cam.dir.x;
				lines_vertecies[i * 2 + 1].y		= tmp_cam.dir.y;
				lines_vertecies[i * 2 + 1].z		= tmp_cam.dir.z;
				lines_vertecies[i * 2 + 1].color	= col;

				rt++;
			}
		}

		pd3dDevice->CreateVertexBuffer( size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBLines, NULL );
		m_pVBLines->Lock( 0, size, (void**)&pVertices, 0 );
		
		memcpy(pVertices, lines_vertecies, size);

		m_pVBLines->Unlock();

		SAFE_DELETE_ARRAY( lines_vertecies );
	}

	m_iVisibleCamsCount				= visible_cams_count;
	m_iVisibleDirectedLightsCount	= visible_lights_count;

	return true;
}

bool CXSIFile::Prepare(LPDIRECT3DDEVICE9 pd3dDevice)
{
	DeleteBuffers();

	PrepareMeshData( pd3dDevice );
	PrepareLightData( pd3dDevice );
	PrepareSelectedMeshData(pd3dDevice);
	PrepareNullsData(pd3dDevice);

	return true;
}

void CXSIFile::InverseMeshNormals(str_t mesh_id)
{
	CXSIMesh *mesh = GetMeshByName( mesh_id );
	mesh->InverseNormals();
}

void CXSIFile::ConvertToLHCoords()
{
	/*
	D3DXMATRIX rotate;
	D3DXMatrixRotationY( &rotate, D3DXToRadian(180) ); 
 
	for(int i = 0; i < m_cdData->vertecies_count; i++)
	{
		m_cdData->vertecies_list[i].z *= -1.0f;

		m_cdData->vertecies_list[i].nx *= -1.0f; 
		m_cdData->vertecies_list[i].ny *= -1.0f; 

		D3DXVECTOR3 ttt;
		ttt.x = m_cdData->vertecies_list[i].nx; 
		ttt.y = m_cdData->vertecies_list[i].ny;
		ttt.z = m_cdData->vertecies_list[i].nz;

		D3DXVec3TransformCoord(&ttt, &ttt, &rotate);

		m_cdData->vertecies_list[i].nx = ttt.x; 
		m_cdData->vertecies_list[i].ny = ttt.y;
		m_cdData->vertecies_list[i].nz = ttt.z;

		ttt.x = m_cdData->vertecies_list[i].x; 
		ttt.y = m_cdData->vertecies_list[i].y;
		ttt.z = m_cdData->vertecies_list[i].z;

		D3DXVec3TransformCoord(&ttt, &ttt, &rotate);

		m_cdData->vertecies_list[i].x = ttt.x; 
		m_cdData->vertecies_list[i].y = ttt.y;
		m_cdData->vertecies_list[i].z = ttt.z;
	}

	int tri_cout = m_cdData->vertex_indices_count / 3;

	for(int i = 0; i < tri_cout; i++)
	{
		int temp;
		temp = m_cdData->vertex_indices_list[i * 3 + 0];
		m_cdData->vertex_indices_list[i * 3 + 0] = m_cdData->vertex_indices_list[i * 3 + 2];
		m_cdData->vertex_indices_list[i * 3 + 2] = temp;
	}
	*/
}

void CXSIFile::RenderSelection(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( m_iSelectedMesh == -1 )
		return;

	if( !m_objMeshes[m_iSelectedMesh].IsVisible() || !pd3dDevice || !m_pVBSelection || !m_pVBSelection2 )
		return;

	DWORD old_mode;
	pd3dDevice->GetRenderState( D3DRS_CULLMODE, &old_mode );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetStreamSource(0, m_pVBSelection[m_iCurrentFrame], 0, sizeof(CVERTEX));
	pd3dDevice->SetFVF(D3DFVF_VERTEX_DESCRIPTOR);
	pd3dDevice->SetIndices(m_pIBSelection);
	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_objMeshes[m_iSelectedMesh].GetVerteciesCount(), 0, m_objMeshes[m_iSelectedMesh].GetTrianglesCount() );

	
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	pd3dDevice->SetStreamSource( 0, m_pVBSelection2[m_iCurrentFrame], 0, sizeof(CVERTEX) );
	pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
	pd3dDevice->SetIndices( m_pIBSelection );
	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_objMeshes[m_iSelectedMesh].GetVerteciesCount(), 0, m_objMeshes[m_iSelectedMesh].GetTrianglesCount() );


	pd3dDevice->SetRenderState( D3DRS_CULLMODE, old_mode );
}

void CXSIFile::SetVColorFromMaterial( bool bValue, LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( bValue )
	{
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
	}
	else
	{
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2 );
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
	}
}

void CXSIFile::Render(LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXMESH ppMesh, D3DXMATRIX mat_grid )
{
	pd3dDevice->SetTexture( 0, NULL );

	DWORD	param1;
	BOOL	param2;
	DWORD	param3;
	DWORD	param4;

	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &param1 );
	pd3dDevice->GetLightEnable( 0, &param2 );
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &param3 );
	pd3dDevice->GetRenderState( D3DRS_CULLMODE, &param4 );

	SetVColorFromMaterial( false, pd3dDevice );

	CBaseScene* pIconCamera	= NULL;
	CBaseScene* pIconLight	= NULL;
	CBaseScene* pIconNull	= NULL;
	
	if( m_pParent )
	{
		pIconCamera	= ((CCookieMachineDlg*)m_pParent)->Get3DIconGlob( ICN_3D_CAMERA );
		pIconLight	= ((CCookieMachineDlg*)m_pParent)->Get3DIconGlob( ICN_3D_LIGHT );
		pIconNull	= ((CCookieMachineDlg*)m_pParent)->Get3DIconGlob( ICN_3D_NULL );
	}

	/* prepare scaling matrices */
	float factor = 1.0f;

	if( !m_pParent )
		factor = 1.0f;
	else
		factor = ((CCookieMachineDlg*)m_pParent)->GetIconsScale();

	///////////////////////////////
	//		render mesh data	 //
	///////////////////////////////

	if( m_iVertNum > 0 && m_iPolyNum > 0 )
	{
		//texturing
		pd3dDevice->SetTexture( 0, m_pTexture );

		//shape
		pd3dDevice->SetStreamSource( 0, m_pVB[m_iCurrentFrame], 0, sizeof( CVERTEX ) );
		pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
		pd3dDevice->SetIndices( m_pIB );
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_iVertNum, 0, m_iPolyNum );
	}


	///////////////////////////////
	//		render nulls         //
	///////////////////////////////
	
	D3DXMATRIX matr_old_world;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->LightEnable( 0, FALSE );		

	// remember shading mode
	DWORD old_val;
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &old_val );

	// set current mode to solid
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );	

	// turn off texture
	pd3dDevice->SetTexture( 0, NULL );

	if( m_vecNulls.size() > 0 && m_pVBNull )
	{
		D3DXMATRIX matr_old_world;
		pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );

		D3DXMATRIX rotX, rotY, rotZ, transl, scale, res;

		pd3dDevice->SetStreamSource( 0, m_pVBNull, 0, sizeof(CVERTEX) );
		pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );

		for( UINT i = 0; i < m_vecNulls.size(); i++ )
		{
			vector<CTRANSFORMEX> matrices = m_vecNulls[i].matrices;

			D3DXMatrixTranslation( &transl, matrices[ m_iCurrentFrame ].pos.x, matrices[ m_iCurrentFrame ].pos.y, matrices[ m_iCurrentFrame ].pos.z );

			D3DXMatrixRotationX( &rotX,	D3DXToRadian( matrices[ m_iCurrentFrame ].rot.x ) );
			D3DXMatrixRotationY( &rotY,	D3DXToRadian( matrices[ m_iCurrentFrame ].rot.y ) );
			D3DXMatrixRotationZ( &rotZ,	D3DXToRadian( matrices[ m_iCurrentFrame ].rot.z ) );

			D3DXMatrixScaling( &scale, factor * matrices[ m_iCurrentFrame ].scl.x, factor * matrices[ m_iCurrentFrame ].scl.y, factor * matrices[ m_iCurrentFrame ].scl.z );

			res = scale * rotX * rotY * rotZ * transl * matrices[ m_iCurrentFrame ].pre_transform;

			pd3dDevice->SetTransform( D3DTS_WORLD, &res );

			pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
		}

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		pd3dDevice->LightEnable( 0, TRUE );

		pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );
	}


	///////////////////////////////
	//		render lines         //
	///////////////////////////////

	if( ( m_vecLights.size() + m_vecCameras.size() ) > 0 && m_pVBLines)
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->LightEnable( 0, TRUE );

		pd3dDevice->SetStreamSource(0, m_pVBLines, 0, sizeof(CVERTEX));
		pd3dDevice->SetFVF(D3DFVF_VERTEX_DESCRIPTOR);
		pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_iVisibleDirectedLightsCount + m_iVisibleCamsCount );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		pd3dDevice->LightEnable( 0, TRUE );
	}
	
	///////////////////////////////
	//		 render light data   //
	///////////////////////////////

	D3DXMATRIX scale, scale_pointer;
	D3DXMatrixScaling( &scale, factor, factor, factor);
	
	factor *= 6;
    
	D3DXMatrixScaling( &scale_pointer, factor, factor, factor );
	/* END prepare scaling matrices */

	if( ppMesh && m_vecLights.size() > 0)
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->LightEnable( 0, TRUE );

		for( UINT i = 0; i < m_vecLights.size(); i++ )
		{
			if( m_vecLights[i].visible )
			{
				D3DXMATRIX matr;

				D3DXMatrixTranslation( &matr, m_vecLights[i].pos.x, m_vecLights[i].pos.y, m_vecLights[i].pos.z );

				matr = scale * matr * mat_grid;

				pd3dDevice->SetTransform( D3DTS_WORLD, &matr );

				D3DXMATRIX ident;
				D3DXMatrixIdentity( &ident );

				if( pIconLight )
					((CEfnCharacter*)pIconLight)->Render( pd3dDevice, false, 0.0 );

				D3DXMatrixTranslation( &matr, m_vecLights[i].targ.x, m_vecLights[i].targ.y, m_vecLights[i].targ.z );

				if( m_vecLights[i].type == LT_SPOT )
				{
					matr = scale_pointer * matr * mat_grid;
					pd3dDevice->SetTransform( D3DTS_WORLD, &matr );
					ppMesh->DrawSubset(0);
				}
			}
		}

		if( m_iSelectedLight != -1 && m_vecLights[m_iSelectedLight].visible )
		{
			D3DXMATRIX matr;
			D3DXMatrixTranslation( &matr, m_vecLights[m_iSelectedLight].pos.x, m_vecLights[m_iSelectedLight].pos.y, m_vecLights[m_iSelectedLight].pos.z );
			matr = scale * matr * mat_grid;
			pd3dDevice->SetTransform( D3DTS_WORLD, &matr );
			
			D3DXMATRIX ident;
			D3DXMatrixIdentity( &ident );

			D3DLIGHT9 light;
			D3DLIGHT9 light_2;
			pd3dDevice->GetLight(0, &light);

			D3DXVECTOR3 vecDir;
			ZeroMemory( &light_2, sizeof(light_2) );
			light_2.Type = D3DLIGHT_DIRECTIONAL;

			light_2.Diffuse.r = 1.0f;
			light_2.Diffuse.g = 0.0f;
			light_2.Diffuse.b = 0.0f;

			light_2.Ambient.r = 1.0f;
			light_2.Ambient.g = 0.2f;
			light_2.Ambient.b = 0.2f;

			vecDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			D3DXVec3Normalize( (D3DXVECTOR3*)&light_2.Direction, &vecDir );
			light_2.Range = 3000.0f;

			//ppMesh->DrawSubset(0);

			pd3dDevice->SetLight( 0, &light_2 );

			pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			pd3dDevice->LightEnable( 0, TRUE );

			pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00FF3333 );

			if( pIconLight )
					((CEfnCharacter*)pIconLight)->Render( pd3dDevice, false, 0.0 );

			pd3dDevice->SetLight( 0, &light);

			pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			pd3dDevice->LightEnable( 0, FALSE );	
		}
	}

	//////////////////////////////////
	//		render cam data			//
	//////////////////////////////////

	if( ppMesh && m_vecCameras.size() > 0 )
	{
		for( UINT i = 0; i < m_vecCameras.size(); i++ )
		{
			if( m_vecCameras[i].visible )
			{
				D3DXMATRIX matr;
				D3DXMatrixTranslation( &matr, m_vecCameras[i].pos.x, m_vecCameras[i].pos.y, m_vecCameras[i].pos.z );
				matr = scale * matr * mat_grid;
				pd3dDevice->SetTransform( D3DTS_WORLD, &matr );
				
				D3DXMATRIX ident;
				D3DXMatrixIdentity( &ident );

				
				if( pIconCamera )
					((CEfnCharacter*)pIconCamera)->Render( pd3dDevice, false, 0.0 );

				D3DXMatrixTranslation( &matr, m_vecCameras[i].dir.x, m_vecCameras[i].dir.y, m_vecCameras[i].dir.z );
				matr = scale_pointer * matr * mat_grid;
				pd3dDevice->SetTransform( D3DTS_WORLD, &matr );
				ppMesh->DrawSubset(0);


				if( m_iSelectedCamera != -1 )
				{
					D3DXMATRIX matr;
					D3DXMatrixTranslation( &matr, m_vecCameras[m_iSelectedCamera].pos.x, m_vecCameras[m_iSelectedCamera].pos.y, m_vecCameras[m_iSelectedCamera].pos.z );
					matr = scale * matr * mat_grid;
					pd3dDevice->SetTransform( D3DTS_WORLD, &matr );
					
					D3DXMATRIX ident;
					D3DXMatrixIdentity( &ident );

					D3DLIGHT9 light;
					D3DLIGHT9 light_2;
					pd3dDevice->GetLight(0, &light);

					D3DXVECTOR3 vecDir;
					ZeroMemory( &light_2, sizeof(light_2) );
					light_2.Type = D3DLIGHT_DIRECTIONAL;

					light_2.Diffuse.r = 1.0f;
					light_2.Diffuse.g = 0.0f;
					light_2.Diffuse.b = 0.0f;

					light_2.Ambient.r = 1.0f;
					light_2.Ambient.g = 0.2f;
					light_2.Ambient.b = 0.2f;

					vecDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
					D3DXVec3Normalize( (D3DXVECTOR3*)&light_2.Direction, &vecDir );
					light_2.Range = 3000.0f;

					//ppMesh->DrawSubset(0);

					pd3dDevice->SetLight( 0, &light_2 );

					pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
					pd3dDevice->LightEnable( 0, TRUE );

					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00FF3333 );

					if( pIconCamera )
						((CEfnCharacter*)pIconCamera)->Render( pd3dDevice, false, 0.0 );

					pd3dDevice->SetLight( 0, &light);

					pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
					pd3dDevice->LightEnable( 0, FALSE );	
				} // if( m_iSelectedCamera != -1 )
			} // if( m_objCameras[i].visible )
		} // for( int i = 0; i < m_iCamerasNum; i++ )

	} //if( ppMesh && m_iCamerasNum > 0)

	pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );

	//set previous mode current
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, old_val );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, param1 );
	pd3dDevice->LightEnable( 0, param2 );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, param3 );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, param4 );
}

void CXSIFile::DeleteBuffers(void)
{
	SAFE_RELEASE( m_pIB );
	SAFE_RELEASE( m_pIBSelection );
	SAFE_RELEASE( m_pTexture );

	SAFE_DELETE_ARRAY3D( m_pVB, m_iFramesNum );
	SAFE_DELETE_ARRAY3D( m_pVBSelection, m_iFramesNum );
	SAFE_DELETE_ARRAY3D( m_pVBSelection2, m_iFramesNum );

	//release light lines
	SAFE_RELEASE( m_pVBLines );
	SAFE_RELEASE( m_pVBNull );
}

void CXSIFile::SetCurrentFrame(int iFrameIndex)
{
	if (iFrameIndex >= 0 && iFrameIndex < m_iFramesNum)
		m_iCurrentFrame = iFrameIndex;
}

bool CXSIFile::LoadFromFile( str_t strDirName, str_t strFileName, bool bRepair )
{
	str_t func_decl_str = "bool CXSIFile::LoadFromFile( str_t strDirName, str_t strFileName, bool bRepair )";

	try
	{
		str_t strPath = strDirName + strFileName;

		m_bUseVertexNormals = true;

		if( bRepair )
			CorrectXSIFile( strPath.data() );

		enum XSI_FILE_STAT xsiFile_stat = Validate( strPath );

		
		if( m_pParent )
			((CCookieMachineDlg*)m_pParent)->ProcessMessagesGlob();

		if( xsiFile_stat == XFS_UNREADABLE )
		{
			MessageBox( NULL, "Can not parse dotXSI file.", "File open error", MB_OK | MB_ICONERROR );
			return false;
		}
		else if( xsiFile_stat == XFS_POLYLIST_DETECTED )
		{
			MessageBox( NULL, "dotXSI file consists non-triangulated polymeshes, \n\rthis feature is not yet supported!", "File open error", MB_OK | MB_ICONERROR );
			//return false;
		}
		else
		{
			if( m_pDlgProgress )
				((CValidateDlg*)m_pDlgProgress)->SetMessageStr("Загрузка файла...");


			/*--- LOAD FILE -------------------------------------------------------------------*/
			int framesCount;

 			m_iProcessedModels = 0;

			m_XsiScene.Read();

			int dd = (int)m_XsiScene.SceneInfo()->GetEnd();
			int ff = (int)m_XsiScene.SceneInfo()->GetStart();

			framesCount = (int)m_XsiScene.SceneInfo()->GetEnd() - (int)m_XsiScene.SceneInfo()->GetStart() + 1;

			int progress_max = CountModels( m_XsiScene.Root() ) + framesCount;

			if( m_pDlgProgress )
				((CValidateDlg*)m_pDlgProgress)->SetProgressRange( 0, progress_max );

			//in case no animation present
			if( framesCount == 0 )
				framesCount = 1;

			m_iXSIFramesOffset = (int)m_XsiScene.SceneInfo()->GetStart();


			if( DetectShapeAnimation( m_XsiScene.Root() ) == false && DetectCurveAnimation( m_XsiScene.Root() ) == false )
				framesCount = 1;

			m_iFramesNum = framesCount;

			int nl_count = CountNulls( m_XsiScene.Root() );

			m_iAmountModels = CountModels( m_XsiScene.Root() );

			Init( m_iAmountModels );

			//recurcively fill models array from XSI data
			StripScene( m_XsiScene.Root() );

			m_XsiScene.Close();

			/*--- LOAD FILE END ---------------------------------------------------------------*/
		}

		return true;
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

//extract light
void CXSIFile::StripLight( CSLModel* pModel )
{
	CSLPointLight*	point	= NULL;
	CSLSpotLight*	spot	= NULL;
	CLIGHT			light;


	/************ GET NAME ***********/
	//remove last char ( which was added by Deep Exploration, see comments below )
	light.name = pModel->Name().GetText();
	light.name = light.name.substr( 0, light.name.length() - 1 );


	/*********** GET LIGHT TYPE ***********/
	if( pModel->GetPrimitiveType() == CSLTemplate::SI_SPOT_LIGHT )
	{
		light.type = LT_SPOT;
		spot = (CSLSpotLight*)pModel->Primitive();
	}

	if( pModel->GetPrimitiveType() == CSLTemplate::SI_POINT_LIGHT )
	{
		light.type = LT_POINT;
		point = (CSLPointLight*)pModel->Primitive();
	}

	/*********** GET POSITION ***********/
	//READ CAREEFULLY!!! this parser uses xsi files created by Deep Exploration converter, which has countless bugs. 
	//the one concerning light sources is - [transformation of position of lightsource] is stored in separate model with name 
	//consisting( name of the lightsource - last character ), [direction of lightsource] is stored in separate model with name
	//consisting( name of the lightsource + string "_Target")
	//for example: SPOT011 - light, SPOT01 - model consisting transformation of position of the light
	//                   SPOT01_Target - model consisting direction of lightsource

	char				nm[256];
	CSLModel*			model_transform = NULL;
	CSIBCVector3D		pos;
	
	if( light.type == LT_SPOT )
		pos = spot->GetPosition();

	if( light.type == LT_POINT )
		pos = point->GetPosition();

	sprintf( nm, "%s", light.name.data() );
	model_transform = pModel->Scene()->FindModel( nm );

	float x = pos.GetX();
	float y = pos.GetY();
	float z = pos.GetZ();

	CalcGlobalTransform( x, y, z, model_transform );

	// convert to LH coordinate system
	D3DXVECTOR3 v_vec( x, y, z );

	if( originator == XO_SOFTIMAGE )
		ConvertVec2LHCoords( &v_vec );
	else
		ConvertVec2LHCoordsDeepEx( &v_vec );

	light.pos = v_vec;

	/*********** GET DIRECTION ***********/
	if( light.type == LT_SPOT )
	{
		if( originator == XO_SOFTIMAGE )
		{
			CSIBCVector3D	inter = spot->GetInterestPosition();
			D3DXVECTOR3		_inter( inter.GetX(), inter.GetY(), inter.GetZ() );

			ConvertVec2LHCoords( &_inter );

			light.targ = _inter;
		}
		else
		{
			sprintf( nm, "%s_Target", light.name.data() );
			model_transform = pModel->Scene()->FindModel( nm );

			x = y = z = 0;

			CSIBCVector3D inter = spot->GetInterestPosition();

			CalcGlobalTransform( x, y, z, model_transform );

			D3DXVECTOR3 v_vec2( x, y, z );
			ConvertVec2LHCoordsDeepEx( &v_vec2 );
			light.targ = v_vec2;
		}
	}

	light.visible	= true;
	light.diffuse.r = 255;
	light.diffuse.g = 255;
	light.diffuse.b = 255;

	m_vecLights.push_back( light );
}

void CXSIFile::StripScene( CSLModel* pModel )
{
	str_t func_decl_str = "void CXSIFile::StripScene( CSLModel* pModel )";

	try
	{
		//analyze model
		//-------------strip meshes---------------
		if( pModel->GetPrimitiveType() == CSLTemplate::SI_MESH )
		{
			_ASSERTE( GetMeshByIndex( m_iProcessedModels ) != NULL );

			StripModel( pModel, GetMeshByIndex( m_iProcessedModels ) );

			m_iProcessedModels++;
		}

		//--------------strip lights------------------
		if( pModel->GetPrimitiveType() == CSLTemplate::SI_SPOT_LIGHT || 
			pModel->GetPrimitiveType() == CSLTemplate::SI_POINT_LIGHT || 
			pModel->GetPrimitiveType() == CSLTemplate::SI_DIRECTIONAL_LIGHT )
		{
			StripLight( pModel );
		}

		//--------------strip cameras------------------
		if( pModel->GetPrimitiveType() == CSLTemplate::SI_CAMERA )
		{
			StripCamera( pModel );
		}

		//--------------strip nulls------------------
		if( pModel->GetPrimitiveType() == CSLTemplate::SI_NULL_OBJECT )
		{
			StripNull( pModel );
		}

		CSLModel **l_childrenList = pModel->GetChildrenList();
	 
		// Loop through all children
		for( int i = 0 ; i < pModel->GetChildrenCount(); i++ )
			StripScene( l_childrenList[i] );
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}

}

D3DXMATRIX CXSIFile::GetTransformation( CSLModel* pModel, int iFrame, CTRANSFORMEX* pTransf )
{
	D3DXMATRIX mIdentity, mLocal;
	D3DXMatrixIdentity( &mIdentity );

	if( !pModel )
		return mIdentity;

	CSLTransform*	Transformation = (CSLTransform*)pModel->Transform();
	CTRANSFORM		trLocal;

	if( Transformation )
	{
		CSIBCVector3D Translation( Transformation->GetTranslation() );
		CSIBCVector3D Rotation( Transformation->GetEulerRotation() );
		CSIBCVector3D Scale( Transformation->GetScale() );
		
		CSLBaseFCurve* XRot = Transformation->GetSpecificFCurve( CSLTemplate::SI_ROTATION_X );
		CSLBaseFCurve* YRot = Transformation->GetSpecificFCurve( CSLTemplate::SI_ROTATION_Y );
		CSLBaseFCurve* ZRot	= Transformation->GetSpecificFCurve( CSLTemplate::SI_ROTATION_Z );

		CSLBaseFCurve* XTrans	= Transformation->GetSpecificFCurve( CSLTemplate::SI_TRANSLATION_X );
		CSLBaseFCurve* YTrans	= Transformation->GetSpecificFCurve( CSLTemplate::SI_TRANSLATION_Y );
		CSLBaseFCurve* ZTrans	= Transformation->GetSpecificFCurve( CSLTemplate::SI_TRANSLATION_Z );

		CSLBaseFCurve* XScale	= Transformation->GetSpecificFCurve( CSLTemplate::SI_SCALING_X );
		CSLBaseFCurve* YScale	= Transformation->GetSpecificFCurve( CSLTemplate::SI_SCALING_Y );
		CSLBaseFCurve* ZScale	= Transformation->GetSpecificFCurve( CSLTemplate::SI_SCALING_Z );


		//translation
		if( XTrans )
		{
			XTrans->Evaluate( (float)iFrame );
			trLocal.pos.x = XTrans->GetLastEvaluation();
		}
		else
		{
			trLocal.pos.x = Translation.GetX();
		}

		if( YTrans )
		{
			YTrans->Evaluate( (float)iFrame );
			trLocal.pos.y = YTrans->GetLastEvaluation();
		}
		else
		{
			trLocal.pos.y = Translation.GetY();
		}

		if( ZTrans )
		{
			ZTrans->Evaluate( (float)iFrame );
			trLocal.pos.z = ZTrans->GetLastEvaluation();
		}
		else
		{
			trLocal.pos.z = Translation.GetZ();
		}


		//rotation
		if( XRot )
		{
			XRot->Evaluate( (float)iFrame );
			trLocal.rot.x = XRot->GetLastEvaluation();
		}
		else
		{
			trLocal.rot.x = Rotation.GetX();
		}
		
		if( YRot )
		{
			YRot->Evaluate( (float)iFrame );
			trLocal.rot.y = YRot->GetLastEvaluation();
		}
		else
		{
			trLocal.rot.y = Rotation.GetY();
		}

		if( ZRot )
		{
			ZRot->Evaluate( (float)iFrame );
			trLocal.rot.z = ZRot->GetLastEvaluation();
		}
		else
		{
			trLocal.rot.z = Rotation.GetZ();
		}


		//scale
		if( XScale )
		{
			XScale->Evaluate( (float)iFrame );
			trLocal.scl.x = XScale->GetLastEvaluation();
		}
		else
		{
			trLocal.scl.x = Scale.GetX();
		}
		
		if( YScale )
		{
			YScale->Evaluate( (float)iFrame );
			trLocal.scl.y = YScale->GetLastEvaluation();
		}
		else
		{
			trLocal.scl.y = Scale.GetY();
		}

		if( ZScale )
		{
			ZScale->Evaluate( (float)iFrame );
			trLocal.scl.z = ZScale->GetLastEvaluation();
		}
		else
		{
			trLocal.scl.z = Scale.GetZ();
		}
	}

	if( !pTransf )
	{
		if( trLocal.scl.x == 0.0f )
			trLocal.scl.x = 1.0f;

		if( trLocal.scl.y == 0.0f )
			trLocal.scl.y = 1.0f;

		if( trLocal.scl.z == 0.0f )
			trLocal.scl.z = 1.0f;
	}

	//convert values to LH coords system
	ConvertVec2LHCoords( &trLocal.pos );

	//rotation
	trLocal.rot.y = 360.0f - trLocal.rot.y;
	trLocal.rot.z = 360.0f - trLocal.rot.z;

	D3DXMATRIX tr, rtX, rtY, rtZ, sc;
	
	D3DXMatrixTranslation( &tr, trLocal.pos.x, trLocal.pos.y, trLocal.pos.z ); 
	D3DXMatrixRotationX( &rtX, D3DXToRadian( trLocal.rot.x ) ); 
	D3DXMatrixRotationY( &rtY, D3DXToRadian( trLocal.rot.y ) ); 
	D3DXMatrixRotationZ( &rtZ, D3DXToRadian( trLocal.rot.z ) ); 
	D3DXMatrixScaling( &sc, trLocal.scl.x, trLocal.scl.y, trLocal.scl.z ); 
	mLocal = sc * rtX * rtY * rtZ * tr;

	CSLModel* pParentModel = pModel->Parent();

	if( pTransf )
	{
		if( pParentModel )
		{
			*pTransf = trLocal;
			pTransf->pre_transform = GetTransformation( pParentModel, iFrame );
		}
	}
	else
	{
		if( pParentModel )
			return mLocal * GetTransformation( pParentModel, iFrame );
		else
			return mLocal;
	}

	return mIdentity;
}

//extract null
void CXSIFile::StripNull( CSLModel* pModel )
{
	CNULL2 pNull;

	pNull.id = pModel->Name().GetText();

	for( int iCurrentFrame = m_iXSIFramesOffset; iCurrentFrame < GetFramesCount() + m_iXSIFramesOffset; iCurrentFrame++ )
	{
		CTRANSFORMEX transf;
		GetTransformation( pModel, iCurrentFrame, &transf );

		pNull.matrices.push_back( transf );
	}

	m_vecNulls.push_back( pNull );

	StripCustomParameter( pModel );
}

long CXSIFile::CountTriangles( CSLMesh* pMesh )
{
	int num_triangles = 0;

	for( int index = 0; index < pMesh->GetTriangleListCount(); index++ )
	{
		num_triangles += pMesh->TriangleLists()[index]->GetTriangleCount();
	}

	return num_triangles;
}

long CXSIFile::CountPolygons( CSLMesh* pModel )
{
	int num_polygons = 0;

	for( int index = 0; index < pModel->GetPolygonListCount(); index++ )
	{
		num_polygons += ( pModel->PolygonLists() )[index]->GetPolygonCount();
	}

	return num_polygons;
}

void CXSIFile::StripModel( CSLModel* model, CXSIMesh* mesh )
{
	str_t func_decl_str = "void CXSIFile::StripModel( CSLModel* model, CXSIMesh* mesh )";

	try
	{
		CSLMesh* xsiMesh = (CSLMesh*)model->Primitive();

		CString msg;
		msg.Format( "Models: %i of %i", m_iProcessedModels + 1,  m_iAmountModels );

		//update progress dialog
		if( m_pDlgProgress )
		{
			((CValidateDlg*)m_pDlgProgress)->SetProgressMessageStrA( msg ); 

			msg = "";
			((CValidateDlg*)m_pDlgProgress)->SetProgressMessageStrB( msg ); 


			((CValidateDlg*)m_pDlgProgress)->Step();
		}

		//declarations
		CSLBaseShape::CSLIndexedVector3DArray*	vertex_list		= NULL;
		CSLBaseShape::CSLVector3DArray*			vertex_list2	= NULL;

		float			x, y, z, a, r, g, b;
		CSLTransform*	transformations	= (CSLTransform*)model->Transform();
		CSLModel*		parent			= model->Parent();

		int				num_tri			= CountTriangles(xsiMesh);
		bool			poly_data		= false;

		if( num_tri == 0 )
		{
			num_tri	  = CountPolygons(xsiMesh);
			poly_data = true;
		}

		SI_Char* nm	= model->Name().GetText();

		//in case model has no tris
		if( num_tri <= 0 )
			return;

		int			iTempIndex, j, index, curr_tri_count = 0, curr_poly_count = 0, curr_tri = 0, curr_poly = 0, iRGBCounter = 0;
		SI_Char*	tex_path	= "";
		SI_Int*		vertex_ptr;
		int			num_vertex;
		int			num_shapes = 0;

		CSLTriangleList**		tri_list	= xsiMesh->TriangleLists();
		CSLPolygonList**		poly_list	= xsiMesh->PolygonLists();
		CSLGlobalMaterial*		g_mat		= model->GlobalMaterial();
		CSLVariantParameter*	temp_param	= NULL;
		CSLBaseShape*			pShape		= xsiMesh->Shape();
		CSLShapeAnimation*		animation	= NULL;
		CSLBaseShape**			shapes		= NULL;
		CSLXSIMaterial*			mat			= NULL;

		if( g_mat )
			mat = (CSLXSIMaterial*)g_mat->GetMaterial();

		x = y = z = 0.0f;
		a = r = g = b = 1.0f;

		//get shape animations
		animation = xsiMesh->ShapeAnimation();

		//shape animation present
		if (animation)
		{
			shapes		= animation->Shapes();
			num_shapes	= animation->GetShapeCount();
		}

		//process mesh name
		mesh->SetName( model->Name().GetText() );

		//find diffuse components
		//iterate through all the shaders
		//if( mat && mat->GetShaderCount() > 0 && mat->GetShaderCount() < 10000 )
		//{
		//	for( int i = 0; i < mat->GetShaderCount(); i++ )
		//	{
		//		temp_param = (CSLVariantParameter*)mat->GetShaderList()[i]->ParameterFromName("diffuse.alpha");
		//		(temp_param != NULL)?a = temp_param->GetValue()->fVal:0;

		//		temp_param = (CSLVariantParameter*)mat->GetShaderList()[i]->ParameterFromName("diffuse.red");
		//		(temp_param != NULL)?r = temp_param->GetValue()->fVal:0;

		//		temp_param = (CSLVariantParameter*)mat->GetShaderList()[i]->ParameterFromName("diffuse.green");
		//		(temp_param != NULL)?g = temp_param->GetValue()->fVal:0;

		//		temp_param = (CSLVariantParameter*)mat->GetShaderList()[i]->ParameterFromName("diffuse.blue");
		//		(temp_param != NULL)?b = temp_param->GetValue()->fVal:0;

		//		mesh->SetColor( 1.0f - a, r, g, b );
		//		break;
		//	}
		//}
		//else
		//{
		//	
		//}

		//set default values
		mesh->SetColor( 1.0f, 1.0f, 1.0f, 1.0f );

		//AfxMessageBox( "!" );

		//shape animation present
		if( shapes )   
			num_vertex = shapes[0]->GetVertexCount();
		else
			num_vertex = xsiMesh->Shape()->GetVertexCount();

		//process vertecies data
		mesh->InitVerteciesList( num_vertex );

		for( int i = 0; i < GetFramesCount(); i++ )
		{
			//update progress dialog
			msg.Format( "Frames: %i of %i", i + 1, GetFramesCount() );
			
			if( m_pDlgProgress )
			{
				((CValidateDlg*)m_pDlgProgress)->SetProgressMessageStrB( msg ); 
				((CValidateDlg*)m_pDlgProgress)->Step();
			}

			//shape animation present
			if( shapes )
			{
				try
				{
					vertex_list = shapes[i]->GetIndexedVertexList();
				}
				catch(...)
				{
					g_ExeptLog->__data.push_back( "CSLIndexedVector3DArray* CSLBaseShape::GetIndexedVertexList()" );
					throw;
				}

				for( j = 0; j < num_vertex; j++ )
				{
					(*vertex_list)[j].m_Vector3D.Get(&x, &y, &z);
					iTempIndex = (int)(*vertex_list)[j].m_fIndex;
		
					CalcGlobalTransform( x, y, z, model );

					D3DXVECTOR3 v_vec(x, y, z);

					if( originator == XO_SOFTIMAGE )
						ConvertVec2LHCoords( &v_vec );
					else
						ConvertVec2LHCoordsDeepEx( &v_vec );

					x = v_vec.x;
					y = v_vec.y;
					z = v_vec.z;

					mesh->SetVertexData( i, iTempIndex, x, y, z, D3DCOLOR_COLORVALUE(1.0, 1.0, 1.0, 1.0) );
				}
			}
			else
			{
				vertex_list2 = xsiMesh->Shape()->GetVertexList();

				for( j = 0; j < num_vertex; j++ )
				{
					(*vertex_list2)[j].Get(&x, &y, &z);

					CalcGlobalTransform(x, y, z, model);

					D3DXVECTOR3 v_vec(x, y, z);

					if( originator == XO_SOFTIMAGE )
						ConvertVec2LHCoords( &v_vec );
					else
						ConvertVec2LHCoordsDeepEx( &v_vec );

					x = v_vec.x;
					y = v_vec.y;
					z = v_vec.z;

					mesh->SetVertexData( i, j, x, y, z, D3DCOLOR_COLORVALUE(1.0, 1.0, 1.0, 1.0) );
				}
			}
		}

		//process triangles data
		mesh->InitTrianglesList(num_tri);

		//process vertex indices----------

		//if reading from triangle lists
		if( !poly_data )
		{
			for( index = 0; index < xsiMesh->GetTriangleListCount(); index++ )
			{
				vertex_ptr     = tri_list[index]->GetVertexIndicesPtr();

				curr_tri_count = tri_list[index]->GetTriangleCount();
				for( i = 0; i < curr_tri_count; i++ )
				{
					mesh->SetTriangleData( curr_tri * 3 + 0, vertex_ptr[i * 3 + 2] );
					mesh->SetTriangleData( curr_tri * 3 + 1, vertex_ptr[i * 3 + 1] );
					mesh->SetTriangleData( curr_tri * 3 + 2, vertex_ptr[i * 3 + 0] );

					curr_tri++;
				} 
			}
		}
		else //if reading from polygon lists
		{
			for( index = 0; index < xsiMesh->GetPolygonListCount(); index++ )
			{
				vertex_ptr = poly_list[index]->GetVertexIndicesPtr();

				curr_poly_count = poly_list[index]->GetPolygonCount();

				for( i = 0; i < curr_poly_count; i++ )
				{
					mesh->SetTriangleData(curr_poly * 3 + 0, vertex_ptr[i * 3 + 2]);
					mesh->SetTriangleData(curr_poly * 3 + 1, vertex_ptr[i * 3 + 1]);
					mesh->SetTriangleData(curr_poly * 3 + 2, vertex_ptr[i * 3 + 0]);

					curr_poly++;
				} 
			}
		}


		//process normals
		mesh->PrecalculateNormals( m_pDlgProgress );

	//   //process texture
		//CSLImageLibrary *il = model->Scene()->GetImageLibrary();

		//if( il != NULL && GetTexturePath() == "" )
		//{
		//	if( il->GetImageCount() > 0 && GetTexturePath().empty() )
		//	{
		//		tex_path = il->GetImageList()[0]->GetSourceFile();
		//		SetTexturePath( tex_path );
		//	}
		//}


		//if textured
		//if( GetTexturePath().empty() == false )
		//{

		//process UV coords
		CSIBCVector2D*	pUVValues	= NULL;
		CSIBCVector2D*	pUVValues2	= NULL;
		int*			pUVIndices	= NULL;
		int*			pUVIndices2	= NULL;


		if( xsiMesh->GetTriangleListCount() == 0 )
		{
			MessageBox( NULL, "dotXSI file consists non triangulated meshes, this feature is not fully supported!\n\rThe program might crash.", "Warning!", MB_OK | MB_ICONWARNING );
			//exit(0);
		}
		else
		{
			pUVIndices	= xsiMesh->TriangleLists()[0]->GetUVIndicesPtr(0); 
			pUVIndices2	= xsiMesh->TriangleLists()[0]->GetUVIndicesPtr(1); 
			
			//depending on xsi version get uv coords array
			if( pShape->Type() == CSLBaseShape::SI_SHAPE )
				pUVValues = ((CSLShape*)pShape)->GetUVCoordListPtr();
			else
			{
				if( ((CSLShape_35*)pShape)->GetUVCoordArrayCount() > 0 )
				{
					pUVValues	= ((CSLShape_35*)pShape)->UVCoordArrays()[0]->GetUVCoordListPtr();

					if( ((CSLShape_35*)pShape)->GetUVCoordArrayCount() > 1 )
					{
						pUVValues2	= ((CSLShape_35*)pShape)->UVCoordArrays()[1]->GetUVCoordListPtr();
						mesh->InitSecondaryUV();
					}
				}
			}
		}


		if( pUVValues != NULL )
		{
			for( j = 0; j < mesh->GetTrianglesCount(); j++ )
			{
				// Calculate the current vertex index from SI_TriangleList
				DWORD dwIndex0 = pUVIndices[ j * 3 + 0 ];
				DWORD dwIndex1 = pUVIndices[ j * 3 + 1 ];
				DWORD dwIndex2 = pUVIndices[ j * 3 + 2 ];

				//convert to LH coords system
				if( pUVValues2 != NULL )
				{ 
					DWORD dwIndex0b = pUVIndices2[ j * 3 + 0 ];
					DWORD dwIndex1b = pUVIndices2[ j * 3 + 1 ];
					DWORD dwIndex2b = pUVIndices2[ j * 3 + 2 ];

					mesh->SetUVData( dwIndex0, j * 3 + 2, pUVValues[ dwIndex0 ].m_fX, pUVValues[ dwIndex0 ].m_fY, pUVValues2[ dwIndex0b ].m_fX, pUVValues2[ dwIndex0b ].m_fY );			
					mesh->SetUVData( dwIndex1, j * 3 + 1, pUVValues[ dwIndex1 ].m_fX, pUVValues[ dwIndex1 ].m_fY, pUVValues2[ dwIndex1b ].m_fX, pUVValues2[ dwIndex1b ].m_fY );			
					mesh->SetUVData( dwIndex2, j * 3 + 0, pUVValues[ dwIndex2 ].m_fX, pUVValues[ dwIndex2 ].m_fY, pUVValues2[ dwIndex2b ].m_fX, pUVValues2[ dwIndex2b ].m_fY );
				}
				else
				{
					mesh->SetUVData( dwIndex0, j * 3 + 2, pUVValues[ dwIndex0 ].m_fX, pUVValues[ dwIndex0 ].m_fY );			
					mesh->SetUVData( dwIndex1, j * 3 + 1, pUVValues[ dwIndex1 ].m_fX, pUVValues[ dwIndex1 ].m_fY );			
					mesh->SetUVData( dwIndex2, j * 3 + 0, pUVValues[ dwIndex2 ].m_fX, pUVValues[ dwIndex2 ].m_fY );					
				}
			}
		}
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}

}

//void CXSIFile::StripModelNorm( CSLModel* pModel, CXSIMesh* pMesh )
//{
//	CSLMesh* xsiMesh = (CSLMesh*)pModel->Primitive();
//
//	CString msg;
//	msg.Format( "Models: %i of %i", m_iProcessedModels + 1,  m_iAmountModels );
//
//
//	//update progress dialog
//	if( m_pDlgProgress )
//	{
//		((CValidateDlg*)m_pDlgProgress)->SetProgressMessageStrA( msg ); 
//		((CValidateDlg*)m_pDlgProgress)->SetProgressMessageStrB( "" ); 
//		((CValidateDlg*)m_pDlgProgress)->Step();
//	}
//
//
//	//declarations
//	CSLBaseShape::CSLIndexedVector3DArray*	vertex_list		= NULL;
//	CSLBaseShape::CSLVector3DArray*			vertex_list2	= NULL;
//
//	float			x, y, z, r, g, b;
//	CSLTransform*	transformations	= (CSLTransform*)pModel->Transform();
//	CSLModel*		parent			= pModel->Parent();
//	int				num_tri			= CountTriangles( xsiMesh );
//    bool			poly_data		= false;
//
//	if( num_tri == 0 )
//	{
//		num_tri		= CountPolygons( xsiMesh );
//		poly_data	= true;
//	}
//
//    SI_Char* nm	= pModel->Name().GetText();
//
//	//in case model has no tris
//	if( num_tri <= 0 )
//		return;
//
//	int			curr_tri_count = 0, curr_poly_count = 0, curr_tri = 0, curr_poly = 0, iRGBCounter = 0;
//	SI_Char*	tex_path	= "";
//	SI_Int*		vertex_ptr;
//	int			num_shapes = 0;
//
//	CSLTriangleList**		tri_list	= xsiMesh->TriangleLists();
//	CSLPolygonList**		poly_list	= xsiMesh->PolygonLists();
//	CSLGlobalMaterial*		g_mat		= pModel->GlobalMaterial();
//	CSLVariantParameter*	temp_param	= NULL;
//	CSLBaseShape*			pShape		= xsiMesh->Shape();
//	CSLShapeAnimation*		animation	= NULL;
//	CSLBaseShape**			shapes		= NULL;
//	CSLXSIMaterial*			mat			= NULL;
//
//	if( g_mat )
//		mat = (CSLXSIMaterial*)g_mat->GetMaterial();
//
//	x = y = z = 0.0f;
//	a = r = g = b = 1.0f;
//
//	//get shape animations
//	animation = xsiMesh->ShapeAnimation();
//
//	//shape animation present
//	if (animation)
//	{
//		shapes		= animation->Shapes();
//		num_shapes	= animation->GetShapeCount();
//	}
//
//	//process mesh name
//	pMesh->SetName( pModel->Name().GetText() );
//
//	//find diffuse components
//	//iterate through all the shaders
//	if( mat && mat->GetShaderCount() < 10000 )
//	{
//		for(int i = 0; i < mat->GetShaderCount(); i++)
//		{
//			temp_param = (CSLVariantParameter*)mat->GetShaderList()[i]->ParameterFromName("diffuse.red");
//			(temp_param != NULL)?r = temp_param->GetValue()->fVal:0;
//
//			temp_param = (CSLVariantParameter*)mat->GetShaderList()[i]->ParameterFromName("diffuse.green");
//			(temp_param != NULL)?g = temp_param->GetValue()->fVal:0;
//
//			temp_param = (CSLVariantParameter*)mat->GetShaderList()[i]->ParameterFromName("diffuse.blue");
//			(temp_param != NULL)?b = temp_param->GetValue()->fVal:0;
//
//			if( r && g && b )
//			  pMesh->SetColor( a, r, g, b );
//		}
//	}
//	else
//	{
//		//set default values
//		pMesh->SetColor( a, r, g, b );
//	}
//
//	//shape animation present
//
//	//process vertecies data
//	pMesh->InitVerteciesList( num_tri * 3 );
//
//	//shape animation present
//	if( shapes )
//	{
//		for( int i = 0; i < GetFramesCount(); i++ )
//		{
//			curr_tri = 0;
//
//			//update progress dialog
//			msg.Format( "Frames: %i of %i", i + 1, GetFramesCount() );
//			
//			if( m_pDlgProgress )
//			{
//				((CValidateDlg*)m_pDlgProgress)->SetProgressMessageStrB( msg ); 
//				((CValidateDlg*)m_pDlgProgress)->Step();
//			}
//
//			vertex_list = shapes[i]->GetIndexedVertexList();
//
//			for( int index = 0; index < xsiMesh->GetTriangleListCount(); index++ )
//			{
//				vertex_ptr     = tri_list[index]->GetVertexIndicesPtr();
//				curr_tri_count = tri_list[index]->GetTriangleCount();
//				
//				for( int k = 0; k < curr_tri_count; k++ )
//				{
//					D3DXVECTOR3 v_vec(0.0f, 0.0f, 0.0f);
//
//					for( int u = 0; u < 3; u++ )
//					{
//						(*vertex_list)[vertex_ptr[k * 3 + (2 - u)]].m_Vector3D.Get( &x, &y, &z );
//
//						CalcGlobalTransform( x, y, z, pModel );
//
//						v_vec = D3DXVECTOR3( x, y, z );
//
//						if( originator == XO_SOFTIMAGE )
//							ConvertVec2LHCoords( &v_vec );
//						else
//							ConvertVec2LHCoordsDeepEx( &v_vec );
//
//						x = v_vec.x;
//						y = v_vec.y;
//						z = v_vec.z;
//
//						pMesh->SetVertexData( i, curr_tri * 3 + u, x, y, z, D3DCOLOR_COLORVALUE( r, g, b, 0.0 ) );
//					}
//
//					curr_tri++;
//				}
//				
//			}
//		}
//	}
//	else
//	{
//		vertex_list2 = xsiMesh->Shape()->GetVertexList();
//
//		for( int index = 0; index < xsiMesh->GetTriangleListCount(); index++ )
//		{
//			vertex_ptr     = tri_list[index]->GetVertexIndicesPtr();
//			curr_tri_count = tri_list[index]->GetTriangleCount();
//
//			for( int k = 0; k < curr_tri_count; k++ )
//			{
//				D3DXVECTOR3 v_vec(0.0f, 0.0f, 0.0f);
//
//				for( int u = 0; u < 3; u++ )
//				{
//					(*vertex_list2)[vertex_ptr[k * 3 + (2 - u)]].Get( &x, &y, &z );
//
//					CalcGlobalTransform( x, y, z, pModel );
//
//					v_vec = D3DXVECTOR3( x, y, z );
//
//					if( originator == XO_SOFTIMAGE )
//						ConvertVec2LHCoords( &v_vec );
//					else
//						ConvertVec2LHCoordsDeepEx( &v_vec );
//
//					x = v_vec.x;
//					y = v_vec.y;
//					z = v_vec.z;
//
//					pMesh->SetVertexData( 0, curr_tri * 3 + u, x, y, z, D3DCOLOR_COLORVALUE( r, g, b, 0.0 ) );
//				}
//
//				curr_tri++;
//			}
//		}
//
//	}
//
//	//process vertex indices----------
//	//process triangles data
//	pMesh->InitTrianglesList( num_tri );
//
//	for( int i = 0; i < num_tri * 3; i ++ )
//	pMesh->SetTriangleData( i, i );
//
//
//	//process normals
//	pMesh->PrecalculateNormals( m_pDlgProgress );
//
//    //process texture
//	CSLImageLibrary *il = pModel->Scene()->GetImageLibrary();
//
//	if( il != NULL && GetTexturePath() == "" )
//	{
//		if( il->GetImageCount() > 0 && GetTexturePath().empty() )
//		{
//			tex_path = il->GetImageList()[0]->GetSourceFile();
//			SetTexturePath( tex_path );
//		}
//	}
//
//	int count;
//
//
//
//	//if textured
//	if( GetTexturePath().empty() == false )
//	{
//
//		if( xsiMesh->GetTriangleListCount() == 0 )
//		{
//			MessageBox( NULL, "dotXSI file consists non triangulated meshes, this feature is not supported!\n\rThe program will now exit...", "Error!", MB_OK | MB_ICONERROR );
//			//exit(0);
//		}
//
//		//process UV coords
//		CSIBCVector2D *pUVValues  = NULL;
//		int           *pUVIndices = xsiMesh->TriangleLists()[0]->GetUVIndicesPtr(0); 
//		
//		//depending on xsi version get uv coords array
//		if( pShape->Type() == CSLBaseShape::SI_SHAPE )
//			pUVValues = ((CSLShape*)pShape)->GetUVCoordListPtr();
//		else
//			if( ((CSLShape_35*)pShape)->GetUVCoordArrayCount() > 0 )
//			{
//				pUVValues = ((CSLShape_35*)pShape)->UVCoordArrays()[0]->GetUVCoordListPtr();
//				count = ((CSLShape_35*)pShape)->GetUVCoordArrayCount();
//			}
//
//		if( num_shapes == 0 )
//			num_shapes = 1;
//
//		if( pUVValues != NULL && num_shapes > 0 )
//		{
//			//if( num_shapes > 1 )
//			//	num_shapes--;
//
//			for( int j = 0; j < pMesh->GetTrianglesCount(); j++ )
//			{
//				// Calculate the current vertex index from SI_TriangleList
//				DWORD dwIndex0 = pUVIndices[ j * 3 + 0 ];
//				DWORD dwIndex1 = pUVIndices[ j * 3 + 1 ];
//				DWORD dwIndex2 = pUVIndices[ j * 3 + 2 ];
//
//				//convert to LH coords system
//				pMesh->SetUVData( j * 3 + 2, pUVValues[ dwIndex0 ].m_fX, pUVValues[ dwIndex0 ].m_fY );			
//				pMesh->SetUVData( j * 3 + 1, pUVValues[ dwIndex1 ].m_fX, pUVValues[ dwIndex1 ].m_fY );			
//				pMesh->SetUVData( j * 3 + 0, pUVValues[ dwIndex2 ].m_fX, pUVValues[ dwIndex2 ].m_fY );
//			}
//		}
//	}
//
//	//clean-up
//}

void CXSIFile::CalcGlobalTransform( float& x, float& y, float& z, CSLModel* pModel )
{
	int				matrices_count	= 0;
	int				i				= 0;
	CSIBCMatrix4x4	xsi_res;
	vector<CSIBCMatrix4x4> mat;

	if( !pModel )
		return;

	CSLModel*		parent_model = pModel;
	CSLTransform*	parent_transform = pModel->Transform();

	//matrices_count++;

	// count matrices from current model up to the root
	while( parent_model && parent_transform )
	{
		mat.push_back( parent_transform->GetMatrix() );

		parent_model		= parent_model->Parent();
		parent_transform	= parent_model->Transform();
	}

	if( mat.size() > 1 )
	{
		xsi_res = mat[0];

		//multiply matrices
		for( UINT i = 1; i < mat.size(); i++ )
		{
			xsi_res = mat[i] * xsi_res;
		}

		xsi_res = xsi_res;// * pModel->Transform()->GetMatrix();
	}
	else
		xsi_res = mat[0];

	//xsi_res = model->Transform()->GetMatrix();

	SI_Float* xsi_raw_mat = xsi_res.Raw();

	D3DXMATRIX mat_res;

	D3DXMatrixIdentity( &mat_res );

	mat_res._11 = xsi_raw_mat[0];
	mat_res._12 = xsi_raw_mat[1];
	mat_res._13 = xsi_raw_mat[2];
	mat_res._14 = xsi_raw_mat[3];

	mat_res._21 = xsi_raw_mat[4];
	mat_res._22 = xsi_raw_mat[5];
	mat_res._23 = xsi_raw_mat[6];
	mat_res._24 = xsi_raw_mat[7];

	mat_res._31 = xsi_raw_mat[8];
	mat_res._32 = xsi_raw_mat[9];
	mat_res._33 = xsi_raw_mat[10];
	mat_res._34 = xsi_raw_mat[11];

	mat_res._41 = xsi_raw_mat[12];
	mat_res._42 = xsi_raw_mat[13];
	mat_res._43 = xsi_raw_mat[14];
	mat_res._44 = xsi_raw_mat[15];

	D3DXVECTOR3 res(x, y, z);
	D3DXVECTOR3 out(0, 0, 0);

	D3DXVec3TransformCoord( &out, &res, &mat_res );

	x = out.x;
	y = out.y;
	z = out.z;
}
int CXSIFile::CountCameras( CSLModel* pModel )
{
	int res = 0;

	CSLModel** l_childrenList = pModel->GetChildrenList();

	if( pModel->GetPrimitiveType() == CSLTemplate::SI_CAMERA )
		res = 1;

	// Loop through all children
	for( int i = 0 ; i < pModel->GetChildrenCount(); i++ )
		res += CountCameras( l_childrenList[i] );

	return res;
}

int	CXSIFile::CountLights( CSLModel* pModel )
{
	int res = 0;

	CSLModel** l_childrenList = pModel->GetChildrenList();

	if(	pModel->GetPrimitiveType() == CSLTemplate::SI_SPOT_LIGHT || 
		pModel->GetPrimitiveType() == CSLTemplate::SI_POINT_LIGHT ||	
		pModel->GetPrimitiveType() == CSLTemplate::SI_DIRECTIONAL_LIGHT )
	{
		res = 1;
	}

	// Loop through all children
	for( int i = 0 ; i < pModel->GetChildrenCount(); i++ )
		res += CountLights( l_childrenList[i] );

	return res;
}

int	CXSIFile::CountNulls( CSLModel* pModel )
{
	int res = 0;

	CSLModel** l_childrenList = pModel->GetChildrenList();

	if(	pModel->GetPrimitiveType() == CSLTemplate::SI_NULL_OBJECT )
		res = 1;

	// Loop through all children
	for( int i = 0 ; i < pModel->GetChildrenCount(); i++ )
		res += CountNulls( l_childrenList[i] );

	return res;
}

bool CXSIFile::DetectShapeAnimation( CSLModel* pModel )
{	
	bool detected = false;

	if( pModel->GetPrimitiveType() == CSLTemplate::SI_MESH )
	{
		_ASSERTE( pModel != NULL );


		CSLMesh*				xsiMesh		= (CSLMesh*)pModel->Primitive();
		CSLShapeAnimation*		animation	= NULL;
		CSLBaseShape**			shapes		= NULL;

		//get shape animations
		animation = xsiMesh->ShapeAnimation();

		//shape animation present
		if( animation )
			shapes = animation->Shapes();

		if( shapes )
			return true;
	}


	CSLModel **l_childrenList = pModel->GetChildrenList();


	// Loop through all children
	for( int i = 0 ; i < pModel->GetChildrenCount(); i++ )
	{
		if( DetectShapeAnimation( l_childrenList[i] ) )
			detected = true;
	}

	return detected;
}

bool CXSIFile::DetectCurveAnimation( CSLModel* pModel )
{
	bool detected = false;

	if( pModel->GetPrimitiveType() == CSLTemplate::SI_NULL_OBJECT )
	{
		_ASSERTE( pModel != NULL );


		CSLNull*		obj_null		= (CSLNull*)pModel->Primitive();
		CSLTransform*	Transformation	= (CSLTransform*)pModel->Transform();

		if( obj_null && Transformation )
		{
			CSIBCVector3D Translation( Transformation->GetTranslation() );
			CSIBCVector3D Rotation( Transformation->GetEulerRotation() );
			CSIBCVector3D Scale( Transformation->GetScale() );
			
			CSLBaseFCurve* XRot = Transformation->GetSpecificFCurve( CSLTemplate::SI_ROTATION_X );
			CSLBaseFCurve* YRot = Transformation->GetSpecificFCurve( CSLTemplate::SI_ROTATION_Y );
			CSLBaseFCurve* ZRot	= Transformation->GetSpecificFCurve( CSLTemplate::SI_ROTATION_Z );

			CSLBaseFCurve* XTrans	= Transformation->GetSpecificFCurve( CSLTemplate::SI_TRANSLATION_X );
			CSLBaseFCurve* YTrans	= Transformation->GetSpecificFCurve( CSLTemplate::SI_TRANSLATION_Y );
			CSLBaseFCurve* ZTrans	= Transformation->GetSpecificFCurve( CSLTemplate::SI_TRANSLATION_Z );

			CSLBaseFCurve* XScale	= Transformation->GetSpecificFCurve( CSLTemplate::SI_SCALING_X );
			CSLBaseFCurve* YScale	= Transformation->GetSpecificFCurve( CSLTemplate::SI_SCALING_Y );
			CSLBaseFCurve* ZScale	= Transformation->GetSpecificFCurve( CSLTemplate::SI_SCALING_Z );

			if( XRot || YRot || ZRot || 
				XTrans || YTrans || ZTrans || 
				XScale || YScale || ZScale )
			{
				return true;
			}
			else
			{
				//look for animated custom params

				int cust = pModel->GetCustomPSetCount();
				CSLCustomPSet** ppPSet; 

				if( cust > 0 )
				{
					ppPSet = pModel->GetCustomPSetList();

					if( ppPSet[0]->GetFCurveCount() > 0 )
						return true;
				}
				
			}
		}
	}


	CSLModel **l_childrenList = pModel->GetChildrenList();


	// Loop through all children
	for( int i = 0 ; i < pModel->GetChildrenCount(); i++ )
	{
		if( DetectCurveAnimation( l_childrenList[i] ) )
			detected = true;
	}

	return detected;
}

int CXSIFile::CountModels( CSLModel* pModel )
{
	int res = 0;

	CSLModel** l_childrenList = pModel->GetChildrenList();

	if( pModel->GetPrimitiveType() == CSLTemplate::SI_MESH )
		res = 1;

	// Loop through all children
	for( int i = 0 ; i < pModel->GetChildrenCount(); i++)
		res += CountModels( l_childrenList[i] );

	return res;
}

enum XSI_FILE_STAT CXSIFile::Validate( str_t strPath )
{
	m_XsiScene.Close();

	if( m_XsiScene.Open( (char*)strPath.data() ) != SI_SUCCESS )
		return XFS_UNREADABLE;

	//check for poly lists
	m_XsiScene.Read();

	/* GET ORIGINATOR */
		
	// SOFTIMAGE|XSI	// 13
	// Deep Exploration	// 16
	CString strTemp	= m_XsiScene.FileInfo()->GetOriginator();
	CString strTemp2 = strTemp.Left( 13 );

	if( strTemp2 == "SOFTIMAGE|XSI" )
		originator = XO_SOFTIMAGE;
	else
	{
		strTemp2 = strTemp.Left( 16 );

		if( strTemp2 == "Deep Exploration" )
			originator = XO_DEEPEX;
		else
			originator = XO_UNDEFINED;
	}

	/* end -- GET ORIGINATOR */

	if( originator == XO_SOFTIMAGE )
	{
		if( DetectPolyLists( m_XsiScene.Root() ) )
			return XFS_POLYLIST_DETECTED;
	}
	
	return XFS_OK;
}

bool CXSIFile::DetectPolyLists( CSLModel* model )
{
	bool detected = false;


	if( model->GetPrimitiveType() == CSLTemplate::SI_MESH )
	{
		_ASSERTE( model != NULL );


		CSLMesh*			xsiMesh		= (CSLMesh*)model->Primitive();
		CSLPolygonList**	poly_list	= xsiMesh->PolygonLists();

		if( poly_list )
			return true;
	}

	CSLModel **l_childrenList = model->GetChildrenList();

	// Loop through all children
	for( int i = 0 ; i < model->GetChildrenCount(); i++ )
	{
		if( DetectPolyLists( l_childrenList[i] ) )
			detected = true;
	}

	return detected;
}

void CXSIFile::SetDirectory( str_t strPath )
{
	m_szFolder = strPath;
}

void CXSIFile::ResetTexture( LPDIRECT3DDEVICE9 pd3dDevice )
{

}

void CXSIFile::SwapYZCoords()
{
	float temp;

	for( int i = 0; i < m_iMeshesNum; i++ )
		m_objMeshes[i].SwapYZCoords();

	for( UINT i = 0; i < m_vecLights.size(); i++ )
	{
		temp = m_vecLights[i].pos.y;
		m_vecLights[i].pos.y = m_vecLights[i].pos.z;
		m_vecLights[i].pos.z = -temp;

		temp = m_vecLights[i].targ.y;
		m_vecLights[i].targ.y = m_vecLights[i].targ.z;
		m_vecLights[i].targ.z = -temp;
	}


	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		temp = m_vecCameras[i].pos.y;
		m_vecCameras[i].pos.y = m_vecCameras[i].pos.z;
		m_vecCameras[i].pos.z = -temp;

		temp = m_vecCameras[i].dir.y;
		m_vecCameras[i].dir.y = m_vecCameras[i].dir.z;
		m_vecCameras[i].dir.z = -temp;

		temp = m_vecCameras[i].upvector.y;
		m_vecCameras[i].upvector.y = m_vecCameras[i].upvector.z;
		m_vecCameras[i].upvector.z = -temp;
	}

	SwapYZCoordsNULLS();
}

void CXSIFile::SwapYZCoordsNULLS(void)
{
	float temp;

	for( UINT i = 0; i < m_vecNulls.size(); i++ )
	{
		temp							= m_vecNulls[i].matrices[0].pos.y;
		m_vecNulls[i].matrices[0].pos.y	= m_vecNulls[i].matrices[0].pos.z;
		m_vecNulls[i].matrices[0].pos.z	= - temp;
	}
}

void CXSIFile::StripCustomParameter( CSLModel* pModel )
{
	int custPSetCount = pModel->GetCustomPSetCount();
	CSLCustomPSet** ppPSet;
	CSLVariantParameter** ppVariantsList;

	if( custPSetCount <= 0 )
		return;

	ppPSet = pModel->GetCustomPSetList();
	
	int custParCount = ppPSet[0]->GetParameterCount();

	if( custParCount <= 0 )
		return;

	ppVariantsList = ppPSet[0]->GetParameterList();

	for( int i = 0; i < custParCount; i++ )
	{
		CCUSTPARAM custpar;
		custpar.id = pModel->Name().GetText();
		custpar.id += ".";
		custpar.id += ppVariantsList[i]->GetName();

		CSLFCurve* curve = ppPSet[0]->GetParameterFCurve( ppVariantsList[i]->GetName() );

		for( int j = 0; j < GetFramesCount(); j++ )
		{
			if( curve )
			{
				curve->Evaluate( (float)j );
				float temp = curve->GetLastEvaluation();
				custpar.data.push_back( curve->GetLastEvaluation() );
			}
			else
				custpar.data.push_back( ppVariantsList[i]->GetFloatValue() );
		}
		
		m_vecCustParams.push_back( custpar );
	}
}

//extract camera
void CXSIFile::StripCamera( CSLModel* pModel )
{
	CSLCamera*		cam				= (CSLCamera*)pModel->Primitive();
	CSLModel*		model_transform	= NULL;
	char			nm[256];
	CSIBCVector3D	pos;
	CCAMERA			camera;


	/************ GET NAME ***********/
	camera.name = pModel->Name().GetText();
	camera.name = camera.name.substr( 0, camera.name.length() - 1 );


	/************ GET POSITION ***********/
	sprintf( nm, "%s", camera.name.data() );
	model_transform = pModel->Scene()->FindModel( nm );

	pos = cam->GetPosition();

	float x = pos.GetX();
	float y = pos.GetY();
	float z = pos.GetZ();

	CalcGlobalTransform( x, y, z, model_transform );

	// convert to LH coordinate system
	D3DXVECTOR3 v_vec(x, y, z);

	if( originator == XO_SOFTIMAGE )
		ConvertVec2LHCoords( &v_vec );
	else
		ConvertVec2LHCoordsDeepEx( &v_vec );


	/************ GET DIRECTION ***********/
	sprintf( nm, "%s_Target", camera.name.data() );
	model_transform = pModel->Scene()->FindModel( nm );

	x = y = z = 0;

	CalcGlobalTransform( x, y, z, model_transform );

	D3DXVECTOR3 v_vec2( x, y, z );

	if( originator == XO_SOFTIMAGE )
		ConvertVec2LHCoords( &v_vec2 );
	else
		ConvertVec2LHCoordsDeepEx( &v_vec2 );

	camera.pos		= v_vec;
	camera.dir		= v_vec2;
	camera.upvector = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	camera.hfov		= cam->GetFieldOfView();
	camera.vfov		= D3DXToDegree( FovX2FovY( D3DXToRadian( camera.hfov ), 4.0f / 3.0f ) );
	camera.visible	= true;

	m_vecCameras.push_back( camera );
}
