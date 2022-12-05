#define STRICT

#include "QMgrResStates.h"

#include "UtilsConverts.h"

#include "QSubSysLog.h"

#include "QMgrScripts.h"
#include "QMgrMiniGames.h"

QMgrResStates::QMgrResStates()
{
	m_iSeqCount = 0;
	m_iEventCount = 0;

	m_bLogEnabled = false;
}

QMgrResStates::~QMgrResStates()
{
	if ( m_mSequences.size()>0 )
	{
		for( seqmap_iter i = m_mSequences.begin(); i != m_mSequences.end(); i++ )
		{
			QSequencesItem* temp = i->second;
			temp->action_list.clear();
			SAFE_DELETE(temp);
		}
		m_mSequences.clear();
	}
	m_mmSequences.clear();

	m_mStates.clear();
	m_mmStates.clear();

	if ( m_mEvents.size()>0 )
	{
		for( eventsmap_iter i = m_mEvents.begin(); i != m_mEvents.end(); i++ )
		{
			QEventsItem* temp = i->second;
			temp->states.clear();
			SAFE_DELETE(temp);
		}
		m_mEvents.clear();
	}
	m_mmEvents.clear();
}

void QMgrResStates::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"state mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"state mgr logging disabled");
	}
}

void QMgrResStates::Collect()
{
	if ( m_mStates.size() > 0 )
	{
		SStatesItem* temp = NULL;
		for( statesmap_iter i = m_mStates.begin(); i != m_mStates.end(); )
		{
			temp = &i->second;

			if ( temp )
			{
				if ( temp->value == -1 )
				{
					i = m_mStates.erase( i );
				}
				else
				{
					i++;
				}
			}
		}
	}
}

std::wstring QMgrResStates::StatesList()
{
	if ( m_mStates.size()>0 )
	{
		std::wstring s = L"-------------STATES LIST-------------\n\n";
		float temp = 0;
		for( statesmap_iter i = m_mStates.begin(); i != m_mStates.end(); ++i )
		{
			temp = i->second.value;
			if ( (int)temp == temp )
				s += L" id=" + i->first + L" value="+int2wstr(i->second.value) + L";\r\n";
			else
				s += L" id=" + i->first + L" value="+dbl2wstr(i->second.value) + L";\r\n";
		}
		return s;
	}
	else
	{
		return L"Нет переменных состояния";
	}
}

std::wstring QMgrResStates::EventsList()
{
	if ( m_mEvents.size()>0 )
	{
		std::wstring s = L"-------------EVENTS LIST-------------\n\n";
		for( eventsmap_iter i = m_mEvents.begin(); i != m_mEvents.end(); ++i )
		{
			s += L"event_id=\"" + i->first + L"\", states=\"" + mConCat(i->second->states, L"\", \"") + L" script=\""+i->second->script + L"\"\n";
		}
		return s;
	}
	else
	{
		return L"Нет евентов";
	}
}

//задает значение стейта
void QMgrResStates::SetStateD( const std::wstring& parent_id, const std::wstring& state_id, double fValue )
{
	if ( m_mStates.size() > 0 )
	{
		statesmap_iter i = m_mStates.find(state_id);

		if ( i != m_mStates.end() )
		{
			if ( i->second.value != fValue)
			{
				i->second.value = fValue;
				if ( fValue != -1.0 )
				{
					EvStackAdd( i->second );
				}
			}
			return;
		}
	}

	SStatesItem StatesItem;
	StatesItem.parent_id	= parent_id;
	StatesItem.state_id		= state_id;
	StatesItem.value		= fValue;

	m_mStates[state_id] = StatesItem;
	m_mmStates.insert( mmapWStrWStr::value_type( parent_id, state_id ) );
	if ( fValue != -1.0f )
	{
		EvStackAdd( StatesItem );
	}
}

//инкрементирует стейт
void QMgrResStates::IncState( const std::wstring& state_id )
{
	if ( m_mStates.size() > 0 )
	{
		statesmap_iter i = m_mStates.find( state_id );

		if ( i != m_mStates.end() )
		{
			SetState( i->second.parent_id, state_id, i->second.value + 1.0 );
		}
	}
}

void QMgrResStates::DelStates( const std::wstring& parent_id )
{
	if ( m_mStates.size() > 0 && m_mmStates.count(parent_id) > 0)
	{
		mmapWStrWStr_iter beg = m_mmEvents.lower_bound(parent_id);
		mmapWStrWStr_iter end = m_mmEvents.upper_bound(parent_id);

		for( mmapWStrWStr_iter i = beg; i != end; i++ )
		{
			m_mStates.erase( i->second );
		}
		m_mmStates.erase(parent_id);
	}
}

double QMgrResStates::GetStateD( const std::wstring& state_id )
{
	if ( m_mStates.size() > 0 )
	{
		statesmap_iter i = m_mStates.find(state_id);

		if ( i != m_mStates.end() )
		{
			return i->second.value;
		}
	}
	return -1.0;
}

//--------------------------------------events------------------------------------------------

void QMgrResStates::SetEvent( const std::wstring& parent_id, const std::wstring& event_id, const std::wstring& sStates, const std::wstring& sScript )
{
	if ( m_mEvents.size()>0 )
	{
		eventsmap_iter i = m_mEvents.find(event_id);

		if ( i != m_mEvents.end() )
		{
			mSplit( i->second->states, sStates, L"," );
			i->second->script = sScript;

			if(m_bLogEnabled)
			{
				g_Log.WriteWarning(L"set event = \"" + event_id + L"\", states = \"" + sStates + L"\" script = \"" + sScript + L"\"");
			}
			return;
		}
	}

	QEventsItem* EventsItem = new QEventsItem();
	EventsItem->parent_id = parent_id;
	EventsItem->event_id  = event_id;
	mSplit( EventsItem->states, sStates, L"," );
	EventsItem->script	 = sScript;

	m_mEvents[event_id] = EventsItem;
	m_mmEvents.insert( mmapWStrWStr::value_type( parent_id, event_id )  );
	if(m_bLogEnabled)
	{
		g_Log.WriteWarning(L"set event = \"" + event_id + L"\", states = \"" + sStates + L"\" script = \"" + sScript + L"\"");
	}
}

void QMgrResStates::DelEvent( const std::wstring& event_id )
{
	std::wstring parent_id = DelEvent2( event_id );
	
	if ( m_mmEvents.size() > 0 && parent_id != L"" )
	{
		if ( m_mmEvents.count( parent_id ) > 0 )
		{
			mmapWStrWStr_iter beg = m_mmEvents.lower_bound( parent_id );
			mmapWStrWStr_iter end = m_mmEvents.upper_bound( parent_id );

			for ( mmapWStrWStr_iter i = beg; i != end; i++)
			{
				if ( i->second == event_id )
				{
					m_mmEvents.erase( i );
					return;
				}
			}
		}
	}
}

std::wstring QMgrResStates::DelEvent2( const std::wstring& event_id )
{
	if ( m_mEvents.size() > 0 )
	{
		eventsmap_iter i = m_mEvents.find( event_id );

		if ( i != m_mEvents.end() )
		{
			QEventsItem* temp = i->second;
			std::wstring parent_id = temp->parent_id;

			temp->states.clear();
			
			SAFE_DELETE(temp);
			m_mEvents.erase( event_id );
			
			return parent_id;
		}
	}
	return L"";
}

void QMgrResStates::DelEvents( const std::wstring& parent_id )
{
	if ( m_mmEvents.size()>0 )
	{
		if ( m_mmEvents.count( parent_id ) > 0 )
		{
			mmapWStrWStr_iter beg = m_mmEvents.lower_bound( parent_id );
			mmapWStrWStr_iter end = m_mmEvents.upper_bound( parent_id );

			for ( mmapWStrWStr_iter i = beg; i != end; i++)
			{
				DelEvent2( i->second );
			}
			m_mmEvents.erase( beg, end );
		}
	}
}

//------------------------------------sequences---------------------------------------

int QMgrResStates::SeqInit( const std::wstring& parent_id )
{
	QSequencesItem* seq = new QSequencesItem();
	seq->parent_id = parent_id;
	seq->seq_id	= m_iSeqCount++;
	seq->seq_state_id = parent_id + L".seq_" + int2wstr(seq->seq_id) + L"_iter";
	seq->seq_event_id = parent_id + L".seq_" + int2wstr(seq->seq_id);

	m_mSequences[seq->seq_id] = seq;
	m_mmSequences.insert( mmapWStrInt::value_type(parent_id, seq->seq_id) );

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"seq init, parent_id = \"" + seq->parent_id + L"\", seq_id = \"" + int2wstr(seq->seq_id) + L"\"");
	}

	return seq->seq_id;
}

void QMgrResStates::SeqAddAct(int seq_id, const std::wstring& sScript )
{
	if ( m_mSequences.size() > 0 )
	{
		seqmap_iter i = m_mSequences.find(seq_id);

		if ( i != m_mSequences.end() )
		{
			i->second->action_list.push_back(sScript);
		}
	}	
}

void QMgrResStates::SeqAddActInc( int seq_id, const std::wstring& sScript )
{
	SeqAddAct( seq_id, sScript + L" SeqIncState(" + int2wstr(seq_id) + L");\r\n" );
}

void QMgrResStates::SeqPause( int seq_id, UINT iMilliSec )
{
	std::wstring temp = L"TimerSet2( L(\"seq_" + int2wstr(seq_id) + L"_pause\"), " + 
						int2wstr(iMilliSec) + L", L(\"SeqIncState(" + int2wstr(seq_id) + L");\"), 1 "+
						L");";
	SeqAddAct( seq_id, temp );
}

void QMgrResStates::SeqGo( int seq_id )
{
	if ( m_mSequences.size() > 0 )
	{
		seqmap_iter i = m_mSequences.find(seq_id);

		if ( i != m_mSequences.end() )
		{
			if( i->second->action_list.size() > 0)
			{
				std::wstring s = L"g_StatesMgr:SeqActGo(" + int2wstr(seq_id) + L");";
				SetEvent( i->second->parent_id, i->second->seq_event_id, i->second->seq_state_id, s);
				SetState( i->second->parent_id, i->second->seq_state_id, 1);
			}
			else
			{
				SeqDel(seq_id);
			}
		}
	}
}

void QMgrResStates::SeqActGo( int seq_id )
{
	if ( m_mSequences.size() > 0 )
	{
		seqmap_iter i = m_mSequences.find( seq_id );

		if ( i != m_mSequences.end() )
		{
			if ( i->second->action_list.size()>0 && GetState( i->second->seq_state_id ) != 0 )
			{
				std::wstring s = *( i->second->action_list.begin() );
				i->second->action_list.erase( i->second->action_list.begin() );
				g_Script.ExecScript(s);
			}
			else
			{
				SeqDel(seq_id);
			}
		}
	}
}

void QMgrResStates::SeqIncState( int seq_id )
{
	if ( m_mSequences.size() > 0 )
	{
		seqmap_iter i = m_mSequences.find(seq_id);

		if ( i != m_mSequences.end() )
		{
			if ( GetState( i->second->seq_state_id ) > 0 )
			{
				IncState( i->second->seq_state_id );
			}
		}
	}
}

void QMgrResStates::SeqZeroState( int seq_id )
{
	if ( m_mSequences.size() > 0 )
	{
		seqmap_iter i = m_mSequences.find(seq_id);

		if ( i != m_mSequences.end() )
		{
			SetState( i->second->parent_id, i->second->seq_state_id, 0 );
		}
	}
}

void QMgrResStates::SeqDel( int seq_id )
{
	std::wstring parent_id = SeqDel2( seq_id );
	
	if ( m_mmSequences.size() > 0 && parent_id !=L"" )
	{
		mmapWStrInt_iter beg = m_mmSequences.lower_bound( parent_id );
		mmapWStrInt_iter end = m_mmSequences.upper_bound( parent_id );

		for( mmapWStrInt_iter i = beg; i != end; i++)
		{
			if ( i->second == seq_id )
			{
				m_mmSequences.erase( i );
				return;
			}
		}
	}
}

std::wstring QMgrResStates::SeqDel2( int seq_id )
{
	if ( m_mSequences.size() > 0 )
	{
		seqmap_iter i = m_mSequences.find( seq_id );

		if ( i != m_mSequences.end() )
		{
			QSequencesItem* temp = i->second;
			std::wstring parent_id = temp->parent_id;
			
			DelEvent( temp->seq_event_id );
			SetState( parent_id, temp->seq_state_id, -1 );
			temp->action_list.clear();
			SAFE_DELETE(temp);
			m_mSequences.erase( seq_id );
			return parent_id;
		}
	}
	return L"";
}

void QMgrResStates::SeqDels( const std::wstring& parent_id )
{
	if ( m_mmSequences.size() > 0 )
	{
		mmapWStrInt_iter beg = m_mmSequences.lower_bound( parent_id );
		mmapWStrInt_iter end = m_mmSequences.upper_bound( parent_id );

		for( mmapWStrInt_iter i = beg; i != end; ++i)
		{
			SeqDel2( i->second );
		}
		m_mmSequences.erase( beg, end );
	}	
}

void QMgrResStates::EvStackAdd( SStatesItem StateItem )
{
	m_lEvStack.push_back( StateItem );
}

//отрабатывает события из стека
void QMgrResStates::EvStackProc()
{
	//если список подписанных обработчиков событий пуст или список событий пуст то выходим
	if ( m_mEvents.size() == 0 || m_lEvStack.size()==0 )
	{
		return;
	}

	//отрабатываем события
	while( m_lEvStack.size() > 0 )
	{
		SStatesItem* StateItem = &(*m_lEvStack.begin());

		//Отрабатываем события для корня, если событие системное то оно отработает и пойдет дальше по
		//минииграм, если останется живо, если же событие уровня миниигры то оно отработает только в нем
		DoEvents( StateItem );
			
		if ( StateItem->parent_id==L"sys" && EvNotKilled() )
		{
			//делаем копию списка игр на случай если событие убьет игру
			zmap m_mZ = g_GamesMgr.GetSortedGameList();

			//бежим по списку игр начиная с передней и перед каждой проверяем не убито ли событие
			//раздаем системное событие минииграм
			QGameBase* m_pGame = NULL;

			//Делаем копию стейта для события
			SStatesItem ChildStateItem = *StateItem;;

			for( zmap_iter z = m_mZ.begin(); z != m_mZ.end() && EvNotKilled(); z++ )
			{
				m_pGame = z->second;
				
				if ( !m_pGame )
					continue;

				if ( m_pGame->isMovePaused() || m_pGame->isFreezed() )
					continue;

				//Меняем парента у стейта события, чтобы миниигра узнала свое событие
				ChildStateItem.parent_id	= m_pGame->GetName();

				DoEvents( &ChildStateItem );
			}
		}

		m_lEvStack.erase( m_lEvStack.begin() );
	}
}

void QMgrResStates::EvStackKill()
{
	m_lEvStack.clear();
}

void QMgrResStates::DoEvents( SStatesItem* StateItem )
{
	//смотрим есть ли евенты для данного парента
	int m_iEventsCount = m_mmEvents.count( StateItem->parent_id );
	if ( m_iEventsCount == 0 )
		return;

	//---------создаем копию таблицы евентов на случай если евенты будут убивать себя-----------
	mmapWStrWStr_iter beg = m_mmEvents.lower_bound(StateItem->parent_id);
	mmapWStrWStr_iter end = m_mmEvents.upper_bound(StateItem->parent_id);
	
	vecWStr m_lEventsTemp;
	m_lEventsTemp.reserve( m_iEventsCount );

	for( mmapWStrWStr_iter i = beg; i != end; i++ )
	{
		m_lEventsTemp.push_back(i->second);
	}
	//------------------------------------------------------------------------------------------

	//-------------------отрабатываем евенты для данного парента----------------------------------
	for( vecWStr_iter i = m_lEventsTemp.begin(); (i != m_lEventsTemp.end()) && EvNotKilled(); i++ )
	{
		//ищем евент с заданным именем
		eventsmap_iter e = m_mEvents.find(*i);

		if ( e != m_mEvents.end() )
		{
			QEventsItem* ev = e->second;

			if ( ev )
			{
				//смотрим если среди стейтов этого евента есть необходимый стейт то выполняем событие
				if ( ev->states.find(StateItem->state_id) != ev->states.end() )
				{
					if(m_bLogEnabled)
					{
						g_Log.WriteWarning(L"exec event, event_id=\"" + ev->event_id + L"\", states=\"" + mConCat(ev->states, L"\", ") + L" script=\""+ev->script + L"\"");
					}

					g_Script.ExecScript(ev->script);
				}
			}
		}
	}
	//----------------------------------------------------------------------------------------------
	m_lEventsTemp.clear();
}

int QMgrResStates::GetEvState()
{
	if ( m_lEvStack.size() > 0 )
	{
		SStatesItem* StateItem = &(*m_lEvStack.begin());

		if ( StateItem )
			return StateItem->value;
	}

	return -1;
}


bool QMgrResStates::EvNotKilled()
{
	SStatesItem* StateItem = &(*m_lEvStack.begin());

	return (StateItem->value!=-1);
}

void QMgrResStates::EvKill()
{
	SStatesItem* StateItem = &(*m_lEvStack.begin());

	StateItem->value		= -1.0;
}