#pragma once

#include "QInterfaces.h"

#define g_GC IMgrGC::GetSingleton()

struct IMgrGC : public IMyUnknown, Singleton <IMgrGC>
{
	virtual void					LogEnabled(bool) = 0;	

	virtual void					Collect( bool bForce ) = 0;

	virtual void					SetCollectionInterval( float fCollectionInterval ) = 0;
	virtual float					GetCollectionInterval() = 0;
	virtual float					GetTimeFromLastCollect() = 0;

};
typedef IMgrGC* LPMGRGC;