#define STRICT

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

#include "QSubSysGraphics.h"

#define TextureFormatsCount 26

#include "UtilsConverts.h"

#include "QSubSysLog.h"

#include "IMgrResFonts.h"
#include "IMgrResTextures.h"
#include "QMgrResObjects.h"
#include "QMgrTimers.h"
#include "QEngine.h"

#include "QObjLight.h"

QSubSysGraphics::QSubSysGraphics()
{
	m_iForceDisp			= 0;
	m_iForceRezMode			= 0;
	m_iForceBits			= 0;
	m_iForceTexBits			= 0;
	m_bForceSW				= false;
	m_bForceFS				= false;
	m_bForceDebug			= false;
	m_sForceSceneID			= L"";
	m_sForceNullID			= L"";

	m_bInitOK				= false;
	m_bLogEnabled			= false;
	
	m_pSprite				= NULL;

	m_fGamma				= 1.0f;
	m_fContrast				= 1.0f;
	m_fFadeGoTime			= 0;
	m_fFadeTime				= 0;
	m_iFadeType				= 0;

	ClearRamp();

	m_iLoadIter				= 1;
	m_iLoadAnimationLength	= 5;
	m_vLoadAnimationPos		= D3DXVECTOR3(0, 0, 0);

	m_fTextureQuality		= 0.5f;
	m_iAnisotropicQuality	= 0;
	m_iTextureBits			= 16;
	m_bUseDXT				= false;
	m_bUseMip1ForBacks		= true;
	m_bUseRimTextureLib		= true;

	m_sKeysHelp				= L"";

	D3DXMatrixIdentity( &matIdentity );

										//num  bit format				str					bink				usage					str						  size alp  pool				supported
	format_list.push_back( FormatListItem( 1,   8, D3DFMT_A8,		L"D3DFMT_A8",		-1,					0,						L"0",						1,	2,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem( 2,  32, D3DFMT_A8R8G8B8,	L"D3DFMT_A8R8G8B8",	BINKSURFACE32A,		0,						L"0",						4,	2,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem( 3,  32, D3DFMT_X8R8G8B8,	L"D3DFMT_X8R8G8B8",	BINKSURFACE32,		0,						L"0",						4,	0,	D3DPOOL_MANAGED,	false ) );

	format_list.push_back( FormatListItem( 4,  16, D3DFMT_R5G6B5,	L"D3DFMT_R5G6B5",	BINKSURFACE565,		0,						L"0",						2,	0,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem( 5,  16, D3DFMT_X1R5G5B5,	L"D3DFMT_X1R5G5B5",	BINKSURFACE555,		0,						L"0",						2,	0,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem( 6,  16, D3DFMT_A4R4G4B4,	L"D3DFMT_A4R4G4B4",	BINKSURFACE4444,	0,						L"0",						2,	2,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem( 7,  16, D3DFMT_A1R5G5B5,	L"D3DFMT_A1R5G5B5",	BINKSURFACE5551,	0,						L"0",						2,	1,	D3DPOOL_MANAGED,	false ) );

	format_list.push_back( FormatListItem( 8,   0, D3DFMT_DXT1,		L"D3DFMT_DXT1",		-1,					0,						L"0",						1,	0,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem( 9,   0, D3DFMT_DXT2,		L"D3DFMT_DXT2",		-1,					0,						L"0",						1,	2,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem(10,   0, D3DFMT_DXT3,		L"D3DFMT_DXT3",		-1,					0,						L"0",						1,	2,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem(11,   0, D3DFMT_DXT4,		L"D3DFMT_DXT4",		-1,					0,						L"0",						1,	2,	D3DPOOL_MANAGED,	false ) );
	format_list.push_back( FormatListItem(12,   0, D3DFMT_DXT5,		L"D3DFMT_DXT5",		-1,					0,						L"0",						1,	2,	D3DPOOL_MANAGED,	false ) );

	format_list.push_back( FormatListItem(13,   8, D3DFMT_A8,		L"D3DFMT_A8",		-1,					D3DUSAGE_DYNAMIC,		L"D3DUSAGE_DYNAMIC",		1,	2,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(14,  32, D3DFMT_A8R8G8B8,	L"D3DFMT_A8R8G8B8",	BINKSURFACE32A,		D3DUSAGE_DYNAMIC,		L"D3DUSAGE_DYNAMIC",		4,	2,	D3DPOOL_DEFAULT,	false ) );	
	format_list.push_back( FormatListItem(15,  32, D3DFMT_X8R8G8B8,	L"D3DFMT_X8R8G8B8",	BINKSURFACE32,		D3DUSAGE_DYNAMIC,		L"D3DUSAGE_DYNAMIC",		4,	0,	D3DPOOL_DEFAULT,	false ) );

	format_list.push_back( FormatListItem(16,  16, D3DFMT_R5G6B5,	L"D3DFMT_R5G6B5",	BINKSURFACE565,		D3DUSAGE_DYNAMIC,		L"D3DUSAGE_DYNAMIC",		2,	0,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(17,  16, D3DFMT_X1R5G5B5,	L"D3DFMT_X1R5G5B5",	BINKSURFACE555,		D3DUSAGE_DYNAMIC,		L"D3DUSAGE_DYNAMIC",		2,	0,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(18,  16, D3DFMT_A4R4G4B4,	L"D3DFMT_A4R4G4B4",	BINKSURFACE4444,	D3DUSAGE_DYNAMIC,		L"D3DUSAGE_DYNAMIC",		2,	2,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(19,  16, D3DFMT_A1R5G5B5,	L"D3DFMT_A1R5G5B5",	BINKSURFACE5551,	D3DUSAGE_DYNAMIC,		L"D3DUSAGE_DYNAMIC",		2,	1,	D3DPOOL_DEFAULT,	false ) );

	format_list.push_back( FormatListItem(20,   8, D3DFMT_A8,		L"D3DFMT_A8",		-1,					D3DUSAGE_RENDERTARGET,	L"D3DUSAGE_RENDERTARGET",	1,	2,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(21,  32, D3DFMT_A8R8G8B8,	L"D3DFMT_A8R8G8B8",	BINKSURFACE32A,		D3DUSAGE_RENDERTARGET,	L"D3DUSAGE_RENDERTARGET",	4,	2,	D3DPOOL_DEFAULT,	false ) );	
	format_list.push_back( FormatListItem(22,  32, D3DFMT_X8R8G8B8,	L"D3DFMT_X8R8G8B8",	BINKSURFACE32,		D3DUSAGE_RENDERTARGET,	L"D3DUSAGE_RENDERTARGET",	4,	0,	D3DPOOL_DEFAULT,	false ) );

	format_list.push_back( FormatListItem(23,  16, D3DFMT_R5G6B5,	L"D3DFMT_R5G6B5",	BINKSURFACE565,		D3DUSAGE_RENDERTARGET,	L"D3DUSAGE_RENDERTARGET",	2,	0,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(24,  16, D3DFMT_X1R5G5B5,	L"D3DFMT_X1R5G5B5",	BINKSURFACE555,		D3DUSAGE_RENDERTARGET,	L"D3DUSAGE_RENDERTARGET",	2,	0,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(25,  16, D3DFMT_A4R4G4B4,	L"D3DFMT_A4R4G4B4",	BINKSURFACE4444,	D3DUSAGE_RENDERTARGET,	L"D3DUSAGE_RENDERTARGET",	2,	2,	D3DPOOL_DEFAULT,	false ) );
	format_list.push_back( FormatListItem(26,  16, D3DFMT_A1R5G5B5,	L"D3DFMT_A1R5G5B5",	BINKSURFACE5551,	D3DUSAGE_RENDERTARGET,	L"D3DUSAGE_RENDERTARGET",	2,	1,	D3DPOOL_DEFAULT,	false ) );

	m_pTextBackVB = NULL;
	m_pTextBackVB = new POSRHW_COL_VERTEX[4];
}

QSubSysGraphics::~QSubSysGraphics()
{
	format_list.clear();
	SAFE_DELETE(m_pTextBackVB);
}

void QSubSysGraphics::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage( L"QSubsysGraphics logging enabled");
	}
	else
	{
		g_Log.WriteMessage( L"QSubsysGraphics logging disabled");
	}
}

void QSubSysGraphics::SetWindowPrm( const std::wstring& sTitle, int iDisplay, int iResMode, bool bWindowed )
{
	int m_iSizeX = 1024;
	int m_iSizeY = 768;

	if ( m_iForceRezMode != 0 )
		iResMode = m_iForceRezMode;

	if ( m_iForceDisp != 0 )
		iDisplay = m_iForceDisp;

	switch(iResMode)
	{
		case 1:
			m_iSizeX = 640;
			m_iSizeY = 480;
			break;

		case 2:
			m_iSizeX = 800;
			m_iSizeY = 600;
			break;

		default:
			m_iSizeX = 1024;
			m_iSizeY = 768;
			break;
	}

	m_stWindowPrm.title			= sTitle;
	m_stWindowPrm.display_num	= iDisplay;
	m_stWindowPrm.SizeX			= m_iSizeX;
	m_stWindowPrm.SizeY			= m_iSizeY;
	m_stWindowPrm.windowed		= bWindowed && !m_bForceFS;
}

bool QSubSysGraphics::CreateWnd(bool bForceSoftware, bool bShowSysCursor )
{
	m_bInitOK			= false;
	m_bForceSoftware	= bForceSoftware || m_bForceSW;
	m_bShowSysCursor	= bShowSysCursor || m_bForceDebug;

	// Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( m_bShowSysCursor, true );

	// Initialize the sample framework and create the desired Win32 window and Direct3D 
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
	
	// Parse the command line, handle the default hotkeys, and show msgboxes
	if ( FAILED( DXUTInit( false, true, true ) ) )
	{
		g_Log.WriteError( L"cant init framework" );
		return false;
	}
	
	if ( FAILED( DXUTCreateWindow(	m_stWindowPrm.title.c_str(), NULL, NULL, NULL, 50, 50 ) ) )
	{
		g_Log.WriteError( L"cant create window" );
		return false;
	}
	
	if ( FAILED( DXUTCreateDevice(	m_stWindowPrm.display_num, 
						m_stWindowPrm.windowed, 
						m_stWindowPrm.SizeX, 
						m_stWindowPrm.SizeY, 
						&QSubSysGraphics::IsDeviceAcceptable, 
						&QSubSysGraphics::ModifyDeviceSettings ) ) )
	{
		g_Log.WriteError( L"cant create device" );
		return false;
	}

	m_iMemoryBegins = dev()->GetAvailableTextureMem();

	SetAnisotropicQuality(m_iAnisotropicQuality);

	m_bInitOK = true;
	return true;
}

void QSubSysGraphics::SetTextureQuality(UINT iTextureQuality, UINT iTextureBits, DWORD iAnisotropicQuality )
{
	switch( iTextureQuality )
	{
		case 5:
			m_fTextureQuality = 0.0625;
			break;	
		
		case 4:
			m_fTextureQuality = 0.125f;
			break;

		case 3:
			m_fTextureQuality = 0.25f;
			break;
		
		case 2:
			m_fTextureQuality = 0.5f;
			break;

		//1
		default:
			m_fTextureQuality = 1.0f;
			break;
	}

	if ( m_iForceTexBits != 0 )
		iTextureBits = m_iForceTexBits;
	
	switch ( iTextureBits )
	{
		case 32:
			m_iTextureBits = 32;
			break;

		//16
		default:
			m_iTextureBits = 16;
			break;
	}

	SetAnisotropicQuality( iAnisotropicQuality );

}

void QSubSysGraphics::SetAnisotropicQuality(DWORD iAnisotropicQuality)
{
	if ( iAnisotropicQuality <= g_Graph.caps()->MaxAnisotropy )
	{
		m_iAnisotropicQuality = iAnisotropicQuality;
		g_Graph.dev()->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, m_iAnisotropicQuality );
	}
	else
	{
		g_Graph.dev()->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, g_Graph.caps()->MaxAnisotropy );	
	}
}

void QSubSysGraphics::SetFiltering( DWORD tex_num, DWORD FilterMin, DWORD FilterMag, DWORD FilterMip )
{
	if ( m_iAnisotropicQuality == 0 )
	{
		if ( FilterMin == D3DTEXF_ANISOTROPIC )	FilterMin = D3DTEXF_LINEAR;
		if ( FilterMag == D3DTEXF_ANISOTROPIC )	FilterMag = D3DTEXF_LINEAR;
		if ( FilterMip == D3DTEXF_ANISOTROPIC )	FilterMip = D3DTEXF_LINEAR;
	}
	g_Graph.dev()->SetSamplerState( tex_num, D3DSAMP_MINFILTER, FilterMin );
	g_Graph.dev()->SetSamplerState( tex_num, D3DSAMP_MAGFILTER, FilterMag );
	g_Graph.dev()->SetSamplerState( tex_num, D3DSAMP_MIPFILTER, FilterMip );
}

//-----------------------------------------------------------------------

void QSubSysGraphics::SetLightsFromGroup( void* pObjMgr, const std::wstring& obj_group_id )
{
	QMgrResObjects* objmgr = (QMgrResObjects*)pObjMgr;
	ClearLights();

	mmapWStrWStr_iter beg, end;
	int l = 0;
	if ( objmgr->GetObjGroup( obj_group_id, &beg, &end ) )
	{
		QObjLight* temp = NULL;

		for( mmapWStrWStr_iter i = beg; i != end; i++ )	
		{
			temp = objmgr->GetObjLight(i->second);

			if ( temp )
			{
				g_Graph.dev()->SetLight( l, temp->GetLight() );
				g_Graph.dev()->LightEnable( l++, true );
			}
		}
	}
}

void QSubSysGraphics::ClearLights()
{
	for ( int i = 0; i<8; i++ )
	{
		g_Graph.dev()->LightEnable( i, false );
	}
}

void QSubSysGraphics::OnCreateDevice()
{
	g_Log.WriteMessage( L"OnCreateDevice()" );

	m_iMemoryBegins = dev()->GetAvailableTextureMem();

	D3DXCreateSprite( g_Graph.dev(), &m_pSprite );
}

void QSubSysGraphics::OnResetDevice()
{
	g_Log.WriteMessage( L"OnResetDevice()" );
	
	ClearRamp();

	g_Graph.dev()->GetViewport(&vp);

	fWidth	= (float)vp.Width;
	fHeight	= (float)vp.Height;
	iWidth	= vp.Width;
	iHeight	= vp.Height;
	fAspect	= fWidth / fHeight;
	fMaxX	= fWidth - 1.0f;
	fMaxY	= fHeight - 1.0f;

	g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
    g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	g_Graph.dev()->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	g_Graph.dev()->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	g_Graph.dev()->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
	g_Graph.dev()->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );

    // Set miscellaneous render states
    g_Graph.dev()->SetRenderState( D3DRS_DITHERENABLE,		true );
    g_Graph.dev()->SetRenderState( D3DRS_SPECULARENABLE,	false );
	g_Graph.dev()->SetRenderState( D3DRS_AMBIENT,			0x00000000 );
	g_Graph.dev()->SetRenderState( D3DRS_NORMALIZENORMALS,	true );
	g_Graph.dev()->SetRenderState( D3DRS_COLORVERTEX,		true );  

	g_Graph.dev()->SetSamplerState( 0, D3DSAMP_BORDERCOLOR , 0x00000000 );
	g_Graph.dev()->SetSamplerState( 1, D3DSAMP_BORDERCOLOR , 0x00000000 );

	SetAnisotropicQuality(m_iAnisotropicQuality);

	FillTextureSupportForTheDevice();

	m_pSprite->OnResetDevice();

}

void QSubSysGraphics::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( m_iFadeType > 0 )
	{
		if ( fTime - m_fFadeGoTime < m_fFadeTime )
		{
			float koef = (float) ( (fTime - m_fFadeGoTime) / m_fFadeTime );
			float koefacc = koef;
			for( int i = 1; i < m_iFadeCurve; i++ )
				koefacc *= koef;
			m_fContrast = ( (m_iFadeType == 1) ? koefacc : 1 - koefacc );
		}
		else
		{
			m_fContrast = ( (m_iFadeType == 1) ? 1.0f : 0.0f );
			m_iFadeType = 0;
		}
	}
}

void QSubSysGraphics::OnFrameRender( double fTime, float fElapsedTime )
{
	g_Graph.SetRamp( m_fGamma, m_fContrast, 1 );

	if ( g_StatesMgr.GetStateB( L"sys.ShowInfo" ) )
	{
		std::wstring s = L"";
		s = DXUTGetFrameStats();
		s += L"\n";
		s += DXUTGetDeviceStats();
		s += L"\nИстратил:  " + int2wstr( (m_iMemoryBegins - dev()->GetAvailableTextureMem())*0.0009765625 ) + L" kb;  ";
		s += L"Осталось: " + int2wstr( dev()->GetAvailableTextureMem()*0.0009765625 ) + L" kb";
		RenderText5( 0, 1, s, 0xffffffff, 0x80000000, L"", 5 );
	}

	if ( g_StatesMgr.GetStateB( L"sys.ShowKeysHelp" ) )
		RenderText5( 0, 0, g_Graph.GetKeysHelp(), 0xffffffff, 0x80000000, L"", 5 );
	else
	{
		if ( g_StatesMgr.GetStateB( L"sys.ShowStates" ) )
		{
			if ( g_StatesMgr.GetStateB( L"sys.ShowStates" ) && g_Graph.GetDebug() )
			{
				int a = g_StatesMgr.GetState(L"sys.StatesListStartY");
				if ( a == 99999 )
				{
					g_Graph.RenderText5( -1, 1, g_StatesMgr.StatesList(), 0xff00ff00, 0x80000000, L"small", 5 );
				}
				else if ( a == -99999 )
				{
					g_Graph.RenderText5( -1, -1, g_StatesMgr.StatesList(), 0xff00ff00, 0x80000000, L"small", 5 );
				}
				else
				{
					g_Graph.RenderText4( 5, a, g_StatesMgr.StatesList(), 0xff00ff00, 0x80000000, L"small", 5 );
				}
			}
		}
		else if ( g_StatesMgr.GetStateB( L"sys.ShowEvents" ) )
			RenderText5( 1, -1, g_StatesMgr.EventsList(), 0xffffffff, 0x80000000, L"", 5 );
		else if ( g_StatesMgr.GetStateB( L"sys.ShowTimers" ) )
			RenderText5( 1, -1, g_Timers.TimersList(), 0xffffffff, 0x80000000, L"", 5 );
	}

	if ( GetDebug() )
		RenderText5( 1, 1, int2wstr( DXUTGetFPS() ) + L" / " + int2wstr(g_Log.GetWarningsCount()) + L" / " + int2wstr(g_Log.GetErrorCount()) + L" (" + g_Log.GetLastError() + L")", 0xffff0000, 0xaa000000, L"", 5 );
}

void QSubSysGraphics::OnLostDevice()
{
	g_Log.WriteMessage(L"OnLostDevice()");

	m_pSprite->OnLostDevice();
}

void QSubSysGraphics::OnDestroyDevice()
{
	g_Log.WriteMessage(L"OnDestroyDevice()");
	
	SAFE_RELEASE( m_pSprite );
}

void QSubSysGraphics::RenderLoad( const std::wstring& sText )
{
	if ( !dev() )
		return;

	//g_Input.SetEnabled( false );

	g_Graph.Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);

	if( SUCCEEDED( dev()->BeginScene() ) )
    {
		LPDIRECT3DTEXTURE9 temp = g_TexMgr.GetTex( L"load"+int2wstr(m_iLoadIter) );

		if ( temp )
		{
			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

			m_pSprite->Draw( temp , NULL, NULL, &m_vLoadAnimationPos, 0xFFFFFFFF);

			m_pSprite->End();
		}
		else
		{
			g_Graph.Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
		}

		D3DRECT rect;
		rect.x1 = 0;
		rect.y1 = vp.Height-21;
		rect.x2 = vp.Width;
		rect.y2 = vp.Height;
		g_Graph.Clear(1, &rect, D3DCLEAR_TARGET, 0xFF888888, 1.0f, 0);

		RenderText2( 10, vp.Height - 18, sText, 0xff000000 );

		dev()->EndScene();

		dev()->Present( NULL, NULL, NULL, NULL );

		m_iLoadIter++;

		if ( m_iLoadIter > m_iLoadAnimationLength )
			m_iLoadIter = 1;
    }
}

void QSubSysGraphics::RenderText4( int iPosX, int iPosY, const std::wstring& sText, D3DCOLOR c, D3DCOLOR back_c, const std::wstring& font_id, int iBorder )
{
	QFontListItem* temp = ( font_id != L"" ? g_FontMgr.GetFontItem( font_id ) : g_FontMgr.GetFontItem( L"default" ) );
												 
	if ( !temp || !m_pSprite )
		return;
	
    RECT rc;
	SetRect( &rc, iPosX, iPosY, 0, 0 );
	HRESULT hr = temp->pFont->DrawText( m_pSprite, sText.c_str(), -1, &rc, DT_NOCLIP | DT_CALCRECT, c );
	if ( SUCCEEDED(hr) && back_c != 0 )
	{
		m_pTextBackVB[0].p = D3DXVECTOR4( rc.left - iBorder, rc.top - iBorder, 0, 1 );
		m_pTextBackVB[1].p = D3DXVECTOR4( rc.right + iBorder, rc.top - iBorder, 0, 1 );
		m_pTextBackVB[2].p = D3DXVECTOR4( rc.left - iBorder, rc.bottom + iBorder, 0, 1 );
		m_pTextBackVB[3].p = D3DXVECTOR4( rc.right + iBorder, rc.bottom + iBorder, 0, 1 );
		m_pTextBackVB[0].c = m_pTextBackVB[1].c = m_pTextBackVB[2].c = m_pTextBackVB[3].c = back_c;

		g_Graph.dev()->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

		g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,				D3DSHADE_FLAT );
		g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,				D3DCULL_CCW );

		g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,				D3DZB_FALSE );
		g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,			false );

		dev()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		SetAlphaBlendNormal();
		dev()->SetFVF( POSRHW_COL_FVF );
		dev()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pTextBackVB, sizeof(POSRHW_COL_VERTEX) );
		SetAlphaBlendOff();
		
		g_Graph.dev()->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	}
	m_pSprite->Begin( D3DXSPRITE_ALPHABLEND );
	temp->pFont->DrawText( m_pSprite, sText.c_str(), -1, &rc, DT_NOCLIP, c );
	m_pSprite->End();
}

void QSubSysGraphics::RenderText5( int iPosX, int iPosY, const std::wstring& sText, D3DCOLOR c, D3DCOLOR back_c, const std::wstring& font_id, int iBorder )
{
	QFontListItem* temp = ( font_id != L"" ? g_FontMgr.GetFontItem( font_id ) : g_FontMgr.GetFontItem( L"default" ) );
												 
	if ( !temp || !m_pSprite )
		return;
	
	iBorder *= fWidth/1024;

    RECT rc;
	RECT rc2;
	SetRect( &rc, 0, 0, 0, 0 );
	HRESULT hr = temp->pFont->DrawText( m_pSprite, sText.c_str(), -1, &rc, DT_NOCLIP | DT_CALCRECT, c );
	rc2.left = rc.left - iBorder;
	rc2.right = rc.right + iBorder;
	rc2.top = rc.top - iBorder;
	rc2.bottom = rc.bottom + iBorder;
	if ( SUCCEEDED(hr) )
	{
		if ( iPosX == -1 )
		{
			rc2.left += iBorder;
			rc2.right += iBorder;
		}
		else if ( iPosX == 0 )
		{
			int temp = rc2.right - rc2.left;
			rc2.left = (iWidth-1 - temp) / 2;
			rc2.right = rc2.left + temp;
		}
		else if ( iPosX == 1 )
		{
			int temp = rc2.right - rc2.left;
			rc2.left = iWidth-1 - temp;
			rc2.right = iWidth;
		}

		if ( iPosY == -1 )
		{
			rc2.top += iBorder;
			rc2.bottom += iBorder;
		}
		else if ( iPosY == 0 )
		{
			int temp = rc2.bottom - rc2.top;
			rc2.top = (iHeight-1 - temp) / 2;
			rc2.bottom = rc2.top + temp;
		}
		else if ( iPosY == 1 )
		{
			int temp = rc2.bottom - rc2.top;
			rc2.top = iHeight-1 - temp;
			rc2.bottom = iWidth;
		}

		rc.left		= rc2.left + iBorder;
		rc.right	= rc2.right - iBorder;
		rc.top		= rc2.top + iBorder;
		rc.bottom	= rc2.bottom - iBorder;

		if ( back_c != 0 )
		{
			m_pTextBackVB[0].p = D3DXVECTOR4( rc2.left, rc2.top, 0, 1 );
			m_pTextBackVB[1].p = D3DXVECTOR4( rc2.right, rc2.top, 0, 1 );
			m_pTextBackVB[2].p = D3DXVECTOR4( rc2.left, rc2.bottom, 0, 1 );
			m_pTextBackVB[3].p = D3DXVECTOR4( rc2.right, rc2.bottom, 0, 1 );

			m_pTextBackVB[0].c = m_pTextBackVB[1].c = m_pTextBackVB[2].c = m_pTextBackVB[3].c = back_c;

			g_Graph.dev()->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

			g_Graph.dev()->SetRenderState( D3DRS_SHADEMODE,				D3DSHADE_FLAT );
			g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,				D3DCULL_CCW );

			g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,				D3DZB_FALSE );
			g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,			false );

			dev()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
			dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			SetAlphaBlendNormal();
			dev()->SetFVF( POSRHW_COL_FVF );
			dev()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pTextBackVB, sizeof(POSRHW_COL_VERTEX) );
			SetAlphaBlendOff();
			
			g_Graph.dev()->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		}

		m_pSprite->Begin( D3DXSPRITE_ALPHABLEND );
		temp->pFont->DrawText( m_pSprite, sText.c_str(), -1, &rc, DT_NOCLIP, c );
		m_pSprite->End();
	}
}

void QSubSysGraphics::SetAlphaTestForNonZeroAlpha()
{
	if ( caps()->AlphaCmpCaps & D3DPCMPCAPS_NOTEQUAL )
	{
		dev()->SetRenderState( D3DRS_ALPHATESTENABLE,	true ); 
		dev()->SetRenderState( D3DRS_ALPHAREF,			(DWORD)0x00000000 );
		dev()->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_NOTEQUAL );
	}
}

void QSubSysGraphics::SetAlphaBlendNormal()
{					 
	dev()->SetRenderState( D3DRS_ALPHABLENDENABLE,	true );
	dev()->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	dev()->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );        
}

void QSubSysGraphics::SetAlphaBlendForNoRender()
{
	dev()->SetRenderState( D3DRS_ALPHABLENDENABLE,	true );
	dev()->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
    dev()->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE );
}

bool QSubSysGraphics::IsValidTextureFormat( D3DFORMAT AdapterFormat, D3DFORMAT TextureFormat, DWORD Usage )
{
	if ( GetUseDXT()==false && 
		 ( TextureFormat==D3DFMT_DXT1 || 
		   TextureFormat==D3DFMT_DXT2 || 
		   TextureFormat==D3DFMT_DXT3 || 
		   TextureFormat==D3DFMT_DXT4 ||
		   TextureFormat==D3DFMT_DXT5  ) )
		   return false;

	return SUCCEEDED( DXUTGetD3DObject()->CheckDeviceFormat( g_Graph.GetWindowPrm()->display_num, D3DDEVTYPE_HAL, 
							AdapterFormat, D3DUSAGE_DYNAMIC, D3DRTYPE_TEXTURE, TextureFormat) );
}

SSupportedTextures QSubSysGraphics::FormatListItem( UINT num, UINT TextureBits,	D3DFORMAT Format, const std::wstring& FormatStr, int BinkSurface, DWORD Usage, const std::wstring& UsageStr, UINT PixelSize, UINT AlphaLevel, D3DPOOL Pool, bool Supported )
{
	SSupportedTextures temp;
	temp.num = num;
	temp.TextureBits = TextureBits;
	temp.Format = Format;
	temp.FormatStr = FormatStr;
	temp.BinkSurface = BinkSurface;
	temp.Usage = Usage;
	temp.UsageStr = UsageStr;
	temp.PixelSize = PixelSize;
	temp.AlphaLevel = AlphaLevel;
	temp.Pool = Pool;
	temp.Supported = Supported;
	return temp;
}

void QSubSysGraphics::FillTextureSupportForTheDevice()
{
	D3DDISPLAYMODE dm;
	DXUTGetD3DObject()->GetAdapterDisplayMode( g_Graph.GetWindowPrm()->display_num, &dm );
	D3DFORMAT af = dm.Format;

	for( UINT i = 0; i < format_list.size(); i++ )
	{
		format_list[i].Supported = IsValidTextureFormat( af, format_list[i].Format, format_list[i].Usage );
		if ( format_list[i].Supported )
		{
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning( L"texture format = \"" + format_list[i].FormatStr + L"\" supported for usage = \"" + format_list[i].UsageStr + L"\"" );
			}
		}
		else
		{
			g_Log.WriteError( L"texture format = \"" + format_list[i].FormatStr + L"\" not supported for usage = \"" + format_list[i].UsageStr + L"\"" );
		}
	}
}

D3DFORMAT* QSubSysGraphics::GetCompFormat( D3DFORMAT Format, DWORD Usage, bool bBinkSurfaceCompatible, bool bUseSelf )
{
	SSupportedTextures* temp = GetCompTex( Format, Usage, bBinkSurfaceCompatible, bUseSelf );

	return ( temp ? &temp->Format : NULL );
}

SSupportedTextures* QSubSysGraphics::GetCompTex( D3DFORMAT Format, DWORD Usage, bool bBinkSurfaceCompatible, bool bUseSelf )
{
	int AlphaLevel = 0;

	//Проверяем что данный формат поддерживается
	for( UINT i = 0; i < format_list.size(); i++ )
	{
		if ( format_list[i].Format==Format )
		{
			//ищем параметры для формата
			AlphaLevel = format_list[i].AlphaLevel;

			if ( format_list[i].Usage==Usage &&	
				( format_list[i].BinkSurface!=-1 || bBinkSurfaceCompatible==false ) &&
				format_list[i].Supported )
			{
				if ( bUseSelf )
				{
					return &format_list[i];
				}
			}
		}
	}

	//Если не поддерживается ищем совместимый, битность совместимого берется из настроек экрана
	return GetCompTex( GetTextureBits(), AlphaLevel, Usage, bBinkSurfaceCompatible );
}

SSupportedTextures* QSubSysGraphics::GetCompTex( D3DFORMAT Format, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible, bool bUseSelf )
{
	if ( AlphaLevel < -1 || AlphaLevel > 2 )
	{
		AlphaLevel = -1;
	}

	//Проверяем что данный формат поддерживается
	for( UINT i = 0; i < format_list.size(); i++ )
	{
		if ( format_list[i].Format==Format )
		{
			if(AlphaLevel==-1)
			{
				AlphaLevel = format_list[i].AlphaLevel;
			}
			if ( format_list[i].Usage==Usage &&	
				( format_list[i].BinkSurface!=-1 || bBinkSurfaceCompatible==false ) &&
				format_list[i].Supported )
			{
				if ( bUseSelf )
				{
					return &format_list[i];
				}
			}
		}
	}

	//Если не поддерживается ищем совместимый, битность совместимого берется из настроек экрана
	return GetCompTex( GetTextureBits(), AlphaLevel, Usage, bBinkSurfaceCompatible );
}

SSupportedTextures* QSubSysGraphics::GetCompTex( UINT TextureBits, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible )
{
	if ( AlphaLevel < -1 || AlphaLevel > 2 )
	{
		g_Log.WriteError( L"unspecefied alpha level in \"SSupportedTextures* QSubSysGraphics::GetCompTex( UINT TextureBits, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible )\"" );
		AlphaLevel = 2;
	}
	if ( !g_Graph.IsDeviceSupportDynamictextures() && Usage==D3DUSAGE_DYNAMIC )
	{
		Usage = 0;
	}

	SSupportedTextures* temp = NULL;

	//Ищем с начальными параметрами
	temp = SearchSupTex( TextureBits, AlphaLevel, Usage, bBinkSurfaceCompatible );

	if ( temp )
		return temp;

	//Если ненашли то ищем в 16-ти битах
	if ( TextureBits != 16 )
		temp = SearchSupTex( 16, AlphaLevel, Usage, bBinkSurfaceCompatible );

	if ( temp )
		return temp;

	//Если ненашли то ищем в 32-х битах
	if ( TextureBits != 32 )
		temp = SearchSupTex( 32, AlphaLevel, Usage, bBinkSurfaceCompatible );

	if ( temp )
		return temp;

	//Если ненашли и нам нужна градиентная альфа то пытаемся найти формат хотябы для 
	//монохромной альфы
	if ( AlphaLevel == 2)
	{
		AlphaLevel = 1;

		//Если ненашли то ищем в 16-ти битах
		if(TextureBits!=16)
			temp = SearchSupTex( 16, AlphaLevel, Usage, bBinkSurfaceCompatible );

		if(temp)
			return temp;

		//Если ненашли то ищем в 32-х битах
		if(TextureBits!=32)
			temp = SearchSupTex( 32, AlphaLevel, Usage, bBinkSurfaceCompatible );

		if(temp)
			return temp;
	}

	//Если не нашли совсем
	g_Log.WriteError( L"cant find texture compatable format for request: bits=\"" + int2wstr(TextureBits) + L"\", AlphaLevel=\"" + int2wstr(AlphaLevel) + L"\", Usage=\"" + int2wstr(Usage) + L"\"" );
	
	return NULL;
}

SSupportedTextures* QSubSysGraphics::SearchSupTex( UINT TextureBits, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible )
{
	if ( AlphaLevel < -1 || AlphaLevel > 2 )
	{
		g_Log.WriteError( L"unspecefied alpha level in \"SSupportedTextures* QSubSysGraphics::SearchSupTex( UINT TextureBits, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible )\"" );
		AlphaLevel = 2;
	}

	for( UINT i = 0; i < format_list.size(); i++ )
	{
		if ( format_list[i].TextureBits==TextureBits &&
			 format_list[i].AlphaLevel==AlphaLevel && 
			 format_list[i].Usage==Usage &&
			 ( format_list[i].BinkSurface!=-1 || bBinkSurfaceCompatible==false ) &&
			 format_list[i].Supported)
		{
			return &format_list[i];
		}
	}
	for( UINT i = 0; i < format_list.size(); i++ )
	{
		if ( format_list[i].TextureBits==TextureBits &&
			 format_list[i].AlphaLevel > AlphaLevel && 
			 format_list[i].Usage==Usage &&
			 ( format_list[i].BinkSurface!=-1 || bBinkSurfaceCompatible==false ) &&
			 format_list[i].Supported)
		{
			return &format_list[i];
		}
	}
	return NULL;
}

std::wstring QSubSysGraphics::GetFormatStr( D3DFORMAT fmt )
{
	for( UINT i = 0; i < format_list.size(); i++ )
	{
		if ( format_list[i].Format == fmt )
		{
			return format_list[i].FormatStr;
		}
	}
	return L"unknown format";
}

UINT QSubSysGraphics::GetTextureBits( D3DFORMAT fmt )
{
	for( UINT i = 0; i < format_list.size(); i++ )
	{
		if ( format_list[i].Format == fmt )
		{
			return format_list[i].TextureBits;
		}
	}
	return -1;
}

void QSubSysGraphics::GoFadeUp2( float fFadeTime, int iCurve )
{
	m_fFadeGoTime = g_Engine.GetTime();
	m_fFadeTime = fFadeTime;
	m_iFadeType = 1;
	m_iFadeCurve = iCurve;
};

void QSubSysGraphics::GoFadeDown2( float fFadeTime, int iCurve )
{
	m_fFadeGoTime = g_Engine.GetTime();
	m_fFadeTime = fFadeTime;
	m_iFadeType = 2;
	m_iFadeCurve = iCurve;
};

//---------------------------------------------------------------------------
// Функция установки Гаммы, контраста и яркости.
// Установка по умолчанию для каждого параметра = 1.0f
// Работает только в fullscreen'е
//---------------------------------------------------------------------------
void QSubSysGraphics::SetRamp( float fGamma, float fContrast, float fBrightness )
{
	if ( fGamma == m_fGammaSet && fContrast == m_fContrastSet && fBrightness == m_fBrightnessSet )
	{
		return;
	}

	m_fGammaSet = fGamma;
	m_fContrastSet = fContrast;
	m_fBrightnessSet = fBrightness;

	float angle = fContrast;
	float offset = ( fBrightness - 1.0f ) * 256.0f;
	for (int i = 0; i < 256; i++)
	{
		float k = 0.00390625f * i;// = i / 256.0f;
		k = pow( k, 1.0f / fGamma );
		k = k * 256.0f;
		float value = k * angle * 256.0f + offset * 256.0f;
		if (value > 65535.0f)  value = 65535.0f;
		if (value < 0.0f)    value = 0.0f;

		ramp.red[i] = ramp.green[i] = ramp.blue[i] = (WORD)value;
	}
	dev()->SetGammaRamp( NULL, NULL, &ramp );
}


//--------------------------------------------------------------------------------------
// Returns true if a particular depth-stencil format can be created and used with
// an adapter format and backbuffer format combination.
//--------------------------------------------------------------------------------------
bool QSubSysGraphics::IsDepthFormatOk( D3DFORMAT DepthFormat,
                      D3DFORMAT AdapterFormat,
                      D3DFORMAT BackBufferFormat )
{
    // Verify that the depth format exists
    HRESULT hr = DXUTGetD3DObject()->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
														D3DDEVTYPE_HAL,
														AdapterFormat,
														D3DUSAGE_DEPTHSTENCIL,
														D3DRTYPE_SURFACE,
														DepthFormat );
    if( FAILED( hr ) ) return false;

    // Verify that the backbuffer format is valid
    hr = DXUTGetD3DObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                                D3DDEVTYPE_HAL,
                                                AdapterFormat,
                                                D3DUSAGE_RENDERTARGET,
                                                D3DRTYPE_SURFACE,
                                                BackBufferFormat );
    if( FAILED( hr ) ) return false;

    // Verify that the depth format is compatible
    hr = DXUTGetD3DObject()->CheckDepthStencilMatch( D3DADAPTER_DEFAULT,
                                                     D3DDEVTYPE_HAL,
                                                     AdapterFormat,
                                                     BackBufferFormat,
                                                     DepthFormat );

    return SUCCEEDED(hr);
}

//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK QSubSysGraphics::IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                  D3DFORMAT BackBufferFormat, bool bWindowed )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    //// Must support vertex shader 1.1
    //if( pCaps->VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
    //    return false;

    //// Must support pixel shader 1.1
    //if( pCaps->PixelShaderVersion < D3DPS_VERSION( 1, 1 ) )
    //    return false;

    // Must support stencil buffer
	if( !g_Graph.IsDepthFormatOk( D3DFMT_D24S8,
                          AdapterFormat,
                          BackBufferFormat ) &&
        !g_Graph.IsDepthFormatOk( D3DFMT_D24X4S4,
                          AdapterFormat,
                          BackBufferFormat ) &&
		!g_Graph.IsDepthFormatOk( D3DFMT_D24FS8,
                          AdapterFormat,
                          BackBufferFormat ) &&
        !g_Graph.IsDepthFormatOk( D3DFMT_D15S1,
                          AdapterFormat,
                          BackBufferFormat )
      )
      return false;

    return true;
}

//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// the sample framework will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
void CALLBACK QSubSysGraphics::ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps )
{
	if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0 && !g_Graph.GetFlagForceSoftware() )
	{
		if ( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
		{
			pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
		else
		{
			pDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

			if ( (pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 )
			{
				pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;
			}
		}
    }
	else
    {
		pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

	pDeviceSettings->pp.MultiSampleType = g_Graph.GetMultiSampleType();
	pDeviceSettings->pp.MultiSampleQuality = g_Graph.GetMultiSampleQuality();

	// Debugging vertex shaders requires either REF or software vertex processing
    // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif

    // This sample requires a stencil buffer.
    if( g_Graph.IsDepthFormatOk( D3DFMT_D24S8, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat ) )
	{
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;
	}
    else if( g_Graph.IsDepthFormatOk( D3DFMT_D24X4S4, pDeviceSettings->AdapterFormat,pDeviceSettings->pp.BackBufferFormat ) )
	{
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24X4S4;
	}
    else if( g_Graph.IsDepthFormatOk( D3DFMT_D24FS8, pDeviceSettings->AdapterFormat,pDeviceSettings->pp.BackBufferFormat ) )
	{
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24FS8;
	}
    else if( g_Graph.IsDepthFormatOk( D3DFMT_D15S1, pDeviceSettings->AdapterFormat,pDeviceSettings->pp.BackBufferFormat ) )
	{
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D15S1;
	}
}
 
//bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
//                                  D3DFORMAT BackBufferFormat, bool bWindowed )
//{
//	return g_Graph.mIsDeviceAcceptable( pCaps, AdapterFormat, BackBufferFormat, bWindowed );
//}
//
//void CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps )
//{
//	g_Graph.mModifyDeviceSettings(pDeviceSettings, pCaps);
//}
