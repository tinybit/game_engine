#define STRICT

#include "QGameQuestSceneData.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "QMgrMiniGames.h"
#include "QMgrScripts.h"
#include "IMgrResFiles.h"
#include "IMgrResEfnModels.h"
#include "IMgrResTextures.h"
#include "QMgrResObjects.h"
#include "QMgrResStates.h"

#include "QObjBase.h"
#include "QObjMan.h"
#include "QObjCar.h"
#include "QObjBird.h"
#include "QObjCamera.h"
#include "QObjLight.h"
#include "QObjShadowVolume.h"
#include "QObjLayerGeom.h"
#include "QObjAction.h"
#include "QObjEfnAnim.h"
#include "QObjVideoAnim.h"
#include "QObj3DVideo.h"

//-----------------------------------------------------------------------------
// Name: QQuestSceneData()
// Desc: QQuestSceneData class constructor
//-----------------------------------------------------------------------------
QGameQuestSceneData::QGameQuestSceneData()
{
	pGlobalMatrix	= NULL;
	pSrcGlobalMatrix= NULL;
	pCamMatrix		= NULL;
	pShadowMatrix	= NULL;
	pTriGrid		= NULL;

	m_sSceneName	= L"";
	m_sEfnID		= L"";

	m_pSearch		= NULL;
	m_search_use	= 0;
	m_search_count	= 20; 

	m_bSceneLoaded	= false;
}

//-----------------------------------------------------------------------------
// Name: ~QQuestSceneData()
// Desc: QQuestSceneData class destructor
//-----------------------------------------------------------------------------
QGameQuestSceneData::~QGameQuestSceneData()
{
	FreeScene();
}

void QGameQuestSceneData::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage( L"QuestSceneData logging enabled");
	}
	else
	{
		g_Log.WriteMessage( L"QuestSceneData logging disabled");
	}
}

bool QGameQuestSceneData::LoadScene( const std::wstring& scene_id )
{
	g_Graph.RenderLoad( L"Load scene data: " + scene_id );

	//FreeScene();

	m_sEfnID = L"Scenes\\" + scene_id + L".geom";
	//Load meshes for scene
	g_EfnMgr.LoadEfn( m_sEfnID );

	/////////////////////////////
	// READ SCENE //
	/////////////////////////////
	std::wstring datafile = L"data\\Models\\Scenes\\" + scene_id + L".scn";

	if (!g_FileMgr.FileExists( datafile ))
	{
		MessageBox(NULL, L"Не найден файл данных сцены", L"Error", MB_OK);
		return false;
	}

	streampos pos;

	//open scn file
	ifstream file( wstr2str(datafile).c_str(), ios::binary);
	file.seekg( pos );

	try
	{
		//scn version
		DWORD scene_version;
		ReadStrm( file, scene_version );

		//read scene id
		ReadStrm( file, m_sSceneName );
		m_sSceneName = scene_id;

		//read cameras count
		ReadStrm( file, m_iCamsCount );

		CCAMERA* cams = new CCAMERA[m_iCamsCount];
		file.read( (char *)cams, sizeof( CCAMERA )*m_iCamsCount );
		for( int i = 0; i < m_iCamsCount; i++ )
		{
			cams[i].fov.x	= D3DXToRadian( cams[i].fov.x );  
			cams[i].fov.y	= FovX2FovY( cams[i].fov.x, 4.0f/3.0f );

			if ( GetObjMgr()->AddObjCam( L"Camera_"+str2wstr(cams[i].name) ) )
			{
				QObjCamera* temp_cam = GetObjMgr()->GetObjCam( L"Camera_"+str2wstr(cams[i].name) );
				temp_cam->SetPerCamPrm( &cams[i].pos, &cams[i].targ, &cams[i].upvec, 
									&D3DXVECTOR2( g_Graph.fWidth, g_Graph.fHeight ),
									0.1f, 100.0f, cams[i].fov.y );
				temp_cam->SetZoomPrm( cams[i].zoom_near, cams[i].zoom_far );
			}
		}
		SAFE_DELETE_ARRAY(cams);

		//read lights count
		ReadStrm( file, m_iLightsCount );
		CLIGHT* lights = new CLIGHT[m_iLightsCount];
		D3DLIGHT9 light;
		file.read( (char *)lights, sizeof( CLIGHT )*m_iLightsCount );
		for( int i = 0; i < m_iLightsCount; i++ )
		{
			int tp = 0;
			
			ZeroMemory(&light, sizeof(D3DLIGHT9));
			light.Position = lights[i].pos;
			light.Direction = (D3DVECTOR)( lights[i].dir - lights[i].pos );
			switch ((LIGHT_TYPE)lights[i].type)
			{
				case LT_DIRECTED:
					light.Type = D3DLIGHT_DIRECTIONAL;
					break;
				
				case LT_SPOT:
					light.Type = D3DLIGHT_DIRECTIONAL;
					//light.Type = D3DLIGHT_SPOT;
					break;
				
				default: // LT_POINT:
					light.Type = D3DLIGHT_POINT;
					break;
			}
			light.Diffuse.r = ( lights[i].diffuse.r / 255) * lights[i].multiplier;
			light.Diffuse.g = ( lights[i].diffuse.g / 255) * lights[i].multiplier;
			light.Diffuse.b = ( lights[i].diffuse.b / 255) * lights[i].multiplier;
			light.Diffuse.a = 0.0f;
			
			light.Specular = light.Diffuse;

			light.Ambient.r = ( lights[i].ambient.r / 255) * lights[i].multiplier;
			light.Ambient.g = ( lights[i].ambient.g / 255) * lights[i].multiplier;	
			light.Ambient.b = ( lights[i].ambient.b / 255) * lights[i].multiplier;
			light.Attenuation0	= 	0.0f;
			light.Attenuation1	=	lights[i].attenuation;
			light.Attenuation2	=	0.0f;
			light.Range			=	lights[i].range;
			light.Falloff		= 	0.0f;
			light.Phi			=	0.0f;
			light.Theta			=	0.0f;

			GetObjMgr()->AddObjLight( L"Light_"+str2wstr(lights[i].name), &light );
		}
		SAFE_DELETE_ARRAY(lights);

		//Read tex count
		ReadStrm( file, m_iTexCount );
		for( int i = 0; i < m_iTexCount; i++)
		{
			std::wstring tex_id;
			ReadStrm( file, tex_id );
		}

		vecWStr s;
		s.reserve( 10 );
		//Read binds count
		ReadStrm( file, m_iBindsCount );
		for( int i = 0; i < m_iBindsCount; i++ )
		{
			int	bind_params_count;
			ReadStrm( file, bind_params_count );
			s.clear();
			
			for(int j = 0; j < bind_params_count; j++)
			{
				std::wstring m_sTemp;
				ReadStrm( file, m_sTemp );
				s.push_back(m_sTemp);
			}

			AddBinds( s );
		}

		//Load grids
		if( LoadGlobalMatrix( file ) )
		{
			file.close();
			m_bSceneLoaded = true;
			return true;
		}
		else
		{
			file.close();
			m_bSceneLoaded = false;
			return false;
		}
	}
	catch( char* str )
	{
		file.close();
		g_Log.WriteError( L"error on scene loading, err_text = \"" + str2wstr(str) + L"\"" );
		m_bSceneLoaded = false;
		return false;
	}
}

void QGameQuestSceneData::FreeScene()
{
	g_Graph.RenderLoad( L"Free scene data: " + m_sSceneName );

	m_bSceneLoaded = false;

	GetObjMgr()->DelAll();
	//g_TexMgr.SetActives( L"Quest.Backgrounds", false );
	g_TexMgr.FreeTexs( L"Quest.Backgrounds" );

	m_search_use = 0;
	if ( m_pSearch )
	{
		for( int i = 0; i < m_search_count; i++)
		{
			SAFE_DELETE_ARRAY(m_pSearch[i].m_search);
			SAFE_DELETE_ARRAY(m_pSearch[i].arr);
			m_pSearch[i].p.clear();
		}
		SAFE_DELETE_ARRAY(m_pSearch);
	}
	SAFE_DELETE_ARRAY(pShadowMatrix);
	SAFE_DELETE_ARRAY(pGlobalMatrix);
	SAFE_DELETE_ARRAY(pGlobalMatrix);
	SAFE_DELETE_ARRAY(pCamMatrix);

	if ( pTriGrid )
	{
		for( int j = 0; j < iZSize; j++ )
		{
			for( int i = 0; i < iXSize; i++ )
			{
				SAFE_DELETE_ARRAY( pTriGrid[i+j*iXSize] );
			}
		}

		SAFE_DELETE_ARRAY( pTriGrid );
	}
	
	m_sSceneName = L"";
	m_sEfnID = L"";
}

bool QGameQuestSceneData::LoadGlobalMatrix( ifstream &file )
{
	D3DXVECTOR2 temp;
	ReadStrm( file, temp );
	scene_pos = D3DXVECTOR3(temp.x, 0, temp.y);
	ReadStrm( file, temp );
	scene_max = D3DXVECTOR3(temp.x, 0, temp.y);
	scene_size = scene_max - scene_pos;
	
	// load cell size
	ReadStrm( file, m_fCellSize );

	// load floor size in cells
	ReadStrm( file, iXSize );
	ReadStrm( file, iZSize );

	UINT t = timeGetTime();
						   
	//allocate data
	pTriGrid = new int*[iXSize*iZSize];

	//load data
	for( int i = 0; i < iXSize; i++ )
	{
		for( int j = 0; j < iZSize; j++ )
		{
			int array_size;
			ReadStrm( file, array_size );

			pTriGrid[ i + j*iXSize ] = new int[array_size];

			file.read((char *)pTriGrid[ i + j*iXSize ], sizeof(int) * array_size);
		}
	}

	//---------------------make path finding matrix------------------------------------------
	pGlobalMatrix = new UINT[ iXSize * iZSize ];
	pSrcGlobalMatrix = new UINT[ iXSize * iZSize ];
	pCamMatrix = new UINT[ iXSize * iZSize ];
	pShadowMatrix = new UINT[ iXSize * iZSize ];
	for( UINT z = 0; z < iZSize; z++ )
	{
		for( UINT x = 0; x < iXSize; x++ )
		{
			pShadowMatrix[ x + z * iXSize ] = 0;
			pCamMatrix[ x + z * iXSize ] = pTriGrid[x+z*iXSize][0];
			pSrcGlobalMatrix[ x + z * iXSize ] = ( ( pTriGrid[x+z*iXSize][1]==0 ) ? CANTWALK : CANWALK );
			pGlobalMatrix[ x + z * iXSize ] = CANWALK;
		}
	}
	OptimizeGlobalMatrix( pSrcGlobalMatrix );

	m_search_use = 0;
	m_pSearch = new SSearch[m_search_count];
	for( int i = 0; i < m_search_count; i++)
	{
		m_pSearch[i].m_search = new UINT[iXSize*iZSize];
		m_pSearch[i].arr = new pt[8];
		m_pSearch[i].p.clear();
		m_pSearch[i].p.reserve(iXSize * iZSize);
	}

	if ( g_Graph.GetShadowQuality() > 0 )
	{
		int iDiv = 1;
		switch(g_Graph.GetShadowQuality())
		{
			case 1:	//25%
				iDiv = 4;
				break;

			case 2:	//50%
				iDiv = 2;
				break;

			default: //100%
				iDiv = 1;
				break;
		}

		if( scene_size.x >= 24.0f)
		{
			m_iShadowTexWidth = 1024 / iDiv;
		}
		else
		{
			m_iShadowTexWidth = 512 / iDiv;
		}

		if( scene_size.z >= 24.0f)
		{
			m_iShadowTexHeight = 1024 / iDiv;
		}
		else
		{
			m_iShadowTexHeight = 512 / iDiv;
		}
	}

	return true;
}

void QGameQuestSceneData::OptimizeGlobalMatrix( UINT* pTemp )
{
	UINT* m_pTemp = new UINT[ iXSize * iZSize ];
	CopyMemory( m_pTemp, pTemp, iXSize * iZSize * sizeof(int) );

	for( int z = 0; z < iZSize; z++ )
	{
		for( int x = 0; x < iXSize; x++ )
		{
			if ( m_pTemp[ x + z * iXSize ]==CANTWALK )
			{
				pTemp[ x + z * iXSize ] = ( CheckRectIsCanWalkExists(m_pTemp, x, z) ? CANTWALK : CANTWALK2 );
			}
		}
	}

	SAFE_DELETE_ARRAY(m_pTemp);
}

bool QGameQuestSceneData::CheckRectIsCanWalkExists( UINT* pTemp, int x, int z )
{
	for(int mz = z-1; mz <= z+1; mz++)
	{
		for(int mx = x-1; mx <= x+1; mx++)
		{
			if( mx>=0 && mx < iXSize && mz >=0 && mz < iZSize && mx != x && mz != z )
			{
				if ( pTemp[ mx + mz * iXSize ] == CANWALK )
					return true;
			}
		}
	}
	return false;
}

inline void QGameQuestSceneData::CheckFindPlace( UINT* grid, vector<iVECTOR2>* p, int x, int z, UINT num, bool* m_path_finded )
{
	if ( !(*m_path_finded) && x >=0 && x < iXSize && z >= 0 && z < iZSize )
	{
		UINT* qwe = &grid[ x + z * iXSize ];
		switch( *qwe )
		{
			//Если  нашли точку проходимости то заполним ее номером итерации поиска
			case CANWALK: 
				*qwe = num;
				iVECTOR2 t_p;	t_p.x = x; 	t_p.y = z;
				p->push_back(t_p);
				break;

			//Если нашли точку старта значит путь найден, выставлем флаг
			case STARTPOINT:
				*m_path_finded = true;
				break;
		}
	}
}

inline void QGameQuestSceneData::CheckPointForOK( const UINT* m_search, const D3DXVECTOR3* pvOrig, int x, int z, D3DXVECTOR3* pvNew, float* pfDist, int* piDiap )
{
	if( x>=0 && x < iXSize && z>=0 && z < iZSize)
	{
		if (m_search[ x + z * iXSize ]==CANWALK)
		{
			D3DXVECTOR3 m_vTemp = GetPosFromGrid2(x, z);
			float fTemp;
			if ( ( fTemp = D3DXVec3Length( &(m_vTemp-*pvOrig) ) ) < *pfDist )
			{
				
				if ( *pfDist == 999999 )
					*piDiap = fTemp / GetCellSize();

				*pvNew = m_vTemp;
				*pfDist = fTemp;
			}
		}
	}
}

bool QGameQuestSceneData::GetClosestPosition( const UINT* m_search, D3DXVECTOR3* pvTarget )
{
	int x;
	int z;
	GetGridFromPos( pvTarget, x, z );
	D3DXVECTOR3 m_vOrig(pvTarget->x, 0, pvTarget->z);
	D3DXVECTOR3 m_vNew, m_vTemp;
	float m_fDist = 999999;
	int m_iDiap = 3 / GetCellSize();

	if ( m_search[ x + z * iXSize]==CANWALK )
	{
		return true;
	}
	else
	{
		for( int i = 1; i < m_iDiap; i++ )
		{
			for( int j = -i; j<=i; j++ )
			{
				CheckPointForOK( m_search, &m_vOrig, x + j, z + i, &m_vNew, &m_fDist, &m_iDiap );
				CheckPointForOK( m_search, &m_vOrig, x + j, z - i, &m_vNew, &m_fDist, &m_iDiap );
				CheckPointForOK( m_search, &m_vOrig, x + i, z + j, &m_vNew, &m_fDist, &m_iDiap );
				CheckPointForOK( m_search, &m_vOrig, x - i, z + j, &m_vNew, &m_fDist, &m_iDiap );
			}
		}
		if ( m_fDist == 999999 )
		{
			return false;
		}
		else
		{
			*pvTarget = m_vNew;
			return true;
		}
	}
}

bool QGameQuestSceneData::ManFindPath( const std::wstring& obj_id, const SNULL* pstTarget)
{
	QObjMan* man = GetObjMgr()->GetObjMan(obj_id);

	if ( man )
	{
		return FindPath( man->GetNullPos(), pstTarget, 
						 man->GetGopa(), man->GetGopaSize(), man->GetCP() );
	}
	else
	{
		return false;
	}
}

bool QGameQuestSceneData::CarFindPath( const std::wstring& obj_id, const SNULL* pstTarget)
{
	QObjCar* car = GetObjMgr()->GetObjCar(obj_id);

	if ( car )
	{
		return FindPath( car->GetNullPos(), pstTarget, 
						 car->GetGopa(), car->GetGopaSize(), car->GetCP() );
	}
	else
	{
		return false;
	}
}

bool QGameQuestSceneData::BirdFindPath( const std::wstring& obj_id, const SNULL* pstTarget)
{
	QObjBird* bird = GetObjMgr()->GetObjBird(obj_id);

	if ( bird )
	{										 
		return FindPath( bird->GetNullPos(), pstTarget, 
						 bird->GetGopa(), bird->GetGopaSize(), bird->GetCP() );
	}
	else
	{
		return false;
	}
}

bool QGameQuestSceneData::FindPath( const SNULL* pstStart, const SNULL* pstTarget, bool* pGopa, int iGopaSize, cpvector* cp )
{
	//Выделяем память для поиска из кеша для поиска, чтобы не работать с ывделением памяти
	SSearch* pSearch = NULL;
	if ( m_search_use < m_search_count)
		pSearch = &m_pSearch[m_search_use++];
	else
		return false;

	UINT* m_search = pSearch->m_search;

	SNULL m_stTarget = *pstTarget;

	int start_x;
	int start_z;
	//Получаем координаты точки на сетке куда мы идем
	GetGridFromPos( &pstStart->vPos, start_x, start_z );

	//Чистим путь
	cp->clear();

	//освобождаем глобальную сетку от своей жопы
	FillGopa( start_x, start_z, pGlobalMatrix, pGopa, iGopaSize, CANWALK);

	//заполняем жопой сетку поиска
	FillMSearch(m_search, pGlobalMatrix, pSrcGlobalMatrix, pGopa, iGopaSize);

	//заполняем жопу обратно в глобальную сетку
	FillGopa( start_x, start_z, pGlobalMatrix, pGopa, iGopaSize, CANTWALK);

	//ищем ближайшую проходимую точку куда мы пойдем
	if ( !GetClosestPosition( m_search, &m_stTarget.vPos ) )
	{
		//Если ненашли то откатываем кэш
		//неправильная версия кэша ПЕРДЕЛАТЬ!!!!!!!!!!!!!
		m_search_use--;
		return false;
	}

	int target_x;
	int target_z;
	//после нахождения ближайшей точки находим ее представление на сетке
	GetGridFromPos( &m_stTarget.vPos, target_x, target_z );
	m_stTarget.vPos = GetPosFromGrid( target_x, target_z );

	//Если мы уже в этой точке то ничего не ищем
	if ( start_x==target_x && start_z==target_z )
	{
		m_search_use--;
		return false;
	}

	//Проверим сначала по прямой
	iVECTOR2 tempS, tempE;
	tempS.x = start_x;  tempS.y = start_z;
	tempE.x = target_x; tempE.y = target_z;

	//если нашли путь по прямой то заполняем путь двумя точками начала, конца и выходим
	if ( checkline( m_search, tempS, tempE ) )
	{
		SCheckPoint temp;
		
		temp.p.vScale=D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		temp.p.vPos	= pstStart->vPos;
		temp.p.vRot = D3DXVECTOR3( 0.0f, atan2f( m_stTarget.vPos.x - pstStart->vPos.x, m_stTarget.vPos.z - pstStart->vPos.z ), 0.0f );
		temp.g.x	= start_x;
		temp.g.y	= start_z;
		temp.l		= 0;
		cp->push_back(temp);

		temp.p.vScale=D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		temp.p.vPos	= m_stTarget.vPos;
		temp.p.vRot = D3DXVECTOR3( 0.0f, atan2f( m_stTarget.vPos.x - pstStart->vPos.x, m_stTarget.vPos.z - pstStart->vPos.z ), 0.0f );
		temp.g.x	= target_x;
		temp.g.y	= target_z;
		temp.l		= D3DXVec2Length( &( D3DXVECTOR2( m_stTarget.vPos.x, m_stTarget.vPos.z ) - 
							             D3DXVECTOR2( pstStart->vPos.x, pstStart->vPos.z ) ) );
		cp->push_back(temp);

		m_search_use--;
		return true;
	}

	//Если ненашли по прямой то ищем волновым методом

	//Заполнили место старта
	m_search[ start_x + start_z * iXSize ] = STARTPOINT;

	//Заполнили место цели
	m_search[ target_x + target_z * iXSize ] = TARGETPOINT;
	
	//погнали
	UINT num = 1;

	vector<iVECTOR2>* p = &pSearch->p;

	iVECTOR2 t_p;

	bool m_path_finded = false;
	bool m_path_notfinded = false;
	UINT num_start = 0;
	UINT old_size = 0;

	//Заполнили в массив точку конца
	t_p.x = target_x;
	t_p.y = target_z;
	p->clear();
	p->push_back(t_p);

	//Ищем путь пока не получим флаг что нашли или ненашли
	while ( !m_path_finded && !m_path_notfinded && num < STARTPOINT )
	{
		//Проверяем что есть новодобавленные точки
		if ( p->size() > old_size )
		{
			num_start = old_size;
			old_size = p->size();

			int x, z;
			//Бежим по всем новым точкам и ищем id начала пути
			for( UINT i = num_start; i < old_size && !m_path_finded; i++ )
			{
				x = (*p)[i].x;
				z = (*p)[i].y;

				CheckFindPlace( m_search, p, x - 1, z - 1, num, &m_path_finded );
				CheckFindPlace( m_search, p, x - 1, z - 0, num, &m_path_finded );
				CheckFindPlace( m_search, p, x - 1, z + 1, num, &m_path_finded );
				CheckFindPlace( m_search, p, x - 0, z + 1, num, &m_path_finded );
				CheckFindPlace( m_search, p, x + 1, z + 1, num, &m_path_finded );
				CheckFindPlace( m_search, p, x + 1, z - 0, num, &m_path_finded );
				CheckFindPlace( m_search, p, x + 1, z - 1, num, &m_path_finded );
				CheckFindPlace( m_search, p, x - 0, z - 1, num, &m_path_finded );
			}
		}
		//Если нету новых точек и нет флага что путь найдем, значит он ненайден
		else
		{
			m_path_notfinded = true;
		}
		num++;
	}

	p->clear();

	//Если путь не нашли
	if ( m_path_notfinded ) 
	{
		//g_Log.WriteError(L"cant find path");
		m_search_use--;
		return false;
	}
	
	//Теперь строим массив с путем
	num = 0;
	SCheckPoint temp;
	temp.l = 0.0f;
	temp.g.x = start_x;
	temp.g.y = start_z;
	temp.p.vPos = pstStart->vPos;
	cp->push_back(temp);

	int x = start_x;
	int z = start_z;
	
	temp.p.vPos = pstStart->vPos;
	float l_temp = 0.0f;

	pt* arr = pSearch->arr;
	pt temp_pt;
	
	do
	{
		arr[0].v.x = x - 1;		arr[0].v.y = z - 0;		arr[0].a = m_search[ arr[0].v.x + arr[0].v.y * iXSize ];
		arr[1].v.x = x - 0;		arr[1].v.y = z + 1;		arr[1].a = m_search[ arr[1].v.x + arr[1].v.y * iXSize ];
		arr[2].v.x = x + 1;		arr[2].v.y = z - 0;		arr[2].a = m_search[ arr[2].v.x + arr[2].v.y * iXSize ];
		arr[3].v.x = x - 0;		arr[3].v.y = z - 1;		arr[3].a = m_search[ arr[3].v.x + arr[3].v.y * iXSize ];

		arr[4].v.x = x - 1;		arr[4].v.y = z - 1;		arr[4].a = m_search[ arr[4].v.x + arr[4].v.y * iXSize ];
		arr[5].v.x = x - 1;		arr[5].v.y = z + 1;		arr[5].a = m_search[ arr[5].v.x + arr[5].v.y * iXSize ];
		arr[6].v.x = x + 1;		arr[6].v.y = z + 1;		arr[6].a = m_search[ arr[6].v.x + arr[6].v.y * iXSize ];
		arr[7].v.x = x + 1;		arr[7].v.y = z - 1;		arr[7].a = m_search[ arr[7].v.x + arr[7].v.y * iXSize ];

		temp_pt.a = STARTPOINT;
		for( int i = 0; i < 8; i++ )
		{
			if ( temp_pt.a > arr[i].a )
			{
				temp_pt = arr[i];
			}
		}

		x = temp_pt.v.x;
		z = temp_pt.v.y;

		temp.p.vPos	= GetPosFromGrid2( x, z );
		temp.g.x	= x;
		temp.g.y	= z;
		
		cp->push_back( temp );
		
		num++;
	} while ( m_search[ x + z * iXSize ] != TARGETPOINT && num < STARTPOINT);

	if ( num >= STARTPOINT ) 
	{
		cp->clear();
		m_search_use--;
		return false;
	}
	else
	{
		OptimizePath( m_search, *cp );
		m_search_use--;
		return true;
	}
}

void QGameQuestSceneData::OptimizePath( UINT* m_search, cpvector& cp )
{
	cpvector n;

	n.clear();
	n.push_back( cp[0] );

	bool ok = false;
	for(int i = 0; i <= cp.size()-2;)
	{
		int j = i+1;
		int k = j;
		while( (j <= cp.size()-2) )
		{
			if(checkline(m_search, cp[i].g, cp[j+1].g))
			{
				k=j+1;
			}
			j++;
		}
		if ( k <= cp.size()-2 )
		{
			n.push_back( cp[k] );
		}
		i = k;
	}

	n.push_back( cp[cp.size()-1] );

	//пересчитали расстояния
	float l = 0;
	for(int i = 0; i <= n.size()-1; i++)
	{
		if(i==0)
		{
			l = 0;
		}
		else
		{
			l += D3DXVec2Length( &( D3DXVECTOR2( n[i].p.vPos.x, n[i].p.vPos.z ) - 
				                    D3DXVECTOR2( n[i-1].p.vPos.x, n[i-1].p.vPos.z ) ) );
		}
		n[i].l = l;
		n[i].p.vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		
		if(i<n.size()-1)
		{
			n[i].p.vRot = D3DXVECTOR3( 0.0f, atan2f( n[i+1].p.vPos.x - n[i].p.vPos.x, n[i+1].p.vPos.z - n[i].p.vPos.z ), 0.0f );
		}
		else
		{
			n[i].p.vRot = n[i-1].p.vRot;
		}
	}

	cp.clear();
	cp = n;
	n.clear();
}

bool QGameQuestSceneData::checkline( UINT* m_search, iVECTOR2 n, iVECTOR2 k )
{
	D3DXVECTOR2 a(n.x,n.y);
	D3DXVECTOR2 b(k.x,k.y);

	int minx = (n.x < k.x) ? n.x : k.x;
	int maxx = (n.x > k.x) ? n.x : k.x;
	int minz = (n.y < k.y) ? n.y : k.y;
	int maxz = (n.y > k.y) ? n.y : k.y;

    for( int i = minx; i <= maxx; i++ )
	{
		for( int j = minz; j <= maxz; j++ )
		{
			if ( m_search[i + j * iXSize] == CANTWALK || m_search[i + j * iXSize] == CANTWALK2 )
			{
				if ( segment2segment( &a, &b, &D3DXVECTOR2(i-0.5f,j-0.5f), &D3DXVECTOR2(i-0.5f,j+0.5f) ) )
				{
					return false;
				}
				if ( segment2segment( &a, &b, &D3DXVECTOR2(i-0.5f,j+0.5f), &D3DXVECTOR2(i+0.5f,j+0.5f) ) )
				{
					return false;
				}
				if ( segment2segment( &a, &b, &D3DXVECTOR2(i+0.5f,j+0.5f), &D3DXVECTOR2(i+0.5f,j-0.5f) ) )
				{
					return false;
				}
				if ( segment2segment( &a, &b, &D3DXVECTOR2(i+0.5f,j-0.5f), &D3DXVECTOR2(i-0.5f,j-0.5f) ) )
				{
					return false;
				}
			}
		}	
	}

	return true;
}

bool QGameQuestSceneData::segment2segment( const D3DXVECTOR2* a, const D3DXVECTOR2* b, const D3DXVECTOR2* c, const D3DXVECTOR2* d )
{
	float z = D3DXVec2Dot( &(*c-*d), &dot( &(*b-*a) ) );
	if (z == 0 )
	{
		return false;
	}
	float r = D3DXVec2Dot( &(*c-*a), &dot( &(*d-*c) ) ) / z;
	float s = D3DXVec2Dot( &(*c-*a), &dot( &(*b-*a) ) ) / z;
	
	if ( (0<=r && r<=1) && (0<=s && s<=1) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool QGameQuestSceneData::MakeGopa( bool** pGopa, int* iGopaSize, float fGopaRadius )
{
	SAFE_DELETE_ARRAY(*pGopa);
	int m_iGopaSize = *iGopaSize = (fGopaRadius > m_fCellSize/2) ? ( int((fGopaRadius - m_fCellSize/2) / m_fCellSize) + 1 ) * 2 + 1 : 1;

	//if ( m_iGopaSize==1 )
	//{
	//	return false;
	//}
	//else
	//{
		bool* m_pGopa = new bool[m_iGopaSize*m_iGopaSize];
		*pGopa = m_pGopa;

		float c = int(m_iGopaSize/2) * m_fCellSize;

		float new_radius = (fGopaRadius * fGopaRadius) + sqrtf((m_fCellSize*m_fCellSize) * 2)/3;

		//Делаем жопу
		for( int z = 0; z < m_iGopaSize; z++)
		{
			for( int x = 0; x < m_iGopaSize; x++)
			{
				float m_x = int(x) * m_fCellSize;
				float m_z = int(z) * m_fCellSize;

				if ( (c-m_x)*(c-m_x) + (c-m_z)*(c-m_z) <= new_radius )
				{
					m_pGopa[ z * m_iGopaSize + x] = true;
				}
				else
				{
					m_pGopa[ z * m_iGopaSize + x] = false;
				}
			}
		}
	//}
	return true;
}

void QGameQuestSceneData::FillGopa( int x, int z, UINT* m_search, bool* pGopa, int iGopaSize, UINT iOp )
{
	int cells_center = (iGopaSize > 1) ? iGopaSize * 0.5f : 0;
	int gm_x, gm_z, gopa_x, gopa_z;
	int i, j;

	for( j = -cells_center; j <= cells_center; j++ )
	{
		for( i = -cells_center; i <= cells_center; i++ )
		{
			gm_x = x + i;
			gm_z = z + j;

			gopa_x = i + cells_center;
			gopa_z = j + cells_center;

			if ( gm_x >= 0 && gm_x < iXSize && gm_z >= 0 && gm_z < iZSize )
			{
				if ( pGopa[ gopa_z * iGopaSize + gopa_x ] )
				{
					m_search[ gm_z * iXSize + gm_x ] = iOp;
				}
			}
		}
	}
}

void QGameQuestSceneData::FillGopa2( int x, int z, UINT* m_search, bool* gopa, int cells_count, UINT iOp )
{
	int cells_center = (cells_count > 1) ? cells_count / 2 : 0;
	int gm_x, gm_z, gopa_x, gopa_z;
	int i, j;

	for( j = -cells_center; j <= cells_center; j++ )
	{
		for( i = 0; i <= cells_center; i++ )
		{
			gm_x = x + i;
			gm_z = z + j;

			gopa_x = i + cells_center;
			gopa_z = j + cells_center;

			if ( gm_x >= 0 && gm_x < iXSize && gm_z >= 0 && gm_z < iZSize )
			{
				if ( gopa[ gopa_z * cells_count + gopa_x ] )
				{
					m_search[ gm_z * iXSize + gm_x ] = iOp;
				}
			}
		}
	}
}

void QGameQuestSceneData::FillGopa3( int x, int z, UINT* m_search, bool* gopa, int cells_count, UINT iOp )
{
	int cells_center = (cells_count > 1) ? cells_count * 0.5f : 0;
	int gm_x, gm_z, gopa_x, gopa_z;
	int i, j;

	for( j = 0; j <= cells_center; j++ )
	{
		for( i = -cells_center; i <= cells_center; i++ )
		{
			gm_x = x + i;
			gm_z = z + j;

			gopa_x = i + cells_center;
			gopa_z = j + cells_center;

			if ( gm_x >= 0 && gm_x < iXSize && gm_z >= 0 && gm_z < iZSize )
			{
				if ( gopa[ gopa_z * cells_count + gopa_x ] )
					m_search[ gm_z * iXSize + gm_x ] = iOp;
			}
		}
	}
}

bool QGameQuestSceneData::TestGopa( int x, int z, UINT* m_search, bool* gopa, int cells_count )
{
	bool ok = true;
	int cells_center = ( (cells_count > 1) ? cells_count * 0.5f : 0 );
	cells_center = ( ( cells_center != 0 ) ? cells_center - 1 : 0 );
	int gm_x, gm_z, gopa_x, gopa_z;
	int i, j;

	for( j = -cells_center; j <= cells_center; j++ )
	{
		for( i = -cells_center; i <= cells_center; i++ )
		{
			gm_x = x + i;
			gm_z = z + j;

			gopa_x = i + cells_center;
			gopa_z = j + cells_center;

			if ( gm_x>=0 && gm_x<iXSize && gm_z>=0 && gm_z<iZSize )
			{
				if ( gopa[ gopa_z * cells_count + gopa_x ] )
				{
					if ( m_search[ gm_z * iXSize + gm_x ] != CANWALK )
						ok = false;
				}
			}
		}
	}

	return ok;
}

void QGameQuestSceneData::FillMSearch( UINT* m_search, UINT* pGlb, UINT* pSrc, bool* pGopa, int iGopaSize )
{
	if(!pGopa)
	{
		g_Log.WriteError( L"pGopa == NULL in void QGameQuestSceneData::FillMSearch( UINT* m_search, UINT pGlb, UINT* pSrc, bool* pGopa, int iGopaSize )" );
		return;
	}

	for( int i = 0; i < iXSize * iZSize; i++ )
		m_search[i] = CANWALK;

	for( int z = 0; z < iZSize; z++ )
	{
		for( int x = 0; x < iXSize; x++ )
		{
			if ( pSrc[ x + z * iXSize ] == CANTWALK || pGlb[ x + z * iXSize ] == CANTWALK )
			{
				if ( z > 0 && ( pSrc[ x + (z-1) * iXSize ] == CANTWALK || pSrc[ x + (z-1) * iXSize ] == CANTWALK ) )
				{
					FillGopa3( x, z, m_search, pGopa, iGopaSize, CANTWALK );	
				}
				else if ( x > 0 && ( pSrc[ (x-1) + z * iXSize ] == CANTWALK || pSrc[ (x-1) + z * iXSize ] == CANTWALK ) )
				{
					FillGopa2( x, z, m_search, pGopa, iGopaSize, CANTWALK );
				}
				else
				{
					FillGopa( x, z, m_search, pGopa, iGopaSize, CANTWALK );
				}
			}
		}
	}
}

//Заполняет текстуру Quest.Floor, temp массив с данными, iValue значение непроходимоcти
void QGameQuestSceneData::FillFloorTex( UINT* temp, long iValue )
{
	if ( g_TexMgr.GetTex( L"Quest.Floor" )==NULL )
		return;

	D3DCOLOR* temp2 = new D3DCOLOR[iXSize * iZSize];

	bool a = false;
	for ( int z = 0; z < iZSize; z++)
	{
		if ( iXSize % 2 == 0 )
			a = !a;

		for ( int x = 0; x < iXSize; x++)
		{
			a = !a;
			if ( temp[ z * iXSize + x] == iValue )
			{
				temp2[ z * iXSize + x] = 0x99000000;
			}
			else
			{
				temp2[ z * iXSize + x] = a ? 0x99ffffff : 0x99cccccc;
			}
		}
	}

	g_TexMgr.FillTex( L"Quest.Floor", temp2 );

	SAFE_DELETE_ARRAY(temp2);
}

SNULL QGameQuestSceneData::GetNull( const std::wstring& null_id )
{
	SNULL TempNull;
	ZeroMemory( &TempNull, sizeof(SNULL) );
	
	if ( !g_EfnMgr.GetEfnNull( m_sEfnID + L".Nulls", null_id, 0, 0, 0.0f, &TempNull ) )
	{
		g_Log.WriteError( L"Cant find a NULL, null_id = \"" + null_id + L"\", in efn_id = \"" + m_sSceneName + L".Nulls\"" );
	}
	return TempNull;
}

int	QGameQuestSceneData::GetPosType( UINT* grid, const D3DXVECTOR3* _pos )
{
	int _x = int( (_pos->x - scene_pos.x) / m_fCellSize );
	int _z = int( (_pos->z - scene_pos.z) / m_fCellSize );

	if ( _x >= 0 && _x < iXSize && _z >= 0 && _z < iZSize )
	{
		return grid[ _x + _z * iXSize ];
	}
	else
	{
		return CANTWALK;
	}
}

int	QGameQuestSceneData::GetGridPosType( UINT* grid, int _x, int _z )
{
	if ( _x >= 0 && _x < iXSize && _z >= 0 && _z < iZSize )
	{
		return grid[ _x + _z * iXSize ];
	}
	else
	{
		return CANTWALK;
	}
}

void QGameQuestSceneData::GetGridFromPos( const D3DXVECTOR3* _pos, int& _x, int& _z )
{
	_x = int( (_pos->x - scene_pos.x) / m_fCellSize );
	_z = int( (_pos->z - scene_pos.z) / m_fCellSize );
	
	if ( _x < 0  ) _x = 0;
	if ( _x >= iXSize  ) _x = iXSize - 1;
	
	if ( _z < 0  ) _z = 0;
	if ( _z >= iZSize  ) _z = iZSize - 1;
}

D3DXVECTOR3 QGameQuestSceneData::GetPosFromGrid( int _x, int _z )
{
	D3DXVECTOR3 m_vPos;

	m_vPos	= GetPosFromGrid2( _x, _z );
	m_vPos.y	= GetYFromPos( &m_vPos );

	return m_vPos;
}

D3DXVECTOR3 QGameQuestSceneData::GetPosFromGrid2( int _x, int _z )
{
	if ( _x < 0  ) _x = 0;
	if ( _x >= iXSize  ) _x = iXSize - 1;
	
	if ( _z < 0  ) _z = 0;
	if ( _z >= iZSize  ) _z = iZSize - 1;

	return D3DXVECTOR3( scene_pos.x + _x * m_fCellSize + m_fCellSize * 0.5f,
						0.0f,
						scene_pos.z + _z * m_fCellSize + m_fCellSize * 0.5f );
}

unsigned int QGameQuestSceneData::GetCamForPos( const D3DXVECTOR3* _pos )
{
	int _x, _z;
	GetGridFromPos( _pos, _x, _z );
	return pCamMatrix[ _x + iXSize * _z ];
}

float QGameQuestSceneData::GetYFromPos( const D3DXVECTOR3* vPos )
{
	int _x;
	int _z;
	GetGridFromPos( vPos, _x, _z );

	if ( _x < 0 || _x >= iXSize || _z < 0 || _z >= iZSize )
		return -1000.0f;

	int m_iTriCount = pTriGrid[_x+_z*iXSize][1];
	
	if ( m_iTriCount==0 ) 
		return -1000.0f;

	int m_iFloorNum = -1;
	QEfnDataListItem* efn_temp = NULL;

	D3DXVECTOR3 vRayPos;
	D3DXVECTOR3 vRayDir;

	vRayPos	= D3DXVECTOR3( vPos->x, -1000.0f, vPos->z );
	vRayDir	= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	//bool b = false;
	float m_fDist = 0.0f;
	for( int i = 0; i < m_iTriCount; i++)
	{
		int m_iTempFloorNum = pTriGrid[_x+_z*iXSize][ 2 + i * 2 ];
		if ( m_iFloorNum!=m_iTempFloorNum )
		{
			efn_temp = NULL;
			m_iFloorNum = m_iTempFloorNum;
			QObjAction* obj = GetObjMgr()->GetObjAction( GetObjMgr()->GetFirstFromGroup( L"Floor_"+int2wstr(m_iFloorNum) ) );
			if ( obj )
				efn_temp = g_EfnMgr.GetEfn( obj->GetEfnID() );
		}
		if (!efn_temp)
			return -1000.0f;

		EFN_CHARACTERDATA* temp = efn_temp->pcdData;

		float m_fDistTemp;
		int m_iTempI1, m_iTempI2, m_iTempI3;

		m_iTempI1 = temp->vertex_indices_list[ pTriGrid[_x+_z*iXSize][ 2 + 2*i + 1 ]*3 + 0 ];
		m_iTempI2 = temp->vertex_indices_list[ pTriGrid[_x+_z*iXSize][ 2 + 2*i + 1 ]*3 + 1 ];
		m_iTempI3 = temp->vertex_indices_list[ pTriGrid[_x+_z*iXSize][ 2 + 2*i + 1 ]*3 + 2 ];
		
		if ( D3DXIntersectTri(	&D3DXVECTOR3( temp->posList[m_iTempI1].x, temp->posList[m_iTempI1].y, temp->posList[m_iTempI1].z ), 
								&D3DXVECTOR3( temp->posList[m_iTempI2].x, temp->posList[m_iTempI2].y, temp->posList[m_iTempI2].z ), 
								&D3DXVECTOR3( temp->posList[m_iTempI3].x, temp->posList[m_iTempI3].y, temp->posList[m_iTempI3].z ), 
								&vRayPos, &vRayDir, NULL, NULL, &m_fDistTemp ) )
		{
			//b = true;
			if ( m_fDistTemp > m_fDist )
				m_fDist = m_fDistTemp;
		}
	}

	//if(b==false)
	//	ShowMessage(L"fuck" + int2wstr(m_iTriCount));

	return vRayPos.y + m_fDist;
}

float QGameQuestSceneData::GetYFromGrid( int _x, int _z )
{
	D3DXVECTOR3 temp = GetPosFromGrid2( _x, _z );
	return GetYFromPos( &temp );
}

//обработка биндов
void QGameQuestSceneData::AddBinds( vecWStr& s )
{
	int m_iBindID = wstr2int( s.size()>0 ? s[0] : L"-1" );
	
	//если параметров в бинде < 5 то он поврежден, выдать ошибку
	if ( s.size()<5 )
	{
		g_Log.WriteError( L"bind damaged, bind_id = " + int2wstr(m_iBindID) );
		return;
	}
	if ( s.size() < 6 )
	{
		s.push_back(L"");
	}

	if ( s[5] != L"" )
	{
		std::wstring str = s[5];
		for( int i = str.length()-1; i >= 0; i-- )
		{
			if ( (str[i]==13) )
			{
				str[i] = 10;
				str.insert(i, L"\r" );
			}
		}
		s[5] = str;
	}

	if ( m_bLogEnabled )
	{
		std::wstring m_sTemp = L"";
		
		for( int i = 1; i <= 5; i++ )
		{
			m_sTemp += s[i];
			if(i<5)
				m_sTemp += L" - ";
		}

		g_Log.WriteMessage( L"bind: " + int2wstr(m_iBindID) + L" :: " + m_sTemp );
	}

	//Регистрация скриптов в сцене
	if ( s[2]==L"Scene" && s[4]==L"Scene" )
	{
		g_Script.ExecScript( s[5] );
		return;
	}

	//Добавление обьектов в сцену
	if ( s[2]==L"Scene" && s[4]==L"Camera" )
	{
		//Камеры уже были добавлены ранее из списка камер, здесь только ассоциируются камеры
		//с их id и добавляются скрипты
		GetObjMgr()->AddObjToGroup( L"Cam_"+int2wstr(m_iBindID), L"Camera_"+s[3] );
		g_Script.ExecScript( s[5] );
		//GetObjMgr()->AddObjToGroup( L"Camera_" + s[3] + L".3D", L"dude" );
		return;
	}
	if ( s[2]==L"Scene" && s[4]==L"Background" )
	{
		if ( s[5] != L"" )
		{
			g_Script.ExecScript(s[5]);
		}
		else
		{
			g_TexMgr.LoadTex3( L"Quest.Backgrounds", L"Backgrounds\\" + m_sSceneName + L"." + s[3], -1, false );
		}
	}
	if ( s[2]==L"Scene" && s[4]==L"ObjAction" )
	{
		GetObjMgr()->AddObjAction( L"ObjAction_" + s[3], m_sEfnID + L"." + s[3], s[5] );
		return;
	}
	if ( s[2]==L"Scene" && s[4]==L"LayerGeom" )
	{
		GetObjMgr()->AddObjLG( L"LayerGeom_" + s[3], m_sEfnID + L"." + s[3], ( s[5] != L"" ? s[5] : L"true" )  );
		return;
	}
	if ( s[2]==L"Scene" && ( s[4]==L"Man" || s[4]==L"Car" || s[4]==L"Bird" || s[4]==L"FreeObj" || s[4]==L"Obj3DVideo" ) )
	{
		g_Script.ExecScript(s[5]);
		return;
	}
	if ( s[2]==L"Scene" && s[4]==L"ShadowVolume" )
	{
		GetObjMgr()->AddObjSV( L"ShadowVolume_" + s[3], m_sEfnID + L"." + s[3] );
		return;
	}
	if ( s[2]==L"Scene" && s[4]==L"Floor" )
	{
		GetObjMgr()->AddObjAction( L"Floor_"+s[3], m_sEfnID + L"." + s[3], s[5] );
		GetObjMgr()->AddObjToGroup( L"Floors", L"Floor_"+s[3]);
		GetObjMgr()->AddObjToGroup( L"Floor_"+int2wstr(m_iBindID), L"Floor_"+s[3]);
		return;
	}
	
	//Привязываем обьекты к камерам
	if ( s[2]==L"Camera" && s[4]==L"Background" )
	{
		GetObjMgr()->AddObjToGroup( L"Camera_" + s[1] + L".Background", L"Backgrounds\\" + m_sSceneName + L"." + s[3] );
		return;
	}
	if ( s[2]==L"Background" && s[4]==L"LayerGeom" )
	{
		GetObjMgr()->AddObjToGroup( L"Background_Backgrounds\\" + m_sSceneName + L"." + s[1] + L".LayerGeoms", L"LayerGeom_" + s[3] );
		return;
	}
	if ( s[2]==L"Camera" && ( s[4]==L"Man" || s[4]==L"Car" || s[4]==L"Bird" || s[4]==L"FreeObj" || s[4]==L"Obj3DVideo") )
	{
		GetObjMgr()->AddObjToGroup( L"Camera_" + s[1] + L".3D", s[4] + L"_" + s[3] );
		return;
	}
	if ( s[2]==L"Camera" && s[4]==L"ShadowVolume" )
	{
		GetObjMgr()->AddObjToGroup( L"Camera_" + s[1] + L".ShadowVolumes", L"ShadowVolume_" + s[3] );
		return;
	}
	if ( s[2]==L"Camera" && s[4]==L"Light" )
	{
		GetObjMgr()->AddObjToGroup( L"Camera_" + s[1] + L".Lights", L"Light_" + s[3] );
		return;
	}
	if ( s[2]==L"Camera" && s[4]==L"ObjAction" )
	{
		GetObjMgr()->AddObjToGroup( L"Camera_" + s[1] + L".ObjActions", L"ObjAction_" + s[3] );
		return;
	}
	if ( s[2]==L"Camera" && s[4]==L"Floor" )
	{
		GetObjMgr()->AddObjToGroup( L"Camera_" + s[1] + L".Floors", L"Floor_" + s[3] );
		return;
	}
	if ( s[2]==L"Camera" && s[4]==L"ShadowFloor" )
	{
		GetObjMgr()->AddObjToGroup( L"Camera_" + s[1] + L".ShadowFloors", L"Floor_" + s[3] );
		return;
	}
}