#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

using namespace std;

//---------------стейты----------------------------------
struct SStatesItem
{
	std::wstring		parent_id;
	std::wstring		state_id;
	double				value;
};

typedef map< std::wstring, SStatesItem, std::less<wstring> > statesmap;
typedef statesmap::iterator statesmap_iter;

//--------------------------евенты---------------------------
class QEventsItem
{
public:
	std::wstring		parent_id;
	std::wstring		event_id;
	mapWStrWStr			states;
	std::wstring		script;
};

typedef map< std::wstring, QEventsItem*, std::less<wstring> > eventsmap;
typedef eventsmap::iterator eventsmap_iter;

//--------------------------секвенции-------------------------------------
class QSequencesItem
{
public:
	std::wstring		parent_id;
	int					seq_id;
	listWStr			action_list;
	std::wstring		seq_state_id;
	std::wstring		seq_event_id;
};

typedef map< int, QSequencesItem*, std::less<int> > seqmap;
typedef seqmap::iterator seqmap_iter;

//--------------------------стек--------------------------------------------
struct SEventStack
{
	std::wstring		parent_id;
	std::wstring		state_id;
};

typedef list< SStatesItem > evstacklist;
typedef evstacklist::iterator evstacklist_iter;

//--------------------------сам класс---------------------
#define g_StatesMgr QMgrResStates::GetSingleton()

class QMgrResStates : public Singleton <QMgrResStates>
{
public:
	
	QMgrResStates();
	~QMgrResStates();

	void					LogEnabled(bool);

	void					Collect();

	std::wstring			StatesList();
	std::wstring			EventsList();

	void					SetState( const std::wstring& parent_id, const std::wstring& state_id, int iValue ) {	SetStateD( parent_id, state_id, (double)iValue ); };
	void					SetStateB( const std::wstring& parent_id, const std::wstring& state_id, bool bValue ) { SetState( parent_id, state_id, ( bValue ) ? 1 : 0 ); };
	void					SetStateF( const std::wstring& parent_id, const std::wstring& state_id, float fValue) { SetStateD( parent_id, state_id, (double)fValue ); };
	void					SetStateD( const std::wstring& parent_id, const std::wstring& state_id, double fValue);
	void					SetStateV2( const std::wstring& parent_id, const std::wstring& state_id, const D3DXVECTOR2* pvValue ) { SetStateF( parent_id, state_id + L".x", pvValue->x );	SetStateF( parent_id, state_id + L".y", pvValue->y ); };
	void					SetStateV3( const std::wstring& parent_id, const std::wstring& state_id, const D3DXVECTOR3* pvValue ) { SetStateF( parent_id, state_id + L".x", pvValue->x );	SetStateF( parent_id, state_id + L".y", pvValue->y ); SetStateF( parent_id, state_id + L".z", pvValue->z ); };
	int						GetState( const std::wstring& state_id ) { return (int)GetStateF( state_id ); };
	bool					GetStateB( const std::wstring& state_id ) { return ( GetState( state_id ) > 0 ) ? true : false; };
	float					GetStateF( const std::wstring& state_id ) { return (float)GetStateD( state_id ); };
	inline double			GetStateD( const std::wstring& state_id );
	D3DXVECTOR2				GetStateV2( const std::wstring& state_id ) {	return D3DXVECTOR2( GetStateF( state_id + L".x" ), GetStateF( state_id + L".y" ) ); };
	D3DXVECTOR3				GetStateV3( const std::wstring& state_id ) {	return D3DXVECTOR3( GetStateF( state_id + L".x" ), GetStateF( state_id + L".y" ), GetStateF( state_id + L".z" ) ); };
	inline void				IncState( const std::wstring& state_id );
	void					DelStates( const std::wstring& parent_id );
	
	void					SetEvent( const std::wstring& parent_id, const std::wstring& event_id, const std::wstring& sStates, const std::wstring& sScript );
	void					DelEvent( const std::wstring& event_id );
	std::wstring			DelEvent2( const std::wstring& event_id );
	void					DelEvents( const std::wstring& parent_id );

	int						SeqInit( const std::wstring& parent_id );
	void					SeqAddAct( int seq_id, const std::wstring& sScript );
	void					SeqAddActInc( int seq_id, const std::wstring& sScript );
	void					SeqPause( int seq_id, UINT iMilliSec );
	void					SeqActGo( int seq_id );
	void					SeqGo( int seq_id );
	void					SeqDel( int seq_id );
	std::wstring			SeqDel2( int seq_id );
	void					SeqDels( const std::wstring& parent_id );
	void					SeqIncState( int seq_id );
	void					SeqZeroState( int seq_id );

	void					EvStackAdd( SStatesItem Stateitem );
	void					EvStackProc();
	void					EvStackKill();
	int						GetEvState();
	bool					EvNotKilled();
	void					EvKill();
	void					DoEvents( SStatesItem* StateItem );

private:

	int						m_iSeqCount;
	int						m_iEventCount;

	statesmap				m_mStates;
	mmapWStrWStr			m_mmStates;
	
	eventsmap				m_mEvents;
	mmapWStrWStr			m_mmEvents;

	seqmap					m_mSequences;
	mmapWStrInt				m_mmSequences;

	evstacklist				m_lEvStack;

	bool					m_bLogEnabled;
};