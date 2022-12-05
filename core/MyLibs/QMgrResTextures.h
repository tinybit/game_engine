#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "IMgrResTextures.h"

using namespace std;

typedef map< std::wstring, QTextureListItem*, std::less<wstring> > texmap;
typedef texmap::iterator texmap_iter;

class QMgrResTextures : public IMgrResTextures
{
public:
	
	QMgrResTextures();
	~QMgrResTextures();

	void					LoadTex( const std::wstring& tex_id ) { LoadTex2( L"sys", tex_id ); };
	void					LoadTex2( const std::wstring& parent_id, const std::wstring& tex_id );
	void					LoadTex3( const std::wstring& parent_id, const std::wstring& tex_id, UINT iMipLevels, bool bActive );
	void					LoadTex4( const std::wstring& parent_id, const std::wstring& tex_id, UINT iMipLevels, bool bActive, int AlphaLevel, float Left, float Top, UINT RealWidth, UINT RealHeight );
	bool					LoadTex5( QTextureListItem* t );

	void					LoadVideo( const std::wstring& parent_id, const std::wstring& tex_id, bool bActive, int AlphaLevel, bool bPrechache );
	void					LoadVideo2( const std::wstring& parent_id, const std::wstring& tex_id, bool bActive, int AlphaLevel, bool bPrechache, float Left, float Top );

	bool					CreateTexture( const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage );
	bool					CreateTexture2( const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage, D3DFORMAT Format );
	bool					CreateTexture3( const std::wstring& parent_id, const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage, D3DFORMAT Format, bool bActive, bool bForcePow2  );

	void					ClearNeeds();
	void					ClearNeeds( const std::wstring& parent_id );
	void					ClearNotNeeded();

	void					FreeTex( const std::wstring tex_id );
	texmap_iter				FreeTex2( const std::wstring tex_id );
	texmap_iter				FreeTex3( const std::wstring tex_id );
	void					FreeTexs( const std::wstring parent_id );

	void					SetActive( const std::wstring& tex_id, bool bActive );
	void					SetActive( QTextureListItem* temp );
	void					SetActives( const std::wstring& parent_id, bool bActive );

	std::wstring			GetTexFileName( const std::wstring& tex_id ) { return L"data\\Textures\\" + tex_id + L".dds"; };

	bool					ReadTex( const std::wstring& tex_id, D3DCOLOR** p );
	bool					ReadTex( LPDIRECT3DTEXTURE9 tex, UINT Width, UINT Height, DWORD Usage, D3DCOLOR** p );
	bool					FillTex( const std::wstring& tex_id, D3DCOLOR* p );
	bool					FillTex( LPDIRECT3DTEXTURE9 tex, UINT Width, UINT Height, DWORD Usage, D3DCOLOR* p );

	void					SetTex( int tex_num, const std::wstring& tex_id);
	void					SetTex( int tex_num, const std::wstring& tex_id, float fFrame );
	void					SetTexNull( int tex_num );
	LPDIRECT3DTEXTURE9		GetTex( const std::wstring& tex_id );
	LPDIRECT3DTEXTURE9		GetTex( const std::wstring& tex_id, float fFrame );
	QTextureListItem*		GetTexItem( const std::wstring& tex_id );

	void					SetTexFrame( const std::wstring& tex_id, float fFrame ) { GetTex( tex_id, fFrame ); };

	DWORD					GetRimFormatByDX( D3DFORMAT fmt );
	DWORD					GetRimSizeFormatByRes( UINT Width );
	
	void					PurgeCache();
	void					ClearCache() { m_lNowTexID.clear(); };

	void					LogEnabled( bool bEnabled );

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnLostDevice();
	void					OnDestroyDevice();

	void					Release() { delete this; };

private:

	vecWStr					m_lNowTexID;

	texmap					m_mTex;
	mmapWStrWStr			m_mmTex;

	bool					m_bLogEnabled;
};