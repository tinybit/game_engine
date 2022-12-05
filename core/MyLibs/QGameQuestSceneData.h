#pragma once
#pragma warning( disable: 4018 4244 )

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QMgrResObjects.h"

#define CANTWALK2	4294967295	//2^32-1
#define CANTWALK	4294967294	//2^32-2
#define CANWALK		4294967293	//2^32-3
#define STARTPOINT	4294967292	//2^32-4
#define TARGETPOINT	0

struct pt
{
	iVECTOR2		v;
	UINT			a;
};

struct SSearch
{
	UINT* m_search;
	vector<iVECTOR2> p;
	pt* arr;
};

class QGameQuestSceneData
{
public:
	QGameQuestSceneData();
	~QGameQuestSceneData();

	bool					LoadScene( const std::wstring& scene_id );
	void					FreeScene();

	D3DXVECTOR3				scene_pos;
	D3DXVECTOR3				scene_size;
	D3DXVECTOR3				scene_max;
	int						iXSize;
	int						iZSize;
	int**					pTriGrid;
	UINT*					pSrcGlobalMatrix;
	UINT*					pGlobalMatrix;
	UINT*					pCamMatrix;
	UINT*					pShadowMatrix;

	inline void				CheckPointForOK( const UINT* m_search, const D3DXVECTOR3* pvOrig, int x, int z, D3DXVECTOR3* pvNew, float* pfDist, int* piDiap );
	bool					GetClosestPosition( const UINT* m_search, D3DXVECTOR3* pvTarget);
	void					CheckFindPlace( UINT* grid, vector<iVECTOR2>* p, int x, int z, UINT num, bool* m_path_finded);
	
	bool					ManFindPath( const std::wstring& obj_id, const SNULL* pstTarget );
	bool					BirdFindPath( const std::wstring& obj_id, const SNULL* pstTarget );
	bool					CarFindPath( const std::wstring& obj_id, const SNULL* pstTarget);
	bool					FindPath( const SNULL* pstStart, const SNULL* pstTarget, bool* pGopa, int iGopaSize, cpvector* cp );

	QMgrResObjects*			GetObjMgr() { return m_pObjMgr; };
	void					SetObjMgr( QMgrResObjects* pObjMgr ) { m_pObjMgr = pObjMgr; };
	
	void					OptimizePath( UINT* m_search, cpvector& cp);
	bool					checkline( UINT* m_search, iVECTOR2 n, iVECTOR2 k);
	bool					segment2segment( const D3DXVECTOR2* a, const D3DXVECTOR2* b, const D3DXVECTOR2* c, const D3DXVECTOR2* d );
	
	void					FillMSearch(UINT* m_search, UINT* pGlb, UINT* pSrc, bool* pGopa, int iGopaSize);
	
	bool					MakeGopa(bool** pGopa, int* iGopaSize, float fGopaRadius );
	void					FillGopa( int x, int z, UINT* m_search, bool* gopa, int cells_count, UINT iOp );
	void					FillGopa2( int x, int z, UINT* m_search, bool* gopa, int cells_count, UINT iOp );
	void					FillGopa3( int x, int z, UINT* m_search, bool* gopa, int cells_count, UINT iOp );
	bool					TestGopa( int x, int z, UINT* m_search, bool* gopa, int cells_count );
	void					FillFloorTex( UINT* temp, long iValue );

	SNULL					GetNull( const std::wstring& null_id );
	
	UINT					GetCamForPos( const D3DXVECTOR3* _pos );
	int						GetPosType( UINT* grid, const D3DXVECTOR3* _pos );
	int						GetGridPosType( UINT* grid, int _x, int _z);
	
	void					GetGridFromPos( const D3DXVECTOR3* _pos, int& _x, int& _z );
	D3DXVECTOR3				GetPosFromGrid( int _x, int _z );
	D3DXVECTOR3				GetPosFromGrid2( int _x, int _z );

	float					GetYFromPos( const D3DXVECTOR3* vPos );
	float					GetYFromGrid( int _x, int _z );

	float					GetCellSize() { return m_fCellSize; };

	bool					isSceneLoaded() { return m_bSceneLoaded; }

	std::wstring			m_sEfnID;

	int						m_iShadowTexWidth;
	int						m_iShadowTexHeight;

	void					LogEnabled( bool bLogEnabled );

private:

	QMgrResObjects*			m_pObjMgr;

	int						m_iCamsCount;
	int						m_iLightsCount;
	int						m_iTexCount;
	int						m_iBindsCount;

	float					m_fCellSize;

	bool					m_bSceneLoaded;

	std::wstring			m_sSceneName;

	bool					LoadGlobalMatrix( ifstream &file );
	void					OptimizeGlobalMatrix( UINT*	pGlobalMatrix );
	bool					CheckRectIsCanWalkExists(  UINT* pGlobalMatrix, int x, int z );
	void					AddBinds( vecWStr& s );

	bool					m_bLogEnabled;

	SSearch*				m_pSearch;
	int						m_search_use;
	int						m_search_count;

};