#pragma once

#include "Scintilla.h"
#include "SciLexer.h"
#include "../utils/common.h"

struct CLexerStyle
{
	CLexerStyle()
	{ 
		style	= 0; 
		fore	= RGB( 0, 0, 0 ); 
		back	= RGB( 255, 255, 255 ); 
		size	= -1; 
		face	= NULL; 
		bold	= false; 
	}

	int			style;
	COLORREF	fore;
	COLORREF	back;
	int			size;
	const char* face;
	bool		bold;
};

class CScriptLexer
{
	bool	IsBlockFinished( const char* pBuff );
	void	ReadKeywordsBlock( FILE* pStream, char* pBuff, str_t strBlockName, str_t& str );
	void	ReadStyleBlock( FILE* pStream, char* pBuff, str_t strBlockName, CLexerStyle& style, int iObjStyle );
	bool	TestForParam( str_t strParam, str_t strLine );
	str_t	CutParam( str_t strParam, str_t strLine );
	str_t	GetParamValue( str_t strParam, str_t strLine );
	void	DefineMarker( int marker, int markerType, COLORREF fore, COLORREF back );
	bool	LoadCfg();
	void	SetAStyle( int style, COLORREF fore, COLORREF back = white, int size = -1, const char* face = 0, bool bold = false );
	void	SetAStyle( CLexerStyle style );
	long	GetColorFromString( str_t strColor );

	static const black		= RGB( 0, 0, 0 );
	static const white		= RGB( 0xff, 0xff, 0xff );
	static const red		= RGB( 0xff, 0, 0 );
	static const green		= RGB( 0, 0xff, 0 );
	static const blue		= RGB( 0, 0, 0xff );
	static const gray		= RGB( 0x80, 0x80, 0x80 );
	static const ligthgray	= RGB( 0xab, 0xab, 0xab );
	static const offWhite	= RGB( 0xff, 0xfb, 0xf0 );
	static const darkGreen	= RGB( 0, 0x80, 0 );
	static const darkBlue	= RGB( 0, 0, 0x80 );
	static const teal		= RGB( 0, 0x80, 0x80 );
	static const darkTeal	= RGB( 0, 80, 128 );
	static const purple		= RGB( 0x80, 0, 0x80 );

	str_t m_strConfigPath;

	str_t m_strLuaKeyWords;
	str_t m_strLuaStdFuncs;
	str_t m_strLuaMyFuncs;

	CLexerStyle m_lxsLuaKeyWords;
	CLexerStyle m_lxsLuaStdFuncs;
	CLexerStyle m_lxsLuaMyFuncs;
	CLexerStyle m_lxsLuaComments;
	CLexerStyle m_lxsLuaNumbers;
	CLexerStyle m_lxsLuaStrings;
	CLexerStyle m_lxsLuaOperators;
	CLexerStyle m_lxsLuaDefault;

	HMODULE m_hLexDll;
	bool	m_bLoadFailed;

	public:
		CScriptLexer();
		~CScriptLexer();

		bool	Load( HWND hParentWnd, str_t strAppPath, str_t strWndName );
		LRESULT	SendEditor( UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0 );
		void	MoveEditor( int x, int y, int width, int height );
		void	MoveEditor( CRect& rect );
		void	CommentSelectedBlock();
		void	UncommentSelectedBlock();
		void	SetFocus();
		void	SetLUALex();
		void	SetOutputLex();
		void	SetNoLex();
		void	Disable();
		HWND	m_hEditor;
		int		m_iEditorID;
};
