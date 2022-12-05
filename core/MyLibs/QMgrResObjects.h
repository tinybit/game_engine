#define STRICT

#pragma once
#pragma warning(disable: 4238 4389)

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

using namespace std;

class QObjBase;
class QObjMan;
class QObjCar;
class QObjBird;
class QObjFree;
class QObjLight;
class QObjCamera;
class QObjShadowVolume;
class QObjLayerGeom;
class QObjAction;
class QObjEfnAnim;
class QObjVideoAnim;
class QObj3DVideo;
class QObjMenuRukzak;
class QObjMenuCursor;
class QObjMenuMenu;

class QGameBase;

typedef map< std::wstring, QObjBase*, std::less<wstring> > objmap;
typedef objmap::iterator objmap_iter;

//-----------------------ZSortedList-----------------------------------------

typedef list< QObjBase* > listQObjBase;
typedef listQObjBase::iterator listQObjBase_iter;

typedef multimap< float, QObjBase*, std::less<float> > mmapFloatQObjBase;
typedef mmapFloatQObjBase::iterator mmapFloatQObjBase_iter;


class QMgrResObjects
{
public:

	//Constructor/Destructor
	QMgrResObjects();
	~QMgrResObjects();

	std::wstring			GetObjList();
	std::wstring			GetObjGroupsList();

	bool					AddObj( std::wstring obj_id, QObjBase* obj, ObjTypes ot );
	bool					AddObjMan( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	bool					AddObjCar( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	bool					AddObjBird( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	bool					AddObjFree( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, bool bFillGopa );
	bool					AddObjFreeE( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, bool bFillGopa );
	bool					AddObjLight( const std::wstring& obj_id, const D3DLIGHT9* pLight );
	bool					AddObjCam( const std::wstring& obj_id );
	bool					AddObjSV( const std::wstring& obj_id, const std::wstring& efn_id );
	bool					AddObjLG( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& state_id );
	bool					AddObjAction( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& sScript );
	bool					AddObjEfnAnim( const std::wstring& obj_id, std::wstring efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 ) { return AddObjEfnAnimE( obj_id, efn_id, L"", tex_id, tex_id2 ); };
	bool					AddObjEfnAnimE( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& meshes_efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	bool					AddObjVideoAnim( const std::wstring& obj_id );
	bool					AddObj3DVideo( const std::wstring& obj_id, const SNULL* null, const std::wstring& move_efn_id, const std::wstring& frames_efn_id );
	bool					AddObjMenuRukzak( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	bool					AddObjMenuCursor( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2, const std::wstring& rukzak_obj_id );
	bool					AddObjMenuMenu( const std::wstring& obj_id, const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );

	void					RegisterObjectInLua( QObjBase* obj );
	void					DeRegisterObjectInLua( QObjBase* obj );

	void					DelObj( const std::wstring& obj_id );
	void					DelAll();

	inline QObjBase*		GetObj( const std::wstring& obj_id );
	QObjMan*				GetObjMan( const std::wstring& obj_id ) { return (QObjMan*)GetObj(obj_id); };
	QObjCar*				GetObjCar( const std::wstring& obj_id ) { return (QObjCar*)GetObj(obj_id); };
	QObjBird*				GetObjBird( const std::wstring& obj_id ) { return (QObjBird*)GetObj(obj_id); };
	QObjFree*				GetObjFree( const std::wstring& obj_id ) { return (QObjFree*)GetObj(obj_id); };
	QObjLight*				GetObjLight( const std::wstring& obj_id ) { return (QObjLight*)GetObj(obj_id); };
	QObjCamera*				GetObjCam( const std::wstring& obj_id ) { return (QObjCamera*)GetObj(obj_id); };
	QObjShadowVolume*		GetObjSV( const std::wstring& obj_id ) { return (QObjShadowVolume*)GetObj(obj_id); };
	QObjLayerGeom*			GetObjLG( const std::wstring& obj_id ) {	return (QObjLayerGeom*)GetObj(obj_id); };
	QObjAction*				GetObjAction( const std::wstring& obj_id ) {	return (QObjAction*)GetObj(obj_id); };
	QObjEfnAnim*			GetObjEfnAnim( const std::wstring& obj_id ) { return (QObjEfnAnim*)GetObj(obj_id); };
	QObjVideoAnim*			GetObjVideoAnim( const std::wstring& obj_id ) { return (QObjVideoAnim*)GetObj(obj_id); };
	QObj3DVideo*			GetObj3DVideo( const std::wstring& obj_id ) { return (QObj3DVideo*)GetObj(obj_id); };
	QObjMenuRukzak*			GetObjMenuRukzak( const std::wstring& obj_id ) { return (QObjMenuRukzak*)GetObj(obj_id); };
	QObjMenuCursor*			GetObjMenuCursor( const std::wstring& obj_id ) { return (QObjMenuCursor*)GetObj(obj_id); };
	QObjMenuMenu*			GetObjMenuMenu( const std::wstring& obj_id ) { return (QObjMenuMenu*)GetObj(obj_id); };

	mmapWStrWStr*			GetGroups() { return &m_mmGroup; };
	int						GetObjGroup( const std::wstring& obj_group_id, mmapWStrWStr_iter* beg, mmapWStrWStr_iter* end );
	bool					AddObjToGroup( const std::wstring& obj_group_id, const std::wstring& obj_id );
	void					DelObjInGroup( const std::wstring& obj_group_id );
	void					DelGroup( const std::wstring& obj_group_id) { m_mmGroup.erase( obj_group_id ); };
	std::wstring			GetFirstFromGroup( const std::wstring& obj_group_id);

	void					GroupDoFrameMove( const std::wstring& obj_group_id, double fTime, float fElapsedTime );
	void					GroupDoFrameRender( const std::wstring& obj_group_id, double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					GroupDoObjRender( const std::wstring& obj_group_id, DWORD rt );

	void					ClearRenderBuffer() { m_lstRenderBuffer.clear(); };
	void					AddObjToRenderBuffer( const std::wstring& obj_id );
	void					AddGroupToRenderBuffer( const std::wstring& group_id );
	void					CalcZSortedRenderBuffer( const D3DXVECTOR3* pvCamPos );
	void					RenderFromZSortedRenderBuffer( double fTime, float fElapsedTime );
	void					ObjRenderFromZSortedRenderBuffer( DWORD rt );
	
	std::wstring			GetIntersectedObjID( const std::wstring& obj_group_id, const SRay* pstRay );
	bool					CheckForRayIntersect( const std::wstring& obj_id,  const SRay* pstRay );
	D3DXVECTOR3*			GetPosFromIntersected() { return &m_stIntersectedPos.vPos; }
	SNULL*					GetNullFromIntersected() { return &m_stIntersectedPos; }
	float					GetDistFromIntersected() { return m_fIntersectedDist; }

	listQObjBase*			GetRenderBuffer() { return &m_lstRenderBuffer; };

	std::wstring			GetScript( const std::wstring& obj_id );

	bool					GetEnabled( const std::wstring& obj_id );
	void					SetEnabled( const std::wstring& obj_id, bool bEnabled );

	void					SetGame( QGameBase* pGame ) { m_pGame = pGame; };
	QGameBase*				GetGame() { return m_pGame; };

	void					LogEnabled( bool bEnabled );

	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnLostDevice();
	void					OnDestroyDevice();

private:

	objmap					m_mObj;
	mmapWStrWStr			m_mmGroup;

	listQObjBase			m_lstRenderBuffer;
	mmapFloatQObjBase		m_mapZSortedRenderBuffer;

	SNULL					m_stIntersectedPos;
	float					m_fIntersectedDist;

	QGameBase*				m_pGame;

	bool					m_bLogEnabled;

};