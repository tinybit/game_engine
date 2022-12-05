#define STRICT

#include "QMgrResFiles.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysLog.h"

QMgrResFiles::QMgrResFiles()
{
	m_bLogEnabled = false;
}

QMgrResFiles::~QMgrResFiles()
{
	if ( m_mFile.size() > 0 )
	{
		for( filemap_iter i = m_mFile.begin(); i != m_mFile.end(); i++ )
		{
			QFileListItem* temp = i->second;

			SAFE_DELETE(temp->FileBuffer);
			SAFE_DELETE(temp);
		}
	}
	m_mFile.clear();
}

void QMgrResFiles::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage( L"file mgr logging enabled" );
	}
	else
	{
		g_Log.WriteMessage( L"file mgr logging disabled" );
	}
}

bool QMgrResFiles::LoadFile( const std::wstring& FileName )
{
	if ( m_mFile.size() > 0 )
	{
		filemap_iter i = m_mFile.find( FileName );

		if ( i != m_mFile.end() )
		{
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning( L"file already loaded, filename = \"" + FileName + L"\"" );
			}
			return true;
		}
	}

	FILE* pFileHandle = NULL;

	QFileListItem* FileListItem = new QFileListItem();
	
	FileListItem->FileName		= FileName;
	FileListItem->FileBuffer	= NULL;
	FileListItem->FileSize		= 0;

	if ( FileRead( FileListItem->FileName, FileListItem->FileBuffer, FileListItem->FileSize ) )
	{
		m_mFile[FileListItem->FileName] = FileListItem;
		return true;
	}
	else
	{
		g_Log.WriteError( L"cant read file = \"" + FileListItem->FileName + L"\"" );
		SAFE_DELETE( FileListItem );
		return false;
	}
}

bool QMgrResFiles::SaveFile( const std::wstring& FileName, void* pBuffer, UINT iBufferSize )
{
	try
	{
		FILE* pkFile = _wfopen( FileName.c_str(), L"wb" );
		fwrite(pBuffer, sizeof(char), iBufferSize, pkFile);
		fclose(pkFile);

		return true;
	}
	catch(...)
	{
		g_Log.WriteError( L"failed to write file, FileName = \"" + FileName + L"\"" );
		return false;
	}
}

void QMgrResFiles::FreeFile( const std::wstring& FileName, bool bFreeBuffers )
{
	filemap_iter i = m_mFile.find( FileName );

	if ( i != m_mFile.end() )
	{
		QFileListItem* temp = i->second;

		if ( temp )
		{
			if ( bFreeBuffers ) 
				SAFE_DELETE_ARRAY( temp->FileBuffer );
			SAFE_DELETE( temp );
		}
		m_mFile.erase(i);
	}
}

UCHAR* QMgrResFiles::GetFileBuffer( const std::wstring& FileName )
{
	return GetFileBuffer( FileName, NULL );
}
	
UCHAR* QMgrResFiles::GetFileBuffer( const std::wstring& FileName, UINT* FileSize )
{
	if ( m_mFile.size() > 0 )
	{
		filemap_iter i = m_mFile.find( FileName );

		if ( i != m_mFile.end() )
		{
			QFileListItem* temp = i->second;

			if ( temp )
			{
				if ( FileSize )
				{
					*FileSize = temp->FileSize;
				}
				return temp->FileBuffer;
			}
		}
	}
	return NULL;
}

bool QMgrResFiles::FileRead( const std::wstring& acFilename, UCHAR*& racBuffer, UINT& riSize)
{
    struct _stat kStat;
	if ( _wstat( acFilename.c_str(), &kStat ) != 0 )
    {
        // file does not exist
        racBuffer = 0;
        riSize = 0;
        return false;
    }

	FILE* pkFile = _wfopen( acFilename.c_str(), L"rb" );
    assert( pkFile );
    if ( !pkFile )
    {
        racBuffer = 0;
        riSize = 0;
        return false;
    }

    riSize = kStat.st_size;
    racBuffer = new UCHAR[riSize];
    int iRead = (int)fread(racBuffer,sizeof(char),riSize,pkFile);
    if ( fclose(pkFile) != 0 || iRead != riSize )
    {
        assert( false );
        delete[] racBuffer;
        racBuffer = 0;
        riSize = 0;
        return false;
    }

    return true;
}

bool QMgrResFiles::FileExists( const std::wstring& s )
{
	FILE		*testfile = NULL;

	try
	{
		if( ( testfile = _wfopen( s.c_str(), L"r") ) == NULL )
		{
			return false;
		}
		else
		{
			fclose(testfile);
			return true;
		}
	}
	catch(...)
	{
		return false;
	}
}