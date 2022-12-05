#include "UtilsCommonByRim.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

std::wstring GetEfnMeshlistVersionAsString( DWORD version )
{
	std::wstring result;

	switch( version )
	{
		case EFN_MESHLIST_VERSION_01:
			result = L"Version 01";
			break;

		case EFN_MESHLIST_VERSION_02:
			result = L"Version 02";
			break;

		default:
			result = L"unknown version";
			break;
	}

	return result;
}

std::wstring GetEfnVersionAsString(DWORD version)
{
	std::wstring result;

	switch( version )
	{
		case EFN_VERSION_01:
			result = L"Version 01";
			break;

		case EFN_VERSION_02:
			result = L"Version 02";
			break;

		case EFN_VERSION_03:
			result = L"Version 03";
			break;

		case EFN_VERSION_04:
			result = L"Version 04";
			break;

		case EFN_VERSION_05:
			result = L"Version 05";
			break;

		case EFN_VERSION_06:
			result = L"Version 06";
			break;

		case EFN_VERSION_07:
			result = L"Version 07";
			break;

		case EFN_VERSION_08:
			result = L"Version 08";
			break;

		case EFN_VERSION_09:
			result = L"Version 09";
			break;

		case EFN_VERSION_10:
			result = L"Version 10";
			break;

		case EFN_VERSION_11:
			result = L"Version 11";
			break;

		case EFN_VERSION_12:
			result = L"Version 12";
			break;


		case EFN_VERSION_13:
			result = L"Version 13";
			break;

		case EFN_VERSION_14:
			result = L"Version 14";
			break;

		case EFN_VERSION_15:
			result = L"Version 15";
			break;

		case EFN_VERSION_16:
			result = L"Version 16";
			break;

		case EFN_VERSION_17:
			result = L"Version 17";
			break;
		
		case EFN_VERSION_18:
			result = L"Version 18";
			break;

		default:
			 result = L"unknown version";
			 break;
	}

	return result;
}

DWORD GetEfnVersion( std::wstring file_path )
{
	streampos		pos;
	DWORD		version;

	ifstream file( wstr2str(file_path).c_str(), ios::binary );
	file.seekg(pos);

	file.read((char *)&version, sizeof(DWORD));

	file.close();

	return version;
}

void ReadStrm( ifstream& stream, std::wstring& strValue )
{
	std::string str_value;
	ReadStrm(stream, str_value);
	strValue = str2wstr(str_value);
}

void ReadStrm( ifstream& stream, std::string& strValue )
{
	int			str_len		= 0;
	char*		temp_str	= NULL;
	
	//read string length
	stream.read( (char *)&str_len, sizeof( int ) );

	//read string data
	if( !(temp_str = new char[str_len]) )
		return ;
	
	stream.read((char *)temp_str, sizeof(char) * str_len);

	strValue.append( temp_str, str_len );

	SAFE_DELETE_ARRAY( temp_str );
}

void	ReadStrm(	 ifstream& stream, int& iValue )
{
	stream.read( (char *)&iValue, sizeof( int ) );
}

void	ReadStrm(	 ifstream& stream, DWORD& iValue )
{
	stream.read( (char *)&iValue, sizeof( DWORD ) );
}

void	ReadStrm(	 ifstream& stream, float& fValue )
{
	stream.read( (char *)&fValue, sizeof( float ) );
}

void ReadStrm( ifstream& stream, D3DXVECTOR3 &vecValue )
{
	stream.read( (char *)&vecValue.x, sizeof( float ) );
	stream.read( (char *)&vecValue.y, sizeof( float ) );
	stream.read( (char *)&vecValue.z, sizeof( float ) );
}

void ReadStrm( ifstream& stream, D3DXVECTOR2 &vecValue )
{
	stream.read( (char *)&vecValue.x, sizeof( float ) );
	stream.read( (char *)&vecValue.y, sizeof( float ) );
}

void ReadStrm( ifstream& stream, bool& bValue )
{
	stream.read( (char *)&bValue, sizeof( bool ) );
}