#pragma once

#include "QInterfaces.h"

#include "bink.h"

struct QBinkListItem
{
	std::wstring		bink_id;
	UINT				width;
	UINT				height;
	bool				HaveAlpha;
	float				fFrameRate;
	UINT				iFramesCount;

	D3DFORMAT			format;
	UINT				bink_surface_type;
	UINT				pixel_size;

	int					iNowFrame;

	HBINK				pBink;
};

#define g_BinkMgr IMgrResBinks::GetSingleton()

struct IMgrResBinks : public IMyUnknown, Singleton <IMgrResBinks>
{
	virtual QBinkListItem*			LoadVideo( const std::wstring& bink_id, bool bPrechache  ) = 0;
	virtual void					FreeVideo( const std::wstring bink_id ) = 0;

	virtual bool					DecompressFrame( const std::wstring& bink_id, void* pTexItem, int fFrame ) = 0;

	virtual void					LogEnabled(bool) = 0;
};
typedef IMgrResBinks* LPMGRRESBINKS;