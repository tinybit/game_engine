#pragma once

#include <string>
#include <d3d9.h>
#include "utils/enums.h"

using namespace std;

class CBaseScene
{
	public:
		/* constructors */
		CBaseScene() {};
		virtual ~CBaseScene() {};

		/* util functions */
		virtual bool	LoadFromFile( string path )				{ return false; };
		virtual bool	SaveToFile( string path, bool bSilent = false ) { return false; };
		virtual bool	Prepare( LPDIRECT3DDEVICE9 pd3dDevice )	{ return false; };
		virtual void	Render( LPDIRECT3DDEVICE9 pd3dDevice )	{};
		virtual void	Reset( LPDIRECT3DDEVICE9 pd3dDevice )	{};
		virtual void	SetRenderNormals( bool bValue ) {};

		FILE_TYPE		GetType()		{ return FileType; };
		string			GetFolder()		{ return FileFolder; };
		string			GetFilename()	{ return FileName; };
		void			SetPaths( const string &strFolder, const string &strFileName ) { FileFolder = strFolder; FileName = strFileName; };

	private:
		string			FileFolder;
		string			FileName;
		
	protected:
		FILE_TYPE		FileType;
};