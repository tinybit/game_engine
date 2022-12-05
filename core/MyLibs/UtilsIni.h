#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

class CIniFile
{
public :

	void			SetFileName (std::wstring filename) { m_sFileName = filename; };
	void			GetFileName (std::wstring& filename) { filename = m_sFileName; };

	void			WriteString (std::wstring section, std::wstring key, std::wstring value);

	std::wstring	GetString ( std::wstring section, std::wstring key, std::wstring def);
	int				GetInt	  ( std::wstring section, std::wstring key, int def);
	float			GetFloat  ( std::wstring section, std::wstring key, float def );

private:

	std::wstring m_sFileName;

};