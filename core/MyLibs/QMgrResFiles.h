#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include <sys/stat.h>

#include "IMgrResFiles.h"

using namespace std;

typedef map< std::wstring, QFileListItem*, std::less<wstring> > filemap;
typedef filemap::iterator filemap_iter;

class QMgrResFiles : public IMgrResFiles
{
public:
	
	QMgrResFiles();
	~QMgrResFiles();

	bool					LoadFile( const std::wstring& FileName );
	bool					SaveFile( const std::wstring& FileName, void* pBuffer, UINT iBufferSize );
	void					FreeFile( const std::wstring& FileName, bool bFreeBuffers );
	
	UCHAR*					GetFileBuffer( const std::wstring& FileName );
	UCHAR*					GetFileBuffer( const std::wstring& FileName, UINT* FileSize );
	
	bool					FileRead( const std::wstring& acFilename, UCHAR*& racBuffer, UINT& riSize);
	bool					FileExists( const std::wstring& s );

	void					LogEnabled(bool);

	void					Release() { delete this; };

private:

	filemap					m_mFile;;

	bool					m_bLogEnabled;
};