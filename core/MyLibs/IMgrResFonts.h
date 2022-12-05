#pragma once

#include "QInterfaces.h"

struct QFontListItem
{
	std::wstring		font_id;
	std::wstring		font_name;
	int					iHeight;
	bool				bBold;
	bool				bItalic;

	LPD3DXFONT			pFont;
};

#define g_FontMgr IMgrResFonts::GetSingleton()

struct IMgrResFonts : public IMyD3DUnknown, Singleton <IMgrResFonts>
{
	virtual void					LoadFont( const std::wstring& font_id, const std::wstring& font_name, int iHeight ) = 0;
	virtual void					LoadFont2( const std::wstring& font_id, const std::wstring& font_name, int iHeight, bool bBold, bool bItalic ) = 0;
	virtual void					FreeFont( const std::wstring& font_id ) = 0;
	virtual void					FreeAll() = 0;

	virtual LPD3DXFONT				GetFont( const std::wstring& font_id ) = 0;
	virtual int						GetFontHeight( const std::wstring& font_id ) = 0;
	virtual QFontListItem*			GetFontItem( const std::wstring& font_id ) = 0;

	virtual void					LogEnabled(bool) = 0;
};
typedef IMgrResFonts* LPMGRRESFONTS;