#pragma once

#include <string>
#include <ddraw.h>
#include "Fast2PassScale.h"

using namespace std;


// utility flags:
// return data format
#define	TP_DDS_FILE			0x00000002
#define	TP_DATA_BUFFER		0x00000004

// make squared?
#define	TP_NON_SQUARED		0x00000020
#define	TP_SQUARED			0x00000040

// required texture resolution
#define	TP_1024_768			0x00000200
#define	TP_800_600			0x00000400
#define	TP_640_480			0x00000800

// required format
#define	TP_A8R8G8B8			0x00002000
#define	TP_A4R4G4B4			0x00004000
#define	TP_A1R5G5B5			0x00008000
#define	TP_R8G8B8			0x00000008
#define	TP_R5G6B5			0x00000080
#define	TP_ORIGINAL_FORMAT	0x00020000

// alpha 1bit conversion mode
#define	TP_ALPHA_ZERO		0x00200080
#define	TP_ALPHA_MIDDLE		0x00400000

// error messages
#define	TP_OK					 0x0000
#define	TP_ERR_INPUTNOTSUPPORTED 0x0002
#define	TP_ERR_UNKNOWN			 0x0004
#define	TP_ERR_MEMALLOC			 0x0008
#define	TP_ERR_NOFLAG			 0x0020

// pre-calculated code masks
const unsigned int CODEMASK[4]	= { 0x03, 0x0c, 0x30, 0xc0 };
const unsigned int CODESHIFT[4]	= { 0, 2, 4, 6 };

// implementation of the S3TC color interpolation ramp
#define	DECODE( i, j ) ( ( ( 2 * ( i ) ) + ( j ) ) / 3 )

//structs definitions
struct DXTColorBlock 
{
	unsigned short b : 5;
	unsigned short g : 6;
	unsigned short r : 5;
};

struct DXT1Block
{
	union 
	{
		DXTColorBlock	c0;		//!< DXT color 1
		unsigned short	v0;
	};

	union 
	{
		DXTColorBlock	c1;		//!< DXT color 2
		unsigned short	v1;
	};

	unsigned char		code[4];//!< code bits
};

// explicit alpha values
struct DXT3AlphaBlock
{
	unsigned char	nAlpha[8];	//!< uncompressed alpha bits
};

// interpolated alpha values
struct DXT5AlphaBlock
{
	unsigned char	nAlpha0;		//!< Explicit alpha
	unsigned char	nAlpha1;		//!< Explicit alpha
	unsigned char	nBits[6];		//!< Code bits (3 bits per code ->  6x8 = 48 bits = 16 codes)
};

// hi precision pixel
struct ErrorBlock
{
	float a, r, g, b;
};

class TexturePreprocessor
{
	public:
		TexturePreprocessor();
		~TexturePreprocessor();

		//interface
		WORD	ConvertTexture( unsigned char* pFile, DWORD dwFlags, void *(&pDataBuffer), UINT& iBufferSize, UINT& iWidth, UINT& iHeight );


	private:
		string	Unpack4CC( DWORD dwD3DFormat );
		string	ReadDDSHeader( unsigned char* pFileBuffer );
		void	DecompressTexture( string strCompressionFormat, unsigned char* pFile );
		void	SafeDeleteArray( void* pData );
		DWORD	CombineColor( const int r, const int g, const int b, const int a );
		void	ClampColor( int& iColorValue );
		int		DecodeAlpha( const int x, const int y, const DXT1Block &rBlock );
		UINT	GetHigherPow2( UINT a );
		int		DecodeInterpolatedAlpha( const int x, const int y, const DXT1Block &rBlock );
		//original
		WORD	ConvertCompressedTexture( DWORD dwFlags, string strDDSType, unsigned char* pFile, int& iBufferSize );
		//A8R8G8B8
		void	ChangeFormatTo_A8R8G8B8( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg );
		//R8G8B8
		void	ChangeFormatTo_R8G8B8( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg );
		//A4R4G4B4
		void	ChangeFormatTo_A4R4G4B4( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg );
		WORD	ConvertTo_A4R4G4B4( DWORD* color );
		void	GetColorsFrom_A4R4G4B4( WORD* color, int& r, int& g, int& b, int& a );
		//A1R5G5B5
		void	ChangeFormatTo_A1R5G5B5( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg );
		WORD	ConvertTo_A1R5G5B5( DWORD* color );
		void	GetColorsFrom_A1R5G5B5( WORD* color, int& r, int& g, int& b, int& a );
		//R5G6B5
		void	ChangeFormatTo_R5G6B5( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg );
		WORD	ConvertTo_R5G6B5( DWORD* color );
		void	GetColorsFrom_R5G6B5( WORD* color, int& r, int& g, int& b );

		BYTE*			m_pImage;
		int				m_iWidth;
		int				m_iHeight;
		int				m_iSqWidth;
		int				m_iSqHeight;
		float			m_fPrecalcErrors[128][4];
};
