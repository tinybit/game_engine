#include "io_utils.h"

bool FileExists( std::string strFileName )
{
	FILE* testfile = NULL;

	if( ( testfile = fopen( strFileName.c_str(), "r") ) == NULL )
	{
		return false;
	}
	else
	{
		fclose(testfile);
		return true;
	}
}

std::string FilenameFromPath( std::string strPath )
{
	for( int i = strPath.length() - 1; i >= 0; i-- )
	{
		if( strPath[i] == '\\' )
			return strPath.substr( i + 1, strPath.length() - i );
	}

	return "";
}

void WR2Strm( ofstream& stream, DWORD dwValue )
{
	DWORD value = dwValue;

	stream.write( (char *)&value, sizeof( DWORD ) );
}

void WR2Strm( ofstream& stream, std::string& strValue )
{
	int str_len = strValue.length();

	//write string length
	stream.write( (const char *)&str_len, sizeof( int ) );

	//write string data
	stream.write( (const char *)strValue.data(), sizeof(char) * str_len );
}

void WR2Strm( ofstream& stream, int iValue )
{
	int value = iValue;

	stream.write( (char *)&value, sizeof( int ) );
}

void WR2Strm( ofstream& stream, float fValue )
{
	stream.write( (char *)&fValue, sizeof( float ) );
}

void WR2Strm( ofstream& stream, D3DXVECTOR3 vecValue )
{
	stream.write( (char *)&vecValue, sizeof( D3DXVECTOR3 ) );
}

void WR2Strm( ofstream& stream, D3DXVECTOR2 vecValue )
{
	stream.write( (char *)&vecValue, sizeof( D3DXVECTOR2 ) );
}

void WR2Strm(	ofstream& stream, bool bValue )
{
	stream.write( (char *)&bValue, sizeof( bool ) );
}

void ReadStrm( ifstream& stream, std::string* strValue )
{
	int		str_len		= 0;
	char*	temp_str	= NULL;
	
	//read string length
	stream.read( (char*)&str_len, sizeof( int ) );

	*strValue = "";

	if( str_len != 0 )
	{
		temp_str = new char[str_len];
		ZeroMemory( temp_str, sizeof(char) * str_len );

		stream.read( (char*)temp_str, str_len );
		strValue->append( temp_str, str_len );
	}

	if( temp_str )
		delete[] temp_str;
}
void ReadStrm( ifstream& stream, DWORD &dwValue )
{
	stream.read( (char *)&dwValue, sizeof( DWORD ) );
}

void ReadStrm( ifstream& stream, int& iValue )
{
	stream.read( (char *)&iValue, sizeof( int ) );
}

void ReadStrm( ifstream& stream, float& fValue )
{
	stream.read( (char *)&fValue, sizeof( float ) );
}

void ReadStrm( ifstream& stream, D3DXVECTOR3& vecValue )
{
	stream.read( (char *)&vecValue, sizeof( D3DXVECTOR3 ) );
}

void ReadStrm( ifstream& stream, D3DXVECTOR2& vecValue )
{
	stream.read( (char *)&vecValue, sizeof( D3DXVECTOR2 ) );
}

void ReadStrm( ifstream& stream, bool &bValue )
{
	stream.read( (char *)&bValue, sizeof( bool ) );
}