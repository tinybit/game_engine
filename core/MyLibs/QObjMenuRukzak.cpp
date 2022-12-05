#define STRICT

#include "QObjMenuRukzak.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "QMgrResSounds.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"
#include "QMgrResStates.h"

QObjMenuRukzak::QObjMenuRukzak( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	
	SetEfnID( efn_id );
	SetTexID( tex_id );
	SetTexID2( tex_id2 );
	
	m_iCounter	= 0;

	m_bOpened	= false;

	D3DXMatrixScaling( &m_matScale, 1.5f, 1.5f, 1.5f );
}

QObjMenuRukzak::~QObjMenuRukzak()
{
}

void QObjMenuRukzak::Init()
{
	if ( GetObjMgr()->AddObjEfnAnim( GetEfnAnimObjID(), GetEfnID(), GetTexID(), GetTexID2() ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
		GetEfnAnim()->SetRenderPrms( true, true, true );
	}

	GetEfnAnim()->DeAttachAll();
	GetEfnAnim()->Stop( false );
	GetEfnAnim()->AnimAdd( L"Close_Idle", 0, -1 );
	GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjMenuRukzak::Kill()
{
	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();
}

void QObjMenuRukzak::AddListItem( const std::wstring& item_id, const std::wstring& item_name, const std::wstring& efn_id, const std::wstring& tex_id, bool bTexSphereMapping, const std::wstring& tex_id2, bool bTex2SphereMapping, const D3DMATERIAL9* pMtrl, bool bFillZBuffer )
{
	if ( m_mRukzakItemsList.size() > 0 )
	{
		mapWStrSRukzakItem_iter i = m_mRukzakItemsList.find( item_id );

		if ( i != m_mRukzakItemsList.end() )
		{
			g_Log.WriteError( L"RukzakItem already in rukzak items list, item_id = " + item_id );
			return;
		}
	}

	SRukzakItem temp;
	temp.item_id			= item_id;
	temp.item_name			= item_name;
	temp.efn_id				= efn_id;
	temp.tex_id				= tex_id;
	temp.bTexSphereMapping	= bTexSphereMapping;
	temp.tex_id2			= tex_id2;
	temp.bTex2SphereMapping	= bTex2SphereMapping;
	temp.bSetMtrl			= ( pMtrl != NULL );
	temp.mtrl				= ( ( pMtrl != NULL ) ? *pMtrl : GetMtrlDef() );
	temp.bFillZBuffer		= bFillZBuffer;
	m_mRukzakItemsList[ item_id ] = temp;

	g_EfnMgr.LoadEfn( efn_id );
	if ( tex_id != L"" )
		g_TexMgr.LoadTex( tex_id );
	if ( tex_id2 != L"" )
		g_TexMgr.LoadTex( tex_id2 );
}

SRukzakItem* QObjMenuRukzak::GetListItem( const std::wstring& item_id )
{
	mapWStrSRukzakItem_iter i = m_mRukzakItemsList.find( item_id );

	return ( ( i != m_mRukzakItemsList.end() ) ? &i->second : NULL );
}

std::wstring QObjMenuRukzak::GetItemName( const std::wstring& item_id )
{
	SRukzakItem* temp =	GetListItem( item_id );

	return ( ( temp != NULL ) ? temp->item_name : L"" );
}

void QObjMenuRukzak::AddChildToListItem( const std::wstring& item_id, const std::wstring& null_id, const std::wstring& child_item_id )
{
	SRukzakItem* temp = GetListItem( item_id );

	SWStrWStr s;
	s.s1 = null_id;
	s.s2 = child_item_id;
	temp->lstAttached.push_back( s );
}

void QObjMenuRukzak::AddItem( const std::wstring& item_id )
{
	if ( m_mRukzakItems.size() > 0 )
	{
		mapWStrInt_iter i = m_mRukzakItems.find( item_id );

		if ( i != m_mRukzakItems.end() )
		{
			g_Log.WriteError( L"RukzakItem already in rukzak items, item_id = " + item_id );
			return;
		}
	}

	m_mRukzakItems[ item_id ] = ++m_iCounter;
	m_mRukzakItemsSorted[ m_iCounter ] = item_id;

	g_StatesMgr.SetStateB( L"Game", L"Game." + item_id + L".Taked", true );
	g_StatesMgr.SetStateB( L"Game", L"Game." + item_id + L".Used", false );

	FillRukzak();
}

void QObjMenuRukzak::RemoveItem( const std::wstring& item_id, bool bUsed )
{
	mapWStrInt_iter i = m_mRukzakItems.find( item_id );
	if ( i != m_mRukzakItems.end() )
	{
		m_mRukzakItemsSorted.erase( i->second );
		m_mRukzakItems.erase( item_id );
	}
	g_StatesMgr.SetStateB( L"Game", L"Game." + item_id + L".Taked", false );
	g_StatesMgr.SetStateB( L"Game", L"Game." + item_id + L".Used", bUsed );
	FillRukzak();
}

void QObjMenuRukzak::SetZoomToItem( const std::wstring& item_id )
{
	if ( IsOpened() == false || item_id == L"" )
		return;

	std::wstring null_id = GetEfnAnim()->GetNullIDForAttachedObj( GetObjID() + L"_" + item_id );

	if ( null_id != L"" )
		GetEfnAnim()->SetModifyMatrix( null_id, GetObjID() + L"_" + item_id, &m_matScale );
}

void QObjMenuRukzak::ClearZooms()
{
	GetEfnAnim()->ClearModifyFlag();
}

void QObjMenuRukzak::Open()
{
	m_bOpened = true;
	FillRukzak();
	GetEfnAnim()->Stop( false );
	GetEfnAnim()->AnimAdd( L"Open", 0, 1 );
	GetEfnAnim()->AnimAdd( L"Open_Idle", 0, -1 );
	GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );
	GetEfnAnim()->SetSendAnimsToChilds( false );
	
}

void QObjMenuRukzak::Close()
{
	m_bOpened = false;
	GetEfnAnim()->Stop( false );
	GetEfnAnim()->AnimAdd3( L"", L"Close", 0, 1, GetObjID() + L":GetEfnAnim():DeAttachAll();" );
	GetEfnAnim()->AnimAdd( L"Close_Idle", 0, -1 );
	GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );
}

void QObjMenuRukzak::FillRukzak()
{
	if ( IsOpened() == false ) 
		return;

	GetEfnAnim()->DeAttachAll();
	
	std::wstring null_id = L"";
	//Бежим по всем позициям в рюкзаке в соответствии с сортировкой взятия
	for( mapIntWStr_iter i = m_mRukzakItemsSorted.begin(); i != m_mRukzakItemsSorted.end(); i++ )
	{
		//Получаем первый свободный нулик в рюкзаке
		null_id = GetEfnAnim()->GetFirstFreeNullID();

		if ( null_id != L"" )
		{
			if ( null_id.substr(0,5)==L"Item_" )
			{
				//получаем SRukzakItem в j из имени item'a i->second
				mapWStrSRukzakItem_iter j = m_mRukzakItemsList.find( i->second );

				if ( j != m_mRukzakItemsList.end() )
				{
					//Добавлем объект в выпадающий список, т.е. в менеджер объектов
					if ( RukzakAttach( GetEfnAnim(), null_id, GetObjID() + L"_" + i->second, &j->second ) )
					{
						SRukzakItem* temp = &j->second;
						//Если объект составной то аттачим его чайлды к нему
						if ( temp->lstAttached.size() > 0 )
						{
							//Получаем добавленный объект, корень составного объекта
							QObjEfnAnim* efn = GetObjMgr()->GetObjEfnAnim( GetObjID() + L"_" + i->second );
							efn->SetRenderPrms( true, true, true );

							//Бежим по всем его аттачам
							for( vecWStrWStr_iter k = temp->lstAttached.begin(); k != temp->lstAttached.end(); k++ )
							{
								//s1 = имя нуля
								//s2 = имя аттаченого объекта из списка объектов для рюкзака, но не в рюкзаке

								SRukzakItem* ra = GetListItem( (*k).s2 );

								if ( ra )
									RukzakAttach( efn, (*k).s1, efn->GetObjID() + L"_" + (*k).s2, ra );
							}
						}
					}
				}
			}
		}
	}
}

bool QObjMenuRukzak::RukzakAttach( QObjEfnAnim* efn, const std::wstring& null_id, const std::wstring& obj_id, SRukzakItem* temp )
{
	if ( efn->AttachObjToNull( null_id, obj_id, temp->efn_id, temp->tex_id, temp->tex_id2, L"" ) )
	{
		QObjEfnAnim* efn2 = GetObjMgr()->GetObjEfnAnim( obj_id );

		if ( temp->bSetMtrl )
			efn2->SetMtrl( &temp->mtrl );

		if ( temp->tex_id != L"" && temp->bTexSphereMapping )
			efn2->SetTexSphereMapping( true );

		if ( temp->tex_id2 != L"" && temp->bTex2SphereMapping )
			efn2->SetTex2SphereMapping( true );

		efn2->SetFillZBuffer( temp->bFillZBuffer );

		return true;
	}
	else
	{
		return false;
	}
}

bool QObjMenuRukzak::CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos )
{
	return ( GetEfnAnim() ? GetEfnAnim()->CheckForRayIntersect( pstRay, pfDist, pvIntrPos ) : false );
}

std::wstring QObjMenuRukzak::GetClickedItemID( const SRay* pstRay )
{
	mmapObjNullAttaches* temp = GetEfnAnim()->GetAttachedObjList();

	QObjEfnAnim* efn = NULL;
	for( mmapObjNullAttaches_iter i = temp->begin(); i != temp->end(); i++ )
	{
		efn = i->second.pObjEfnAnim;

		if ( efn )
		{
			if ( efn->CheckForRayIntersect( pstRay, NULL, NULL ) )
			{
				std::wstring q = efn->GetObjID();
				std::wstring s = q.substr( GetObjID().length()+1, q.length() - GetObjID().length() - 1 );
				return s;
			}
		}
	}
	return L"";
}

std::wstring QObjMenuRukzak::GetItemIDFromNum( int num )
{
	mmapObjNullAttaches* temp = GetEfnAnim()->GetAttachedObjList();

	QObjEfnAnim* efn = NULL;
	int j = 0;
	for( mmapObjNullAttaches_iter i = temp->begin(); i != temp->end(); i++ )
	{
		efn = i->second.pObjEfnAnim;

		if ( efn )
		{
			if ( num == j )
			{
				std::wstring q = efn->GetObjID();
				std::wstring s = q.substr( GetObjID().length()+1, q.length() - GetObjID().length() - 1 );
				return s;
			}
		}
		j++;
	}
	return L"";
}

void QObjMenuRukzak::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->OnFrameMove( fTime, fElapsedTime );
}

void QObjMenuRukzak::OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	if ( GetEfnAnim() )
	{
		GetEfnAnim()->OnFrameRender( fTime, fElapsedTime, ORT_NONTRANSPARENT );
		GetEfnAnim()->OnFrameRender( fTime, fElapsedTime, ORT_TRANSPARENT );
	}
}

//void QObjMenuRukzak::Go( double fTime, int seq_id )
//{
//	if ( GetEfnAnim() )
//		GetEfnAnim()->Go( fTime, 0, seq_id );
//}
//
//void QObjMenuRukzak::Stop(bool bOK)
//{
//	if ( GetEfnAnim() )
//		GetEfnAnim()->Stop(bOK);
//}