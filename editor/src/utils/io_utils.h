#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <d3dx9math.h>

using namespace std;

bool		FileExists( std::string strFileName );
std::string	FilenameFromPath( std::string strPath );

void		WR2Strm( ofstream& stream, DWORD dwValue );
void		WR2Strm( ofstream& stream, std::string& strValue );
void		WR2Strm( ofstream& stream, int iValue );
void		WR2Strm( ofstream& stream, float fValue );
void		WR2Strm( ofstream& stream, D3DXVECTOR3 vecValue );
void		WR2Strm( ofstream& stream, D3DXVECTOR2 vecValue );
void		WR2Strm( ofstream& stream, bool bValue );

void		ReadStrm( ifstream& stream, DWORD& dwValue );
void		ReadStrm( ifstream& stream, std::string* strValue );
void		ReadStrm( ifstream& stream, int& iValue );
void		ReadStrm( ifstream& stream, float& fValue );
void		ReadStrm( ifstream& stream, D3DXVECTOR3& vecValue );
void		ReadStrm( ifstream& stream, D3DXVECTOR2& vecValue );
void		ReadStrm( ifstream& stream, bool& bValue );