#include "ViewMgr.h"

#include "dialogs/CookieMachineDlg.h"

CViewMgr::CViewMgr()
{
	m_fZoomFront		= 10.0f;
	m_fZoomTop			= 10.0f;
	m_fZoomRight		= 10.0f;
	m_pParent			= NULL;
	m_bIsRotating		= false;
	m_bIsTranslating	= false;
	m_bIsZooming		= false;

	m_SpotLightsConeVisible = false; 

	ChangeRenderView( RV_PERSPECTIVE );

	m_camTop.dir		= D3DXVECTOR3( 0, 0, 0 );
	m_camTop.pos		= D3DXVECTOR3( 0, 200, 0 );
	m_camTop.upvector	= D3DXVECTOR3( 0, 0, 1 );

	m_camFront.dir		= D3DXVECTOR3( 0, 0, 0 );
	m_camFront.pos		= D3DXVECTOR3( 0, 0, -200 );
	m_camFront.upvector	= D3DXVECTOR3( 0, 1, 0 );

	m_camRight.dir		= D3DXVECTOR3( 0, 0, 0 );
	m_camRight.pos		= D3DXVECTOR3( 200, 0, 0 );
	m_camRight.upvector	= D3DXVECTOR3( 0, 1, 0 );
}

CViewMgr::~CViewMgr()
{
}

void CViewMgr::StoreOldValue()
{
	m_fZoomFrontOld	= m_fZoomFront;
	m_fZoomTopOld	= m_fZoomTop;
	m_fZoomRightOld	= m_fZoomRight;
}

void CViewMgr::ChangeRenderView( enum RENDER_VIEW RenderView )
{
	//view is already set
	if( m_CurrView == RenderView )
		return;

	switch( RenderView )
	{
		case RV_PERSPECTIVE:
			m_camCurrent = &m_camPerspective;
			
			m_CurrView = RV_PERSPECTIVE;
			break;

		case RV_FRONT:
			m_camCurrent = &m_camFront;

			m_CurrView = RV_FRONT;
			break;

		case RV_TOP:
			m_camCurrent = &m_camTop;

			m_CurrView = RV_TOP;
			break;

		case RV_RIGHT:
			m_camCurrent = &m_camRight;

			m_CurrView = RV_RIGHT;
			break;
	}
};

void CViewMgr::LockCameraOnPoint( enum RENDER_VIEW rv, const D3DXVECTOR3& pt, float fZoom, D3DXVECTOR3& oobMin, D3DXVECTOR3& oobMax, float fAspect )
{
	D3DXVECTOR3 vOOBBCenter = pt;

	if( rv == RV_CURRENT )
		rv = m_CurrView;

	switch( rv )
	{
		case RV_PERSPECTIVE:
			{
				//check whether our cam is already locked on obj
				D3DXVECTOR3 zoom_tmp = m_camPerspective.pos - m_camPerspective.dir;

				D3DXVec3Normalize( &zoom_tmp, &zoom_tmp );
				D3DXVec3Scale( &zoom_tmp, &zoom_tmp, fZoom );

				D3DXVECTOR3 pos_aft_zoom = m_camPerspective.dir + zoom_tmp;

				if( D3DXVec3Length( &D3DXVECTOR3( m_camPerspective.pos - pos_aft_zoom ) ) < EPSILON )
					return;

				D3DXVECTOR3 a = m_camPerspective.dir - m_camPerspective.pos;
				D3DXVECTOR3 b = vOOBBCenter - m_camPerspective.pos;
				D3DXVECTOR3	temp, axis;

				a.y = 0.0f;
				b.y = 0.0f;

				float angY = GetAngle( &a, &b, &D3DXVECTOR3(0, 1, 0) );

				D3DXMATRIX yrot, xrot;
				D3DXMatrixRotationY( &yrot, angY );

				a = m_camPerspective.dir - m_camPerspective.pos;
				b = vOOBBCenter - m_camPerspective.pos;

				D3DXVec3Normalize( &a, &a );
				D3DXVec3Normalize( &b, &b );

				D3DXVec3TransformCoord( &a, &a, &yrot );

				D3DXVec3Cross( &axis, &a, &b );

				float angX = GetAngle( &a, &b, &axis );

				D3DXMatrixRotationAxis( &xrot, &axis, angX );

				if( angY != 0.0f )
					D3DXVec3TransformCoord( &m_camPerspective.upvector, &m_camPerspective.upvector, &yrot );

				if( angX != 0.0f )
					D3DXVec3TransformCoord( &m_camPerspective.upvector, &m_camPerspective.upvector, &xrot );

				m_camPerspective.dir = vOOBBCenter;

				CorrectCamUpvector();

				D3DXVECTOR3 zoom = m_camPerspective.pos - m_camPerspective.dir;
				D3DXVec3Normalize( &zoom, &zoom );
				D3DXVec3Scale( &zoom, &zoom, fZoom );
				m_camPerspective.pos = m_camPerspective.dir + zoom;
			}
			break;

		case RV_FRONT:
			{
				m_camFront.pos.y = vOOBBCenter.y;
				m_camFront.pos.x = vOOBBCenter.x;

				m_camFront.dir.y = vOOBBCenter.y;
				m_camFront.dir.x = vOOBBCenter.x;

				float view_size = 0.0f;
				float width		= oobMax.x - oobMin.x;
				float height	= oobMax.y - oobMin.y; 

				view_size = width;

				if( width < height )
					view_size = height / fAspect;

				view_size += view_size / 4.0f;

				m_fZoomFront = view_size;
			}
			break;

		case RV_TOP:
			{
				m_camTop.pos.x = vOOBBCenter.x;
				m_camTop.pos.z = vOOBBCenter.z;

				m_camTop.dir.x = vOOBBCenter.x;
				m_camTop.dir.z = vOOBBCenter.z;

				float view_size = 0.0f;
				float width		= oobMax.x - oobMin.x;
				float height	= oobMax.z - oobMin.z;

				view_size = width;

				if( width < height )
					view_size = height / fAspect;

				view_size += view_size / 4.0f;

				m_fZoomTop = view_size;
			}
			break;

		case RV_RIGHT:
			{
				m_camRight.pos.y = vOOBBCenter.y;
				m_camRight.pos.z = vOOBBCenter.z;

				m_camRight.dir.y = vOOBBCenter.y;
				m_camRight.dir.z = vOOBBCenter.z;

				float view_size = 0.0f;
				float width		= oobMax.z - oobMin.z;
				float height	= oobMax.y - oobMin.y;

				view_size = width;

				if( width < height )
					view_size = height / fAspect;

				view_size += view_size / 4.0f;

				m_fZoomRight = view_size;
			}
			break;
	}
}

//void CViewMgr::LockCameraOnPoint( CCAMERA* pCam, const D3DXVECTOR3& pt )
//{
//	D3DXVECTOR3	temp, axis, vOOBBCenter = pt;
//	D3DXVECTOR3 a = pCam->dir - pCam->pos;
//	D3DXVECTOR3 b = vOOBBCenter - pCam->pos;
//
//	a.y = 0.0f;
//	b.y = 0.0f;
//
//	float angY = GetAngle( &a, &b, &D3DXVECTOR3(0, 1, 0) );
//
//	D3DXMATRIX yrot, xrot;
//	D3DXMatrixRotationY( &yrot, angY );
//
//	a = pCam->dir - pCam->pos;
//	b = vOOBBCenter - pCam->pos;
//
//	D3DXVec3Normalize( &a, &a );
//	D3DXVec3Normalize( &b, &b );
//
//	D3DXVec3TransformCoord( &a, &a, &yrot );
//
//	D3DXVec3Cross( &axis, &a, &b );
//
//	float angX = GetAngle( &a, &b, &axis );
//
//	D3DXMatrixRotationAxis( &xrot, &axis, angX );
//
//	if( angY != 0.0f )
//		D3DXVec3TransformCoord( &pCam->upvector, &pCam->upvector, &yrot );
//
//	if( angX != 0.0f )
//		D3DXVec3TransformCoord( &pCam->upvector, &pCam->upvector, &xrot );
//
//	pCam->dir = vOOBBCenter;
//
//	CorrectCamUpvector();
//
//	D3DXVECTOR3 zoom = pCam->pos - pCam->dir;
//	D3DXVec3Normalize( &zoom, &zoom );
//	D3DXVec3Scale( &zoom, &zoom, 10 );
//
//	pCam->pos = pCam->dir + zoom;
//}

void CViewMgr::MoveCamByMouse( float& fXOffset, float& fYOffset )
{
	static float oldval = 0.0f;

	if( m_bIsRotating && m_CurrView == RV_PERSPECTIVE )
	{
		D3DXMATRIX rotY, rotX, trans0, res, rot90, mAxis;
		D3DXVECTOR3 axis, up = D3DXVECTOR3(0,1,0), temp;

		//rotate around Y axis
		D3DXMatrixRotationY( &rotY, -fXOffset / 300.0f );

		D3DXVec3TransformCoord( &m_camCurrent->pos, &vecTempCamPos, &rotY );
		D3DXVec3TransformCoord( &m_camCurrent->upvector, &vecTempCamUp, &rotY );

		//find axis of rotation ( X' )
		D3DXVec3Cross( &axis, &m_camCurrent->upvector, &m_camCurrent->pos );

		//rotate around X' axis
		D3DXMatrixRotationAxis( &mAxis, &axis, fYOffset / 300.0f );

		D3DXVec3TransformCoord( &m_camCurrent->pos, &m_camCurrent->pos, &mAxis );
		D3DXVec3TransformCoord( &m_camCurrent->upvector, &m_camCurrent->upvector, &mAxis );

		D3DXVec3Normalize( &m_camCurrent->upvector, &m_camCurrent->upvector );

		m_camCurrent->pos += m_camCurrent->dir;
	}	
	else if( m_bIsZooming )
	{
		float change = m_fTempCamLen + fYOffset / (m_fTempCamLen / pow( m_fTempCamLen, 2) * 500);

		if( change < 0.2f )
			change = 0.2f;

		if( m_CurrView == RV_PERSPECTIVE )
		{
			D3DXVec3Scale( &m_camCurrent->pos, &vecTempCamPos, change );
			m_camCurrent->pos += m_camCurrent->dir;
		}
		else if( m_CurrView == RV_TOP )
		{
			if( m_fZoomTopOld + fYOffset / 20 < 0.2f )
				fYOffset = oldval;

			m_fZoomTop = m_fZoomTopOld + fYOffset / 20;

			if( m_fZoomTop < 0.2f )
				m_fZoomTop = 0.2f;

			oldval = fYOffset;
		}
		else if( m_CurrView == RV_FRONT )
		{
			if( m_fZoomFrontOld + fYOffset / 20 < 0.2f )
				fYOffset = oldval;

			m_fZoomFront = m_fZoomFrontOld + fYOffset / 20;

			if( m_fZoomFront < 0.2f )
				m_fZoomFront = 0.2f;

			oldval = fYOffset;
		}
		else if( m_CurrView == RV_RIGHT )
		{
			if( m_fZoomRightOld + fYOffset / 20 < 0.2f )
				fYOffset = oldval;

			m_fZoomRight = m_fZoomRightOld + fYOffset / 20;

			if( m_fZoomRight < 0.2f )
				m_fZoomRight = 0.2f;

			oldval = fYOffset;
		}
	}
	else if( m_bIsTranslating )
	{
		D3DXVec3Normalize( &m_camCurrent->upvector, &m_camCurrent->upvector );

		D3DXVECTOR3 offX, offY, axis;

		D3DXVec3Cross( &axis, &m_camCurrent->upvector, &(m_camCurrent->pos - m_camCurrent->dir) );
		D3DXVec3Normalize( &axis, &axis );

		D3DXVec3Scale( &offX, &axis, fXOffset / 50.0f );
		D3DXVec3Scale( &offY, &m_camCurrent->upvector, fYOffset / 50.0f );

		m_camCurrent->pos = vecTempCamPos - offX - offY;
		m_camCurrent->dir = vecTempCamDir - offX - offY;
	}
}

void CViewMgr::StartCamTranslating()
{
	if( m_bIsTranslating )
		return;

	CorrectCamUpvector();

	m_bIsTranslating	= true;

	vecTempCamPos = m_camCurrent->pos;
	vecTempCamDir = m_camCurrent->dir;

	m_bIsZooming	= false;
	m_bIsRotating	= false;
}

void CViewMgr::StartCamRotating()
{
	if( m_bIsRotating )
		return;

	CorrectCamUpvector();

	m_bIsRotating		= true;

	vecTempCamPos		= m_camCurrent->pos - m_camCurrent->dir;
	vecTempCamUp		= m_camCurrent->upvector;

	m_bIsZooming		= false;
	m_bIsTranslating	= false;
}

void CViewMgr::StartCamZooming()
{
	if( m_bIsZooming )
		return;

	if( m_CurrView == RV_FRONT )
		m_fZoomFrontOld	= m_fZoomFront;

	if( m_CurrView == RV_RIGHT )
		m_fZoomRightOld	= m_fZoomRight;

	if( m_CurrView == RV_TOP )
		m_fZoomTopOld = m_fZoomTop;

	CorrectCamUpvector();

	m_bIsZooming	= true;

	vecTempCamPos = m_camCurrent->pos - m_camCurrent->dir;
	m_fTempCamLen = D3DXVec3Length( &vecTempCamPos );
	D3DXVec3Normalize( &vecTempCamPos, &vecTempCamPos );

	m_bIsTranslating	= false;
	m_bIsRotating		= false;
}

bool CViewMgr::IsCameraOperationInProgress()
{
	if( m_bIsZooming || m_bIsTranslating || m_bIsRotating )
		return true;

	return false;
}

void CViewMgr::CorrectCamUpvector()
{
	D3DXVECTOR3 vec	= m_camCurrent->dir - m_camCurrent->pos;
	D3DXVECTOR3 cross;

	D3DXVec3Cross( &cross, &vec, &m_camCurrent->upvector );
	D3DXVec3Normalize( &cross, &cross );

	cross.y = 0.0f;

	D3DXVec3Cross( &m_camCurrent->upvector, &cross, &vec );
	D3DXVec3Normalize( &m_camCurrent->upvector, &m_camCurrent->upvector );
}