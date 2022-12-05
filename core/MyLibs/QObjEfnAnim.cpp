#define STRICT

#include "QObjEfnAnim.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"
#include "QMath.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResEfnModels.h"
#include "IMgrResTextures.h"
#include "QMgrResStates.h"
#include "QMgrResSounds.h"
#include "QMgrResObjects.h"
#include "QMgrScripts.h"

QObjEfnAnim::QObjEfnAnim( const std::wstring& efn_id, const std::wstring& meshs_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 )
{
	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	
	SetEfnID( efn_id );
	SetEfnID2( meshs_efn_id );
	SetTexID( tex_id );
	SetTexID2( tex_id2 );
	m_sDefaultEfnID		= ( GetEfnID2() != L"" ? GetEfnID2() + L".Morf_Base" : GetEfnID() );
	m_bTexSphereMapping	= false;
	m_bTex2SphereMapping= false;

	m_bStarted			= false;
	EfnAnimTemp			= NULL;

	m_iFrame			= 0;
	m_iNextFrame		= 0;
	m_fTweenFactor		= 0.0f;

	m_bRenderRoot		= true;
	m_bRenderChilds		= false;
	m_bCalcChildsInOnFrameMove = false;
	m_bFillZBuffer		= true;
	m_bSendAnimsToChilds= false;
	m_bZeroAnim			= true;
	m_bCullNone			= false;

	m_iSeqID			= -1;

	vecAnim.clear();
	m_mmObjAttaches.clear();

	SetCoords( &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &D3DXVECTOR3( 1.0f, 1.0f, 1.0f ),  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
}

QObjEfnAnim::~QObjEfnAnim()
{
}

void QObjEfnAnim::Init()
{
	Stop(false);

	g_EfnMgr.LoadEfn( GetEfnID() );

	if ( GetEfnID2() != L"" )
	{
		g_EfnMgr.LoadEfn( GetEfnID2() );

		g_EfnMgr.MakeMorf( GetEfnID2() + L".ResultMesh", GetEfnID(), GetEfnID2(), 0, 0, 0 );
	}

	if ( GetTexID() != L"" )
		g_TexMgr.LoadTex( GetTexID() );

	if ( GetTexID2() != L"" )
		g_TexMgr.LoadTex( GetTexID2() );

	SetMtrl( g_EfnMgr.GetEfnMtrl( m_sDefaultEfnID ) );

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjEfnAnim::Kill()
{
	Stop(false);
	DeAttachAll();

	OnLostDevice();
	OnDestroyDevice();
}

void QObjEfnAnim::AnimAdd( const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes)
{
	AnimAdd2( L"", sAnimName, fAnimRotSpeed, iAnimRepeatTimes );	
}

void QObjEfnAnim::AnimAdd2( const std::wstring& efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes)
{
	AnimAdd3( efn_id, sAnimName, fAnimRotSpeed, iAnimRepeatTimes, L"" );
}

void QObjEfnAnim::AnimAdd3( const std::wstring& efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes, const std::wstring& sScript )
{
	SEfnAnimSeq AnimTemp;

	AnimTemp.iType					= 1;
	AnimTemp.sEfnID					= (efn_id==L"") ? GetEfnID() : efn_id;
	AnimTemp.sMeshesEfnID			= L"";
	AnimTemp.sAnimName				= sAnimName;
	AnimTemp.fAnimFrameRate			= g_EfnMgr.GetAnimFrameRate( AnimTemp.sEfnID );

	AnimTemp.iAnimFramesCount		= g_EfnMgr.GetAnimFramesCount( AnimTemp.sEfnID, sAnimName );
	AnimTemp.iAnimRepeatTimes		= iAnimRepeatTimes;
	AnimTemp.fAnimRotSpeed			= fAnimRotSpeed;

	AnimTemp.fAnimTimeLength		= g_EfnMgr.GetAnimTimeLength( AnimTemp.sEfnID, sAnimName );
	AnimTemp.fAnimTotalTimeLength	= AnimTemp.fAnimTimeLength * iAnimRepeatTimes;

	AnimTemp.fAnimZLength			= g_EfnMgr.GetAnimZLength( AnimTemp.sEfnID, sAnimName );
	AnimTemp.fAnimTotalZLength		= AnimTemp.fAnimZLength * iAnimRepeatTimes;

	AnimTemp.sScript				= sScript;
	AnimTemp.bScriptExecuted		= false;

	AnimTemp.fAnimGoZ			= ( ( vecAnim.size()==0 ) ? 0.0 : vecAnim[vecAnim.size()-1].fAnimGoZ + vecAnim[vecAnim.size()-1].fAnimTotalZLength );

	vecAnim.push_back(AnimTemp);

	if ( m_bSendAnimsToChilds )
	{
		if ( m_mmObjAttaches.size() > 0 )
		{
			for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
			{
				if ( i->second.pObjEfnAnim )
					i->second.pObjEfnAnim->AnimAdd3( efn_id, sAnimName, fAnimRotSpeed, iAnimRepeatTimes, L"" );
			}
		}
	}
}

void QObjEfnAnim::MorfAnimAdd( const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes )
{
	MorfAnimAdd2( L"", L"", sAnimName, fAnimRotSpeed, iAnimRepeatTimes );
}

void QObjEfnAnim::MorfAnimAdd2( const std::wstring& efn_id, const std::wstring& meshs_efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes )
{
	MorfAnimAdd3( efn_id, meshs_efn_id, sAnimName, fAnimRotSpeed, iAnimRepeatTimes, L"" );
}

void QObjEfnAnim::MorfAnimAdd3( const std::wstring& efn_id, const std::wstring& meshs_efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes, const std::wstring& sScript )
{
	SEfnAnimSeq AnimTemp;

	AnimTemp.iType					= 2;
	AnimTemp.sEfnID					= (efn_id==L"") ? GetEfnID() : efn_id;
	AnimTemp.sMeshesEfnID			= (meshs_efn_id==L"") ? GetEfnID2() : meshs_efn_id;
	AnimTemp.sAnimName				= sAnimName;
	AnimTemp.fAnimFrameRate			= g_EfnMgr.GetAnimFrameRate( AnimTemp.sEfnID );

	AnimTemp.iAnimFramesCount		= g_EfnMgr.GetAnimFramesCount( AnimTemp.sEfnID, sAnimName );
	AnimTemp.iAnimRepeatTimes		= iAnimRepeatTimes;
	AnimTemp.fAnimRotSpeed			= fAnimRotSpeed;

	AnimTemp.fAnimTimeLength		= g_EfnMgr.GetAnimTimeLength( AnimTemp.sEfnID, sAnimName );
	AnimTemp.fAnimTotalTimeLength	= AnimTemp.fAnimTimeLength * iAnimRepeatTimes;

	AnimTemp.fAnimZLength			= g_EfnMgr.GetAnimZLength( AnimTemp.sEfnID, sAnimName );
	AnimTemp.fAnimTotalZLength		= AnimTemp.fAnimZLength * iAnimRepeatTimes;

	AnimTemp.sScript				= sScript;
	AnimTemp.bScriptExecuted		= false;

	AnimTemp.fAnimGoZ			= ( ( vecAnim.size()==0 ) ? 0.0 : vecAnim[vecAnim.size()-1].fAnimGoZ + vecAnim[vecAnim.size()-1].fAnimTotalZLength );

	vecAnim.push_back(AnimTemp);

	if ( m_bSendAnimsToChilds )
	{
		if ( m_mmObjAttaches.size() > 0 )
		{
			for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
			{
				if ( i->second.pObjEfnAnim )
					i->second.pObjEfnAnim->MorfAnimAdd3( efn_id, meshs_efn_id, sAnimName, fAnimRotSpeed, iAnimRepeatTimes, L"" );
			}
		}
	}
}

void QObjEfnAnim::Go( double fTime, double fPathLength, int seq_id )
{
	m_fOffsetOnPath		= 0.0;
	m_fAnimSliderKoef	= 0.0;
	m_fPathLength		= fPathLength;
	EfnAnimTemp			= NULL;
	m_iSeqID			= seq_id;
	m_bStarted			= true;
	m_iAnimI			= 0;
	m_fMorfLastTime		= fTime - 10.0;

	if ( vecAnim.size() > 0 )
	{
		vecAnim[0].fAnimGoTime = fTime;
		
		for( UINT i = 1; i < vecAnim.size(); i++)
		{
			vecAnim[i].fAnimGoTime = vecAnim[i-1].fAnimGoTime + vecAnim[i-1].fAnimTotalTimeLength;
		}

		double m_fOffsetForAnim = vecAnim[vecAnim.size()-1].fAnimGoZ + vecAnim[vecAnim.size()-1].fAnimTotalZLength;

		m_fAnimSliderKoef = ( ( m_fOffsetForAnim != 0.0 ) ? m_fPathLength / m_fOffsetForAnim : 0.0 );
	}

	if ( m_bSendAnimsToChilds )
	{
		if ( m_mmObjAttaches.size() > 0 )
		{
			for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
			{
				if ( i->second.pObjEfnAnim )
					i->second.pObjEfnAnim->Go( fTime, fPathLength, -1 );
			}
		}
	}
}

void QObjEfnAnim::Stop( bool bOK )
{
	if ( m_bSendAnimsToChilds )
	{
		if ( m_mmObjAttaches.size() > 0 )
		{
			for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
			{
				if ( i->second.pObjEfnAnim )
					i->second.pObjEfnAnim->Stop( bOK );
			}
		}
	}

	vecAnim.clear();

	m_bStarted			= false;
	m_fOffsetOnPath		= 0.0;
	m_fAnimSliderKoef	= 0.0;
	m_fPathLength		= 0.0;
	EfnAnimTemp			= NULL;

	ClearPathLastEvents();

	if ( m_iSeqID != -1 )
	{
		int iTemp	= m_iSeqID;
		m_iSeqID	= -1;

		if ( bOK )
		{
			g_StatesMgr.SeqIncState(iTemp);
		}
		else
		{
			g_StatesMgr.SeqZeroState(iTemp);
		}
	}
}

bool QObjEfnAnim::AttachObjToNull( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id )
{
	return AttachObjToNullC( null_id, obj_id, efn_id, tex_id, tex_id2, attach_null_id, NULL );
}

bool QObjEfnAnim::AttachObjToNullC( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id, const D3DXCOLOR* pstColor )
{
	SObjNullAttaches temp;
	temp.obj_id		= obj_id;
	temp.null_id	= attach_null_id;
	temp.bModifyNull= false;
	ZeroMemory( &temp.matModify, sizeof(D3DXMATRIX) );

	if ( GetObjMgr()->AddObjEfnAnim( obj_id, efn_id, tex_id, tex_id2 ) )
	{
		temp.pObjEfnAnim = GetObjMgr()->GetObjEfnAnim( obj_id );
		if ( pstColor )
			temp.pObjEfnAnim->SetMtrlColor( pstColor );
		m_mmObjAttaches.insert( mmapObjNullAttaches::value_type( null_id, temp ) );

		CalcChildsObjEfnAnimCoords();

		return true;
	}
	else
	{
		return false;
	}
}

bool QObjEfnAnim::AttachObjToNullE( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id )
{
	return AttachObjToNullEC( null_id, obj_id, efn_id, meshes_efn_id, tex_id, tex_id2, attach_null_id, NULL );
}

bool QObjEfnAnim::AttachObjToNullEC( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id, const D3DXCOLOR* pstColor )
{
	SObjNullAttaches temp;
	temp.obj_id		= obj_id;
	temp.null_id	= attach_null_id;
	temp.bModifyNull= false;
	ZeroMemory( &temp.matModify, sizeof(D3DXMATRIX) );

	if ( GetObjMgr()->AddObjEfnAnimE( obj_id, efn_id, meshes_efn_id, tex_id, tex_id2 ) )
	{
		temp.pObjEfnAnim = GetObjMgr()->GetObjEfnAnim( obj_id );
		if ( pstColor )
			temp.pObjEfnAnim->SetMtrlColor( pstColor );
		m_mmObjAttaches.insert( mmapObjNullAttaches::value_type( null_id, temp ) );

		CalcChildsObjEfnAnimCoords();

		return true;
	}
	else
	{
		return false;
	}
}

void QObjEfnAnim::DeAttachObjFromNull( const std::wstring& null_id, const std::wstring& obj_id )
{
	if ( m_mmObjAttaches.count( null_id ) > 0 )
	{
		mmapObjNullAttaches_iter beg = m_mmObjAttaches.lower_bound( null_id );
		mmapObjNullAttaches_iter end = m_mmObjAttaches.upper_bound( null_id );
		for ( mmapObjNullAttaches_iter i = beg; i != end; )
		{
			if ( i->second.obj_id == obj_id )
			{
				GetObjMgr()->DelObj( i->second.obj_id );
				i = m_mmObjAttaches.erase( i );
			}
			else
			{
				i++;
			}
		}
	}
}

void QObjEfnAnim::DeAttachObjsFromNull( const std::wstring& null_id )
{
	if ( m_mmObjAttaches.count( null_id ) > 0 )
	{
		mmapObjNullAttaches_iter beg = m_mmObjAttaches.lower_bound( null_id );
		mmapObjNullAttaches_iter end = m_mmObjAttaches.upper_bound( null_id );
		for ( mmapObjNullAttaches_iter i = beg; i != end; i++ )
		{
			GetObjMgr()->DelObj( i->second.obj_id );
		}
		m_mmObjAttaches.erase( null_id );
	}
}

void QObjEfnAnim::DeAttachAll()
{
	for ( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
	{
		GetObjMgr()->DelObj( i->second.obj_id );
	}
	m_mmObjAttaches.clear();
}

void QObjEfnAnim::SetNullModifyMatrix( const std::wstring& null_id, const D3DXMATRIX* pmatModify )
{
	if ( m_mmObjAttaches.count( null_id ) > 0 )
	{
		mmapObjNullAttaches_iter beg = m_mmObjAttaches.lower_bound( null_id );
		mmapObjNullAttaches_iter end = m_mmObjAttaches.upper_bound( null_id );
		SObjNullAttaches* temp = NULL;
		for ( mmapObjNullAttaches_iter i = beg; i != end; i++ )
		{
			temp = &i->second;

			if ( temp )
			{
				if ( pmatModify != NULL )
				{
					temp->bModifyNull	= true;
					temp->matModify		= *pmatModify;
				}
				else
				{
					temp->bModifyNull	= false;
				}
			}
		}
	}
}

void QObjEfnAnim::SetModifyMatrix( const std::wstring& null_id, const std::wstring& obj_id, const D3DXMATRIX* pmatModify )
{
	if ( m_mmObjAttaches.count( null_id ) > 0 )
	{
		mmapObjNullAttaches_iter beg = m_mmObjAttaches.lower_bound( null_id );
		mmapObjNullAttaches_iter end = m_mmObjAttaches.upper_bound( null_id );
		SObjNullAttaches* temp = NULL;
		for ( mmapObjNullAttaches_iter i = beg; i != end; i++ )
		{
			temp = &i->second;

			if ( temp )
			{
				if ( temp->obj_id == obj_id )
				{
					if ( pmatModify != NULL )
					{
						temp->bModifyNull	= true;
						temp->matModify		= *pmatModify;
					}
					else
					{
						temp->bModifyNull	= false;
					}
					return;
				}
			}
		}
	}
}

void QObjEfnAnim::ClearModifyFlag()
{
	if ( m_mmObjAttaches.size() > 0 )
	{
		for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
		{
			i->second.bModifyNull = false;
		}
	}
}


void QObjEfnAnim::AddObjEfnAnimToRenderBuffer()
{
	GetObjMgr()->GetRenderBuffer()->push_back( this );

	if ( m_mmObjAttaches.size() > 0 )
	{
		for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
		{
			if ( i->second.pObjEfnAnim )
				i->second.pObjEfnAnim->AddObjEfnAnimToRenderBuffer();
		}
	}
}

void QObjEfnAnim::SetPos( const SNULL* pstNull )
{
	SetNullPos( pstNull );

	D3DXMatrixRotationX( &matRotX, GetNullPos()->vRot.x );
	D3DXMatrixRotationY( &matRotY, GetNullPos()->vRot.y );
	D3DXMatrixRotationZ( &matRotZ, GetNullPos()->vRot.z );
	D3DXMatrixTranslation( &matTrans, GetNullPos()->vPos.x, GetNullPos()->vPos.y, GetNullPos()->vPos.z );
	D3DXMatrixScaling( &matScale, GetNullPos()->vScale.x, GetNullPos()->vScale.y, GetNullPos()->vScale.z );

	matRot = matRotX * matRotY * matRotZ;

	matWorld = matScale * matRot * matTrans;

	CalcChildsObjEfnAnimCoords();
}

void QObjEfnAnim::SetCoords( const D3DXVECTOR3* pvPos, const D3DXVECTOR3* pvScale, const D3DXVECTOR3* pvRot )
{
	GetNullPos()->vPos		= *pvPos;
	GetNullPos()->vScale	= *pvScale;
	GetNullPos()->vRot		= *pvRot;

	SetPos( GetNullPos() );
}

void QObjEfnAnim::SetPosition( const D3DXVECTOR3* pvPos )
{
	GetNullPos()->vPos	= *pvPos;

	D3DXMatrixTranslation( &matTrans, GetNullPos()->vPos.x, GetNullPos()->vPos.y, GetNullPos()->vPos.z );

	matWorld = matScale * matRot * matTrans;

	CalcChildsObjEfnAnimCoords();
}

void QObjEfnAnim::SetScale( const D3DXVECTOR3* pvScale )
{
	GetNullPos()->vScale	= *pvScale;

	D3DXMatrixScaling( &matScale, GetNullPos()->vScale.x, GetNullPos()->vScale.y, GetNullPos()->vScale.z );

	matWorld = matScale * matRot * matTrans;

	CalcChildsObjEfnAnimCoords();
}

void QObjEfnAnim::SetRotation( const D3DXVECTOR3* pvRot )
{
	GetNullPos()->vRot		= *pvRot;

	D3DXMatrixRotationX( &matRotX, GetNullPos()->vRot.x );
	D3DXMatrixRotationY( &matRotY, GetNullPos()->vRot.y );
	D3DXMatrixRotationZ( &matRotZ, GetNullPos()->vRot.z );

	matRot = matRotX * matRotY * matRotZ;

	matWorld = matScale * matRot * matTrans;

	CalcChildsObjEfnAnimCoords();
}

void QObjEfnAnim::SetWorld( const D3DXMATRIX* pmatWorld )
{
	matWorld = *pmatWorld;

	D3DXVec3TransformCoord( &GetNullPos()->vPos, &D3DXVECTOR3(0.0f,0.0f,0.0f), &matWorld );

	CalcChildsObjEfnAnimCoords();
};

void QObjEfnAnim::CalcChildsObjEfnAnimCoords()
{
	if ( m_mmObjAttaches.size() > 0 )
	{
		D3DXMATRIX matRes;
		D3DXMATRIX matNull;
		D3DXMATRIX matAttachNull;

		for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
		{
			if ( i->second.pObjEfnAnim )
			{
															//имя нуля
				if ( g_EfnMgr.GetEfnNullMatrix( GetAnimEfnID(), i->first, m_iFrame, m_iNextFrame, m_fTweenFactor, &matNull ) )
				{
					if ( i->second.bModifyNull )
						matNull = i->second.matModify * matNull;

					if ( i->second.null_id != L"" )
					{
						if ( g_EfnMgr.GetEfnNullMatrix( i->second.pObjEfnAnim->GetAnimEfnID(), i->second.null_id, 0, 0, 0.0f, &matAttachNull ) )
						{
							matRes = matNull * matAttachNull * matWorld;
						}
					}
					else
					{
						matRes = matNull * matWorld;
					}

					i->second.pObjEfnAnim->SetWorld( &matRes );
				}
			}
		}
	}
}

void QObjEfnAnim::SetEfnAnimEnabled( bool bEnabled )
{
	this->SetEnabled( bEnabled );

	for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
	{
		if ( i->second.pObjEfnAnim )
			i->second.pObjEfnAnim->SetEfnAnimEnabled( bEnabled );
	}
}

void QObjEfnAnim::SetEfnAnimVisible( bool bVisible )
{
	this->SetVisible( bVisible );

	for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
	{
		if ( i->second.pObjEfnAnim )
			i->second.pObjEfnAnim->SetEfnAnimVisible( bVisible );
	}
}

std::wstring QObjEfnAnim::GetFirstFreeNullID()
{
	listWStr temp;
	g_EfnMgr.GetEfnNullList( m_sDefaultEfnID, &temp );
	
	std::wstring s = L"";
	for( listWStr::iterator i = temp.begin(); i != temp.end(); i++ )
	{
		if ( m_mmObjAttaches.find( *i ) == m_mmObjAttaches.end() )
			return *i;
	}

	return L"";
}

std::wstring QObjEfnAnim::GetNullIDForAttachedObj( const std::wstring& obj_id )
{
	if ( m_mmObjAttaches.size() > 0 )
	{
		SObjNullAttaches* temp = NULL;
		for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
		{
			if ( i->second.obj_id == obj_id )
				return i->first;
		}
	}
	return L"";
}

void QObjEfnAnim::AddPathLastEvent( double fPathLast, const std::wstring& sScript )
{
	SPathLastEvent temp;
	temp.fPathLast	= fPathLast;
	temp.sScript	= sScript;

	m_lstPathLastEvents.push_back( temp );
}

void QObjEfnAnim::DoPathLastEvents()
{
	if ( m_lstPathLastEvents.size() > 0 )
	{
		for( listSPathLastEvent_iter i = m_lstPathLastEvents.begin(); i != m_lstPathLastEvents.end(); )
		{
			if ( GetPathLast() < i->fPathLast )
			{
				g_Script.ExecScript( i->sScript );
				i = m_lstPathLastEvents.erase( i );
			}
			else
			{
				i++;
			}
		}
	}
}

void QObjEfnAnim::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( GetEnabled()==false )
		return;

	EfnAnimTemp = NULL;

	//нахождение текущей анимации
	if ( vecAnim.size() > 0 && m_bStarted ) 
	{
		for( UINT i = 0; i <= vecAnim.size() && !EfnAnimTemp; i++ )
		{
			if ( i > 0 )
			{
				if ( vecAnim[i-1].bScriptExecuted == false )
				{
					vecAnim[i-1].bScriptExecuted = true;
					g_Script.ExecScript( vecAnim[i-1].sScript );
				}
			}
			if ( i < vecAnim.size() )
			{
				if( vecAnim[i].iAnimRepeatTimes == -1 )
				{
					EfnAnimTemp = &vecAnim[i];
					m_iAnimI = i;
				}
				else if ( (vecAnim[i].fAnimGoTime <= fTime) && (fTime < vecAnim[i].fAnimGoTime+vecAnim[i].fAnimTotalTimeLength) )
				{
					EfnAnimTemp = &vecAnim[i];
					m_iAnimI = i;
				}
			}
		}

		if ( !EfnAnimTemp )
		{
			Stop( true );
		}
	}

	if ( EfnAnimTemp )
	{
		m_fAnimFrameIndex = (fTime - EfnAnimTemp->fAnimGoTime) * EfnAnimTemp->fAnimFrameRate;

		int m_iAnimProshlo = (int) (m_fAnimFrameIndex / EfnAnimTemp->iAnimFramesCount);

		m_fAnimFrameIndex -= m_iAnimProshlo * EfnAnimTemp->iAnimFramesCount;

		//получаем кадры для рендера из индекса анимации
		g_EfnMgr.GetAnimFrame( EfnAnimTemp->sEfnID, EfnAnimTemp->sAnimName, m_fAnimFrameIndex, &m_iFrame, &m_iNextFrame, &m_fTweenFactor );

		//m_sDefaultEfnID = EfnAnimTemp->sEfnID;

		//Играем звук eсли переключился кадр
		if ( m_iFrameSoundPlaying != m_iFrame )
		{
			m_sSoundScript = g_EfnMgr.GetFrameScript( EfnAnimTemp->sEfnID, m_iFrame );

			m_iFrameSoundPlaying = m_iFrame;
			if ( m_sSoundScript.length() != 0 )
			{
				g_SndMgr.PlaySnd( m_sSoundScript );
			}
		}

		if ( EfnAnimTemp->iType == 1 )
		{
			//Выравнивание кадров между анимациями
			if ( EfnAnimTemp->iAnimRepeatTimes != -1 )
			{
				//если это последний кадр анимации 
				if ( m_iAnimProshlo == EfnAnimTemp->iAnimRepeatTimes-1 && 
					(int)m_fAnimFrameIndex == EfnAnimTemp->iAnimFramesCount-1 )
				{
					// если последняя
					if ( m_iAnimI == vecAnim.size()-1 )
					{
						m_iNextFrame = m_iFrame;
					}
					else
					{
						//если анимация не последняя то взять первый кадр из следующей анимации если она типа == 1
						if ( vecAnim[m_iAnimI+1].iType == 1 )
						{
							g_EfnMgr.GetAnimFrame( vecAnim[m_iAnimI+1].sEfnID, vecAnim[m_iAnimI+1].sAnimName, 0.0f, &m_iNextFrame, NULL, NULL );
						}
						else
						{
							//Иначе сделать следующим текущий
							m_iNextFrame = m_iFrame;
						}
					}
				}
			}
		}
		else // ( EfnAnimTemp->iType == 2 )
		{
			if ( fTime > m_fMorfLastTime + 0.02 )
			{
				m_fMorfLastTime = fTime;
				g_EfnMgr.MakeMorf( EfnAnimTemp->sMeshesEfnID + L".ResultMesh", EfnAnimTemp->sEfnID, EfnAnimTemp->sMeshesEfnID, m_iFrame, m_iNextFrame, m_fTweenFactor );
			}
			//m_sDefaultEfnID = EfnAnimTemp->sMeshesEfnID + L".ResultMesh";
		}

		//Нахождение смещения от времени
		m_fMyTimeElapsed = (fTime - EfnAnimTemp->fAnimGoTime) / EfnAnimTemp->fAnimTimeLength;
		m_fOffsetOnPath = EfnAnimTemp->fAnimZLength * (int)m_fMyTimeElapsed;
		m_fFrame = ( m_fMyTimeElapsed - (int)m_fMyTimeElapsed ) * EfnAnimTemp->iAnimFramesCount;

		QEfnDataListItem* efn =  g_EfnMgr.GetEfn( EfnAnimTemp->sEfnID );
		EFN_CHARACTERANIMATION* temp = g_EfnMgr.GetAnim( EfnAnimTemp->sEfnID, EfnAnimTemp->sAnimName );

		if ( efn != NULL && temp != NULL )
		{
			for( int i = 0; i < (int)m_fFrame; i++)
			{
				m_fOffsetOnPath += g_EfnMgr.GetZOffset( efn, temp->frames_list[i]);
			}
			m_fOffsetOnPath += g_EfnMgr.GetZOffset( efn, temp->frames_list[(int)m_fFrame] ) * (m_fFrame - (int)m_fFrame);
		}

		m_fOffsetOnPath += EfnAnimTemp->fAnimGoZ;
		m_fOffsetOnPath *= m_fAnimSliderKoef;

		DoPathLastEvents();
	}
	else
	{
		if ( m_bZeroAnim )
		{
			m_fFrame = 0.0;
			m_iFrame = m_iNextFrame = 0;
			m_fTweenFactor = 0.0f;
		}
	}

	if ( m_bCalcChildsInOnFrameMove )
	{
		CalcChildsObjEfnAnimCoords();
	}

	if ( m_mmObjAttaches.size() > 0 )
	{
		for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
		{
			if ( i->second.pObjEfnAnim )
			{
				i->second.pObjEfnAnim->OnFrameMove( fTime, fElapsedTime );
			}
		}
	}
}

void QObjEfnAnim::OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort )
{
	if ( GetVisible()==false )
		return;

	if ( m_bRenderRoot )
	{
		if ( ( GetMtrl()->Diffuse.a == 1.0f && ort == ORT_NONTRANSPARENT ) ||
			 ( GetMtrl()->Diffuse.a != 1.0f && 
				( ort == ORT_TRANSPARENT || 
					( ort == ORT_FILLZFORTRANSPARENTWITHOUTZ && GetFillZBuffer() == false ) 
			    ) 
			 ) 
		   )
		{
			//Устанавливаем альфу
			if ( ort == ORT_TRANSPARENT )
				g_Graph.SetAlphaBlendNormal();
			else if ( ort == ORT_FILLZFORTRANSPARENTWITHOUTZ )
				g_Graph.SetAlphaBlendForNoRender();
			else
				g_Graph.SetAlphaBlendOff();

			if ( ort != ORT_FILLZFORTRANSPARENTWITHOUTZ )
			{
				//Устанавливаем режимы наложения текстур
				if ( m_bTexSphereMapping && GetTexID() != L"" )
					g_Graph.dev()->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_SPHEREMAP );

				if ( m_bTex2SphereMapping && GetTexID2() != L"" )
					g_Graph.dev()->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_SPHEREMAP );

				//Устанавливаем текстуры
				if ( GetTexID() != L"" )
				{
					g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
					g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE  );	
					g_TexMgr.SetTex( 0, GetTexID() );    
					g_Graph.SetFilteringAll( 0, D3DTEXF_ANISOTROPIC );
				}
				else
				{
					g_Graph.dev()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2  );
					g_Graph.dev()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2  );	
					g_TexMgr.SetTexNull( 0 );    
					g_Graph.SetFilteringAll( 0, D3DTEXF_POINT );
				}

				g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE  );	
				if ( GetTexID2() != L"" )
				{
					g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED  );
					g_TexMgr.SetTex( 1, GetTexID2() ); 
					g_Graph.SetFilteringAll( 1, D3DTEXF_ANISOTROPIC );
				}
				else
				{
					g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE  );
					g_TexMgr.SetTexNull( 1 );
					g_Graph.SetFilteringAll( 1, D3DTEXF_POINT );
				}

				//Включаем спекуляр если надо
				if ( GetMtrl()->Power != 0 )
					g_Graph.dev()->SetRenderState( D3DRS_SPECULARENABLE, true );

				//Устанавливаем материал
					g_Graph.dev()->SetMaterial( GetMtrl() );
			}

			//Выставляем параметры Z буфера
			g_Graph.dev()->SetRenderState( D3DRS_ZENABLE,			D3DZB_TRUE );

			if ( ort == ORT_NONTRANSPARENT )
			{
				g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		GetFillZBuffer() );
				g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,	( GetCullNone() ? D3DCULL_NONE : D3DCULL_CCW ) );
				RenderRoot();
			}
			else if ( ort == ORT_FILLZFORTRANSPARENTWITHOUTZ )
			{
				g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		true );
				g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,	( GetCullNone() ? D3DCULL_NONE : D3DCULL_CCW ) );
				RenderRoot();
			}
			else // if ( ort == ORT_TRANSPARENT )
			{
				g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		false );
				g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
				RenderRoot();

				g_Graph.dev()->SetRenderState( D3DRS_ZWRITEENABLE,		GetFillZBuffer() );
				g_Graph.dev()->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
				RenderRoot();
			}

			if ( ort != ORT_FILLZFORTRANSPARENTWITHOUTZ )
			{
				//Обнуляем 2-ю текстуру
				g_TexMgr.SetTexNull( 1 );

				//Выключаем спекуляр
				if ( GetMtrl()->Power != 0 )
					g_Graph.dev()->SetRenderState( D3DRS_SPECULARENABLE, false );

				//Востанавливаем параметры текстурирования
				if ( m_bTexSphereMapping )
					g_Graph.dev()->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

				if ( m_bTex2SphereMapping )
					g_Graph.dev()->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

				//Отключаем мульти текстурирование
				if ( GetTexID2() != L"" )
				{
					g_Graph.dev()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE  );
					g_Graph.dev()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE  );	
				}
			}

			//Выключаем альфальфу
			if ( ort == ORT_TRANSPARENT )
				g_Graph.SetAlphaBlendOff();
		}
	}

	if ( m_bRenderChilds )
	{
		if ( m_mmObjAttaches.size() > 0 )
		{
			for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
			{
				if ( i->second.pObjEfnAnim )
					i->second.pObjEfnAnim->OnFrameRender( fTime, fElapsedTime, ort );
			}
		}
	}
}
 
void QObjEfnAnim::ObjRender( DWORD rt )
{
	if ( m_bRenderRoot )
	{
		if ( EfnAnimTemp != NULL )
		{
			if ( EfnAnimTemp->iType == 1 )
			{
				g_EfnMgr.Render( EfnAnimTemp->sEfnID, &matWorld, m_iFrame, m_iNextFrame, m_fTweenFactor, RT_POS_NORM_POS_NORM_TEX );
			}
			else
			{
				g_EfnMgr.Render( EfnAnimTemp->sMeshesEfnID + L".ResultMesh", &matWorld, RT_POS_NORM_TEX );
			}
		}
		else
		{
			g_EfnMgr.Render( m_sDefaultEfnID, &matWorld, m_iFrame, m_iNextFrame, m_fTweenFactor, RT_POS_NORM_TEX );
		}
	}

	if ( m_bRenderChilds )
	{
		if ( m_mmObjAttaches.size() > 0 )
		{
			for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
			{
				if ( i->second.pObjEfnAnim )
				{
					i->second.pObjEfnAnim->ObjRender( rt );
				}
			}
		}
	}
}

void QObjEfnAnim::RenderRoot()
{
	if ( EfnAnimTemp != NULL )
	{
		if ( EfnAnimTemp->iType == 1 )
		{
			g_EfnMgr.Render( EfnAnimTemp->sEfnID, &matWorld, m_iFrame, m_iNextFrame, m_fTweenFactor, RT_POS_NORM_POS_NORM_TEX );
		}
		else
		{
			g_EfnMgr.Render( EfnAnimTemp->sMeshesEfnID + L".ResultMesh", &matWorld, RT_POS_NORM_TEX );
		}
	}
	else
	{
		g_EfnMgr.Render( m_sDefaultEfnID, &matWorld, m_iFrame, m_iNextFrame, m_fTweenFactor, RT_POS_NORM_POS_NORM_TEX );
	}
}

bool QObjEfnAnim::CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos )
{
	CSPHERE* sphere = g_EfnMgr.GetEfnSphere( GetAnimEfnID(), (float)m_iFrame );

	if( !sphere )
		return false;

	if( IntersectRay2Sphere( pstRay, &sphere->pos, sphere->radius, &matWorld ) )
	{
		if ( pfDist )
			*pfDist = D3DXVec3Length( &(pstRay->vPos - GetNullPos()->vPos) );

		if ( pvIntrPos )
			*pvIntrPos = GetNullPos()->vPos;

		//HACK
		return true;
		/*D3DXVECTOR3* pOOBB = g_EfnMgr.GetEfnOOBB( GetEfnID(), (float)m_iFrame );

		return IntersectRay2Box( pstRay, pOOBB, &matWorld, pfDist, pvIntrPos );*/
	}
	else
	{
		for( mmapObjNullAttaches_iter i = m_mmObjAttaches.begin(); i != m_mmObjAttaches.end(); i++ )
		{
			if ( i->second.pObjEfnAnim )
			{
				if ( i->second.pObjEfnAnim->CheckForRayIntersect( pstRay, pfDist, pvIntrPos ) )
				{
					if ( pfDist )
						*pfDist = D3DXVec3Length( &(pstRay->vPos - GetNullPos()->vPos) );

					if ( pvIntrPos )
						*pvIntrPos = GetNullPos()->vPos;

					return true;
				}
			}
		}
		return false;	
	}
	
}