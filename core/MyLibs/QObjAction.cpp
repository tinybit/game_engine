#define STRICT

#include "QObjAction.h"

#include "UtilsConverts.h"
#include "QMath.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "QMgrScripts.h"
#include "IMgrResEfnModels.h"


QObjAction::QObjAction( const std::wstring& efn_id, const std::wstring& sScript )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	SetEfnID( efn_id );
	
	SetScript( sScript );
	g_Script.ExecScript( sScript );
}

void QObjAction::Init()
{
	SetMtrl( g_EfnMgr.GetEfnMtrl( GetEfnID() ) );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjAction::Kill()
{
	OnLostDevice();
	OnDestroyDevice();
}

bool QObjAction::CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos )
{
	if ( GetEnabled() == false )
		return false;

	QEfnDataListItem* efn_temp = g_EfnMgr.GetEfn( GetEfnID() );

	if (!efn_temp) return false;

	EFN_CHARACTERDATA* m_cdData = efn_temp->pcdData;

	if ( !IntersectRay2Sphere( pstRay, &m_cdData->pBoundingSpheres[0].pos, m_cdData->pBoundingSpheres[0].radius, NULL ) )
	{
		return false;
	}

	bool m_bIntersected = false;
	float m_fDist = 999999999.0f;
	float m_fTempDist = -1.0f;
	float m_fU, m_fV, m_fTempU, m_fTempV;
	D3DXVECTOR3 v1, v2, v3;
	int i1, i2, i3, m_iTempI1, m_iTempI2, m_iTempI3;

	for( int i = 0; i < m_cdData->vertex_indices_count/3; i++)
	{
		m_iTempI1 = m_cdData->vertex_indices_list[i*3+0];
		m_iTempI2 = m_cdData->vertex_indices_list[i*3+1];
		m_iTempI3 = m_cdData->vertex_indices_list[i*3+2];
		
		if ( D3DXIntersectTri(	&D3DXVECTOR3(m_cdData->posList[m_iTempI1].x, m_cdData->posList[m_iTempI1].y, m_cdData->posList[m_iTempI1].z), 
								&D3DXVECTOR3(m_cdData->posList[m_iTempI2].x, m_cdData->posList[m_iTempI2].y, m_cdData->posList[m_iTempI2].z), 
								&D3DXVECTOR3(m_cdData->posList[m_iTempI3].x, m_cdData->posList[m_iTempI3].y, m_cdData->posList[m_iTempI3].z), 
								&pstRay->vPos, &pstRay->vDir, &m_fTempU, &m_fTempV, &m_fTempDist ) )
		{
			m_bIntersected = true;

			if ( m_fTempDist<m_fDist )
			{
				m_fDist = m_fTempDist;
				m_fU = m_fTempU;
				m_fV = m_fTempV;
				i1 = m_iTempI1;
				i2 = m_iTempI2;
				i3 = m_iTempI3;
			}
		}
	}

	if ( m_bIntersected && pfDist)
	{
		*pfDist = m_fDist;
	}
	if ( m_bIntersected && pvIntrPos)
	{
		D3DXVec3BaryCentric( pvIntrPos, 
							 &D3DXVECTOR3(m_cdData->posList[i1].x, m_cdData->posList[i1].y, m_cdData->posList[i1].z), 
							 &D3DXVECTOR3(m_cdData->posList[i2].x, m_cdData->posList[i2].y, m_cdData->posList[i2].z), 
							 &D3DXVECTOR3(m_cdData->posList[i3].x, m_cdData->posList[i3].y, m_cdData->posList[i3].z), 
							 m_fU, m_fV );
	}

	return m_bIntersected;
}

void QObjAction::ObjRender( DWORD rt )
{
	if ( GetVisible() )
		g_EfnMgr.Render( GetEfnID(), (RENDER_TYPES)rt );
}