#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

struct IMyUnknown
{
	virtual void Release() = 0;
};

struct IMyD3DUnknown : public IMyUnknown
{
	virtual void					OnCreateDevice() = 0;
	virtual void					OnResetDevice() = 0;
	virtual void					OnLostDevice() = 0;
	virtual void					OnDestroyDevice() = 0;
};

struct IMyD3DRender : public IMyD3DUnknown
{
	virtual void					OnFrameMove( double fTime, float fElapsedTime ) = 0;
	virtual void					OnFrameRender( double fTime, float fElapsedTime ) = 0;
};