#pragma once

#include "utils/common.h"

struct EFN_ANIMATION
{
	EFN_ANIMATION()
	{
		name			= "";		//animation name
		frames_list		= NULL;		//frames index list 
		frames_count	= 0;		//frames total count
		IsTransition	= false;
	}

	str_t	name;			//animation name
	int*	frames_list;	//frames index list 
	int		frames_count;	//frames total count
	bool	IsTransition;
};