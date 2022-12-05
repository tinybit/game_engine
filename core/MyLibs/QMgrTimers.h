#pragma once
#pragma warning( disable : 4244 )

#include "dxstdafx.h"
#include "stl_inc.h"

#include "CommonDefines.h"

using namespace std;

void CALLBACK OnTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

struct STimer
{
	std::wstring	sTimerID;
	UINT_PTR		iTimerID;
	int				iInterval;
	std::wstring	sScript;
	int				iRepeatCount;
	bool			bPaused;
};

typedef map< std::wstring, STimer, std::less<wstring> > timersmap;
typedef timersmap::iterator timersmap_iter;

typedef map< UINT, std::wstring, std::less<UINT> > timersmap2;
typedef timersmap2::iterator timersmap2_iter;

#define g_Timers QMgrTimers::GetSingleton()

class QMgrTimers : public Singleton <QMgrTimers>
{
public:
	//Constructor/Destructor
	QMgrTimers();
	~QMgrTimers();

	void					LogEnabled(bool);

	std::wstring			TimersList();

	void					TimerSet( const std::wstring& timer_id, int iInterval, const std::wstring& sScript );
	void					TimerSet2( const std::wstring& timer_id, int iInterval, const std::wstring& sScript, int iRepeatCount );
	void					TimerKill( const std::wstring& timer_id );
	void					TimerKillAll();
	void					TimerPause( const std::wstring& timer_id );
	void					TimerContinue( const std::wstring& timer_id );

	timersmap				m_mTimers;
	timersmap2				m_mTimers2;

private:
	
	bool					m_bLogEnabled;

};