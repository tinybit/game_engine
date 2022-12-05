#define STRICT

#include "QSubSysLog.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

QSubSysLog::QSubSysLog( const std::wstring& file )
{
	m_sLastError = L"";
	m_bDebugMode = true;

	logfile_name = "data\\Log\\"+wstr2str(file);
	_ErrorsCount = 0;
	_WarningsCount = 0;

	m_bLineOdd = false;

	try
	{
		logfile = fopen(logfile_name.c_str(), "w");
	}
	catch(...)
	{
		logfile = NULL;
	}

	StartBaner();
}

QSubSysLog::~QSubSysLog()
{
	EndBaner();

	try
	{
		fclose(logfile);
	}
	catch(...)
	{
	}
}

void QSubSysLog::DebugMode(bool bDebugMode)
{
	m_bDebugMode = bDebugMode;
}

void QSubSysLog::StartBaner()
{
	if (logfile!=NULL)
	{
		std::string s = "";
		s += "<html>";
		s += "<title>Log</title>";
		s += "<meta http-equiv=\"Content-Type\"content=\"text/html; charset=windows-1251\">";
		s += "<body bgcolor=\"#000000\" text=\"#aaaaaa\">";
		s += "<pre>";
		s += "<h1>Work Log ("+GetTime()+")</h1><br>";
		s += "<table width=\"100%\" bgcolor=\"#999999\">";
		try
		{
			fwrite(s.c_str(), sizeof(char), s.length(), logfile);
		}
		catch(...)
		{
			ShowMessage(L"Cant start log");
		}
	}
	else
	{
		ShowMessage(L"Cant start log");
	}
}

void QSubSysLog::EndBaner()
{
	if (logfile!=NULL)
	{
		std::string s = "";
		s += "</table>";
		s += "<B>"+int2str(_ErrorsCount)+" error(s), " + int2str(_WarningsCount) + " warning(s)";
		s += "</B></pre></body></html>";
		try
		{
			fwrite(s.c_str(), sizeof(char), s.length(), logfile);
		}
		catch(...)
		{
		}
	}
	else
	{
		ShowMessage(L"Cant end log");
	}
}

//---------------------------------------------------------------------------

std::string QSubSysLog::GetTime()
{
    SYSTEMTIME time;
    GetSystemTime(&time);

	return int2str(time.wHour, 2)+":"+int2str(time.wMinute, 2)+":"+int2str(time.wSecond,2) + ":"+int2str(time.wMilliseconds,3);
}

void QSubSysLog::SetPrefix( const std::wstring& s )
{
	m_sPrefix = wstr2str(s);
}

void QSubSysLog::Write( const std::wstring& sColor, const std::wstring& sMsg )
{
	if (logfile==NULL)
	{
		return;
	}
	m_bLineOdd = !m_bLineOdd;

	std::string s = "";
	std::string sPrefix = "";
	if(m_sPrefix=="")
	{
		sPrefix = "<font font size = 2 color=\"#FFFFFF\" face = Verdana>root>&nbsp;</font>";
	}
	else
	{
		sPrefix = "<font font size = 2 color=\"#9999FF\" face = Verdana>"+m_sPrefix+">&nbsp;</font>";
	}

	if(m_bLineOdd)
	{
		s += "<tr bgcolor=\"#000000\"><td>";
	}
	else
	{
		s += "<tr bgcolor=\"#000000\"><td>";
	}

	if ( sMsg.length()<255 )
	{
		s += GetTime()+"&nbsp;</td><td>" + sPrefix + "</td><td width=100%><font size = 2 Color = \"" + wstr2str(sColor) + "\" face = Verdana>" + wstr2str(sMsg) + "</font>";
	}
	else
	{
		s += GetTime()+"&nbsp;</td><td>" + sPrefix + "</td><td width=100%>";
		s += "<textarea style=\"{width: 100%;}\" rows=5>"+wstr2str(sMsg)+"</textarea>";
	}
	s += "</td></tr>\r\n";

	try
	{
		fwrite(s.c_str(), sizeof(char), s.length(), logfile);
		fwrite(" ", sizeof(char), 1, logfile);
		if(m_bDebugMode)
		{
			fclose(logfile);
			logfile = fopen(logfile_name.c_str(), "a");
		}
	}
	catch(...)
	{
	}
}

void QSubSysLog::WriteMessage( const std::wstring& sMsg )
{
	Write(L"#00FF00", sMsg);
}

void QSubSysLog::WriteWarning( const std::wstring& sMsg )
{
	Write(L"#FFFF00", sMsg);
    ++_WarningsCount;
}

void QSubSysLog::WriteError( const std::wstring& sMsg )
{
	m_sLastError = sMsg;
	Write(L"#FF0000", sMsg);
    ++_ErrorsCount;
}