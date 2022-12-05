#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjBase.h"
#include "QObjEfnAnim.h"

#include "QGameQuestSceneData.h"

using namespace std;

class QObjMan : public QObjBase
{
public:

	QObjMan( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	~QObjMan();

	void					Init();
	void					Kill();

	void					Go( double fTime, bool bRun, int seq_id );
	void					Stop( bool bOK );

	SNULL*					GetTarget() { return &m_stTarget; };
	bool					GetManRun() { return m_bRun; };

	bool*					GetGopa() { return m_pGopa; };
	int						GetGopaSize() { return m_iGopaSize; };
	cpvector*				GetCP() { return &cp; };
	double					GetPathLength() { if(cp.size()>0) { return cp[cp.size()-1].l; } else { return 0.0; } };
	float					GetAngle() { return GetNullPos()->vRot.y; }
	float					GetNewAngle() { return m_stNewPos.vRot.y; }

	void					SetPos( const SNULL* pstNull );
	void					Teleport( const SNULL* pstNull );

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos ) {	return ( GetEfnAnim() ? GetEfnAnim()->CheckForRayIntersect( pstRay, pfDist, pvIntrPos ) : false ); };
	
	void					OnCollision( const std::wstring& obj_id, const D3DXVECTOR3* pvCollisionPos );

	void					ObjRender( DWORD rt ) { if ( GetEfnAnim() )	GetEfnAnim()->ObjRender( rt ); };

	void					RenderPath();
	bool					GetNewCoords();
	float					GetAngleDif();

	SNULL*					Path( int iIndex );

	void					OnCreateDevice();
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					OnLostDevice() {};
	void					OnDestroyDevice();

private:

	SNULL					m_stNewPos;

	SNULL					m_stTarget;
	bool					m_bRun;

	cpvector				cp;
	double					m_fOffsetOnPath;

	bool*					m_pGopa;
	int						m_iGopaSize;

	LPD3DXMESH				m_pBoxMesh;

	D3DMATERIAL9			m_stMtrl;
	D3DMATERIAL9			m_stMtrl2;

	QGameQuestSceneData*	m_pQuestScn;

};