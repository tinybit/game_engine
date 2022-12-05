#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "IMgrResFonts.h"

using namespace std;

typedef map< std::wstring, QFontListItem*, std::less<wstring> > fontmap;
typedef fontmap::iterator fontmap_iter;

class QMgrResFonts : public IMgrResFonts
{
public:
	
	QMgrResFonts();
	~QMgrResFonts();

	void					LoadFont( const std::wstring& font_id, const std::wstring& font_name, int iHeight );
	void					LoadFont2( const std::wstring& font_id, const std::wstring& font_name, int iHeight, bool bBold, bool bItalic );
	void					FreeFont( const std::wstring& font_id );
	void					FreeAll();

	LPD3DXFONT				GetFont( const std::wstring& font_id );
	int						GetFontHeight( const std::wstring& font_id );
	QFontListItem*			GetFontItem( const std::wstring& font_id );

	void					LogEnabled(bool);

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnLostDevice();
	void					OnDestroyDevice();

	void					Release() { delete this; };

private:

	fontmap					m_mFonts;

	bool					m_bLogEnabled;
};