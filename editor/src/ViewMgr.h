#pragma once

#include "utils/common.h"

enum RENDER_VIEW
{
	RV_PERSPECTIVE	= 11,
	RV_FRONT		= 22,
	RV_TOP			= 33,
	RV_RIGHT		= 44,
	RV_CURRENT		= 55
};

class CViewMgr
{
	CWnd*	m_pParent;
	bool	m_bIsRotating;
	bool	m_bIsTranslating;
	bool	m_bIsZooming;

	public:
		CViewMgr();
		~CViewMgr();

		void		StoreOldValue();
		void		SetParent( CWnd* pParent ) { m_pParent = pParent; };
		void		ChangeRenderView( enum RENDER_VIEW RenderView );
		void		MoveCamByMouse( float& fXOffset, float& fYOffset );
		void		CorrectCamUpvector();
		void		StartCamTranslating();
		void		StartCamRotating();
		void		StartCamZooming();
		void		FinishCamTranslating()	{ m_bIsTranslating	= false; CorrectCamUpvector(); };
		void		FinishCamRotating()		{ m_bIsRotating		= false; CorrectCamUpvector(); };
		void		FinishCamZooming()		{ m_bIsZooming = false; CorrectCamUpvector(); };

		bool		IsCamTranslating()	{ return m_bIsTranslating; };
		bool		IsCamRotating()		{ return m_bIsRotating; };
		bool		IsCamZooming()		{ return m_bIsZooming; };
		bool		IsCameraOperationInProgress();
		//void		LockCameraOnPoint( CCAMERA* pCam, const D3DXVECTOR3& pt, float fZoom );
		void		LockCameraOnPoint( enum RENDER_VIEW rv, const D3DXVECTOR3& pt, float fZoom, D3DXVECTOR3& oobMin, D3DXVECTOR3& oobMax, float fAspect );
		

		CCAMERA*			m_camCurrent;

		CCAMERA				m_camPerspective;
		CCAMERA				m_camFront;
		CCAMERA				m_camTop;
		CCAMERA				m_camRight;


		D3DXVECTOR3			vecTempCamPos;
		D3DXVECTOR3			vecTempCamDir;
		D3DXVECTOR3			vecTempCamUp;
		float				m_fTempCamLen;

		float		m_fZoomFront;
		float		m_fZoomTop;
		float		m_fZoomRight;
		float		m_fZoomFrontOld;
		float		m_fZoomTopOld;
		float		m_fZoomRightOld;
		D3DXVECTOR2	m_PosFront;
		D3DXVECTOR2	m_PosTop;
		D3DXVECTOR2	m_PosRight;
		enum RENDER_VIEW m_CurrView;

		bool		m_SpotLightsConeVisible;
};
