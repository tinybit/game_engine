#pragma once

#include "utils/common.h"

class CD3Dtext
{
	public:
		CD3Dtext( ID3DXFont* pFont, ID3DXSprite* pSprite, int nLineHeight );

		void SetInsertionPos( int x, int y ) { m_pt.x = x; m_pt.y = y; }
		void SetForegroundColor( D3DXCOLOR clr ) { m_clr = clr; }

		void Begin();
		HRESULT DrawFormattedTextLine( const char* strMsg, ... );
		HRESULT DrawTextLine( char* strMsg );
		HRESULT DrawFormattedTextLine( RECT &rc, DWORD dwFlags, const char* strMsg, ... );
		HRESULT DrawTextLine( RECT &rc, DWORD dwFlags, const char* strMsg );
		void End();

	protected:
		ID3DXFont*   m_pFont;
		ID3DXSprite* m_pSprite;
		D3DXCOLOR    m_clr;
		POINT        m_pt;
		int          m_nLineHeight;
};
