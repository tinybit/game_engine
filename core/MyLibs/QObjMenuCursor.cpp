#define STRICT

#include "QObjMenuCursor.h"

#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysInput.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "QMgrResSounds.h"
#include "QMgrMiniGames.h"
#include "QMgrScripts.h"

#include "QObjMenuRukzak.h"

QObjMenuCursor::QObjMenuCursor( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& rukzak_obj_id )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	
	SetEfnID( efn_id );
	SetTexID( tex_id );
	SetTexID2( tex_id2 );

	m_sItemID			= L"";

	m_iCursorType		= 0;
	m_sCursorAnim		= L"";
	m_sHint				= L"";	

	m_sRukzakObjID = rukzak_obj_id;
}

QObjMenuCursor::~QObjMenuCursor()
{
}

void QObjMenuCursor::Init()
{
	if ( GetObjMgr()->AddObjEfnAnim( GetEfnAnimObjID(), GetEfnID(), GetTexID(), GetTexID2() ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
		GetEfnAnim()->SetRenderPrms( true, true, true );
	}

	SetCursorAnim( L"" );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjMenuCursor::Kill()
{
	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();
}

void QObjMenuCursor::SetItem( const std::wstring& item_id )
{
	m_sItemID = item_id;

	GetEfnAnim()->DeAttachAll();

	QObjMenuRukzak* obj = GetObjMgr()->GetObjMenuRukzak( m_sRukzakObjID );

	if ( obj )
	{
		SRukzakItem* temp = obj->GetListItem( item_id );

		if ( temp )
		{
			if ( CursorAttach( GetEfnAnim(), L"in_hand", GetObjID() + L"_" + item_id, temp ) )
			{
				//Если объект составной то аттачим его чайлды к нему
				if ( temp->lstAttached.size() > 0 )
				{
					//Получаем добавленный объект, корень составного объекта
					QObjEfnAnim* efn = GetObjMgr()->GetObjEfnAnim( GetObjID() + L"_" + item_id );
					efn->SetRenderPrms( true, true, true );

					//Бежим по всем его аттачам
					for( vecWStrWStr_iter k = temp->lstAttached.begin(); k != temp->lstAttached.end(); k++ )
					{
						//s1 = имя нуля
						//s2 = имя аттаченого объекта из списка объектов для рюкзака, но не в рюкзаке

						SRukzakItem* ra = obj->GetListItem( (*k).s2 );

						if ( ra )
							CursorAttach( efn, (*k).s1, efn->GetObjID() + L"_" + (*k).s2, ra );
					}
				}
			}
		}
	}
}

void QObjMenuCursor::Clear()
{
	m_sItemID = L"";
	GetEfnAnim()->DeAttachAll();
}

bool QObjMenuCursor::CursorAttach( QObjEfnAnim* efn, const std::wstring& null_id, const std::wstring& obj_id, void* pSRukzakItem )
{
	SRukzakItem* temp = (SRukzakItem*)pSRukzakItem;
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

void QObjMenuCursor::SetCursorAnim( const std::wstring& sAnim )
{
	SetCursorAnim2( L"", sAnim );
}

void QObjMenuCursor::SetCursorAnim2( const std::wstring efn_id, const std::wstring& sAnim )
{
	std::wstring s = ( (sAnim != L"") ? sAnim : L"stand" );

	if ( s != m_sCursorAnim )
	{
		m_sCursorAnim = s;

		if ( s == L"stand" )
			m_iCursorType = 0;
		else
			m_iCursorType = 1;
		
		GetEfnAnim()->Stop(false);
		GetEfnAnim()->AnimAdd2( efn_id, m_sCursorAnim, 0, -1 );
		GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );
	}
}

void QObjMenuCursor::MouseClick( int iMouseEvType )
{
	if ( GetCursorType() == 0 )
	{
		if ( (iMouseEvType == 0) || (iMouseEvType == 1) || (iMouseEvType == 3) )
		{
			GetEfnAnim()->Stop( false );
			
			if ( iMouseEvType == 0 )
			{
				GetEfnAnim()->AnimAdd( L"click end", 0, 1 );
				GetEfnAnim()->AnimAdd( L"click end freeze", 0, -1 );
			}
			else
			{
				GetEfnAnim()->AnimAdd( L"click start", 0, 1 );
				GetEfnAnim()->AnimAdd( L"click start freeze", 0, -1 );
			}

			GetEfnAnim()->Go( GetGame()->GetGameTime(), 0, -1 );
		}
	}
}

void QObjMenuCursor::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->OnFrameMove( fTime, fElapsedTime );
}

void QObjMenuCursor::OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	if ( GetEfnAnim() )
	{
		GetEfnAnim()->OnFrameRender( fTime, fElapsedTime, ORT_NONTRANSPARENT );
		GetEfnAnim()->OnFrameRender( fTime, fElapsedTime, ORT_TRANSPARENT );
	}

	if ( m_sHint != L"" )
		g_Graph.RenderText4( g_Mouse.x + 50, g_Mouse.y + 15 , m_sHint, 0xFFDCFF00, 0x80000000, L"default", 5 );
}