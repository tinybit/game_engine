#include "ExeptionLogger.h"
#include "dialogs\CookieMachineDlg.h"

CExeptionLogger::CExeptionLogger(void)
{
}

CExeptionLogger::~CExeptionLogger(void)
{
}

string CExeptionLogger::GetExeptionLog()
{
	string strExeption = "Stack contents: \r\n\r\n";

	for( int i = (int)(__data.size()) - 1; i >= 0; i-- )
	{
		if( i == 0 )
			strExeption += __data[i];
		else
			strExeption += __data[i] + " ->\r\n";
	}

	return strExeption;
}