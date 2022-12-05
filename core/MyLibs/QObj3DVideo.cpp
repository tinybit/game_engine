#define STRICT

#include "QObj3DVideo.h"

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

QObj3DVideo::QObj3DVideo( const SNULL* null, const std::wstring& move_efn_id, const std::wstring& frames_efn_id )
{
	SetNullPos( null );
	SetEfnID( move_efn_id );
	SetEfnID2( frames_efn_id );
	
	m_bStarted			= false;
	m_iSeqID			= -1;
}

QObj3DVideo::~QObj3DVideo()
{
}

void QObj3DVideo::Init()
{
	Stop(false);

	g_EfnMgr.LoadEfn( GetEfnID2() );

	if ( GetObjMgr()->AddObjEfnAnim( GetEfnAnimObjID(), GetEfnID(), L"", L"" ) )
	{
		SetEfnAnim( GetObjMgr()->GetObjEfnAnim( GetEfnAnimObjID() ) );
		GetEfnAnim()->SetRenderPrms( false, false, true );
		GetEfnAnim()->SetPos( GetNullPos() );
	}

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObj3DVideo::Kill()
{
	Stop(false);

	GetObjMgr()->DelObj( GetEfnAnimObjID() );

	OnLostDevice();
	OnDestroyDevice();
}

void QObj3DVideo::Go( double fTime, int seq_id )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->Go( fTime, 0, seq_id );
}

void QObj3DVideo::Stop( bool bOK )
{
	if ( GetEfnAnim() )
		GetEfnAnim()->Stop( bOK );
}

void QObj3DVideo::SetPos( const SNULL* pstNull )
{
	SetNullPos( pstNull );
	
	if ( GetEfnAnim() )
		GetEfnAnim()->SetPos( GetNullPos() );
}

bool QObj3DVideo::AttachObjEfnAnimToTimeLineNull( const std::wstring& obj_id, const std::wstring& null_id )
{
	m_mmFramesBinds.insert( mmapWStrWStr::value_type( null_id, obj_id ) );
	
	return true;
}

void QObj3DVideo::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( GetEnabled()==false )
		return;

	if ( GetEfnAnim() )
	{
		GetEfnAnim()->OnFrameMove( fTime, fElapsedTime );

		QEfnDataListItem* efn = g_EfnMgr.GetEfn( GetEfnID2() );
		int m_iObjLeftFrame, m_iObjRightFrame;

		if ( GetEfnAnim()->GetAttachedObjList()->size() > 0 && efn != NULL )
		{
			QObjEfnAnim* obj = NULL;

			for( mmapWStrWStr_iter i = m_mmFramesBinds.begin(); i != m_mmFramesBinds.end(); i++ )
			{
				obj = GetObjMgr()->GetObjEfnAnim( i->second );

				if ( obj )
				{
					if ( g_EfnMgr.GetEfnNullFor3DVideo( efn, i->first, GetEfnAnim()->GetFrame(), GetEfnAnim()->GetNextFrame(), &m_iObjLeftFrame, &m_iObjRightFrame ) )
					{
						obj->SetFrame( m_iObjLeftFrame );
						obj->SetNextFrame( m_iObjRightFrame );
						obj->SetTweenFactor( GetEfnAnim()->GetTweenFactor() );
					}
				}
			}
		}
	}
}