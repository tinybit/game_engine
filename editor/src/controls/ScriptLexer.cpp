#pragma warning( disable: 4996 )

#include "../sys/StdAfx.h"
#include "ScriptLexer.h"

CScriptLexer::CScriptLexer()
{
	m_strConfigPath		= "lexer.cfg";
	m_strLuaKeyWords	= "";
	m_strLuaStdFuncs	= "";
	m_strLuaMyFuncs		= "";

	m_bLoadFailed		= false;
	m_hLexDll			= NULL;
	
	//m_hLexDll			= LoadLibrary( "SciLexer.DLL" );

	//if( !m_hLexDll )
	//{
	//	MessageBox( NULL, "The SciLexer.DLL could not be loaded.", "Error loading SciLexer.DLL", MB_OK | MB_ICONERROR );
	//	m_bLoadFailed = true;
	//}
}

CScriptLexer::~CScriptLexer()
{
	//if( m_hLexDll )
	//{
	//	FreeLibrary( m_hLexDll );
	//	m_hLexDll = NULL;
	//}
}

LRESULT	CScriptLexer::SendEditor( UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return ::SendMessage( m_hEditor, Msg, wParam, lParam ); 
}

void CScriptLexer::ReadKeywordsBlock( FILE* pStream, char* pBuff, str_t strBlockName, str_t& str )
{
	char	buff[512];
	str_t	strTemp;

	if( strcmp( pBuff, strBlockName.data() ) == 0 )
	{
		do
		{
			fgets( buff, 511, pStream );
			strTemp = buff;
			strTemp = TrimStr( strTemp );

			sprintf( buff, "%s", strTemp.data() );
			sprintf( pBuff, "%s", strTemp.data() );

			if( !IsBlockFinished( buff ) && buff[0] != '#' )
			{
				str += buff;
				str += " ";
			}
			else if( IsBlockFinished( buff ) )
			{
				long pos = ftell( pStream );
				strTemp = buff;
				pos -= (long)(sizeof(char) * (strTemp.length() + 2 ));

				fseek( pStream, pos, SEEK_SET );

				return;
			}
		}
		while( !IsBlockFinished( buff ) && !feof( pStream ) );
	}
}

long CScriptLexer::GetColorFromString( str_t strColor )
{
	long res = 0;

	hash_map<str_t, long> hash;

	hash["black"]		= black;
	hash["white"]		= white;
	hash["red"]			= red;
	hash["green"]		= green;
	hash["blue"]		= blue;
	hash["offWhite"]	= offWhite;
	hash["darkGreen"]	= darkGreen;
	hash["darkBlue"]	= darkBlue;
	hash["teal"]		= teal;
	hash["darkTeal"]	= darkTeal;
	hash["purple"]		= purple;

	hash_map<str_t, long>::iterator i = hash.find( strColor );

	if( i != hash.end() )
	{
		return i->second;
	}
	else
	{
		sscanf( strColor.data(), "%x", &res );

		res = ( ( res & 0xff ) << 16 ) | ( ( ( res >> 8 ) & 0xff ) << 8 ) | ( ( res >> 16 ) & 0xff );

		return res;
	}
}

void CScriptLexer::ReadStyleBlock( FILE* pStream, char* pBuff, str_t strBlockName, CLexerStyle& style, int iObjStyle )
{
	char buff[512];

	style.style = iObjStyle;

	if( strcmp( pBuff, strBlockName.data() ) == 0 )
	{
		do
		{
			str_t strTemp, strTemp2;

			fgets( buff, 511, pStream );
			strTemp = buff;
			strTemp2 = TrimStr( strTemp );

			sprintf( buff, "%s", strTemp2.data() );
			sprintf( pBuff, "%s", strTemp2.data() );

			if( !IsBlockFinished( buff ) && buff[0] != '#' )
			{
				// analyze line
				if( TestForParam( "forecolor", strTemp2 ) )
				{
					str_t Value = GetParamValue( "forecolor", strTemp2 );
					style.fore = GetColorFromString( Value );
				}
				else if( TestForParam( "backcolor", strTemp2 ) )
				{
					str_t Value = GetParamValue( "backcolor", strTemp2 );				
					style.back = GetColorFromString( Value );
				}
				else if( TestForParam( "size", strTemp2 ) )
				{
					str_t Value = GetParamValue( "size", strTemp2 );
					style.size = atoi( Value.data() );

					if( style.size < -1 )
						style.size = -1;
				}
				else if( TestForParam( "name", strTemp2 ) )
				{
					str_t Value = GetParamValue( "name", strTemp2 );
					Value = Value.substr( 1, Value.length() - 2 );
					style.face = Value.data();
				}
				else if( TestForParam( "bold", strTemp2 ) )
				{
					str_t Value = GetParamValue( "bold", strTemp2 );

					if( Value == "true" )
						style.bold = true;
					else
						style.bold = false;
				}
				else
				{}
			}
			else if( IsBlockFinished( buff ) )
			{
				long pos = ftell( pStream );
				strTemp = buff;
				pos -= (long)( sizeof(char) * ( strTemp.length() + 2 ) );

				fseek( pStream, pos, SEEK_SET );

				return;
			}
		}
		while( !IsBlockFinished( buff ) && !feof( pStream ) );
	}
}

bool CScriptLexer::TestForParam( str_t strParam, str_t strLine )
{
	if( strLine.substr( 0, strParam.length() ) == strParam )
		return true;

	return false;
}

str_t CScriptLexer::CutParam( str_t strParam, str_t strLine )
{
	CString a = strLine.data();
	a = a.Right( (int)strLine.length() - (int)strParam.length() );

	return a.GetBuffer();
}

str_t CScriptLexer::GetParamValue( str_t strParam, str_t strLine )
{
	str_t strTemp = CutParam( strParam, strLine );

	int eq_idx = (int)strTemp.find( '=', 0 );

	if( eq_idx == -1 )
		return "";

	strTemp = TrimStr( strTemp );

	str_t strTemp2 = CutParam( "=", strTemp );
	strTemp2 = TrimStr( strTemp2 );

	return strTemp2;
}

void CScriptLexer::SetAStyle( int style, COLORREF fore, COLORREF back, int size, const char* face, bool bold )
{
	SendEditor( SCI_STYLESETFORE, style, fore );
	SendEditor( SCI_STYLESETBACK, style, back );

	if( size >= 1 )
		SendEditor( SCI_STYLESETSIZE, style, size );

	if( face )
		SendEditor( SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face) );

	if( bold )
		SendEditor( SCI_STYLESETBOLD, style, bold );
}

void CScriptLexer::SetAStyle( CLexerStyle style )
{
	SetAStyle( style.style, style.fore, style.back, style.size, style.face, style.bold );
}

bool CScriptLexer::IsBlockFinished( const char* pBuff )
{
	if( strcmp( pBuff, "[LuaKeywords]" ) == 0 ||
		strcmp( pBuff, "[LuaKeywordsStyle]" ) == 0 ||
		strcmp( pBuff, "[LuaStdFunctions]" ) == 0 ||
		strcmp( pBuff, "[LuaStdFunctionsStyle]" ) == 0 ||
		strcmp( pBuff, "[LuaMyFunctions]" ) == 0 ||
		strcmp( pBuff, "[LuaMyFunctionsStyle]" ) == 0 ||
		strcmp( pBuff, "[LuaDefaultStyle]" ) == 0 ||
		strcmp( pBuff, "[LuaOperatorStyle]" ) == 0 ||
		strcmp( pBuff, "[LuaCommentsStyle]" ) == 0 ||
		strcmp( pBuff, "[LuaStringsStyle]" ) == 0 ||
		strcmp( pBuff, "[LuaNumbersStyle]" ) == 0 )
	{
		return true;
	}

	return false;
}

bool CScriptLexer::LoadCfg()
{
	char	buff[512];
	FILE*	stream;
	str_t	strTemp;
	
	m_strLuaKeyWords	= "";
	m_strLuaStdFuncs	= "";
	m_strLuaMyFuncs		= "";

	if( FileExists( m_strConfigPath.data() ) == false )
		return false;
	else
	{
		stream = fopen( m_strConfigPath.data(), "r" );

		if( !stream )
		{
			fclose( stream );
			return false;
		}

		//load LuaKeywords list
		while( !feof( stream )  )
		{
			fscanf( stream, "%s", buff );

			ReadKeywordsBlock( stream, buff, "[LuaKeywords]", m_strLuaKeyWords );
			ReadKeywordsBlock( stream, buff, "[LuaStdFunctions]", m_strLuaStdFuncs );
			ReadKeywordsBlock( stream, buff, "[LuaMyFunctions]", m_strLuaMyFuncs );
			ReadStyleBlock( stream, buff, "[LuaKeywordsStyle]", m_lxsLuaKeyWords, SCE_LUA_WORD );
			ReadStyleBlock( stream, buff, "[LuaStdFunctionsStyle]", m_lxsLuaStdFuncs, SCE_LUA_WORD2 );
			ReadStyleBlock( stream, buff, "[LuaMyFunctionsStyle]", m_lxsLuaMyFuncs, SCE_LUA_WORD3 );
			ReadStyleBlock( stream, buff, "[LuaCommentsStyle]", m_lxsLuaComments, SCE_LUA_COMMENT );
			ReadStyleBlock( stream, buff, "[LuaNumbersStyle]", m_lxsLuaNumbers, SCE_LUA_NUMBER );
			ReadStyleBlock( stream, buff, "[LuaDefaultStyle]", m_lxsLuaDefault, SCE_LUA_DEFAULT );
			ReadStyleBlock( stream, buff, "[LuaStringsStyle]", m_lxsLuaStrings, SCE_LUA_STRING );
			ReadStyleBlock( stream, buff, "[LuaOperatorStyle]", m_lxsLuaOperators, SCE_LUA_OPERATOR );
		}
	}

	fclose( stream );

	m_strLuaKeyWords = TrimStr( m_strLuaKeyWords );
	m_strLuaStdFuncs = TrimStr( m_strLuaStdFuncs );
	m_strLuaMyFuncs	 = TrimStr( m_strLuaMyFuncs );

	return true;
}

void CScriptLexer::MoveEditor( int x, int y, int width, int height )
{
	::MoveWindow( m_hEditor, x, y, width, height, true );
}

void CScriptLexer::MoveEditor( CRect& rect )
{
	::MoveWindow( m_hEditor, rect.left, rect.top, rect.Width(), rect.Height(), true );
}

void CScriptLexer::DefineMarker( int marker, int markerType, COLORREF fore, COLORREF back )
{
    SendEditor( SCI_MARKERDEFINE, marker, markerType );
    SendEditor( SCI_MARKERSETFORE, marker, fore );
    SendEditor( SCI_MARKERSETBACK, marker, back );
}

void CScriptLexer::SetFocus()
{
	::SetFocus( m_hEditor );
}

void CScriptLexer::CommentSelectedBlock()
{
	int start	= SendEditor( SCI_GETSELECTIONSTART );
	int end		= SendEditor( SCI_GETSELECTIONEND );
	
	int sLine = SendEditor( SCI_LINEFROMPOSITION, start );
	int eLine = SendEditor( SCI_LINEFROMPOSITION, end );

	vector<int> sPos;

	for( int i = sLine; i <= eLine; i++ )
		sPos.push_back( SendEditor( SCI_GETLINESELSTARTPOSITION, i ) );

	for( UINT i = 0; i < sPos.size(); i++ )
		SendEditor( SCI_INSERTTEXT, sPos[i] + i * 2, (LPARAM)"--\0" );
}

void CScriptLexer::UncommentSelectedBlock()
{
	int start	= SendEditor( SCI_GETSELECTIONSTART );
	int end		= SendEditor( SCI_GETSELECTIONEND );
	
	int sLine = SendEditor( SCI_LINEFROMPOSITION, start );
	int eLine = SendEditor( SCI_LINEFROMPOSITION, end );

	vector<int> sPos;

	char buff[512];

	sPos.push_back(0);

	for( int i = sLine; i <= eLine; i++ )
	{
		int	lstart = SendEditor( SCI_GETLINESELSTARTPOSITION, i );
		int length = SendEditor( SCI_GETLINESELENDPOSITION, i ) - lstart + 1;

		if( length <= 0 )
			continue;

		SendEditor( SCI_GETLINE, i, (LPARAM)buff );

		char* res = strstr( buff, "--" );

		if( res != NULL )
		{
			int pos = (int)( res - buff );
			
			bool dirty = false;

			for( int i = 0; i < pos; i++ )
			{
				if( buff[i] != ' ' && buff[i] != '\t' )
				{
					dirty = true;
					break;
				}
			}

			if( !dirty )
				sPos.push_back( lstart + pos );
		}
	}

	int iTextLength = SendEditor( SCI_GETTEXTLENGTH );

	if( sPos.size() <= 1 )
		return;

	for( UINT i = sPos.size() - 1; i >= 1; i-- )
	{
		if( sPos[i] > iTextLength || sPos[i] + 2 > iTextLength )
			continue;

		SendEditor( SCI_SETTARGETSTART, sPos[i] );
		SendEditor( SCI_SETTARGETEND, sPos[i] + 2 );
		SendEditor( SCI_REPLACETARGET, -1, (LPARAM)"" );
	}

}

void CScriptLexer::Disable()
{
	SetAStyle( STYLE_DEFAULT, ligthgray, ligthgray, 10, "Courier New" );
	SendEditor( SCI_STYLECLEARALL );	// Copies global style to all others
}

void CScriptLexer::SetLUALex()
{
	SendEditor( SCI_SETLEXER, SCLEX_LUA );
	SendEditor( SCI_SETLEXERLANGUAGE, 0, (LPARAM)"lua" );
	SendEditor( SCI_SETTABWIDTH, 4 );

	//setup margins
	//line number margin
	SendEditor( SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER );
	SendEditor( SCI_SETMARGINMASKN, 0, STYLE_LINENUMBER );
	SendEditor( SCI_SETMARGINWIDTHN, 0, 32 );

	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"0");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.comment", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.preprocessor", (LPARAM)"1");

	//folding margin
	SendEditor(SCI_SETMARGINWIDTHN, 1, 0);

	SendEditor(SCI_SETMARGINTYPEN,  1, SC_MARGIN_SYMBOL);
	SendEditor(SCI_SETMARGINMASKN, 1, SC_MASK_FOLDERS);
	SendEditor(SCI_SETMARGINWIDTHN, 1, 20);

	DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS, white, blue );
    DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS, white, black );
    DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE, white, black);
    DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER, white, black);
    DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED, white, black);
    DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED, white, black);
    DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER, white, black);

	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER );
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED );
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE );
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER );

	SendEditor(SCI_SETMARGINSENSITIVEN, 1, 1);

	SendEditor( SCI_SETFOLDFLAGS, 16, 0 ); // 16  Draw line below if not expanded


	//setup styling
	SendEditor( SCI_SETSTYLEBITS, 7 );

	SetAStyle( STYLE_DEFAULT, black, white, 10, "Courier New" );
	SendEditor( SCI_STYLECLEARALL );	// Copies global style to all others

	// code default is used for all the document's text
	SetAStyle( m_lxsLuaDefault );

	// keywords
	if( m_strLuaKeyWords != "" )
	{
		SendEditor( SCI_SETKEYWORDS, 0, reinterpret_cast<LPARAM>( m_strLuaKeyWords.data() ) );
		SetAStyle( m_lxsLuaKeyWords );
	}

	if( m_strLuaStdFuncs != "" )
	{
		SendEditor( SCI_SETKEYWORDS, 1, reinterpret_cast<LPARAM>( m_strLuaStdFuncs.data() ) );
		SetAStyle( m_lxsLuaStdFuncs );
	}

	if( m_strLuaMyFuncs != "" )
	{
		SendEditor( SCI_SETKEYWORDS, 2, reinterpret_cast<LPARAM>( m_strLuaMyFuncs.data() ) );
		SetAStyle( m_lxsLuaMyFuncs );
	}

	//comments
	m_lxsLuaComments.style = SCE_LUA_COMMENTLINE;
	SetAStyle( m_lxsLuaComments );
	m_lxsLuaComments.style = SCE_LUA_COMMENT;
	SetAStyle( m_lxsLuaComments );
	m_lxsLuaComments.style = SCE_LUA_COMMENTDOC;
	SetAStyle( m_lxsLuaComments );

	//numbers
	SetAStyle( m_lxsLuaNumbers );

	//strings and chars
	m_lxsLuaStrings.style = SCE_LUA_STRING;
	SetAStyle( m_lxsLuaStrings );
	m_lxsLuaStrings.style = SCE_LUA_STRINGEOL;
	SetAStyle( m_lxsLuaStrings );
	m_lxsLuaStrings.style = SCE_LUA_CHARACTER;
	SetAStyle( m_lxsLuaStrings );

	//operators
	SetAStyle( m_lxsLuaOperators );

	SendEditor( SCI_SETYCARETPOLICY, CARET_EVEN );
	SendEditor( SCI_SETYCARETPOLICY, CARET_JUMPS );
	SendEditor( SCI_SETXCARETPOLICY, CARET_EVEN );
	SendEditor( SCI_SETXCARETPOLICY, CARET_JUMPS );

	//SendEditor( SCI_SETMODEVENTMASK, 0x01 & 0x02 );
}

void CScriptLexer::SetOutputLex()
{
	SendEditor( SCI_SETLEXER, SCLEX_NULL );
	SendEditor( SCI_SETLEXERLANGUAGE, 0, (LPARAM)"" );
	SendEditor( SCI_SETTABWIDTH, 4 );

	//setup margins
	//line number margin
	SendEditor( SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER );
	SendEditor( SCI_SETMARGINMASKN, 0, STYLE_LINENUMBER );
	SendEditor( SCI_SETMARGINWIDTHN, 0, 0 );

	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"0");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.comment", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.preprocessor", (LPARAM)"1");

	//folding margin
	SendEditor(SCI_SETMARGINWIDTHN, 1, 0);

	//SendEditor(SCI_SETMARGINTYPEN,  1, SC_MARGIN_SYMBOL);
	SendEditor(SCI_SETMARGINWIDTHN, 1, 0);


	//setup styling
	SendEditor( SCI_SETSTYLEBITS, 7 );

	SetAStyle( STYLE_DEFAULT, black, white, 8, "Courier New" );
	SendEditor( SCI_STYLECLEARALL );	// Copies global style to all others

	// code default is used for all the document's text
	//SetAStyle( m_lxsLuaDefault );

	// keywords
	SendEditor( SCI_SETKEYWORDS, 0, reinterpret_cast<LPARAM>( "" ) );
	SendEditor( SCI_SETKEYWORDS, 1, reinterpret_cast<LPARAM>( "" ) );
	SendEditor( SCI_SETKEYWORDS, 2, reinterpret_cast<LPARAM>( "" ) );
}

void CScriptLexer::SetNoLex()
{
	SendEditor( SCI_SETLEXER, SCLEX_NULL );
	SendEditor( SCI_SETLEXERLANGUAGE, 0, (LPARAM)"" );
	SendEditor( SCI_SETTABWIDTH, 4 );

	//setup margins
	//line number margin
	SendEditor( SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER );
	SendEditor( SCI_SETMARGINMASKN, 0, STYLE_LINENUMBER );
	SendEditor( SCI_SETMARGINWIDTHN, 0, 32 );

	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"0");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.comment", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.preprocessor", (LPARAM)"1");

	//folding margin
	SendEditor(SCI_SETMARGINWIDTHN, 1, 0);

	//SendEditor(SCI_SETMARGINTYPEN,  1, SC_MARGIN_SYMBOL);
	SendEditor(SCI_SETMARGINWIDTHN, 1, 0);


	//setup styling
	SendEditor( SCI_SETSTYLEBITS, 7 );

	SetAStyle( STYLE_DEFAULT, black, white, 10, "Courier New" );
	SendEditor( SCI_STYLECLEARALL );	// Copies global style to all others

	// code default is used for all the document's text
	SetAStyle( m_lxsLuaDefault );

	// keywords
	SendEditor( SCI_SETKEYWORDS, 0, reinterpret_cast<LPARAM>( "" ) );
	SendEditor( SCI_SETKEYWORDS, 1, reinterpret_cast<LPARAM>( "" ) );
	SendEditor( SCI_SETKEYWORDS, 2, reinterpret_cast<LPARAM>( "" ) );
}

bool CScriptLexer::Load( HWND hParentWnd, str_t strAppPath, str_t strWndName )
{
	if( IsWindow( hParentWnd ) == false || m_bLoadFailed )
		return false;

	m_hEditor = ::CreateWindowEx( WS_EX_CLIENTEDGE, "Scintilla", strWndName.data(), WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_BORDER, 
							  10, 10, 500, 800, hParentWnd, 0, AfxGetInstanceHandle(), 0 );

	m_iEditorID = GetDlgCtrlID( m_hEditor );

	m_strConfigPath = strAppPath + "\\" + m_strConfigPath;

	LoadCfg();

	SetLUALex();

	::ShowWindow( m_hEditor, SW_SHOW );

	return true;
}