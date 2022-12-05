#pragma once

#include "QInterfaces.h"

struct QFileListItem
{
	std::wstring		FileName;
	UINT				FileSize;
	UCHAR*				FileBuffer;
};

#define g_FileMgr IMgrResFiles::GetSingleton()

struct IMgrResFiles : public IMyUnknown, Singleton <IMgrResFiles>
{
	virtual bool					LoadFile( const std::wstring& FileName ) = 0;
	virtual bool					SaveFile( const std::wstring& FileName, void* pBuffer, UINT iBufferSize ) = 0;
	virtual void					FreeFile( const std::wstring& FileName, bool bFreeBuffers ) = 0;
	
	virtual UCHAR*					GetFileBuffer( const std::wstring& FileName ) = 0;
	virtual UCHAR*					GetFileBuffer( const std::wstring& FileName, UINT* FileSize ) = 0;

	virtual bool					FileExists( const std::wstring& FileName ) = 0;

	virtual void					LogEnabled(bool) = 0;
};
typedef IMgrResFiles* LPMGRRESFILES;