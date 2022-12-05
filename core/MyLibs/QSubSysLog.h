#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "CommonDefines.h"

#define g_Log QSubSysLog::GetSingleton()

class QSubSysLog : public Singleton <QSubSysLog>
{
public:

	//Constructor/Destructor
	QSubSysLog( const std::wstring& file );
	~QSubSysLog();

	void			StartBaner();
	void			EndBaner();

	void			SetPrefix( const std::wstring& s );
	void			Write( const std::wstring& sColor, const std::wstring& sMsg );
	void			WriteMessage( const std::wstring& sMsg );
    void			WriteWarning( const std::wstring& sMsg );
    void			WriteError( const std::wstring& sMsg );

	std::wstring	GetLastError() { return m_sLastError; };

	void			DebugMode(bool bDebugMode);

	int				GetErrorCount() { return _ErrorsCount; };
	int				GetWarningsCount() { return _WarningsCount; };


private:

	bool			m_bDebugMode;

	std::wstring	m_sLastError;

	FILE*			logfile;

	bool			m_bLineOdd;
	std::string		logfile_name;
	std::string		m_sPrefix;

    int				_ErrorsCount;
    int				_WarningsCount;

    //пишем в лог текущее время
	std::string		GetTime();
};