#define STRICT

#include "QObjMan.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "QMgrResSounds.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"
#include "QMgrResStates.h"

QObjMan::QObjMan( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	m_pGopa						= NULL;
	ZeroMemory( GetNullPos(), sizeof(SNULL) );

	m_fOffsetOnPath				= 0.0;

	m_pBoxMesh					= NULL;

	SetEfnID( efn_id );
	SetTexID( tex_id );
	SetTexID2( tex_id2 );
}

QObjMan::~QObjMan()
{
	Stop( false );
}

void QObjMan::Init()
{
	if ( GetGame() )
	{
		if ( GetGame()->GetGameType() == GT_QUEST )
		{
			m_pQuestScn = ((QGameQuest*)GetGame())->GetScene();
		}
		else
		{
			g_Log.WriteError( L"Man can only be created in game Quest, obj_id = \"" + GetObjID() + L"\"" );
		}
	}

	if ( GetObjMgr()->AddObjEfnAnim( GetEfnAnimObjID(), GetEfnID(), GetTexID(), GetTexID2() ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
		GetEfnAnim()->SetRenderPrms( true, false, false );
	}

	SetGopaRadius( g_EfnMgr.GetGlobalCylinderRadius( GetEfnID() ) );
	m_pQuestScn->MakeGopa( &m_pGopa, &m_iGopaSize, GetGopaRadius() );

	ZeroMemory( &m_stMtrl, sizeof( D3DMATERIAL9 ) );
	m_stMtrl.Diffuse = m_stMtrl.Ambient = m_stMtrl.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	ZeroMemory( &m_stMtrl2, sizeof( D3DMATERIAL9 ) );
	m_stMtrl2.Diffuse = m_stMtrl2.Ambient = m_stMtrl2.Emissive = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjMan::Kill()
{
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );
	SAFE_DELETE_ARRAY(m_pGopa);

	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();
}

void QObjMan::Go( double fTime, bool bRun, int seq_id )
{
	m_bRun = bRun;
	m_stTarget = *Path( -1 );
	if ( GetEfnAnim() )
		GetEfnAnim()->Go( fTime, GetPathLength(), seq_id );
}

void QObjMan::Stop( bool bOK )
{
	cp.clear();
	m_fOffsetOnPath = 0.0;
	if ( GetEfnAnim() )
		GetEfnAnim()->Stop( bOK );	
}

void QObjMan::SetPos( const SNULL* pstNull )
{
	SetNullPos( pstNull );
	
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	if ( m_pQuestScn->TestGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize ) )
	{
		m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANTWALK );
	}
	else
	{
		g_Log.WriteError( L"man:SetPos set man on impassability, obj_id = \"" + GetObjID() + L"\", (x,y) = (" + int2wstr(x) + L", " + int2wstr(z) + L")" );
	}
}

void QObjMan::Teleport( const SNULL* pstNull )
{
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK);

	SetPos( pstNull );
}

void QObjMan::OnCollision( const std::wstring& obj_id, const D3DXVECTOR3* pvCollisionPos )
{
	if ( g_Script.Exists2( L"Scene", GetObjID() + L"_OnCollision" ) )
	{
		g_Script.ExecScript( L"Scene:" + GetObjID() + L"_OnCollision( L(\"" + obj_id + L"\"), D3DXVECTOR3" + vec3_2wstr(pvCollisionPos) + L" );" );
	}
	else if ( g_Script.Exists( L"Man_OnCollision" ) )
	{
		g_Script.ExecScript( L"Man_OnCollision( L(\"" + GetObjID() + L"\"), L(\"" + obj_id + L"\"), D3DXVECTOR3" + vec3_2wstr(pvCollisionPos) + L" );" );
	}
	else
	{
		Stop( false );
	}
}

bool QObjMan::GetNewCoords()
{
	if ( cp.size()==0 ) 
	{
		m_stNewPos.vPos = GetNullPos()->vPos;	
		m_stNewPos.vRot = GetNullPos()->vRot;
		return false;
	}

	int cp_i;
	if ( cp[cp.size()-1].l <= m_fOffsetOnPath )
	{
		cp_i = cp.size()-1;
	}
	else
	{
		for( cp_i = 0; cp_i < cp.size()-1; cp_i++)
		{
			if ( cp[cp_i].l <= m_fOffsetOnPath && m_fOffsetOnPath < cp[cp_i+1].l )
			{
				break;
			}
		}
	}
	
	if ( cp_i < cp.size()-1 )
	{
		float cp_len =  cp[cp_i+1].l - cp[cp_i].l;
		D3DXVec3Lerp( &m_stNewPos.vPos, &cp[cp_i].p.vPos, &cp[cp_i+1].p.vPos, (m_fOffsetOnPath - cp[cp_i].l) / cp_len );
	}
	else
	{
		m_stNewPos.vPos = cp[cp_i].p.vPos;
	}
	m_stNewPos.vRot = cp[cp_i].p.vRot;

	if ( cp_i != 0 )
	{
		cp.erase( cp.begin() );
	}

	return true;
}

float QObjMan::GetAngleDif() 
{
	return AngleDif( m_stNewPos.vRot.y, GetNullPos()->vRot.y );
}

SNULL* QObjMan::Path(int iIndex)
{
	if ( cp.size() == 0 )
		return GetNullPos();

	iIndex = ( iIndex < 0 ? cp.size()-fabs((float)iIndex) : iIndex );

	if ( iIndex<0 )
	{
		return &cp[0].p;
	}
	else if ( iIndex < cp.size() )
	{
		return &cp[iIndex].p;
	}
	else
	{
		return &cp[cp.size()-1].p;
	}
}

void QObjMan::OnFrameMove( double fTime, float fElapsedTime )
{
	SEfnAnimSeq* EfnAnimTemp = NULL;
	if ( GetEfnAnim() )
	{
		GetEfnAnim()->OnFrameMove( fTime, fElapsedTime );

		EfnAnimTemp = GetEfnAnim()->GetEfnAnimTemp();
	}

	if ( EfnAnimTemp )
	{
		if ( EfnAnimTemp->sAnimName==L"turn left" || 
			EfnAnimTemp->sAnimName==L"turn right" )
		{
			GetNullPos()->vRot.y += EfnAnimTemp->fAnimRotSpeed * fElapsedTime;
		}
		else if( cp.size()>0 )
		{
			m_fOffsetOnPath = GetEfnAnim()->GetOffsetOnPath();

			//---------------------------------------------
			GetNewCoords();

			//если поворачивается быстрее чем на заданную скорость в секунду то замедлить
			if ( fabs(GetAngleDif()) > EfnAnimTemp->fAnimRotSpeed * fElapsedTime )
			{
				GetNullPos()->vRot.y += EfnAnimTemp->fAnimRotSpeed * fElapsedTime * ( GetAngleDif()/fabs(GetAngleDif()) );
			}
			else
			{
				GetNullPos()->vRot = m_stNewPos.vRot;
			}

			//----Collision test-----------
			std::wstring	m_sCollisedObjID;
			D3DXVECTOR3		m_vCollisedPos;
			if ( GetGame()->CollisionTest( this, &m_stNewPos.vPos, &m_sCollisedObjID, &m_vCollisedPos ) )
			{
				int x, z;
				m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z);
				m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK);

				m_pQuestScn->GetGridFromPos( &m_stNewPos.vPos, x, z);
				m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANTWALK);

				GetNullPos()->vPos.x = m_stNewPos.vPos.x;
				GetNullPos()->vPos.z = m_stNewPos.vPos.z;
			}
			else
			{
				OnCollision( m_sCollisedObjID, &m_vCollisedPos );
			}
		}
	}

	//Нахождение высоты обьекта на полу
	float temp_y = m_pQuestScn->GetYFromPos( &GetNullPos()->vPos );
	
	if ( temp_y > -1000.0f )
	{
		GetNullPos()->vPos.y = temp_y;
	}

	if ( GetEfnAnim() )
		GetEfnAnim()->SetPos( GetNullPos() );
}

void QObjMan::OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	if ( GetVisible() == false )
		return;

	if( g_StatesMgr.GetStateB( L"Quest.rp_RenderPath" ) )
		RenderPath();
}

void QObjMan::RenderPath()
{
	if ( cp.size() > 0 )
	{
		//render lines between check points
		g_Graph.dev()->SetMaterial( &m_stMtrl );
		D3DXMATRIX matT2;
		D3DXMATRIX matT3;
		D3DXVECTOR3 f;
		for( int i = 0; i < cp.size()-1; i++)
		{
			for ( int j = 1; j < 10; j++ )
			{
				D3DXVec3Lerp( &f, &cp[i].p.vPos, &cp[i+1].p.vPos, (float)j/10 );
				f.y = m_pQuestScn->GetYFromPos( &f );
				D3DXMatrixTranslation( &matT2, f.x, f.y, f.z);
				g_Graph.dev()->SetTransform( D3DTS_WORLD, &matT2 );
				m_pBoxMesh->DrawSubset(0);
			}
		}

		//render check points
		g_Graph.dev()->SetMaterial( &m_stMtrl2 );
		D3DXMatrixScaling( &matT3, 1.5, 1.5, 1.5);
		for( int i = 0; i < cp.size(); i++)
		{
			f = cp[i].p.vPos;
			f.y = m_pQuestScn->GetYFromPos( &f );
			D3DXMatrixTranslation( &matT2, f.x, f.y, f.z);
			g_Graph.dev()->SetTransform( D3DTS_WORLD, &(matT3 * matT2) );
			m_pBoxMesh->DrawSubset(0);
		}
	}
}

void QObjMan::OnCreateDevice()
{
	if( FAILED( D3DXCreateBox( g_Graph.dev(), 0.02f, 0.02f, 0.02f, &m_pBoxMesh, NULL ) ) )
        return;
}

void QObjMan::OnDestroyDevice()
{
	SAFE_RELEASE( m_pBoxMesh );
}

//m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
//------------------------------------
//if (o)
//{
//	D3DXMATRIX m;
//	D3DXPLANE p;
//	D3DXPlaneFromPointNormal( &p, &g_ResMgr.scn->GetRoom()->pos, &D3DXVECTOR3(0,1,0));
//	D3DXMatrixReflect( &m, &p );
//	D3DXMATRIX matWorldReflect = matWorld*m;
//	
//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldReflect );
//	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_CW );

//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

//	g_ResMgr.efn->Draw(m_sEfnID);

//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
//}
