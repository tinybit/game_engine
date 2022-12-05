#define STRICT

#include "QMgrResSounds.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

QMgrResSounds::QMgrResSounds()
{
	m_bLogEnabled	= false;

	m_bSoundInited	= false;
}

QMgrResSounds::~QMgrResSounds()
{
	if(!m_bSoundInited)
		return;

	FSOUND_StopSound( FSOUND_ALL );
	while ( m_mSnd.size()>0 )
	{
		FreeSnd( m_mSnd.begin()->first ); 
	}

	FSOUND_Close();
}

bool QMgrResSounds::Init( int mixrate, int maxsoftwarechannels )
{
	m_bSoundInited = FSOUND_Init(mixrate, maxsoftwarechannels, 0);
		
	if(m_bSoundInited)
	{
		g_Log.WriteMessage(L"Sound system initialized with params: smpl rate = " + int2wstr(mixrate) + L", channels = " + int2wstr(maxsoftwarechannels) );
	}
	else
	{
		g_Log.WriteError(L"Cant init sound system with params: smpl rate = " + int2wstr(mixrate) + L", channels = " + int2wstr(maxsoftwarechannels) );
	}

	// Create IDirectSound using the primary sound device
	//if ( !m_pDirectSound )
	//{
	//	if( SUCCEEDED( DirectSoundCreate8( NULL, &m_pDirectSound, NULL ) ) )
	//	{
	//		// Set DirectSound coop level
	//		if( FAILED( m_pDirectSound->SetCooperativeLevel( DXUTGetHWND(), DSSCL_NORMAL ) ) )
	//		{
	//			g_Log.WriteError( L"cant start DirectSound interface" );
	//		}
	//	}
	//}

	return m_bSoundInited;
}

void QMgrResSounds::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"sounds mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"sounds mgr logging disabled");
	}
}

void QMgrResSounds::LoadSmpl( const std::wstring& snd_id )
{
	LoadSnd( snd_id, 0 );
}

void QMgrResSounds::LoadStrm( const std::wstring& snd_id )
{
	LoadSnd( snd_id, 1 );
}

void QMgrResSounds::LoadSnd( const std::wstring& snd_id, UINT iType )
{
	if(!m_bSoundInited)
		return;

	if ( m_mSnd.size()>0 )
	{
		sndmap_iter i = m_mSnd.find(snd_id);

		if ( i != m_mSnd.end() )
		{
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning(L"sound already loaded, snd_id = " + snd_id + L", type = \"" + ( i->second.iType==0 ? L"Sample" : L"Stream" ) + L"\"" );
			}
			return;
		}
	}

	g_Graph.RenderLoad( L"Load sound: " + snd_id );

	std::string m_sTemp = "data\\Sounds\\" + wstr2str(snd_id);

	void* temp = NULL;
	if ( iType == 0 )
	{
		temp = FSOUND_Sample_Load(FSOUND_FREE, m_sTemp.c_str(), FSOUND_NORMAL, 0);
	}
	else
	{
		temp = FSOUND_Stream_OpenFile(m_sTemp.c_str(), FSOUND_NORMAL, 0);
	}
	if ( temp != NULL )
	{
		SSoundListItem SoundListItem;
		SoundListItem.snd_id	= snd_id;
		SoundListItem.iType		= iType;
		SoundListItem.iLengthMs	= ( iType == 0 ? 0 : FSOUND_Stream_GetLengthMs( (FSOUND_STREAM*)temp ) );
		SoundListItem.iLength	= ( iType == 0 ? FSOUND_Sample_GetLength( (FSOUND_SAMPLE*)temp ) : FSOUND_Stream_GetLength( (FSOUND_STREAM*)temp ) );
		SoundListItem.pSnd		= temp;
		m_mSnd[snd_id] = SoundListItem;

		if ( m_bLogEnabled )
		{
			g_Log.WriteWarning(L"sound loaded, snd_id = " + snd_id );
		}
	}
	else
	{
		g_Log.WriteError( L"failed to load sound, snd_id = \"" + snd_id + L"\", type = \"" + ( iType==0 ? L"Sample" : L"Stream" ) + L"\"" );
	}
}

void QMgrResSounds::FreeSnd( const std::wstring snd_id )
{
	if(!m_bSoundInited)
		return;

	if ( m_mSnd.size()>0 )
	{
		sndmap_iter i = m_mSnd.find(snd_id);

		if ( i != m_mSnd.end() )
		{
			if ( i->second.iType == 0 )
			{
				FSOUND_Sample_Free( (FSOUND_SAMPLE*)i->second.pSnd );
			}
			else
			{
				FSOUND_Stream_Stop( (FSOUND_STREAM*)i->second.pSnd );
				FSOUND_Stream_Close( (FSOUND_STREAM*)i->second.pSnd );
			}
			
			m_mSnd.erase(snd_id);
			
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning(L"sound free, snd_id = " + snd_id );
			}
		}
	}

}
void QMgrResSounds::PlaySnd( const std::wstring& snd_id )
{
	PlaySnd2( snd_id, false, 1.0f );
}

void QMgrResSounds::PlaySnd2( const std::wstring& snd_id, bool bLooped, float fVolume )
{
	if(!m_bSoundInited)
		return;

	if ( m_mSnd.size()>0 )
	{
		sndmap_iter i = m_mSnd.find(snd_id);

		if ( i != m_mSnd.end() )
		{
			int m_iChanel = -1;

			if ( i->second.iType == 0 )
			{
				m_iChanel = FSOUND_PlaySound(FSOUND_FREE, (FSOUND_SAMPLE*)i->second.pSnd);
			}
			else
			{
				FSOUND_Stream_Stop( (FSOUND_STREAM*)i->second.pSnd );
				m_iChanel = FSOUND_Stream_Play(FSOUND_FREE, (FSOUND_STREAM*)i->second.pSnd);
			}

			if(m_iChanel!=-1)
			{
				FSOUND_SetVolume(m_iChanel, (int)(fVolume*255));
			}
			else
			{
				g_Log.WriteError( L"failed to play stream sound, snd_id = \"" + snd_id + L"\"" );
			}
			return;
		}
	}
}

void QMgrResSounds::StopSnd( const std::wstring& snd_id )
{
	if(!m_bSoundInited)
		return;

	if ( m_mSnd.size()>0 )
	{
		sndmap_iter i = m_mSnd.find(snd_id);

		if ( i != m_mSnd.end() )
		{
			FSOUND_Stream_Stop( (FSOUND_STREAM*)i->second.pSnd );
		}
	}
}

void QMgrResSounds::SetPercent( const std::wstring& snd_id, float fPercent )
{
	if(!m_bSoundInited)
		return;

	if ( m_mSnd.size()>0 )
	{
		sndmap_iter i = m_mSnd.find(snd_id);

		if ( i != m_mSnd.end() )
		{
			int iMilliSec = (int) ( fPercent * i->second.iLengthMs );
			FSOUND_Stream_SetTime( (FSOUND_STREAM*)i->second.pSnd, iMilliSec );
		}
	}
}

void QMgrResSounds::SetTime( const std::wstring& snd_id, int iMilliSec )
{
	if(!m_bSoundInited)
		return;

	if ( m_mSnd.size()>0 )
	{
		sndmap_iter i = m_mSnd.find(snd_id);

		if ( i != m_mSnd.end() )
		{
			FSOUND_Stream_SetTime( (FSOUND_STREAM*)i->second.pSnd, iMilliSec );
		}
	}
}

int QMgrResSounds::GetTime( const std::wstring& snd_id )
{
	if(!m_bSoundInited)
		return 0;

	if ( m_mSnd.size()>0 )
	{
		sndmap_iter i = m_mSnd.find(snd_id);

		if ( i != m_mSnd.end() )
		{
			return FSOUND_Stream_GetTime( (FSOUND_STREAM*)i->second.pSnd );
		}
	}
	return 0;
}

void QMgrResSounds::OnResetDevice()
{
	FSOUND_SetPaused( FSOUND_ALL, false );
}

void QMgrResSounds::OnLostDevice()
{
	FSOUND_SetPaused( FSOUND_ALL, true );
}