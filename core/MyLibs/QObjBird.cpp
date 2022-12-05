#define STRICT

#include "QObjBird.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "QMgrResSounds.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"

QObjBird::QObjBird( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );

	SetEfnID( efn_id );
	SetTexID( tex_id );
	SetTexID2( tex_id2 );

	m_pGopa						= NULL;
	m_bIsFly					= false;
}

QObjBird::~QObjBird()
{
    Stop( false );
}

void QObjBird::Init()
{
	if ( GetGame() )
	{
		if ( GetGame()->GetGameType() == GT_QUEST )
		{
			m_pQuestScn = ((QGameQuest*)GetGame())->GetScene();
		}
		else
		{
			g_Log.WriteError( L"Bird can only be created in game Quest, obj_id = \"" + GetObjID() + L"\"" );
		}
	}

	if ( GetObjMgr()->AddObjEfnAnim( GetEfnAnimObjID(), GetEfnID(), GetTexID(), GetTexID2() ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
	}

	SetGopaRadius( g_EfnMgr.GetGlobalCylinderRadius( GetEfnID() ) );
	m_pQuestScn->MakeGopa( &m_pGopa, &m_iGopaSize, GetGopaRadius() );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjBird::Kill()
{
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );

	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();

	SAFE_DELETE_ARRAY(m_pGopa);
}

void QObjBird::SetPos( const SNULL* pstNull )
{
	SetNullPos( pstNull );
	m_stTarget = *pstNull;
	
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANTWALK );
}

void QObjBird::Teleport( const SNULL* pstNull )
{
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z );
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );

	SetPos( pstNull );
}

void QObjBird::OnCollision( const std::wstring& obj_id, const D3DXVECTOR3* pvCollisionPos )
{
	if ( g_Script.Exists2( L"Scene", GetObjID() + L"_OnCollision" ) )
	{
		g_Script.ExecScript( L"Scene:" + GetObjID() + L"_OnCollision( L(\"" + obj_id + L"\"), D3DXVECTOR3" + vec3_2wstr(pvCollisionPos) + L" );" );
	}
	else if ( g_Script.Exists( L"Bird_OnCollision" ) )
	{
		g_Script.ExecScript( L"Bird_OnCollision( L(\"" + GetObjID() + L"\"), L(\"" + obj_id + L"\"), D3DXVECTOR3" + vec3_2wstr(pvCollisionPos) + L" );" );
	}
	else
	{
		Stop( false );
	}
}

bool QObjBird::GetNewCoords()
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

	return true;
}

bool QObjBird::GetNewFlyCoords()
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

	int i1 = ( cp_i > 0 ? cp_i-1 : 0 );
	int i2 = cp_i;
	int i3 = ( cp_i < cp.size()-1 ? cp_i+1 : cp_i );
	int i4 = ( cp_i < cp.size()-2 ? cp_i+2 : cp.size()-1 );
	if ( cp_i < cp.size()-1 )
	{
		float cp_len =  cp[cp_i+1].l - cp[cp_i].l;
		D3DXVec3CatmullRom( &m_stNewPos.vPos, 
			&cp[i1].p.vPos, &cp[i2].p.vPos, &cp[i3].p.vPos, &cp[i4].p.vPos, 
			(m_fOffsetOnPath - cp[cp_i].l) / cp_len );
		D3DXVECTOR3 m_vTemp;
		D3DXVec3CatmullRom( &m_vTemp, 
			&cp[i1].p.vPos, &cp[i2].p.vPos, &cp[i3].p.vPos, &cp[i4].p.vPos, 
			(m_fOffsetOnPath+0.01 - cp[cp_i].l) / cp_len );

		m_stNewPos.vRot = D3DXVECTOR3( 0.0f, atan2f( m_vTemp.x - m_stNewPos.vPos.x, m_vTemp.z - m_stNewPos.vPos.z ), 0.0f );
	}
	else
	{
		m_stNewPos.vPos = cp[cp_i].p.vPos;
	}

	return true;
}

float QObjBird::GetAngleDif() 
{
	return AngleDif( m_stNewPos.vRot.y, GetNullPos()->vRot.y );
}

SNULL* QObjBird::Path(int iIndex)
{
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

void QObjBird::OnFrameMove( double fTime, float fElapsedTime )
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
		if ( m_bIsFly )
		{
			m_fOffsetOnPath = GetEfnAnim()->GetOffsetOnPath();

			GetNewFlyCoords();

			GetNullPos()->vPos = m_stNewPos.vPos;
			GetNullPos()->vRot = m_stNewPos.vRot;
		}
		else
		{
			if ( EfnAnimTemp->sAnimName==L"turn left" || 
				EfnAnimTemp->sAnimName==L"turn right" )
			{
				GetNullPos()->vRot.y += EfnAnimTemp->fAnimRotSpeed * fElapsedTime;
			}
			else if( cp.size()>0 )
			{
				m_fOffsetOnPath = GetEfnAnim()->GetOffsetOnPath();

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

			//Нахождение высоты обьекта на полу
			float temp_y = m_pQuestScn->GetYFromPos( &GetNullPos()->vPos );
			if ( temp_y > -1000.0f )
				GetNullPos()->vPos.y = temp_y;
		}
	}
	else
	{
		//Нахождение высоты обьекта на полу
		float temp_y = m_pQuestScn->GetYFromPos( &GetNullPos()->vPos );
		if ( temp_y > -1000.0f )
			GetNullPos()->vPos.y = temp_y;
	}

	if ( GetEfnAnim() )
			GetEfnAnim()->SetPos( GetNullPos() );
}

void QObjBird::Go( double fTime, int seq_id )
{
	m_bIsFly = false;
	m_stTarget		= *Path( -1 );
	if ( GetEfnAnim() )
		GetEfnAnim()->Go( fTime, GetPathLength(), seq_id );
}

void QObjBird::Fly( double fTime, int seq_id )
{
	//Земля прощай
	int x, z;
	m_pQuestScn->GetGridFromPos( &GetNullPos()->vPos, x, z);
	m_pQuestScn->FillGopa( x, z, m_pQuestScn->pGlobalMatrix, m_pGopa, m_iGopaSize, CANWALK );
	GetGame()->DelActiveObj( GetObjID() );

	//Заполняем точку приземления
	D3DXVECTOR3 temp = Path( -1 )->vPos;
	temp.y = m_pQuestScn->GetYFromPos( &Path( -1 )->vPos );
	AddCP( &temp );

	//В добрый путь
	m_bIsFly = true;
	m_stTarget		= *Path( -1 );
	if ( GetEfnAnim() )
	{
		float l = g_EfnMgr.GetAnimZLength( GetEfnAnim()->GetEfnID(), L"fly cycle" );
		l = GetPathLength() / l;
		GetEfnAnim()->AnimAdd( L"fly cycle", 0, l );
		GetEfnAnim()->Go( fTime, GetPathLength(), seq_id );
	}
}

void QObjBird::Stop( bool bOK )
{
	m_bIsFly = false;
	cp.clear();
	m_fOffsetOnPath = 0.0;
	if ( GetEfnAnim() )
		GetEfnAnim()->Stop( bOK );
}

void QObjBird::AddCP( const D3DXVECTOR3* pvCP )
{
	if ( cp.size() == 0 )
	{
		SCheckPoint temp2;
		temp2.p = *GetNullPos();
		temp2.l = 0.0;
		cp.push_back( temp2 );
	}

	SCheckPoint temp;
	ZeroMemory( &temp, sizeof(SCheckPoint) );
	temp.p.vPos = *pvCP;
	temp.p.vScale = D3DXVECTOR3(1,1,1);

	SCheckPoint* last = &cp[ cp.size()-1 ];
	temp.l = last->l + D3DXVec3Length( &( temp.p.vPos - last->p.vPos ) );

	cp.push_back(temp);
}