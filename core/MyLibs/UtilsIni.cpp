#define STRICT

#include "UtilsIni.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

void CIniFile::WriteString (std::wstring section, std::wstring key, std::wstring value)
{
	WritePrivateProfileString ( section.c_str(), key.c_str(), value.c_str(), m_sFileName.c_str() );
}

std::wstring CIniFile::GetString ( std::wstring section, std::wstring key, std::wstring def )
{
	WCHAR cBuffer[256];
	
	int read=GetPrivateProfileString( section.c_str(), key.c_str(), NULL, cBuffer, 255, m_sFileName.c_str() );

	if (read!=0)
	{
		return cBuffer;
	}
	else
	{
		return def;	
	}
}

int CIniFile::GetInt (std::wstring section, std::wstring key, int def)
{
	return GetPrivateProfileInt ( section.c_str(), key.c_str(), def, m_sFileName.c_str() );
}

float CIniFile::GetFloat ( std::wstring section, std::wstring key, float def )
{
	std::wstring s = GetString( section, key, float2wstr(def) );
	return wstr2float(s);
}
