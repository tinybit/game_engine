#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QObjBase.h"

//-------------------------anim controller-------------------------------
struct SVideoAnimSeq
{
	std::wstring			sTexID;
	int						iFrameFrom;
	int						iFrameTo;
	int						iRepeatTimes;
	int						iRepeatCounter;
	float					fFrameRate;
	double					fFrame;
	double					fAnimGoTime;
	double					fAnimTotalTimeLength;
};

typedef std::vector<SVideoAnimSeq> vecVideoAnim;

//--------------------------class-------------------------------------------

class QObjVideoAnim : public QObjBase
{
public:

	QObjVideoAnim();
	~QObjVideoAnim();

	void					Init();
	void					Kill();

	void					AnimAdd( const std::wstring& tex_id, int iFrameFrom, int iFrameTo, int iRepeatTimes );
	
	void					Go( double fTime, int seq_id );
	void					Stop(bool bOK);

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) { return false; };

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort ) {};
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt ) {};

private:

	vecVideoAnim			vecAnim;
	SVideoAnimSeq*			VideoAnimTemp;

	int						m_iAnimI;

	bool					m_bStarted;

	int						m_iSeqID;
};