#define STRICT

#include "QMgrResShaders.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysLog.h"

QMgrResShaders::QMgrResShaders()
{
	m_bLogEnabled = false;
}

QMgrResShaders::~QMgrResShaders()
{
	FreeShaders();
}

void QMgrResShaders::LoadShader(std::wstring shad_id)
{
	if ( m_mShad.size() > 0 )
	{
		shadmap_iter i = m_mShad.find(shad_id);

		if ( i != m_mShad.end() )
		{
			return;
		}
	}
	g_Graph.RenderLoad( L"Load shader: " + shad_id );

	ID3DXEffect* temp = NULL;

	if ( LoadShader2( shad_id, &temp ) )
	{
		m_mShad[shad_id] = temp;

		if ( m_bLogEnabled )
		{
			g_Log.WriteWarning( L"shader loaded, shad_id = \"" + shad_id + L"\"" );
		}
	}
	else
	{
		g_Log.WriteError( L"shader load error, shad_id = \"" + shad_id + L"\"" );
	}
}

bool QMgrResShaders::LoadShader2( std::wstring shad_id, ID3DXEffect** temp )
{
	DWORD dwShaderFlags = D3DXSHADER_NO_PRESHADER | D3DXSHADER_PREFER_FLOW_CONTROL;

	std::wstring filename =  L"data\\Shaders\\" + shad_id + L".fx";
	return SUCCEEDED( D3DXCreateEffectFromFile( g_Graph.dev(), filename.c_str(), 
											NULL, NULL, dwShaderFlags, NULL, temp, NULL ) );
}

void QMgrResShaders::FreeShader(std::wstring shad_id)
{
	if ( m_mShad.size() > 0 )
	{
		shadmap_iter i = m_mShad.find(shad_id);

		if ( i != m_mShad.end() )
		{
			SAFE_RELEASE(i->second);
			m_mShad.erase(shad_id);
			
			if ( m_bLogEnabled )
			{
				g_Log.WriteWarning( L"shader free. shad_id = \"" + shad_id + L"\"" );
			}
		}
	}
}

void QMgrResShaders::FreeShaders()
{
	while ( m_mShad.size() > 0 )
	{
		FreeShader( m_mShad.begin()->first );
	}
}
	
ID3DXEffect* QMgrResShaders::GetShader(std::wstring shad_id)
{
	if ( m_mShad.size() > 0 )
	{
		shadmap_iter i = m_mShad.find(shad_id);

		if ( i != m_mShad.end() )
		{
			return i->second;
		}
	}
	return NULL;
}

void QMgrResShaders::CheckForReload( shadmap_iter i )
{
	if ( !i->second )
	{
		if ( LoadShader2( i->first, &i->second ) )
		{
			if(m_bLogEnabled)
			{
				g_Log.WriteWarning(L"Shader was reloaded, shad_id = \"" + i->first + L"\"");
			}
		}
		else
		{
			g_Log.WriteError(L"Shader reload failed, shad_id = \"" + i->first + L"\"");
		}
	}
	else
	{
		if(m_bLogEnabled)
		{
			g_Log.WriteWarning(L"Shader is already loaded, no need to load it again, shad_id = \"" + i->first + L"\"");
		}
	}
}

void QMgrResShaders::OnCreateDevice()
{
	if ( m_mShad.size()>0 )
	{
		for ( shadmap_iter i = m_mShad.begin(); i != m_mShad.end(); i++ )
		{
			CheckForReload(i);
		}
	}
}

void QMgrResShaders::OnResetDevice()
{
	if ( m_mShad.size()>0 )
	{
		for ( shadmap_iter i = m_mShad.begin(); i != m_mShad.end(); i++ )
		{
			i->second->OnResetDevice();
		}
	}
}

void QMgrResShaders::OnLostDevice()
{
	if ( m_mShad.size()>0 )
	{
		for ( shadmap_iter i = m_mShad.begin(); i != m_mShad.end(); i++ )
		{
			i->second->OnLostDevice();
		}
	}
}

void	QMgrResShaders::OnDestroyDevice()
{
	if ( m_mShad.size() > 0 )
	{
		for ( shadmap_iter i = m_mShad.begin(); i != m_mShad.end(); i++ )
		{
			SAFE_RELEASE(i->second);
		}
	}
}

void QMgrResShaders::LogEnabled( bool bLogEnabled )
{
	m_bLogEnabled = bLogEnabled;

	if(m_bLogEnabled)
	{
		g_Log.WriteMessage(L"shaders mgr logging enabled");
	}
	else
	{
		g_Log.WriteMessage(L"shaders mgr logging disabled");
	}
}