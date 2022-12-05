#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "IMgrGC.h"

class QMgrGC : public IMgrGC
{
public:
	QMgrGC();
	~QMgrGC();

	void					LogEnabled(bool);	

	void					Collect( bool bForce );

	void					SetCollectionInterval( float fCollectionInterval ) { m_fCollectionInterval = fCollectionInterval; };
	float					GetCollectionInterval() { return m_fCollectionInterval; };
	float					GetTimeFromLastCollect();

	void					Release() { delete this; };

private:

	bool					m_bLogEnabled;
	
	double					m_fLastCollectTime;
	float					m_fCollectionInterval;

};