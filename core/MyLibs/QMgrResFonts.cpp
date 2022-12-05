#define STRICT

#include "QMgrResFonts.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

QMgrResFonts::QMgrResFonts()
{
	m_bLogEnabled = false;
}

QMgrResFonts::~QMgrResFonts()
{
	FreeAll();
}

void QMgrResFonts::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"font mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"font mgr logging disabled");
	}
}

void QMgrResFonts::LoadFont( const std::wstring& font_id, const std::wstring& font_name, int iHeight )
{
	LoadFont2( font_id, font_name, iHeight, false, false );
}

void QMgrResFonts::LoadFont2( const std::wstring& font_id, const std::wstring& font_name, int iHeight, bool bBold, bool bItalic )
{
	QFontListItem* temp = GetFontItem( font_id );
	if ( temp )
	{
		if(m_bLogEnabled)
		{
			g_Log.WriteWarning( L"font already loaded, font_id = \"" + font_id + L"\"" );
		}
		return;
	}

	temp = new QFontListItem;
	temp->font_id	= font_id;
	temp->font_name	= font_name;
	temp->iHeight	= (int)(( g_Graph.fWidth/1024 ) * iHeight);
	temp->bBold		= bBold;
	temp->bItalic	= bItalic;
	temp->pFont		= NULL;
	
	DWORD Weight =  ( temp->bBold ) ? FW_BOLD : FW_NORMAL;

	if ( SUCCEEDED( D3DXCreateFont( g_Graph.dev(), temp->iHeight, 0, Weight, 1, temp->bItalic, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						 temp->font_name.c_str(), &temp->pFont ) ) )
	{
		m_mFonts[ font_id ] = temp;				 

		if(m_bLogEnabled)
		{
			g_Log.WriteMessage( L"font was loaded, font_id = \"" + font_id + L"\"" );
		}
	}
	else
	{
		SAFE_DELETE(temp);
		g_Log.WriteError( L"failed to create font, font_id = \"" + font_id + L"\"" );
	}
}

void QMgrResFonts::FreeFont( const std::wstring& font_id )
{
	if ( m_mFonts.size() > 0 )
	{
		fontmap_iter i = m_mFonts.find( font_id );

		if ( i != m_mFonts.end() )
		{
			QFontListItem* temp = i->second;

			if(temp)
			{
				SAFE_RELEASE(temp->pFont);
				SAFE_DELETE(temp);
				if(m_bLogEnabled)
				{
					g_Log.WriteMessage( L"font free, font_id = \"" + font_id + L"\"" );
				}
			}
			m_mFonts.erase(i);
		}
	}
}

void QMgrResFonts::FreeAll()
{
	if ( m_mFonts.size() > 0 )
	{
		for ( fontmap_iter i = m_mFonts.begin(); i != m_mFonts.end(); i++ )
		{
			QFontListItem* temp = i->second;

			if(temp)
			{
				SAFE_RELEASE(temp->pFont);
				SAFE_DELETE(temp);
				if(m_bLogEnabled)
				{
					g_Log.WriteMessage( L"font free, font_id = \"" + i->first + L"\"" );
				}
			}
		}
		m_mFonts.clear();
	}
}

LPD3DXFONT QMgrResFonts::GetFont( const std::wstring& font_id )
{
	QFontListItem* temp = GetFontItem( font_id );

	if ( temp )
	{
		return temp->pFont;
	}
	else
	{
		if ( font_id != L"default" )
		{
			temp = GetFontItem( L"default" );

			if ( temp )
			{
				return temp->pFont;
			}
		}
	}

	return NULL;
}

int QMgrResFonts::GetFontHeight( const std::wstring& font_id )
{
	QFontListItem* temp = GetFontItem( font_id );

	if(temp)
	{
		return temp->iHeight;
	}
	else
	{
		if ( font_id != L"default" )
		{
			temp = GetFontItem( L"default" );

			if ( temp )
			{
				return temp->iHeight;
			}
		}
	}

	return 15;	
}

QFontListItem* QMgrResFonts::GetFontItem( const std::wstring& font_id )
{
	if ( m_mFonts.size() > 0 )
	{
		fontmap_iter i = m_mFonts.find( font_id );
		
		if ( i != m_mFonts.end() )
		{
			return i->second;
		}
	}
	return NULL;
}

void QMgrResFonts::OnCreateDevice()
{
	if ( m_mFonts.size() > 0 )
	{
		for ( fontmap_iter i = m_mFonts.begin(); i != m_mFonts.end(); i++ )
		{
			QFontListItem* temp = i->second;

			if ( temp )
			{
				DWORD Weight =  ( temp->bBold ) ? FW_BOLD : FW_NORMAL;

				if ( SUCCEEDED( D3DXCreateFont( g_Graph.dev(), temp->iHeight, 0, Weight, 1, temp->bItalic, DEFAULT_CHARSET, 
									OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
									temp->font_name.c_str(), &temp->pFont ) ) )
				{
					if(m_bLogEnabled)
					{
						g_Log.WriteWarning(L"font was recreated, font = \"" + temp->font_id + L"\"");
					}
				}
				else
				{
					g_Log.WriteError(L"font failed to recreate, font = \"" + temp->font_id + L"\"");
				}
			}
		}
	}
}

void QMgrResFonts::OnResetDevice()
{
	if ( m_mFonts.size() > 0 )
	{
		for ( fontmap_iter i = m_mFonts.begin(); i != m_mFonts.end(); i++ )
		{
			QFontListItem* temp = i->second;

			if ( temp )
			{
				temp->pFont->OnResetDevice();
			}
		}
	}
}

void QMgrResFonts::OnLostDevice()
{
	if ( m_mFonts.size() > 0 )
	{
		for ( fontmap_iter i = m_mFonts.begin(); i != m_mFonts.end(); i++ )
		{
			QFontListItem* temp = i->second;

			if ( temp )
			{
				temp->pFont->OnLostDevice();
			}
		}
	}
}

void QMgrResFonts::OnDestroyDevice()
{
	if ( m_mFonts.size() > 0 )
	{
		for ( fontmap_iter i = m_mFonts.begin(); i != m_mFonts.end(); i++ )
		{
			QFontListItem* temp = i->second;

			if ( temp )
			{
				SAFE_RELEASE(temp->pFont);

				if(m_bLogEnabled)
				{
					g_Log.WriteWarning(L"font was released, font = \"" + temp->font_id + L"\"");
				}
			}
		}
	}
}