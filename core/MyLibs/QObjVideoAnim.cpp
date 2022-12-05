#define STRICT

#include "QObjVideoAnim.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResTextures.h"
#include "QMgrResStates.h"

QObjVideoAnim::QObjVideoAnim()
{
	VideoAnimTemp	= NULL;
	ZeroMemory( GetNullPos(), sizeof(SNULL) );


	m_bStarted		= false;
	m_iSeqID		= -1;
}

QObjVideoAnim::~QObjVideoAnim()
{
	vecAnim.clear();
}

void QObjVideoAnim::Init()
{
	Stop(false);

	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjVideoAnim::Kill()
{
	Stop(false);

	OnLostDevice();
	OnDestroyDevice();
}

void QObjVideoAnim::AnimAdd( const std::wstring& tex_id, int iFrameFrom, int iFrameTo, int iRepeatTimes )
{
	SVideoAnimSeq AnimTemp;

	AnimTemp.sTexID			= tex_id;
	AnimTemp.iRepeatTimes	= iRepeatTimes;
	AnimTemp.iRepeatCounter	= 0;

	QTextureListItem* temp	= g_TexMgr.GetTexItem( tex_id );
	AnimTemp.fFrameRate		= temp->fFrameRate;
	AnimTemp.iFrameFrom		= (iFrameFrom==-1) ? 0 : iFrameFrom;
	AnimTemp.fFrame			= AnimTemp.iFrameFrom;
	AnimTemp.iFrameTo		= (iFrameTo==-1) ? temp->iFramesCount-1 : iFrameTo;
	AnimTemp.fAnimTotalTimeLength = (iRepeatTimes==-1) ? 0 : iRepeatTimes * ( (float)temp->iFramesCount / temp->fFrameRate );

	vecAnim.push_back(AnimTemp);
}

void QObjVideoAnim::Go( double fTime, int seq_id )
{
	m_iSeqID		= seq_id;
	m_bStarted		= true;

	if ( vecAnim.size() > 0 )
	{
		vecAnim[0].fAnimGoTime = fTime;

		for( UINT i = 1; i < vecAnim.size(); i++)
		{
			vecAnim[i].fAnimGoTime += vecAnim[i-1].fAnimGoTime + vecAnim[i-1].fAnimTotalTimeLength;
		}
	}

}

void QObjVideoAnim::Stop(bool bOK)
{
	VideoAnimTemp			= NULL;
	m_bStarted				= false;
	vecAnim.clear();

	if ( m_iSeqID==-1 )
	{
		return;
	}

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

void QObjVideoAnim::OnFrameMove( double fTime, float fElapsedTime )
{
	VideoAnimTemp = NULL;

	//нахождение текущей анимации
	if ( vecAnim.size()>0 && m_bStarted ) 
	{
		for( UINT i = 0; i < vecAnim.size() && !VideoAnimTemp; i++)
		{
			if ( vecAnim[i].iRepeatTimes==-1 )
			{
				VideoAnimTemp = &vecAnim[i];
			}
			else if ( (vecAnim[i].fAnimGoTime <= fTime) && (fTime <= vecAnim[i].fAnimGoTime+vecAnim[i].fAnimTotalTimeLength) )
			{
				VideoAnimTemp = &vecAnim[i];
				m_iAnimI = i;
			}
		}

		if(!VideoAnimTemp)
		{
			Stop( true );
		}
	}

	if ( VideoAnimTemp )
	{
		VideoAnimTemp->fFrame += fElapsedTime * VideoAnimTemp->fFrameRate;
		
		if ( (int)VideoAnimTemp->fFrame > VideoAnimTemp->iFrameTo )
		{
			if ( VideoAnimTemp->iRepeatTimes==-1 )
			{
				VideoAnimTemp->fFrame -= VideoAnimTemp->iFrameTo - VideoAnimTemp->iFrameFrom;
			}
			else
			{
				if ( VideoAnimTemp->iRepeatCounter < VideoAnimTemp->iRepeatTimes-1 )
				{
					vecAnim.erase( vecAnim.begin() );
					VideoAnimTemp = NULL;
					g_Log.WriteError(L"fuck in videoanim");
				}
			}
		}
		g_TexMgr.GetTex( VideoAnimTemp->sTexID, (float)VideoAnimTemp->fFrame );
	}
}