#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "QObjBase.h"

//-------------------------anim controller-------------------------------
struct SEfnAnimSeq
{
	UINT					iType;
	std::wstring			sEfnID;
	std::wstring			sMeshesEfnID;
	std::wstring			sAnimName;
	float					fAnimFrameRate;
	double					fAnimGoTime;
	float					fAnimRotSpeed;
	int						iAnimFramesCount;
	int						iAnimRepeatTimes;
	double					fAnimTimeLength;
	double					fAnimTotalTimeLength;
	double					fAnimZLength;
	double					fAnimTotalZLength;
	double					fAnimGoZ;
	std::wstring			sScript;
	bool					bScriptExecuted;
};

typedef std::vector<SEfnAnimSeq> vecSEfnAnimSeq;

//-------------------------attach-----------------------------------------
struct SObjNullAttaches
{
	std::wstring			null_id;
	std::wstring			obj_id;
	QObjEfnAnim*			pObjEfnAnim;

	bool					bModifyNull;
	D3DXMATRIX				matModify;
};											  

typedef multimap< wstring, SObjNullAttaches, std::less< wstring > > mmapObjNullAttaches;
typedef mmapObjNullAttaches::iterator mmapObjNullAttaches_iter;

//------------------------path last events--------------------------------
struct SPathLastEvent
{
	double			fPathLast;
	std::wstring	sScript;
};

typedef list<SPathLastEvent> listSPathLastEvent;
typedef listSPathLastEvent::iterator listSPathLastEvent_iter;

//--------------------------class-------------------------------------------
class QObjEfnAnim : public QObjBase
{
public:

	QObjEfnAnim( const std::wstring& efn_id, const std::wstring& meshs_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	~QObjEfnAnim();

	void					Init();
	void					Kill();

	void					AnimAdd( const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes );
	void					AnimAdd2( const std::wstring& efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes );
	void					AnimAdd3( const std::wstring& efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes, const std::wstring& sScript );
	void					MorfAnimAdd( const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes );
	void					MorfAnimAdd2( const std::wstring& efn_id, const std::wstring& meshs_efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes );
	void					MorfAnimAdd3( const std::wstring& efn_id, const std::wstring& meshs_efn_id, const std::wstring& sAnimName, float fAnimRotSpeed, int iAnimRepeatTimes, const std::wstring& sScript );
	void					Go( double fTime, double fPathLength, int seq_id );
	void					Stop(bool bOK);

	SEfnAnimSeq*			GetEfnAnimTemp() { return EfnAnimTemp; };
	double					GetOffsetOnPath() { return m_fOffsetOnPath; };
	double					GetPathLength() { return m_fPathLength; };
	double					GetPathLast() { return m_fPathLength - m_fOffsetOnPath; };
	int						GetSeqID() { return m_iSeqID; };
	
	int						GetFrame() { return m_iFrame; };
	int						GetNextFrame() { return m_iNextFrame; };
	float					GetTweenFactor() { return m_fTweenFactor; };

	void					SetFrame( int iFrame ) { m_iFrame = iFrame; };
	void					SetNextFrame( int iFrame ) { m_iNextFrame = iFrame; };
	void					SetTweenFactor( float fTweenFactor ) { m_fTweenFactor = fTweenFactor; };

	bool					AttachObjToNull( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id );
	bool					AttachObjToNullC( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id, const D3DXCOLOR* pstColor );
	bool					AttachObjToNullE( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id );
	bool					AttachObjToNullEC( const std::wstring& null_id, const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& attach_null_id, const D3DXCOLOR* pstColor );
	void					DeAttachObjFromNull( const std::wstring& null_id, const std::wstring& obj_id );
	void					DeAttachObjsFromNull( const std::wstring& null_id );
	void					DeAttachAll();

	void					SetNullModifyMatrix( const std::wstring& null_id, const D3DXMATRIX* pmatModify );
	void					SetModifyMatrix( const std::wstring& null_id, const std::wstring& obj_id, const D3DXMATRIX* pmatModify );
	void					ClearModifyFlag();

	void					AddObjEfnAnimToRenderBuffer();

	void					SetPos( const SNULL* stNull );
	void					SetCoords( const D3DXVECTOR3* pvPos, const D3DXVECTOR3* pvScale, const D3DXVECTOR3* pvRot );
	void					SetPosition( const D3DXVECTOR3* pvPos );
	void					SetScale( const D3DXVECTOR3* pvScale );
	void					SetRotation( const D3DXVECTOR3* pvRot );
	void					SetWorld( const D3DXMATRIX* pmatWorld );

	void					CalcChildsObjEfnAnimCoords();

	std::wstring			GetAnimEfnID() { if ( EfnAnimTemp && vecAnim.size()>0 ) { return EfnAnimTemp->sEfnID; } else { return GetEfnID(); } }
	std::wstring			GetAnimName() { if ( EfnAnimTemp && vecAnim.size()>0 ) { return EfnAnimTemp->sAnimName; } else { return L""; } };

	bool					IsStarted() { return m_bStarted; };

	void					SetRenderPrms( bool bRenderRoot, bool bRenderChilds, bool bCalcChildsInOnFrameMove ) { m_bRenderRoot = bRenderRoot; m_bRenderChilds = bRenderChilds; m_bCalcChildsInOnFrameMove = bCalcChildsInOnFrameMove; };
	void					SetSendAnimsToChilds( bool bSend ) { m_bSendAnimsToChilds = bSend; };
	void					SetZeroAnim( bool bZero ) { m_bZeroAnim = bZero; };

	void					SetCullNone( bool b ) { m_bCullNone = b; };
	bool					GetCullNone() { return m_bCullNone; };
	
	void					SetFillZBuffer( bool bFillZBuffer ) { m_bFillZBuffer = bFillZBuffer; };
	bool					GetFillZBuffer() { return m_bFillZBuffer; };

	void					SetTexSphereMapping( bool bSphereMapping ) { m_bTexSphereMapping = bSphereMapping; };
	void					SetTex2SphereMapping( bool bSphereMapping ) { m_bTex2SphereMapping = bSphereMapping; };

	void					SetEfnAnimEnabled( bool bEnabled );
	void					SetEfnAnimVisible( bool bVisible );

	std::wstring			GetFirstFreeNullID();

	mmapObjNullAttaches*	GetAttachedObjList() { return &m_mmObjAttaches; };

	std::wstring			GetNullIDForAttachedObj( const std::wstring& obj_id );

	void					AddPathLastEvent( double fPathLast, const std::wstring& sScript );
	void					DoPathLastEvents();
	void					ClearPathLastEvents() { m_lstPathLastEvents.clear(); };

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos );

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

	void					ObjRender( DWORD rt );
	void					RenderRoot();

private:

	vecSEfnAnimSeq			vecAnim;
	SEfnAnimSeq*			EfnAnimTemp;

	listSPathLastEvent		m_lstPathLastEvents;

	bool					m_bCalcChildsInOnFrameMove;

	bool					m_bStarted;

	bool					m_bRenderRoot;
	bool					m_bRenderChilds;
	bool					m_bFillZBuffer;
	bool					m_bSendAnimsToChilds;
	bool					m_bZeroAnim;
	bool					m_bCullNone;

	mmapObjNullAttaches		m_mmObjAttaches;

	std::wstring			m_sDefaultEfnID;
	bool					m_bTexSphereMapping;
	bool					m_bTex2SphereMapping;

	std::wstring			m_sSoundScript;

	int						m_iSeqID;
	double					m_fOffsetOnPath;
	double					m_fPathLength;
	double					m_fAnimSliderKoef;

	double					m_fMorfLastTime;

	double					m_fMyTimeElapsed;
	
	int						m_iAnimI;
	double					m_fAnimFrameIndex;

	double					m_fFrame;
	int						m_iFrame;
	int						m_iNextFrame;
	float					m_fTweenFactor;
	int						m_iFrameSoundPlaying;

	D3DXMATRIX				matTrans;
	D3DXMATRIX				matScale;
	D3DXMATRIX				matRotX;
	D3DXMATRIX				matRotY;
	D3DXMATRIX				matRotZ;
	D3DXMATRIX				matRot;

	D3DXMATRIX				matWorld;
};