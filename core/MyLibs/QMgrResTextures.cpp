#define STRICT

#include "QMgrResTextures.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"
#include "QMath.h"
			 
#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "TexturePreprocessor.h"

#include "IMgrResFiles.h"
#include "IMgrResBinks.h"


QMgrResTextures::QMgrResTextures()
{
	m_bLogEnabled = false;

	PurgeCache();
}

QMgrResTextures::~QMgrResTextures()
{
	ClearNeeds();
	ClearNotNeeded();

	ClearCache();
}

void QMgrResTextures::OnCreateDevice()
{
	if ( m_mTex.size() > 0 )
	{
		for ( texmap_iter i = m_mTex.begin(); i != m_mTex.end(); i++ )
			SetActive( i->second );
	}
}

void QMgrResTextures::OnResetDevice()
{
	if ( m_mTex.size() > 0 )
	{
		for ( texmap_iter i = m_mTex.begin(); i != m_mTex.end(); i++ )
			SetActive( i->second );
	}
}

void QMgrResTextures::OnLostDevice()
{
	if ( m_mTex.size() > 0 )
	{
		for ( texmap_iter i = m_mTex.begin(); i != m_mTex.end(); i++ )
		{
			if ( i->second->Pool == D3DPOOL_DEFAULT )
			{
				SAFE_RELEASE(i->second->tex);
				if ( i->second->tt == TT_VIDEO || i->second->tt == TT_LOADEDSEQ )
				{
					i->second->iNowFrame = -1;
				}
			}
		}
	}

	PurgeCache();
}

void QMgrResTextures::OnDestroyDevice()
{
	if ( m_mTex.size() > 0 )
	{
		for( texmap_iter i = m_mTex.begin(); i != m_mTex.end(); i++ )
		{
			SAFE_RELEASE( i->second->tex );
			if ( i->second->tt == TT_VIDEO || i->second->tt == TT_LOADEDSEQ )
			{
				i->second->iNowFrame = -1;
			}
		}
	}

	PurgeCache();
}

void QMgrResTextures::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"texture mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"texture mgr logging disabled");
	}
}

void QMgrResTextures::SetTex( int tex_num, const std::wstring& tex_id )
{
	if ( m_lNowTexID[tex_num] != tex_id )
	{
		m_lNowTexID[tex_num] = tex_id;
		g_Graph.dev()->SetTexture( tex_num, GetTex(tex_id) );
	}
}

void QMgrResTextures::SetTex( int tex_num, const std::wstring& tex_id, float fFrame )
{
	if ( m_lNowTexID[tex_num] != tex_id )
	{
		m_lNowTexID[tex_num] = tex_id;
		g_Graph.dev()->SetTexture( tex_num, GetTex(tex_id, fFrame) );
	}
}

void QMgrResTextures::SetTexNull( int tex_num )
{
	if ( m_lNowTexID[tex_num] != L"" )
	{
		m_lNowTexID[tex_num] = L"";
		g_Graph.dev()->SetTexture( tex_num, NULL ); 
	}
}

LPDIRECT3DTEXTURE9 QMgrResTextures::GetTex( const std::wstring& tex_id )
{
	QTextureListItem* temp = GetTexItem( tex_id );

	if ( temp )
	{
		return GetTex( tex_id, (float)( ( temp->iNowFrame==-1 ) ? 0 : temp->iNowFrame ) );
	}
	else
	{
		return NULL;
	}
}

LPDIRECT3DTEXTURE9 QMgrResTextures::GetTex( const std::wstring& tex_id, float fFrame )
{
	QTextureListItem* temp = GetTexItem( tex_id );

	if ( temp != NULL )
	{
		if ( temp->tt == TT_LOADED || temp->tt == TT_CREATED )
		{
			return temp->tex;
		}
		else
		{
			if ( temp->tex )
			{
				int iFrame = (int)fFrame;
				if ( temp->iNowFrame != iFrame )
				{
					if ( g_BinkMgr.DecompressFrame( tex_id, temp, iFrame ) )
					{
						temp->iNowFrame = iFrame;
					}
				}
				return temp->tex;
			}
		}
	}
	else
	{
		if ( tex_id != L"sys\\default" )
		{
			temp = GetTexItem( L"sys\\default" );
			if ( temp != NULL )
			{
				return temp->tex;
			}
		}
	}
    
	return NULL;
}

QTextureListItem* QMgrResTextures::GetTexItem( const std::wstring& tex_id )
{
	if ( m_mTex.size() > 0 )
	{
		texmap_iter i = m_mTex.find(tex_id);;

		if ( i != m_mTex.end() )
		{
			return i->second;
		}
	}
    
	return NULL;
}

DWORD QMgrResTextures::GetRimFormatByDX( D3DFORMAT fmt )
{
	switch(fmt)
	{
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			return TP_A8R8G8B8;
			break;

		case D3DFMT_A4R4G4B4:
			return TP_A4R4G4B4;
			break;

		case D3DFMT_A1R5G5B5:
			return TP_A1R5G5B5;
			break;

		case D3DFMT_R5G6B5:
			return TP_R5G6B5;
			break;

		case D3DFMT_R8G8B8:
			return TP_R8G8B8;
			break;

		case D3DFMT_DXT1:
		case D3DFMT_DXT3:
		case D3DFMT_DXT5:
			return TP_ORIGINAL_FORMAT;
			break;
	}
	return 0;
}

DWORD QMgrResTextures::GetRimSizeFormatByRes( UINT Width )
{
	switch(Width)
	{
		case 1024:
			return TP_1024_768;
			break;

		case 800:
			return TP_800_600;
			break;

		case 640:
			return TP_640_480;
			break;
	}
	return 0;
}


void QMgrResTextures::LoadTex2( const std::wstring& parent_id, const std::wstring& tex_id )
{
	//устанавливаем 0 в кол-во мип уровней для автоматической генерации цепочки мипмапов
	LoadTex4( parent_id, tex_id, 0, true, -1, 0, 0, 0, 0 ); 
}

void QMgrResTextures::LoadTex3( const std::wstring& parent_id, const std::wstring& tex_id, UINT iMipLevels, bool bActive )
{
	LoadTex4( parent_id, tex_id, iMipLevels, bActive, 1, 0, 0, 0, 0 );
}

void QMgrResTextures::LoadTex4( const std::wstring& parent_id, const std::wstring& tex_id, UINT iMipLevels, bool bActive, int AlphaLevel, float Left, float Top, UINT RealWidth, UINT RealHeight )
{
	if ( tex_id == L"" )
	{
		g_Log.WriteError( L"tex_id ==\"\" in \"void QMgrResTextures::LoadTex4( std::wstring parent_id, std::wstring tex_id, UINT iMipLevels, bool bActive, int AlphaLevel, float Left, float Top, UINT RealWidth, UINT RealHeight )\"" );
		return;
	}

	QTextureListItem* temp = GetTexItem( tex_id );
	if ( temp != NULL )
	{
		temp->needs = true;
		return;
	}

	QTextureListItem* t		= new QTextureListItem();
	t->Active				= bActive;
	t->parent_id			= parent_id;
	t->tex_id				= tex_id;
	t->tt					= TT_LOADED;
	t->MipLevels			= iMipLevels;
	t->Left					= Left;
	t->Top					= Top;
	t->RealWidth			= RealWidth;
	t->RealHeight			= RealHeight;
	t->AlphaLevel			= AlphaLevel;
	t->Usage				= 0;
	t->Pool					= D3DPOOL_MANAGED;
	t->needs				= true;
	t->tex					= NULL;
	
	t->fFrameRate			= 0;
	t->iFramesCount			= 1;
	t->iNowFrame			= -1;

	t->pFileBuffer			= NULL;
	t->iFileBufferSize		= 0;

	if ( LoadTex5( t ) )
	{
		m_mTex[tex_id] = t;
		m_mmTex.insert( mmapWStrWStr::value_type( parent_id, tex_id ) );

		if ( m_bLogEnabled )
		{
			g_Log.WriteWarning( L"Texture loaded, parent_id = \"" + parent_id + L"\", tex_id = \"" + tex_id + L"\", format: \"" + g_Graph.GetFormatStr(t->Format) + L"\": bits=\"" + int2wstr(g_Graph.GetTextureBits(t->Format)) + L"\", AlphaLevel=\"" + int2wstr(t->AlphaLevel) + L"\", Usage=\"" + int2wstr(t->Usage) + L"\", MipLevels = \"" + int2wstr(t->MipLevels) + L"\"" );
		}
	}
	else
	{
		SAFE_DELETE(t);
		g_Log.WriteError( L"Texture load error, parent_id = \"" + parent_id + L"\", tex_id = \"" + tex_id + L"\"" );
	}
}

bool QMgrResTextures::LoadTex5( QTextureListItem* t )
{
	if (!t)
		return false;

	g_Graph.RenderLoad( L"Load texture: " + t->tex_id );

	bool m_bUseSelf = false;

	std::wstring pic_file = GetTexFileName( t->tex_id );
	
	if ( t->pFileBuffer == NULL )
	{
		if ( !g_FileMgr.LoadFile( pic_file ) )
			return false;

		UINT tempbuffersize = 0;
		UCHAR* tempbuffer = g_FileMgr.GetFileBuffer( pic_file, &tempbuffersize );

		D3DXIMAGE_INFO m_stImageInfo;
	
		//Читаем информацию из файла
		if ( FAILED( D3DXGetImageInfoFromFileInMemory( tempbuffer, tempbuffersize, &m_stImageInfo ) ) )
		{
			g_Log.WriteError( L"Failed to read tex info, tex_id = \"" + t->tex_id + L"\"" );
			g_FileMgr.FreeFile( pic_file, true );
			return false;
		}

		if ( t->MipLevels != 0 )
		{
			m_bUseSelf		= g_Graph.vp.Width==1024;
			if ( g_Graph.GetUseMip1ForBacks() == true )
			{
				t->MipLevels = 1;
			}
			else
			{
				if ( t->MipLevels == -1 )
				{
					if( m_stImageInfo.Width == 1024 )
						t->MipLevels = 1;
					else
						t->MipLevels = 2;
				}
			}
		}
		else
		{
			m_bUseSelf		= g_Graph.GetTextureQuality() == 1.0f;
		}

		SSupportedTextures* st = NULL;
		if ( t->AlphaLevel!=-1 )
		{
			st = g_Graph.GetCompTex( m_stImageInfo.Format, t->AlphaLevel, t->Usage, false, m_bUseSelf );
		}
		else
		{
			st = g_Graph.GetCompTex( m_stImageInfo.Format, t->Usage, false, m_bUseSelf );
			if(st)
				t->AlphaLevel = st->AlphaLevel;
		}

		if ( !st )
		{
			g_FileMgr.FreeFile( pic_file, true );
			return false;
		}

		t->Format	= st->Format;

		//Если МипУровней==1 то это фон, и его необходимо смаштабировать с резолюцией
		if ( t->MipLevels!=0 )
		{
			t->ImgWidth		= (int) ( (float)m_stImageInfo.Width * (g_Graph.fWidth/1024) );
			t->ImgHeight	= (int) ( (float)m_stImageInfo.Height * (g_Graph.fHeight/768) );
			t->RealWidth	= (int) ( (float)t->RealWidth * (g_Graph.fWidth/1024) );
			t->RealHeight	= (int) ( (float)t->RealHeight * (g_Graph.fHeight/768) );
			t->Left			= t->Left * (g_Graph.fWidth/1024);
			t->Top			= t->Top * (g_Graph.fHeight/768);

			if ( GetRimFormatByDX(t->Format)!=0 && g_Graph.GetUseRimTextureLib() )
			{
				DWORD dwFlags = TP_DDS_FILE | TP_SQUARED | 
								GetRimSizeFormatByRes(g_Graph.vp.Width) | GetRimFormatByDX(t->Format) |
								TP_ALPHA_MIDDLE;
				TexturePreprocessor tp;
				WORD result = tp.ConvertTexture( tempbuffer,
												dwFlags,
												t->pFileBuffer,
												t->iFileBufferSize,
												t->TexWidth, t->TexHeight );
				if ( result == 0 )
				{
					if ( m_bLogEnabled )
					{
						g_Log.WriteWarning( L"Texture parent_id = \"" + t->parent_id + L"\", tex_id = \"" + t->tex_id + L"\", was converted by rim lib, format: \"" + st->FormatStr + L"\": bits=\"" + int2wstr(st->TextureBits) + L"\", AlphaLevel=\"" + int2wstr(t->AlphaLevel) + L"\", Usage=\"" + int2wstr(st->Usage) + L"\"" );
					}
				}
				else
				{
					g_Log.WriteError( L"Texture parent_id = \"" + t->parent_id + L"\", tex_id = \"" + t->tex_id + L"\", cant be converted by rim lib, error = \"" + int2wstr(result) + L"\", format: \"" + st->FormatStr + L"\": bits=\"" + int2wstr(st->TextureBits) + L"\", AlphaLevel=\"" + int2wstr(t->AlphaLevel) + L"\", Usage=\"" + int2wstr(st->Usage) + L"\"" );
				}
				//g_FileMgr.SaveFile( L"d:\\1\\" + t->tex_id + L".dds", t->pFileBuffer, t->iFileBufferSize );
				g_FileMgr.FreeFile( pic_file, true );
			}
			else
			{
				t->pFileBuffer = tempbuffer;
				t->iFileBufferSize = tempbuffersize;
				g_FileMgr.FreeFile( pic_file, false );
			}
			
		}
		else
		{
			//Иначе оригинальный размер домноженный на коэфициент качества текстур
			t->ImgWidth		= GetHigherPow2((int) ( g_Graph.GetTextureQuality() * m_stImageInfo.Width ));
			t->ImgHeight	= GetHigherPow2((int) ( g_Graph.GetTextureQuality() * m_stImageInfo.Height ));
			t->ImgWidth		= (t->ImgWidth>0) ? t->ImgWidth : 1;
			t->ImgHeight	= (t->ImgHeight>0) ? t->ImgHeight : 1;
			if ( !IsNumThePow2( t->ImgWidth ) || !IsNumThePow2( t->ImgHeight ) )
			{
				g_Log.WriteError( L"error, textures with miplevels > 1 must be the power2, tex_id=\"" + t->tex_id + L"\"" );
			}

			t->pFileBuffer = tempbuffer;
			t->iFileBufferSize = tempbuffersize;
			g_FileMgr.FreeFile( pic_file, false );
		}
	}

	//Получаем размер текстуры по степени двойки
	t->TexWidth		= GetHigherPow2(t->ImgWidth);
	t->TexHeight	= GetHigherPow2(t->ImgHeight);
	t->fWidthAspect	= (float)t->ImgWidth / t->TexWidth;
	t->fHeightAspect= (float)t->ImgHeight / t->TexHeight;

	if ( t->RealWidth==0 || t->RealHeight==0 )
	{
		t->RealWidth	= t->ImgWidth;
		t->RealHeight	= t->ImgHeight;
	}

	SetActive( t );

	return true;
}

void QMgrResTextures::LoadVideo( const std::wstring& parent_id, const std::wstring& tex_id, bool bActive, int AlphaLevel, bool bPrechache  )
{
	LoadVideo2( parent_id, tex_id, bActive, AlphaLevel, false, 0.0f, 0.0f );
}

void QMgrResTextures::LoadVideo2( const std::wstring& parent_id, const std::wstring& tex_id, bool bActive, int AlphaLevel, bool bPrechache, float Left, float Top )
{
	g_Graph.RenderLoad( L"Load video: " + tex_id );

	QBinkListItem* m_pBink = g_BinkMgr.LoadVideo( tex_id, bPrechache  );

	if(!m_pBink)
		return;

	if ( !m_pBink->HaveAlpha && AlphaLevel>0 )
	{
		AlphaLevel = 0;
	}

	SSupportedTextures* st = g_Graph.GetCompTex( g_Graph.GetTextureBits(), AlphaLevel, D3DUSAGE_DYNAMIC, true );

	if ( !st )
	{
		g_Log.WriteError( L"cant get compatable texture format, for request: AlphaLevel=\"" + int2wstr(AlphaLevel) + L"\"" );
		g_BinkMgr.FreeVideo( tex_id );
		return;
	}

	m_pBink->format			= st->Format;
	m_pBink->pixel_size		= st->PixelSize;
	m_pBink->bink_surface_type	= st->BinkSurface;

	if ( CreateTexture3( parent_id, tex_id, m_pBink->width, m_pBink->height, st->Usage, st->Format, bActive, true ) )
	{
		QTextureListItem* t = GetTexItem( tex_id );

		if ( t )
		{
			t->Left			= Left;
			t->Top			= Top;
			t->tt			= TT_VIDEO;
			t->fFrameRate	= m_pBink->fFrameRate;
			t->iFramesCount	= m_pBink->iFramesCount;
		}
	}
	else
	{
		g_Log.WriteError( L"cant create texture in \"void QMgrResTextures::LoadVideo( std::wstring parent_id, std::wstring tex_id )\"" );
		g_BinkMgr.FreeVideo( tex_id );
	}
}

void QMgrResTextures::ClearNeeds()
{
	ClearNeeds(L"");
}

void QMgrResTextures::ClearNeeds( const std::wstring& parent_id )
{
	if ( m_mTex.size() > 0 )
	{
		for( texmap_iter i = m_mTex.begin(); i != m_mTex.end(); i++ )
		{
			if ( i->second->parent_id==parent_id || parent_id==L"")
				i->second->needs = false;
		}
	}
}

void QMgrResTextures::ClearNotNeeded()
{
	if ( m_mTex.size() > 0 )
	{
		for( texmap_iter i = m_mTex.begin(); i != m_mTex.end(); )
		{
			if ( i->second->needs == false )
			{
				i = FreeTex2( i->first );
			}
			else
			{
				i++;
			}
		}
	}
}

bool QMgrResTextures::ReadTex( const std::wstring& tex_id, D3DCOLOR** p )
{
	QTextureListItem* temp = GetTexItem( tex_id );
	
	if ( temp != NULL )
	{
		return ReadTex( temp->tex, temp->ImgWidth, temp->ImgHeight, temp->Usage, p );
	}
	else
	{
		return false;
	}
}

bool QMgrResTextures::ReadTex( LPDIRECT3DTEXTURE9 tex, UINT Width, UINT Height, DWORD Usage, D3DCOLOR** p )
{
	DWORD Flags = D3DLOCK_READONLY;

	D3DLOCKED_RECT rect;
	if (tex->LockRect(0, &rect, NULL, Flags) != D3D_OK )
	{
		g_Log.WriteError( L"Cant lock texture in bool QMgrResTextures::ReadTex( LPDIRECT3DTEXTURE9 tex, int Width, int Height, DWORD Usage, D3DCOLOR** p )" );
		return false;
	}

	D3DCOLOR* temp = (D3DCOLOR*)rect.pBits;
	D3DCOLOR* c = *p;

	c = new D3DCOLOR[ Width * Height ];
	int pitch_size = rect.Pitch >> 2;

	for( UINT j = 0; j < Height; j++)
	{
		for( UINT i = 0; i < Width; i++)
		{
			c[ j * Width + i ] = temp[ j * pitch_size + i ];
		}
	}

	tex->UnlockRect(0);

	return true;
}


bool QMgrResTextures::FillTex( const std::wstring& tex_id, D3DCOLOR* p )
{
	QTextureListItem* temp = GetTexItem( tex_id );
	
	if( temp != NULL )
	{
		return FillTex( temp->tex, temp->ImgWidth, temp->ImgHeight, temp->Usage, p );
	}
	else
	{
		return false;
	}
}

bool QMgrResTextures::FillTex( LPDIRECT3DTEXTURE9 tex, UINT Width, UINT Height, DWORD Usage, D3DCOLOR* p )
{
	if ( tex==NULL )
		return false;

	DWORD Flags = 0;

	switch( Usage )
	{
		case D3DUSAGE_DYNAMIC:
			Flags = D3DLOCK_DISCARD;
			break;

		case 0:
			Flags = 0;
			break;

		default:
			return false;
			break;
	}

	D3DLOCKED_RECT rect;
	if ( FAILED ( tex->LockRect(0, &rect, NULL, Flags) ) )
	{
		g_Log.WriteError( L"Cant lock texture in \"bool QMgrResTextures::FillTex( LPDIRECT3DTEXTURE9 tex, int Width, int Height, DWORD Usage, D3DCOLOR* p )\"" );
		return false;
	}

	D3DCOLOR* temp =  (D3DCOLOR*)rect.pBits;
	int pitch_size = rect.Pitch >> 2;

	UINT tex_ofs = 0, p_ofs = 0;
	for( UINT j = 0; j < Height; j++ )
	{
		tex_ofs = j * pitch_size;
		p_ofs = j * Width;
		for( UINT i = 0; i < Width; i++ )
		{
			temp[ tex_ofs + i ] = ( p != NULL ? p[ p_ofs + i ] : 0 );
		}
	}

	tex->UnlockRect(0);

	return true;
}

bool QMgrResTextures::CreateTexture( const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage )
{
	return CreateTexture2( tex_id, Width, Height, Usage, D3DFMT_A8R8G8B8 );
}

bool QMgrResTextures::CreateTexture2( const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage, D3DFORMAT Format )
{
	return CreateTexture3( L"sys", tex_id, Width, Height, Usage, Format, true, false );
}

bool QMgrResTextures::CreateTexture3( const std::wstring& parent_id, const std::wstring& tex_id, UINT Width, UINT Height, DWORD Usage, D3DFORMAT Format, bool bActive, bool bForcePow2 )
{
	QTextureListItem* temp_tex = GetTexItem( tex_id );
	if ( temp_tex != NULL )
	{
		temp_tex->needs = true;
		return true;
	}

	SSupportedTextures* st = g_Graph.GetCompTex( Format, Usage, false, true );

	if ( !st )
		return false;

	UINT TexWidth = Width;
	UINT TexHeight = Height;
	
	if ( g_Graph.IsDeviceSupportNonPow2() == false || bForcePow2 )
	{
		TexWidth = GetHigherPow2( Width );
		TexHeight = GetHigherPow2( Height );
	}

	QTextureListItem* t		= new QTextureListItem();
	t->Active				= bActive;
	t->parent_id			= parent_id;
	t->tex_id				= tex_id;
	t->tt					= TT_CREATED;
	t->ImgWidth				= Width;
	t->ImgHeight			= Height;
	t->TexWidth				= TexWidth;
	t->TexHeight			= TexHeight;
	t->fWidthAspect			= (float)Width / TexWidth;
	t->fHeightAspect		= (float)Height / TexHeight;
	t->RealWidth			= Width;
	t->RealHeight			= Height;
	t->MipLevels			= 1;
	t->Usage				= st->Usage;
	t->Format				= st->Format;
	t->Pool					= st->Pool;
	t->AlphaLevel			= st->AlphaLevel;
	t->tex					= NULL;
	t->needs				= true;

	t->fFrameRate			= 0;
	t->iFramesCount			= 1;
	t->iNowFrame			= -1;

	t->pFileBuffer			= NULL;
	t->iFileBufferSize		= 0;

	m_mTex[tex_id] = t;
	m_mmTex.insert( mmapWStrWStr::value_type( parent_id, tex_id ) );

	if(m_bLogEnabled)
	{
		g_Log.WriteWarning( L"Texture created, parent_id = \"" + parent_id + L"\", tex_id = \"" + tex_id + L"\", format: \"" + g_Graph.GetFormatStr(t->Format) + L"\": bits=\"" + int2wstr(g_Graph.GetTextureBits(t->Format)) + L"\", AlphaLevel=\"" + int2wstr(t->AlphaLevel) + L"\", Usage=\"" + int2wstr(t->Usage) + L"\", MipLevels = \"" + int2wstr(t->MipLevels) + L"\"" );
	}

	SetActive( t );

	return true;
}

void QMgrResTextures::PurgeCache()
{
	m_lNowTexID.clear();
	m_lNowTexID.reserve(8);
	for( int i = 0; i < 8; i++)
	{
		m_lNowTexID.push_back( L"-1" );
	}
}

void QMgrResTextures::FreeTex( const std::wstring tex_id )
{
	FreeTex2( tex_id );
}

texmap_iter QMgrResTextures::FreeTex2( const std::wstring tex_id )
{
	QTextureListItem* temp = GetTexItem(tex_id);
	std::wstring parent_id = (temp!=NULL) ? temp->parent_id : L"";
	
	texmap_iter next_i = FreeTex3(tex_id);

	if ( m_mmTex.size() > 0 && parent_id !=L"" )
	{
		mmapWStrWStr_iter beg = m_mmTex.lower_bound( parent_id );
		mmapWStrWStr_iter end = m_mmTex.upper_bound( parent_id );

		for( mmapWStrWStr_iter i = beg; i != end; i++ )
		{
			if ( i->second == tex_id )
			{
				m_mmTex.erase( i );
				return next_i;
			}
		}
	}
	return next_i;
}

texmap_iter QMgrResTextures::FreeTex3( const std::wstring tex_id )
{
	texmap_iter i = m_mTex.find( tex_id );
	
	if( i != m_mTex.end() )
	{
		QTextureListItem* temp = i->second;

		if ( temp != NULL )
		{
			SAFE_DELETE_ARRAY( temp->pFileBuffer );
			SAFE_RELEASE( temp->tex );
			if ( temp->tt == TT_VIDEO )
			{
				g_BinkMgr.FreeVideo( tex_id );
			}
			if(m_bLogEnabled)
			{
				g_Log.WriteWarning( L"Texture free, parent_id = \"" + temp->parent_id + L"\", tex_id = \"" + tex_id + L"\"" );
			}
			SAFE_DELETE( temp );

			return m_mTex.erase(i);
		}
	}
	return m_mTex.end();
}

void QMgrResTextures::FreeTexs( const std::wstring parent_id )
{
	if ( m_mmTex.size() > 0 )
	{
		mmapWStrWStr_iter beg = m_mmTex.lower_bound( parent_id );
		mmapWStrWStr_iter end = m_mmTex.upper_bound( parent_id );

		for( mmapWStrWStr_iter i = beg; i != end; i++ )
		{
			FreeTex3( i->second );
		}
		m_mmTex.erase( beg, end );
	}	
}

void QMgrResTextures::SetActive( const std::wstring& tex_id, bool bActive )
{
	QTextureListItem* temp = GetTexItem( tex_id );

	if ( temp )
	{
		temp->Active = bActive;
		SetActive( temp );
	}
}

void QMgrResTextures::SetActive( QTextureListItem* t )
{
	if ( !t )
		return;

	if ( t->Active )
	{
		if ( t->tt == TT_LOADED )
		{
			if ( t->tex == NULL )
			{
				t->Active = SUCCEEDED(	D3DXCreateTextureFromFileInMemoryEx( g_Graph.dev(), 
															t->pFileBuffer, t->iFileBufferSize,
															t->TexWidth, t->TexHeight, t->MipLevels,
															t->Usage, t->Format, t->Pool,
															(( t->MipLevels != 0 ) ? D3DX_FILTER_NONE : D3DX_DEFAULT),
															D3DX_DEFAULT,
															0, NULL, NULL, &t->tex ) );
				if ( !t->Active )
				{
					g_Log.WriteError( L"Texture load error in \"void QMgrResTextures::SetActive( QTextureListItem* t )\", tex_id = \"" + t->tex_id + L"\"" );
				}
			}
			else
			{
				t->Active = true;
			}
		}
		else if ( t->tt == TT_CREATED || t->tt == TT_VIDEO || t->tt == TT_LOADEDSEQ )
		{
			t->Active = SUCCEEDED( D3DXCreateTexture( g_Graph.dev(), t->TexWidth, t->TexHeight, 1, t->Usage, t->Format, t->Pool, &t->tex ) );

			if ( t->Active )
			{
				FillTex( t->tex, t->TexWidth, t->TexHeight, t->Usage, NULL );
			}
			else
			{
				g_Log.WriteError( L"Texture create error in \"void QMgrResTextures::SetActive( QTextureListItem* t )\", tex_id = \", tex_id = \"" + t->tex_id + L"\"" );
			}
		}
	}
	else
	{
		SAFE_RELEASE(t->tex);
		t->iNowFrame = -1;
		t->Active = false;
	}
}

void QMgrResTextures::SetActives( const std::wstring& parent_id, bool bActive )
{
	if ( m_mmTex.size() > 0 )
	{
		mmapWStrWStr_iter beg = m_mmTex.lower_bound( parent_id );
		mmapWStrWStr_iter end = m_mmTex.upper_bound( parent_id );

		for( mmapWStrWStr_iter i = beg; i != end; ++i)
		{
			SetActive( i->second, bActive );
		}
	}
}
