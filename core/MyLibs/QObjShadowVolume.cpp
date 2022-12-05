#define STRICT

#include "QObjShadowVolume.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "IMgrResTextures.h"
#include "QMgrResStates.h"


QObjShadowVolume::QObjShadowVolume( const std::wstring& efn_id )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	SetEfnID( efn_id );
}

QObjShadowVolume::~QObjShadowVolume()
{
}


void QObjShadowVolume::Init()
{
	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjShadowVolume::Kill()
{
	OnLostDevice();
	OnDestroyDevice();
}

void QObjShadowVolume::CalcShadow()
{
    g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,				true );
	g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,				false );
	g_Graph.SetLightingEnabled( false );

    // Set up stencil compare fuction, reference value, and masks.
    // Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
    // Note: since we set up the stencil-test to always pass, the STENCILFAIL
    // renderstate is really not needed.
    g_Graph.dev()->SetRenderState( D3DRS_STENCILFUNC,				D3DCMP_ALWAYS );
    g_Graph.dev()->SetRenderState( D3DRS_STENCILZFAIL,				D3DSTENCILOP_KEEP );
    g_Graph.dev()->SetRenderState( D3DRS_STENCILFAIL,				D3DSTENCILOP_KEEP );

    // If ztest passes, inc/decrement stencil buffer value
    g_Graph.dev()->SetRenderState( D3DRS_STENCILREF,				128 );
    g_Graph.dev()->SetRenderState( D3DRS_STENCILMASK,				0xffffffff );
    g_Graph.dev()->SetRenderState( D3DRS_STENCILWRITEMASK,			0xffffffff );

    // Make sure that no pixels get drawn to the frame buffer
	g_Graph.SetAlphaBlendForNoRender();

	if( ( DXUTGetDeviceCaps()->StencilCaps & D3DSTENCILCAPS_TWOSIDED ) != 0 )
	{
		// With 2-sided stencil, we can avoid rendering twice:
        g_Graph.dev()->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE,	true );

        g_Graph.dev()->SetRenderState( D3DRS_CCW_STENCILFUNC,		D3DCMP_ALWAYS );
        g_Graph.dev()->SetRenderState( D3DRS_CCW_STENCILZFAIL,		D3DSTENCILOP_KEEP );
        g_Graph.dev()->SetRenderState( D3DRS_CCW_STENCILFAIL,		D3DSTENCILOP_KEEP );
		g_Graph.dev()->SetRenderState( D3DRS_STENCILPASS,			D3DSTENCILOP_DECR );
		g_Graph.dev()->SetRenderState( D3DRS_CCW_STENCILPASS,		D3DSTENCILOP_INCR );

        g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,				D3DCULL_NONE );
		ObjRender(RT_POS);

        g_Graph.dev()->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE,	false );
    }
    else
    {
		g_Graph.dev()->SetRenderState( D3DRS_STENCILPASS,			D3DSTENCILOP_INCR );
        g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,				D3DCULL_CCW );
        ObjRender(RT_POS);

        g_Graph.dev()->SetRenderState( D3DRS_STENCILPASS,			D3DSTENCILOP_DECR );
		g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,				D3DCULL_CW );
		ObjRender(RT_POS);
    }

	g_Graph.SetAlphaBlendOff();
}

void QObjShadowVolume::RenderShadow()
{
    // Set renderstates (disable z-buffering, enable stencil, disable fog, and
    // turn on alphablending)
    
	g_Graph.SetLightingEnabled( true );
	g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,				D3DSHADE_FLAT );
	g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,				D3DCULL_CCW );

	g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,				D3DZB_TRUE );
    g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,			false );
	
	g_Graph.SetAlphaBlendNormal();

	g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,			true );

    g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 );
    g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG2 );

    g_Graph.dev()->SetRenderState( D3DRS_STENCILREF,			128 );
	g_Graph.dev()->SetRenderState( D3DRS_STENCILFUNC,			D3DCMP_GREATER );
    g_Graph.dev()->SetRenderState( D3DRS_STENCILPASS,			D3DSTENCILOP_KEEP );

	g_EfnMgr.RenderOOBB( GetEfnID(), g_Graph.GetIdentity(), 0.0f );

    // Restore render states
    g_Graph.dev()->SetRenderState( D3DRS_STENCILENABLE,			false );
	
	g_Graph.SetAlphaBlendOff();

	g_Graph.SetLightingEnabled( false );
}

void QObjShadowVolume::OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	if ( GetEnabled() == false )
		return;

	if ( g_StatesMgr.GetStateB(L"Quest.rp_RenderShadowVolumesOOBB") )
	{
		g_Graph.SetLightingEnabled( false );
		g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,				D3DSHADE_FLAT );
		g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,				D3DCULL_CCW );

		g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,				D3DZB_TRUE );
		g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,			false );

		g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 );
		g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG2 );
		
		g_EfnMgr.RenderOOBB( GetEfnID(), g_Graph.GetIdentity(), 0.0f );
	}
	else
	{
		CalcShadow();
		RenderShadow();
	}
	
}

void QObjShadowVolume::ObjRender( DWORD rt )
{
	g_EfnMgr.Render( GetEfnID(), (RENDER_TYPES)rt );
}