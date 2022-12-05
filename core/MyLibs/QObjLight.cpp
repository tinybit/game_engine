#define STRICT

#include "QObjLight.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

QObjLight::QObjLight( const D3DLIGHT9* pLight )
{
	m_stLight				= *pLight;
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	GetNullPos()->vPos		= m_stLight.Position;
}

void QObjLight::Init()
{
	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjLight::Kill()
{
	OnLostDevice();
	OnDestroyDevice();
}

void QObjLight::ObjRender( DWORD rt )
{
}