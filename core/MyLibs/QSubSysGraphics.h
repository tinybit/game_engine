#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "bink.h"

#include "UtilsDefines.h"

struct SSupportedTextures
{
	UINT			num;
	UINT			TextureBits;
	D3DFORMAT		Format;
	std::wstring	FormatStr;
	int				BinkSurface;
	DWORD			Usage;
	std::wstring	UsageStr;
	UINT			PixelSize;
	int				AlphaLevel;
	D3DPOOL			Pool;
	bool			Supported;
};

struct SWindowPrm
{
	std::wstring title;
	int display_num;
	bool windowed;
	int SizeX;
	int SizeY;
};

#define g_Graph QSubSysGraphics::GetSingleton()

class QSubSysGraphics : public Singleton <QSubSysGraphics>
{
public:
	//Constructor/Destructor
	QSubSysGraphics();
	~QSubSysGraphics();

	D3DVIEWPORT9			vp;
	float					fWidth, fHeight, fAspect;
	UINT					iWidth, iHeight;
	float					fMaxX, fMaxY;

	LPDIRECT3DDEVICE9		dev() { return DXUTGetD3DDevice(); };
	const D3DCAPS9*			caps() { return DXUTGetDeviceCaps(); }
	void					Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { dev()->Clear( Count, pRects, Flags, Color, Z, Stencil ); };

	void					SetLightingEnabled( bool bEnabled ) { g_Graph.dev()->SetRenderState( D3DRS_LIGHTING, bEnabled ); };
	void					ClearLights();
	void					SetLightsFromGroup( void* pObjMgr, const std::wstring& obj_group_id );

	void					RenderText( int iPosX, int iPosY, const std::wstring& sText ) { RenderText2( iPosX, iPosY, sText, 0xffffffff ); };
	void					RenderText2( int iPosX, int iPosY, const std::wstring& sText, D3DCOLOR c ) { RenderText3( iPosX, iPosY, sText, c, 0x00000000 ); };
	void					RenderText3( int iPosX, int iPosY, const std::wstring& sText, D3DCOLOR c, D3DCOLOR back_c ) { RenderText4( iPosX, iPosY, sText, c, back_c, L"default", 0 ); };
	void					RenderText4( int iPosX, int iPosY, const std::wstring& sText, D3DCOLOR c, D3DCOLOR back_c, const std::wstring& font_id, int iBorder );
	void					RenderText5( int iPosX, int iPosY, const std::wstring& sText, D3DCOLOR c, D3DCOLOR back_c, const std::wstring& font_id, int iBorder );

	void					RenderLoad( const std::wstring& sText );
	
	void					SetAlphaTestForNonZeroAlpha();
	void					SetAlphaTestOff() { dev()->SetRenderState( D3DRS_ALPHATESTENABLE,	true ); };
	
	void					SetAlphaBlendNormal();
	void					SetAlphaBlendForNoRender();
	void					SetAlphaBlendOff() { dev()->SetRenderState( D3DRS_ALPHABLENDENABLE,	false ); };

	void					SetAntialiasingEnabled( bool bEnabled ) { g_Graph.dev()->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, bEnabled ); };

	void					SetWorld( const D3DXMATRIX* pmatWorld ) { dev()->SetTransform( D3DTS_WORLD, pmatWorld ); };
	void					SetWorldIdentity() { dev()->SetTransform( D3DTS_WORLD, &matIdentity ); };
	D3DXMATRIX*				GetIdentity() { return &matIdentity; };

	void					SetWindowPrm( const std::wstring& sTitle, int iDisplay, int iResMode, bool bWindowed );
	SWindowPrm*				GetWindowPrm() { return &m_stWindowPrm; }
											 
	void					SetForceDisp( int iForceDisp ) { m_iForceDisp = iForceDisp; } ;
	void					SetForceRezMode( int iForceResMode ) { m_iForceRezMode = iForceResMode; } ;
	void					SetForceBits( int iForceBits ) { m_iForceBits = iForceBits; } ;
	void					SetForceTexBits( int iForceTexBits ) { m_iForceTexBits = iForceTexBits; } ;
	void					SetForceSW( bool bForceSW ) { m_bForceSW = bForceSW; } ;
	void					SetForceFS( bool bForceFS ) { m_bForceFS = bForceFS; } ;
	void					SetForceDebug( bool bForceDebug ) { m_bForceDebug = bForceDebug; } ;
	void					SetForceStartScene( const std::wstring& scene_id ) { m_sForceSceneID = scene_id; } ;
	void					SetForceStartNull( const std::wstring& null_id ) { m_sForceNullID = null_id; } ;

	bool					GetDebug() { return m_bForceDebug; };
	
	bool					CreateWnd( bool bForceSoftware, bool bShowSysCursor );

	bool					IsDeviceSupportDynamictextures() { return ( g_Graph.caps()->Caps2 & D3DCAPS2_DYNAMICTEXTURES ) != 0; };
	bool					IsDeviceSupportNonPow2() { return (g_Graph.caps()->TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0; };
	bool					IsValidTextureFormat( D3DFORMAT AdapterFormat, D3DFORMAT TextureFormat, DWORD Usage );

	SSupportedTextures		FormatListItem( UINT num, UINT TextureBits,	D3DFORMAT Format, const std::wstring& FormatStr, int BinkSurface, DWORD Usage, const std::wstring& UsageStr, UINT PixelSize, UINT AlphaLevel, D3DPOOL Pool, bool Supported );
	void					FillTextureSupportForTheDevice();
	D3DFORMAT*				GetCompFormat( D3DFORMAT Format, DWORD Usage, bool bBinkSurfaceCompatible, bool bUseSelf );
	SSupportedTextures*		GetCompTex( D3DFORMAT Format, DWORD Usage, bool bBinkSurfaceCompatible, bool bUseSelf );
	SSupportedTextures*		GetCompTex( D3DFORMAT Format, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible, bool bUseSelf );
	SSupportedTextures*		GetCompTex( UINT TextureBits, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible );
	SSupportedTextures*		SearchSupTex( UINT TextureBits, int AlphaLevel, DWORD Usage, bool bBinkSurfaceCompatible );
	std::wstring			GetFormatStr( D3DFORMAT fmt );
	UINT					GetTextureBits( D3DFORMAT fmt );

	void					SetTextureQuality(UINT iTextureQuality, UINT iTextureBits, DWORD iAnisotropicQuality );
	void					SetAnisotropicQuality( DWORD iAnisotropicQuality );
	void					SetUseDXT( bool bUseDxt ) { m_bUseDXT = bUseDxt; };
	void					SetUseMip1ForBacks( bool bUseMip1ForBacks ) { m_bUseMip1ForBacks = bUseMip1ForBacks; };
	void					SetShadowQuality( int iShadowQuality ) { m_iShadowQuality = iShadowQuality; };
	void					SetMultiSample( DWORD iMultiSampleType, DWORD iMultiSampleQuality ) { m_eMultiSampleType = (D3DMULTISAMPLE_TYPE)iMultiSampleType; m_iMultiSampleQuality = iMultiSampleQuality; };
	void					SetUseRimTextureLib( bool bUseRimTextureLib ) { m_bUseRimTextureLib = bUseRimTextureLib; };
	
	float					GetTextureQuality() { return m_fTextureQuality; };
	UINT					GetTextureBits() { return m_iTextureBits; }
	DWORD					GetAnisotropicQuality() { return m_iAnisotropicQuality; };
	bool					GetUseDXT() { return m_bUseDXT; };
	bool					GetUseMip1ForBacks() { return m_bUseMip1ForBacks; };
	int						GetShadowQuality() { return m_iShadowQuality; };
	D3DMULTISAMPLE_TYPE		GetMultiSampleType() { return m_eMultiSampleType; };
	DWORD					GetMultiSampleQuality() { return m_iMultiSampleQuality; };
	void					SetFilteringAll( DWORD tex_num, DWORD Filter ) { SetFiltering( tex_num, Filter, Filter, Filter ); };
	void					SetFiltering( DWORD tex_num, DWORD FilterMin, DWORD FilterMag, DWORD FilterMip );
	bool					GetUseRimTextureLib() { return m_bUseRimTextureLib; };
	
	bool					GetFlagForceSoftware() { return m_bForceSoftware; };
	bool					GetFlagShowCursor() { return m_bShowSysCursor; };
	bool					GetFlagGraphInitOK() { return m_bInitOK; };
	std::wstring			GetForceStartScene() { return m_sForceSceneID; };
	std::wstring			GetForceStartNull() { return m_sForceNullID; };

	void					SetGamma( float fGamma ) { m_fGamma = fGamma; };

	void					GoFadeUp( float fFadeTime ) { GoFadeUp2( fFadeTime, 1 ); };
	void					GoFadeUp2( float fFadeTime, int iCurve );
	void					GoFadeDown( float fFadeTime ) { GoFadeDown2( fFadeTime, 1 ); };
	void					GoFadeDown2( float fFadeTime, int iCurve );
	void					SetRamp( float WindowGamma, float WindowContrast, float WindowBrightness );
	void					ClearRamp() { m_fContrastSet = m_fGammaSet = m_fBrightnessSet = -1; };

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime );
	void					OnLostDevice();
	void					OnDestroyDevice();

	void					SetLoadAnimLength( int iLoadAnimationLength ) { m_iLoadAnimationLength = iLoadAnimationLength; };
	void					SetLoadAnimPos( D3DXVECTOR3 vPos ) { m_vLoadAnimationPos = vPos; };

	void					SetKeysHelp( const std::wstring& sText ) { m_sKeysHelp = sText; };
	std::wstring			GetKeysHelp() { return m_sKeysHelp; };

	void					LogEnabled( bool bEnabled );

	bool					IsDepthFormatOk( D3DFORMAT DepthFormat,
											D3DFORMAT AdapterFormat,
											D3DFORMAT BackBufferFormat );
	static bool CALLBACK	IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
											D3DFORMAT BackBufferFormat, bool bWindowed );
	static void CALLBACK	ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps );

private:

	int						m_iForceDisp;
	int						m_iForceRezMode;
	int						m_iForceBits;
	int						m_iForceTexBits;
	bool					m_bForceSW;
	bool					m_bForceFS;
	bool					m_bForceDebug;
	std::wstring			m_sForceSceneID;
	std::wstring			m_sForceNullID;

	bool					m_bLogEnabled;

	float					m_fTextureQuality;
	UINT					m_iTextureBits;
	DWORD					m_iAnisotropicQuality;
	bool					m_bUseDXT;
	bool					m_bUseMip1ForBacks;
	D3DMULTISAMPLE_TYPE		m_eMultiSampleType;
	DWORD					m_iMultiSampleQuality;
	int						m_iShadowQuality;
	bool					m_bUseRimTextureLib;

	std::vector<SSupportedTextures>	format_list;
	
	D3DXVECTOR3				m_vLoadAnimationPos;
	int						m_iLoadAnimationLength;
	int						m_iLoadIter;

	bool					m_bForceSoftware;
	bool					m_bShowSysCursor;
	bool					m_bInitOK;

	SWindowPrm				m_stWindowPrm;

	D3DGAMMARAMP			ramp;
	float					m_fGamma;
	float					m_fContrast;
	double					m_fFadeGoTime;
	float					m_fFadeTime;
	int						m_iFadeType;
	int						m_iFadeCurve;

	float					m_fGammaSet;
	float					m_fContrastSet;
	float					m_fBrightnessSet; 

	LPD3DXSPRITE            m_pSprite;

	D3DXMATRIX				matIdentity;

	POSRHW_COL_VERTEX*		m_pTextBackVB;

	UINT					m_iMemoryBegins;

	std::wstring			m_sKeysHelp;
};