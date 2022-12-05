#pragma once

#include "QInterfaces.h"

enum TexTypes
{
	TT_LOADED,
	TT_CREATED,
	TT_VIDEO,
	TT_LOADEDSEQ
};

struct QTextureListItem
{
	std::wstring		parent_id;
	std::wstring		tex_id;
	TexTypes			tt;
	float				Left;
	float				Top;
	UINT				TexWidth;
    UINT				TexHeight;
	UINT				ImgWidth;
    UINT				ImgHeight;
	UINT				RealWidth;
	UINT				RealHeight;
    UINT				MipLevels;
	float				fWidthAspect;
	float				fHeightAspect;
    DWORD				Usage;
	int					AlphaLevel;
    D3DFORMAT			Format;
    D3DPOOL				Pool;
	bool				needs;
	bool				Active;
	LPDIRECT3DTEXTURE9	tex;
	
	int					iNowFrame;
	int					iFramesCount;
	float				fFrameRate;

	void*				pFileBuffer;
	UINT				iFileBufferSize;
};

#define g_TexMgr IMgrResTextures::GetSingleton()

struct IMgrResTextures : public IMyD3DUnknown, Singleton <IMgrResTextures>
{
	virtual void					LoadTex( const std::wstring& tex_id ) = 0;
	virtual void					LoadTex2( const std::wstring& parent_id, const std::wstring& tex_id ) = 0;
	virtual void					LoadTex3( const std::wstring& parent_id, const std::wstring& tex_id, UINT iMipLevels, bool bActive ) = 0;
	virtual void					LoadTex4( const std::wstring& parent_id, const std::wstring& tex_id, UINT iMipLevels, bool bActive, int iAlphaLevel, float Left, float Top, UINT RealWidth, UINT RealHeight ) = 0;
	
	virtual bool					CreateTexture( const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage ) = 0;
	virtual bool					CreateTexture2( const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage, D3DFORMAT Format ) = 0;
	virtual bool					CreateTexture3( const std::wstring& parent_id, const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage, D3DFORMAT Format, bool bActive, bool bForcePow2  ) = 0;

	virtual void					LoadVideo( const std::wstring& parent_id, const std::wstring& tex_id, bool bActive, int AlphaLevel, bool bPrechache ) = 0;
	virtual void					LoadVideo2( const std::wstring& parent_id, const std::wstring& tex_id, bool bActive, int AlphaLevel, bool bPrechache, float Left, float Top ) = 0;

	virtual void					FreeTex( const std::wstring tex_id ) = 0;
	virtual void					FreeTexs( const std::wstring parent_id ) = 0;

	virtual void					SetActive( const std::wstring& tex_id, bool bActive ) = 0;
	virtual void					SetActive( QTextureListItem* temp ) = 0;
	virtual void					SetActives( const std::wstring& parent_id, bool bActive ) = 0;

	virtual bool					ReadTex( const std::wstring& tex_id, D3DCOLOR** p ) = 0;
	virtual bool					ReadTex( LPDIRECT3DTEXTURE9 tex, UINT Width, UINT Height, DWORD Usage, D3DCOLOR** p ) = 0;
	virtual bool					FillTex( const std::wstring& tex_id, D3DCOLOR* p ) = 0;
	virtual bool					FillTex( LPDIRECT3DTEXTURE9 tex, UINT Width, UINT Height, DWORD Usage, D3DCOLOR* p ) = 0;

	virtual void					SetTex( int tex_num, const std::wstring& tex_id ) = 0;
	virtual void					SetTex( int tex_num, const std::wstring& tex_id, float fFrame ) = 0;
	virtual void					SetTexNull( int tex_num ) = 0;
	virtual LPDIRECT3DTEXTURE9		GetTex( const std::wstring& tex_id ) = 0;
	virtual LPDIRECT3DTEXTURE9		GetTex( const std::wstring& tex_id, float fFrame ) = 0;
	virtual QTextureListItem*		GetTexItem( const std::wstring& tex_id ) = 0;

	virtual void					SetTexFrame( const std::wstring& tex_id, float fFrame ) = 0;

	virtual void					LogEnabled(bool) = 0;
};
typedef IMgrResTextures* LPMGRRESTEXTURES;