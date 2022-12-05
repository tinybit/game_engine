#define STRICT

#include "QGameQuest.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResTextures.h"
#include "IMgrResEfnModels.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"
#include "QMgrResStates.h"
#include "QMgrResShaders.h"

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

QGameQuest::QGameQuest()
{
	m_bsVB				= NULL;

	back_surf			= NULL;
	shadow_surf			= NULL;

	m_sTargetObj		= L"";

	m_bCanRenderFloor	= false;
	m_bCanRenderShadow	= false;

	m_fLastSetCam		= 0.0;
	m_fSetCamInterval	= 0.1f;

	GetScene()->LogEnabled( false );
	GetScene()->SetObjMgr( GetObjMgr() );
}

QGameQuest::~QGameQuest()
{
	GetScene()->FreeScene();
	//g_TexMgr.FreeTexs( L"Quest.Backgrounds" );
}

void QGameQuest::Init()
{
	g_Script.RegisterEvents( GetName() );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}

	DoEvent(ON_INIT);

	Continue();
}

void QGameQuest::Kill()
{
	Pause();

	OnLostDevice();
	OnDestroyDevice();

	DoEvent(ON_KILL);
}

void QGameQuest::OnCreateDevice()
{
	if( FAILED( g_Graph.dev()->CreateVertexBuffer(	4 * sizeof(POS_TEX_VERTEX),
													D3DUSAGE_WRITEONLY, POS_TEX_FVF,
													D3DPOOL_MANAGED, 
													&m_bsVB, NULL ) ) )
        return;

	SetBackTexCoords( NULL );
}

void QGameQuest::OnResetDevice()
{
	if ( m_bCanRenderShadow )
	{
		g_Graph.dev()->GetRenderTarget(0, &back_surf);

		g_TexMgr.GetTex( GetName() + L".Shadow" )->GetSurfaceLevel(0, &shadow_surf);
	}
}

void QGameQuest::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( !GetScene()->isSceneLoaded() ) 
		return;

	if ( fTime - m_fLastSetCam > m_fSetCamInterval )
		g_Script.ExecFunc( GetName() + L"_SetCam" );

	GetObjMgr()->GroupDoFrameMove( L"ObjVideoAnims", GetGameTime(), GetGameElapsedTime() );
	GetObjMgr()->GroupDoFrameMove( L"Obj3DVideos", GetGameTime(), GetGameElapsedTime() );
	GetObjMgr()->GroupDoFrameMove( L"Mans", GetGameTime(), GetGameElapsedTime() );
	GetObjMgr()->GroupDoFrameMove( L"Birds", GetGameTime(), GetGameElapsedTime() );
	GetObjMgr()->GroupDoFrameMove( L"Cars", GetGameTime(), GetGameElapsedTime() );
	GetObjMgr()->GroupDoFrameMove( L"FreeObjs", GetGameTime(), GetGameElapsedTime() );

	GetObjMgr()->ClearRenderBuffer();

	if ( Cam() )
	{
		if ( m_sTargetObj != L"" )
		{
			try
			{
				QObjBase* obj = GetObjMgr()->GetObj( m_sTargetObj );
				if ( obj ) Cam()->SetPP( &obj->GetNullPos()->vPos, fElapsedTime );
			}
			catch(...)
			{
				Cam()->ResetPP();
			}

		}
		else
		{
			Cam()->ResetPP();
		}

		Cam()->ComputeCam();

		GetObjMgr()->AddObjToRenderBuffer( L"dude" );
		GetObjMgr()->AddGroupToRenderBuffer( Cam3DStr() );
		//GetObjMgr()->AddGroupToRenderBuffer( L"Particles" );
		GetObjMgr()->CalcZSortedRenderBuffer( &Cam()->GetNullPos()->vPos );
	}
}

void QGameQuest::OnFrameRender( double fTime, float fElapsedTime )
{
	if ( !GetScene()->isSceneLoaded() )
	{
		g_Graph.Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
		g_Graph.RenderText5( 0, 0, L"Сцены не загружены", 0xffff0000, 0, L"", 0 );
		return;
	}

	g_Graph.ClearLights();

	if ( Cam() )
	{
		g_Graph.Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 128 );
		
		//RenderBack
		if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderBack") )
			RenderBack();

		if ( g_StatesMgr.GetStateB( L"Quest.rp_QuestWareFrame" ) == true )
		{
			g_Graph.dev()->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		}
		else
		{
			g_Graph.dev()->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		}
	}
	else
	{
		g_Graph.Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
		g_Graph.RenderText5( 0, 0, L"Камера не установлена", 0xffff0000, 0, L"", 0 );
		return;
	}
	

	//RenderFloors
	if ( m_bCanRenderFloor )
		RenderFloor();

	//RenderShadow
	if ( m_bCanRenderShadow )
	{
		MakeShadow();
		RenderShadow();
	}

	//Render3D
	if ( g_StatesMgr.GetStateB(L"Quest.rp_Render3D") )
		Render3D();

	g_Graph.ClearLights();

	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderShadowVolumes") )
		RenderShadowVolumes();

	//RenderLayerGeoms
	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderLayers") )
		RenderLayers();

	//------------------------------------------------------------------------

	//Render nulls
	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderNulls") )
		RenderNulls();

	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderActiveObjsCount") )
		g_Graph.RenderText5( 1, -1, GetActiveObjsStrList(), 0xffffffff, 0x80000000, L"", 5 );

	//RenderObjActions
	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderObjActions") )
		RenderObjActions();

	//Render obj list
	if ( ( g_StatesMgr.GetStateB( L"Quest.rp_ShowObjList" ) || g_StatesMgr.GetStateB( L"Quest.rp_ShowObjGroupsList" ) ) && g_Graph.GetDebug() )
	{
		int a = g_StatesMgr.GetState(L"Quest.ObjListStartY");
		if ( a == 99999 )
		{
			if ( g_StatesMgr.GetStateB( L"Quest.rp_ShowObjList" ) )
				g_Graph.RenderText5( -1, 1, GetObjMgr()->GetObjList(), 0xff00ff00, 0x80000000, L"small", 5 );
			else
				g_Graph.RenderText5( -1, 1, GetObjMgr()->GetObjGroupsList(), 0xff00ff00, 0x80000000, L"small", 5 );
		}
		else if ( a == -99999 )
		{
			if ( g_StatesMgr.GetStateB( L"Quest.rp_ShowObjList" ) )
				g_Graph.RenderText5( -1, -1, GetObjMgr()->GetObjList(), 0xff00ff00, 0x80000000, L"small", 5 );
			else
				g_Graph.RenderText5( -1, -1, GetObjMgr()->GetObjGroupsList(), 0xff00ff00, 0x80000000, L"small", 5 );
		}
		else
		{
			if ( g_StatesMgr.GetStateB( L"Quest.rp_ShowObjList" ) )
				g_Graph.RenderText4( 5, a, GetObjMgr()->GetObjList(), 0xff00ff00, 0x80000000, L"small", 5 );
			else
				g_Graph.RenderText4( 5, a, GetObjMgr()->GetObjGroupsList(), 0xff00ff00, 0x80000000, L"small", 5 );
		}
	}

	g_Graph.dev()->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

void QGameQuest::OnLostDevice()
{
	SAFE_RELEASE(shadow_surf);
	SAFE_RELEASE(back_surf);
}

void QGameQuest::OnDestroyDevice()
{
	SAFE_RELEASE( m_bsVB );
}

bool QGameQuest::LoadScene( const std::wstring& scene_id )
{
	SetCam( L"" );

	if ( !GetScene()->LoadScene( scene_id ) )
		return false;

	g_Graph.dev()->EvictManagedResources();

	m_bCanRenderFloor = false;
	if ( g_StatesMgr.GetStateB( L"Quest.rp_RenderFloor" ) )
	{
		//создаем текстуру для отображения пола
		g_TexMgr.FreeTex( L"Quest.Floor" );
		m_bCanRenderFloor = g_TexMgr.CreateTexture3( L"Quest", L"Quest.Floor", GetScene()->iXSize, GetScene()->iZSize, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, true, false );
	}
	
	m_bCanRenderShadow = false;
	if ( g_Graph.GetShadowQuality() > 0 )
	{
		//Создаем текстуру для тени
		g_TexMgr.FreeTex( L"Quest.Shadow" );
		m_bCanRenderShadow = g_TexMgr.CreateTexture3( L"Quest", L"Quest.Shadow", GetScene()->m_iShadowTexWidth, GetScene()->m_iShadowTexHeight, D3DUSAGE_RENDERTARGET, D3DFMT_A8, true, true );
	}

	OnLostDevice();
	OnResetDevice();

	D3DXMatrixLookAtLH( &m_matShadowViewOrtho, 
						&D3DXVECTOR3( 0.0f,500.0f, 0.0f ),
						&D3DXVECTOR3( 0.0f,  0.0f, 0.0f ), 
						&D3DXVECTOR3( 0.0f,  0.0f, 1.0f ) );

	D3DXMatrixOrthoOffCenterLH( &m_matShadowProjOrtho, 
								GetScene()->scene_pos.x, GetScene()->scene_pos.x + GetScene()->scene_size.x,
								GetScene()->scene_pos.z + GetScene()->scene_size.z, GetScene()->scene_pos.z,
								0.0f, 1000.0f );

	return true;
}

SNULL QGameQuest::GetClosestNull( SNULL* pstNull )
{
	SNULL m_stTemp = *pstNull;
	GetScene()->GetClosestPosition( GetScene()->pGlobalMatrix, &m_stTemp.vPos );
	return m_stTemp;
}

void QGameQuest::SetCam( const std::wstring& sActiveCamera )
{
	if ( sActiveCamera == L"" )
	{
		m_pActiveCamera = NULL;
		m_sActiveCamera = L"";
		return;
	}

	if ( m_sActiveCamera == sActiveCamera )
		return;

	m_pActiveCamera = GetObjMgr()->GetObjCam( sActiveCamera );

	if ( m_pActiveCamera )
	{
		m_sActiveCamera = sActiveCamera;

		g_TexMgr.SetActives( L"Quest.Backgrounds", false );

		QTextureListItem* temp_tex = g_TexMgr.GetTexItem( GetObjMgr()->GetFirstFromGroup( CamBackStr() ) );

		SetBackTexCoords( temp_tex );
		m_pActiveCamera->SetTex( temp_tex );

		mmapWStrWStr_iter beg, end, beg2, end2;

		//Здесь получаем все текстуры для камеры
		if ( GetObjMgr()->GetObjGroup( CamBackStr(), &beg, &end ) > 0 )
		{
			QObjLayerGeom* temp_lg = NULL;

			for( mmapWStrWStr_iter i = beg; i != end; i++ )
			{
				//i->second это tex_id
				temp_tex = g_TexMgr.GetTexItem( i->second );

				if ( temp_tex )
				{
					temp_tex->Active = true;
					g_TexMgr.SetActive( temp_tex );

					//Для каждой текстуры получаем ее LayerGeoms
					if ( GetObjMgr()->GetObjGroup( CamBackLayerGeomsStr( i->second ), &beg2, &end2 ) > 0 )
					{
						//j->second это obj_id для LayerGeom
						for( mmapWStrWStr_iter j = beg2; j != end2; j++ )
						{
							temp_lg = GetObjMgr()->GetObjLG( j->second );
								
							//Расчитываем UV для геометрии в соответствии с текстурой которой она принадлежит
							if(temp_lg)
								temp_lg->CalcUV( Cam(), temp_tex );
						}
					}
				}
			}
		}
	}
}

void QGameQuest::SetBackTexCoords( void* pTex )
{
	QTextureListItem* t = (QTextureListItem*)pTex;
	float tu = 1.0f;
	float tv = 1.0f;
	if(t)
	{
		tu = t->fWidthAspect;
		tv = t->fHeightAspect;
	}
	POS_TEX_VERTEX* v = NULL;
    m_bsVB->Lock( 0, 0, (void**)&v, 0 );

		v[0].p = D3DXVECTOR3(   0.0f,			g_Graph.fMaxY,	90.0f );
		v[1].p = D3DXVECTOR3(	g_Graph.fMaxX,	g_Graph.fMaxY,	90.0f );
		v[2].p = D3DXVECTOR3(   0.0f,			0.0f,			90.0f );
		v[3].p = D3DXVECTOR3(	g_Graph.fMaxX,	0.0f,			90.0f );
		
		v[0].u = 0.0f;  v[0].v = 0.0f;
		v[1].u = tu;	v[1].v = 0.0f;
		v[2].u = 0.0f;  v[2].v = tv;
		v[3].u = tu;	v[3].v = tv;

    m_bsVB->Unlock();
}

bool QGameQuest::CollisionTest( QObjBase* pObj, const D3DXVECTOR3* pvNewPos, std::wstring* psCollisedObjID, D3DXVECTOR3* pvCollisionPos )
{
	if ( GetActiveObjsList()->size() > 1 && pObj != NULL )
	{
		if ( GetActiveObjsList()->find( pObj->GetObjID() ) == GetActiveObjsList()->end() )
			return true;

		float m_fDist;
		D3DXVECTOR2 m_vObj(pvNewPos->x, pvNewPos->z), m_vTemp;
		
		QObjBase* temp_obj = NULL;
		for( mapWStrQObjBase_iter i = GetActiveObjsList()->begin(); i != GetActiveObjsList()->end(); i++ )
		{
			temp_obj = i->second;
			if ( temp_obj != NULL && temp_obj != pObj )
			{
				m_vTemp.x	= temp_obj->GetNullPos()->vPos.x - m_vObj.x;
				m_vTemp.y	= temp_obj->GetNullPos()->vPos.z - m_vObj.y;
				m_fDist		= pObj->GetGopaRadius() + temp_obj->GetGopaRadius();

				if ( fabs(m_vTemp.x) < m_fDist &&
					 fabs(m_vTemp.y) < m_fDist )
				{
					if ( D3DXVec2Length( &m_vTemp ) < m_fDist )
					{
						if ( psCollisedObjID ) *psCollisedObjID = i->first;
						if ( pvCollisionPos )
						{
							D3DXVec2Normalize( &m_vTemp, &m_vTemp );
							m_vObj += m_vTemp * pObj->GetGopaRadius();
							pvCollisionPos->x = m_vObj.x;
							pvCollisionPos->z = m_vObj.y;
						}
						return false;
					}
				}
			}
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Name: RenderBack()
// Desc: отрисовыает фон
//-----------------------------------------------------------------------------
void QGameQuest::RenderBack()
{
	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );
	g_Graph.SetAlphaBlendOff();
	g_Graph.SetAlphaTestOff();
	g_Graph.SetLightingEnabled( false );

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_FALSE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		false );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE  );

	g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	
	g_Graph.SetFiltering( 0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, ( g_Graph.GetUseMip1ForBacks() ? D3DTEXF_POINT : D3DTEXF_LINEAR ) );

	g_TexMgr.SetTex( 0, GetObjMgr()->GetFirstFromGroup( CamBackStr() ) );

	Cam()->SetOrthoCam();
	g_Graph.dev()->SetStreamSource( 0, m_bsVB, 0, sizeof(POS_TEX_VERTEX) );
	g_Graph.dev()->SetFVF( POS_TEX_FVF );
	g_Graph.dev()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
}

void QGameQuest::RenderLayers()
{
	ID3DXEffect* m_pEffect = g_ShadMgr.GetShader(L"LayerGeom");

	if ( !m_pEffect )
		return;

	Cam()->SetPerCam();

	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );

	g_Graph.SetAlphaBlendNormal();

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_TRUE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		false );
	g_Graph.SetLightingEnabled( false );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED );

	bool m_bRenderLayersWithDefault = false;
	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderLayersWithDefault") )
	{
		m_bRenderLayersWithDefault = true;
		g_Graph.SetFilteringAll( 0, D3DTEXF_POINT );
		g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
		g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
		g_TexMgr.SetTex( 0, L"sys\\default" );
	}
	else
	{
		g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
		g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );

		g_Graph.SetFiltering( 0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, ( g_Graph.GetUseMip1ForBacks() ? D3DTEXF_POINT : D3DTEXF_LINEAR ) );
	}

	m_pEffect->SetMatrix( "g_mWorldViewProjection", &Cam()->matViewProjPer );

	m_pEffect->SetTechnique( "my" );

	mmapWStrWStr_iter beg, end;

	g_Graph.SetAlphaTestForNonZeroAlpha();

	UINT cPasses;
	m_pEffect->Begin(&cPasses, 0);

	for ( int iPass = 0; iPass < cPasses; iPass++ )
	{
		m_pEffect->BeginPass(iPass);

		if ( GetObjMgr()->GetObjGroup( CamBackStr(), &beg, &end ) > 0 )
		{
			for( mmapWStrWStr_iter i = beg; i != end; i++ )
			{
				if ( !m_bRenderLayersWithDefault )
					g_TexMgr.SetTex( 0, i->second );
		
				GetObjMgr()->GroupDoFrameRender( CamBackLayerGeomsStr(i->second), GetGameTime(), GetGameElapsedTime(), ORT_TRANSPARENT );
			}
		}
		
		m_pEffect->EndPass();
	}

	m_pEffect->End();

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	// отключаем прозрачность
	g_Graph.SetAlphaBlendOff();
	g_Graph.SetAlphaTestOff();

	g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	g_Graph.dev()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
}

//-----------------------------------------------------------------------------
// Name: RenderFloor()
// Desc: отрисовывает пол
//-----------------------------------------------------------------------------
void QGameQuest::RenderFloor()
{
	if ( !g_StatesMgr.GetStateB(L"Quest.rp_RenderFloorWalk") &&
		 !g_StatesMgr.GetStateB(L"Quest.rp_RenderFloorCam") )
		 return;

	Cam()->SetPerCam();

	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );
	g_Graph.SetAlphaBlendOff();

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_TRUE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		true );
	g_Graph.SetLightingEnabled( false );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE  );

	//создаем текстуру грида
	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderFloorWalk") )
	{
		GetScene()->FillFloorTex(  GetScene()->pGlobalMatrix, CANTWALK );
	}
	else
	{
		if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderFloorCam") )
		{
			GetScene()->FillFloorTex(  GetScene()->pCamMatrix, 0 );
		}
	}
	
	g_Graph.SetFilteringAll( 0, D3DTEXF_POINT );

	g_TexMgr.SetTex( 0, L"Quest.Floor" );

	GetObjMgr()->GroupDoObjRender( CamFloorsStr(), RT_POS_TEX );
}

void QGameQuest::RenderShadow()
{
	Cam()->SetPerCam();

	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );

	g_Graph.SetAlphaBlendNormal();
	g_Graph.SetAlphaTestForNonZeroAlpha();

	g_Graph.SetLightingEnabled( true );
	g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_FLAT );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_TRUE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		true );

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE  );

	g_Graph.SetFiltering( 0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_POINT );

	g_TexMgr.SetTex( 0, L"Quest.Shadow" );

	mmapWStrWStr_iter beg, end;

	if ( GetObjMgr()->GetObjGroup( CamShadowFloorsStr(), &beg, &end ) > 0 )
	{
		QObjBase* obj = NULL;
		for( mmapWStrWStr_iter i = beg; i != end; i++ )
		{
			obj = GetObjMgr()->GetObj( i->second );
				
			if ( obj )
			{
				g_Graph.dev()->SetMaterial( obj->GetMtrl() );
				obj->ObjRender( RT_POS_TEX );
			}
		}
	}

	g_Graph.SetAlphaBlendOff();
	g_Graph.SetAlphaTestOff();
}

void QGameQuest::Render3D()
{
	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );

	g_Graph.SetLightingEnabled( true );
	g_Graph.SetLightsFromGroup( GetObjMgr(), CamLightsStr() );
	g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD );

	g_Graph.SetAlphaTestForNonZeroAlpha();
	g_Graph.SetAntialiasingEnabled( true );

	Cam()->SetPerCam();

	//Рисуем из сортированного рендер буфера объектов
	GetObjMgr()->RenderFromZSortedRenderBuffer( GetGameTime(), GetGameElapsedTime() );

	g_Graph.SetAntialiasingEnabled( false );
	g_Graph.SetAlphaTestOff();

	//---------------------------------------------------------
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE  );
	g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_FLAT );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
	g_Graph.ClearLights();
	try
	{
		GetObjMgr()->GetObjMan(L"dude")->OnFrameRender( 0, 0, ORT_NONTRANSPARENT );
	}
	catch(...) {}

	g_Graph.SetLightingEnabled( false );
}

void QGameQuest::RenderObjActions()
{
	Cam()->SetPerCam();

	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );
	g_Graph.SetAlphaBlendOff();

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE  );

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_FALSE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		false );
	g_Graph.SetLightingEnabled( false );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );

	g_TexMgr.SetTexNull(0);

	GetObjMgr()->GroupDoObjRender( CamObjActionsStr(), RT_POS );
}

void QGameQuest::RenderShadowVolumes()
{
	Cam()->SetPerCam();

	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderShadowVolumesSiluet") )
	{
		GetObjMgr()->GroupDoObjRender( CamShadowVolumesStr(), RT_POS );
	}
	else
	{
		GetObjMgr()->GroupDoFrameRender( CamShadowVolumesStr(), GetGameTime(), GetGameElapsedTime(), ORT_TRANSPARENT );
	}
}

void QGameQuest::RenderNulls()
{
	Cam()->SetPerCam();

	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,			false );
	g_Graph.SetAlphaBlendOff();

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE  );

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,		D3DZB_TRUE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,	true );
	g_Graph.SetLightingEnabled( true );
	g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,		D3DSHADE_GOURAUD);
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,		D3DCULL_CCW );

	EFN_CHARACTERDATA* m_cdData = g_EfnMgr.GetEfn( GetScene()->m_sEfnID+L".Nulls" )->pcdData;
	D3DXMATRIX matR;
	D3DXMATRIX matT;
	D3DXMATRIX matS;
	D3DXMatrixScaling( &matS, 0.1f, 0.1f, 0.1f);

	for ( mapWStrVecSNulls_iter i = m_cdData->nulls_map.begin(); i != m_cdData->nulls_map.end(); i++ )
	{
		vecSNulls nullvec_temp = i->second;
		D3DXMatrixRotationY( &matR, nullvec_temp[0].vRot.y );
		D3DXMatrixTranslation( &matT, nullvec_temp[0].vPos.x, nullvec_temp[0].vPos.y, nullvec_temp[0].vPos.z );
		
		g_EfnMgr.Render( L"NullIcon", &(matS*matR*matT), RT_POS_NORM );
	}

}

void QGameQuest::MakeShadow()
{
	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,		false );
	g_Graph.SetAlphaBlendOff();
	g_Graph.SetLightingEnabled( false );

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_FALSE );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		false );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE  );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE  );

	g_Graph.dev()->SetRenderTarget(0, shadow_surf);
	
	g_Graph.Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

	g_Graph.dev()->SetTransform( D3DTS_VIEW,		&m_matShadowViewOrtho ); //View
	g_Graph.dev()->SetTransform( D3DTS_PROJECTION,	&m_matShadowProjOrtho ); //Projection

	GetObjMgr()->ObjRenderFromZSortedRenderBuffer( RT_POS );

	g_Graph.dev()->SetRenderTarget(0, back_surf );
}