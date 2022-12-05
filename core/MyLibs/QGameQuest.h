#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QGameBase.h"

#include "QGameQuestSceneData.h"

class QGameQuest : public QGameBase
{
public:
	QGameQuest();
	~QGameQuest();

	void					Init();
	void					Kill();

	bool					LoadScene( const std::wstring& scene_id );

	QGameQuestSceneData*	GetScene() { return &m_SceneData; };

	SNULL					GetClosestNull( SNULL* pstNull );

	std::wstring			CamFloorsStr() { return m_sActiveCamera + L".Floors"; }
	std::wstring			CamShadowFloorsStr() { return m_sActiveCamera + L".ShadowFloors"; }
	std::wstring			CamBackStr() { return m_sActiveCamera + L".Background"; }
	std::wstring			CamBackLayerGeomsStr(std::wstring tex_id) { return L"Background_" + tex_id + L".LayerGeoms"; }
	std::wstring			CamLightsStr() { return m_sActiveCamera + L".Lights"; }
	std::wstring			Cam3DStr() { return m_sActiveCamera + L".3D"; }
	std::wstring			CamObjActionsStr() { return m_sActiveCamera + L".ObjActions"; }
	std::wstring			CamShadowVolumesStr() { return m_sActiveCamera + L".ShadowVolumes"; }

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime );
	void					OnLostDevice();
	void					OnDestroyDevice();

	void					SetCam( const std::wstring& sActiveCamera );
	void					SetBackTexCoords( void* pTex );

	bool					CollisionTest( QObjBase* pObj, const D3DXVECTOR3* pvNewPos, std::wstring* pCollisedObjID, D3DXVECTOR3* pvCollisionPos );

	void					SetCamTarget( const std::wstring& obj_id ) { m_sTargetObj = obj_id; };
	std::wstring			GetCamTarget() { return m_sTargetObj; };
	void					SetIntervalForSetCam( float fInterval ) { m_fSetCamInterval = fInterval; };

	void					RenderBack();
	void					RenderLayers();
	void					RenderFloor();
	void					RenderShadow();
	void					MakeShadow();
	void					Render3D();
	void					RenderNulls();
	void					RenderObjActions();
	void					RenderShadowVolumes();

private:

	LPDIRECT3DSURFACE9		back_surf;
	LPDIRECT3DSURFACE9		shadow_surf;

	LPDIRECT3DVERTEXBUFFER9 m_bsVB;

	D3DXMATRIX				m_matShadowViewOrtho;
	D3DXMATRIX				m_matShadowProjOrtho;

	QGameQuestSceneData		m_SceneData;

	bool					m_bCanRenderFloor;
	bool					m_bCanRenderShadow;

	ObjTypes				m_iTargetObjType;
	std::wstring			m_sTargetObj;

	double					m_fLastSetCam;
	float					m_fSetCamInterval;

};