#pragma warning(disable: 4172)

#include "Common.h"

// string utils
char* _2Upper( char* string )
{
	locale loc1 ( "Russian_Russia" );

	use_facet<ctype<char> > ( loc1 ).toupper( &string[0], &string[strlen(&string[0])] );
	return string;
}

SCAMERA_LOAD::SCAMERA_LOAD( CCAMERA2& cam )
{
	vPerPos			= cam.pos;
	vPerTarg		= cam.targ;
	vPerUpVec		= cam.upvec;
	vPerFov			= cam.fov;
	fPerNearPlane	= cam.z_near;
	fPerFarPlane	= cam.z_far;

	fZoomNear		= cam.zoom_near;
	fZoomFar		= cam.zoom_far;

	fFogStart		= cam.fFogStart;	
	fFogEnd			= cam.fFogEnd;		
	fFogDensity		= cam.fFogDensity;	
	iFogColor		= cam.iFogColor;	
	iFogType		= cam.iFogType;	

	bApplyEffectsOnShadow = cam.bApplyEffectsOnShadow;
}

str_t _2Upper( str_t& string )
{
	str_t strLoc = _2Upper( (char*)string.data() );

	return strLoc;
}

char* _2Lower( char* string )
{
	locale loc1 ( "Russian_Russia" );

	use_facet<ctype<char> > ( loc1 ).tolower( &string[0], &string[strlen(&string[0])] );
	return string;
}

str_t _2Lower( str_t& string )
{
	str_t strLoc = _2Lower( (char*)string.data() );

	return strLoc;
}

void GetARGBFromDWORD( int& a, int& r, int& g, int& b, DWORD dwColor )
{
	a = (dwColor >> 24) & 0xFF;
	r = (dwColor >> 16) & 0xFF;
	g = (dwColor >> 8) & 0xFF;
	b = dwColor & 0xFF;
}

std::string TrimStr( std::string strValue )
{
	int beg = 0;
	int end = 0;
	int counter = 0;
	
	//trim leading spaces
	while( (strValue[counter] == ' ' || strValue[counter] == '\t' || strValue[counter] == '\n' || strValue[counter] == '\r\n' ) && 
			counter != strValue.length() )
	{
		counter++;
	}

	beg = counter;

	counter = strValue.length() - 1;

	//trim back spaces
	while( (strValue[counter] == ' ' || strValue[counter] == '\t' || strValue[counter] == '\n' || strValue[counter] == '\r\n' ) && 
			counter >= 0 )
	{
		counter--;
		end++;
	}

	int u = strValue.length();

	if( beg + strValue.length() - ( beg + end ) > strValue.length() )
		return "";

	if( u - ( beg + end ) < 0 )
		return "";

	CString temp = strValue.substr( beg, strValue.length() - ( beg + end ) ).data();

	return temp.GetBuffer();
}

//test
enum EFN_FILETYPE GetEfnFiletype( str_t path )
{
	streampos						pos;
	enum EFN_FILETYPE	eft;

	ifstream file( path.c_str(), ios::binary );
	file.seekg(pos);

	DWORD version;
	file.read((char *)&version, sizeof(DWORD));

	int dd;
	file.read((char *)&dd, sizeof(int));

	file.close();

	eft = (EFN_FILETYPE)dd;

	return eft;
}

float FovX2FovY(float fFovX, float fAspect)
{
	float xScale = 1 / tanf(fFovX / 2);
	float yScale = xScale * fAspect;
	return 2 * atanf(1/yScale);
};

float FovY2FovX(float fFovY, float fAspect)
{
	float yScale = 1 / tanf(fFovY / 2);
	float xScale = yScale / fAspect;
	return 2 * atanf(1/xScale);
};

void CorrectXSIFile( str_t path )
{
	FILE*		stream		= NULL;
	FILE*		temp_stream = NULL;
	char		buff[1024];
	CString		tmp_str;

	stream		= fopen( path.data(), "a+" );
	temp_stream = fopen( "c:\\cookie_machine_temp_file.dat", "w" );

	if( !stream || !temp_stream )
		return;

	// move to the beginning of the files
	fseek( stream, 0L, SEEK_SET );
	fseek( temp_stream, 0L, SEEK_SET );

	// read original file, create correct version at c:\\temp.dat
	while( !feof( stream ) )
	{
 		fgets( buff, 1024, stream );

		tmp_str = buff;
		tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );

		if( tmp_str == "SI_FileInfo{\0" )
		{
			sprintf( buff, "%s\n", "SI_FileInfo {" );
		}

		fprintf( temp_stream, "%s", buff );
	}

	fclose( stream );
	fclose( temp_stream );



	//copy temp file's contents into original file
	stream			= fopen( path.data(), "w" );
	temp_stream = fopen( "c:\\cookie_machine_temp_file.dat", "r" );

	// move to the beginning of the files
	fseek( stream, 0L, SEEK_SET );
	fseek( temp_stream, 0L, SEEK_SET );

	while( !feof( temp_stream ) )
	{
 		fgets( buff, 1024, temp_stream );
		fprintf( stream, "%s", buff );
	}

	fclose( stream );
	fclose( temp_stream );

	DeleteFile( "c:\\cookie_machine_temp_file.dat" );
}

DWORD GetScnVersion( str_t file_path )
{
	streampos		pos;
	DWORD		version;

	ifstream file( file_path.c_str(), ios::binary );
	file.seekg(pos);

	file.read( (char *)&version, sizeof(DWORD) );

	file.close();

	return version;
}

str_t GetScnVersionAsString(DWORD version)
{
	str_t result;

	switch( version )
	{
		case SCN_VERSION_01:
			result = "Version 01";
			break;

		case SCN_VERSION_02:
			result = "Version 02";
			break;

		case SCN_VERSION_03:
			result = "Version 03";
			break;

		case SCN_VERSION_04:
			result = "Version 04";
			break;
		
		case SCN_VERSION_05:
			result = "Version 05";
			break;

		default:
			 result = "unknown version";
			 break;
	}

	return result;
}

void GetTypesList( StrVec &listTypes )
{
	listTypes.push_back( "Man" );
	listTypes.push_back( "Car" );
	listTypes.push_back( "Bird" );
	listTypes.push_back( "Free" );
	listTypes.push_back( "Camera" );
	listTypes.push_back( "Light" );
	listTypes.push_back( "ShadowVolume" );
	listTypes.push_back( "ShadowCaster" );
	listTypes.push_back( "ShadowReceiver" );
	listTypes.push_back( "LayerGeom" );
	listTypes.push_back( "Action" );
	listTypes.push_back( "EfnAnim" );
	listTypes.push_back( "VideoAnim" );
	listTypes.push_back( "Video3D" );
	listTypes.push_back( "MenuRukzak" );
	listTypes.push_back( "MenuCursor" );
	listTypes.push_back( "MenuMenu" );
	listTypes.push_back( "Background" );
	listTypes.push_back( "Scene" );
	listTypes.push_back( "Floor" );
	listTypes.push_back( "CameraSelect" );
	listTypes.push_back( "OcclusionGeom" );
	listTypes.push_back( "SndOcclusionGeom" );
	listTypes.push_back( "DynamicFloor" );
	listTypes.push_back( "ParticlesMgr" );
}

str_t GetPrexifFromType( str_t strType )
{
	if( strType == "Man" )
		return "mn";

	if( strType == "Camera" )
		return "cm";

	if( strType == "Light" )
		return "lt";

	if( strType == "ShadowVolume" )
		return "sv";

	if( strType == "LightVolume" )
		return "lv";

	if( strType == "LayerGeom" )
		return "lg";

	if( strType == "Action" )
		return "oa";

	if( strType == "Background" )
		return "bg";

	if( strType == "Scene" )
		return "sc";

	if( strType == "Floor" )
		return "fl";

	if( strType == "Glass" )
		return "gl";

	if( strType == "Reflection" )
		return "rf";

	if( strType == "CameraSelect" )
		return "cs";

	return "";
}

str_t StripTypeHeader( str_t &strValue )
{
	str_t strLoc = strValue;

	int len = strLoc.length();
	int index = strLoc.find( ".", 0 );
	str_t strType = strLoc.substr( 0,  index );
	strLoc = strLoc.substr( index + 1, len );
	
	strValue = strLoc;

	return strType;
}

str_t GetObjTypeStr( enum ObjTypes obj_t )
{
	if( obj_t == OT_MAN )
		return "Man";

	if( obj_t == OT_CAMERA )
		return "Camera";

	if( obj_t == OT_LIGHT )
		return "Light";

	if( obj_t == OT_SHADOWVOLUME )
		return "ShadowVolume";

	if( obj_t == OT_LIGHTVOLUME )
		return "LightVolume";

	if( obj_t == OT_LAYERGEOM )
		return "LayerGeom";

	if( obj_t == OT_ACTION )
		return "Action";

	if( obj_t == OT_BACK )
		return "Background";

	if( obj_t == OT_SCENE )
		return "Scene";

	if( obj_t == OT_FLOOR )
		return "Floor";

	if( obj_t == OT_GLASS )
		return "Glass";

	if( obj_t == OT_REFLECTION )
		return "Reflection";
	
	if( obj_t == OT_CAMSELECT )
		return "CameraSelect";

	return "";
}

CString CSTRim( CString str )
{
	if( str.GetBuffer()[0] = '"' )
		str = str.Left( str.GetLength() - 1 );

	if( str.GetBuffer()[str.GetLength()] = '"' )
		str = str.Right( str.GetLength() - 1 );

	if( str.GetBuffer()[0] = '"' )
		str = str.Left( str.GetLength() - 1 );

	if( str.GetBuffer()[str.GetLength()] = '"' )
		str = str.Right( str.GetLength() - 1 );

	str.Trim();

	return str;
}

void RemoveDupesFromVector( std::vector <int>* vec )
{
	std::vector <int> temp;
	bool found = false;

	temp.clear();

	for( unsigned int i = 0; i < vec->size(); i++ )
	{
		for( unsigned int j = 0; j < temp.size(); j++ )
		{
			if( temp.at(j) == vec->at(i) )
			{
				found = true;
				break;
			}
		}

		if( !found )
		{
			temp.push_back( vec->at(i) );
		}

		found = false;
	}

	vec->clear();

	for( unsigned int i = 0; i < temp.size(); i++ )
		vec->push_back( temp.at( i ) );
}

DWORD GetEfnMeshlistVersion( str_t file_path )
{
	streampos	pos;
	DWORD		version;

	ifstream file( file_path.c_str(), ios::binary );
	file.seekg(pos);

	file.read((char *)&version, sizeof(DWORD));

	file.close();

	return version;
}

str_t GetEfnMeshlistVersionAsString( DWORD version )
{
	str_t result;

	switch( version )
	{
		case EFN_MESHLIST_VERSION_01:
			result = "Version 01";
			break;

		case EFN_MESHLIST_VERSION_02:
			result = "Version 02";
			break;

		default:
			result = "unknown version";
			break;
	}

	return result;
}

DWORD GetEfnVersion( str_t file_path )
{
	streampos	pos;
	DWORD		version;

	ifstream file( file_path.c_str(), ios::binary );
	file.seekg(pos);

	file.read((char *)&version, sizeof(DWORD));

	file.close();

	return version;
}

str_t GetEfnVersionAsString(DWORD version)
{
	str_t result;

	switch( version )
	{
		case EFN_VERSION_01:
			result = "Version 01";
			break;

		case EFN_VERSION_02:
			result = "Version 02";
			break;

		case EFN_VERSION_03:
			result = "Version 03";
			break;

		case EFN_VERSION_04:
			result = "Version 04";
			break;

		case EFN_VERSION_05:
			result = "Version 05";
			break;

		case EFN_VERSION_06:
			result = "Version 06";
			break;

		case EFN_VERSION_07:
			result = "Version 07";
			break;

		case EFN_VERSION_08:
			result = "Version 08";
			break;

		case EFN_VERSION_09:
			result = "Version 09";
			break;

		case EFN_VERSION_10:
			result = "Version 10";
			break;

		case EFN_VERSION_11:
			result = "Version 11";
			break;

		case EFN_VERSION_12:
			result = "Version 12";
			break;

		case EFN_VERSION_13:
			result = "Version 13";
			break;

		case EFN_VERSION_14:
			result = "Version 14";
			break;

		case EFN_VERSION_15:
			result = "Version 15";
			break;

		case EFN_VERSION_16:
			result = "Version 16";
			break;

		case EFN_VERSION_17:
			result = "Version 17";
			break;
		
		case EFN_VERSION_18:
			result = "Version 18";
			break;

		case EFN_VERSION_19:
			result = "Version 19";
			break;

		case EFN_VERSION_20:
			result = "Version 20";
			break;

		case EFN_VERSION_21:
			result = "Version 21";
			break;

		case EFN_VERSION_22:
			result = "Version 22";
			break;

		case EFN_VERSION_23:
			result = "Version 23";
			break;

		default:
			result = "unknown version";
			break;
	}

	return result;
}

// common utils
void EnableMenuItem(CMenu* menu, int ID, bool enable)
{
	CMenu*		submenu	= NULL;
    int				ret				= menu->GetMenuItemCount();
	bool			found			= false;
	CString		str;

	//find menu item by id
	//go through submenus
	for( unsigned int i = 0; i < menu->GetMenuItemCount(); i++ )
	{
		submenu = menu->GetSubMenu(i);

		submenu->GetMenuString( ID, str, MF_BYCOMMAND );
		
		if( str.GetLength() > 0  )
		{
			found = true;
			break;
		}
	}

	if( found )
	{
		if( enable )
			submenu->ModifyMenu( ID, MF_BYCOMMAND | MF_ENABLED, ID, str);
		else
			submenu->ModifyMenu( ID, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED, ID, str);
	}
}

CVector* ToCVec( const D3DXVECTOR3 vec )
{
	return &CVector(vec.x, vec.y, vec.z);
}

CVector* ToCVec( const D3DXVECTOR4 vec )
{
  return &CVector(vec.x, vec.y, vec.z);
}

D3DXVECTOR3& ToDX3Vec( const CVector vec )
{
	return D3DXVECTOR3(vec.x, vec.y, vec.z);
}

void DeleteFuncEntriesByKey( IFuncMap* pFuncs, int iKey )
{
	pair<IFuncMap_iter, IFuncMap_iter> func_entries;
	func_entries = pFuncs->equal_range( iKey );

	while( func_entries.first != func_entries.second )
	{
		SAFE_DELETE( func_entries.first->second );
		func_entries.first++;
	}
}