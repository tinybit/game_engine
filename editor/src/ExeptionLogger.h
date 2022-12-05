#pragma once

#include "sys/StdAfx.h"
#include "utils/common.h"

#define g_ExeptLog CExeptionLogger::GetSingletonPtr()

class CExeptionLogger : public Singleton<CExeptionLogger>
{
	public:
		CExeptionLogger(void);
		~CExeptionLogger(void);

		string GetExeptionLog();

		vector<string> __data;
};
