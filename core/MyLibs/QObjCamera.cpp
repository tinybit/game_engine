#define STRICT

#include "QObjCamera.h"

#include "UtilsConverts.h"
#include "QMath.h"

#include "QSubSysGraphics.h"
#include "QSubSysInput.h"
#include "QSubSysLog.h"

QObjCamera::QObjCamera()
{
	ZeroMemory( &stCamera, sizeof(SCAMERA) );

	ZeroMemory( GetNullPos(), sizeof(SNULL) );
	SetPerCamPrm(	&D3DXVECTOR3( 0.0f, 0.0f, -10.0f ),
					&D3DXVECTOR3( 0.0f, 0.0f,   0.0f ),
					&D3DXVECTOR3( 0.0f, 1.0f,   0.0f ),	
					&D3DXVECTOR2( g_Graph.fWidth, g_Graph.fHeight ),
					0.1f, 100.0f, D3DX_PI / 4 );

	SetOrthoCamPrm(	&D3DXVECTOR3( 0.0f, 0.0f, -10.0f ),
					&D3DXVECTOR3( 0.0f, 0.0f,   0.0f ),
					&D3DXVECTOR3( 0.0f, 1.0f,   0.0f ),	
					&D3DXVECTOR2( g_Graph.fWidth, g_Graph.fHeight ),
					0.0f, 100.0f );

	SetZoomPrm( 0.0f, 0.1f );

	m_vPP			= stCamera.vPerPos;

	mouse_x			=
	mouse_y			=
	mouse_left_x	= 
	mouse_left_y	= 
	mouse_right_x	= 
	mouse_right_y	= 
	ortho_mouse_x	=
	ortho_mouse_y	=
	ortho_mouse_left_x	=
	ortho_mouse_left_y	=
	ortho_mouse_right_x	=
	ortho_mouse_right_y	= -1;

	D3DXMatrixIdentity( &matIdentity );
}

void QObjCamera::Init()
{
	if ( g_Graph.dev() )
	{
		OnCreateDevice();
		OnResetDevice();
	}
}

void QObjCamera::Kill()
{
	OnLostDevice();
	OnDestroyDevice();
}

void QObjCamera::SetPP( const D3DXVECTOR3* pvPP, float fElapsedTime )
{
	m_vPP.x = pvPP->x; 
	m_vPP.z = pvPP->z; 
	if ( fabs(m_vPP.y - pvPP->y) < 0.5f )
		m_vPP.y += (pvPP->y - m_vPP.y) * fElapsedTime * 10;
	else
		m_vPP.y = pvPP->y;
}

void QObjCamera::SetPerCamPrm( const D3DXVECTOR3* pvPerPos, const D3DXVECTOR3* pvPerTarg, const D3DXVECTOR3* pvPerUpVec, const D3DXVECTOR2* pvPerBackSize, float fPerNearPlane, float fPerFarPlane, float fFovY )
{
	GetNullPos()->vPos = stCamera.vPerPos = *pvPerPos;
	stCamera.vPerTarg		= *pvPerTarg;
	stCamera.vPerUpVec		= *pvPerUpVec;
	stCamera.vPerBackSize	= *pvPerBackSize;
	stCamera.fPerAspect		= stCamera.vPerBackSize.x / stCamera.vPerBackSize.y;
	stCamera.vPerFov.y		= fFovY;
	stCamera.vPerFov.x		= FovY2FovX( fFovY, stCamera.fPerAspect );
	stCamera.fPerNearPlane	= fPerNearPlane;
	stCamera.fPerFarPlane	= fPerFarPlane;

	D3DXMatrixLookAtLH( &matViewPer, &stCamera.vPerPos, &stCamera.vPerTarg, &stCamera.vPerUpVec );

	D3DXMatrixPerspectiveFovLH( &matProjPerOrig, 
								stCamera.vPerFov.y, 
								stCamera.fPerAspect, 
								stCamera.fPerNearPlane, 
								stCamera.fPerFarPlane );
	
	matProjPer		= matProjPerOrig;	
}

void QObjCamera::SetPerCamPos( const D3DXVECTOR3* pvPos )
{
	GetNullPos()->vPos = stCamera.vPerPos = *pvPos;
	D3DXMatrixLookAtLH( &matViewPer, &stCamera.vPerPos, &stCamera.vPerTarg, &stCamera.vPerUpVec );
}

void QObjCamera::SetPerCamTarget( const D3DXVECTOR3* pvTarget ) 
{ 
	stCamera.vPerTarg = *pvTarget; 
	D3DXMatrixLookAtLH( &matViewPer, &stCamera.vPerPos, &stCamera.vPerTarg, &stCamera.vPerUpVec );
}

void QObjCamera::SetOrthoCamPrm( const D3DXVECTOR3* pvOrthoPos, const D3DXVECTOR3* pvOrthoTarg, const D3DXVECTOR3* pvOrthoUpVec, const D3DXVECTOR2* pvOrthoBackSize, float fOrthoNearPlane, float fOrthoFarPlane )
{
	stCamera.vOrthoPos		= *pvOrthoPos;
	stCamera.vOrthoTarg		= *pvOrthoTarg;
	stCamera.vOrthoUpVec	= *pvOrthoUpVec;
	stCamera.vOrthoBackSize	= *pvOrthoBackSize;
	stCamera.fOrthoAspect	= stCamera.vOrthoBackSize.x / stCamera.vOrthoBackSize.y;
	stCamera.fOrthoNearPlane= fOrthoNearPlane;
	stCamera.fOrthoFarPlane	= fOrthoFarPlane;

	D3DXMatrixLookAtLH( &matViewOrtho, &stCamera.vOrthoPos, &stCamera.vOrthoTarg, &stCamera.vOrthoUpVec );

	D3DXMatrixOrthoOffCenterLH( &matProjOrthoOrig, 
								0, stCamera.vOrthoBackSize.x - 1,
								0, stCamera.vOrthoBackSize.y - 1,
								stCamera.fOrthoNearPlane, stCamera.fOrthoFarPlane );

	matProjOrtho	= matProjOrthoOrig;
}

void QObjCamera::SetZoomPrm( float fZoomNear, float fZoomFar )
{
	stCamera.fZoomNear	= fZoomNear;
	stCamera.fZoomFar	= fZoomFar;
	if ( stCamera.fZoomFar < stCamera.fZoomNear)
	{
		stCamera.fZoomFar = stCamera.fZoomNear + 0.1f;
	}
}

void QObjCamera::SetOrthoCamPos( const D3DXVECTOR3* pvPos )
{
	stCamera.vOrthoPos = *pvPos; 
	D3DXMatrixLookAtLH( &matViewPer, &stCamera.vOrthoPos, &stCamera.vOrthoTarg, &stCamera.vOrthoUpVec );
}

void QObjCamera::SetOrthoCamTarget( const D3DXVECTOR3* pvTarget ) 
{ 
	stCamera.vOrthoTarg = *pvTarget; 
	D3DXMatrixLookAtLH( &matViewPer, &stCamera.vOrthoPos, &stCamera.vOrthoTarg, &stCamera.vOrthoUpVec );
}

void QObjCamera::SetPerCam()
{
	g_Graph.dev()->SetTransform( D3DTS_WORLD,		&matIdentity );//World
	g_Graph.dev()->SetTransform( D3DTS_VIEW,		&matViewPer ); //View
	g_Graph.dev()->SetTransform( D3DTS_PROJECTION,	&matProjPer ); //Projection
}

void QObjCamera::SetOrthoCam()
{
	g_Graph.dev()->SetTransform( D3DTS_WORLD,		&matIdentity );  //World
	g_Graph.dev()->SetTransform( D3DTS_VIEW,		&matViewOrtho ); //View
	g_Graph.dev()->SetTransform( D3DTS_PROJECTION,	&matProjOrtho ); //Projection
}

void QObjCamera::SetTex( const QTextureListItem* t ) 
{
	if ( t )
	{
		m_stTex = *t;
	}
	else
	{
		m_stTex.ImgWidth	= g_Graph.iWidth;
		m_stTex.ImgHeight	= g_Graph.iHeight;
		m_stTex.RealWidth	= g_Graph.iWidth;
		m_stTex.RealHeight	= g_Graph.iHeight;
		m_stTex.TexWidth	= GetHigherPow2(g_Graph.iWidth);
		m_stTex.TexHeight	= GetHigherPow2(g_Graph.iHeight);
		m_stTex.fWidthAspect= (float)m_stTex.ImgWidth / m_stTex.TexWidth;
		m_stTex.fHeightAspect= (float)m_stTex.ImgHeight / m_stTex.TexHeight;
	}

	m_fImgScreenMinScale = GetMin( (float)m_stTex.ImgWidth / stCamera.vOrthoBackSize.x, (float)m_stTex.ImgHeight / stCamera.vOrthoBackSize.y );
	m_fImgScreenMaxScale = GetMax( (float)m_stTex.ImgWidth / stCamera.vOrthoBackSize.x, (float)m_stTex.ImgHeight / stCamera.vOrthoBackSize.y );
	m_fOrigScreenMaxScale = (float)m_stTex.RealWidth / stCamera.vOrthoBackSize.x;
	m_fOrigScreenMinScale = m_fOrigScreenMaxScale / m_fImgScreenMinScale;
	m_fOrigImgMinScale = GetMin( (float)m_stTex.RealWidth / m_stTex.ImgWidth, (float)m_stTex.RealHeight / m_stTex.ImgHeight );
	m_fScale = m_fOrigScreenMinScale;

	if ( ( (float)m_stTex.ImgWidth / stCamera.fOrthoAspect ) >= (float)m_stTex.ImgHeight )
	{
		m_fOrthoScaleX = 1.0f;
		m_fOrthoScaleY = ((float)m_stTex.ImgWidth / stCamera.fOrthoAspect ) / m_stTex.ImgHeight;
	}
	else
	{
		m_fOrthoScaleX = ((float)m_stTex.ImgHeight * stCamera.fOrthoAspect ) / m_stTex.ImgWidth;
		m_fOrthoScaleY = 1.0f;	
	}
}

void QObjCamera::ComputeCam()
{
	if ( m_vPP != stCamera.vPerPos )
	{
		m_fDist = D3DXVec3Length( &(m_vPP - stCamera.vPerPos) );

		if ( m_fDist > stCamera.fZoomFar )
		{
			m_fScale = m_fOrigScreenMaxScale;
		}
		else if ( m_fDist > stCamera.fZoomNear )
		{
			m_fScale = m_fOrigScreenMinScale + (m_fOrigScreenMaxScale - m_fOrigScreenMinScale) * (m_fDist - stCamera.fZoomNear) / ( stCamera.fZoomFar - stCamera.fZoomNear );
		}
		
		D3DXVec3Project( &m_vTempProkz, &(m_vPP+D3DXVECTOR3(0.0f, 0.75f, 0.0f)), &g_Graph.vp, &matProjPerOrig, &matViewPer, &matIdentity );

		m_fTransX = (                   - m_vTempProkz.x  / g_Graph.fMaxX + 0.5f ) * m_fScale;
		m_fTransY = ( -(g_Graph.fHeight - m_vTempProkz.y) / g_Graph.fMaxY + 0.5f ) * m_fScale;
	}
	else
	{
		m_fScale = m_fOrigScreenMinScale;
		m_fTransX = m_fTransY = 0.5f;
	}
	m_fScale2 = m_fScale / m_fOrigImgMinScale;

    m_fOrthoW = g_Graph.fMaxX / m_fScale2 * m_fOrthoScaleX;
	m_fOrthoH = g_Graph.fMaxY / m_fScale2 * m_fOrthoScaleY;
	m_fOrthoL = -m_fTransX * m_fOrthoW + (g_Graph.fMaxX - m_fOrthoW) * 0.5f;
	m_fOrthoT = -m_fTransY * m_fOrthoH + (g_Graph.fMaxY - m_fOrthoH) * 0.5f;

	if ( m_fOrthoL				<          0.0f ) m_fOrthoL = 0.0f;
	if ( m_fOrthoL + m_fOrthoW	> g_Graph.fMaxX ) m_fOrthoL = g_Graph.fMaxX - m_fOrthoW;
	if ( m_fOrthoT				<          0.0f ) m_fOrthoT = 0.0f;
	if ( m_fOrthoT + m_fOrthoH	> g_Graph.fMaxY ) m_fOrthoT = g_Graph.fMaxY - m_fOrthoH;
	m_fOrthoL += 0.5f;
	m_fOrthoT -= 0.5f;

	m_fTransX = ( -m_fOrthoL + (g_Graph.fMaxX - m_fOrthoW) * 0.5f ) / m_fOrthoW;
	m_fTransY = ( -m_fOrthoT + (g_Graph.fMaxY - m_fOrthoH) * 0.5f ) / m_fOrthoH;

	//Создание матрицы для ортогоналки
	D3DXMatrixOrthoOffCenterLH( &matProjOrtho, 
								m_fOrthoL, m_fOrthoL + m_fOrthoW,
								m_fOrthoT, m_fOrthoT + m_fOrthoH,
								0.0f, 100.0f );

	//Маштабирование для переспективной матрицы
	D3DXMatrixScaling( &m_matPerScale, m_fScale, m_fScale, 1.0f );

	//Смещение для переспктивной матрицы
	D3DXMatrixTranslation( &m_matPerTrans, m_fTransX * 2.0f, m_fTransY * 2.0f, 0.0f );

	matProjPer =  matProjPerOrig * m_matPerScale * m_matPerTrans;
	matViewProjPer = matViewPer * matProjPer;
}

SRay* QObjCamera::GetMouseRay()
{
	if ( g_Mouse.x!=mouse_x || g_Mouse.y!=mouse_y )
	{
		mouse_x = g_Mouse.x; mouse_y = g_Mouse.y;
		D3DXVec3Unproject( &m_stMouseRay.vPos, &D3DXVECTOR3( (float)mouse_x, (float)mouse_y,-1.0f), &g_Graph.vp, &matProjPer, &matViewPer, &matIdentity);	
		D3DXVec3Unproject( &m_stMouseRay.vTar, &D3DXVECTOR3( (float)mouse_x, (float)mouse_y, 1.0f), &g_Graph.vp, &matProjPer, &matViewPer, &matIdentity);
		D3DXVec3Normalize( &m_stMouseRay.vDir, &(m_stMouseRay.vTar - m_stMouseRay.vPos) );
	}	
	return &m_stMouseRay;
}

SRay* QObjCamera::GetLeftClickRay()
{
	if ( g_Mouse.left_x!=mouse_left_x || g_Mouse.left_y!=mouse_left_y )
	{
		mouse_left_x = g_Mouse.left_x; mouse_left_y = g_Mouse.left_y;
		D3DXVec3Unproject( &m_stLeftClickRay.vPos, &D3DXVECTOR3( (float)mouse_left_x, (float)mouse_left_y, -1.0f), &g_Graph.vp, &matProjPer, &matViewPer, &matIdentity);
		D3DXVec3Unproject( &m_stLeftClickRay.vTar, &D3DXVECTOR3( (float)mouse_left_x, (float)mouse_left_y,  1.0f), &g_Graph.vp, &matProjPer, &matViewPer, &matIdentity);
		D3DXVec3Normalize( &m_stLeftClickRay.vDir , &(m_stLeftClickRay.vTar - m_stLeftClickRay.vPos) );
	}	
	return &m_stLeftClickRay;	
}

SRay* QObjCamera::GetRightClickRay()
{
	if ( g_Mouse.right_x!=mouse_right_x || g_Mouse.right_y!=mouse_right_y )
	{
		mouse_right_x = g_Mouse.right_x; mouse_right_y = g_Mouse.right_y;
		D3DXVec3Unproject( &m_stRightClickRay.vPos, &D3DXVECTOR3( (float)mouse_right_x, (float)mouse_right_y,-1.0f), &g_Graph.vp, &matProjPer, &matViewPer, &matIdentity);	
		D3DXVec3Unproject( &m_stRightClickRay.vTar, &D3DXVECTOR3( (float)mouse_right_x, (float)mouse_right_y, 1.0f), &g_Graph.vp, &matProjPer, &matViewPer, &matIdentity);
		D3DXVec3Normalize( &m_stRightClickRay.vDir, &(m_stRightClickRay.vTar - m_stRightClickRay.vPos) );
	}	
	return &m_stRightClickRay;	
}

SRay* QObjCamera::GetOrthoMouseRay()
{
	if ( g_Mouse.x!=ortho_mouse_x || g_Mouse.y!=ortho_mouse_y )
	{
		ortho_mouse_x = g_Mouse.x; ortho_mouse_y = g_Mouse.y;
		D3DXVec3Unproject( &m_stOrthoMouseRay.vPos, &D3DXVECTOR3( (float)ortho_mouse_x, (float)ortho_mouse_y,-1.0f), &g_Graph.vp, &matProjOrtho, &matViewOrtho, &matIdentity);	
		D3DXVec3Unproject( &m_stOrthoMouseRay.vTar, &D3DXVECTOR3( (float)ortho_mouse_x, (float)ortho_mouse_y, 1.0f), &g_Graph.vp, &matProjOrtho, &matViewOrtho, &matIdentity);
		D3DXVec3Normalize( &m_stOrthoMouseRay.vDir, &(m_stOrthoMouseRay.vTar - m_stOrthoMouseRay.vPos) );
	}	
	return &m_stOrthoMouseRay;
}

SRay* QObjCamera::GetOrthoLeftClickRay()
{
	if ( g_Mouse.left_x!=ortho_mouse_left_x || g_Mouse.left_y!=ortho_mouse_left_y )
	{
		ortho_mouse_left_x = g_Mouse.left_x; ortho_mouse_left_y = g_Mouse.left_y;
		D3DXVec3Unproject( &m_stOrthoLeftClickRay.vPos, &D3DXVECTOR3( (float)ortho_mouse_left_x, (float)ortho_mouse_left_y,-1.0f), &g_Graph.vp, &matProjOrtho, &matViewOrtho, &matIdentity);	
		D3DXVec3Unproject( &m_stOrthoLeftClickRay.vTar, &D3DXVECTOR3( (float)ortho_mouse_left_x, (float)ortho_mouse_left_y, 1.0f), &g_Graph.vp, &matProjOrtho, &matViewOrtho, &matIdentity);
		D3DXVec3Normalize( &m_stOrthoLeftClickRay.vDir , &(m_stOrthoLeftClickRay.vTar - m_stOrthoLeftClickRay.vPos) );
	}	
	return &m_stOrthoLeftClickRay;	
}

SRay* QObjCamera::GetOrthoRightClickRay()
{
	if ( g_Mouse.right_x!=ortho_mouse_right_x || g_Mouse.right_y!=ortho_mouse_right_y )
	{
		ortho_mouse_right_x = g_Mouse.right_x; ortho_mouse_right_y = g_Mouse.right_y;
		D3DXVec3Unproject( &m_stOrthoRightClickRay.vPos, &D3DXVECTOR3( (float)ortho_mouse_right_x, (float)ortho_mouse_right_y,-1.0f), &g_Graph.vp, &matProjOrtho, &matViewOrtho, &matIdentity);	
		D3DXVec3Unproject( &m_stOrthoRightClickRay.vTar, &D3DXVECTOR3( (float)ortho_mouse_right_x, (float)ortho_mouse_right_y, 1.0f), &g_Graph.vp, &matProjOrtho, &matViewOrtho, &matIdentity);
		D3DXVec3Normalize( &m_stOrthoRightClickRay.vDir, &(m_stOrthoRightClickRay.vTar - m_stOrthoRightClickRay.vPos) );
	}	
	return &m_stOrthoRightClickRay;	
}