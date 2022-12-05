#pragma once
#pragma warning( disable : 4101 )
 
#include "dxstdafx.h"
#include "dsound.h"
#include "stl_inc.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

#include "IMgrResBinks.h"

using namespace std;

typedef std::map< wstring, QBinkListItem*, std::less<wstring> > mapWStrQBinkListItem;
typedef mapWStrQBinkListItem::iterator   mapWStrQBinkListItem_iter;

class QMgrResBinks : public IMgrResBinks
{
public:
	QMgrResBinks();
	~QMgrResBinks();

	QBinkListItem*			LoadVideo( const std::wstring& bink_id, bool bPrechache );
	void					FreeVideo( const std::wstring bink_id);

	bool					DecompressFrame( const std::wstring& bink_id, void* pTexItem, int fFrame );

	std::wstring			GetFileByID( const std::wstring& bink_id );

	bool					LockTexture( void* pTexItem, QBinkListItem* pBinkListItem, void* out_pixel_buffer, u32* out_buffer_pitch, u32* bink_surface_type, u32* src_x, u32* src_y, u32* src_w, u32* src_h );

	void					Release() { delete this; };

	void					LogEnabled(bool);

private:

	mapWStrQBinkListItem	m_mBinks;

	LPDIRECTSOUND8			m_pDirectSound;

	bool					m_bLogEnabled;

};