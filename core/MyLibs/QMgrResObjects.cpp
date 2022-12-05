#define STRICT

#include "QMgrResObjects.h"

#include "UtilsConverts.h"

#include "QSubSysLog.h"
#include "QSubSysGraphics.h"

#include "IMgrResEfnModels.h"
#include "QMgrResStates.h"
#include "QMgrScripts.h"

#include "QGameBase.h"

#include "QObjBase.h"
#include "QObjMan.h"
#include "QObjCar.h"
#include "QObjBird.h"
#include "QObjFree.h"
#include "QObjCamera.h"
#include "QObjLight.h"
#include "QObjShadowVolume.h"
#include "QObjLayerGeom.h"
#include "QObjAction.h"
#include "QObjEfnAnim.h"
#include "QObjVideoAnim.h"
#include "QObj3DVideo.h"
#include "QObjMenuRukzak.h"
#include "QObjMenuCursor.h"
#include "QObjMenuMenu.h"


QMgrResObjects::QMgrResObjects()
{
	m_bLogEnabled = false;

	m_mObj.clear();
	m_mmGroup.clear();
	m_lstRenderBuffer.clear();
	m_mapZSortedRenderBuffer.clear();

}

QMgrResObjects::~QMgrResObjects()
{
	DelAll();
}

std::wstring QMgrResObjects::GetObjList()
{
	std::wstring s = L"-------------" + GetGame()->GetName() + L" OBJECTS LIST-------------\n\n";

	if ( m_mObj.size() > 0 )
	{
		QObjBase* obj = NULL;
		for( objmap_iter i = m_mObj.begin(); i != m_mObj.end(); i++ )
		{
			obj = i->second;

			if ( obj )
			{
				s += L"obj_id = \"" + obj->GetObjID() + L"\", type = " + obj->GetTypeStr() + L", pos = " + vec3_2wstr(&obj->GetNullPos()->vPos);

				if ( obj->GetType() == OT_EFNANIM )
				{
					s += L", anim = \"" + ((QObjEfnAnim*)obj)->GetAnimName() + L"\"\n";
					s += L"                     path length = " + dbl2wstr( ((QObjEfnAnim*)obj)->GetPathLength() ) + L", ";
					s += L"path last = " + dbl2wstr( ((QObjEfnAnim*)obj)->GetPathLast() ) + L", iFrame = " + int2wstr(((QObjEfnAnim*)obj)->GetFrame()) + L"\n";
				}
				else
				{
					s += L"\n";
				}
			}
		}
	}
	return s;
}

std::wstring QMgrResObjects::GetObjGroupsList()
{
	std::wstring s = L"-------------" + GetGame()->GetName() + L" OBJECTS GROUPS LIST-------------\n\n";

	if ( m_mmGroup.size() > 0 )
	{
		for( mmapWStrWStr_iter i = m_mmGroup.begin(); i != m_mmGroup.end(); i++ )
		{
			s += i->first + L"   -   " + i->second + L"\n";
		}
	}
	return s;
}

void QMgrResObjects::DelAll()
{
	while( m_mObj.size() > 0 )
	{
		DelObj( m_mObj.begin()->first );
	}
	m_mmGroup.clear();
	m_lstRenderBuffer.clear();
	m_mapZSortedRenderBuffer.clear();
}

void QMgrResObjects::LogEnabled(bool bLogEnabled)
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage( L"object mgr logging enabled" );
	}
	else
	{
		g_Log.WriteMessage( L"object mgr logging disabled" );
	}
}

bool QMgrResObjects::AddObj( std::wstring obj_id, QObjBase* obj, ObjTypes ot )
{
	if ( obj == NULL )
	{
		g_Log.WriteError(L"Object is null in \"bool QMgrResObjects::AddObj(std::wstring obj_id, QObject* obj, ObjTypes ot )\", obj_id = \"" + obj_id + L"\"" );
		return false;
	}

	//check for errors in objects names
	if ( obj_id.find_first_of( L" " ) != obj_id.npos ||
		 obj_id.find_first_of( L"." ) != obj_id.npos )
	{
		g_Log.WriteError( L"cant use space char and '.' in object id, obj_id = \"" + obj_id + L"\" all this characters replaced with '_'" );
		int a;
		std::wstring s = L"_";
		while( (a = obj_id.find_first_of( L" " )) != obj_id.npos )
		{
			obj_id = obj_id.replace( a, 1, s );
		}
		while( (a = obj_id.find_first_of( L"." )) != obj_id.npos )
		{
			obj_id = obj_id.replace( a, 1, s );
		}
	}

	

	if ( m_mObj.size() > 0 )
	{
		objmap_iter i = m_mObj.find(obj_id);

		if ( i != m_mObj.end() )
		{
			SAFE_DELETE(obj);
			g_Log.WriteError(L"Object already added. obj_id = \"" + obj_id + L"\"" );
			return false;
		}
	}

	m_mObj[obj_id] = obj;
	
	obj->SetObjID( obj_id );
	obj->SetType( ot );
	obj->SetObjMgr( this );
	obj->SetMtrl( NULL );
	obj->SetEfnAnim( NULL );
	obj->SetEnabled( true );
	obj->SetVisible( true );
	obj->SetGame( m_pGame );
	obj->Init();

	RegisterObjectInLua( obj );

	if ( m_bLogEnabled )
	{
		g_Log.WriteWarning( L"Object added. type = \"" + obj->GetTypeStr() + L"\", obj_id = \"" + obj_id + L"\")" );
	}

	return true;
}

bool QMgrResObjects::AddObjMan( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	if ( AddObj( obj_id, new QObjMan( efn_id, tex_id, tex_id2 ), OT_MAN ) )
	{
		return AddObjToGroup( L"Mans", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjCar( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	if ( AddObj( obj_id, new QObjCar( efn_id, tex_id, tex_id2 ), OT_CAR ) )
	{
		return AddObjToGroup( L"Cars", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjBird( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	if ( AddObj( obj_id, new QObjBird( efn_id, tex_id, tex_id2 ), OT_BIRD ) )
	{
		return AddObjToGroup( L"Birds", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjFree( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, bool bFillGopa )
{
	if ( AddObj( obj_id, new QObjFree( efn_id, L"", tex_id, tex_id2, bFillGopa ), OT_FREE ) )
	{
		return AddObjToGroup( L"FreeObjs", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjFreeE( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, bool bFillGopa )
{
	if ( AddObj( obj_id, new QObjFree( efn_id, meshes_efn_id, tex_id, tex_id2, bFillGopa ), OT_FREE ) )
	{
		return AddObjToGroup( L"FreeObjs", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjLight( const std::wstring& obj_id, const D3DLIGHT9* pLight )
{
	if ( AddObj( obj_id, new QObjLight( pLight ), OT_LIGHT ) )
	{
		return AddObjToGroup( L"Lights", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjCam( const std::wstring& obj_id )
{
	if ( AddObj( obj_id, new QObjCamera(), OT_CAMERA ) )
	{
		return AddObjToGroup( L"Cameras", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjSV( const std::wstring& obj_id, const std::wstring& efn_id)
{
	if ( AddObj( obj_id, new QObjShadowVolume( efn_id ), OT_SHADOWVOLUME ) )
	{
		return AddObjToGroup( L"ShadowVolumes", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjLG( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& state_id )
{
	if ( AddObj( obj_id, new QObjLayerGeom( efn_id, state_id ), OT_LAYERGEOM ) )
	{
		return AddObjToGroup( L"LayerGeoms", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjAction( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& sScript )
{
	if ( AddObj( obj_id, new QObjAction( efn_id, sScript ), OT_ACTION ) )
	{
		return AddObjToGroup( L"ObjActions", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjEfnAnimE( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	if ( AddObj( obj_id, new QObjEfnAnim( efn_id, meshes_efn_id, tex_id, tex_id2 ), OT_EFNANIM ) )
	{
		return AddObjToGroup( L"ObjEfnAnims", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjVideoAnim( const std::wstring& obj_id )
{
	if ( AddObj( obj_id, new QObjVideoAnim(), OT_VIDEOANIM ) )
	{
		return AddObjToGroup( L"ObjVideoAnims", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObj3DVideo( const std::wstring& obj_id, const SNULL* null, const std::wstring& move_efn_id, const std::wstring& frames_efn_id )
{
	if ( AddObj( obj_id, new QObj3DVideo( null, move_efn_id, frames_efn_id ), OT_3DVIDEO ) )
	{
		return AddObjToGroup( L"Obj3DVideos", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjMenuRukzak( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	if ( AddObj( obj_id, new QObjMenuRukzak( efn_id, tex_id, tex_id2 ), OT_MENURUKZAK ) )
	{
		return AddObjToGroup( L"MenuItems", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjMenuCursor( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& rukzak_obj_id )
{
	if ( AddObj( obj_id, new QObjMenuCursor( efn_id, tex_id, tex_id2, rukzak_obj_id ), OT_MENUCURSOR ) )
	{
		return AddObjToGroup( L"MenuItems", obj_id );
	}
	return false;
}

bool QMgrResObjects::AddObjMenuMenu( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	if ( AddObj( obj_id, new QObjMenuMenu( efn_id, tex_id, tex_id2 ), OT_MENUMENU ) )
	{
		return AddObjToGroup( L"MenuItems", obj_id );
	}
	return false;
}

void QMgrResObjects::RegisterObjectInLua( QObjBase* obj )
{
	if ( obj == NULL )
		return;

	std::wstring s = GetGame()->GetName() + L"OMgr." + obj->GetObjID() + L" = " + GetGame()->GetName() + L"ObjMgr:";
	switch( obj->GetType() )
	{
		case OT_MAN:
			s += L"GetObjMan( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_CAR:
			s += L"GetObjCar( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_BIRD:
			s += L"GetObjBird( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_FREE:
			s += L"GetObjFree( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_CAMERA:
			s += L"GetObjCam( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_LIGHT:
			s += L"GetObjLight( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_FLOOR:
			s += L"GetObjFloor( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_SHADOWVOLUME:
			s += L"GetObjSV( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_LAYERGEOM:
			s += L"GetObjLG( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_ACTION:
			s += L"GetObjAction( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_EFNANIM:
			s += L"GetObjEfnAnim( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_VIDEOANIM:
			s += L"GetObjVideoAnim( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_3DVIDEO:
			s += L"GetObj3DVideo( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_MENURUKZAK:
			s += L"GetObjMenuRukzak( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_MENUCURSOR:
			s += L"GetObjMenuCursor( L(\"" + obj->GetObjID() + L"\") );";
			break;

		case OT_MENUMENU:
			s += L"GetObjMenuMenu( L(\"" + obj->GetObjID() + L"\") );";
			break;

		default:
			g_Log.WriteError( L"cant find type to register in lua, in \"void QMgrResObjects::RegisterObjectInLua( QObjBase* obj )\"" );
			return;
	}
	g_Script.ExecScript( s );
}

void QMgrResObjects::DeRegisterObjectInLua( QObjBase* obj )
{
	if ( obj == NULL )
		return;

	std::wstring s = GetGame()->GetName() + L"OMgr." + obj->GetObjID() + L" = nil;";	
	g_Script.ExecScript(s);
}

void QMgrResObjects::DelObj( const std::wstring& obj_id )
{
	if ( m_mObj.size() > 0 )
	{
		objmap_iter i = m_mObj.find(obj_id);

		if ( i!=m_mObj.end() )
		{
			if(m_bLogEnabled)
			{
				g_Log.WriteWarning(L"Object deleted. obj_id=(" + i->first + L")");
			}
			QObjBase* obj = i->second;

			if ( obj )
			{
				obj->RemoveObjFromCollisionList();
				DeRegisterObjectInLua( obj );
				obj->Kill();
				SAFE_DELETE(obj);
			}
			m_mObj.erase(obj_id);
		}
	}
}

void QMgrResObjects::DelObjInGroup( const std::wstring& obj_group_id )
{

	//if(m_lObj.size()>0)
	//{
	//	for(objlist_iter i = m_lObj.begin(); i != m_lObj.end();)
	//	{
	//		if( (*i)->GetObjGroupID()==obj_group_id )
	//		{
	//			DelObj(i);
	//			i = m_lObj.erase(i);
	//		}
	//		else
	//		{
	//			++i;
	//		}
	//	}
	//}
}

inline QObjBase* QMgrResObjects::GetObj( const std::wstring& obj_id )
{
	objmap_iter i = m_mObj.find(obj_id);

	return ( ( i != m_mObj.end() ) ? i->second : NULL );
}


 
std::wstring QMgrResObjects::GetFirstFromGroup( const std::wstring& obj_group_id )
{
	mmapWStrWStr_iter i = m_mmGroup.find( obj_group_id );

	return ( ( i!=m_mmGroup.end() ) ? i->second : L"" );
}

void QMgrResObjects::GroupDoFrameMove( const std::wstring& obj_group_id, double fTime, float fElapsedTime )
{
	mmapWStrWStr_iter beg, end;

	if ( GetObjGroup( obj_group_id, &beg, &end ) > 0 )
	{
		QObjBase* obj = NULL;
		for( mmapWStrWStr_iter i = beg; i != end; ++i )
		{
			obj = GetObj( i->second );
				
			if ( obj )
				obj->OnFrameMove( fTime, fElapsedTime );
		}
	}
}

void QMgrResObjects::GroupDoFrameRender( const std::wstring& obj_group_id, double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	mmapWStrWStr_iter beg, end;

	if ( GetObjGroup( obj_group_id, &beg, &end ) > 0 )
	{
		QObjBase* obj = NULL;
		for( mmapWStrWStr_iter i = beg; i != end; i++ )
		{
			obj = GetObj( i->second );
				
			if ( obj )
				obj->OnFrameRender( fTime, fElapsedTime, ort );
		}
	}
}

void QMgrResObjects::GroupDoObjRender( const std::wstring& obj_group_id, DWORD rt )
{
	mmapWStrWStr_iter beg, end;

	if ( GetObjGroup( obj_group_id, &beg, &end ) > 0 )
	{
		QObjBase* obj = NULL;
		for( mmapWStrWStr_iter i = beg; i != end; i++ )
		{
			obj = GetObj( i->second );
				
			if ( obj )
				obj->ObjRender((RENDER_TYPES)rt);
		}
	}
}

void QMgrResObjects::AddObjToRenderBuffer( const std::wstring& obj_id )
{
	QObjBase* obj = GetObj( obj_id );
	
	if ( obj )
	{
		if ( obj->GetType() == OT_EFNANIM )
		{
			((QObjEfnAnim*)obj)->AddObjEfnAnimToRenderBuffer();
		}
		else if ( obj->GetEfnAnim() )
		{
			obj->GetEfnAnim()->AddObjEfnAnimToRenderBuffer();
		}
	}
}

void QMgrResObjects::AddGroupToRenderBuffer( const std::wstring& group_id )
{
	mmapWStrWStr_iter beg, end;

	int m_iCount = GetObjGroup( group_id, &beg, &end );

	if ( m_iCount == 0 )
		return;
	
	for( mmapWStrWStr_iter i = beg; i != end; i++ )
	{
		AddObjToRenderBuffer( i->second );
	}
}


void QMgrResObjects::CalcZSortedRenderBuffer( const D3DXVECTOR3* pvCamPos )
{
	m_mapZSortedRenderBuffer.clear();

	if ( m_lstRenderBuffer.size() == 0 || pvCamPos == NULL )
		return;

	D3DXVECTOR3 m_vCamPos = *pvCamPos;
	float m_fDist = 0.0f;
	
	for( listQObjBase_iter i = m_lstRenderBuffer.begin(); i != m_lstRenderBuffer.end(); i++ )
	{
		if ( *i )
		{
			m_fDist = D3DXVec3LengthSq( &( (*i)->GetNullPos()->vPos - m_vCamPos ) );
			m_mapZSortedRenderBuffer.insert( mmapFloatQObjBase_iter::value_type( m_fDist, (*i) ) );
		}
	}
}

void QMgrResObjects::RenderFromZSortedRenderBuffer( double fTime, float fElapsedTime )
{
	if ( m_mapZSortedRenderBuffer.size() > 0 )
	{
		QObjBase* obj = NULL;

		//Сначала рисуем все непрозрачные спереди назад
		mmapFloatQObjBase_iter i;
		for( i = m_mapZSortedRenderBuffer.begin(); i != m_mapZSortedRenderBuffer.end(); i++ )
		{
			obj = i->second;

			if(obj)
				obj->OnFrameRender( fTime, fElapsedTime, ORT_NONTRANSPARENT );
		}

		//Затем все прозрачные сзади вперед
		i = m_mapZSortedRenderBuffer.end();
		while ( i != m_mapZSortedRenderBuffer.begin() )
		{
			i--;

			obj = i->second;

			if(obj)
				obj->OnFrameRender( fTime, fElapsedTime, ORT_TRANSPARENT );
		}
		
		//Заполняем Z буффер для объектов которые этого не сделали, т.е. для сложных полупрозрачных
		i = m_mapZSortedRenderBuffer.end();
		while ( i != m_mapZSortedRenderBuffer.begin() )
		{
			i--;

			obj = i->second;

			if(obj)
				obj->OnFrameRender( fTime, fElapsedTime, ORT_FILLZFORTRANSPARENTWITHOUTZ );
		}
	}
}

void QMgrResObjects::ObjRenderFromZSortedRenderBuffer( DWORD rt )
{
	QObjBase* obj = NULL;

	for( mmapFloatQObjBase_iter i = m_mapZSortedRenderBuffer.begin(); i != m_mapZSortedRenderBuffer.end(); i++ )
	{
		obj = i->second;

		if(obj)
			obj->ObjRender( (RENDER_TYPES)rt );
	}
}

std::wstring QMgrResObjects::GetIntersectedObjID( const std::wstring& obj_group_id, const SRay* pstRay )
{
	std::wstring m_sTemp = L"";
	float m_fDist = 999999999999.0f;
	float m_fTempDist;
	D3DXVECTOR3 m_vIntrPos;
	D3DXVECTOR3 m_vTempIntrPos;

	mmapWStrWStr_iter beg, end;

	if ( GetObjGroup( obj_group_id, &beg, &end ) > 0 )
	{
		QObjBase* obj = NULL;
		for( mmapWStrWStr_iter i = beg; i != end; ++i )
		{
			obj = GetObj( i->second );
				
			if ( obj )
			{
				if (obj->CheckForRayIntersect( pstRay, &m_fTempDist, &m_vTempIntrPos ))
				{
					if ( m_fTempDist < m_fDist )
					{
						m_sTemp = obj->GetObjID();
						m_vIntrPos = m_vTempIntrPos;
						m_fDist = m_fTempDist;
					}
				}
			}
		}
	}

	if ( m_sTemp!=L"" )
	{
		ZeroMemory( &m_stIntersectedPos, sizeof(SNULL) );
		m_stIntersectedPos.vPos = m_vIntrPos;
		m_stIntersectedPos.vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		m_fIntersectedDist = m_fDist;
	}
	
	return m_sTemp;
}

bool QMgrResObjects::CheckForRayIntersect( const std::wstring& obj_id, const SRay* pvRay )
{
	QObjBase* temp = GetObj( obj_id );
				
	if ( temp )
	{
		float		m_fTempDist;
		D3DXVECTOR3	m_vTempIntrPos;

		if ( temp->CheckForRayIntersect( pvRay, &m_fTempDist, &m_vTempIntrPos ) )
		{
			ZeroMemory( &m_stIntersectedPos, sizeof(SNULL) );
			m_stIntersectedPos.vPos = m_vTempIntrPos;
			m_stIntersectedPos.vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

			m_fIntersectedDist = m_fTempDist;

			return true;
		}
	}
	return false;
}

std::wstring QMgrResObjects::GetScript( const std::wstring& obj_id )
{
	QObjBase* obj = GetObj( obj_id );
	return ( ( obj != NULL ) ? obj->GetScript() : L"" );
}

bool QMgrResObjects::GetEnabled( const std::wstring& obj_id )
{
	QObjBase* obj = GetObj( obj_id );
	return ( ( obj != NULL ) ?  obj->GetEnabled() : false );
}

void QMgrResObjects::SetEnabled( const std::wstring& obj_id, bool bEnabled )
{
	QObjBase* obj = GetObj( obj_id );
	
	if ( obj )
		return obj->SetEnabled( bEnabled );
}

bool QMgrResObjects::AddObjToGroup( const std::wstring& obj_group_id, const std::wstring& obj_id )
{
	m_mmGroup.insert( mmapWStrWStr::value_type( obj_group_id, obj_id ) );
	
	if ( m_bLogEnabled )
	{
		g_Log.WriteWarning(L"Object \"" + obj_id + L"\" added to group \"" + obj_group_id + L"\"");
	}

	return true;
}

int QMgrResObjects::GetObjGroup( const std::wstring& obj_group_id, mmapWStrWStr_iter* beg, mmapWStrWStr_iter* end)
{
	if( m_mmGroup.size()>0 )
	{
		int m_iObjCount = m_mmGroup.count(obj_group_id);

		if ( m_iObjCount > 0 )
		{
			*beg = m_mmGroup.lower_bound(obj_group_id);
			*end = m_mmGroup.upper_bound(obj_group_id);
			return m_iObjCount;
		}
	}
	return 0;
}

void QMgrResObjects::OnCreateDevice()
{
	if( !m_mObj.empty() )
	{
		QObjBase* obj = NULL;

		for( objmap_iter i = m_mObj.begin(); i != m_mObj.end(); i++ )
		{
			obj = i->second;

			if ( obj )
				obj->OnCreateDevice();
		}
	}
}

void QMgrResObjects::OnResetDevice()
{
	if( !m_mObj.empty() )
	{
		QObjBase* obj = NULL;

		for( objmap_iter i = m_mObj.begin(); i != m_mObj.end(); i++ )
		{
			obj = i->second;

			if ( obj )
				obj->OnResetDevice();
		}
	}
}

void QMgrResObjects::OnLostDevice()
{
	if( !m_mObj.empty() )
	{
		QObjBase* obj = NULL;

		for( objmap_iter i = m_mObj.begin(); i != m_mObj.end(); i++ )
		{
			obj = i->second;

			if ( obj )
				obj->OnLostDevice();
		}
	}
}

void QMgrResObjects::OnDestroyDevice()
{
	if( !m_mObj.empty() )
	{
		QObjBase* obj = NULL;

		for( objmap_iter i = m_mObj.begin(); i != m_mObj.end(); i++ )
		{
			obj = i->second;

			if ( obj )
				obj->OnDestroyDevice();
		}
	}
}