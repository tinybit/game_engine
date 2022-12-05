#pragma once

#include "dxstdafx.h"

#include "UtilsDefines.h"

#include "IMgrResTextures.h"

#include "QObjBase.h"

class QObjCamera  : public QObjBase
{
public:

	QObjCamera();
	~QObjCamera() {};

	void					Init();
	void					Kill();

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) { return false; };

	void					SetPP( const D3DXVECTOR3* pvPP, float fElapsedTime );
	void					ResetPP() { m_vPP = stCamera.vPerPos; };
	void					SetTex( const QTextureListItem* t );
	QTextureListItem*		GetTex() { return &m_stTex; };
	
	void					ComputeCam();

	void					SetPerCam();
	void					SetOrthoCam();

	void					SetPerCamPrm( const D3DXVECTOR3* pvPerPos, const D3DXVECTOR3* pvPerTarg, const D3DXVECTOR3* pvPerUpVec, const D3DXVECTOR2* pvPerBackSize, float PerNearPlane, float PerFarPlane, float fFovY );
	void					SetPerCamPos( const D3DXVECTOR3* pvPos );
	void					SetPerCamTarget( const D3DXVECTOR3* pvTarget );

	void					SetOrthoCamPrm( const D3DXVECTOR3* pvOrthoPos, const D3DXVECTOR3* vOrthoTarg, const D3DXVECTOR3* vOrthoUpVec, const D3DXVECTOR2* vOrthoBackSize, float OrthoNearPlane, float OrthoFarPlane );
	void					SetOrthoCamPos( const D3DXVECTOR3* pvPos );
	void					SetOrthoCamTarget( const D3DXVECTOR3* pvTarget );

	void					SetZoomPrm( float fZoomNear, float fZoomFar );

	SRay*					GetMouseRay();
	SRay*					GetLeftClickRay();
	SRay*					GetRightClickRay();

	SRay*					GetOrthoMouseRay();
	SRay*					GetOrthoLeftClickRay();
	SRay*					GetOrthoRightClickRay();

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime ) {};
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort ) {};
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt ) {};

	D3DXMATRIX				matIdentity;

	D3DXMATRIX				matViewPer;
	D3DXMATRIX				matProjPerOrig;
	D3DXMATRIX				matProjPer;
	D3DXMATRIX				matViewProjPer;

	D3DXMATRIX				matViewOrtho;
	D3DXMATRIX				matProjOrthoOrig;	
	D3DXMATRIX				matProjOrtho;

	SCAMERA					stCamera;
	
private:

	D3DXVECTOR3				m_vPP;
			
	D3DXVECTOR3				m_vTempProkz;

	QTextureListItem		m_stTex;

	float					m_fImgScreenMinScale;
	float					m_fImgScreenMaxScale;
	float					m_fOrigScreenMaxScale;
	float					m_fOrigScreenMinScale;
	float					m_fOrigImgMinScale;
	float					m_fDist;
	float					m_fTransX;
	float					m_fTransY;
	float					m_fOrthoL;
	float					m_fOrthoT;
	float					m_fOrthoW;
	float					m_fOrthoH;
	float					m_fOrthoScaleX;
	float					m_fOrthoScaleY;

	float					m_fScale;
	float					m_fScale2;

	D3DXMATRIX				m_matPerScale;
	D3DXMATRIX				m_matPerTrans;

	int						mouse_x, mouse_y;
	int						mouse_left_x, mouse_left_y;
	int						mouse_right_x, mouse_right_y;

	SRay					m_stMouseRay;
	SRay					m_stLeftClickRay;
	SRay					m_stRightClickRay;

	int						ortho_mouse_x, ortho_mouse_y;
	int						ortho_mouse_left_x, ortho_mouse_left_y;
	int						ortho_mouse_right_x, ortho_mouse_right_y;

	SRay					m_stOrthoMouseRay;
	SRay					m_stOrthoLeftClickRay;
	SRay					m_stOrthoRightClickRay;
};