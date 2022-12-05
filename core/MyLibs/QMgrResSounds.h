#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "fmod.h"
#include "fmod_errors.h"

using namespace std;

struct SSoundListItem
{
	std::wstring	snd_id;
	UINT			iType;
	UINT			iLengthMs;
	UINT			iLength;

	void*			pSnd;
};

typedef map< std::wstring, SSoundListItem, std::less<wstring> > sndmap;
typedef sndmap::iterator sndmap_iter;

#define g_SndMgr QMgrResSounds::GetSingleton()

class QMgrResSounds : public Singleton <QMgrResSounds>
{
public:
	//Constructor/Destructor
	QMgrResSounds();
	~QMgrResSounds();

	void					LogEnabled(bool);

	bool					Init( int mixrate, int maxsoftwarechannels );

	void					LoadSmpl( const std::wstring& snd_id );
	void					LoadStrm( const std::wstring& snd_id );
	void					FreeSnd( const std::wstring snd_id );

	void					PlaySnd( const std::wstring& snd_id );
	void					PlaySnd2( const std::wstring& snd_id, bool bLooped, float fVolume );
	void					StopSnd( const std::wstring& snd_id );

	void					SetPercent( const std::wstring& snd_id, float fPercent );

	void					SetTime( const std::wstring& snd_id, int iMilliSec );
	int						GetTime( const std::wstring& snd_id );

	void					OnResetDevice();
	void					OnLostDevice();

private:

	void					LoadSnd( const std::wstring& snd_id, UINT iType );

	bool					m_bLogEnabled;

	sndmap					m_mSnd;

	bool					m_bSoundInited;

};