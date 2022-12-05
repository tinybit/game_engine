#define STRICT

#include "QMgrResEfnModels.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResFiles.h"

QMgrResEfnModels::QMgrResEfnModels()
{
	PurgeCache();
	m_bLogEnabled	= false;

	m_pVD_POS					= NULL;
	m_pVD_POS_NORM				= NULL;
	m_pVD_POS_TEX				= NULL;
	m_pVD_POS_COL_TEX			= NULL;
	m_pVD_POS_NORM_TEX			= NULL;
	m_pVD_POS_NORM_COL			= NULL;
	m_pVD_POS_NORM_POS_NORM_TEX	= NULL;
}

QMgrResEfnModels::~QMgrResEfnModels()
{
	if ( m_mEfn.size() > 0 )
	{
		for( efnmap_iter i = m_mEfn.begin(); i != m_mEfn.end(); ++i )
		{
			FreeEfn(i->second);
		}
		m_mEfn.clear();
	}
}

void QMgrResEfnModels::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if ( m_bLogEnabled )
	{
		g_Log.WriteMessage(L"efn mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"efn mgr logging disabled");
	}
}

void QMgrResEfnModels::OnCreateDevice()
{
	/******************************* create vertex declaration ******************************/
	D3DVERTEXELEMENT9 el_POS[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};
	g_Graph.dev()->CreateVertexDeclaration( el_POS,	&m_pVD_POS );

	D3DVERTEXELEMENT9 el_POS_NORM[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		D3DDECL_END()
	};
	g_Graph.dev()->CreateVertexDeclaration( el_POS_NORM,	&m_pVD_POS_NORM );

	D3DVERTEXELEMENT9 el_POS_TEX[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	g_Graph.dev()->CreateVertexDeclaration( el_POS_TEX,	&m_pVD_POS_TEX );

	D3DVERTEXELEMENT9 el_POS_COL_TEX[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 2, 0, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	g_Graph.dev()->CreateVertexDeclaration( el_POS_COL_TEX,	&m_pVD_POS_COL_TEX );

	D3DVERTEXELEMENT9 el_POS_NORM_TEX[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 2, 0, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	g_Graph.dev()->CreateVertexDeclaration( el_POS_NORM_TEX,	&m_pVD_POS_NORM_TEX );

	D3DVERTEXELEMENT9 el_POS_NORM_COL[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 2, 0, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};
	g_Graph.dev()->CreateVertexDeclaration( el_POS_NORM_COL,	&m_pVD_POS_NORM_COL );

	D3DVERTEXELEMENT9 el_POS_NORM_POS_NORM_TEX[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 2, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
		{ 3, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
		{ 4, 0, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	g_Graph.dev()->CreateVertexDeclaration( el_POS_NORM_POS_NORM_TEX,		&m_pVD_POS_NORM_POS_NORM_TEX );
	/************************END* create vertex declaration *******************************/

	if ( m_mEfn.size() > 0 )
	{
		for( efnmap_iter i = m_mEfn.begin(); i != m_mEfn.end(); i++ )
		{
			InitBuffers( i->second, D3DUSAGE_WRITEONLY );
		}
	}
}

void QMgrResEfnModels::OnResetDevice()
{
	if ( m_mEfn.size() > 0 )
	{
		for( efnmap_iter i = m_mEfn.begin(); i != m_mEfn.end(); i++ )
		{
			InitBuffers( i->second, D3DUSAGE_DYNAMIC );
		}
	}
}

void QMgrResEfnModels::OnLostDevice()
{
	if ( m_mEfn.size() > 0 )
	{
		for( efnmap_iter i = m_mEfn.begin(); i != m_mEfn.end(); i++ )
		{
			FreeBuffers( i->second, D3DUSAGE_DYNAMIC );
		}
	}
}

void QMgrResEfnModels::OnDestroyDevice()
{
	if ( m_mEfn.size() > 0 )
	{
		for( efnmap_iter i = m_mEfn.begin(); i != m_mEfn.end(); i++ )
		{
			FreeBuffers( i->second, D3DUSAGE_WRITEONLY );
		}
	}

	SAFE_RELEASE( m_pVD_POS );
	SAFE_RELEASE( m_pVD_POS_NORM );
	SAFE_RELEASE( m_pVD_POS_TEX );
	SAFE_RELEASE( m_pVD_POS_COL_TEX );
	SAFE_RELEASE( m_pVD_POS_NORM_TEX );
	SAFE_RELEASE( m_pVD_POS_NORM_COL );
	SAFE_RELEASE( m_pVD_POS_NORM_POS_NORM_TEX );
}

void QMgrResEfnModels::LoadEfn( const std::wstring& efn_id )
{
	if ( m_mEfn.size() > 0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i!=m_mEfn.end() )
		{
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning(L"efn is already loaded, no need to load it again, efn_id = \"" + efn_id + L"\"");
			}
			return;
		}
	}

	g_Graph.RenderLoad( L"Load EFN: " + efn_id );

	EFN_CHARACTERDATA*	m_cdData = NULL;

	int meshcount = LoadData(efn_id, m_cdData);

	if ( meshcount > 0 )
	{
		if ( meshcount == 1 )
		{
			QEfnDataListItem *EfnDataListItem = new QEfnDataListItem();

			EfnDataListItem->efn_id			= efn_id;
			EfnDataListItem->strTexturePath	= L"";
			EfnDataListItem->bMorf_BaseWrited=true;
			EfnDataListItem->pcdData		= m_cdData;
			EfnDataListItem->pIB			= NULL;
			EfnDataListItem->iIBUsage		= D3DUSAGE_WRITEONLY;
			EfnDataListItem->pPosBuff		= NULL;
			EfnDataListItem->iPosBuffUsage	= D3DUSAGE_WRITEONLY;
			EfnDataListItem->pNormBuff		= NULL;
			EfnDataListItem->iNormBuffUsage	= D3DUSAGE_WRITEONLY;
			EfnDataListItem->pUvBuff		= NULL;
			EfnDataListItem->iUvBuffUsage	= D3DUSAGE_WRITEONLY;
			EfnDataListItem->pVColorBuff	= NULL;
			EfnDataListItem->iVColorBuffUsage=D3DUSAGE_WRITEONLY;
			EfnDataListItem->pOOBB_IB		= NULL;
			EfnDataListItem->pOOBBPosBuff	= NULL;
			ZeroMemory( &EfnDataListItem->stMtrl, sizeof( D3DMATERIAL9 ) );

			EfnDataListItem->stMtrl = EfnDataListItem->pcdData->material;

			if ( m_bLogEnabled )
			{
				g_Log.WriteMessage(L"efn loaded, efn_id = \"" + efn_id + L"\"");
			}

			InitBuffers( EfnDataListItem, D3DUSAGE_WRITEONLY );
			m_mEfn[efn_id] = EfnDataListItem;

			PurgeCache();
		}

		if( meshcount > 1 )
		{
			//free array of data
			for( int i = 0; i < meshcount; i++)
			{
				QEfnDataListItem* TempEfn = GetEfn( efn_id + L"." + m_cdData[i].name );

				if ( TempEfn )
					continue;

				QEfnDataListItem *EfnDataListItem = new QEfnDataListItem();

				EfnDataListItem->efn_id			= efn_id + L"." + m_cdData[i].name;
				EfnDataListItem->strTexturePath	= m_cdData[i].texture_filename;
				EfnDataListItem->bMorf_BaseWrited=true;
				EfnDataListItem->pIB			= NULL;
				EfnDataListItem->iIBUsage		= D3DUSAGE_WRITEONLY;
				EfnDataListItem->pPosBuff		= NULL;
				EfnDataListItem->iPosBuffUsage	= D3DUSAGE_WRITEONLY;
				EfnDataListItem->pNormBuff		= NULL;
				EfnDataListItem->iNormBuffUsage	= D3DUSAGE_WRITEONLY;
				EfnDataListItem->pUvBuff		= NULL;
				EfnDataListItem->iUvBuffUsage	= D3DUSAGE_WRITEONLY;
				EfnDataListItem->pVColorBuff	= NULL;
				EfnDataListItem->iVColorBuffUsage=D3DUSAGE_WRITEONLY;
				EfnDataListItem->pOOBB_IB		= NULL;
				EfnDataListItem->pOOBBPosBuff	= NULL;
				ZeroMemory( &EfnDataListItem->stMtrl, sizeof( D3DMATERIAL9 ) );

				EfnDataListItem->pcdData						= new EFN_CHARACTERDATA;

				CopyCData( EfnDataListItem->pcdData, &m_cdData[i] );

				EfnDataListItem->stMtrl = EfnDataListItem->pcdData->material;

				if(m_bLogEnabled)
				{
					g_Log.WriteMessage(L"efn loaded, efn_id = \"" + EfnDataListItem->efn_id + L"\"");
				}

				InitBuffers( EfnDataListItem, D3DUSAGE_WRITEONLY );
				m_mEfn[EfnDataListItem->efn_id] = EfnDataListItem;

				PurgeCache();

				FreeData(&m_cdData[i], false);
			}
			SAFE_DELETE_ARRAY( m_cdData );
		}
	}
	else
	{
		g_Log.WriteError(L"efn load error, efn_id = \"" + efn_id + L"\"");
	}
}

void QMgrResEfnModels::CopyCData( EFN_CHARACTERDATA* dest, EFN_CHARACTERDATA* src )
{
	*dest						= *src;

	dest->aabbList				= NULL;
	dest->pBoundingSpheres		= NULL;
	dest->z_coord_offset_list	= NULL;

	if ( dest->anim_frames_count > 0 )
	{
		if ( src->aabbList )
			dest->aabbList			= new D3DXVECTOR3*[dest->anim_frames_count];
		if ( src->pBoundingSpheres )
			dest->pBoundingSpheres	= new CSPHERE[dest->anim_frames_count];
		if ( src->z_coord_offset_list )
			dest->z_coord_offset_list = new D3DXVECTOR3[dest->anim_frames_count];

		for( int i = 0; i < dest->anim_frames_count; i++)
		{
			if ( src->aabbList )
			{
				dest->aabbList[i]	= new D3DXVECTOR3[8];
				for( int j = 0; j < 8; j++)
				{
					dest->aabbList[i][j] = src->aabbList[i][j];
				}
			}

			if ( src->pBoundingSpheres )
			{
				dest->pBoundingSpheres[i] = src->pBoundingSpheres[i];
			}

			if ( src->z_coord_offset_list )
			{
				dest->z_coord_offset_list[i] = src->z_coord_offset_list[i];
			}
		}
	}

	dest->anim_list.clear();

	dest->posList				= NULL;
	dest->normList				= NULL;

	if ( dest->vertecies_count > 0 )
	{
		if(src->posList)
			dest->posList		= new D3DXVECTOR3[src->vertecies_count];
		if(src->normList)
			dest->normList		= new D3DXVECTOR3[src->vertecies_count];

		for( int k = 0; k < dest->vertecies_count; k++)
		{
			if ( src->posList )
				dest->posList[k]	= src->posList[k];
			if ( src->normList )
				dest->normList[k]	= src->normList[k];
		}
	}

	dest->uvList				= NULL;
	dest->vcolList				= NULL;

	if ( dest->vertecies_per_frame_count > 0 )
	{
		if ( src->uvList )
			dest->uvList		= new CVERTEX_UV[dest->vertecies_per_frame_count];
		if ( src->vcolList )
			dest->vcolList		= new long[dest->vertecies_per_frame_count];
		for( int k = 0; k < dest->vertecies_per_frame_count; k++)
		{
			if(src->uvList)
				dest->uvList[k] = src->uvList[k];
			if(src->vcolList)
				dest->vcolList[k] = src->vcolList[k];
		}
	}

	dest->vertex_indices_list	= NULL;

	if ( dest->vertex_indices_count > 0 && src->vertex_indices_list)
	{
		dest->vertex_indices_list	= new int[dest->vertex_indices_count];
		for( int k = 0; k < dest->vertex_indices_count; k++)
		{
			dest->vertex_indices_list[k] = src->vertex_indices_list[k];
		}
	}

	if ( src->nulls_map.size()>0 )
	{
		for( mapWStrVecSNulls_iter i = src->nulls_map.begin(); i != src->nulls_map.end(); i++)
		{
			dest->nulls_map[i->first] = i->second;
		}
	}

	dest->reverse_index = NULL;

	if ( dest->vertecies_per_frame_count > 0 && src->reverse_index)
	{
		dest->reverse_index = new int*[dest->vertecies_per_frame_count];

		for( int i = 0; i < dest->vertecies_per_frame_count; i++ )
		{
			int temp_rv = 1;
			dest->reverse_index[i] = new int[temp_rv];

			for( int j = 0; j < temp_rv; j++)
			{
				dest->reverse_index[i][j] = src->reverse_index[i][j];
			}
		}
	}

	dest->material = src->material;
}

std::wstring QMgrResEfnModels::GetFileFromID( const std::wstring& efn_id )
{
	return L"data\\models\\" + efn_id + L".efn";
}

bool QMgrResEfnModels::LoadDataAnimation(EFN_CHARACTERDATA* (&m_cdData), ifstream& file)
{
	m_cdData = new EFN_CHARACTERDATA;

	m_cdData->anim_frames_count			= 0;
	m_cdData->animations_count			= 0;
	m_cdData->vertecies_count			= 0;
	m_cdData->vertecies_per_frame_count	= 0;
	m_cdData->vertex_indices_count		= 0;
	m_cdData->name						= L"";
	m_cdData->texture_filename			= L"";
	m_cdData->anim_list.clear();
	m_cdData->posList					= NULL;
	m_cdData->normList					= NULL;
	m_cdData->uvList					= NULL;
	m_cdData->vcolList					= NULL;
	m_cdData->vertex_indices_list		= NULL;
	m_cdData->z_coord_offset_list		= NULL;
	m_cdData->aabbList					= NULL;
	m_cdData->pBoundingSpheres			= NULL;
	m_cdData->nulls_map.clear();
	m_cdData->global_cylinder_radius	= 0;
	m_cdData->reverse_index				= NULL;
	m_cdData->frames_script.clear();

	ReadStrm( file, m_cdData->name );				// name

	/************************************ load vertecies list *******************************/
	ReadStrm( file, m_cdData->vertecies_count );

	if ( m_cdData->vertecies_count > 0 )
	{
		m_cdData->posList	= new D3DXVECTOR3[m_cdData->vertecies_count];
		m_cdData->normList	= new D3DXVECTOR3[m_cdData->vertecies_count];

		file.read((char *)m_cdData->posList, sizeof(D3DXVECTOR3) * m_cdData->vertecies_count);
	}
	/********************************END* load vertecies list *******************************/


	/************************************ load indices list **********************************/
	ReadStrm( file, m_cdData->vertex_indices_count );

	if ( m_cdData->vertex_indices_count > 0 )
	{
		m_cdData->vertex_indices_list = new int[m_cdData->vertex_indices_count];

		file.read((char *)m_cdData->vertex_indices_list, sizeof(int) * m_cdData->vertex_indices_count);
	}
	/********************************END* load indices list ********************************/

	ReadStrm( file, m_cdData->vertecies_per_frame_count ); // amount of vertecies per frame
	ReadStrm( file, m_cdData->anim_frames_count ); // amount of frames

	if ( m_cdData->anim_frames_count > 0 )
	{
		m_cdData->frames_script.reserve( m_cdData->anim_frames_count );
		
		std::wstring m_sTemp;
		for( int i = 0; i < m_cdData->anim_frames_count; i++ )
		{
			ReadStrm( file, m_sTemp );
			m_cdData->frames_script.push_back( m_sTemp );
		}
	}

	/***************************************** load NULLS list ********************************/
	int m_iNullsCount;
	ReadStrm( file, m_iNullsCount );

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		//null id
		std::wstring null_id;
		ReadStrm( file, null_id );

		vecSNulls* NullVecTemp = &m_cdData->nulls_map[null_id];

		for( int j = 0; j < m_cdData->anim_frames_count; j++ )
		{
			SNULL temp;
			ReadStrm( file, temp.vPos);
			ReadStrm( file, temp.vRot);
			ReadStrm( file, temp.vScale);

			if( fabs( temp.vRot.x ) == 360 )
				temp.vRot.x = 0.0f;

			if( fabs( temp.vRot.y ) == 360 )
				temp.vRot.y = 0.0f;

			if( fabs( temp.vRot.z ) == 360 )
				temp.vRot.z = 0.0f;

			while(temp.vRot.x>=180.0f)	temp.vRot.x-=360.0f;
			while(temp.vRot.x<-180.0f)	temp.vRot.x+=360.0f;
			while(temp.vRot.y>=180.0f)	temp.vRot.y-=360.0f;
			while(temp.vRot.y<-180.0f)	temp.vRot.y+=360.0f;
			while(temp.vRot.z>=180.0f)	temp.vRot.z-=360.0f;
			while(temp.vRot.z<-180.0f)	temp.vRot.z+=360.0f;
			temp.vRot.x = D3DXToRadian( temp.vRot.x );
			temp.vRot.y = D3DXToRadian( temp.vRot.y );
			temp.vRot.z = D3DXToRadian( temp.vRot.z );

			NullVecTemp->push_back(temp);
		}
	}
	/***********************************END* load nulls list ********************************/

	ReadStrm( file, m_cdData->global_cylinder_radius ); // global radius

	/************************************ load list of aabbs *********************************/
	m_cdData->aabbList = new D3DXVECTOR3*[ m_cdData->anim_frames_count ];

	for( int i = 0; i < m_cdData->anim_frames_count; i++)
	{
		m_cdData->aabbList[i] = new D3DXVECTOR3[8];
		file.read( (char *)m_cdData->aabbList[i], sizeof(D3DXVECTOR3) * 8 );
	}
	/********************************END* load list of aabbs *******************************/


	/************************************ load list of bounding spheres ********************/
	m_cdData->pBoundingSpheres = new CSPHERE[ m_cdData->anim_frames_count ];
	file.read( (char *)m_cdData->pBoundingSpheres, sizeof(CSPHERE) * m_cdData->anim_frames_count );
	/********************************END* load list of bounding spheres *****************/


	/************************************ load list of z-offsets ******************************/
	m_cdData->z_coord_offset_list = new D3DXVECTOR3[m_cdData->anim_frames_count];

	if( !m_cdData->z_coord_offset_list )
		return false;

	file.read((char *)m_cdData->z_coord_offset_list, sizeof(D3DXVECTOR3) * m_cdData->anim_frames_count);
	/********************************END* load list of z-offsets ***************************/


	/************************************ load amount of animations **********************/
	file.read((char *)&m_cdData->animations_count, sizeof(int));
	/************************************ load amount of animations **********************/


	/************************************ load list of animations ***************************/
	if( m_cdData->animations_count > 0)
	{
		//read animations list
		for(int i = 0; i < m_cdData->animations_count; i++)
		{
			EFN_CHARACTERANIMATION temp;
			ReadStrm( file, temp.name );				//get name

			ReadStrm( file, temp.frames_count );	//get frames count

			temp.frames_list = new int[temp.frames_count]; //get frames list

			if( !temp.frames_list )
				return false;

			file.read((char *)temp.frames_list, sizeof(int) * temp.frames_count);

			ReadStrm( file, temp.bHuinya );

			m_cdData->anim_list[temp.name] = temp;
		}
	}
	/********************************END* load list of animations **********************/


	/************************************ load reverse index *********************************/
	if ( m_cdData->vertecies_per_frame_count )
	{
		m_cdData->reverse_index = new int*[m_cdData->vertecies_per_frame_count];

		for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
		{
			int temp_rv;

			// read tris amount
			file.read((char *)&temp_rv, sizeof(int));

			m_cdData->reverse_index[i] = new int[temp_rv];

			// read tris list
			file.read((char *)m_cdData->reverse_index[i], sizeof(int) * temp_rv);
		}
	}
	/********************************END* load reverse index *********************************/


	/************************************ load uv list ********************************************/
	if ( m_cdData->vertecies_per_frame_count )
	{
		m_cdData->uvList = new CVERTEX_UV[m_cdData->vertecies_per_frame_count];

		file.read( (char *)m_cdData->uvList, sizeof(CVERTEX_UV) * m_cdData->vertecies_per_frame_count );
	}
	/************************************END* load uv list **************************************/


	/************************************* load vert color list ************************************/
	if ( m_cdData->vertecies_per_frame_count )
	{
		m_cdData->vcolList = new long[m_cdData->vertecies_per_frame_count];

		file.read( (char *)m_cdData->vcolList, sizeof(long) * m_cdData->vertecies_per_frame_count );
	}
	/********************************END* load vert color list ************************************/

	ReadStrm( file, m_cdData->fFPS );

	/*********************************** calc anims params ****************************************/
	if( m_cdData->animations_count > 0)
	{
		//read animations list
		for( mapWStrEFNANIM_iter i = m_cdData->anim_list.begin(); i != m_cdData->anim_list.end(); i++ )
		{
			i->second.m_fAnimTimeLength = (float)i->second.frames_count / m_cdData->fFPS;

			i->second.m_fAnimZLength = 0.0f;
			for( int j = 0; j < i->second.frames_count; j++ )
			{
				i->second.m_fAnimZLength += m_cdData->z_coord_offset_list[ i->second.frames_list[j] ].z;
			}
		}
	}
	/**********************************END* calc anims params *************************************/

	file.read( (char *)&m_cdData->material, sizeof(D3DMATERIAL9) );

	PrecalculateNormals(m_cdData);

	return true;
}


bool QMgrResEfnModels::LoadDataMeshlist(EFN_CHARACTERDATA* (&m_cdData), ifstream& file, int& count)
{
	std::wstring temp_str;

	ReadStrm( file, temp_str );

	//load meshes count
	int mesh_count;
	ReadStrm( file, mesh_count );

	//create containers array
	m_cdData = new EFN_CHARACTERDATA[mesh_count];

	count = mesh_count;

	//load meshes
	for( int i = 0; i < mesh_count; i++ )
	{
		m_cdData[i].anim_frames_count			= 1;
		m_cdData[i].animations_count			= 0;
		m_cdData[i].vertecies_count				= 0;
		m_cdData[i].vertecies_per_frame_count	= 0;
		m_cdData[i].vertex_indices_count		= 0;
		m_cdData[i].name						= L"";
		m_cdData[i].texture_filename			= L"";
		m_cdData[i].anim_list.clear();
		m_cdData[i].posList						= NULL;
		m_cdData[i].normList					= NULL;
		m_cdData[i].uvList						= NULL;
		m_cdData[i].vcolList					= NULL;
		m_cdData[i].vertex_indices_list			= NULL;
		m_cdData[i].z_coord_offset_list			= NULL;
		m_cdData[i].aabbList					= NULL;
		m_cdData[i].pBoundingSpheres			= NULL;
		m_cdData[i].nulls_map.clear();
		m_cdData[i].reverse_index				= NULL;
		m_cdData[i].global_cylinder_radius		= 0;

		//name
		ReadStrm( file, m_cdData[i].name );

		/************************************ verticies ****************************************/
		ReadStrm( file, m_cdData[i].vertecies_count );
		m_cdData[i].vertecies_per_frame_count = m_cdData[i].vertecies_count;

		if ( m_cdData[i].vertecies_count > 0 )
		{
			CVERTEX* pTempVert = new CVERTEX[ m_cdData[i].vertecies_count ];
			file.read( (char*)pTempVert, sizeof(CVERTEX) * m_cdData[i].vertecies_count );

			m_cdData[i].posList     = new D3DXVECTOR3[m_cdData[i].vertecies_count];
			m_cdData[i].normList    = new D3DXVECTOR3[m_cdData[i].vertecies_count];
			m_cdData[i].uvList      = new CVERTEX_UV[m_cdData[i].vertecies_per_frame_count];
			m_cdData[i].vcolList    = new long [m_cdData[i].vertecies_per_frame_count];

			for( int j = 0; j < m_cdData[i].vertecies_count; j++ )
			{
				m_cdData[i].posList[j].x = pTempVert[j].x;
				m_cdData[i].posList[j].y = pTempVert[j].y;
				m_cdData[i].posList[j].z = pTempVert[j].z;

				m_cdData[i].normList[j].x = pTempVert[j].nx;
				m_cdData[i].normList[j].y = pTempVert[j].ny;
				m_cdData[i].normList[j].z = pTempVert[j].nz;

				m_cdData[i].uvList[j].u = pTempVert[j].u;
				m_cdData[i].uvList[j].v = pTempVert[j].v;
				m_cdData[i].vcolList[j] = pTempVert[j].color;
			}

			SAFE_DELETE_ARRAY( pTempVert );
		}
		/***********************************END* verticies ****************************************/


		/*************************indicies*****************************/
		ReadStrm( file, m_cdData[i].vertex_indices_count );

		if ( m_cdData[i].vertex_indices_count > 0 )
		{
			m_cdData[i].vertex_indices_list = new int[m_cdData[i].vertex_indices_count];
			file.read((char *)m_cdData[i].vertex_indices_list, sizeof(int) * m_cdData[i].vertex_indices_count);
		}
		/************************END*indicies*****************************/


		/*************************BSPHERES AND OOBB*****************************/
		m_cdData[i].pBoundingSpheres = new CSPHERE[1];

		ReadStrm( file, m_cdData[i].pBoundingSpheres[0].radius );
		ReadStrm( file, m_cdData[i].pBoundingSpheres[0].pos );

		m_cdData[i].aabbList	= new D3DXVECTOR3*[1];
		m_cdData[i].aabbList[0] = new D3DXVECTOR3[8];

		file.read( (char *)m_cdData[i].aabbList[0], sizeof(D3DXVECTOR3) * 8 );
		/*********************END*BSPHERES AND OOBB*****************************/


		/********************************** NULLS *****************************/
		int m_iNullsCount;
		ReadStrm( file, m_iNullsCount );

		for( int k = 0; k < m_iNullsCount; k++ )
		{
			//null id
			std::wstring null_id;
			ReadStrm( file, null_id );

			vecSNulls* NullVecTemp = &m_cdData[i].nulls_map[null_id];

			for( int j = 0; j < m_cdData[i].anim_frames_count; j++ )
			{
				SNULL temp;
				ReadStrm( file, temp.vPos );
				ReadStrm( file, temp.vRot );
				ReadStrm( file, temp.vScale );
			
				while(temp.vRot.x>=180.0f)	temp.vRot.x-=360.0f;
				while(temp.vRot.x<-180.0f)	temp.vRot.x+=360.0f;
				while(temp.vRot.y>=180.0f)	temp.vRot.y-=360.0f;
				while(temp.vRot.y<-180.0f)	temp.vRot.y+=360.0f;
				while(temp.vRot.z>=180.0f)	temp.vRot.z-=360.0f;
				while(temp.vRot.z<-180.0f)	temp.vRot.z+=360.0f;
				temp.vRot.x = D3DXToRadian( temp.vRot.x );
				temp.vRot.y = D3DXToRadian( temp.vRot.y );
				temp.vRot.z = D3DXToRadian( temp.vRot.z );

				NullVecTemp->push_back(temp);
			}
		}
		/*******************************END* NULLS *****************************/

		/************************************ load reverse index *********************************/
		if ( m_cdData[i].vertecies_count > 0 )
		{
			m_cdData[i].reverse_index = new int*[m_cdData[i].vertecies_per_frame_count];

			for( int j = 0; j< m_cdData[i].vertecies_per_frame_count; j++ )
			{
				int temp_rv;

				// read tris amount
				file.read((char *)&temp_rv, sizeof(int));

				m_cdData[i].reverse_index[j] = new int[temp_rv];

				// read tris list
				file.read((char *)m_cdData[i].reverse_index[j], sizeof(int) * temp_rv);
			}
		}
		/********************************END* load reverse index *********************************/

		file.read( (char *)&m_cdData[i].material, sizeof(D3DMATERIAL9) );

		PrecalculateNormals(&m_cdData[i]);
	}

	return true;
}

int QMgrResEfnModels::LoadData( const std::wstring& efn_id, EFN_CHARACTERDATA* (&m_cdData) )
{
	streampos pos;

	std::wstring efnfilename = GetFileFromID(efn_id);

	if ( !g_FileMgr.FileExists( efnfilename ) )
		return 0;

	DWORD file_version/* = GetEfnVersion( efnfilename )*/;

	ifstream file( wstr2str( efnfilename ).c_str(), ios::binary);

	file.seekg(pos);

	/************************************ type ********************************************/
	int efn_type;
	ReadStrm( file, file_version );							// version
	ReadStrm( file, efn_type );								// type
	/********************************END* type *****************************************/

	// read animated mesh data
	if ( efn_type == EF_ANIMATION )
	{
		if( file_version != EFN_CURRENT_VERSION )
		{
			g_Log.WriteError( L"Unable to parse file! EFN file:" + efnfilename + L" has version ["+GetEfnVersionAsString( file_version )+L"], current version is: ["+GetEfnVersionAsString( EFN_CURRENT_VERSION )+L"]." );
			file.close();
			return 0;
		}

		LoadDataAnimation( m_cdData, file );

		file.close();
		return 1;
	}
	//read simple meshes data
	else if ( efn_type == EF_MESHLIST )
	{
		if( file_version != EFN_MESHLIST_CURRENT_VERSION )
		{
			g_Log.WriteError( L"Unable to parse file! EFN meshlist file:" + efnfilename + L" has version ["+GetEfnMeshlistVersionAsString( file_version )+L"], current version is: ["+GetEfnMeshlistVersionAsString( EFN_MESHLIST_CURRENT_VERSION )+L"]." );
			file.close();
			return 0;
		}

		int	meshes_count;
		LoadDataMeshlist( m_cdData, file, meshes_count );

		file.close();
		return meshes_count;
	}
	else
	{
		file.close();
		return 0;
	}
}

void QMgrResEfnModels::PrecalculateNormals(EFN_CHARACTERDATA* m_cdData)
{
	int i, j, k, vert1, vert2, vert3, iFrames, iTriCount, iVPF;

	CVector vA, vB, vC, vTmp1, vTmp2;
	CVector** vTriNormals;
	D3DXVECTOR3* vlist;
	D3DXVECTOR3* nlist;

	iFrames			= m_cdData->anim_frames_count;
	iVPF			= m_cdData->vertecies_per_frame_count;
	iTriCount		= m_cdData->vertex_indices_count / 3;
	vlist			= m_cdData->posList;
	nlist			= m_cdData->normList;

	//-----------------------------polygon normals calculations---------------------------
	//allocate memory
	vTriNormals = new CVector*[iFrames];

	for( i = 0; i < iFrames; i++ )
		vTriNormals[i] = new CVector[iTriCount];

	for( j = 0; j < iFrames; j++ )
	{
		for( i = 0; i < iTriCount; i++ )
		{
			//get indexes of vertecies from triangle
			vert1 = m_cdData->vertex_indices_list[i * 3 + 0];
			vert2 = m_cdData->vertex_indices_list[i * 3 + 1];
			vert3 = m_cdData->vertex_indices_list[i * 3 + 2];


			vA.SetData( vlist[ vert1 + j * iVPF ].x, vlist[ vert1 + j * iVPF ].y, vlist[ vert1 + j * iVPF ].z );
			vB.SetData( vlist[ vert2 + j * iVPF ].x, vlist[ vert2 + j * iVPF ].y, vlist[ vert2 + j * iVPF ].z );
			vC.SetData( vlist[ vert3 + j * iVPF ].x, vlist[ vert3 + j * iVPF ].y, vlist[ vert3 + j * iVPF ].z );


			vTmp1 = vB - vA;
			vTmp2 = vC - vA;

			vTriNormals[j][i] = vTmp1 ^ vTmp2;
			vTriNormals[j][i].Normalize();
			vTriNormals[j][i].Flip();
		}
	}
	//------------------------END--polygon normals calculations---------------------//


	//-----------------------------vertex normals calculations----------------------------//
	int count = 0;

	for( int j = 0; j < iFrames; j++ )
	{
		//summ up normals using cache
		for( k = 0; k < iVPF; k++ )
		{
			count = 0;
			vTmp1.Clear();

			for (int m = 1; m < m_cdData->reverse_index[k][0]; m++)
			{
				int tri_index = m_cdData->reverse_index[k][m];

				vTmp1 += vTriNormals[j][ tri_index ];
				count++;
			}

			//calc average vector
			vTmp1 /= (float)count;
			vTmp1.UnitVector();
			vTmp1.Flip();

			nlist[ j * iVPF + k ].x = vTmp1.x;
			nlist[ j * iVPF + k ].y = vTmp1.y;
			nlist[ j * iVPF + k ].z = vTmp1.z;
		}

	}  // for (j = 0; j < iFrames; j++)

	for( i = 0; i < iFrames; i++ )
		SAFE_DELETE_ARRAY( vTriNormals[i] );

	SAFE_DELETE_ARRAY( vTriNormals );
}

void QMgrResEfnModels::FreeEfnByID( const std::wstring& efn_id )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i!=m_mEfn.end() )
		{
			FreeEfn(i->second);
			m_mEfn.erase(efn_id);
			PurgeCache();
			return;
		}
	}
}

void QMgrResEfnModels::FreeEfn( QEfnDataListItem* EfnDataListItem )
{
	if(m_bLogEnabled)
	{
		g_Log.WriteWarning(L"efn free, efn_id = \"" + EfnDataListItem->efn_id + L"\"");
	}
	FreeBuffers( EfnDataListItem, D3DUSAGE_WRITEONLY );
	FreeBuffers( EfnDataListItem, D3DUSAGE_DYNAMIC );
	FreeData(EfnDataListItem->pcdData);
	SAFE_DELETE(EfnDataListItem);

}

void QMgrResEfnModels::CloneEfn( const std::wstring& efn_id, const std::wstring& src_efn_id, DWORD iIBUsage, DWORD iPosBuffUsage, DWORD iNormBuffUsage, DWORD iUvBuffUsage, DWORD iVColorBuffUsage )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i!=m_mEfn.end() )
		{
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning(L"efn is already exists, cant clone, efn_id = \"" + efn_id + L"\"");
			}
			return;
		}
	}

	QEfnDataListItem* src = GetEfn( src_efn_id );

	if ( !src )
		return;

	//g_Graph.RenderLoad( L"Clone EFN: \"" + efn_id + L"\" from efn: \"" + src_efn_id + L"\"" );

	EFN_CHARACTERDATA*	m_cdData = src->pcdData;

	QEfnDataListItem *EfnDataListItem = new QEfnDataListItem();

	EfnDataListItem->efn_id			= efn_id;
	EfnDataListItem->strTexturePath	= src->strTexturePath;
	EfnDataListItem->pcdData			= NULL;
	EfnDataListItem->pIB			= NULL;
	EfnDataListItem->iIBUsage		= iIBUsage;
	EfnDataListItem->pPosBuff		= NULL;
	EfnDataListItem->iPosBuffUsage	= iPosBuffUsage;
	EfnDataListItem->pNormBuff		= NULL;
	EfnDataListItem->iNormBuffUsage	= iNormBuffUsage;
	EfnDataListItem->pUvBuff		= NULL;
	EfnDataListItem->iUvBuffUsage	= iUvBuffUsage;
	EfnDataListItem->pVColorBuff	= NULL;
	EfnDataListItem->iVColorBuffUsage=iVColorBuffUsage;
	EfnDataListItem->pOOBB_IB		= NULL;
	EfnDataListItem->pOOBBPosBuff	= NULL;
	ZeroMemory( &EfnDataListItem->stMtrl, sizeof( D3DMATERIAL9 ) );

	EfnDataListItem->pcdData			= new EFN_CHARACTERDATA;

	CopyCData( EfnDataListItem->pcdData, m_cdData );

	if ( EfnDataListItem->pcdData->vcolList )
	{
		EfnDataListItem->stMtrl.Diffuse = EfnDataListItem->stMtrl.Ambient = D3DXCOLOR( EfnDataListItem->pcdData->vcolList[0] );
	}
	else
	{
		EfnDataListItem->stMtrl.Diffuse = EfnDataListItem->stMtrl.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"efn loaded, efn_id = \"" + efn_id + L"\"");
	}

	InitBuffers( EfnDataListItem, D3DUSAGE_WRITEONLY );
	InitBuffers( EfnDataListItem, D3DUSAGE_DYNAMIC );
	m_mEfn[efn_id] = EfnDataListItem;

	PurgeCache();
}

void QMgrResEfnModels::FreeData( EFN_CHARACTERDATA* m_cdData )
{
	FreeData( m_cdData, true );
}

void QMgrResEfnModels::FreeData( EFN_CHARACTERDATA* m_cdData,  bool bDelete )
{
	if(m_bLogEnabled)
	{
		g_Log.WriteWarning(L"efn free data, efn_id = \"" + m_cdData->name + L"\"");
	}
	if( !m_cdData )return;

	if( m_cdData->nulls_map.size()>0 )
	{
		for( mapWStrVecSNulls_iter i = m_cdData->nulls_map.begin(); i != m_cdData->nulls_map.begin(); i++ )
		{
			i->second.clear();
		}
		m_cdData->nulls_map.clear();
	}

	if( m_cdData->reverse_index )
	{
		for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
			SAFE_DELETE_ARRAY( m_cdData->reverse_index[i] );

		SAFE_DELETE_ARRAY( m_cdData->reverse_index );
	}

	if ( m_cdData->aabbList )
	{
		for( int i = 0; i < m_cdData->anim_frames_count; i++ )
			SAFE_DELETE_ARRAY( m_cdData->aabbList[i] );

		SAFE_DELETE_ARRAY( m_cdData->aabbList );
	}

	if(m_cdData->anim_list.size()>0)
	{
		//delete animations lists
		for ( mapWStrEFNANIM_iter i = m_cdData->anim_list.begin(); i != m_cdData->anim_list.end(); i++ )
		{
			EFN_CHARACTERANIMATION temp = i->second;
			SAFE_DELETE_ARRAY(temp.frames_list);
		}

		//delete animations
		m_cdData->anim_list.clear();
	}

	SAFE_DELETE_ARRAY( m_cdData->posList );
	SAFE_DELETE_ARRAY( m_cdData->normList );
    SAFE_DELETE_ARRAY( m_cdData->uvList );
	SAFE_DELETE_ARRAY( m_cdData->vcolList );

	SAFE_DELETE_ARRAY( m_cdData->vertex_indices_list );
	SAFE_DELETE_ARRAY( m_cdData->z_coord_offset_list );
	SAFE_DELETE_ARRAY( m_cdData->pBoundingSpheres );

	if ( bDelete ) SAFE_DELETE(m_cdData);
}

bool QMgrResEfnModels::InitBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"bool QMgrResEfnModels::InitBuffers(QEfnDataListItem* EfnDataListItem)\"");
		return false;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"bool QMgrResEfnModels::InitBuffers(QEfnDataListItem* EfnDataListItem)\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return false;
	}

	if ( m_cdData->vertex_indices_count == 0 )
		return true;

	/******************************* create oobb buffers ****************************/
	if ( Usage == D3DUSAGE_WRITEONLY )
	{
		if( FAILED( g_Graph.dev()->CreateIndexBuffer( sizeof(unsigned short) * 12 * 3,
													D3DUSAGE_WRITEONLY,
													D3DFMT_INDEX16,
													D3DPOOL_MANAGED,
													&EfnDataListItem->pOOBB_IB,
													NULL )))
			return false;

		UINT OOBBxyzSize	= sizeof( D3DXVECTOR3 ) * 8;
		EfnDataListItem->pOOBBPosBuff	= new LPDIRECT3DVERTEXBUFFER9[ m_cdData->anim_frames_count ];

		for( int i = 0; i < m_cdData->anim_frames_count; i++ )
		{
			//oobb pos
			if( FAILED( g_Graph.dev()->CreateVertexBuffer( OOBBxyzSize,
											D3DUSAGE_WRITEONLY,
											D3DFVF_XYZ,
											D3DPOOL_MANAGED,
											&EfnDataListItem->pOOBBPosBuff[i], NULL ) ) )
				return false;
		}
	}
	/*****************************END* create buffers **************************/

	// IB
	if ( EfnDataListItem->iIBUsage == Usage )
	{
		UINT IndexBufferDateTypeSize = ( m_cdData->vertex_indices_count < 65536 ? 2 : 4 );
		_D3DFORMAT IndexBufferFormat = ( m_cdData->vertex_indices_count < 65536 ? D3DFMT_INDEX16 : D3DFMT_INDEX32 );

		if( FAILED( g_Graph.dev()->CreateIndexBuffer( IndexBufferDateTypeSize * m_cdData->vertex_indices_count,
													EfnDataListItem->iIBUsage,
													IndexBufferFormat,
													( EfnDataListItem->iIBUsage == D3DUSAGE_WRITEONLY ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT ),
													&EfnDataListItem->pIB,
													NULL )))
			return false;
	}

	unsigned int		xyzSize		= sizeof( D3DXVECTOR3 ) * m_cdData->vertecies_per_frame_count;

	// pos
	if ( EfnDataListItem->iPosBuffUsage == Usage )
	{
		EfnDataListItem->pPosBuff		= new LPDIRECT3DVERTEXBUFFER9[ m_cdData->anim_frames_count ];

		for( int i = 0; i < m_cdData->anim_frames_count; i++ )
		{
			//model pos
			if( FAILED( g_Graph.dev()->CreateVertexBuffer( xyzSize,
											EfnDataListItem->iPosBuffUsage,
											D3DFVF_XYZ,
											( EfnDataListItem->iPosBuffUsage == D3DUSAGE_WRITEONLY ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT ),
											&EfnDataListItem->pPosBuff[i], NULL ) ) )
				return false;
		}
	}

	// norm
	if ( EfnDataListItem->iNormBuffUsage == Usage )
	{
		EfnDataListItem->pNormBuff		= new LPDIRECT3DVERTEXBUFFER9[ m_cdData->anim_frames_count ];

		for( int i = 0; i < m_cdData->anim_frames_count; i++ )
		{
			if( FAILED( g_Graph.dev()->CreateVertexBuffer( xyzSize,
											EfnDataListItem->iNormBuffUsage,
											D3DFVF_NORMAL,
											( EfnDataListItem->iNormBuffUsage == D3DUSAGE_WRITEONLY ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT ),
											&EfnDataListItem->pNormBuff[i], NULL ) ) )
				return false;
		}
	}

	// uv
	if ( EfnDataListItem->iUvBuffUsage == Usage )
	{
		UINT uvSize		= sizeof( CVERTEX_UV ) * m_cdData->vertecies_per_frame_count;

		if( FAILED( g_Graph.dev()->CreateVertexBuffer( uvSize,
											EfnDataListItem->iUvBuffUsage,
											D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0),
											( EfnDataListItem->iUvBuffUsage == D3DUSAGE_WRITEONLY ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT ),
											&EfnDataListItem->pUvBuff, NULL ) ) )
			return false;
	}

	// vertex color
	if ( EfnDataListItem->iVColorBuffUsage == Usage )
	{
		UINT		vcolSize	= sizeof( long ) * m_cdData->vertecies_per_frame_count;

		if( FAILED( g_Graph.dev()->CreateVertexBuffer( vcolSize,
											EfnDataListItem->iVColorBuffUsage,
											D3DFVF_DIFFUSE,
											( EfnDataListItem->iVColorBuffUsage == D3DUSAGE_WRITEONLY ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT ),
											&EfnDataListItem->pVColorBuff, NULL ) ) )
			return false;
	}

	/***************************END* create vertex buffers **************************/

	return FillBuffers( EfnDataListItem, Usage );
}

bool QMgrResEfnModels::FillBuffers( QEfnDataListItem* EfnDataListItem, DWORD Usage )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"bool QMgrResEfnModels::FillBuffers( QEfnDataListItem* EfnDataListItem )\"");
		return false;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"bool QMgrResEfnModels::FillBuffers( QEfnDataListItem* EfnDataListItem )\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return false;
	}

	//******************************model buffers********************************************
	if ( EfnDataListItem->iIBUsage == Usage )
	{
		FillIndexBuffer( EfnDataListItem );
	}

	//delete vertex buffers
	if ( EfnDataListItem->iPosBuffUsage == Usage )
	{
		FillPosBuffer( EfnDataListItem );
	}

	if ( EfnDataListItem->iNormBuffUsage == Usage )
	{
		FillNormBuffer( EfnDataListItem );
	}

	if ( EfnDataListItem->iUvBuffUsage == Usage )
	{
		FillUVBuffer( EfnDataListItem );
	}

	if ( EfnDataListItem->iVColorBuffUsage == Usage )
	{
		FillColorBuffer( EfnDataListItem );
	}
	//**************************END*model buffers********************************************

	unsigned int		xyzSize		= sizeof( D3DXVECTOR3 ) * m_cdData->vertecies_per_frame_count;

	VOID* pVertices;
	unsigned short* pShortIndices;
	//******************************oobb buffers********************************************
	//lock buffer
	if( FAILED( EfnDataListItem->pOOBB_IB->Lock(0, sizeof(unsigned short) * 12 * 3, (void**)&pShortIndices, 0))) return false;

		pShortIndices[ 0*3 + 0]	= 0;
		pShortIndices[ 0*3 + 1]	= 1;
		pShortIndices[ 0*3 + 2]	= 2;

		pShortIndices[ 1*3 + 0]	= 0;
		pShortIndices[ 1*3 + 1]	= 2;
		pShortIndices[ 1*3 + 2]	= 3;

		pShortIndices[ 2*3 + 0]	= 4;
		pShortIndices[ 2*3 + 1]	= 6;
		pShortIndices[ 2*3 + 2]	= 5;

		pShortIndices[ 3*3 + 0]	= 4;
		pShortIndices[ 3*3 + 1]	= 7;
		pShortIndices[ 3*3 + 2]	= 6;

		pShortIndices[ 4*3 + 0]	= 4;
		pShortIndices[ 4*3 + 1]	= 5;
		pShortIndices[ 4*3 + 2]	= 1;

		pShortIndices[ 5*3 + 0]	= 4;
		pShortIndices[ 5*3 + 1]	= 1;
		pShortIndices[ 5*3 + 2]	= 0;

		pShortIndices[ 6*3 + 0]	= 7;
		pShortIndices[ 6*3 + 1]	= 4;
		pShortIndices[ 6*3 + 2]	= 0;

		pShortIndices[ 7*3 + 0]	= 7;
		pShortIndices[ 7*3 + 1]	= 0;
		pShortIndices[ 7*3 + 2]	= 3;

		pShortIndices[ 8*3 + 0]	= 7;
		pShortIndices[ 8*3 + 1]	= 2;
		pShortIndices[ 8*3 + 2]	= 6;

		pShortIndices[ 9*3 + 0]	= 7;
		pShortIndices[ 9*3 + 1]	= 3;
		pShortIndices[ 9*3 + 2]	= 2;

		pShortIndices[10*3 + 0]	= 6;
		pShortIndices[10*3 + 1]	= 1;
		pShortIndices[10*3 + 2]	= 5;

		pShortIndices[11*3 + 0]	= 6;
		pShortIndices[11*3 + 1]	= 2;
		pShortIndices[11*3 + 2]	= 1;

	//unlock buffer
	if( FAILED(EfnDataListItem->pOOBB_IB->Unlock())) return false;

	for( int i = 0; i < m_cdData->anim_frames_count; i++ )
	{
		//pos
		if( FAILED( EfnDataListItem->pOOBBPosBuff[i]->Lock( 0, xyzSize, (void**)&pVertices, 0 ) ) ) return false;

		memcpy( (D3DXVECTOR3*)pVertices, EfnDataListItem->pcdData->aabbList[i], sizeof( D3DXVECTOR3 ) * 8 );

		if( FAILED( EfnDataListItem->pOOBBPosBuff[i]->Unlock() ) ) return false;
	}

	//**************************END*oobb buffers********************************************

	return true;
}

bool QMgrResEfnModels::FillIndexBuffer( QEfnDataListItem *EfnDataListItem )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"bool QMgrResEfnModels::FillIndexBuffers( QEfnDataListItem *EfnDataListItem )\"");
		return false;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"bool QMgrResEfnModels::FillIndexBuffers( QEfnDataListItem *EfnDataListItem )\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return false;
	}

	VOID* pIndices;
	unsigned short* pShortIndices;

	DWORD Flags = ( EfnDataListItem->iIBUsage == D3DUSAGE_WRITEONLY ? 0 : D3DLOCK_DISCARD );
	UINT IndexDataTypeSize = ( m_cdData->vertex_indices_count < 65536 ? 2 : 4 );

	if ( m_cdData->vertex_indices_count < 65536 )
	{
		if( FAILED( EfnDataListItem->pIB->Lock(0, IndexDataTypeSize * m_cdData->vertex_indices_count, (void**)&pShortIndices, Flags))) return false;

		for(int i = 0; i < m_cdData->vertex_indices_count; i++)
		{
			pShortIndices[i] = m_cdData->vertex_indices_list[i];
		}
	}
	else
	{
		if( FAILED( EfnDataListItem->pIB->Lock(0, IndexDataTypeSize * m_cdData->vertex_indices_count, (void**)&pIndices, Flags))) return false;
		memcpy(pIndices, m_cdData->vertex_indices_list, sizeof(int) * m_cdData->vertex_indices_count);
	}

	//unlock buffer
	if( FAILED(EfnDataListItem->pIB->Unlock())) return false;

	return true;
}

bool QMgrResEfnModels::FillPosBuffer( QEfnDataListItem *EfnDataListItem )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"bool QMgrResEfnModels::FillPosBuffers( QEfnDataListItem *EfnDataListItem )\"");
		return false;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"bool QMgrResEfnModels::FillPosBuffers( QEfnDataListItem *EfnDataListItem )\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return false;
	}

	//pos
	VOID* pVertices;

	D3DXVECTOR3*		pData = NULL;
	D3DXVECTOR3*		posArray	= m_cdData->posList;
	unsigned int		xyzSize		= sizeof( D3DXVECTOR3 ) * m_cdData->vertecies_per_frame_count;

	DWORD Flags = ( EfnDataListItem->iPosBuffUsage == D3DUSAGE_WRITEONLY ? 0 : D3DLOCK_DISCARD );

	for( int i = 0; i < m_cdData->anim_frames_count; i++ )
	{
		if( FAILED( EfnDataListItem->pPosBuff[i]->Lock( 0, xyzSize, (void**)&pVertices, Flags ) ) ) return false;

		pData = posArray;
		pData += i * m_cdData->vertecies_per_frame_count;
		memcpy((D3DXVECTOR3*)pVertices, pData, xyzSize );

		if( FAILED( EfnDataListItem->pPosBuff[i]->Unlock() ) ) return false;
	}

	return true;
}

bool QMgrResEfnModels::FillNormBuffer( QEfnDataListItem *EfnDataListItem )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"bool QMgrResEfnModels::FillNormBuffers( QEfnDataListItem *EfnDataListItem )\"");
		return false;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"bool QMgrResEfnModels::FillNormBuffers( QEfnDataListItem *EfnDataListItem )\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return false;
	}

	//norm
	VOID* pVertices;

	D3DXVECTOR3*		pData = NULL;
	D3DXVECTOR3*		normArray	= m_cdData->normList;
	unsigned int		xyzSize		= sizeof( D3DXVECTOR3 ) * m_cdData->vertecies_per_frame_count;

	DWORD Flags = ( EfnDataListItem->iNormBuffUsage == D3DUSAGE_WRITEONLY ? 0 : D3DLOCK_DISCARD );

	for( int i = 0; i < m_cdData->anim_frames_count; i++ )
	{
		if( FAILED( EfnDataListItem->pNormBuff[i]->Lock( 0, xyzSize, (void**)&pVertices, Flags ) ) ) return false;

		pData = normArray;
		pData += i * m_cdData->vertecies_per_frame_count;
		memcpy((D3DXVECTOR3*)pVertices, pData, xyzSize );

		if( FAILED( EfnDataListItem->pNormBuff[i]->Unlock() ) ) return false;
	}

	return true;
}

bool QMgrResEfnModels::FillUVBuffer( QEfnDataListItem *EfnDataListItem )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"bool QMgrResEfnModels::FillUVBuffers( QEfnDataListItem *EfnDataListItem )\"");
		return false;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"bool QMgrResEfnModels::FillUVBuffers( QEfnDataListItem *EfnDataListItem )\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return false;
	}

	//uv
	VOID* pVertices;

	CVERTEX_UV*			uvArray		= m_cdData->uvList;
	unsigned int		uvSize		= sizeof( CVERTEX_UV ) * m_cdData->vertecies_per_frame_count;

	DWORD Flags = ( EfnDataListItem->iUvBuffUsage == D3DUSAGE_WRITEONLY ? 0 : D3DLOCK_DISCARD );

	if( FAILED( EfnDataListItem->pUvBuff->Lock( 0, uvSize, (void**)&pVertices, Flags ) ) ) return false;
	memcpy((CVERTEX_UV*)pVertices, uvArray, uvSize );
	if( FAILED( EfnDataListItem->pUvBuff->Unlock() ) ) return false;

	return true;
}

bool QMgrResEfnModels::FillColorBuffer( QEfnDataListItem *EfnDataListItem )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"bool QMgrResEfnModels::FillColorBuffers( QEfnDataListItem *EfnDataListItem )\"");
		return false;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"bool QMgrResEfnModels::FillColorBuffers( QEfnDataListItem *EfnDataListItem )\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return false;
	}

	//vertex color
	VOID* pVertices;

	long*				vcolArray	= m_cdData->vcolList;
	unsigned int		vcolSize	= sizeof( long ) * m_cdData->vertecies_per_frame_count;

	DWORD Flags = ( EfnDataListItem->iVColorBuffUsage == D3DUSAGE_WRITEONLY ? 0 : D3DLOCK_DISCARD );

	if( FAILED( EfnDataListItem->pVColorBuff->Lock( 0, vcolSize, (void**)&pVertices, Flags ) ) ) return false;
	memcpy( (long*)pVertices, vcolArray, vcolSize );
	if( FAILED( EfnDataListItem->pVColorBuff->Unlock() ) ) return false;

	return true;
}

void QMgrResEfnModels::FreeBuffers( QEfnDataListItem *EfnDataListItem, DWORD Usage )
{
	if (!EfnDataListItem)
	{
		g_Log.WriteError(L"EfnDataListItem is null in \"void QMgrResEfnModels::FreeBuffers(QEfnDataListItem *EfnDataListItem)\"");
		return;
	}

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;
	if (!m_cdData)
	{
		g_Log.WriteError(L"m_cdData is null in \"void QMgrResEfnModels::FreeBuffers(QEfnDataListItem *EfnDataListItem)\", efn_id = \"" + EfnDataListItem->efn_id + L"\"");
		return;
	}

	if(m_bLogEnabled)
	{
		g_Log.WriteWarning(L"efn free buffers, efn_id = \"" + EfnDataListItem->efn_id + L"\"");
	}

	//delete index buffer
	if ( Usage == D3DUSAGE_WRITEONLY )
	{
		SAFE_RELEASE( EfnDataListItem->pOOBB_IB );
		SAFE_DELETE_ARRAY3D( EfnDataListItem->pOOBBPosBuff, m_cdData->anim_frames_count );
	}

	if ( EfnDataListItem->iIBUsage == Usage )
	{
		SAFE_RELEASE( EfnDataListItem->pIB );
	}

	//delete vertex buffers
	if ( EfnDataListItem->iPosBuffUsage == Usage )
	{
		SAFE_DELETE_ARRAY3D( EfnDataListItem->pPosBuff, m_cdData->anim_frames_count );
	}

	if ( EfnDataListItem->iNormBuffUsage == Usage )
	{
		SAFE_DELETE_ARRAY3D( EfnDataListItem->pNormBuff, m_cdData->anim_frames_count );
	}

	if ( EfnDataListItem->iUvBuffUsage == Usage )
	{
		SAFE_RELEASE( EfnDataListItem->pUvBuff );
	}

	if ( EfnDataListItem->iVColorBuffUsage == Usage )
	{
		SAFE_RELEASE( EfnDataListItem->pVColorBuff );
	}
}

void QMgrResEfnModels::MakeMorf( const std::wstring& efn_id, const std::wstring& nulls_efn_id, const std::wstring& meshs_efn_id, int iLeftFrame, int iRightFrame, float fTweenFactor )
{
	//Получаем item в который будем писать результат
	QEfnDataListItem* ResultMesh = GetEfn( efn_id );
	//если его нет то создаем
	if ( ResultMesh == NULL )
	{
		CloneEfn( efn_id, meshs_efn_id + L".Morf_Base", D3DUSAGE_WRITEONLY, D3DUSAGE_DYNAMIC, D3DUSAGE_DYNAMIC, D3DUSAGE_WRITEONLY, D3DUSAGE_WRITEONLY );
		ResultMesh = GetEfn( efn_id );
		ResultMesh->bMorf_BaseWrited = true;
	}
	
	//Получаем item из которого берем анимацию
	QEfnDataListItem* Nulls	= g_EfnMgr.GetEfn( nulls_efn_id );

	//Получаем базовый мэш
	QEfnDataListItem* Base	= g_EfnMgr.GetEfn( meshs_efn_id + L".Morf_Base" );

	if(	!Nulls || !ResultMesh || !Base )
		return;

	//Проверем указанный кадр на корректность
	if ( iLeftFrame < 0 || iLeftFrame > Nulls->pcdData->anim_frames_count )
		iLeftFrame = 0;

	if ( iRightFrame < 0 || iRightFrame > Nulls->pcdData->anim_frames_count )
		iRightFrame = 0;
	
	//Сумма изменений от базового меша
	float m_fTotalSum = 0.0f;

	vecKoefs Koefs;
	Koefs.reserve( Nulls->pcdData->nulls_map.size() );

	//Ищем параметры из нулей для текущего кадра морфа
	SMorfKoefs tempmorf;
	std::wstring m_sTempStr;
	for ( mapWStrVecSNulls_iter i = Nulls->pcdData->nulls_map.begin(); i != Nulls->pcdData->nulls_map.end(); i++ )
	{
		m_sTempStr = i->first;

		// HACK name
		if ( m_sTempStr.substr( 0, 5 ) == L"Morf_" )
		{
			tempmorf.name = meshs_efn_id + L"." + m_sTempStr;
			tempmorf.posList = g_EfnMgr.GetEfn( tempmorf.name )->pcdData->posList;
			tempmorf.normList = g_EfnMgr.GetEfn( tempmorf.name )->pcdData->normList;
			g_EfnMgr.GetEfnNullForMeshMorf( Nulls, i->first, iLeftFrame, iRightFrame, fTweenFactor, &tempmorf.value );
			Koefs.push_back( tempmorf );

			m_fTotalSum += fabs(tempmorf.value);
		}
	}
	
	//Если модификаций нет то вкачиваем базовый мэш
	if ( m_fTotalSum == 0.0f )
	{
		if ( !ResultMesh->bMorf_BaseWrited )
		{
			CopyMemory( ResultMesh->pcdData->posList, Base->pcdData->posList, ResultMesh->pcdData->vertecies_count * sizeof(D3DXVECTOR3) );
			CopyMemory( ResultMesh->pcdData->normList, Base->pcdData->normList, ResultMesh->pcdData->vertecies_count * sizeof(D3DXVECTOR3) );
		
			g_EfnMgr.FillPosBuffer( ResultMesh );
			g_EfnMgr.FillNormBuffer( ResultMesh );
		}

		ResultMesh->bMorf_BaseWrited = true;
	}
	else //Иначе делаем морф
	{
		D3DXVECTOR3* pvResultMeshPos = NULL;
		D3DXVECTOR3* pvResultMeshNorm = NULL;;
		D3DXVECTOR3* pBasePos = NULL;
		D3DXVECTOR3* pBaseNorm = NULL;
		
		// бежим по каждой вершине
		for( int i = 0; i < ResultMesh->pcdData->vertecies_count; i++ )
		{
			pBasePos = &Base->pcdData->posList[i];
			pBaseNorm = &Base->pcdData->normList[i];

			pvResultMeshPos = &ResultMesh->pcdData->posList[i];
			pvResultMeshNorm = &ResultMesh->pcdData->normList[i];
			
			*pvResultMeshPos = *pBasePos;
			*pvResultMeshNorm = *pBaseNorm;

			for ( vecKoefsIter j = Koefs.begin(); j != Koefs.end(); j++ )
			{
				if ( (*j).value != 0 )
				{
					*pvResultMeshPos += (*j).value * ( (*j).posList[i] - *pBasePos );
					*pvResultMeshNorm += (*j).value * ( (*j).normList[i] - *pBaseNorm );
				}
			}
		}

		g_EfnMgr.FillPosBuffer( ResultMesh );
		g_EfnMgr.FillNormBuffer( ResultMesh );

		ResultMesh->bMorf_BaseWrited = false;
	}
	Koefs.clear();
}

void QMgrResEfnModels::Render( const std::wstring& efn_id, RENDER_TYPES r_type )
{
	Render( efn_id, g_Graph.GetIdentity(), r_type );
}

void QMgrResEfnModels::Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, RENDER_TYPES r_type )
{
	Render( efn_id, pmatWorld, L"", 0.0f, r_type );
}

void QMgrResEfnModels::Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame, RENDER_TYPES r_type )
{
	Render( efn_id, pmatWorld, L"", fFrame, r_type );
}

void QMgrResEfnModels::Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, const std::wstring& anim_name, float fFrameIndex, RENDER_TYPES r_type )
{
	int m_iLeftFrame = 0;
	int m_iRightFrame = 0;
	float m_fTweenFactor = 0.0f;

	GetAnimFrame(efn_id, anim_name, fFrameIndex, &m_iLeftFrame, &m_iRightFrame, &m_fTweenFactor);
	Render(efn_id, pmatWorld, m_iLeftFrame, m_iRightFrame, m_fTweenFactor, r_type);
}

void QMgrResEfnModels::Render( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, int iLeftFrame, int iRightFrame, float fTweenFactor, RENDER_TYPES r_type )
{
	if ( m_mEfn.size() > 0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i!=m_mEfn.end() )
		{
			Render( i->second, pmatWorld, iLeftFrame, iRightFrame, fTweenFactor, r_type );
		}
	}
}

void QMgrResEfnModels::Render( const QEfnDataListItem *EfnDataListItem, const D3DXMATRIX* pmatWorld, int iLeftFrame, int iRightFrame, float fTweenFactor, RENDER_TYPES r_type )
{
	if( !EfnDataListItem )
		return;

	if( !EfnDataListItem->pcdData )
		return;

	if( ! ( EfnDataListItem->pcdData->vertex_indices_count > 0 && EfnDataListItem->pcdData->vertecies_count > 0 &&
			EfnDataListItem->pIB && EfnDataListItem->pPosBuff &&
			EfnDataListItem->pNormBuff && EfnDataListItem->pUvBuff &&
			EfnDataListItem->pVColorBuff ) )
		return;

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;

	if(r_type==RT_POS_NORM_POS_NORM_TEX)
	{
		if ( fTweenFactor == 0.0f ||
			 (iLeftFrame == iRightFrame) ||
			 ( DXUTGetDeviceCaps()->VertexProcessingCaps & D3DVTXPCAPS_TWEENING) == 0 )
		{
			r_type = RT_POS_NORM_TEX;
		}
	}

	switch(r_type)
	{
		case RT_POS:
			if ( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count )
			{
				g_Graph.dev()->SetVertexDeclaration( m_pVD_POS );

				g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pPosBuff[iLeftFrame],		0, sizeof(D3DXVECTOR3) );
				g_Graph.dev()->SetIndices( EfnDataListItem->pIB );

				g_Graph.SetWorld( pmatWorld );
				g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

				g_Graph.dev()->SetVertexDeclaration( NULL );
			}
			return;
			break;

		case RT_POS_NORM:
			if ( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count )
			{
				g_Graph.dev()->SetVertexDeclaration( m_pVD_POS_NORM );

				g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pPosBuff[iLeftFrame],		0, sizeof(D3DXVECTOR3) );
				g_Graph.dev()->SetStreamSource( 1, EfnDataListItem->pNormBuff[iLeftFrame],	0, sizeof(D3DXVECTOR3) );
				g_Graph.dev()->SetIndices( EfnDataListItem->pIB );

				g_Graph.SetWorld( pmatWorld );
				g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

				g_Graph.dev()->SetVertexDeclaration( NULL );
			}
			return;
			break;

		case RT_POS_TEX:
			if ( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count )
			{
				g_Graph.dev()->SetVertexDeclaration( m_pVD_POS_TEX );

				g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pPosBuff[iLeftFrame],		0, sizeof(D3DXVECTOR3) );
				g_Graph.dev()->SetStreamSource( 1, EfnDataListItem->pUvBuff,					0, sizeof(CVERTEX_UV) );
				g_Graph.dev()->SetIndices( EfnDataListItem->pIB );

				g_Graph.SetWorld( pmatWorld );
				g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

				g_Graph.dev()->SetVertexDeclaration( NULL );
			}
			return;
			break;

		//case RT_POS_COL_TEX:
		//	if ( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count )
		//	{
		//		g_Graph.dev()->SetVertexDeclaration( m_pVD_POS_COL_TEX );

		//		g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pPosBuff[iLeftFrame],		0, sizeof(D3DXVECTOR3) );
		//		g_Graph.dev()->SetStreamSource( 1, EfnDataListItem->pVColorBuff,				0, sizeof(long) );
		//		g_Graph.dev()->SetStreamSource( 2, EfnDataListItem->pUvBuff,					0, sizeof(CVERTEX_UV) );
		//		g_Graph.dev()->SetIndices( EfnDataListItem->pIB );

		//		g_Graph.SetWorld( pmatWorld );
		//		g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

		//		g_Graph.dev()->SetVertexDeclaration( NULL );
		//		return;
		//	}
		//	break;

		case RT_POS_NORM_TEX:
			if( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count )
			{
				g_Graph.dev()->SetVertexDeclaration( m_pVD_POS_NORM_TEX );

				g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pPosBuff[iLeftFrame],	0, sizeof(D3DXVECTOR3) ); // set to your first keyframe
				g_Graph.dev()->SetStreamSource( 1, EfnDataListItem->pNormBuff[iLeftFrame],	0, sizeof(D3DXVECTOR3) ); // set to your first keyframe
				g_Graph.dev()->SetStreamSource( 2, EfnDataListItem->pUvBuff,				0, sizeof(CVERTEX_UV) ); // set to your second keyframe
				g_Graph.dev()->SetIndices( EfnDataListItem->pIB );

				g_Graph.SetWorld( pmatWorld );
				g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

				g_Graph.dev()->SetVertexDeclaration( NULL );
			}
			return;
			break;

		//case RT_POS_NORM_COL:
		//	if( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count )
		//	{
		//		g_Graph.dev()->SetVertexDeclaration( m_pVD_POS_NORM_COL );

		//		g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pPosBuff[iLeftFrame],	0, sizeof(D3DXVECTOR3) ); // set to your first keyframe
		//		g_Graph.dev()->SetStreamSource( 1, EfnDataListItem->pNormBuff[iLeftFrame],	0, sizeof(D3DXVECTOR3) ); // set to your first keyframe
		//		g_Graph.dev()->SetStreamSource( 2, EfnDataListItem->pVColorBuff,			0, sizeof(long) ); // set to your second keyframe
		//		g_Graph.dev()->SetIndices( EfnDataListItem->pIB );

		//		g_Graph.SetWorld( pmatWorld );
		//		g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

		//		g_Graph.dev()->SetVertexDeclaration( NULL );
		//		return;
		//	}
		//	break;

		case RT_POS_NORM_POS_NORM_TEX:
			if ( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count &&
				iRightFrame >= 0 && iRightFrame < m_cdData->anim_frames_count )
			{
				g_Graph.dev()->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_TWEENING );
				g_Graph.dev()->SetRenderState( D3DRS_TWEENFACTOR, FtoDW( fTweenFactor ) );

				// Set our vertex declaration
				g_Graph.dev()->SetVertexDeclaration( m_pVD_POS_NORM_POS_NORM_TEX );

				// Set the stream sources that the vertex declaration will point to
				g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pPosBuff[iLeftFrame],	0, sizeof(D3DXVECTOR3) ); // set to your first keyframe
				g_Graph.dev()->SetStreamSource( 1, EfnDataListItem->pNormBuff[iLeftFrame],	0, sizeof(D3DXVECTOR3) ); // set to your first keyframe
				g_Graph.dev()->SetStreamSource( 2, EfnDataListItem->pPosBuff[iRightFrame],	0, sizeof(D3DXVECTOR3) ); // set to your second keyframe
				g_Graph.dev()->SetStreamSource( 3, EfnDataListItem->pNormBuff[iRightFrame],	0, sizeof(D3DXVECTOR3) ); // set to your second keyframe
				g_Graph.dev()->SetStreamSource( 4, EfnDataListItem->pUvBuff,				0, sizeof(CVERTEX_UV) ); // set to your second keyframe
				g_Graph.dev()->SetIndices( EfnDataListItem->pIB );

				// Draw the tweened model!
				g_Graph.SetWorld( pmatWorld );
				g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

				g_Graph.dev()->SetVertexDeclaration( NULL );
				g_Graph.dev()->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
			}
			return;
			break;
	}
}

void QMgrResEfnModels::RenderOOBB( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame )
{
	RenderOOBB( efn_id, pmatWorld, fFrame, NULL );
}

void QMgrResEfnModels::RenderOOBB( const std::wstring& efn_id, const D3DXMATRIX* pmatWorld, float fFrame, const D3DXCOLOR* pstColor )
{
	QEfnDataListItem* EfnDataListItem = GetEfn( efn_id );

	if( !EfnDataListItem )
		return;

	if( !EfnDataListItem->pcdData || ! ( EfnDataListItem->pOOBB_IB && EfnDataListItem->pOOBBPosBuff ) )
		return;

	EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;

	int iFrame = (int)fFrame;
	if ( iFrame >= 0 && iFrame < m_cdData->anim_frames_count )
	{
		if ( pstColor )
		{
			D3DMATERIAL9 m_stMtrl;
			ZeroMemory( &m_stMtrl, sizeof(D3DMATERIAL9) );
			m_stMtrl.Diffuse = m_stMtrl.Ambient = *pstColor;
			g_Graph.dev()->SetMaterial( &m_stMtrl );
		}
		else
		{
			g_Graph.dev()->SetMaterial( &EfnDataListItem->stMtrl );
		}

		g_Graph.dev()->SetVertexDeclaration( m_pVD_POS );

		g_Graph.dev()->SetStreamSource( 0, EfnDataListItem->pOOBBPosBuff[iFrame],	0, sizeof(D3DXVECTOR3) );
		g_Graph.dev()->SetIndices( EfnDataListItem->pOOBB_IB );

		g_Graph.SetWorld( pmatWorld );
		g_Graph.dev()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

		g_Graph.dev()->SetVertexDeclaration( NULL );
	}
}

void QMgrResEfnModels::GetEfnNullList( const std::wstring& efn_id, listWStr* plstWstr )
{
	if ( m_mEfn.size() > 0 && plstWstr)
	{
		plstWstr->clear();

		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			QEfnDataListItem* EfnDataListItem = i->second;

			if ( EfnDataListItem )
			{
				EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;

				if ( m_cdData )
				{
					for( mapWStrVecSNulls_iter i = m_cdData->nulls_map.begin(); i != m_cdData->nulls_map.end(); i++ )
					{
						plstWstr->push_back( i->first );
					}
				}
			}
		}
	}
}

bool QMgrResEfnModels::GetEfnNull( const std::wstring& efn_id, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, SNULL* pstNull )
{
	if ( m_mEfn.size() > 0 && pstNull != NULL)
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			return GetEfnNull( i->second, null_id, iLeftFrame, iRightFrame, fTweenFactor, pstNull );
		}
	}
	return false;
}

bool QMgrResEfnModels::GetEfnNull( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, SNULL* pstNull )
{
	if ( EfnDataListItem != NULL && pstNull != NULL)
	{
		EFN_CHARACTERDATA* m_cdData = EfnDataListItem->pcdData;

		if ( iLeftFrame >= 0 && iLeftFrame < m_cdData->anim_frames_count &&
			iRightFrame >= 0 && iRightFrame < m_cdData->anim_frames_count )
		{
			mapWStrVecSNulls_iter j = m_cdData->nulls_map.find(null_id);

			if ( j != m_cdData->nulls_map.end() )
			{
				SNULL* LeftFrameNull = &(j->second)[iLeftFrame];
				SNULL* RightFrameNull = &(j->second)[iRightFrame];

				pstNull->vRot =
					LeftFrameNull->vRot +
					D3DXVECTOR3( AngleDif( RightFrameNull->vRot.x, LeftFrameNull->vRot.x ),
								 AngleDif( RightFrameNull->vRot.y, LeftFrameNull->vRot.y ),
								 AngleDif( RightFrameNull->vRot.z, LeftFrameNull->vRot.z ) ) * fTweenFactor;
				D3DXVec3Lerp( &pstNull->vPos, &LeftFrameNull->vPos, &RightFrameNull->vPos, fTweenFactor );
				D3DXVec3Lerp( &pstNull->vScale, &LeftFrameNull->vScale, &RightFrameNull->vScale, fTweenFactor );
				return true;
			}
		}
	}
	return false;
}

bool QMgrResEfnModels::GetEfnNullMatrix( const std::wstring& efn_id, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, D3DXMATRIX* pMat )
{
	SNULL TempNull;
	if ( GetEfnNull( efn_id, null_id, iLeftFrame, iRightFrame, fTweenFactor, &TempNull ) && pMat != NULL )
	{
		D3DXMATRIX matTrans;
		D3DXMATRIX matRotX, matRotY, matRotZ;
		D3DXMATRIX matScale;
		D3DXMatrixTranslation( &matTrans, TempNull.vPos.x, TempNull.vPos.y, TempNull.vPos.z );
		D3DXMatrixRotationX( &matRotX, TempNull.vRot.x );
		D3DXMatrixRotationY( &matRotY, TempNull.vRot.y );
		D3DXMatrixRotationZ( &matRotZ, TempNull.vRot.z );
		D3DXMatrixScaling( &matScale, TempNull.vScale.x, TempNull.vScale.y, TempNull.vScale.z );

		*pMat = matScale * ( matRotX * matRotY * matRotZ ) * matTrans;
		return true;
	}

	D3DXMatrixIdentity( pMat );
	return false;
}

bool QMgrResEfnModels::GetEfnNullForMeshMorf( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, float fTweenFactor, float* pfRotY )
{
	if ( EfnDataListItem != NULL && pfRotY != NULL)
	{
		if ( iLeftFrame >= 0 && iLeftFrame < EfnDataListItem->pcdData->anim_frames_count &&
			iRightFrame >= 0 && iRightFrame < EfnDataListItem->pcdData->anim_frames_count )
		{
			mapWStrVecSNulls_iter i = EfnDataListItem->pcdData->nulls_map.find(null_id);

			if ( i != EfnDataListItem->pcdData->nulls_map.end() )
			{
				float LeftRotY = (i->second)[iLeftFrame].vRot.y;
				float RightRotY = (i->second)[iRightFrame].vRot.y;

				*pfRotY = -D3DXToDegree( LeftRotY + fTweenFactor * ( RightRotY - LeftRotY ) );
				return true;
			}
		}
	}
	return false;
}

bool QMgrResEfnModels::GetEfnNullFor3DVideo( const QEfnDataListItem* EfnDataListItem, const std::wstring& null_id, int iLeftFrame, int iRightFrame, int* piLeftFrame, int* piRightFrame )
{
	if ( EfnDataListItem != NULL && piLeftFrame != NULL && piRightFrame != NULL )
	{
		if ( iLeftFrame >= 0 && iLeftFrame < EfnDataListItem->pcdData->anim_frames_count &&
			iRightFrame >= 0 && iRightFrame < EfnDataListItem->pcdData->anim_frames_count )
		{
			mapWStrVecSNulls_iter i = EfnDataListItem->pcdData->nulls_map.find(null_id);

			if ( i != EfnDataListItem->pcdData->nulls_map.end() )
			{
				*piLeftFrame = (int)(i->second)[iLeftFrame].vScale.y;
				*piRightFrame = (int)(i->second)[iRightFrame].vScale.y;
				return true;
			}
		}
	}
	return false;
}

CSPHERE* QMgrResEfnModels::GetEfnSphere( const std::wstring& efn_id, float fFrame)
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			int m_iFrame = (int)fFrame;

			if(m_iFrame>=0 && m_iFrame < i->second->pcdData->anim_frames_count)
			{
				return &i->second->pcdData->pBoundingSpheres[m_iFrame];
			}
		}
	}
	return NULL;
}

D3DXVECTOR3* QMgrResEfnModels::GetEfnOOBB( const std::wstring& efn_id, float fFrame)
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			int m_iFrame = (int)fFrame;
			if(m_iFrame>=0 && m_iFrame < i->second->pcdData->anim_frames_count)
			{
				return i->second->pcdData->aabbList[m_iFrame];
			}
		}
	}
	return NULL;
}

int	QMgrResEfnModels::GetTriCount( const std::wstring& efn_id )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = m_mEfn.find(efn_id);

		if ( i != m_mEfn.end() )
		{
			QEfnDataListItem* temp = i->second;
			return temp->pcdData->vertex_indices_count/3;
		}
	}
	return -1;
}

//bool QMgrResEfnModels::GetTri( const std::wstring& efn_id, int iTriNum, D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3)
//{
//	if ( m_mEfn.size()>0 )
//	{
//		efnmap_iter i = m_mEfn.find(efn_id);
//
//		if ( i != m_mEfn.end() )
//		{
//			QEfnDataListItem* temp = i->second;
//			int i1 = temp->m_cdData->vertex_indices_list[iTriNum*3 + 0];
//			int i2 = temp->m_cdData->vertex_indices_list[iTriNum*3 + 1];
//			int i3 = temp->m_cdData->vertex_indices_list[iTriNum*3 + 2];
//
//			*v1 = D3DXVECTOR3(temp->m_cdData->vertecies_list[i1].x,temp->m_cdData->vertecies_list[i1].y,temp->m_cdData->vertecies_list[i1].z);
//			*v2 = D3DXVECTOR3(temp->m_cdData->vertecies_list[i2].x,temp->m_cdData->vertecies_list[i2].y,temp->m_cdData->vertecies_list[i2].z);
//			*v3 = D3DXVECTOR3(temp->m_cdData->vertecies_list[i3].x,temp->m_cdData->vertecies_list[i3].y,temp->m_cdData->vertecies_list[i3].z);
//
//			return true;
//		}
//	}
//	return false;
//}

QEfnDataListItem* QMgrResEfnModels::GetEfn( const std::wstring& efn_id )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			return i->second;
		}
	}
	return NULL;
}

inline efnmap_iter QMgrResEfnModels::GetEfnIter( const std::wstring& efn_id )
{
	if ( m_sEfnCache==efn_id )
	{
		return m_itEfnCache;
	}
	else
	{
		m_itEfnCache = m_mEfn.find(efn_id);
		m_sEfnCache = efn_id;
		return m_itEfnCache;
	}
}

std::wstring QMgrResEfnModels::GetFrameScript( const std::wstring& efn_id, int iFrame)
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			if ( i->second->pcdData && iFrame >= 0 && iFrame < i->second->pcdData->anim_frames_count)
			{
				return i->second->pcdData->frames_script[iFrame];
			}
			else
			{
				return L"";
			}
		}
	}
	return L"";
}

float QMgrResEfnModels::GetZOffset( const std::wstring& efn_id, int iFrame)
{
	if ( m_mEfn.size() > 0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			return GetZOffset(i->second, iFrame);
		}
	}
	return 0;
}

float QMgrResEfnModels::GetZOffset( const QEfnDataListItem *EfnDataListItem, int iFrame)
{
	if ( EfnDataListItem->pcdData && iFrame >= 0 && iFrame < EfnDataListItem->pcdData->anim_frames_count)
	{
		return EfnDataListItem->pcdData->z_coord_offset_list[iFrame].z;
	}
	else
	{
		return 0.0f;
	}
}

D3DMATERIAL9* QMgrResEfnModels::GetEfnMtrl( const std::wstring& efn_id )
{
	QEfnDataListItem* temp = GetEfn( efn_id );

	if ( temp )
		return &temp->stMtrl;
	else
		return NULL;

}

float QMgrResEfnModels::GetGlobalCylinderRadius( const std::wstring& efn_id )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			return i->second->pcdData->global_cylinder_radius;
		}
	}
	return 0.0f;
}

int QMgrResEfnModels::GetAnimCount( const std::wstring& efn_id )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			return i->second->pcdData->animations_count;
		}
	}
	return 0;
}

int QMgrResEfnModels::GetAnimFramesCount( const std::wstring& efn_id, const std::wstring& anim_name )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			mapWStrEFNANIM_iter j = i->second->pcdData->anim_list.find(anim_name);

			if ( j != i->second->pcdData->anim_list.end() )
			{
				return j->second.frames_count;
			}
		}
	}
	return 0;
}

bool QMgrResEfnModels::GetAnimFrame( const std::wstring& efn_id, const std::wstring& anim_name, const double fFrameIndex, int* piLeftFrame, int* piRightFrame, float* pfTweenFactor )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			int m_iLeftFrame;
			int m_iRightFrame;
			float m_fTweenFactor;

			int iFrameIndex = (int)fFrameIndex;

			if(anim_name==L"")
			{
				*piLeftFrame = iFrameIndex;
				*piRightFrame = iFrameIndex + 1;
				*pfTweenFactor = (float) (fFrameIndex - iFrameIndex);
				return false;
			}

			mapWStrEFNANIM_iter j = i->second->pcdData->anim_list.find(anim_name);

			if ( j != i->second->pcdData->anim_list.end() )
			{
				m_fTweenFactor = (float) (fFrameIndex - iFrameIndex);

				if ( iFrameIndex < j->second.frames_count )
				{
					m_iLeftFrame = j->second.frames_list[iFrameIndex];
				}
				else
				{
					m_iLeftFrame = j->second.frames_list[0];
				}

				if ( iFrameIndex + 1 < j->second.frames_count )
				{
					m_iRightFrame = j->second.frames_list[iFrameIndex+1];
				}
				else
				{
					m_iRightFrame = j->second.frames_list[0];
				}

				if(pfTweenFactor)*pfTweenFactor = m_fTweenFactor;
				if(piLeftFrame)*piLeftFrame = m_iLeftFrame;
				if(piRightFrame)*piRightFrame = m_iRightFrame;

				return true;
			}
		}
	}
	if(pfTweenFactor)*pfTweenFactor = 0;
	if(piLeftFrame)*piLeftFrame = 0;
	if(piRightFrame)*piRightFrame = 0.0f;
	return false;
}

EFN_CHARACTERANIMATION* QMgrResEfnModels::GetAnim( const std::wstring& efn_id, const std::wstring& anim_name )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			mapWStrEFNANIM_iter j = i->second->pcdData->anim_list.find(anim_name);

			if ( j != i->second->pcdData->anim_list.end() )
			{
				return &j->second;
			}
		}
	}
	return NULL;
}

float QMgrResEfnModels::GetAnimTimeLength( const std::wstring& efn_id, const std::wstring& anim_name )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			mapWStrEFNANIM_iter j = i->second->pcdData->anim_list.find(anim_name);

			if ( j != i->second->pcdData->anim_list.end() )
			{
				return j->second.m_fAnimTimeLength;
			}
		}
	}
	return 0;
}

float QMgrResEfnModels::GetAnimFrameRate( const std::wstring& efn_id )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			return i->second->pcdData->fFPS;
		}
	}
	return 0;
}

float QMgrResEfnModels::GetAnimZLength( const std::wstring& efn_id, const std::wstring& anim_name )
{
	if ( m_mEfn.size()>0 )
	{
		efnmap_iter i = GetEfnIter(efn_id);

		if ( i != m_mEfn.end() )
		{
			mapWStrEFNANIM_iter j = i->second->pcdData->anim_list.find(anim_name);

			if ( j != i->second->pcdData->anim_list.end() )
			{
				return j->second.m_fAnimZLength;
			}
		}
	}
	return 0;
}
