#include ".\d3dtext.h"

//--------------------------------------------------------------------------------------
CD3Dtext::CD3Dtext( ID3DXFont* pFont, ID3DXSprite* pSprite, int nLineHeight )
{
    m_pFont = pFont;
    m_pSprite = pSprite;
    m_clr = D3DXCOLOR(1,1,1,1);
    m_pt.x = 0; 
    m_pt.y = 0; 
    m_nLineHeight = nLineHeight;
}


//--------------------------------------------------------------------------------------
HRESULT CD3Dtext::DrawFormattedTextLine( const char* strMsg, ... )
{
    char strBuffer[512];
    
    va_list args;
    va_start(args, strMsg);
    _vsnprintf( strBuffer, 512, strMsg, args );
    strBuffer[511] = L'\0';
    va_end(args);

    return DrawTextLine( strBuffer );
}


//--------------------------------------------------------------------------------------
HRESULT CD3Dtext::DrawTextLine( char* strMsg )
{
    if( NULL == m_pFont ) 
        AfxMessageBox( "DrawTextLine" );

    HRESULT hr = 0;
    RECT rc;

    SetRect( &rc, m_pt.x, m_pt.y, 0, 0 );

	m_pFont->DrawText( m_pSprite, strMsg, strlen(strMsg) + 1, &rc, DT_CALCRECT, m_clr );
	rc.right -= 5;


	hr = m_pFont->DrawText( m_pSprite, strMsg, strlen(strMsg) + 1, &rc, 0, m_clr );
	

	if( FAILED(hr) )
        AfxMessageBox( "DrawTextLine2" );

    m_pt.y += m_nLineHeight;

    return S_OK;
}


HRESULT CD3Dtext::DrawFormattedTextLine( RECT &rc, DWORD dwFlags, const char* strMsg, ... )
{
    char strBuffer[512];
    
    va_list args;
    va_start(args, strMsg);
    _vsnprintf( strBuffer, 512, strMsg, args );
    strBuffer[511] = L'\0';
    va_end(args);

    return DrawTextLine( rc, dwFlags, strBuffer );
}


HRESULT CD3Dtext::DrawTextLine( RECT &rc, DWORD dwFlags, const char* strMsg )
{
    //if( NULL == m_pFont ) 
    //    return DXUT_ERR_MSGBOX( L"DrawTextLine", E_INVALIDARG );

    HRESULT hr;
    hr = m_pFont->DrawText( m_pSprite, strMsg, -1, &rc, dwFlags, m_clr );
    
	//if( FAILED(hr) )
    //    return DXTRACE_ERR_MSGBOX( L"DrawText", hr );

    m_pt.y += m_nLineHeight;

    return S_OK;
}


//--------------------------------------------------------------------------------------
void CD3Dtext::Begin()
{
    if( m_pSprite )
        m_pSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
}
void CD3Dtext::End()
{
    if( m_pSprite )
        m_pSprite->End();
}