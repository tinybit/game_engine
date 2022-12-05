#define STRICT

#include "QMgrResBinks.h"

#include "IMgrResTextures.h"

QMgrResBinks::QMgrResBinks()
{
	m_pDirectSound = NULL;
	BinkSoundUseDirectSound( m_pDirectSound );
}

QMgrResBinks::~QMgrResBinks()
{
	SAFE_RELEASE(m_pDirectSound);

	while( m_mBinks.size() > 0  )
	{
		FreeVideo( m_mBinks.begin()->first );
	}
	
}

void QMgrResBinks::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"bink mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"bink mgr logging disabled");
	}
}

QBinkListItem* QMgrResBinks::LoadVideo( const std::wstring& bink_id, bool bPrechache  )
{
	QBinkListItem* m_pBink = new QBinkListItem;
	ZeroMemory( m_pBink, sizeof(QBinkListItem) );
	std::string m_sFileName = wstr2str( GetFileByID( bink_id ) );

	m_pBink->pBink = BinkOpen( m_sFileName.c_str(), BINKALPHA | ( bPrechache ? BINKPRELOADALL : 0 ) );

	if ( !m_pBink->pBink )
	{
		g_Log.WriteError( L"error on load bink, bink_id = \"" + bink_id + L"\"" );
		return NULL;
	}

	BinkSetSoundOnOff( m_pBink->pBink, 1 );

	m_pBink->width			= m_pBink->pBink->Width;
	m_pBink->height			= m_pBink->pBink->Height;
	m_pBink->HaveAlpha		= (m_pBink->pBink->OpenFlags & BINKALPHA) != 0;
	m_pBink->iFramesCount	= m_pBink->pBink->Frames;
	m_pBink->fFrameRate		= (float)m_pBink->pBink->FrameRate;
	m_pBink->iNowFrame		= -1;

	m_mBinks[bink_id] = m_pBink;

	mapWStrQBinkListItem_iter i = m_mBinks.find(bink_id);

	if ( i != m_mBinks.end() )
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}

void QMgrResBinks::FreeVideo( const std::wstring bink_id )
{
	if ( m_mBinks.size() > 0 )
	{
		mapWStrQBinkListItem_iter i = m_mBinks.find( bink_id );

		if ( i != m_mBinks.end() )
		{
			BinkClose( i->second->pBink );
			m_mBinks.erase(bink_id);
		}
	}
}

bool QMgrResBinks::DecompressFrame( const std::wstring& bink_id, void* pTexItem, int iFrame )
{
	if ( m_mBinks.size() > 0 )
	{
		mapWStrQBinkListItem_iter i = m_mBinks.find( bink_id );

		if ( i != m_mBinks.end() )
		{
			QBinkListItem* m_pBink = i->second;
			QTextureListItem* temp = (QTextureListItem*)pTexItem;

			void* buffer;
			U32 pixel_pitch;
			U32 bink_surf_type;
			U32 src_x, src_y, src_w, src_h;

			// Decompress the Bink frame.
			if ( m_pBink->iNowFrame!=-1 && m_pBink->iNowFrame==iFrame-1 )
			{
				BinkNextFrame( m_pBink->pBink );
			}
			else
			{
				BinkGoto( m_pBink->pBink, iFrame, NULL );
			}
			BinkDoFrame( m_pBink->pBink );
			m_pBink->iNowFrame = iFrame;

			// Lock the 3D image so that we can copy the decompressed frame into it.
			if ( LockTexture( temp, m_pBink, &buffer, &pixel_pitch, &bink_surf_type,
										&src_x, &src_y, &src_w, &src_h ) )
			{

				// Copy the decompressed frame into the 3D image.
				BinkCopyToBufferRect( m_pBink->pBink, buffer, pixel_pitch, m_pBink->pBink->Height, 0, 0, src_x, src_y, src_w, src_h, bink_surf_type | BINKCOPYALL );

				// Unlock the 3D image.
				if( FAILED( temp->tex->UnlockRect( 0 ) ) ) return false;

				return true;
			}

		}
	}

	return false;
}

std::wstring QMgrResBinks::GetFileByID( const std::wstring& bink_id )
{
	return L"data\\Video\\" + bink_id;
}

//-------------------------------------------------------------------

bool QMgrResBinks::LockTexture( void* pTexItem, QBinkListItem* BinkListItem, void* out_pixel_buffer, u32* out_buffer_pitch, u32* out_surface_type, u32* src_x, u32* src_y, u32* src_w, u32* src_h )
{
	QTextureListItem* temp = (QTextureListItem*)pTexItem;
	LPDIRECT3DTEXTURE9 tex = temp->tex;
	if ( out_pixel_buffer || out_buffer_pitch )
	{
		D3DLOCKED_RECT locked_rect;

		DWORD Flags = 0;
		if ( temp->Usage == D3DUSAGE_DYNAMIC && g_Graph.IsDeviceSupportDynamictextures() )
		{
			Flags = D3DLOCK_DISCARD;
		}

		if ( FAILED( tex->LockRect( 0, &locked_rect, NULL, Flags ) ) )
		{
			return false;
		}

		if ( out_pixel_buffer )
		{
			*( void** )out_pixel_buffer = locked_rect.pBits;
		}

		if ( out_buffer_pitch )
		{
			*out_buffer_pitch = locked_rect.Pitch;
		}

		if ( src_x )
		{
			*src_x = 0;
		}

		if ( src_y )
		{
			*src_y = 0;
		}

		if ( src_w )
		{
			*src_w = BinkListItem->width;
		}

		if ( src_h )
		{
			*src_h = BinkListItem->height;
		}
	}

	if ( out_surface_type )
	{
		*out_surface_type = BinkListItem->bink_surface_type;
	}

	return true;
}