#pragma once

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"

using namespace std;

typedef map< std::wstring, ID3DXEffect*, std::less<wstring> > shadmap;
typedef shadmap::iterator shadmap_iter;

#define g_ShadMgr QMgrResShaders::GetSingleton()

class QMgrResShaders : public Singleton <QMgrResShaders>
{
public:
	
	QMgrResShaders();
	~QMgrResShaders();

	void					LogEnabled(bool);

	void					LoadShader(std::wstring shad_id);
	bool					LoadShader2( std::wstring shad_id, ID3DXEffect** temp );
	void					FreeShader(std::wstring shad_id);
	void					FreeShaders();
	ID3DXEffect*			GetShader(std::wstring shad_id);

	void					CheckForReload( shadmap_iter i );
	
	void					OnCreateDevice();
	void					OnResetDevice();
	void					OnLostDevice();
	void					OnDestroyDevice();

private:

	shadmap					m_mShad;

	bool					m_bLogEnabled;
};