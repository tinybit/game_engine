#pragma warning( disable: 4244 )

#include "sys/stdafx.h"

#include "SceneFile.h"
#include "EfnMeshlist.h"
#include "dialogs/ValidateDlg.h"
#include "dialogs/CookieMachineDlg.h"

void CSceneFile::Initialize()
{
	m_strSceneID		= "";
	m_strEditedObject	= "";
	FileType			= FT_SCN;

	m_pMeshes			= NULL;

	m_vMin				= D3DXVECTOR2( 0.0f, 0.0f );
	m_vMax				= D3DXVECTOR2( 0.0f, 0.0f );
	m_fCellSize			= 0.0f;
	m_iXGridSize		= 0;
	m_iZGridSize		= 0;

	m_pppGrid			= NULL;

	m_pTexture			= NULL;
	m_bGridVisible		= false;
	m_pProgressWnd		= NULL;

	m_pLightBulb		= NULL;
	m_pCamBox			= NULL;

	m_pVBCamLines		= NULL;
	m_pVBLightsLines	= NULL;	
	m_pSphere			= NULL;
	m_pCone				= NULL;

	m_pFloorsEdge		= NULL;
	m_iEdgesCount		= 0;

	m_pCurves			= NULL;

	m_bIsLightRangeVisible	= false;
	m_bGridSelectorsVisible	= false;
	m_iVisibleCamsCount		= 0;
	m_iSelectedCamera		= -1;

	m_iSelectedLight		= -1;
	m_bIsCamZoomNearActive	= false;
	m_bIsCamZoomFarActive	= false;


	m_iShadowMeshVisibilityMode	= -1;
	m_pShadowMesh				= NULL;
	m_iShadowMeshEdgesCount		= 0;

	ZeroMemory( &m_BlueMtrl, sizeof(m_BlueMtrl) );
	m_BlueMtrl.Diffuse = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f );
	m_BlueMtrl.Ambient = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f );

	ZeroMemory( &m_RedMtrl, sizeof( m_RedMtrl ) );
	m_RedMtrl.Diffuse = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );
	m_RedMtrl.Ambient = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );

	ZeroMemory( &m_YellowMtrl, sizeof( m_YellowMtrl ) );
	m_YellowMtrl.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f );
	m_YellowMtrl.Ambient = D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f );

	ZeroMemory( &m_WhiteMtrl, sizeof( m_WhiteMtrl ) );
	m_WhiteMtrl.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_WhiteMtrl.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );

	ZeroMemory( &m_BlackMtrl, sizeof( m_BlackMtrl ) );
	m_BlackMtrl.Diffuse = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
	m_BlackMtrl.Ambient = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );

	ZeroMemory( &m_DarkYellow, sizeof( m_DarkYellow ) );
	m_DarkYellow.Diffuse = D3DXCOLOR( 0.9058f, 0.7725f, 0.0196f, 1.0f );
	m_DarkYellow.Ambient = D3DXCOLOR( 0.9058f, 0.7725f, 0.0196f, 1.0f );
}

void CSceneFile::DeleteAllLights()
{
	for( UINT i = 0; i < m_vecLights.size(); i++ )
		SAFE_DELETE( m_vecLights[i] );

	m_vecLights.clear();	
}

void CSceneFile::DeleteAllCameras()
{
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
		SAFE_DELETE( m_vecCameras[i] );

	m_vecCameras.clear();
}

void CSceneFile::BuildFuncsList()
{
	if( m_lBinds.empty() )
		return;

	SafeReleaseFuncsBuffer();

	ListStrVec_iter iter = m_lBinds.begin();

	while( iter != m_lBinds.end() )
	{
		CString script_str = "";

		if( (*iter).size() <= 5 )
		{
			iter++;
			continue;
		}

		script_str = (*iter)[5].data();
		script_str = script_str.Trim();

		if( script_str.GetLength() == 0 )
		{
			iter++;
			continue;
		}
		
		int offset = script_str.Find( "function", 0 );

		while( offset != -1 )
		{
			int BaseOffset = offset;

			offset += 8;

			bool found = false;

			for( int g = offset; g < script_str.GetLength(); g++ )
			{
				if( script_str[g] != ' ' )
				{
					found = true;
					offset = g;
					break;
				}
			}

			int		start	= offset;
			int		end		= 0;
			CString strFuncName;

			if( found && script_str.Find( '(', offset ) != -1 )
			{
				strFuncName = script_str.Mid( offset, script_str.Find( '(', offset ) );

				strFuncName.Trim();
				strFuncName = strFuncName.Left( strFuncName.Find( '(', 0 ) );
				strFuncName.Trim();
				
				FuncEntry* pFuncEntry	= new FuncEntry;

				pFuncEntry->strClassName	= "";
				pFuncEntry->strFuncName		= "";

				int iClOffset = strFuncName.Find( ':', 0 );

				if( iClOffset != -1 )
				{
					pFuncEntry->strClassName	= strFuncName.Left( iClOffset );
					pFuncEntry->strClassName.Trim();
					pFuncEntry->strFuncName		= strFuncName.Right( strFuncName.GetLength() - iClOffset - 1 );
					pFuncEntry->strFuncName.Trim();
				}
				else
				{
					pFuncEntry->strFuncName = strFuncName;
				}

				pFuncEntry->iBind		= atoi((*iter)[0].data());
				pFuncEntry->iOffset		= BaseOffset;

				m_mapFuncs.insert( IFuncMap_pair( pFuncEntry->iBind, pFuncEntry ) );

				offset = script_str.Find( "function", offset + strFuncName.GetLength() );
			}
			else
				offset = -1;
		} // while( offset != -1 )

		iter++;
	} // while( iter != m_pBinds->end() )
}

void CSceneFile::Destroy()
{
	SAFE_DELETE_ARRAY( m_pCurves );
	SAFE_DELETE_ARRAY( m_pFloorsEdge );


	for( UINT i = 0; i < m_vecLights.size(); i++ )
		SAFE_DELETE( m_vecLights[i] );

	m_vecLights.clear();
	
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
		SAFE_DELETE( m_vecCameras[i] );

	m_vecCameras.clear();

	SAFE_DELETE( m_pMeshes );
	SAFE_RELEASE( m_pVBCamLines );
	SAFE_RELEASE( m_pSphere );
	SAFE_RELEASE( m_pCone );

	DeleteGrid();

	SAFE_RELEASE( m_pTexture );

	for( UINT i = 0; i < m_vecTextRes.size(); i++ )
		SAFE_DELETE( m_vecTextRes[i] );

	for( UINT i = 0; i < m_vecCurves.size(); i++ )
		SAFE_DELETE( m_vecCurves[i] );

	SafeReleaseFuncsBuffer();
}

void CSceneFile::SafeReleaseFuncsBuffer()
{
	// release funcs list entries
	for( IFuncMap_iter i = m_mapFuncs.begin(); i != m_mapFuncs.end(); ++i )
	{
		SAFE_DELETE( i->second );
	}

	m_mapFuncs.clear();
}

void CSceneFile::DeleteLight( str_t strID )
{
	for( UINT i = 0; i < m_vecLights.size(); i++ )
	{
		if( strID == m_vecLights[i]->name )
		{
			SAFE_DELETE( m_vecLights[i] );
			m_vecLights.erase( m_vecLights.begin() + i );
			break;
		} 
	}
}

void CSceneFile::DeleteCamera( str_t strID )
{
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		if( strID == m_vecCameras[i]->name )
		{
			SAFE_DELETE( m_vecCameras[i] );
			m_vecCameras.erase( m_vecCameras.begin() + i );
			break;
		}
	}
}

void CSceneFile::ShowAllCameras( bool bShow )
{
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
		m_vecCameras[i]->visible = bShow;
}

void CSceneFile::ShowAllLights( bool bShow )
{
	for( UINT i = 0; i < m_vecLights.size(); i++ )
		m_vecLights[i]->visible = bShow;
}

bool CSceneFile::IsSupportedVersion( DWORD dwVersion )
{
	if( dwVersion == SCN_VERSION_01 || 
		dwVersion == SCN_VERSION_02 ||
		dwVersion == SCN_VERSION_03 || 
		dwVersion == SCN_VERSION_04 ||
		dwVersion == SCN_VERSION_05 )
		return true;

	return false;
}

bool CSceneFile::LoadFromFile( str_t path )
{
	int iTemp = 0;

	if( path == "" )
		return false;

	streampos pos;

	ifstream file( path.data(), ios::binary );
	file.seekg( pos );

	//load scn file version
	file.read( (char *)&m_dwFileVersion, sizeof(DWORD) );

	if( IsSupportedVersion( m_dwFileVersion ) == false )
	{
		file.close();
		return false;
	}

	//read scene ID
	ReadStrm( file, &m_strSceneID );

	//WR2Strm( file, m_vecCameras[i]->id );

	//SCAMERA_LOAD tmpCam( *m_vecCameras[i] );
	//file.write( (const char *)&tmpCam, sizeof(SCAMERA_LOAD) );

	//read cams data
	ReadStrm( file, iTemp );
	
	for( int i = 0; i < iTemp; i++ )
	{
		CCAMERA2* cam = new CCAMERA2;
		cam->visible = true;

		if( m_dwFileVersion < SCN_VERSION_05 )
		{
			file.read( (char*)cam, sizeof(CCAMERA2) );	
			cam->fov.y = D3DXToDegree( FovX2FovY( D3DXToRadian( cam->fov.x ), 4.0f / 3.0f ) );
		}
		else
		{
			SCAMERA_LOAD tmpCam;

			ReadStrm( file, &cam->name );
			file.read( (char*)&tmpCam, sizeof(SCAMERA_LOAD) );

			*cam = tmpCam;
		}

		m_vecCameras.push_back( cam );
	}

	//read lights data
	ReadStrm( file, iTemp );	
	for( int i = 0; i < iTemp; i++ )
	{
		CLIGHT* lightRes = new CLIGHT;
		lightRes->visible = true;

		if( m_dwFileVersion < SCN_VERSION_05 )
			file.read( (char*)lightRes, sizeof(CLIGHT) );
		else
		{
			ReadStrm( file, &lightRes->name );
			ReadStrm( file, lightRes->multiplier );
			
			D3DLIGHT9 light;
			file.read( (char*)&light, sizeof(D3DLIGHT9) );

			lightRes->pos = light.Position;
			lightRes->targ = lightRes->pos + (D3DXVECTOR3)light.Direction * 5.0f;

			//D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &(lightRes->targ - lightRes->pos) );

			switch(light.Type)
			{
				case D3DLIGHT_DIRECTIONAL:
					lightRes->type = LT_DIRECTED;
					break;
				
				case D3DLIGHT_POINT:
					lightRes->type = LT_POINT;	//light.Type = D3DLIGHT_SPOT;
					break;
				
				default: // LT_POINT:
					throw 0;
					break;
			}

			lightRes->diffuse.r = (int)(light.Diffuse.r * 255);
			lightRes->diffuse.g = (int)(light.Diffuse.g * 255);
			lightRes->diffuse.b = (int)(light.Diffuse.b * 255);
			lightRes->diffuse.a = 0.0f;
			
			lightRes->specular = lightRes->diffuse;

			lightRes->ambient.r = (int)(light.Ambient.r * 255);
			lightRes->ambient.g = (int)(light.Ambient.g * 255);
			lightRes->ambient.b = (int)(light.Ambient.b * 255);
			lightRes->ambient.a = 0.0f;

			lightRes->attenuation	= light.Attenuation1;
			lightRes->range			= light.Range;
		}

		m_vecLights.push_back( lightRes );
	}
    
    //load textures list    
    int tex_vec_len; 

	//read textures
    ReadStrm( file, tex_vec_len );
    for( int i = 0; i < tex_vec_len; i++ )
    {
        str_t tex_temp;
        ReadStrm( file, &tex_temp );
        m_vecTextures.push_back( tex_temp );
    }

 	// load binds
	int iBindsCount		= 0;
	int iParamsCount	= 0;

	ReadStrm( file, iBindsCount );

	for( int i = 0; i < iBindsCount; i++ )
	{
		ReadStrm( file, iParamsCount );
		
		StrVec strVec;

		for( int j = 0; j < iParamsCount; j++ )
		{
			str_t param;
			ReadStrm( file, &param );

			strVec.push_back( param );
		}

		m_lBinds.push_back( strVec );

		strVec.clear();
	}

	ReadStrm( file, m_vMin );
	ReadStrm( file, m_vMax );

	ReadStrm( file, m_fCellSize );
	ReadStrm( file, m_iXGridSize );
	ReadStrm( file, m_iZGridSize );

	m_pppGrid = new int**[m_iXGridSize];
	for( int i = 0; i < m_iXGridSize; i++ )
		m_pppGrid[i] = new int*[m_iZGridSize];

	for( int i = 0; i < m_iXGridSize; i++ )
	{
		for( int j = 0; j < m_iZGridSize; j++ )
		{
			int size; 
			ReadStrm( file, size ); 
			
			m_pppGrid[i][j] = new int[size];

			file.read( (char *)m_pppGrid[i][j], sizeof(int) * size );
		}
	}

	if( m_dwFileVersion > SCN_VERSION_01 )
	{
		int		iCustObjCount = 0;
		str_t	strTemp = "";

		ReadStrm( file, iCustObjCount );

		for( int i = 0; i < iCustObjCount; i++ )
		{
			ReadStrm( file, &strTemp );
			m_vecCustomObjects.push_back( strTemp );
		}

		int iCustTypesCount = 0;
		ReadStrm( file, iCustTypesCount );

		for( int i = 0; i < iCustTypesCount; i++ )
		{
			ReadStrm( file, &strTemp );
			m_vecCustomTypes.push_back( strTemp );
		}		
	}

	if( m_dwFileVersion > SCN_VERSION_02 )
	{
		int	iTextResCount = 0;
		ReadStrm( file, iTextResCount );

		for( int i = 0; i < iTextResCount; i++ )
		{
			TextResItem* item = new TextResItem();

			ReadStrm( file, &item->id );
			ReadStrm( file, &item->comments );
			ReadStrm( file, &item->data );

			m_vecTextRes.push_back( item );
		}
	}

	if( m_dwFileVersion > SCN_VERSION_03 )
	{
		//curves
		int iCurvesCount = 0;
		int iNullsCount = 0;
		str_t strTemp;

		ReadStrm( file, iCurvesCount );

		for( int i = 0; i < iCurvesCount; i++ )
		{
			CurveItem* crv_item = new CurveItem;

			ReadStrm( file, &crv_item->id );
			ReadStrm( file, crv_item->length );

			ReadStrm( file, iNullsCount );

			for( int j = 0; j < iNullsCount; j++ )
			{
				ReadStrm( file, &strTemp );

				crv_item->nulls.push_back( strTemp );
			}

			int type;
			ReadStrm( file, type );

			crv_item->type = (CURVE_TYPE)type;

			m_vecCurves.push_back( crv_item );
		}
	}

	file.close();

	if( LoadMeshlist() == false )
		return false;

	ShowAllCameras( true );
	BuildFuncsList();

	return true;
}

void CSceneFile::SetParent( CWnd* pWnd )
{ 
	m_pParentWnd = pWnd; 
	((CEfnMeshlist*)m_pMeshes)->SetParent( pWnd ); 
}

bool CSceneFile::Prepare( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pMeshes->Prepare( pd3dDevice );

	PrepareGridTexture( pd3dDevice );
	PrepareCamsLines( pd3dDevice );
	PrepareLightsLines( pd3dDevice );
	PrepareCurves();

	SAFE_RELEASE( m_pSphere );
	D3DXCreateSphere( pd3dDevice, 1.0f, 30, 30, &m_pSphere, NULL );

	SAFE_RELEASE( m_pCone );
	D3DXCreateCylinder( pd3dDevice, 0.0f, 1.0f, 1, 12, 1, &m_pCone, NULL );

	D3DXVECTOR3 vecDir;
	ZeroMemory( &m_SelectionLight, sizeof(m_SelectionLight) );
	m_SelectionLight.Type = D3DLIGHT_DIRECTIONAL;

	m_SelectionLight.Diffuse.r = 1.0f;
	m_SelectionLight.Diffuse.g = 0.0f;
	m_SelectionLight.Diffuse.b = 0.0f;

	m_SelectionLight.Ambient.r = 1.0f;
	m_SelectionLight.Ambient.g = 0.2f;
	m_SelectionLight.Ambient.b = 0.2f;

	vecDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXVec3Normalize( (D3DXVECTOR3*)&m_SelectionLight.Direction, &vecDir );
	m_SelectionLight.Range = 3000.0f;


	return false;
}

void CSceneFile::PrepareCamsLines( LPDIRECT3DDEVICE9 pd3dDevice )
{
	VOID*	pVertices;
	int			visible_cams_count = 0;


	//count visible cameras
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		if( m_vecCameras[i]->visible )
			visible_cams_count++;
	}

	//prepare lines for drawing [directed and spot] lights
	if( visible_cams_count > 0 )
	{	
		int			vert_num		= visible_cams_count * 2;
		long		size			= sizeof(CVERTEX) * vert_num;
		CVERTEX*	lines_vertecies	= new CVERTEX[vert_num];


		int rt = 0;

		for( int i = 0; i < visible_cams_count; i++ )
		{
			for( UINT k = rt; k < m_vecCameras.size(); k++ )
			{
				if( m_vecCameras[k]->visible )
				{
					rt = k;
					break;
				}
			}

			CCAMERA2	tmp_cam	= *m_vecCameras[rt];
			DWORD		col		= 0x000000FF;

			if( rt == m_iSelectedCamera )
				col = 0x00FF0000;

			lines_vertecies[i * 2].x			= tmp_cam.pos.x;
			lines_vertecies[i * 2].y			= tmp_cam.pos.y;
			lines_vertecies[i * 2].z			= tmp_cam.pos.z;
			lines_vertecies[i * 2].color		= col;

			lines_vertecies[i * 2 + 1].x		= tmp_cam.targ.x;
			lines_vertecies[i * 2 + 1].y		= tmp_cam.targ.y;
			lines_vertecies[i * 2 + 1].z		= tmp_cam.targ.z;
			lines_vertecies[i * 2 + 1].color	= col;

			rt++;
		}

		pd3dDevice->CreateVertexBuffer( size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBCamLines, NULL );
		m_pVBCamLines->Lock( 0, size, (void**)&pVertices, 0 );
		
		memcpy( pVertices, lines_vertecies, size );

		m_pVBCamLines->Unlock();

		SAFE_DELETE_ARRAY( lines_vertecies );
	}

	m_iVisibleCamsCount = visible_cams_count;
}

void CSceneFile::PrepareLightsLines( LPDIRECT3DDEVICE9 pd3dDevice )
{
	VOID*	pVertices;
	int			visible_lights_count = 0;


	//count visible cameras
	for( UINT i = 0; i < m_vecLights.size(); i++)
	{
		if( m_vecLights[i]->visible && ( m_vecLights[i]->type == LT_DIRECTED || m_vecLights[i]->type == LT_SPOT ) )
			visible_lights_count++;
	}

	//prepare lines for drawing [directed and spot] lights
	if( visible_lights_count > 0 )
	{	
		int			vert_num		= visible_lights_count * 2;
		long		size			= sizeof(CVERTEX) * vert_num;
		CVERTEX*	lines_vertecies	= new CVERTEX[vert_num];


		int rt = 0;

		for( int i = 0; i < visible_lights_count; i++ )
		{
			for( UINT k = rt; k < m_vecLights.size(); k++ )
			{
				if( m_vecLights[k]->visible && ( m_vecLights[k]->type == LT_DIRECTED || m_vecLights[k]->type == LT_SPOT ) )
				{
					rt = k;
					break;
				}
			}

			CLIGHT 	tmp_light	= *m_vecLights[rt];
			DWORD	col			= 0x00FFFF00;

			if( rt == m_iSelectedLight )
				col = 0x00FF0000;

			lines_vertecies[i * 2].x			= tmp_light.pos.x;
			lines_vertecies[i * 2].y			= tmp_light.pos.y;
			lines_vertecies[i * 2].z			= tmp_light.pos.z;
			lines_vertecies[i * 2].color		= col;

			lines_vertecies[i * 2 + 1].x		= tmp_light.targ.x;
			lines_vertecies[i * 2 + 1].y		= tmp_light.targ.y;
			lines_vertecies[i * 2 + 1].z		= tmp_light.targ.z;
			lines_vertecies[i * 2 + 1].color	= col;

			rt++;
		}

		pd3dDevice->CreateVertexBuffer( size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBLightsLines, NULL );
		m_pVBLightsLines->Lock( 0, size, (void**)&pVertices, 0 );
		
		memcpy( pVertices, lines_vertecies, size );

		m_pVBLightsLines->Unlock();

		SAFE_DELETE_ARRAY( lines_vertecies );
	}

	m_iVisibleLightsCount = visible_lights_count;
}

void CSceneFile::PrepareGridTexture( LPDIRECT3DDEVICE9 pd3dDevice )
{
	//prepare grid texture
	if( m_bGridSelectorsVisible || m_bGridVisible )
	{
		if( m_pppGrid && m_iXGridSize > 0 && m_iZGridSize > 0 )
		{
			SAFE_RELEASE( m_pTexture );

			pd3dDevice->CreateTexture( m_iXGridSize, m_iZGridSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, NULL );

			D3DLOCKED_RECT lockedRect;
			m_pTexture->LockRect( 0, &lockedRect, NULL, D3DLOCK_DISCARD );

			DWORD m_dwWidth = lockedRect.Pitch >> 2;
			DWORD m_dwHeight = m_iZGridSize;

			for( int y = 0; y < m_iZGridSize; y++ )
			{	
				for( int x = 0; x < m_iXGridSize; x++ )
				{
					if( m_bGridVisible )
					{
						if( m_pppGrid[ x ][ y ][1] != 0 )
						{
							if( y % 2 )
							{
								if( x % 2 )
									( (DWORD*)lockedRect.pBits )[y * m_dwWidth + x] = 0x00FF0000;
								else
									( (DWORD*)lockedRect.pBits )[y * m_dwWidth + x] = 0x000000FF;
							}
							else
							{
								if( x % 2 )
									( (DWORD*)lockedRect.pBits )[y * m_dwWidth + x] = 0x000000FF;
								else
									( (DWORD*)lockedRect.pBits )[y * m_dwWidth + x] = 0x00FF0000;							
							}
						}
						else
							( (DWORD*)lockedRect.pBits )[y * m_dwWidth + x] = 0x0000FF00;
					}
					else
					{
						if( m_pppGrid[ x ][ y ][0] != 0 )
							( (DWORD*)lockedRect.pBits )[y * m_dwWidth + x] = 0x00FF0000;
						else
							( (DWORD*)lockedRect.pBits )[y * m_dwWidth + x] = 0x000000FF;
					}
				}
			}

			m_pTexture->UnlockRect( 0 );
		}
	}
}

void CSceneFile::SetVColorFromMaterial( bool bValue, LPDIRECT3DDEVICE9 pd3dDevice )
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

void CSceneFile::Reset( LPDIRECT3DDEVICE9 pd3dDevice )
{
	SAFE_RELEASE( m_pSphere );
	D3DXCreateSphere( pd3dDevice, 1.0f, 30, 30, &m_pSphere, NULL );

	SAFE_RELEASE( m_pCone );
	D3DXCreateCylinder( pd3dDevice, 0.0f, 1.0f, 1, 12, 1, &m_pCone, NULL );
}

void CSceneFile::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	DWORD	param1;
	BOOL	param2;
	DWORD	param3;
	DWORD	param4;
	DWORD	param5;
	D3DMATERIAL9 mat;

	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &param1 );
	pd3dDevice->GetLightEnable( 0, &param2 );
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &param3 );
	pd3dDevice->GetRenderState( D3DRS_CULLMODE, &param4 );
	pd3dDevice->GetRenderState( D3DRS_AMBIENT, &param5 );
	pd3dDevice->GetMaterial( &mat );

	RenderCams( pd3dDevice );
	RenderLights( pd3dDevice );
	RenderCurves( pd3dDevice );

	//render
	if( m_pFloorsEdge )
	{
		SetVColorFromMaterial( true, pd3dDevice );
		pd3dDevice->SetMaterial( &m_RedMtrl );
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );

		pd3dDevice->SetFVF( D3DFVF_VERTEX3D );
		pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, m_iEdgesCount / 2, m_pFloorsEdge, sizeof( D3DXVECTOR3 ) );
	}

	SetVColorFromMaterial( true, pd3dDevice );
	pd3dDevice->SetMaterial( &mat );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, param1 );
	pd3dDevice->LightEnable( 0, param2 );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, param3 );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, param4 );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, param5 );

	//render meshes
	if( m_bGridSelectorsVisible || m_bGridVisible )
	{
		// Enable alpha blending.
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		// Set the source blend state.
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );

		// Set the destination blend state.
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );


		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );  
			
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );

		pd3dDevice->SetTexture( 0, m_pTexture );
	}
	else
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );


	m_pMeshes->Render( pd3dDevice );

	RenderShadowMesh( pd3dDevice );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, param1 );
	pd3dDevice->LightEnable( 0, param2 );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, param3 );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, param4 );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, param5 );
}

D3DXVECTOR3 CSceneFile::CalcCatmullSpline3D( D3DXVECTOR3* a, D3DXVECTOR3* b, D3DXVECTOR3* c, D3DXVECTOR3* d, float u )
{
	D3DXVECTOR3 res;

	/*res.x = a->x * ( -pow( u, 3 ) + 2.0f * pow( u, 2 ) - u ) +
			b->x * ( 3.0f * pow( u, 3 ) - 5.0f * pow( u, 2 ) + 2.0f ) +
			c->x * ( -3.0f * pow( u, 3 ) + 4.0f * pow( u, 2 ) + u ) + 
			d->x * ( pow( u, 3 ) - pow( u, 2 ) );

	res.y = a->y * ( -pow( u, 3.0f ) + 2.0f * pow( u, 2.0f ) - u ) +
			b->y * ( 3.0f * pow( u, 3.0f ) - 5.0f * pow( u, 2.0f ) + 2.0f ) +
			c->y * ( -3.0f * pow( u, 3.0f ) + 4.0f * pow( u, 2.0f ) + u ) + 
			d->y * ( pow( u, 3.0f ) - pow( u, 2.0f ) );

	res.z = a->z * ( -pow( u, 3.0f ) + 2.0f * pow( u, 2.0f ) - u ) +
			b->z * ( 3.0f * pow( u, 3.0f ) - 5.0f * pow( u, 2.0f ) + 2.0f ) +
			c->z * ( -3.0f * pow( u, 3.0f ) + 4.0f * pow( u, 2.0f ) + u ) + 
			d->z * ( pow( u, 3.0f ) - pow( u, 2.0f ) );*/

	 res.x = (float)(d->x * (1.0/6) * powf(1.0f - u, 3) + 
        c->x * (1.0/6) * ( 3.0 * powf(u, 3) - 6.0 * powf(u, 2) + 4.0) +
        b->x * (1.0/6) * (-3.0 * powf(u, 3) + 3.0 * powf(u, 2) + 3.0 * u + 1.0) +
        a->x * (1.0/6) * powf(u, 3));

      res.y = (float)(d->y * (1.0/6) * powf(1.0f - u, 3) + 
        c->y * (1.0/6) * ( 3.0 * powf(u, 3) - 6.0 * powf(u, 2) + 4.0) +
        b->y * (1.0/6) * (-3.0 * powf(u, 3) + 3.0 * powf(u, 2) + 3.0 * u + 1.0) +
        a->y * (1.0/6) * powf(u, 3));

	  res.z = (float)(d->z * (1.0/6) * powf(1.0f - u, 3) + 
        c->z * (1.0/6) * ( 3.0 * powf(u, 3) - 6.0 * powf(u, 2) + 4.0) +
        b->z * (1.0/6) * (-3.0 * powf(u, 3) + 3.0 * powf(u, 2) + 3.0 * u + 1.0) +
        a->z * (1.0/6) * powf(u, 3));

	return res;
}

void CSceneFile::PrepareCurves()
{
	int vcount = 0;

	SAFE_DELETE_ARRAY( m_pCurves );

	CEfnMeshlist*	meshlist	= ((CEfnMeshlist*)m_pMeshes);
	CMesh*			nulls_mesh	= meshlist->GetMeshByName( "Nulls" );

	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		vcount += (int)m_vecCurves[i]->nulls.size();

		if( m_vecCurves[i]->type == CATMULROM )
		{
			if( (int)m_vecCurves[i]->nulls.size() > 0 )
				vcount += ( (int)m_vecCurves[i]->nulls.size() - 1 ) * CURVE_STEP - 1;
		}
	}

	if( vcount == 0 )
		return;

	m_pCurves = new D3DXVECTOR3[ vcount + m_vecCurves.size() ];

	vcount = 0;

	float step = 1.0f / CURVE_STEP;

	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		vector<D3DXVECTOR3> buff;
		vector<D3DXVECTOR3> buff_2;

		for( UINT j = 0; j < m_vecCurves[i]->nulls.size(); j++ )
		{
			D3DXVECTOR3 vTemp;

			CNULL* pNull4 = nulls_mesh->GetNullByName( m_vecCurves[i]->nulls[j] );

			vTemp.x = pNull4->matrices[0][0];
			vTemp.y = pNull4->matrices[0][1];
			vTemp.z = pNull4->matrices[0][2];

			buff.push_back( vTemp );
		}

		if( buff.size() == 0 )
			continue;
		
		int top = 0;

		if( m_vecCurves[i]->type == CATMULROM )
			top = (int)buff.size() - 1;
		else
			top = (int)buff.size();

		m_vecCurves[i]->length = 0;

		for( int j = 0; j < top; j++ )
		{
			if( m_vecCurves[i]->type == CATMULROM )
			{
				D3DXVECTOR3 _a, A, B, b_, out;

				A = buff[j];
				B = buff[j + 1];


				if( (j - 1) < 0 )
					_a = A;
				else
					_a = buff[j - 1];


				if( (j + 2) >= (int)buff.size() )
					b_ = B;
				else
					b_ = buff[j + 2];

				
				int u = 0;

				if( j > 0 )
					u = 1;

				for( u; u <= CURVE_STEP; u++ )
				{
					float theta = step * u;

					m_pCurves[vcount] = *D3DXVec3CatmullRom( &out, &_a, &A, &B, &b_, theta );

					buff_2.push_back( m_pCurves[vcount] );

					vcount++;
				}
			}
			else if( m_vecCurves[i]->type == LINEAR )
			{
				m_pCurves[vcount] = buff[j];
				vcount++;
			}
		}


		//recalc curve length
		if( m_vecCurves[i]->type == CATMULROM )
		{
			if( buff_2.size() > 1 )
			{
				for( UINT u = 0; u < buff_2.size() - 1; u++ )
					m_vecCurves[i]->length += D3DXVec3Length( &(buff_2[u] - buff_2[u + 1]) );
			}
		}
		else if( m_vecCurves[i]->type == LINEAR )
		{
			if( buff.size() > 1 )
			{
				for( UINT u = 0; u < buff.size() - 1; u++ )
					m_vecCurves[i]->length += D3DXVec3Length( &(buff[u] - buff[u + 1]) );
			}
		}

	}
}

void CSceneFile::RenderLightCone( LPDIRECT3DDEVICE9 pd3dDevice, CLIGHT* light )
{
	D3DXMATRIX matr_old_world;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );

	float angX, angY;
	CalcAzimuthAnglesFromVec( &light->pos, &light->targ, angX, angY, true );

	float length = D3DXVec3Length( &(light->targ - light->pos) );

	//render light cone
	D3DXMATRIX matx, res = matr_old_world;

	D3DXMatrixTranslation( &matx, light->pos.x, light->pos.y, light->pos.z );
	res = matx * res;

	D3DXMatrixRotationY( &matx, angY );
	res = matx * res;

	D3DXMatrixRotationX( &matx, angX );
	res = matx * res;

	D3DXMatrixScaling( &matx, 10, 10, length );
	res = matx * res;


	pd3dDevice->SetTransform( D3DTS_WORLD, &res );

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	((CEfnCharacter*)m_pLightCone)->Render( pd3dDevice, true, 0, false );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

void CSceneFile::RenderLights( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int i_dir_lt_count = 0;

	DWORD dwCull;
	pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCull );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	//store global transform
	D3DXMATRIX matr_old_world;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );

	SetVColorFromMaterial( true, pd3dDevice );

	if( m_vecLights.size() > 0 && m_pLightBulb )
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

		D3DXMATRIX transl_pos, scale_pos, transl_targ, scale_targ;
		float fIconScale = 1.0f;

		//get icon scale
		if( m_pParentWnd )
			fIconScale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();


		//render icons
		for( UINT i = 0; i < m_vecLights.size(); i++ )
		{
			if( m_vecLights[i]->visible )
			{
				if( m_iSelectedLight == i )
				{
					pd3dDevice->SetMaterial( &m_RedMtrl );
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );
				}
				else
				{
					pd3dDevice->SetMaterial( &m_YellowMtrl );
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffff00 );
				}


				if( m_vecLights[i]->type == LT_DIRECTED || m_vecLights[i]->type == LT_SPOT )
				{
					//render light lines
					pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );
					pd3dDevice->SetStreamSource( 0, m_pVBLightsLines, 0, sizeof(CVERTEX) );
					pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
					pd3dDevice->DrawPrimitive( D3DPT_LINELIST, i_dir_lt_count * 2, 1 );
					
					if( ((CCookieMachineDlg*)m_pParentWnd)->GetViewMgrGlob()->m_SpotLightsConeVisible )
						RenderLightCone( pd3dDevice, m_vecLights[i] );

					i_dir_lt_count++;
				}

				// prepare POS transformation
				D3DXMatrixTranslation( &transl_pos, m_vecLights[i]->pos.x, m_vecLights[i]->pos.y, m_vecLights[i]->pos.z );
				D3DXMatrixScaling( &scale_pos, fIconScale, fIconScale, fIconScale );

				pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( scale_pos * transl_pos * matr_old_world) );

				//render cam icon
				((CEfnCharacter*)m_pLightBulb)->Render( pd3dDevice, false, 0.0f );

				//render range
				if( m_iSelectedLight == i && m_bIsLightRangeVisible	)
				{
					pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
					D3DXMatrixScaling( &scale_targ, m_vecLights[i]->range, m_vecLights[i]->range, m_vecLights[i]->range );
					pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( scale_targ * transl_pos * matr_old_world) );
					m_pSphere->DrawSubset( 0 );
				}

				pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

				if( m_vecLights[i]->type == LT_DIRECTED || m_vecLights[i]->type == LT_SPOT )
				{
					// prepare TARG transformation
					D3DXMatrixTranslation( &transl_targ, m_vecLights[i]->targ.x, m_vecLights[i]->targ.y, m_vecLights[i]->targ.z );
					D3DXMatrixScaling( &scale_targ, fIconScale * 3, fIconScale * 3, fIconScale * 3 );
					pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( scale_targ * transl_targ * matr_old_world) );

					pd3dDevice->SetMaterial( &m_WhiteMtrl );
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
					
					//render target sphere
					m_pSphere->DrawSubset( 0 );
				}
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCull );
	SetVColorFromMaterial( false, pd3dDevice );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );
}

void CSceneFile::RenderCurves( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !m_pCurves )
		return;

	SetVColorFromMaterial( true, pd3dDevice );
	pd3dDevice->SetMaterial( &m_WhiteMtrl );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	pd3dDevice->SetFVF( D3DFVF_VERTEX3D );

	D3DXVECTOR3* pData = m_pCurves;

	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		int vectecies_cnt = m_vecCurves[i]->nulls.size();

		if( vectecies_cnt > 1 )
		{
			if( m_vecCurves[i]->visible )
			{
				if( m_vecCurves[i]->selected )
				{
					pd3dDevice->SetMaterial( &m_RedMtrl );
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );	
				}
				else
				{
					pd3dDevice->SetMaterial( &m_WhiteMtrl );
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
				}

				if( m_vecCurves[i]->type == LINEAR )
					pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, vectecies_cnt - 1, pData, sizeof( D3DXVECTOR3 ) );
				else if( m_vecCurves[i]->type == CATMULROM )
					pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, ( vectecies_cnt - 1 ) * CURVE_STEP, pData, sizeof( D3DXVECTOR3 ) );
			}


			if( m_vecCurves[i]->type == LINEAR )
				pData += vectecies_cnt;
			else if( m_vecCurves[i]->type == CATMULROM )
				pData += ( vectecies_cnt - 1 ) * CURVE_STEP + 1;
		}
	}
}

void CSceneFile::DeleteAllCurves()
{
	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		SAFE_DELETE( m_vecCurves[i] );
	}

	SAFE_DELETE_ARRAY( m_pCurves );

	m_vecCurves.clear();
}

void CSceneFile::ShowEverything( bool bValue, LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( bValue )
	{
		ShowAllCameras( true );
		ShowAllLights( true );
		((CEfnMeshlist*)m_pMeshes)->ShowAllMeshes( true );
		((CEfnMeshlist*)m_pMeshes)->ShowMeshes( "Nulls", true );	
	}
	else
	{	
		ShowAllCameras( false );
		ShowAllLights( false );
		((CEfnMeshlist*)m_pMeshes)->ShowAllMeshes( false );
		((CEfnMeshlist*)m_pMeshes)->ShowMeshes( "Nulls", false );		
	}

	PrepareCamsLines( pd3dDevice );
	PrepareLightsLines( pd3dDevice );
}

bool CSceneFile::AddTextResource( str_t ID )
{
	if( GetTextResourceByName( ID ) != NULL )
		return false;

	TextResItem* item = new TextResItem( ID, "", "" );

	m_vecTextRes.push_back( item );

	return true;
}

TextResItem* CSceneFile::GetTextResourceByName( str_t ID )
{
	for( UINT i = 0; i < m_vecTextRes.size(); i++ )
	{
		if( m_vecTextRes[i]->id == ID )
			return m_vecTextRes[i];
	}

	return NULL;
}

TextResItem* CSceneFile::GetTextResourceByIndex( int index )
{
	if( index < 0 || m_vecTextRes.size() == 0 )
		return NULL;

	return m_vecTextRes[index];
}

void CSceneFile::DeleteTextResource( TextResItem* item )
{
	vector<TextResItem*>::iterator iter = m_vecTextRes.begin();

	for( UINT i = 0; i < m_vecTextRes.size(); i++ )
	{
		if( m_vecTextRes[i] == item )
		{
			SAFE_DELETE( item );
			m_vecTextRes.erase( iter );
			break;
		}

		iter++;
	}
}

TextResItem* CSceneFile::AddNewTextResource()
{
	str_t new_id = "ID_NEWRESOURCE";

	TextResItem*	temp		= NULL;
	CString			strTemp		= "";
	int				iCounter	= 0;

	temp = GetTextResourceByName( new_id );

	if( temp != NULL )
	{
		while( temp )
		{
			strTemp.Format( "%s_%i", new_id.data(), iCounter );
			temp = GetTextResourceByName( strTemp.GetBuffer() );
			iCounter++;
		}

		new_id = strTemp.GetBuffer();
	}

	if( AddTextResource( new_id ) )
	{
		return GetTextResourceByName( new_id );
	}
	else
		return NULL;
}

float CSceneFile::IntersectByRay( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation, enum OBJECT_TYPES& objType, str_t& strObjID )
{
	StrVec			cameras, lights;
	vector< float >	distances_cm;
	vector< float >	distances_lt;
	float			temp_cm	= 999999.0f;
	int				idx_cm	= -1;
	float			temp_lt	= 999999.0f;
	int				idx_lt	= -1;
	int				isected_object_type = 0;
	D3DXMATRIX		mLightTransformation, transl, scale;

	if( !m_pLightBulb || !m_pCamBox || !m_pMeshes )
		return -1.0f;

	/*****************************/
	/*		intersect lights	 */
	/*****************************/
	// oobb and bsphere intersections
	for( UINT i = 0; i < m_vecLights.size(); i++ )
	{
		if( m_vecLights[i]->visible == false )
			continue;

		D3DXMatrixTranslation( &transl, m_vecLights[i]->pos.x, m_vecLights[i]->pos.y, m_vecLights[i]->pos.z );
		float icon_scale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();
		D3DXMatrixScaling( &scale, icon_scale, icon_scale, icon_scale );
		mLightTransformation = scale * transl * mTransformation;

		if( m_pLightBulb->GetType() == FT_EFN )
		{
			if( ((CEfnCharacter*)m_pLightBulb)->IntersectOOBB( pFrom, pTo, mLightTransformation ) && ((CEfnCharacter*)m_pLightBulb)->IntersectBSphere( pFrom, pTo, mLightTransformation ) )
				lights.push_back( m_vecLights[i]->name.data() );
		}
		else if( m_pLightBulb->GetType() == FT_EFN_MESHLIST )
		{
			CMesh* pMesh = ((CEfnMeshlist*)m_pLightBulb)->GetMeshByIndex( 0 );

			if( pMesh )
			{
				if( pMesh->IntersectOOBB( pFrom, pTo, mLightTransformation ) && pMesh->IntersectBSphere( pFrom, pTo, mLightTransformation ) )
					lights.push_back( m_vecLights[i]->name.data() );
			}
		}
		else
		{}
	}

	// poly intersections
	for( unsigned int i = 0; i < lights.size(); i++ )
	{
		float dist = -1.0f;

		CLIGHT* light = GetLightByID( lights[i] );

		D3DXMatrixTranslation( &transl, light->pos.x, light->pos.y, light->pos.z );
		float icon_scale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();
		D3DXMatrixScaling( &scale, icon_scale, icon_scale, icon_scale );
		mLightTransformation = scale * transl * mTransformation;


		if( m_pLightBulb->GetType() == FT_EFN )
		{
			dist = ((CEfnCharacter*)m_pLightBulb)->IntersectPolys( pFrom, pTo, mLightTransformation );
		}
		else if( m_pLightBulb->GetType() == FT_EFN_MESHLIST )
		{
			CMesh* pMesh = ((CEfnMeshlist*)m_pLightBulb)->GetMeshByIndex( 0 );

			if( pMesh )
				dist = pMesh->IntersectPolys( pFrom, pTo, mLightTransformation );
		}

		distances_lt.push_back( dist );
	}

	// closest intersection
	for( unsigned int i = 0; i < distances_lt.size(); i++ )
	{
		if( distances_lt[i] != -1.0f && distances_lt[i] < temp_lt )
		{
			temp_lt	= distances_lt[i];
			idx_lt	= i;
		}
	}



	/*****************************/
	/*		intersect cams		 */
	/*****************************/
	// oobb and bsphere intersections
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		if( m_vecCameras[i]->visible == false )
			continue;

		float angX, angY;
		D3DXMATRIX	mat_X, mat_Y;
		CalcAzimuthAnglesFromVec( &m_vecCameras[i]->pos, &m_vecCameras[i]->targ, angX, angY );

		D3DXMatrixRotationY( &mat_Y, angY + D3DX_PI );
		D3DXMatrixRotationX( &mat_X, angX );

		D3DXMatrixTranslation( &transl, m_vecCameras[i]->pos.x, m_vecCameras[i]->pos.y, m_vecCameras[i]->pos.z );
		float icon_scale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();
		D3DXMatrixScaling( &scale, icon_scale, icon_scale, icon_scale );
		mLightTransformation = scale * mat_X * mat_Y * transl * mTransformation;

		if( m_pCamBox->GetType() == FT_EFN )
		{
			if( ((CEfnCharacter*)m_pCamBox)->IntersectOOBB( pFrom, pTo, mLightTransformation ) && ((CEfnCharacter*)m_pCamBox)->IntersectBSphere( pFrom, pTo, mLightTransformation ) )
				cameras.push_back( m_vecCameras[i]->name.data() );
		}
		else if( m_pCamBox->GetType() == FT_EFN_MESHLIST )
		{
			CMesh* pMesh = ((CEfnMeshlist*)m_pCamBox)->GetMeshByIndex( 0 );

			if( pMesh )
			{
				if( pMesh->IntersectOOBB( pFrom, pTo, mLightTransformation ) && pMesh->IntersectBSphere( pFrom, pTo, mLightTransformation ) )
					cameras.push_back( m_vecCameras[i]->name.data() );
			}
		}
		else
		{}
	}

	// poly intersections
	for( unsigned int i = 0; i < cameras.size(); i++ )
	{
		float dist = -1.0f;

		CCAMERA2* camera = GetCameraByID( cameras[i] );

		float angX, angY;
		D3DXMATRIX	mat_X, mat_Y;
		CalcAzimuthAnglesFromVec( &camera->pos, &camera->targ, angX, angY );

		D3DXMatrixRotationY( &mat_Y, angY + D3DX_PI );
		D3DXMatrixRotationX( &mat_X, angX );

		D3DXMatrixTranslation( &transl, camera->pos.x, camera->pos.y, camera->pos.z );
		float icon_scale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();
		D3DXMatrixScaling( &scale, icon_scale, icon_scale, icon_scale );
		mLightTransformation = scale * mat_X * mat_Y * transl * mTransformation;


		if( m_pCamBox->GetType() == FT_EFN )
		{
			dist = ((CEfnCharacter*)m_pCamBox)->IntersectPolys( pFrom, pTo, mLightTransformation );
		}
		else if( m_pCamBox->GetType() == FT_EFN_MESHLIST )
		{
			CMesh* pMesh = ((CEfnMeshlist*)m_pCamBox)->GetMeshByIndex( 0 );

			if( pMesh )
				dist = pMesh->IntersectPolys( pFrom, pTo, mLightTransformation );
		}

		distances_cm.push_back( dist );
	}

	// closest intersection
	for( unsigned int i = 0; i < distances_cm.size(); i++ )
	{
		if( distances_cm[i] != -1.0f && distances_cm[i] < temp_cm )
		{
			temp_cm	= distances_cm[i];
			idx_cm	= i;
		}
	}

	
	enum	OBJECT_TYPES _objType;
	str_t	_strObjID;

	float misc_dist = ((CEfnMeshlist*)m_pMeshes)->IntersectByRay( pFrom, pTo, mTransformation, _objType, _strObjID );


	bool light_isected	= false;
	bool camera_isected	= false;
	bool misc_isected	= false;

	if( cameras.size() > 0 && idx_cm != -1 )
		camera_isected = true;

	if( lights.size() > 0 && idx_lt != -1 )
		light_isected = true;

	if( _objType != OBJ_UNDEFINED )
		misc_isected = true;


	float closest = 9999999;

	if( light_isected && temp_lt < closest )
		closest = temp_lt;

	if( camera_isected && temp_cm < closest )
		closest = temp_cm;

	if( misc_isected && misc_dist < closest )
		closest = misc_dist;


	if( closest == temp_lt )
	{
		strObjID	= lights[idx_lt];
		objType		= OBJ_LIGHT;
		return temp_lt;		
	}
	else if( closest == temp_cm )
	{
		strObjID	= cameras[idx_cm];
		objType		= OBJ_CAMERA;
		return temp_cm;	
	}
	else if( closest == misc_dist )
	{
		strObjID	= _strObjID;
		objType		= _objType;
		return misc_dist;
	}
	else
	{
		strObjID	= "";
		objType		= OBJ_UNDEFINED;
		return -1.0f;
	}
}

bool CSceneFile::PrepareGridDataSelectors( vector<float>* TriMeshDims, vector<float>* MeshDims, StrVec* FloorMeshes, float fCellSize )
{
	bool bErrHappened = false;
	int tri_offset = 0;

	for( unsigned int i = 0; i < FloorMeshes->size(); i++ )
	{
		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*FloorMeshes)[i] );

		int tri_count = pMesh->GetIndicesCount() / 3;

		float fMinX_mesh = 999999, fMinZ_mesh = 999999, fMaxX_mesh = -999999, fMaxZ_mesh = -999999;

		for( int  j = 0; j < tri_count; j++ )
		{
			CVERTEX tA = pMesh->GetVertecies()[ pMesh->GetIndices()[ j * 3 + 0 ] ];
			CVERTEX tB = pMesh->GetVertecies()[ pMesh->GetIndices()[ j * 3 + 1 ] ];
			CVERTEX tC = pMesh->GetVertecies()[ pMesh->GetIndices()[ j * 3 + 2 ] ];

			D3DXVECTOR3 A( tA.x, tA.y, tA.z );
			D3DXVECTOR3 B( tB.x, tB.y, tB.z );
			D3DXVECTOR3 C( tC.x, tC.y, tC.z );

			float	fMinX, fMinZ, fMaxX, fMaxZ;

			//find min-max borders of tri
			fMinX = tA.x < tB.x ? tA.x : tB.x;
			fMinX = tC.x < fMinX ? tC.x : fMinX;

			fMinZ = tA.z < tB.z ? tA.z : tB.z;
			fMinZ = tC.z < fMinZ ? tC.z : fMinZ;

			fMaxX = tA.x > tB.x ? tA.x : tB.x;
			fMaxX = tC.x > fMaxX ? tC.x : fMaxX;

			fMaxZ = tA.z > tB.z ? tA.z : tB.z;
			fMaxZ = tC.z > fMaxZ ? tC.z : fMaxZ;

			fMinX_mesh = fMinX < fMinX_mesh ? fMinX : fMinX_mesh;
			fMinZ_mesh = fMinZ < fMinZ_mesh ? fMinZ : fMinZ_mesh;
			fMaxX_mesh = fMaxX > fMaxX_mesh ? fMaxX : fMaxX_mesh;
			fMaxZ_mesh = fMaxZ > fMaxZ_mesh ? fMaxZ : fMaxZ_mesh;

			TriMeshDims[ tri_offset + j ].push_back( fMinX );
			TriMeshDims[ tri_offset + j ].push_back( fMinZ );
			TriMeshDims[ tri_offset + j ].push_back( fMaxX );
			TriMeshDims[ tri_offset + j ].push_back( fMaxZ );
		}

		MeshDims[i].push_back( fMinX_mesh - fCellSize );
		MeshDims[i].push_back( fMinZ_mesh - fCellSize );
		MeshDims[i].push_back( fMaxX_mesh + fCellSize );
		MeshDims[i].push_back( fMaxZ_mesh + fCellSize );

		tri_offset += tri_count;
	}

	return !bErrHappened;
}

bool CSceneFile::PrepareGridDataFloors( vector<int>* MeshIndexes, vector<float>* TriMeshDims, vector<bool>*	TriMeshEdges, vector<float>* MeshDims, StrVec* FloorMeshes, float fCellSize )
{
	bool	bErrHappened	= false;
	int		tri_offset		= 0;
	vector<D3DXVECTOR3> vFloorsEdge;

	for( unsigned int i = 0; i < FloorMeshes->size(); i++ )
	{
		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*FloorMeshes)[i] );

		//find silouette edges
		for( int k = 0; k < pMesh->GetIndicesCount() / 3; k++ )
		{
			int vA = pMesh->GetIndices()[k * 3 + 0];
			int vB = pMesh->GetIndices()[k * 3 + 1];
			int vC = pMesh->GetIndices()[k * 3 + 2];

			bool similar_edgeA_found = false;
			bool similar_edgeB_found = false;
			bool similar_edgeC_found = false;

			for( int m = 0; m < pMesh->GetIndicesCount() / 3; m++ )
			{
				if( k == m )
					continue;
			
				int vA2 = pMesh->GetIndices()[m * 3 + 0];
				int vB2 = pMesh->GetIndices()[m * 3 + 1];
				int vC2 = pMesh->GetIndices()[m * 3 + 2];

				if( !similar_edgeA_found )
				{
					if( ( vA == vB2 && vB == vA2 ) || 
						( vA == vC2 && vB == vB2 ) ||
						( vA == vA2 && vB == vC2 ) )
					{
						similar_edgeA_found = true;
					}
				}

				if( !similar_edgeB_found )
				{
					if( ( vB == vB2 && vC == vA2 ) || 
						( vB == vC2 && vC == vB2 ) ||
						( vB == vA2 && vC == vC2 ) )
					{
						similar_edgeB_found = true;
					}
				}

				if( !similar_edgeC_found )
				{
					if( ( vC == vB2 && vA == vA2 ) || 
						( vC == vC2 && vA == vB2 ) ||
						( vC == vA2 && vA == vC2 ) )
					{
						similar_edgeC_found = true;
					}
				}
			}

			if( !similar_edgeA_found )
			{
				D3DXVECTOR3 temp( pMesh->GetVertecies()[vA].x, pMesh->GetVertecies()[vA].y, pMesh->GetVertecies()[vA].z ), 
							temp2( pMesh->GetVertecies()[vB].x, pMesh->GetVertecies()[vB].y, pMesh->GetVertecies()[vB].z );
				vFloorsEdge.push_back( temp );
				vFloorsEdge.push_back( temp2 );
			}

			if( !similar_edgeB_found )
			{
				D3DXVECTOR3 temp( pMesh->GetVertecies()[vB].x, pMesh->GetVertecies()[vB].y, pMesh->GetVertecies()[vB].z ), 
							temp2( pMesh->GetVertecies()[vC].x, pMesh->GetVertecies()[vC].y, pMesh->GetVertecies()[vC].z );
				vFloorsEdge.push_back( temp );
				vFloorsEdge.push_back( temp2 );
			}

			if( !similar_edgeC_found )
			{
				D3DXVECTOR3 temp( pMesh->GetVertecies()[vC].x, pMesh->GetVertecies()[vC].y, pMesh->GetVertecies()[vC].z ), 
							temp2( pMesh->GetVertecies()[vA].x, pMesh->GetVertecies()[vA].y, pMesh->GetVertecies()[vA].z );
				vFloorsEdge.push_back( temp );
				vFloorsEdge.push_back( temp2 );
			}

			TriMeshEdges->push_back( similar_edgeA_found );
			TriMeshEdges->push_back( similar_edgeB_found );
			TriMeshEdges->push_back( similar_edgeC_found );
		}
	}

	SAFE_DELETE_ARRAY( m_pFloorsEdge );

	m_pFloorsEdge = new D3DXVECTOR3[vFloorsEdge.size()];

	for( UINT i = 0; i < vFloorsEdge.size(); i++ )
		m_pFloorsEdge[i] = vFloorsEdge[i];

	m_iEdgesCount = vFloorsEdge.size();

	tri_offset = 0;

	for( unsigned int i = 0; i < FloorMeshes->size(); i++ )
	{

		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*FloorMeshes)[i] );
		
		//find mesh 2 scene bind idx
		int idx = -1;

		//get list of floor meshes
		ListStrVec_iter lst_iter = m_lBinds.begin();

		while( lst_iter != m_lBinds.end() )
		{
			StrVec	strVec = *lst_iter;

			if( strVec[2] == "Scene" && strVec[3] == (*FloorMeshes)[i] && strVec[4] == "Floor"  )
			{
				idx = atoi( strVec[0].data() );
				break;
			}

			lst_iter++;
		}

		// in case no bind found, skip mesh
		if( idx == -1 )
		{
			CString strErrorMsg;
			strErrorMsg.Format( "Не найден бинд для меша пола с ID: %s!", (*FloorMeshes)[i].data() );
			MessageBox( NULL, strErrorMsg, "Critical Error!", MB_OK | MB_ICONERROR );
			bErrHappened = true;

			continue;
		}

		MeshIndexes->push_back( idx );

		int tri_count = pMesh->GetIndicesCount() / 3;

		float fMinX_mesh = 999999, fMinZ_mesh = 999999, fMaxX_mesh = -999999, fMaxZ_mesh = -999999;

		for( int  j = 0; j < tri_count; j++ )
		{
			CVERTEX tA = pMesh->GetVertecies()[ pMesh->GetIndices()[ j * 3 + 0 ] ];
			CVERTEX tB = pMesh->GetVertecies()[ pMesh->GetIndices()[ j * 3 + 1 ] ];
			CVERTEX tC = pMesh->GetVertecies()[ pMesh->GetIndices()[ j * 3 + 2 ] ];

			D3DXVECTOR3 A( tA.x, tA.y, tA.z );
			D3DXVECTOR3 B( tB.x, tB.y, tB.z );
			D3DXVECTOR3 C( tC.x, tC.y, tC.z );

			float	fMinX, fMinZ, fMaxX, fMaxZ;

			//find min-max borders of tri
			fMinX = tA.x < tB.x ? tA.x : tB.x;
			fMinX = tC.x < fMinX ? tC.x : fMinX;

			fMinZ = tA.z < tB.z ? tA.z : tB.z;
			fMinZ = tC.z < fMinZ ? tC.z : fMinZ;

			fMaxX = tA.x > tB.x ? tA.x : tB.x;
			fMaxX = tC.x > fMaxX ? tC.x : fMaxX;

			fMaxZ = tA.z > tB.z ? tA.z : tB.z;
			fMaxZ = tC.z > fMaxZ ? tC.z : fMaxZ;

			fMinX_mesh = fMinX < fMinX_mesh ? fMinX : fMinX_mesh;
			fMinZ_mesh = fMinZ < fMinZ_mesh ? fMinZ : fMinZ_mesh;
			fMaxX_mesh = fMaxX > fMaxX_mesh ? fMaxX : fMaxX_mesh;
			fMaxZ_mesh = fMaxZ > fMaxZ_mesh ? fMaxZ : fMaxZ_mesh;

			TriMeshDims[ tri_offset + j ].push_back( fMinX );
			TriMeshDims[ tri_offset + j ].push_back( fMinZ );
			TriMeshDims[ tri_offset + j ].push_back( fMaxX );
			TriMeshDims[ tri_offset + j ].push_back( fMaxZ );
		}

		MeshDims[i].push_back( fMinX_mesh - fCellSize );
		MeshDims[i].push_back( fMinZ_mesh - fCellSize );
		MeshDims[i].push_back( fMaxX_mesh + fCellSize );
		MeshDims[i].push_back( fMaxZ_mesh + fCellSize );

		tri_offset += tri_count;
	}

	return !bErrHappened;
}

void CSceneFile::CalcAzimuthAnglesFromVec( D3DXVECTOR3* from, D3DXVECTOR3* to, float& angX, float& angY, bool bLightHack )
{
	//render cam icon
	D3DXVECTOR3 lt_vecY( 0, 0, 1 );
	D3DXVECTOR3 lt_vecX( -1, 0, 0 );
	D3DXVECTOR3 lt_vec = *to - *from;
	D3DXVECTOR3 lt_vec_temp;
	D3DXMATRIX	mat_back_rot;

	float length = D3DXVec3Length( &lt_vec );

	lt_vec.y = 0;
	D3DXVec3Normalize( &lt_vec, &lt_vec );
	
	angY = acos( ClampFloat( D3DXVec3Dot( &lt_vec, &lt_vecY ) ) );
	
	D3DXMatrixRotationY( &mat_back_rot, angY );
	D3DXVec3TransformCoord( &lt_vec, &lt_vec, &mat_back_rot );

	lt_vec_temp = lt_vec - lt_vecY;
	float f = D3DXVec3Length( &lt_vec_temp );


	if( f < 0.000001 )
		angY = -angY;

	lt_vec = *to - *from;
	D3DXMatrixRotationY( &mat_back_rot, angY );
	D3DXVec3TransformCoord( &lt_vec, &lt_vec, &mat_back_rot );

	D3DXVec3Normalize( &lt_vec, &lt_vec );

	D3DXVec3TransformCoord( &lt_vec, &lt_vec, &mat_back_rot );

	lt_vecX = lt_vec;
	lt_vecX.y = 0;

	D3DXVec3Normalize( &lt_vecX, &lt_vecX );

	angX = acos( ClampFloat( D3DXVec3Dot( &lt_vec, &lt_vecX ) ) );

	if( bLightHack )
	{
		if( fabs(lt_vec.y - sin(angX)) < EPSILON )
			angX = -angX;
	}
	else
	{
		if( fabs(lt_vec.y + sin(angX)) < EPSILON )
			angX = -angX;
	}
}

void CSceneFile::Scale( D3DXMATRIX mScale )
{
	if( m_vecCameras.empty() )
		return;

	D3DXVECTOR3 tmp;

	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		D3DXVec3TransformCoord( &m_vecCameras[i]->pos, &m_vecCameras[i]->pos, &mScale );
		D3DXVec3TransformCoord( &m_vecCameras[i]->targ, &m_vecCameras[i]->targ, &mScale );
	}
}

void CSceneFile::RenderCams( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	DWORD dwCull;
	pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCull );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//store global transform
	D3DXMATRIX matr_old_world;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );

	DWORD amb;
	pd3dDevice->GetRenderState( D3DRS_AMBIENT, &amb );

	SetVColorFromMaterial( true, pd3dDevice );

	if( m_vecCameras.size() > 0 && m_pCamBox )
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

		D3DXMATRIX transl_pos, scale_pos, transl_targ, scale_targ;
		float fIconScale = 1.0f;

		//get icon scale
		if( m_pParentWnd )
			fIconScale = ((CCookieMachineDlg*)m_pParentWnd)->GetIconsScale();

		//render icons
		for( UINT i = 0; i < m_vecCameras.size(); i++ )
		{
			if( m_vecCameras[i]->visible == false )
			{
				int h = 0;
			}

			if( m_vecCameras[i]->visible )
			{
				if( m_iSelectedCamera == i )
				{
					pd3dDevice->SetMaterial( &m_RedMtrl );
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );
				}
				else
				{
					pd3dDevice->SetMaterial( &m_BlueMtrl );
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff0000ff );
				}
				
				//render cam lines
				pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );
				pd3dDevice->SetStreamSource( 0, m_pVBCamLines, 0, sizeof(CVERTEX) );
				pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
				pd3dDevice->DrawPrimitive( D3DPT_LINELIST, i * 2, 1 );

				float angX, angY;
				D3DXMATRIX	mat_X, mat_Y;
				CalcAzimuthAnglesFromVec( &m_vecCameras[i]->pos, &m_vecCameras[i]->targ, angX, angY );

				D3DXMatrixRotationY( &mat_Y, angY + D3DX_PI );
				D3DXMatrixRotationX( &mat_X, angX );


				// prepare POS transformation
				D3DXMatrixTranslation( &transl_pos, m_vecCameras[i]->pos.x, m_vecCameras[i]->pos.y, m_vecCameras[i]->pos.z );
				

				if( m_iSelectedCamera == i && ( m_bIsCamZoomNearActive || m_bIsCamZoomFarActive ) )
				{
					pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

					if( m_bIsCamZoomNearActive )
						D3DXMatrixScaling( &scale_pos, m_vecCameras[i]->zoom_near, m_vecCameras[i]->zoom_near, m_vecCameras[i]->zoom_near );

					if( m_bIsCamZoomFarActive )
						D3DXMatrixScaling( &scale_pos, m_vecCameras[i]->zoom_far, m_vecCameras[i]->zoom_far, m_vecCameras[i]->zoom_far );

					pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( scale_pos * transl_pos * matr_old_world) );
					m_pSphere->DrawSubset( 0 );
				}

				pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				D3DXMatrixScaling( &scale_pos, fIconScale, fIconScale, fIconScale );
				pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( scale_pos * mat_X * mat_Y * transl_pos * matr_old_world) );

				pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				pd3dDevice->LightEnable( 0, true );

				//render cam icon
				((CEfnCharacter*)m_pCamBox)->Render( pd3dDevice, false, 0.0f );

				// prepare TARG transformation
				D3DXMatrixTranslation( &transl_targ, m_vecCameras[i]->targ.x, m_vecCameras[i]->targ.y, m_vecCameras[i]->targ.z );
				D3DXMatrixScaling( &scale_targ, fIconScale * 3, fIconScale * 3, fIconScale * 3 );
				pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( scale_targ * transl_targ * matr_old_world) );

				pd3dDevice->SetMaterial( &m_WhiteMtrl );
				pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

				//render target sphere
				m_pSphere->DrawSubset( 0 );
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCull );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, amb );
	SetVColorFromMaterial( false, pd3dDevice );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );
}

CCAMERA2* CSceneFile::GetCameraByID( str_t strCamID )
{
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		if( m_vecCameras[i]->name == strCamID )
			return m_vecCameras[i];
	}

	return NULL;
}

CLIGHT* CSceneFile::GetLightByID( str_t strLightID )
{
	for( UINT i = 0; i < m_vecLights.size(); i++ )
	{
		if( m_vecLights[i]->name == strLightID )
			return m_vecLights[i];
	}

	return NULL;
}

void CSceneFile::SetSelectedCamera( str_t strCamID )
{
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
	{
		if( m_vecCameras[i]->name == strCamID )
		{
			m_iSelectedCamera = i;
			break;
		}
	}
}

void CSceneFile::SetSelectedLight( str_t strLightID )
{
	for( UINT i = 0; i < m_vecLights.size(); i++ )
	{
		if( m_vecLights[i]->name == strLightID )
		{
			m_iSelectedLight = i;
			break;
		}
	}
}

void CSceneFile::DeleteGrid()
{
	if( m_pppGrid )
	{
		for( int i = 0; i < m_iXGridSize; i++ )
		{
			for( int j = 0; j < m_iZGridSize; j++ )
			{
				SAFE_DELETE_ARRAY( m_pppGrid[i][j] );
			}

			SAFE_DELETE_ARRAY( m_pppGrid[i] );
		}

		SAFE_DELETE_ARRAY( m_pppGrid );
	}
}

void CSceneFile::RecalcGridUV()
{
	m_vMin = D3DXVECTOR2( 999999, 999999 );
	m_vMax = D3DXVECTOR2( -999999, -999999 );

	StrVec		FloorMeshes;
	StrVec		CamSelectors;

	//get list of floor meshes
	ListStrVec_iter lst_iter = m_lBinds.begin();

	while( lst_iter != m_lBinds.end() )
	{
		StrVec	strVec = *lst_iter;

		if( strVec[2] == "Scene" && strVec[4] == "Floor" )
			FloorMeshes.push_back( strVec[3] );

		lst_iter++;
	}

	if( FloorMeshes.size() < 1 )
	{
		MessageBox( NULL, "Floor meshes are not defined! \n\rPlease, make sure that nesessary binds are present.", "Warning!", MB_OK | MB_ICONEXCLAMATION );	
		return;
	}

	//get list of floor camera selectors
	lst_iter = m_lBinds.begin();

	while( lst_iter != m_lBinds.end() )
	{
		StrVec	strVec = *lst_iter;

		if( strVec[2] == "Camera" && strVec[4] == "CameraSelect" )
			CamSelectors.push_back( strVec[3] );

		lst_iter++;
	}

	if( CamSelectors.size() < 1 )
	{
		MessageBox( NULL, "Camera selectors are not defined! \n\rPlease, make sure that nesessary binds are present.", "Warning!", MB_OK | MB_ICONEXCLAMATION );	
		return;
	}

	// find min and max of the floor
	for( unsigned  int i = 0; i < FloorMeshes.size(); i++ )
	{
		CMesh* mesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( FloorMeshes[i] );

		for( int j = 0; j < mesh->GetVerteciesCount(); j++ )
		{
			if( mesh->GetVertecies()[j].x > m_vMax.x )
				m_vMax.x = mesh->GetVertecies()[j].x;

			if( mesh->GetVertecies()[j].z > m_vMax.y )
				m_vMax.y = mesh->GetVertecies()[j].z;

			if( mesh->GetVertecies()[j].x < m_vMin.x )
				m_vMin.x = mesh->GetVertecies()[j].x;

			if( mesh->GetVertecies()[j].z < m_vMin.y )
				m_vMin.y = mesh->GetVertecies()[j].z;
		}
	}

	
	// grid border
	m_vMin.x		-=		m_fCellSize;
	m_vMin.y		-=		m_fCellSize;
	m_vMax.x		+=	m_fCellSize;
	m_vMax.y		+=	m_fCellSize;

	
	// amount of cells fitting in width and height
	m_iXGridSize	= (int)fabs( ( m_vMax.x - m_vMin.x ) / m_fCellSize ) + 1;
	m_iZGridSize	= (int)fabs( ( m_vMax.y - m_vMin.y ) / m_fCellSize ) + 1;

	m_vMax.x		= m_vMin.x  + m_fCellSize * m_iXGridSize;
	m_vMax.y		= m_vMin.y  + m_fCellSize * m_iZGridSize;


	//create uv values for floors
	D3DXVECTOR2 FloorDim = m_vMax - m_vMin;

	for( unsigned int i = 0; i < FloorMeshes.size(); i++ )
	{
		CMesh* mesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( FloorMeshes[i] );

		for( int j = 0; j < mesh->GetVerteciesCount(); j++ )
		{
			float xV = mesh->GetVertecies()[j].x - m_vMin.x;
			float zV = mesh->GetVertecies()[j].z - m_vMin.y;

			mesh->GetVertecies()[j].u		= xV / FloorDim.x;
			mesh->GetVertecies()[j].v		= zV / FloorDim.y;
			mesh->GetVertecies()[j].color	= 0x33000000;
		}
	}
}

//////////////////////////////
//		CREATE GRID			//
//////////////////////////////
void CSceneFile::RecalcGrid()
{
	m_vMin = D3DXVECTOR2( 999999, 999999 );
	m_vMax = D3DXVECTOR2( -999999, -999999 );

	StrVec		FloorMeshes;
	StrVec		CamSelectors;

	//get list of floor meshes
	ListStrVec_iter lst_iter = m_lBinds.begin();

	while( lst_iter != m_lBinds.end() )
	{
		StrVec	strVec = *lst_iter;

		if( strVec[2] == "Scene" && strVec[4] == "Floor" )
			FloorMeshes.push_back( strVec[3] );

		lst_iter++;
	}

	if( FloorMeshes.size() < 1 )
	{
		MessageBox( NULL, "Floor meshes are not defined! \n\rPlease, make sure that nesessary binds are present.", "Warning!", MB_OK | MB_ICONEXCLAMATION );	
		return;
	}

	//get list of floor camera selectors
	lst_iter = m_lBinds.begin();

	while( lst_iter != m_lBinds.end() )
	{
		StrVec	strVec = *lst_iter;

		if( strVec[2] == "Camera" && strVec[4] == "CameraSelect" )
		{
			str_t f = strVec[3];
			CamSelectors.push_back( strVec[3] );
		}

		lst_iter++;
	}

	if( CamSelectors.size() < 1 )
	{
		MessageBox( NULL, "Camera selectors are not defined! \n\rPlease, make sure that nesessary binds are present.", "Warning!", MB_OK | MB_ICONEXCLAMATION );	
		//return;
	}
	
	DeleteGrid();



	// find min and max of the floor
	for( unsigned  int i = 0; i < FloorMeshes.size(); i++ )
	{
		CMesh* mesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( FloorMeshes[i] );

		for( int j = 0; j < mesh->GetVerteciesCount(); j++ )
		{
			if( mesh->GetVertecies()[j].x > m_vMax.x )
				m_vMax.x = mesh->GetVertecies()[j].x;

			if( mesh->GetVertecies()[j].z > m_vMax.y )
				m_vMax.y = mesh->GetVertecies()[j].z;

			if( mesh->GetVertecies()[j].x < m_vMin.x )
				m_vMin.x = mesh->GetVertecies()[j].x;

			if( mesh->GetVertecies()[j].z < m_vMin.y )
				m_vMin.y = mesh->GetVertecies()[j].z;
		}
	}

	
	// grid border
	m_vMin.x		-=	m_fCellSize;
	m_vMin.y		-=	m_fCellSize;
	m_vMax.x		+=	m_fCellSize;
	m_vMax.y		+=	m_fCellSize;

	
	// amount of cells fitting in width and height
	m_iXGridSize	= (int)fabs( ( m_vMax.x - m_vMin.x ) / m_fCellSize ) + 1;
	m_iZGridSize	= (int)fabs( ( m_vMax.y - m_vMin.y ) / m_fCellSize ) + 1;

	m_vMax.x		= m_vMin.x  + m_fCellSize * m_iXGridSize;
	m_vMax.y		= m_vMin.y  + m_fCellSize * m_iZGridSize;


	//create uv values for floors
	D3DXVECTOR2 FloorDim = m_vMax - m_vMin;

	for( unsigned int i = 0; i < FloorMeshes.size(); i++ )
	{
		CMesh* mesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( FloorMeshes[i] );

		for( int j = 0; j < mesh->GetVerteciesCount(); j++ )
		{
			float xV = mesh->GetVertecies()[j].x - m_vMin.x;
			float zV = mesh->GetVertecies()[j].z - m_vMin.y;

			mesh->GetVertecies()[j].u		= xV / FloorDim.x;
			mesh->GetVertecies()[j].v		= zV / FloorDim.y;
			mesh->GetVertecies()[j].color	= 0x33000000;
		}
	}

	// create grid
	m_pppGrid = new int**[m_iXGridSize];

	for( int i = 0; i < m_iXGridSize; i++ )
		m_pppGrid[i] = new int*[m_iZGridSize];


	int*	tri_list	= NULL;
	int		tri_count	= 0;


	//create vector grid for floors
	vector<int>** pVecGrid = new vector<int>*[m_iXGridSize];

	for( int i = 0; i < m_iXGridSize; i++ )
		pVecGrid[i] = new vector<int>[m_iZGridSize];

	//create int grid for cam selects
	int** pCamSelGrid = new int*[m_iXGridSize];
	
	for( int i = 0; i < m_iXGridSize; i++ )
	{
		pCamSelGrid[i] = new int[m_iZGridSize];

		ZeroMemory( pCamSelGrid[i], sizeof( int ) * m_iZGridSize );
	}


	IntersectFloorMeshesWithGrid( &FloorMeshes, pVecGrid, &m_vMin, m_fCellSize );
	IntersectCamSelectorsWithGrid( pCamSelGrid, &CamSelectors, &m_vMin, m_fCellSize, m_iXGridSize, m_iZGridSize );

	//vector<int> trr;

	//copy vec grid data to actual grid
	for( int i = 0; i < m_iXGridSize; i++ )
	{
		for( int j = 0; j < m_iZGridSize; j++ )
		{
			int tris_datasize = pVecGrid[i][j].size();

			m_pppGrid[i][j]	= new int[ tris_datasize + 2 ];

			m_pppGrid[i][j][0] = pCamSelGrid[i][j];//CalcCamSelector( &CamSelectors, i, j, &m_vMin, m_fCellSize );
			m_pppGrid[i][j][1] = tris_datasize / 2;

			//if( tris_datasize / 2 > 0 )
			//	trr.push_back( tris_datasize / 2 );

			for( int u = 0; u < tris_datasize; u++ )
				m_pppGrid[i][j][u + 2] = pVecGrid[i][j][u];

			
		}
	}

	//int countTot	= m_iXGridSize * m_iZGridSize;
	//int	countOk		= 0;
	//int	countBad	= 0;

	//for( int i = 0; i < m_iXGridSize; i++ )
	//{
	//	for( int j = 0; j < m_iZGridSize; j++ )
	//	{
	//		if( m_pppGrid[i][j][1] > 0 )
	//			countOk++;
	//		else if( m_pppGrid[i][j][1] == 0 )
	//			countBad++;
	//		else
	//			AfxMessageBox( "Error floors recalc!" );
	//	}
	//}

	//CString strTempOut;
	//strTempOut.Format( "total: %d, walkable: %d, nonwalkable: %d", countTot, countOk, countBad );
	//AfxMessageBox( strTempOut );
	//int rezz = 0;
	//float rezz2 = 0.0f;
	//int max = -999;

	//for( int i = 0; i < trr.size(); i++ )
	//{
	//	rezz += trr[i];

	//	if( trr[i] > max )
	//		max = trr[i];
	//}

	//rezz2 = rezz / (float)trr.size();

	//CString fggg;
	//
	//fggg.Format( "%f", rezz2 );
	//AfxMessageBox( fggg );

	//fggg.Format( "%i", max );
	//AfxMessageBox( fggg );


	//delete vec grid
	for( int i = 0; i < m_iXGridSize; i++ )
		SAFE_DELETE_ARRAY( pVecGrid[i] );

	SAFE_DELETE_ARRAY( pVecGrid );

	//delete cam select grid
	for( int i = 0; i < m_iXGridSize; i++ )
		SAFE_DELETE_ARRAY( pCamSelGrid[i] );

	SAFE_DELETE_ARRAY( pCamSelGrid );
}

void CSceneFile::IntersectFloorMeshesWithGrid( StrVec* FloorMeshes, vector<int>** pGrid, D3DXVECTOR2* pvMin, float fCellSize )
{
	//go through floor meshes, get their bind indexes, and calc mins & maxes
	int iTotalTriCount = 0;

	for( unsigned int i = 0; i < FloorMeshes->size(); i++ )
	{
		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*FloorMeshes)[i] );

		iTotalTriCount += pMesh->GetIndicesCount() / 3;
	}

	int		progrs_val	= iTotalTriCount;
	float	step		= 0.0f;

	if( progrs_val > 100 )
		step = progrs_val / 100.0f;
	else
		step = 100.0f / progrs_val;

	float counter = 0.0f;
	int percent = 0;

	if( m_pProgressWnd )
	{
		((CValidateDlg*)m_pProgressWnd)->SetProgressRange( 0, 100 );
		((CValidateDlg*)m_pProgressWnd)->SetPos( 0 );
		((CValidateDlg*)m_pProgressWnd)->SetMessageStr( "Preparing grid data..." );
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrA( "Step 1 of 4" );
	}

	vector<int>		MeshIndexes;
	vector<bool>	TriMeshEdges;
	vector<float>*	TriMeshDims	= new vector<float>[iTotalTriCount];
	vector<float>*	MeshDims	= new vector<float>[FloorMeshes->size()];
	int				iTriOffset	= 0;
	StrMap			complete_cells_map;
	D3DXVECTOR3		cell_verts[4], cell_dir( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR2		_cell_verts[4];

	if( PrepareGridDataFloors( &MeshIndexes, TriMeshDims, &TriMeshEdges, MeshDims, FloorMeshes, fCellSize ) == false )
		return;

	/*int edg_cnt = 0;

	for( int i = 0; i < iTotalTriCount * 3; i++ )
	{
		if( TriMeshEdges[i] == false )
			edg_cnt++;
	}

	CString sgfd;
	sgfd.Format( "%i", edg_cnt );

	AfxMessageBox( sgfd );*/

	//collect cells that completely overlay floor meshes
	for( unsigned int msh = 0; msh < FloorMeshes->size(); msh++ )
	{
		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*FloorMeshes)[msh] );

		int	iTrisCount = pMesh->GetIndicesCount() / 3;

		for( int i = 0; i < iTrisCount; i++ )
		{
			CVERTEX tA = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 0 ] ];
			CVERTEX tB = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 1 ] ];
			CVERTEX tC = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 2 ] ];

			D3DXVECTOR3 A( tA.x, tA.y, tA.z );
			D3DXVECTOR3 B( tB.x, tB.y, tB.z );
			D3DXVECTOR3 C( tC.x, tC.y, tC.z );

			D3DXVECTOR2 patchMin;
			int			patchX, patchZ;
				
			int patchOffsetX = (int)( ( TriMeshDims[ iTriOffset + i ][0] - pvMin->x ) / fCellSize );
			int patchOffsetZ = (int)( ( TriMeshDims[ iTriOffset + i ][1] - pvMin->y ) / fCellSize );

			patchMin.x = pvMin->x + patchOffsetX * fCellSize;
			patchMin.y = pvMin->y + patchOffsetZ * fCellSize;

			patchX = (int)( ( ( TriMeshDims[ iTriOffset + i ][2] - patchMin.x ) / fCellSize ) ) + 1;
			patchZ = (int)( ( ( TriMeshDims[ iTriOffset + i ][3] - patchMin.y ) / fCellSize ) ) + 1;

			for( int u = 0; u < patchX; u++ )
			{
				for( int v = 0; v < patchZ; v++ )
				{
					////////////////////////////
					// PREPARE CELL VERTECIES //
					////////////////////////////
					float cell_x_min = patchMin.x + fCellSize * u;
					float cell_z_min = patchMin.y + fCellSize * v;

					cell_verts[0] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min );				//A
					cell_verts[1] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min );				//B
					cell_verts[2] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min + fCellSize );	//C
					cell_verts[3] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min + fCellSize );	//D

					/////////////////////////////////////
					// DO CELLVERT - TRI INTERSECTIONS //
					/////////////////////////////////////						
					/*** cell vertecies with tris ***/
					for( int j = 0; j < 4; j++ )
					{
						if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts[j], &cell_dir ) != -1.0f )
						{
							CString temp;
							temp.Format( "%i_%i", patchOffsetX + u, patchOffsetZ + v );

							complete_cells_map[temp.GetBuffer()] = "";

							break;
						}
					}


				}
			}


			if( m_pProgressWnd )
			{
				if( progrs_val > 100 )
				{
					if( counter < step )
						counter += 1.0f;
					else
					{
						counter = fabs( step - counter );
						((CValidateDlg*)m_pProgressWnd)->Step();
						counter += 1.0f;
						percent++;
					}
				}
				else
				{
					while( counter < step )
					{
						counter += 1.0f;
						percent++;
						((CValidateDlg*)m_pProgressWnd)->Step();
					}

					counter = fabs( step - counter );
				}

				CString prc;
				prc.Format( "%i%%", percent );
					
				((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( prc );
			}
		
		}

		iTriOffset += iTrisCount;
	}


	((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( "100%" );


	//prepare progress data
	progrs_val = complete_cells_map.size();

	if( progrs_val > 100 )
		step = progrs_val / 100.0f;
	else
		step = 100.0f / progrs_val;

	counter = 0.0f;
	percent = 0;

	if( m_pProgressWnd )
	{
		if( complete_cells_map.size() > 1000 )
			((CValidateDlg*)m_pProgressWnd)->SetProgressRange( 0, 100 );
		else
			((CValidateDlg*)m_pProgressWnd)->SetProgressRange( 0, complete_cells_map.size() );

		((CValidateDlg*)m_pProgressWnd)->SetPos( 0 );
		((CValidateDlg*)m_pProgressWnd)->SetMessageStr( "Calculating floors..." );
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrA( "Step 2 of 4" );
	}
	//end prepare progress data

	StrMap tris;

	for( StrMap_iter iter = complete_cells_map.begin(); iter != complete_cells_map.end(); ++iter )
	{
		str_t	cellX = iter->first.substr( 0, iter->first.find( "_" ) );
		str_t	cellZ = iter->first.substr( iter->first.find( "_" ) + 1, iter->first.length() - iter->first.find( "_" ) );
		int		iCellX = atoi( cellX.data() );
		int		iCellZ = atoi( cellZ.data() );
		bool	bVIsect[4];
		bool	bTopEdge	= false;
		bool	bBottomEdge = false;
		bool	bLeftEdge	= false;
		bool	bRightEdge	= false;
		bool	bEmptyEdge	= false;

		bVIsect[0] = false;
		bVIsect[1] = false;
		bVIsect[2] = false;
		bVIsect[3] = false;
		iTriOffset = 0;

		tris.clear();

		for( unsigned int msh = 0; msh < FloorMeshes->size(); msh++ )
		{
			//bTopEdge = false;

			CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*FloorMeshes)[msh] );

			int	iTrisCount = pMesh->GetIndicesCount() / 3;

			float cell_x_min = pvMin->x + fCellSize * iCellX;
			float cell_z_min = pvMin->y + fCellSize * iCellZ;

			
			//  ^ Z
			//  |    C-----D
			//  |    |     |
			//  |    |     |      <----- cell
			//  |    A-----B  
			//  |  
			//  |               X
			//  |--------------->

			cell_verts[0] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min );				//A
			cell_verts[1] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min );				//B
			cell_verts[2] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min + fCellSize );	//C
			cell_verts[3] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min + fCellSize );	//D

			_cell_verts[0] = D3DXVECTOR2( cell_x_min			, cell_z_min );				//A
			_cell_verts[1] = D3DXVECTOR2( cell_x_min + fCellSize, cell_z_min );				//B
			_cell_verts[2] = D3DXVECTOR2( cell_x_min			, cell_z_min + fCellSize );	//C
			_cell_verts[3] = D3DXVECTOR2( cell_x_min + fCellSize, cell_z_min + fCellSize );	//D


			if( cell_verts[0].x > MeshDims[msh][0] && cell_verts[0].x < MeshDims[msh][2] && cell_verts[0].z > MeshDims[msh][1] && cell_verts[0].z < MeshDims[msh][3] &&
				cell_verts[1].x > MeshDims[msh][0] && cell_verts[1].x < MeshDims[msh][2] && cell_verts[1].z > MeshDims[msh][1] && cell_verts[1].z < MeshDims[msh][3] &&
				cell_verts[2].x > MeshDims[msh][0] && cell_verts[2].x < MeshDims[msh][2] && cell_verts[2].z > MeshDims[msh][1] && cell_verts[2].z < MeshDims[msh][3] &&
				cell_verts[3].x > MeshDims[msh][0] && cell_verts[3].x < MeshDims[msh][2] && cell_verts[3].z > MeshDims[msh][1] && cell_verts[3].z < MeshDims[msh][3] )
			{
				for( int i = 0; i < iTrisCount; i++ )
				{
					CVERTEX tA = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 0 ] ];
					CVERTEX tB = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 1 ] ];
					CVERTEX tC = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 2 ] ];

					D3DXVECTOR3 A( tA.x, tA.y, tA.z );
					D3DXVECTOR3 B( tB.x, tB.y, tB.z );
					D3DXVECTOR3 C( tC.x, tC.y, tC.z );

					D3DXVECTOR2 _A( tA.x, tA.z );
					D3DXVECTOR2 _B( tB.x, tB.z );
					D3DXVECTOR2 _C( tC.x, tC.z );

					/////////////////////////////////////
					// DO CELLVERT - TRI INTERSECTIONS //
					/////////////////////////////////////						
					/*** cell vertecies with tris ***/
					for( int j = 0; j < 4; j++ )
					{
						if( cell_verts[j].x > TriMeshDims[ iTriOffset + i ][0] &&
							cell_verts[j].x < TriMeshDims[ iTriOffset + i ][2] &&
							cell_verts[j].z > TriMeshDims[ iTriOffset + i ][1] &&
							cell_verts[j].z < TriMeshDims[ iTriOffset + i ][3] )
						{
							if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts[j], &cell_dir ) != -1.0f )
								bVIsect[j] = true;

							CString val;
							val.Format( "%i_%i", MeshIndexes[msh], i );
							tris[ val.GetBuffer() ] = "";
						}
					}
				}
			}

			for( int i = 0; i < iTrisCount; i++ )
			{
				CVERTEX tA = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 0 ] ];
				CVERTEX tB = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 1 ] ];
				CVERTEX tC = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 2 ] ];

				D3DXVECTOR2 _A( tA.x, tA.z );
				D3DXVECTOR2 _B( tB.x, tB.z );
				D3DXVECTOR2 _C( tC.x, tC.z );

				//if( !bEmptyEdge )
				//{
				//	D3DXVECTOR2 isect;

				//	bool bEdge = CalcLineLineIntersection( &_cell_verts[2], &_cell_verts[3], &_A, &_B, &isect );
				//	if( bEdge && !TriMeshEdges[iTriOffset * 3 + i * 3 + 0] )
				//		bEmptyEdge = true;
				//}

				//if( !bEmptyEdge )
				//{
				//	D3DXVECTOR2 isect;

				//	bool bEdge = CalcLineLineIntersection( &_cell_verts[2], &_cell_verts[3], &_B, &_C, &isect );
				//	if( bEdge && !TriMeshEdges[iTriOffset * 3 + i * 3 + 1] )
				//		bEmptyEdge = true;
				//}

				//if( !bEmptyEdge )
				//{
				//	D3DXVECTOR2 isect;

				//	bool bEdge = CalcLineLineIntersection( &_cell_verts[2], &_cell_verts[3], &_C, &_A, &isect );
				//	if( bEdge && !TriMeshEdges[iTriOffset * 3 + i * 3 + 2] )
				//		bEmptyEdge = true;
				//}
			}
			
			iTriOffset += iTrisCount;
		}
		
		if( bVIsect[0] && bVIsect[1] && bVIsect[2] && bVIsect[3] )
		{
			for( StrMap_iter iter2 = tris.begin(); iter2 != tris.end(); ++iter2 )
			{
				str_t	strMesh	= iter2->first.substr( 0, iter2->first.find( "_" ) );
				str_t	strTri	= iter2->first.substr( iter2->first.find( "_" ) + 1, iter2->first.length() - iter2->first.find( "_" ) );
				int		iMesh	= atoi( strMesh.data() );
				int		iTri	= atoi( strTri.data() );
			
				pGrid[iCellX][iCellZ].push_back( iMesh );
				pGrid[iCellX][iCellZ].push_back( iTri );
			}
		}

		if( m_pProgressWnd )
		{
			if( progrs_val > 100 )
			{
				if( counter < step )
					counter += 1.0f;
				else
				{
					counter = fabs( step - counter );
					((CValidateDlg*)m_pProgressWnd)->Step();
					counter += 1.0f;
					percent++;
				}
			}
			else
			{
				while( counter < step )
				{
					counter += 1.0f;
					percent++;
					((CValidateDlg*)m_pProgressWnd)->Step();
				}

				counter = fabs( step - counter );
			}

			CString prc;
			prc.Format( "%i%%", percent );
				
			((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( prc );
		}

	}
	
	SAFE_DELETE_ARRAY( TriMeshDims );
	SAFE_DELETE_ARRAY( MeshDims );

	if( m_pProgressWnd )
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( "100%" );
}

CurveItem* CSceneFile::GetCurveByName( str_t strID )
{
	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		if( m_vecCurves[i]->id == strID )
			return m_vecCurves[i];
	}

	return NULL;
}

void CSceneFile::ShowAllCurves( bool bValue )
{
	for( UINT i = 0; i < m_vecCurves.size(); i++ )
		m_vecCurves[i]->visible = bValue;
}

void CSceneFile::AddNewCurve( str_t ID )
{
	CurveItem* crv = new CurveItem( ID );
	m_vecCurves.push_back( crv );
}

CurveItem* CSceneFile::GetCurveByIndex( int iIndex )
{
	if( iIndex >= 0 && iIndex < (int)m_vecCurves.size() )
		return m_vecCurves[iIndex];

	return NULL;
}

void CSceneFile::DeleteCurve( str_t strID )
{
	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		if( m_vecCurves[i]->id == strID )
		{
			SAFE_DELETE( m_vecCurves[i] );
			m_vecCurves.erase( m_vecCurves.begin() + i );
		}
	}
}

void CSceneFile::DeselectAllCurves()
{
	for( UINT i = 0; i < m_vecCurves.size(); i++ )
		m_vecCurves[i]->selected = false;
}

void CSceneFile::ShowGrid( bool bShow )
{ 
	m_bGridSelectorsVisible		= false;
	m_bGridVisible				= bShow;

	((CEfnMeshlist*)m_pMeshes)->EnableMaterials( !bShow );
}

void CSceneFile::ShowGridSelectors( bool bShow )
{
	m_bGridVisible				= false;
	m_bGridSelectorsVisible		= bShow;

	((CEfnMeshlist*)m_pMeshes)->EnableMaterials( !bShow );
}

void CSceneFile::IntersectCamSelectorsWithGrid( int** pCamSelGrid, StrVec* pSelectorNames, D3DXVECTOR2* pvMin, float fCellSize, int iGridMaxX, int iGridMaxZ )
{
	//go through floor meshes, get their bind indexes, and calc mins & maxes
	int iTotalTriCount = 0;

	for( unsigned int i = 0; i < pSelectorNames->size(); i++ )
	{
		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*pSelectorNames)[i] );

		if( !pMesh )
		{
			CString strErrMsg;
			strErrMsg.Format( "CamSelect \"%s\" was not found!", ((*pSelectorNames)[i]).data() );

			MessageBox( NULL, strErrMsg, "Error!", MB_OK | MB_ICONERROR );

			return;
		}

		iTotalTriCount += pMesh->GetIndicesCount() / 3;
	}

	int		progrs_val	= iTotalTriCount;
	float	step		= 0.0f;

	if( progrs_val > 100 )
		step = progrs_val / 100.0f;
	else
		step = 100.0f / progrs_val;

	float	counter = 0.0f;
	int		percent = 0;

	if( m_pProgressWnd )
	{
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( "" );
		((CValidateDlg*)m_pProgressWnd)->SetProgressRange( 0, 100 );
		((CValidateDlg*)m_pProgressWnd)->SetPos( 0 );
		((CValidateDlg*)m_pProgressWnd)->SetMessageStr( "Calculating camera selectors..." );
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrA( "Step 3 of 4" );
	}

	vector<float>*	TriMeshDims	= new vector<float>[iTotalTriCount];
	vector<float>*	MeshDims	= new vector<float>[pSelectorNames->size()];
	int				iTriOffset	= 0;

	if( PrepareGridDataSelectors( TriMeshDims, MeshDims, pSelectorNames, fCellSize ) == false )
		return;

	std::vector<int> tris_res;		// resulting list of meshidx-triindex
	int camID = 0;
	StrVec				CamNames;	// list of cam names
	std::vector<int>	CamIDs;		// list of cam uids
	int					iTotalTrisCounter = 0;

	//cell's corners
	D3DXVECTOR3			cell_verts[4], cell_dir( 0.0f, 1.0f, 0.0f );						
	bool				vert_ok[4];				//intersection flag

	//////////////////////////////////////
	//		SEARCH FOR CAMERAS UIDS		//
	//////////////////////////////////////
	//look for cam names
	for( unsigned int i = 0; i < pSelectorNames->size(); i++ )
	{
		//get list of cam selectors
		ListStrVec_iter lst_iter = m_lBinds.begin();

		while( lst_iter != m_lBinds.end() )
		{
			if( (*lst_iter)[2] == "Camera" && (*lst_iter)[4] == "CameraSelect" && (*lst_iter)[3] == (*pSelectorNames)[i] )
			{
				CamNames.push_back( (*lst_iter)[1] );
				break;
			}

			lst_iter++;
		}
	}
	
	//look for cam uids
	for( unsigned int i = 0; i < CamNames.size(); i++ )
	{
		//get list of cam selectors
		ListStrVec_iter lst_iter = m_lBinds.begin();

		while( lst_iter != m_lBinds.end() )
		{
			if( (*lst_iter)[2] == "Scene" && (*lst_iter)[4] == "Camera" && (*lst_iter)[3] == CamNames[i] )
			{
				CamIDs.push_back( atoi( (*lst_iter)[0].data()  ) );
				break;
			}

			lst_iter++;
		}
	}

	//raw cells for each mesh
	StrMap* cellsOfMesh = new StrMap[pSelectorNames->size()];

	for( unsigned int msh = 0; msh < pSelectorNames->size(); msh++ )
	{
		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*pSelectorNames)[msh] );

		int	iTrisCount = pMesh->GetIndicesCount() / 3;

		for( int i = 0; i < iTrisCount; i++ )
		{
			CVERTEX tA = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 0 ] ];
			CVERTEX tB = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 1 ] ];
			CVERTEX tC = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 2 ] ];

			D3DXVECTOR3 A( tA.x, tA.y, tA.z );
			D3DXVECTOR3 B( tB.x, tB.y, tB.z );
			D3DXVECTOR3 C( tC.x, tC.y, tC.z );

			D3DXVECTOR2 patchMin;
			int			patchX, patchZ;
				
			int patchOffsetX = (int)( ( TriMeshDims[ iTriOffset + i ][0] - pvMin->x ) / fCellSize );
			int patchOffsetZ = (int)( ( TriMeshDims[ iTriOffset + i ][1] - pvMin->y ) / fCellSize );

			patchMin.x = pvMin->x + patchOffsetX * fCellSize;
			patchMin.y = pvMin->y + patchOffsetZ * fCellSize;

			patchX = (int)( ( ( TriMeshDims[ iTriOffset + i ][2] - patchMin.x ) / fCellSize ) ) + 1;
			patchZ = (int)( ( ( TriMeshDims[ iTriOffset + i ][3] - patchMin.y ) / fCellSize ) ) + 1;

			for( int u = 0; u < patchX; u++ )
			{
				for( int v = 0; v < patchZ; v++ )
				{
					////////////////////////////
					// PREPARE CELL VERTECIES //
					////////////////////////////
					float cell_x_min = patchMin.x + fCellSize * u;
					float cell_z_min = patchMin.y + fCellSize * v;

					cell_verts[0] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min );				//A
					cell_verts[1] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min );				//B
					cell_verts[2] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min + fCellSize );	//C
					cell_verts[3] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min + fCellSize );	//D

					/////////////////////////////////////
					// DO CELLVERT - TRI INTERSECTIONS //
					/////////////////////////////////////						
					/*** cell vertecies with tris ***/
					for( int j = 0; j < 4; j++ )
					{
						if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts[j], &cell_dir ) != -1.0f )
						{
							CString cellXY;
							cellXY.Format( "%i_%i", patchOffsetX + u, patchOffsetZ + v );
							cellsOfMesh[msh][cellXY.GetBuffer()] = "";
						}
					}
					
				}
			}

			if( m_pProgressWnd )
			{
				if( progrs_val > 100 )
				{
					if( counter < step )
						counter += 1.0f;
					else
					{
						counter = fabs( step - counter );
						((CValidateDlg*)m_pProgressWnd)->Step();
						counter += 1.0f;
						percent++;
					}
				}
				else
				{
					while( counter < step )
					{
						counter += 1.0f;
						percent++;
						((CValidateDlg*)m_pProgressWnd)->Step();
					}

					counter = fabs( step - counter );
				}

				CString prc;
				prc.Format( "%i%%", percent );
					
				((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( prc );
			}
		}

		iTriOffset += iTrisCount;
	}

	if( m_pProgressWnd )
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( "100%" );

	progrs_val = pSelectorNames->size();

	if( progrs_val > 100 )
		step = progrs_val / 100.0f;
	else
		step = 100.0f / progrs_val;

	counter = 0.0f;
	percent = 0;

	if( m_pProgressWnd )
	{
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( "0%" );
		((CValidateDlg*)m_pProgressWnd)->SetProgressRange( 0, 100 );
		((CValidateDlg*)m_pProgressWnd)->SetPos( 0 );
		((CValidateDlg*)m_pProgressWnd)->SetMessageStr( "Finishing..." );
		((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrA( "Step 4 of 4" );
	}

	for( unsigned int msh = 0; msh < pSelectorNames->size(); msh++ )
	{
		CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->GetMeshByName( (*pSelectorNames)[msh] );

		for( StrMap_iter iter = cellsOfMesh[msh].begin(); iter != cellsOfMesh[msh].end(); ++iter )
		{
			str_t	cellX = iter->first.substr( 0, iter->first.find( "_" ) );
			str_t	cellZ = iter->first.substr( iter->first.find( "_" ) + 1, iter->first.length() - iter->first.find( "_" ) );
			int		iCellX = atoi( cellX.data() );
			int		iCellZ = atoi( cellZ.data() );

			float cell_x_min = pvMin->x + fCellSize * iCellX;
			float cell_z_min = pvMin->y + fCellSize * iCellZ;

			cell_verts[0] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min );				//A
			cell_verts[1] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min );				//B
			cell_verts[2] = D3DXVECTOR3( cell_x_min,			 0.0f, cell_z_min + fCellSize );	//C
			cell_verts[3] = D3DXVECTOR3( cell_x_min + fCellSize, 0.0f, cell_z_min + fCellSize );	//D

			int	iTrisCount = pMesh->GetIndicesCount() / 3;

			for( int gg = 0; gg < 4; gg++ )
				vert_ok[gg] = false;

			for( int i = 0; i < iTrisCount; i++ )
			{
				CVERTEX tA = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 0 ] ];
				CVERTEX tB = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 1 ] ];
				CVERTEX tC = pMesh->GetVertecies()[ pMesh->GetIndices()[ i * 3 + 2 ] ];

				D3DXVECTOR3 A( tA.x, tA.y, tA.z );
				D3DXVECTOR3 B( tB.x, tB.y, tB.z );
				D3DXVECTOR3 C( tC.x, tC.y, tC.z );

				for( int j = 0; j < 4; j++ )
				{
					if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts[j], &cell_dir ) != -1.0f )
						vert_ok[j] = true;
				}
			}
			
			if( vert_ok[0] && vert_ok[1] && vert_ok[2] && vert_ok[3] )
			{
				if( iCellX > 0 && iCellX < iGridMaxX && iCellZ > 0 && iCellZ < iGridMaxZ )
					pCamSelGrid[iCellX][iCellZ] = CamIDs[msh];
			}
		}

		if( m_pProgressWnd )
		{
			if( progrs_val > 100 )
			{
				if( counter < step )
					counter += 1.0f;
				else
				{
					counter = fabs( step - counter );
					((CValidateDlg*)m_pProgressWnd)->Step();
					counter += 1.0f;
					percent++;
				}
			}
			else
			{
				while( counter < step )
				{
					counter += 1.0f;
					percent++;
					((CValidateDlg*)m_pProgressWnd)->Step();
				}

				counter = fabs( step - counter );
			}

			CString prc;
			prc.Format( "%i%%", percent );
				
			((CValidateDlg*)m_pProgressWnd)->SetProgressMessageStrD( prc );
		}
	}

	SAFE_DELETE_ARRAY( cellsOfMesh );
	SAFE_DELETE_ARRAY( TriMeshDims );
	SAFE_DELETE_ARRAY( MeshDims );
}

bool CSceneFile::SaveToFile( str_t path )
{
	int iTemp = 0;

	if( m_dwFileVersion != SCN_CURR_VERSION )
	{
		str_t file_ver = GetScnVersionAsString( m_dwFileVersion );
		str_t curr_ver = GetScnVersionAsString( SCN_CURR_VERSION );

		str_t err_msg	= "Файл имеет версию " + file_ver + ", он может быть сохранён только в\n\r версию " + curr_ver + ". Сохранить файл?";
	
		if( MessageBox( NULL, err_msg.data(), "Внимание!", MB_YESNO ) != IDYES )
			return false;
	}

	((CEfnMeshlist*)m_pMeshes)->SaveToFile( m_pMeshes->GetFolder() + GetSceneID() + ".geom.efn" );

	if( path == "" )
		return false;

	streampos pos;

	ofstream file( path.data(), ios::binary );
	file.seekp( pos );

	//store scn file version
	m_dwFileVersion = SCN_CURR_VERSION;
	file.write( (char *)&m_dwFileVersion, sizeof(DWORD) );

	WR2Strm( file, m_strSceneID );


	//write cams
	//reset all cams upvectors to 0.0f, 1.0f, 0.0f
	for( UINT i = 0; i < m_vecCameras.size(); i++ )
		m_vecCameras[i]->upvec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	iTemp = (int)m_vecCameras.size();
	WR2Strm( file, iTemp );

	for( unsigned int i = 0; i < m_vecCameras.size(); i++ )
	{
		//file.write( (const char *)m_vecCameras[i], sizeof(CCAMERA2) );

		WR2Strm( file, m_vecCameras[i]->name );

		SCAMERA_LOAD tmpCam( *m_vecCameras[i] );
		file.write( (const char *)&tmpCam, sizeof(SCAMERA_LOAD) );
	}

	//write lights
	iTemp = (int)m_vecLights.size();
	WR2Strm( file, iTemp );

	for( unsigned int i = 0; i < m_vecLights.size(); i++ )
	{
		//file.write( (const char *)m_vecLights[i], sizeof(CLIGHT) );
		WR2Strm( file, m_vecLights[i]->name );
		WR2Strm( file, m_vecLights[i]->multiplier );
		
		D3DLIGHT9 light;

		float f1div255 = 1.0f / 255.0f;

		light.Position = m_vecLights[i]->pos;
		D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &(m_vecLights[i]->targ - m_vecLights[i]->pos) );

		switch ((LIGHT_TYPE)m_vecLights[i]->type)
		{
			case LT_DIRECTED:
				light.Type = D3DLIGHT_DIRECTIONAL;
				break;
			
			case LT_SPOT:
				light.Type = D3DLIGHT_DIRECTIONAL;	//light.Type = D3DLIGHT_SPOT;
				break;
			
			default: // LT_POINT:
				light.Type = D3DLIGHT_POINT;
				break;
		}
	
		light.Diffuse.r		= m_vecLights[i]->diffuse.r * f1div255;
		light.Diffuse.g		= m_vecLights[i]->diffuse.g * f1div255;
		light.Diffuse.b		= m_vecLights[i]->diffuse.b * f1div255;
		light.Diffuse.a		= 0.0f;
		
		light.Specular		= light.Diffuse;

		light.Ambient.r		= m_vecLights[i]->ambient.r * f1div255;
		light.Ambient.g		= m_vecLights[i]->ambient.g * f1div255;	
		light.Ambient.b		= m_vecLights[i]->ambient.b * f1div255;
		light.Ambient.a		= 0.0f;

		light.Attenuation0	= 0.0f;
		light.Attenuation1	= m_vecLights[i]->attenuation;
		light.Attenuation2	= 0.0f;
		light.Range			= m_vecLights[i]->range;
		light.Falloff		= 0.0f;
		light.Phi			= 0.0f;
		light.Theta			= 0.0f;

		file.write( (const char *)&light, sizeof(D3DLIGHT9) );
	}


	//write textures
    WR2Strm( file, ((int)m_vecTextures.size()) );
    for( unsigned int i = 0; i < m_vecTextures.size(); i++ )
        WR2Strm( file, m_vecTextures[i] );

	// write binds
	WR2Strm( file, (int)(m_lBinds.size()) );
	
	ListStrVec_iter lst_iter = m_lBinds.begin();

	while( lst_iter != m_lBinds.end() )
	{
		StrVec		strVec		= *lst_iter;
		StrVec_iter	str_iter	= strVec.begin();

		// num of params in bind
		WR2Strm( file, (int)(strVec.size()) );

		int i = 0;

		while( str_iter != strVec.end() )
		{
			str_t param = *str_iter;

	/*		if( i == 2 || i == 4 )
			{
				if( param == "ShadowFloor" )
					param = "ShadowReceiver";
				else if ( param == "ObjAction" )
					param = "Action";
				else if ( param == "Obj3DVideo" )
					param = "Video3D";
				else if ( param == "FreeObj" )
					param = "Free";
				else
				{}
			}
			else if( i == 5 )
			{
				CString strTemp = param.data();

				strTemp.Replace( "ObjAction", "Action" );
				strTemp.Replace( "FreeObj", "Free" );
				strTemp.Replace( "AddObj3DVideo", "AddObjVideo3D" );
				strTemp.Replace( "Obj3DVideo", "Video3D" );
				strTemp.Replace( "SetCamAttachedTo3DVideo", "SetCamAttachedToVideo3D" );

				param = strTemp.GetBuffer();
			}
			else
			{}*/

			WR2Strm( file, param );

			str_iter++;
			i++;
		}

		lst_iter++;
	}



	WR2Strm( file, m_vMin );
	WR2Strm( file, m_vMax );

	WR2Strm( file, m_fCellSize );
	WR2Strm( file, m_iXGridSize );
	WR2Strm( file, m_iZGridSize );

	for( int i = 0; i < m_iXGridSize; i++ )
	{
		for( int j = 0; j < m_iZGridSize; j++ )
		{
			WR2Strm( file, m_pppGrid[i][j][1] * 2 + 2 ); 
			file.write( (const char *)m_pppGrid[i][j], sizeof(int) * (m_pppGrid[i][j][1] * 2 + 2) );
		}
	}

	//custom objects and types
	iTemp = m_vecCustomObjects.size();
	WR2Strm( file, iTemp );

	for( UINT i = 0; i < m_vecCustomObjects.size(); i++ )
		WR2Strm( file, m_vecCustomObjects[i] );


	iTemp = m_vecCustomTypes.size();
	WR2Strm( file, iTemp );

	for( UINT i = 0; i < m_vecCustomTypes.size(); i++ )
		WR2Strm( file, m_vecCustomTypes[i] );


	//text resources
	iTemp = m_vecTextRes.size();
	WR2Strm( file, iTemp );

	for( UINT i = 0; i < m_vecTextRes.size(); i++ )
	{
		WR2Strm( file, m_vecTextRes[i]->id );
		WR2Strm( file, m_vecTextRes[i]->comments );
		WR2Strm( file, m_vecTextRes[i]->data );
	}

	//curves
	iTemp = m_vecCurves.size();
	WR2Strm( file, iTemp );

	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		WR2Strm( file, m_vecCurves[i]->id );
		WR2Strm( file, m_vecCurves[i]->length );

		iTemp = m_vecCurves[i]->nulls.size();
		WR2Strm( file, iTemp );

		for( UINT j = 0; j < m_vecCurves[i]->nulls.size(); j++ )
			WR2Strm( file, m_vecCurves[i]->nulls[j] );

		WR2Strm( file, (int)m_vecCurves[i]->type );
	}

	file.close();

	return true;
}

bool CSceneFile::LoadMeshlist()
{
	CString path, file;
	path.Format( "%s%s%s", this->GetFolder().data(), GetSceneID().data(), ".geom.efn" );
	file.Format( "%s%s", GetSceneID().data(), ".geom.efn" );

	if( FileExists( path.GetBuffer() ) == false  )
	{
		CString strError;
		strError.Format( "Scene's meshlist is missing! \n\r\n\r[%s]", path.GetBuffer() );

		MessageBox( NULL, strError, "Внимание!", MB_OK | MB_ICONERROR );
		return false;
	}

	CEfnMeshlist* meshlist = new CEfnMeshlist;

	if( meshlist->LoadFromFile( path.GetBuffer() ) == false )
	{
		SAFE_DELETE( meshlist );

		CString strError;
		strError.Format( "Scene's meshlist failed to load!: \n\r[%s]", path.GetBuffer() );

		MessageBox( NULL, strError, "Corrupted meshlist file!", MB_OK | MB_ICONERROR );
		return false;
	}
	else
	{
		m_pMeshes = meshlist;
		m_pMeshes->SetPaths( this->GetFolder(), file.GetBuffer() );
	}

	return true;
}

void CSceneFile::TranslateWholeSceneHack( D3DXVECTOR3* pTranslate )
{
	for( int i = 0; i < GetLightsCount(); i++ )
	{
		GetLightByIndex(i)->pos += *pTranslate;
		GetLightByIndex(i)->targ += *pTranslate;
	}

	for( int i = 0; i < GetCamerasCount(); i++ )
	{
		GetCameraByIndex(i)->pos	+= *pTranslate;
		GetCameraByIndex(i)->targ	+= *pTranslate;
	}

	((CEfnMeshlist*)GetMeshlist())->TranslateWholeSceneHack( pTranslate );
}

CCAMERA2* CSceneFile::AddCamera( str_t strCamID )
{
	CCAMERA2* cam = new CCAMERA2( strCamID );
	m_vecCameras.push_back( cam );

	return cam;
}

CLIGHT* CSceneFile::AddLight( str_t strLightID )
{
	CLIGHT* light = new CLIGHT( strLightID );
	m_vecLights.push_back( light );

	return light;
}

void CSceneFile::RemoveNullFromCurves( str_t strNullID )
{
	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		for( UINT j = 0; j < m_vecCurves[i]->nulls.size(); j++ )
		{
			if( m_vecCurves[i]->nulls[j] == strNullID )
			{
				m_vecCurves[i]->nulls.erase( m_vecCurves[i]->nulls.begin() + j );
				j = 0;
			}
		}
	}
}

void CSceneFile::RenameNullInCurves( str_t strCurrNullID, str_t strNewNullID )
{
	for( UINT i = 0; i < m_vecCurves.size(); i++ )
	{
		for( UINT j = 0; j < m_vecCurves[i]->nulls.size(); j++ )
		{
			if( m_vecCurves[i]->nulls[j] == strCurrNullID )
				m_vecCurves[i]->nulls[j] = strNewNullID;
		}
	}
}

str_t CSceneFile::GetSceneID()
{
	return GetFilename().substr( 0, GetFilename().length() - 4 );
}

bool CSceneFile::InitFromXSICharacter( CXSIFile* xsi_file )
{
	//copy light data
	for( int i = 0; i < xsi_file->GetLightsCount(); i++ )
	{
		CLIGHT* light = new CLIGHT;
		*light = *xsi_file->GetLightByIndex( i );

		if( light->type == LT_SPOT )
			light->type = LT_DIRECTED;

		m_vecLights.push_back( light );
	}

	// copy cam data
	for( int i = 0; i < xsi_file->GetCamerasCount(); i++ )
	{
		CCAMERA2* cam = new CCAMERA2;
		*cam = *xsi_file->GetCameraByIndex( i );
		m_vecCameras.push_back( cam );
	}


	LoadMeshlist();

	return true;
}

void CSceneFile::BeginShadowMeshBoxDrawing( D3DXVECTOR3* pos )
{
	// m_iShadowVisibilityMode -- (1) box

	m_pShadowMeshPos = *pos;
	m_iShadowMeshVisibilityMode = TMDM_BOX;
}

void CSceneFile::EndShadowMeshDrawing()
{
	m_iShadowMeshVisibilityMode = -1;
	m_iShadowMeshEdgesCount		= 0;
	SAFE_DELETE_ARRAY( m_pShadowMesh );
}

void CSceneFile::SetShadowMeshParams( D3DXVECTOR3* size, D3DXVECTOR3* tesselation )
{
	if( m_iShadowMeshVisibilityMode == TMDM_BOX )
	{
		//count edges
		m_iShadowMeshEdgesCount = 12 + ( (tesselation->x - 1) * 4 ) + ( (tesselation->y - 1) * 4 ) + ( (tesselation->z - 1) * 4 ); 

		//create edges buffer
		SAFE_DELETE_ARRAY( m_pShadowMesh );
		m_pShadowMesh = new D3DXVECTOR3[m_iShadowMeshEdgesCount * 2];

		float hX = size->x / 2.0f;
		float hY = size->y / 2.0f;
		float hZ = size->z / 2.0f;

		//lower frame
		m_pShadowMesh[0] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	0.0f,  hZ );
		m_pShadowMesh[1] = m_pShadowMeshPos + D3DXVECTOR3(	hX,	0.0f,  hZ );
		m_pShadowMesh[2] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	0.0f, -hZ );
		m_pShadowMesh[3] = m_pShadowMeshPos + D3DXVECTOR3(	hX,	0.0f, -hZ );
		m_pShadowMesh[4] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	0.0f,  hZ );
		m_pShadowMesh[5] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	0.0f, -hZ );
		m_pShadowMesh[6] = m_pShadowMeshPos + D3DXVECTOR3(  hX,	0.0f,  hZ );
		m_pShadowMesh[7] = m_pShadowMeshPos + D3DXVECTOR3(  hX,	0.0f, -hZ );

		//upper frame
		m_pShadowMesh[8] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	size->y,  hZ );
		m_pShadowMesh[9] = m_pShadowMeshPos + D3DXVECTOR3(	hX,	size->y,  hZ );
		m_pShadowMesh[10] = m_pShadowMeshPos + D3DXVECTOR3(-hX,	size->y, -hZ );
		m_pShadowMesh[11] = m_pShadowMeshPos + D3DXVECTOR3(	hX,	size->y, -hZ );
		m_pShadowMesh[12] = m_pShadowMeshPos + D3DXVECTOR3(-hX,	size->y,  hZ );
		m_pShadowMesh[13] = m_pShadowMeshPos + D3DXVECTOR3(-hX,	size->y, -hZ );
		m_pShadowMesh[14] = m_pShadowMeshPos + D3DXVECTOR3( hX,	size->y,  hZ );
		m_pShadowMesh[15] = m_pShadowMeshPos + D3DXVECTOR3( hX,	size->y, -hZ );

		//sides
		m_pShadowMesh[16] = m_pShadowMeshPos + D3DXVECTOR3( -hX, 0.0f,	  hZ );
		m_pShadowMesh[17] = m_pShadowMeshPos + D3DXVECTOR3(	-hX, size->y, hZ );
		m_pShadowMesh[18] = m_pShadowMeshPos + D3DXVECTOR3(  hX, 0.0f,	  hZ );
		m_pShadowMesh[19] = m_pShadowMeshPos + D3DXVECTOR3(	 hX, size->y, hZ );
		m_pShadowMesh[20] = m_pShadowMeshPos + D3DXVECTOR3( -hX, 0.0f,    -hZ );
		m_pShadowMesh[21] = m_pShadowMeshPos + D3DXVECTOR3( -hX, size->y, -hZ );
		m_pShadowMesh[22] = m_pShadowMeshPos + D3DXVECTOR3(  hX, 0.0f,    -hZ );
		m_pShadowMesh[23] = m_pShadowMeshPos + D3DXVECTOR3(  hX, size->y, -hZ );

		int iOffset = 24;

		float tessFactorU = size->y / tesselation->x;

		for( int i = 0; i < tesselation->x - 1; i++ )
		{
			//lower frame
			m_pShadowMesh[iOffset + 8 * i + 0] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	tessFactorU * (i + 1),  hZ );
			m_pShadowMesh[iOffset + 8 * i + 1] = m_pShadowMeshPos + D3DXVECTOR3(  hX,	tessFactorU * (i + 1),  hZ );
			m_pShadowMesh[iOffset + 8 * i + 2] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	tessFactorU * (i + 1), -hZ );
			m_pShadowMesh[iOffset + 8 * i + 3] = m_pShadowMeshPos + D3DXVECTOR3(  hX,	tessFactorU * (i + 1), -hZ );
			m_pShadowMesh[iOffset + 8 * i + 4] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	tessFactorU * (i + 1),  hZ );
			m_pShadowMesh[iOffset + 8 * i + 5] = m_pShadowMeshPos + D3DXVECTOR3( -hX,	tessFactorU * (i + 1), -hZ );
			m_pShadowMesh[iOffset + 8 * i + 6] = m_pShadowMeshPos + D3DXVECTOR3(  hX,	tessFactorU * (i + 1),  hZ );
			m_pShadowMesh[iOffset + 8 * i + 7] = m_pShadowMeshPos + D3DXVECTOR3(  hX,	tessFactorU * (i + 1), -hZ );
		}

		iOffset = iOffset + 8 * (tesselation->x - 2) + 8;

		float tessFactorV = size->x / tesselation->y;

		for( int i = 0; i < tesselation->y - 1; i++ )
		{
			//lower frame
			m_pShadowMesh[iOffset + 8 * i + 0] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), size->y,  hZ );
			m_pShadowMesh[iOffset + 8 * i + 1] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), 0.0f,	    hZ );
			m_pShadowMesh[iOffset + 8 * i + 2] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), size->y, -hZ );
			m_pShadowMesh[iOffset + 8 * i + 3] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), 0.0f,	   -hZ );
			m_pShadowMesh[iOffset + 8 * i + 4] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), 0.0f,		hZ );
			m_pShadowMesh[iOffset + 8 * i + 5] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), 0.0f,	   -hZ );
			m_pShadowMesh[iOffset + 8 * i + 6] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), size->y,  hZ );
			m_pShadowMesh[iOffset + 8 * i + 7] = m_pShadowMeshPos + D3DXVECTOR3( -hX + tessFactorV * (i + 1), size->y, -hZ );
		}

		iOffset = iOffset + 8 * (tesselation->y - 2) + 8;

		float tessFactorBase = size->z / tesselation->z;

		for( int i = 0; i < tesselation->z - 1; i++ )
		{
			//lower frame
			m_pShadowMesh[iOffset + 8 * i + 0] = m_pShadowMeshPos + D3DXVECTOR3( -hX, 0.0f,	   -hZ + tessFactorBase * (i + 1) );
			m_pShadowMesh[iOffset + 8 * i + 1] = m_pShadowMeshPos + D3DXVECTOR3(  hX, 0.0f,	   -hZ + tessFactorBase * (i + 1) );
			m_pShadowMesh[iOffset + 8 * i + 2] = m_pShadowMeshPos + D3DXVECTOR3( -hX, size->y, -hZ + tessFactorBase * (i + 1) );
			m_pShadowMesh[iOffset + 8 * i + 3] = m_pShadowMeshPos + D3DXVECTOR3(  hX, size->y, -hZ + tessFactorBase * (i + 1) );
			m_pShadowMesh[iOffset + 8 * i + 4] = m_pShadowMeshPos + D3DXVECTOR3(  hX, 0.0f,	   -hZ + tessFactorBase * (i + 1) );
			m_pShadowMesh[iOffset + 8 * i + 5] = m_pShadowMeshPos + D3DXVECTOR3(  hX, size->y, -hZ + tessFactorBase * (i + 1) );
			m_pShadowMesh[iOffset + 8 * i + 6] = m_pShadowMeshPos + D3DXVECTOR3( -hX, 0.0f,    -hZ + tessFactorBase * (i + 1) );
			m_pShadowMesh[iOffset + 8 * i + 7] = m_pShadowMeshPos + D3DXVECTOR3( -hX, size->y, -hZ + tessFactorBase * (i + 1) );
		}
	}
}

void CSceneFile::CreateMeshFromShadowMesh( CString ID, D3DXVECTOR3* pos, D3DXVECTOR3* size, D3DXVECTOR3* tesselation, LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_iShadowMeshVisibilityMode == TMDM_BOX )
	{
		int iX = tesselation->y;
		int iY = tesselation->x;
		int iZ = tesselation->z;

		float fDeltaX = size->x / iX;
		float fDeltaY = size->y / iY;
		float fDeltaZ = size->z / iZ;

		int iVertsPerBoxFaceXY = (iX + 1) * (iY + 1);
		int iIdxPerBoxFaceXY = iX * iY * 6;
		int iVertsPerBoxFaceYZ = (iY + 1) * (iZ + 1);
		int iIdxPerBoxFaceYZ = iY * iZ * 6;
		int iVertsPerBoxFaceXZ = (iX + 1) * (iZ + 1);
		int iIdxPerBoxFaceXZ = iX * iZ * 6;

		if( m_iShadowMeshVisibilityMode == 1 )
		{
			int iGlobOffVert = 0;
			int iGlobOffInd = 0;

			CMesh* pMesh = ((CEfnMeshlist*)m_pMeshes)->AddMesh( ID.GetBuffer(0) );

			// UV side -- front
			
			CVERTEX* pVert = new CVERTEX[iVertsPerBoxFaceXY * 2 + iVertsPerBoxFaceYZ * 2 + iVertsPerBoxFaceXZ * 2];
			int* pInd = new int[iIdxPerBoxFaceXY * 2 + iIdxPerBoxFaceYZ * 2 + iIdxPerBoxFaceXZ * 2];

			// vertecies
			for( int i = 0; i <= iY; i++ )
			{
				for( int j = 0; j <= iX; j++ )
				{
					pVert[i * (iX + 1) + j].nx	= 0.0f;
					pVert[i * (iX + 1) + j].ny	= 0.0f;
					pVert[i * (iX + 1) + j].nz	= 1.0f;
					pVert[i * (iX + 1) + j].x	= (pos->x - size->x / 2.0f) + j * fDeltaX;
					pVert[i * (iX + 1) + j].y	= (pos->y + size->y ) - i * fDeltaY;
					pVert[i * (iX + 1) + j].z	= (pos->z - size->z / 2);
				}
			}

			//indicies
			for( int i = 0; i < iY; i++ )
			{
				for( int j = 0; j < iX; j++ )
				{
					int ioffset = i * iX * 6 + j * 6;

					pInd[ioffset + 0] = i * (iX + 1) + j;
					pInd[ioffset + 1] = (i + 1) * (iX + 1) + j + 1;
					pInd[ioffset + 2] = (i + 1) * (iX + 1) + j;
					pInd[ioffset + 3] = i * (iX + 1) + j;
					pInd[ioffset + 4] = i * (iX + 1) + j + 1;
					pInd[ioffset + 5] = (i + 1) * (iX + 1) + j + 1;
				}
			}

			iGlobOffVert = iVertsPerBoxFaceXY;
			iGlobOffInd = iIdxPerBoxFaceXY;

			// UV side -- back
			// vertecies
			for( int i = 0; i <= iY; i++ )
			{
				for( int j = 0; j <= iX; j++ )
				{
					pVert[iGlobOffVert + i * (iX + 1) + j].nx	= 0.0f;
					pVert[iGlobOffVert + i * (iX + 1) + j].ny	= 0.0f;
					pVert[iGlobOffVert + i * (iX + 1) + j].nz	= -1.0f;
					pVert[iGlobOffVert + i * (iX + 1) + j].x	= (pos->x - size->x / 2.0f) + j * fDeltaX;
					pVert[iGlobOffVert + i * (iX + 1) + j].y	= (pos->y + size->y ) - i * fDeltaY;
					pVert[iGlobOffVert + i * (iX + 1) + j].z	= (pos->z + size->z / 2);
				}
			}

			//indicies
			for( int i = 0; i < iY; i++ )
			{
				for( int j = 0; j < iX; j++ )
				{
					int ioffset = i * iX * 6 + j * 6;

					pInd[iGlobOffInd + ioffset + 0] = iGlobOffVert + i * (iX + 1) + j;
					pInd[iGlobOffInd + ioffset + 1] = iGlobOffVert + (i + 1) * (iX + 1) + j;
					pInd[iGlobOffInd + ioffset + 2] = iGlobOffVert + (i + 1) * (iX + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 3] = iGlobOffVert + i * (iX + 1) + j;
					pInd[iGlobOffInd + ioffset + 4] = iGlobOffVert + (i + 1) * (iX + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 5] = iGlobOffVert + i * (iX + 1) + j + 1;
				}
			}

			iGlobOffVert = iVertsPerBoxFaceXY * 2;
			iGlobOffInd = iIdxPerBoxFaceXY * 2;

			// UV side -- right
			// vertecies
			for( int i = 0; i <= iY; i++ )
			{
				for( int j = 0; j <= iZ; j++ )
				{
					pVert[iGlobOffVert + i * (iZ + 1) + j].nx	= 1.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].ny	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].nz	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].x	= (pos->x + size->x / 2.0f);
					pVert[iGlobOffVert + i * (iZ + 1) + j].y	= (pos->y + size->y ) - i * fDeltaY;
					pVert[iGlobOffVert + i * (iZ + 1) + j].z	= (pos->z - size->z / 2.0f) + j * fDeltaZ;
				}
			}

			//indicies
			for( int i = 0; i < iY; i++ )
			{
				for( int j = 0; j < iZ; j++ )
				{
					int ioffset = i * iZ * 6 + j * 6;

					pInd[iGlobOffInd + ioffset + 0] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 1] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 2] = iGlobOffVert + (i + 1) * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 3] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 4] = iGlobOffVert + i * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 5] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
				}
			}

			iGlobOffVert = iVertsPerBoxFaceXY * 2 + iVertsPerBoxFaceYZ;
			iGlobOffInd = iIdxPerBoxFaceXY * 2 + iIdxPerBoxFaceYZ;

			// UV side -- left
			// vertecies
			for( int i = 0; i <= iY; i++ )
			{
				for( int j = 0; j <= iZ; j++ )
				{
					pVert[iGlobOffVert + i * (iZ + 1) + j].nx	= 1.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].ny	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].nz	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].x	= (pos->x - size->x / 2.0f);
					pVert[iGlobOffVert + i * (iZ + 1) + j].y	= (pos->y + size->y ) - i * fDeltaY;
					pVert[iGlobOffVert + i * (iZ + 1) + j].z	= (pos->z - size->z / 2.0f) + j * fDeltaZ;
				}
			}

			//indicies
			for( int i = 0; i < iY; i++ )
			{
				for( int j = 0; j < iZ; j++ )
				{
					int ioffset = i * iZ * 6 + j * 6;

					pInd[iGlobOffInd + ioffset + 0] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 1] = iGlobOffVert + (i + 1) * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 2] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 3] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 4] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 5] = iGlobOffVert + i * (iZ + 1) + j + 1;
				}
			}

			iGlobOffVert = iVertsPerBoxFaceXY * 2 + iVertsPerBoxFaceYZ * 2;
			iGlobOffInd = iIdxPerBoxFaceXY * 2 + iIdxPerBoxFaceYZ * 2;

			// UV side -- top
			// vertecies
			for( int i = 0; i <= iX; i++ )
			{
				for( int j = 0; j <= iZ; j++ )
				{
					pVert[iGlobOffVert + i * (iZ + 1) + j].nx	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].ny	= 1.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].nz	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].x	= (pos->x - size->x / 2.0f ) + i * fDeltaX;
					pVert[iGlobOffVert + i * (iZ + 1) + j].y	= pos->y + size->y;
					pVert[iGlobOffVert + i * (iZ + 1) + j].z	= (pos->z - size->z / 2.0f) + j * fDeltaZ;
				}
			}

			//indicies
			for( int i = 0; i < iX; i++ )
			{
				for( int j = 0; j < iZ; j++ )
				{
					int ioffset = i * iZ * 6 + j * 6;

					pInd[iGlobOffInd + ioffset + 0] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 1] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 2] = iGlobOffVert + (i + 1) * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 3] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 4] = iGlobOffVert + i * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 5] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
				}
			}

			iGlobOffVert = iVertsPerBoxFaceXY * 2 + iVertsPerBoxFaceYZ * 2 + iVertsPerBoxFaceXZ;
			iGlobOffInd = iIdxPerBoxFaceXY * 2 + iIdxPerBoxFaceYZ * 2 + iIdxPerBoxFaceXZ;

			// UV side -- bottom
			// vertecies
			for( int i = 0; i <= iX; i++ )
			{
				for( int j = 0; j <= iZ; j++ )
				{
					pVert[iGlobOffVert + i * (iZ + 1) + j].nx	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].ny	= -1.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].nz	= 0.0f;
					pVert[iGlobOffVert + i * (iZ + 1) + j].x	= (pos->x - size->x / 2.0f ) + i * fDeltaX;
					pVert[iGlobOffVert + i * (iZ + 1) + j].y	= pos->y;
					pVert[iGlobOffVert + i * (iZ + 1) + j].z	= (pos->z - size->z / 2.0f) + j * fDeltaZ;
				}
			}

			//indicies
			for( int i = 0; i < iX; i++ )
			{
				for( int j = 0; j < iZ; j++ )
				{
					int ioffset = i * iZ * 6 + j * 6;

					pInd[iGlobOffInd + ioffset + 0] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 1] = iGlobOffVert + (i + 1) * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 2] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 3] = iGlobOffVert + i * (iZ + 1) + j;
					pInd[iGlobOffInd + ioffset + 4] = iGlobOffVert + (i + 1) * (iZ + 1) + j + 1;
					pInd[iGlobOffInd + ioffset + 5] = iGlobOffVert + i * (iZ + 1) + j + 1;
				}
			}

			pMesh->SetIndices( pInd, iIdxPerBoxFaceXY * 2 + iIdxPerBoxFaceYZ * 2 + iIdxPerBoxFaceXZ * 2 );
			SAFE_DELETE_ARRAY( pInd );

			pMesh->SetVertecies( pVert, iVertsPerBoxFaceXY * 2 + iVertsPerBoxFaceYZ * 2 + iVertsPerBoxFaceXZ * 2 );
			SAFE_DELETE_ARRAY( pVert );

			pMesh->CalcReverseIndex();
			pMesh->PrecalculateNormals();
			pMesh->CalcOOBB();
			pMesh->CalcBoundingSphere();
			pMesh->Prepare( pd3dDevice );
		}
	}
}

void CSceneFile::RenderShadowMesh( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_iShadowMeshVisibilityMode == -1 )
		return;

	if( m_pShadowMesh )
	{
		SetVColorFromMaterial( true, pd3dDevice );

		pd3dDevice->SetFVF( D3DFVF_VERTEX3D );

		pd3dDevice->SetMaterial( &m_DarkYellow );
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffE7C505 );
		pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, m_iShadowMeshEdgesCount, m_pShadowMesh, sizeof( D3DXVECTOR3 ) );

	/*	pd3dDevice->SetMaterial( &m_BlueMtrl  );
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff0000ff );		
		pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, m_iShadowMeshEdgesCount * 2, m_pShadowMesh, sizeof( D3DXVECTOR3 ) );*/
	}
}