#define STRICT

#include "QObjLayerGeom.h"

#include "UtilsConverts.h"
#include "QMath.h"

#include "QSubSysGraphics.h"

#include "QMgrScripts.h"

#include "QObjCamera.h"


QObjLayerGeom::QObjLayerGeom( const std::wstring& efn_id, const std::wstring& sScript )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	SetEfnID( efn_id );
	SetScript( sScript );
}

void QObjLayerGeom::Init()
{
	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjLayerGeom::Kill()
{
	OnLostDevice();
	OnDestroyDevice();
}

void QObjLayerGeom::CalcUV( void* pCam, const QTextureListItem* layer_tex )
{
	QObjCamera* cam = (QObjCamera*)pCam;
	if ( !cam || !layer_tex ) return;

	QTextureListItem* temp_tex = cam->GetTex();
	QEfnDataListItem* efn_temp = g_EfnMgr.GetEfn( GetEfnID() );

	if ( !efn_temp )
		return;
	
	if ( !efn_temp->pcdData )
		return;

	float left, top, width_koef, height_koef;
	
	//Для слоев которые берутся из главного фона
	if ( temp_tex->tex_id == layer_tex->tex_id )
	{
		left = -( (float)(temp_tex->RealWidth-temp_tex->ImgWidth)*0.5f ) / temp_tex->TexWidth;
		top = -( (float)(temp_tex->RealHeight-temp_tex->ImgHeight)*0.5f ) / temp_tex->TexHeight;

		width_koef = temp_tex->fWidthAspect / g_Graph.fWidth * (float)temp_tex->RealWidth / temp_tex->ImgWidth;
		height_koef = temp_tex->fHeightAspect / g_Graph.fHeight * (float)temp_tex->RealHeight / temp_tex->ImgHeight;
	}
	else
	{
		//Для дополнительных слоев
		if ( layer_tex->tt == TT_VIDEO )
		{
			left = -( (float) (temp_tex->RealWidth*1024.0f / g_Graph.fWidth-temp_tex->ImgWidth*1024.0f / g_Graph.fWidth)*0.5f + layer_tex->Left ) / layer_tex->TexWidth;
			top = -( (float) (temp_tex->RealHeight*1024.0f / g_Graph.fWidth-temp_tex->ImgHeight*1024.0f / g_Graph.fWidth)*0.5f + layer_tex->Top ) / layer_tex->TexHeight;
		}
		else
		{
			left = -( (float) (temp_tex->RealWidth-temp_tex->ImgWidth)*0.5f + layer_tex->Left ) / layer_tex->TexWidth;
			top = -( (float) (temp_tex->RealHeight-temp_tex->ImgHeight)*0.5f + layer_tex->Top ) / layer_tex->TexHeight;
		}

		width_koef = layer_tex->fWidthAspect / g_Graph.fWidth * (float)temp_tex->RealWidth / layer_tex->ImgWidth;
		height_koef = layer_tex->fHeightAspect / g_Graph.fHeight * (float)temp_tex->RealHeight / layer_tex->ImgHeight;
		
		if ( layer_tex->tt == TT_VIDEO )
		{
			width_koef *= 1024.0f / g_Graph.fWidth;
			height_koef *= 768.0f / g_Graph.fHeight;
		}
	}

	EFN_CHARACTERDATA* m_cdData = efn_temp->pcdData;

	D3DXVECTOR3 m_vProjectedVector;
	for( int i = 0; i < m_cdData->vertecies_count; i++)
	{
		D3DXVec3Project( &m_vProjectedVector, &m_cdData->posList[i], &g_Graph.vp, &cam->matProjPerOrig, &cam->matViewPer, &cam->matIdentity );
		
		m_cdData->uvList[i].u = left + width_koef * m_vProjectedVector.x;
		m_cdData->uvList[i].v = top + height_koef * m_vProjectedVector.y;
	}

	g_EfnMgr.FillUVBuffer( efn_temp );
}

inline void QObjLayerGeom::OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	if ( GetVisible() == false )
		return;

	if ( g_Script.ExecBool( GetScript() ) )
		ObjRender( RT_POS_TEX );
}