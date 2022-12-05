#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QObjBase.h"
#include "QObjEfnAnim.h"

//--------------------------class-------------------------------------------
class QObj3DVideo : public QObjBase
{
public:

	QObj3DVideo( const SNULL* null, const std::wstring& move_efn_id, const std::wstring& frames_efn_id );
	~QObj3DVideo();

	void					Init();
	void					Kill();

	void					Go( double fTime, int seq_id );
	void					Stop(bool bOK);

	void					SetPos( const SNULL* stNull );

	int						GetSeqID() { return m_iSeqID; };

	bool					IsStarted() { return GetEfnAnim()->IsStarted(); };

	bool					AttachObjEfnAnimToTimeLineNull( const std::wstring& obj_id, const std::wstring& null_id );

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) {	return false; };

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort ) {};
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt ) {};

private:

	mmapWStrWStr			m_mmFramesBinds;

	bool					m_bStarted;

	int						m_iSeqID;

};