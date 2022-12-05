#pragma once

#include <string>
#include "bink.h"

struct SBINKINF
{
	int			iWidth;
	int			iHeight;

	int			iWidthPow2;
	int			iHeightPow2;
	
	bool		bHaveAlpha;
	float		fFrameRate;
	int			iFramesCount;

	int			iPixelCount;
};

bool GetBinkInfo( const std::string& sBinkID, SBINKINF* pInf );