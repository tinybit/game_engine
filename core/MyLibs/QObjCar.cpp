#define STRICT

#include "QObjCar.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "QMgrResSounds.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"

QObjCar::QObjCar( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	SetEfnID( efn_id );
	SetTexID( tex_id );
	SetTexID2( tex_id2 );

	m_pGopa						= NULL;
}

QObjCar::~QObjCar()
{
    Stop( false );
}

void QObjCar::Init()
{
	if ( GetGame() )
	{
		if ( GetGame()->GetGameType() == GT_QUEST )
		{
			m_pQuestScn = ((QGameQuest*)GetGame())->GetScene();
		}
		else
		{
			g_Log.WriteError( L"Car can only be created in game Quest, obj_id = \"" + GetObjID() + L"\"" );
		}
	}

	if ( GetObjMgr()->AddObjEfnAnim( GetEfnAnimObjID(), GetEfnID(), GetTexID(), GetTexID2() ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
		GetEfnAnim()->SetRenderPrms( true, false, false );
	}

	SetGopaRadius( g_EfnMgr.GetGlobalCylinderRadius( GetEfnID() ) );
	m_pQuestScn->MakeGopa( &m_pGopa, &m_iGopaSize, GetGopaRadius() );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjCar::Kill()
{
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );

	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();

	SAFE_DELETE_ARRAY(m_pGopa);
}

void QObjCar::SetPos( const SNULL* pstNull )
{
	SetNullPos( pstNull );
	m_stTarget = *pstNull;
	
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANTWALK );
}

void QObjCar::Teleport( const SNULL* pstNull )
{
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );

	SetPos( pstNull );
}

void QObjCar::OnCollision( const std::wstring& obj_id, const D3DXVECTOR3* pvCollisionPos )
{
	if ( g_Script.Exists2( L"Scene", GetObjID() + L"_OnCollision" ) )
	{
		g_Script.ExecScript( L"Scene:" + GetObjID() + L"_OnCollision( L(\"" + obj_id + L"\"), D3DXVECTOR3" + vec3_2wstr(pvCollisionPos) + L" );" );
	}
	else if ( g_Script.Exists( L"Car_OnCollision" ) )
	{
		g_Script.ExecScript( L"Car_OnCollision( L(\"" + GetObjID() + L"\"), L(\"" + obj_id + L"\"), D3DXVECTOR3" + vec3_2wstr(pvCollisionPos) + L" );" );
	}
	else
	{
		Stop( false );
	}
}

void QObjCar::ObjRender( DWORD rt )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->ObjRender( rt );
}

bool QObjCar::GetNewCoords()
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

float QObjCar::GetAngleDif() 
{
	return AngleDif( m_stNewPos.vRot.y, GetNullPos()->vRot.y );
}

SNULL* QObjCar::Path(int iIndex)
{
	iIndex = ( iIndex < 0 ? cp.size()-fabs((float)iIndex) : iIndex );

	if ( iIndex < 0 )
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

void QObjCar::OnFrameMove( double fTime, float fElapsedTime )
{
	SEfnAnimSeq* EfnAnimTemp = NULL;
	if ( GetEfnAnim() )
	{
		GetEfnAnim()->OnFrameMove( fTime, fElapsedTime );

		EfnAnimTemp = GetEfnAnim()->GetEfnAnimTemp();

		if ( !EfnAnimTemp )
			Stop( true );
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
	if(temp_y > -1000.0f)
	{
		GetNullPos()->vPos.y = temp_y;
	}

	if ( GetEfnAnim() )
		GetEfnAnim()->SetPos( GetNullPos() );
}

void QObjCar::Go( double fTime, int seq_id )
{
	m_stTarget = *Path( -1 );
	if ( GetEfnAnim() )
		GetEfnAnim()->Go( fTime, GetPathLength(), seq_id );
}

void QObjCar::Stop(bool bOK)
{
	cp.clear();
	m_fOffsetOnPath = 0.0;
	if ( GetEfnAnim() )
		GetEfnAnim()->Stop( bOK );
}