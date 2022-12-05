#define STRICT

#include "QSubSysGraphics.h"

#include "QObjBase.h"

#include "QGameBase.h"
#include "QObjEfnAnim.h"

std::wstring QObjBase::GetTypeStr()
{
	switch(m_eType)
	{
		case OT_MAN:
			return L"Man";
			break;

		case OT_CAR:
			return L"Car";
			break;

		case OT_BIRD:
			return L"Bird";
			break;

		case OT_FREE:
			return L"FreeObj";
			break;

		case OT_CAMERA:
			return L"Camera";
			break;

		case OT_LIGHT:
			return L"Light";
			break;

		case OT_FLOOR:
			return L"Floor";
			break;

		case OT_SHADOWVOLUME:
			return L"ShadowVolume";
			break;

		case OT_LAYERGEOM:
			return L"LayerGeom";
			break;

		case OT_ACTION:
			return L"ObjAction";
			break;

		case OT_BACK:
			return L"Background";
			break;

		case OT_SCENE:
			return L"Scene";
			break;

		case OT_REFLECTION:
			return L"Reflection";
			break;
	
		case OT_CAMSELECT:
			return L"CameraSelect";
			break;

		case OT_EFNANIM:
			return L"ObjEfnAnim";
			break;

		case OT_VIDEOANIM:
			return L"ObjVideoAnim";
			break;

		case OT_3DVIDEO:
			return L"Obj3DVideo";
			break;

		case OT_MENURUKZAK:
			return L"ObjMenuRukzak";
			break;

		case OT_MENUCURSOR:
			return L"ObjMenuCursor";
			break;

		case OT_MENUMENU:
			return L"ObjMenuMenu";
			break;
	}
	return L"";
}

bool QObjBase::IsAnimStarted()
{
	return ( GetEfnAnim() ? GetEfnAnim()->IsStarted() : false );
}

void QObjBase::SetEnabled( bool bEnabled )
{
	m_bEnabled = bEnabled;

	if ( GetEfnAnim() )
		GetEfnAnim()->SetEfnAnimEnabled( bEnabled );
}

void QObjBase::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	if ( GetEfnAnim() )
		GetEfnAnim()->SetEfnAnimVisible( bVisible );
}

void QObjBase::GoEfnAnim( double fTime, double fPathLength, int seq_id )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->Go( fTime, fPathLength, seq_id );
}

void QObjBase::StopEfnAnim( bool bOK )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->Stop( bOK );
}

void QObjBase::AddObjToCollisionList()
{
	if ( GetGame() )
		GetGame()->SetActiveObj( GetObjID() );
}

void QObjBase::RemoveObjFromCollisionList()
{
	if ( GetGame() )
		GetGame()->DelActiveObj( GetObjID() );
}