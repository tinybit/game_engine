#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

#include "QObjBase.h"
#include "QObjEfnAnim.h"

using namespace std;

struct SRukzakItem
{
	std::wstring	item_id;
	std::wstring	item_name;
	std::wstring	efn_id;
	std::wstring	tex_id;
	bool			bTexSphereMapping;
	std::wstring	tex_id2;
	bool			bTex2SphereMapping;
	D3DMATERIAL9	mtrl;
	bool			bSetMtrl;
	bool			bFillZBuffer;

	vecWStrWStr		lstAttached;
};

typedef std::map< wstring, SRukzakItem, std::less<wstring> > mapWStrSRukzakItem; 
typedef mapWStrSRukzakItem::iterator mapWStrSRukzakItem_iter;

class QObjMenuRukzak : public QObjBase
{
public:

	QObjMenuRukzak( const std::wstring& efn_id, const std::wstring& tex_id, const std::wstring& tex_id2 );
	~QObjMenuRukzak();

	void					Init();
	void					Kill();

	//void					Go( double fTime, int seq_id );
	//void					Stop(bool bOK);

	void					AddListItem( const std::wstring& item_id, const std::wstring& item_name, const std::wstring& efn_id, const std::wstring& tex_id, bool bTexSphereMapping, const std::wstring& tex_id2, bool bTex2SphereMapping, const D3DMATERIAL9* pMtrl, bool bFillZBuffer );
	SRukzakItem*			GetListItem( const std::wstring& item_id );

	std::wstring			GetItemName( const std::wstring& item_id );

	void					AddChildToListItem( const std::wstring& item_id, const std::wstring& null_id, const std::wstring& child_item_id );

	void					AddItem( const std::wstring& item_id );
	void					RemoveItem( const std::wstring& item_id, bool bUsed );

	void					SetZoomToItem( const std::wstring& item_id );
	void					ClearZooms();

	bool					IsOpened() { return m_bOpened; };
	void					Open();
	void					Close();

	void					FillRukzak();
	bool					RukzakAttach( QObjEfnAnim* efn, const std::wstring& null_id, const std::wstring& obj_id, SRukzakItem* temp );

	bool					CheckForRayIntersect( const SRay* pstRay, float* pfDist, D3DXVECTOR3* pvIntrPos );
	std::wstring			GetClickedItemID( const SRay* pstRay );
	std::wstring			GetItemIDFromNum( int num );
	
	void					ObjRender( DWORD rt ) {	if ( GetEfnAnim() )	GetEfnAnim()->ObjRender( rt ); };

	void					OnCreateDevice() {};
	void					OnResetDevice() {};
	void					OnFrameMove( double fTime, float fElapsedTime );
	void					OnFrameRender( double fTime, float fElapsedTime, ObjRenderTypes ort );
	void					OnLostDevice() {};
	void					OnDestroyDevice() {};

private:

	bool					m_bOpened;

	int						m_iCounter;

	mapWStrSRukzakItem		m_mRukzakItemsList;

	mapWStrInt				m_mRukzakItems;
	mapIntWStr				m_mRukzakItemsSorted;

	D3DXMATRIX				m_matScale;

};