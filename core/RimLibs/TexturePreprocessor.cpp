
#include ".\texturepreprocessor.h"

#pragma warning( disable :4244 )

TexturePreprocessor::TexturePreprocessor()
{
	m_pImage	= NULL;
	m_iWidth	= 0;
	m_iHeight	= 0;
	m_iSqWidth	= 0;
	m_iSqHeight	= 0;

	for( int i = 0; i < 128; i++ )
	{
		if( i < 64 )
		{
			m_fPrecalcErrors[i][0] = (float)i / 16;
			m_fPrecalcErrors[i][1] = (float)i / 16 * 3;
			m_fPrecalcErrors[i][2] = (float)i / 16 * 5;
			m_fPrecalcErrors[i][3] = (float)i / 16 * 7;
		}
		else
		{
			m_fPrecalcErrors[i][0] = (float)(-i + 63) / 16;
			m_fPrecalcErrors[i][1] = (float)(-i + 63) / 16 * 3;
			m_fPrecalcErrors[i][2] = (float)(-i + 63) / 16 * 5;
			m_fPrecalcErrors[i][3] = (float)(-i + 63) / 16 * 7;		
		}
	}
}

TexturePreprocessor::~TexturePreprocessor()
{
}

void inline TexturePreprocessor::SafeDeleteArray( void* pData )
{
	if( pData )
		delete []pData;
}

inline DWORD TexturePreprocessor::CombineColor( const int r, const int g, const int b, const int a )
{
	return (a << 24) | (r << 16 ) | ( g << 8 ) | b;
}

string TexturePreprocessor::Unpack4CC( DWORD dwD3DFormat )
{
	string res;

	res += (char)dwD3DFormat;
	res += (char)(dwD3DFormat >> 8);
	res += (char)(dwD3DFormat >> 16);
	res += (char)(dwD3DFormat >> 24);

	if( res == "DXT1" || res == "DXT2" || res == "DXT3" || res == "DXT4" || res == "DXT5" )
		return res;

	return "";
}

void inline TexturePreprocessor::ClampColor( int& iColorValue )
{
	iColorValue = iColorValue > 255 ? 255 : iColorValue;
	iColorValue = iColorValue < 0 ? 0 : iColorValue;
}

WORD inline TexturePreprocessor::ConvertTo_A4R4G4B4( DWORD* color )
{
	return	( (( (*color >> 24) & 0xff ) >> 4 ) << 12 ) |
			( (( (*color >> 16) & 0xff ) >> 4 ) << 8 ) |
			( (( (*color >> 8)  & 0xff ) >> 4 ) << 4) |
			(( *color & 0xff ) >> 4 );
}

void inline TexturePreprocessor::GetColorsFrom_A4R4G4B4( WORD* color, int& r, int& g, int& b, int& a )
{
	a = *color >> 8 & 0xff;
	r = *color >> 4 & 0xff;
	g = *color & 0xff;
	b = *color << 4 & 0xff;
}

WORD inline TexturePreprocessor::ConvertTo_A1R5G5B5( DWORD* color )
{
	return (( (*color >> 24) & 0xff ) >> 1 ) << 15 |
			( (( (*color >> 16) & 0xff ) >> 3 ) << 10 ) |
			( (( (*color >> 8)  & 0xff ) >> 3 ) << 5 ) |
			(( *color & 0xff ) >> 3 );
}

void inline TexturePreprocessor::GetColorsFrom_A1R5G5B5( WORD* color, int& r, int& g, int& b, int& a )
{
	a = *color >> 4 & 0xff;
	r = *color >> 7 & 0xff;
	g = *color >> 2 & 0xff;
	b = *color << 3 & 0xff;
}

WORD inline TexturePreprocessor::ConvertTo_R5G6B5( DWORD* color )
{
	return	( (( (*color >> 16) & 0xff ) >> 3 ) << 11 ) |
			( (( (*color >> 8)  & 0xff ) >> 3 ) << 6) |
			(( *color & 0xff ) >> 3 );
}

void inline TexturePreprocessor::GetColorsFrom_R5G6B5( WORD* color, int& r, int& g, int& b )
{
	r = *color >> 8 & 0xff;
	g = *color >> 3 & 0xff;
	b = *color << 3 & 0xff;
}

string TexturePreprocessor::ReadDDSHeader( unsigned char* pFileBuffer )
{
	DDSURFACEDESC2 surfDesc;

	if( !pFileBuffer )
		return "";

	unsigned char* pStream = pFileBuffer;

	//skip first dword, it's dds file ID -- "DDS|"
	pStream += 4;

	//read dds header
	memcpy( &surfDesc, pStream, sizeof(DDSURFACEDESC2) );

	m_iWidth	= (int)surfDesc.dwWidth;
	m_iHeight	= (int)surfDesc.dwHeight;

	return Unpack4CC( surfDesc.ddpfPixelFormat.dwFourCC );
}

UINT TexturePreprocessor::GetHigherPow2( UINT a )
{
	UINT i = 0;

	for( i = 1; i < a; i = i * 2 )
	{}

	return i;
}

WORD TexturePreprocessor::ConvertCompressedTexture( DWORD dwFlags, string strDDSType, unsigned char* pFile, int& iBufferSize )
{
	BYTE*			pTempImage;
	int				size		= 0;
	unsigned char*	pFilePtr	= pFile;
	int				div_size	= 2;

	if( strDDSType == "DXT3" || strDDSType == "DXT5" )
		div_size = 1;

	//make sure old buffer is empty
	SafeDeleteArray( m_pImage );


	//check all flags for nesessary combination
	if( dwFlags & TP_1024_768 )
	{	
	}
	else if( dwFlags & TP_800_600 )
	{
		return TP_ERR_INPUTNOTSUPPORTED;	
	}
	else if( dwFlags & TP_640_480 )
	{
		return TP_ERR_INPUTNOTSUPPORTED;
	}
	else
	{
		return TP_ERR_NOFLAG;
	}


	if( dwFlags & TP_DDS_FILE )
	{
		size += 124 + 4;
	}
	else if( dwFlags & TP_DATA_BUFFER )
	{
	}
	else
	{
		return TP_ERR_NOFLAG;
	}
		
	if( dwFlags & TP_SQUARED )
	{
		m_iSqWidth	= GetHigherPow2( m_iWidth );
		m_iSqHeight = GetHigherPow2( m_iHeight );

		size += ( m_iSqWidth * m_iSqHeight ) / div_size;
	}
	else if( dwFlags & TP_NON_SQUARED )
	{
		size += ( m_iWidth * m_iHeight ) / div_size;
	}
	else
	{
		return TP_ERR_NOFLAG;
	}


	//create new buffer
	m_pImage = new BYTE[size];

	if( !m_pImage )
		return TP_ERR_MEMALLOC;

	iBufferSize = size;
	ZeroMemory( m_pImage, size );
	pTempImage = m_pImage;

	

	//square if needed
	if( dwFlags & TP_SQUARED )
	{
		DDSURFACEDESC2 surfDesc;

		memcpy( pTempImage, pFilePtr, 4 );	
		pTempImage	+= 4;
		pFilePtr	+= 4;

		//copy header if needed
		if( dwFlags & TP_DDS_FILE )
		{
			memcpy( &surfDesc, pFilePtr, 124 );	

			surfDesc.dwWidth	= m_iSqWidth;
			surfDesc.dwHeight	= m_iSqHeight;

			memcpy( pTempImage, &surfDesc, 124 );	
			pTempImage	+= 124;
			pFilePtr	+= 124;
		}

		//copy data
		if( strDDSType == "DXT1" )
		{
			for( int i = 0; i < m_iHeight / 4; i++ )
			{
				memcpy( pTempImage, pFilePtr, m_iWidth * 2 );
 				pFilePtr	+= m_iWidth * 2;
				pTempImage	+= m_iSqWidth * 2;
			}
		}
		else
		{
			for( int i = 0; i < m_iHeight / 4; i++ )
			{
				memcpy( pTempImage, pFilePtr, m_iWidth * 4 );
 				pFilePtr	+= m_iWidth * 4;
				pTempImage	+= m_iSqWidth * 4;
			}		
		}

		m_iWidth	= m_iSqWidth;
		m_iHeight	= m_iSqHeight;
	}
	else
	{
		//copy header if needed
		if( dwFlags & TP_DDS_FILE )
		{
			memcpy( pTempImage, pFilePtr, 124 + 4 );	
			pTempImage	+= 124 + 4;
			pFilePtr	+= 124 + 4;
		}

		memcpy( pTempImage, pFilePtr, size - (124 + 4) );
	}

	return TP_OK;
}

WORD TexturePreprocessor::ConvertTexture( unsigned char* pFile, DWORD dwFlags, void *(&pDataBuffer), UINT& iBufferSize, UINT& iWidth, UINT& iHeight )
{
	// read dds header
	string	dds_type	= ReadDDSHeader( pFile );
	int		iSqWidth	= m_iWidth;
	int		iSqHeight	= m_iHeight;
	DWORD*	pImage		= NULL;


	// make sure we use dxt1, 3 or 5, then decompress texture
	if( dds_type == "DXT1" || dds_type == "DXT3" || dds_type == "DXT5" )
	{
		m_pImage = new BYTE[4 * m_iWidth * m_iHeight];

		if( !m_pImage )
			return TP_ERR_MEMALLOC;

		if( dwFlags & TP_ORIGINAL_FORMAT )
		{			
			int bsize;
			WORD res_err = ConvertCompressedTexture( dwFlags, dds_type, pFile, bsize );

			if( res_err == 0 )
			{
				pDataBuffer	= m_pImage;
				iBufferSize = bsize;
				iWidth		= m_iWidth;
				iHeight		= m_iHeight;

				return TP_OK;
			}
			else
			{
				return res_err;
			}
		}
		else
		{
			DecompressTexture( dds_type, pFile );
		}
	}
	else
	{
		return TP_ERR_INPUTNOTSUPPORTED;
	}


	// resize if needed
	int		iNewHeight;
	int		iNewWidth;
	bool	resizeRequired = true;
	pImage = (DWORD*)m_pImage;


	// check resizing flags
	if( dwFlags & TP_1024_768 )
	{
		//skip
		resizeRequired = false;	
	} 
	else if( dwFlags & TP_800_600 )
	{
		// coefficient: 0.78125
		iNewHeight	= m_iHeight * 0.78125f;
		iNewWidth	= m_iWidth * 0.78125f;
	}
	else if( dwFlags & TP_640_480 )
	{
		// coefficient: 0.625
		iNewHeight	= m_iHeight * 0.625f;
		iNewWidth	= m_iWidth * 0.625f;
	}
	else
	{
		return TP_ERR_NOFLAG;
	}


	//check for alpha conversion flags
	if( dwFlags & TP_ALPHA_ZERO )
	{
	}
	else if( dwFlags & TP_ALPHA_MIDDLE )
	{
	}
	else
	{
		return TP_ERR_NOFLAG;
	}

		
	if( resizeRequired )
	{
		CFast2PassScale scaler;
		DWORD*	pImageTemp = new DWORD[iNewHeight * iNewWidth]; //allocate memory for new image 
		DWORD*	pAlphaTemp = new DWORD[iNewHeight * iNewWidth]; //allocate memory for new alpha

		if( !pImageTemp )
			return TP_ERR_MEMALLOC;

		if( !pAlphaTemp )
			return TP_ERR_MEMALLOC;

		ZeroMemory( pImageTemp, sizeof(DWORD) * iNewHeight * iNewWidth );
		ZeroMemory( pAlphaTemp, sizeof(DWORD) * iNewHeight * iNewWidth );

		//resize image
		scaler.Scale( pImage, m_iWidth, m_iHeight, pImageTemp, iNewWidth, iNewHeight );

		//create temp alpha array
		for( int i = 0; i < m_iWidth * m_iHeight; i++ )
			pImage[i] = ( (pImage[i] >> 24) & 0xff ) << 16 | ( (pImage[i] >> 24) & 0xff ) << 8 | ( ( pImage[i] >> 24) & 0xff );

		//resize alpha
		scaler.Scale( pImage, m_iWidth, m_iHeight, pAlphaTemp, iNewWidth, iNewHeight );

		for( int i = 0; i < iNewHeight * iNewWidth; i++ )
			pImageTemp[i] = ( (pAlphaTemp[i] & 0xff) << 24 ) | ( ((pImageTemp[i] >> 16) & 0xff) << 16 ) | ( ((pImageTemp[i] >> 8) & 0xff) << 8 ) | (pImageTemp[i] & 0xff);

		//delete old image
		SafeDeleteArray( m_pImage );
		SafeDeleteArray( pAlphaTemp );
		
		//reassign arrays for source and alpha
		m_iWidth	= iNewWidth;
		m_iHeight	= iNewHeight;
		m_pImage	= (BYTE*)pImageTemp;
	}


	// check squaring flags
	if( dwFlags & TP_SQUARED )
	{
		//calc closest square dimentions
		iSqWidth	= GetHigherPow2( m_iWidth );
		iSqHeight	= GetHigherPow2( m_iHeight );
	} 
	else if( dwFlags & TP_NON_SQUARED )
	{
		//skip
	}
	else
	{
		return TP_ERR_NOFLAG;
	}

	m_iSqWidth	= iSqWidth;
	m_iSqHeight = iSqHeight;

	// check FORMAT flags and change image format as required
	WORD wErrMessage = 0;
	int iBuffSize = 0;

	if( dwFlags & TP_A8R8G8B8 )
	{
		ChangeFormatTo_A8R8G8B8( dwFlags, iBuffSize, wErrMessage );
	} 
	else if( dwFlags & TP_A4R4G4B4 )
	{
		ChangeFormatTo_A4R4G4B4( dwFlags, iBuffSize, wErrMessage );
	}
	else if( dwFlags & TP_A1R5G5B5 )
	{
		ChangeFormatTo_A1R5G5B5( dwFlags, iBuffSize, wErrMessage );
	}
	else if( dwFlags & TP_R5G6B5 )
	{
		ChangeFormatTo_R5G6B5( dwFlags, iBuffSize, wErrMessage );
	}
	//else if( dwFlags & TP_R8G8B8 )
	//{
	//	ChangeFormatTo_R8G8B8( dwFlags, iBuffSize, wErrMessage );
	//}
	else
	{
		return TP_ERR_NOFLAG;
	}

	pDataBuffer	= m_pImage;
	iBufferSize	= iBuffSize;
	iWidth		= m_iWidth;
	iHeight		= m_iHeight;

	if( wErrMessage != 0 )
		return wErrMessage;

	return TP_OK;
}

void TexturePreprocessor::ChangeFormatTo_A8R8G8B8( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg )
{
	DWORD			dwMagic = 542327876;
	DDSURFACEDESC2	surf_desc;
	BYTE*			sqImage		= NULL;
	BYTE*			sqImagePtr	= NULL;
	BYTE*			pImagePtr	= m_pImage;

	//check flags
	if( !(dwFlags & TP_DDS_FILE) && !(dwFlags & TP_DATA_BUFFER) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	if( !(dwFlags & TP_SQUARED) && !(dwFlags & TP_NON_SQUARED) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	
	//surface
	ZeroMemory( &surf_desc, sizeof(DDSURFACEDESC2) );

	surf_desc.dwSize = 124;
	surf_desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
	
	//caps
	surf_desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	//pixel format
	surf_desc.ddpfPixelFormat.dwSize			= 32;
	surf_desc.ddpfPixelFormat.dwFlags			= DDPF_RGB | DDPF_ALPHAPIXELS;
	surf_desc.ddpfPixelFormat.dwRGBBitCount		= 32;
	surf_desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
	surf_desc.ddpfPixelFormat.dwRBitMask		= 0x00ff0000;
	surf_desc.ddpfPixelFormat.dwGBitMask		= 0x0000ff00;
	surf_desc.ddpfPixelFormat.dwBBitMask		= 0x000000ff;


	// sq requested
	if( dwFlags & TP_SQUARED )
	{
		surf_desc.dwHeight	= m_iSqHeight;
		surf_desc.dwWidth	= m_iSqWidth;

		iBufferSize = 4 * m_iSqWidth * m_iSqHeight;

		if( dwFlags & TP_DDS_FILE )
			iBufferSize += 124 + 4;

		//make copy of an array
		sqImage		= new BYTE[ iBufferSize ];
		sqImagePtr	= sqImage;

		if( !sqImage )
		{
			wErrMsg = TP_ERR_MEMALLOC;
			return;
		}

		ZeroMemory( sqImage, iBufferSize );

		if( dwFlags & TP_DDS_FILE )
		{
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;
		}

		//squarify image
		for( int i = 0; i < m_iHeight; i++ )
		{
			memcpy( sqImagePtr, pImagePtr, sizeof(m_pImage) * m_iWidth );
			pImagePtr	+= 4 * m_iWidth;
			sqImagePtr	+= 4 * m_iSqWidth;
		}

		m_iHeight			= m_iSqHeight;
		m_iWidth			= m_iSqWidth;
	
		SafeDeleteArray( m_pImage );
		m_pImage = sqImage;
	}
	else
	{
		surf_desc.dwHeight	= m_iHeight;
		surf_desc.dwWidth	= m_iWidth;

		iBufferSize = 4 * m_iHeight * m_iWidth;

		if( dwFlags & TP_DDS_FILE )
		{
			iBufferSize += 124 + 4;

			//make copy of an array
			sqImage		= new BYTE[ iBufferSize ];
			sqImagePtr	= sqImage;

			if( !sqImage )
			{
				wErrMsg = TP_ERR_MEMALLOC;
				return;
			}
			
			ZeroMemory( sqImage, iBufferSize );
			
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;

			memcpy( sqImagePtr, m_pImage, sizeof(m_pImage) * m_iWidth * m_iHeight );

			SafeDeleteArray( m_pImage );
			m_pImage = sqImage;
		}
	}
}

void TexturePreprocessor::ChangeFormatTo_R8G8B8( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg )
{
	DWORD			dwMagic = 542327876;
	DDSURFACEDESC2	surf_desc;
	BYTE*			sqImage		= NULL;
	BYTE*			sqImagePtr	= NULL;
	BYTE*			pImagePtr	= NULL;

	//check flags
	if( !(dwFlags & TP_DDS_FILE) && !(dwFlags & TP_DATA_BUFFER) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	if( !(dwFlags & TP_SQUARED) && !(dwFlags & TP_NON_SQUARED) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	
	//surface
	ZeroMemory( &surf_desc, sizeof(DDSURFACEDESC2) );

	surf_desc.dwSize = 124;
	surf_desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
	
	//caps
	surf_desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	//pixel format
	surf_desc.ddpfPixelFormat.dwSize			= 32;
	surf_desc.ddpfPixelFormat.dwFlags			= DDPF_RGB;
	surf_desc.ddpfPixelFormat.dwRGBBitCount		= 24; 
	surf_desc.ddpfPixelFormat.dwRBitMask		= 0xff0000;
	surf_desc.ddpfPixelFormat.dwGBitMask		= 0x00ff00;
	surf_desc.ddpfPixelFormat.dwBBitMask		= 0x0000ff;



	//convert m_pImage 2 R8G8B8
	BYTE* pImage		= new BYTE[ 3 * (m_iWidth * m_iHeight) ];
	BYTE* pTemp			= m_pImage;
	BYTE* pImageTemp	= pImage;

	ZeroMemory( pImage, 3 * (m_iWidth * m_iHeight) );

	for( int i = 0; i < m_iWidth * m_iHeight; i++ )
	{
		memcpy( pImageTemp, pTemp, 3 );
		pImageTemp += 3;
		pTemp += 4;
	}

	//replace current image
	SafeDeleteArray( m_pImage );
	m_pImage = pImage;

	pImagePtr = m_pImage;


	// sq requested
	if( dwFlags & TP_SQUARED )
	{
		surf_desc.dwHeight	= m_iSqHeight;
		surf_desc.dwWidth	= m_iSqWidth;


		iBufferSize = 3 * m_iSqWidth * m_iSqHeight;

		if( dwFlags & TP_DDS_FILE )
			iBufferSize += 124 + 4;

		//make copy of an array
		sqImage		= new BYTE[ iBufferSize ];
		sqImagePtr	= sqImage;

		if( !sqImage )
		{
			wErrMsg = TP_ERR_MEMALLOC;
			return;
		}

		ZeroMemory( sqImage, iBufferSize );

		if( dwFlags & TP_DDS_FILE )
		{
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;
		}

		//squarify mage
		for( int i = 0; i < m_iHeight; i++ )
		{
			memcpy( sqImagePtr, pImagePtr, 3 * m_iWidth );
			pImagePtr	+= 3 * m_iWidth;
			sqImagePtr	+= 3 * m_iSqWidth;
		}
	
		m_iHeight			= m_iSqHeight;
		m_iWidth			= m_iSqWidth;

		SafeDeleteArray( m_pImage );
		m_pImage = sqImage;
	}
	else
	{
		surf_desc.dwHeight	= m_iHeight;
		surf_desc.dwWidth	= m_iWidth;

		iBufferSize = 3 * m_iHeight * m_iWidth;

		if( dwFlags & TP_DDS_FILE )
		{
			iBufferSize += 124 + 4;

			//make copy of an array
			sqImage		= new BYTE[ iBufferSize ];
			sqImagePtr	= sqImage;

			if( !sqImage )
			{
				wErrMsg = TP_ERR_MEMALLOC;
				return;
			}
			
			ZeroMemory( sqImage, iBufferSize );
			
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;

			memcpy( sqImagePtr, m_pImage, 3 * m_iWidth * m_iHeight );

			SafeDeleteArray( m_pImage );
			m_pImage = sqImage;
		}
	}
}

void TexturePreprocessor::ChangeFormatTo_A4R4G4B4( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg )
{
	DWORD			dwMagic = 542327876;
	DDSURFACEDESC2	surf_desc;
	BYTE*			sqImage		= NULL;
	BYTE*			sqImagePtr	= NULL;
	BYTE*			pImagePtr	= NULL;

	//check flags
	if( !(dwFlags & TP_DDS_FILE) && !(dwFlags & TP_DATA_BUFFER) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	if( !(dwFlags & TP_SQUARED) && !(dwFlags & TP_NON_SQUARED) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	
	//surface
	ZeroMemory( &surf_desc, sizeof(DDSURFACEDESC2) );

	surf_desc.dwSize = 124;
	surf_desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
	
	//caps
	surf_desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	//pixel format
	surf_desc.ddpfPixelFormat.dwSize			= 32;
	surf_desc.ddpfPixelFormat.dwFlags			= DDPF_RGB | DDPF_ALPHAPIXELS;
	surf_desc.ddpfPixelFormat.dwRGBBitCount		= 16;
	surf_desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xf000;
	surf_desc.ddpfPixelFormat.dwRBitMask		= 0x0f00;
	surf_desc.ddpfPixelFormat.dwGBitMask		= 0x00f0;
	surf_desc.ddpfPixelFormat.dwBBitMask		= 0x000f;


	//convert m_pImage 2 A4R4G4B4  ==floyd-steinberg DITHERING==
	BYTE*	pImage			= NULL;
	BYTE*	pTemp			= NULL;
	BYTE*	pImageTemp		= NULL;
	WORD*	pImageTemp2		= NULL;
	ErrorBlock*	pErrorVals	= NULL;

	pImage		= new BYTE[ 2 * m_iWidth * m_iHeight ];
	pErrorVals	= new ErrorBlock[ m_iWidth * m_iHeight ];
	pImageTemp2	= (WORD*)pImage;
	ZeroMemory( pImage, 2 * m_iWidth * m_iHeight );
	ZeroMemory( pErrorVals, sizeof(ErrorBlock) * m_iWidth * m_iHeight );
	
	pImageTemp2	= (WORD*)pImage;
	
	int a, r, g, b, _a, _r, _g, _b;
	float hex_part_r, hex_part_g, hex_part_b, hex_part_a;
	int indx, t_indx;

	//// dither image ////
	//collect error values
	for( int i = 0; i < m_iHeight; i++ )
	{
		for( int j = 0; j < m_iWidth; j++ )
		{
			indx = i * m_iWidth + j;

			//pixel itself
			_a = ( ( ((DWORD*)m_pImage)[indx] >> 24 ) & 0xff ) + pErrorVals[indx].a;
			_r = ( ( ((DWORD*)m_pImage)[indx] >> 16 ) & 0xff ) + pErrorVals[indx].r;
			_g = ( ( ((DWORD*)m_pImage)[indx] >> 8 ) & 0xff ) + pErrorVals[indx].g;
			_b = ( ((DWORD*)m_pImage)[indx] & 0xff ) + pErrorVals[indx].b;

			ClampColor( _a );
			ClampColor( _r );
			ClampColor( _g );
			ClampColor( _b );


			DWORD temp = ( ( _a & 0xff ) << 24 ) | ( ( _r & 0xff ) << 16 ) | ( ( _g & 0xff ) << 8 ) | ( _b & 0xff );
			WORD temp2 = ConvertTo_A4R4G4B4( &temp );

			GetColorsFrom_A4R4G4B4( &temp2, r, g, b, a );
			
			pImageTemp2[indx] = temp2;

			hex_part_r = (float)( _r - r ) / 16;
			hex_part_g = (float)( _g - g ) / 16;
			hex_part_b = (float)( _b - b ) / 16;
			hex_part_a = (float)( _a - a ) / 16;

			if( j + 1 < m_iWidth )
			{
				t_indx = indx + 1;
				pErrorVals[t_indx].r += hex_part_r * 7;
				pErrorVals[t_indx].g += hex_part_g * 7;
				pErrorVals[t_indx].b += hex_part_b * 7;
				pErrorVals[t_indx].a += hex_part_a * 7;
			}

			if( i + 1 < m_iHeight )
			{
				t_indx = indx + m_iWidth;

				pErrorVals[t_indx].r += hex_part_r * 5;
				pErrorVals[t_indx].g += hex_part_g * 5;
				pErrorVals[t_indx].b += hex_part_b * 5;
				pErrorVals[t_indx].a += hex_part_a * 5;


				if( j + 1 < m_iWidth )
				{ 
					t_indx = indx + m_iWidth + 1;

					pErrorVals[t_indx].r += hex_part_r;
					pErrorVals[t_indx].g += hex_part_g;
					pErrorVals[t_indx].b += hex_part_b;
					pErrorVals[t_indx].a += hex_part_a;
				}

				if( j - 1 > 0 )
				{
					t_indx = indx + m_iWidth - 1;

					pErrorVals[t_indx].r += hex_part_r * 3;
					pErrorVals[t_indx].g += hex_part_g * 3;
					pErrorVals[t_indx].b += hex_part_b * 3;
					pErrorVals[t_indx].a += hex_part_a * 3;
				}
			}

		}
	}		

	//replace current image
	SafeDeleteArray( m_pImage );
	SafeDeleteArray( pErrorVals );
	m_pImage = (BYTE*)pImageTemp2;

	pImagePtr = m_pImage;


	// sq requested
	if( dwFlags & TP_SQUARED )
	{
		surf_desc.dwHeight	= m_iSqHeight;
		surf_desc.dwWidth	= m_iSqWidth;

		iBufferSize = 2 * m_iSqWidth * m_iSqHeight;

		if( dwFlags & TP_DDS_FILE )
			iBufferSize += 124 + 4;

		//make copy of an array
		sqImage		= new BYTE[ iBufferSize ];
		sqImagePtr	= sqImage;

		if( !sqImage )
		{
			wErrMsg = TP_ERR_MEMALLOC;
			return;
		}

		ZeroMemory( sqImage, iBufferSize );

		if( dwFlags & TP_DDS_FILE )
		{
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;
		}

		//squarify mage
		for( int i = 0; i < m_iHeight; i++ )
		{
			memcpy( sqImagePtr, pImagePtr, 2 * m_iWidth );
			pImagePtr	+= 2 * m_iWidth;
			sqImagePtr	+= 2 * m_iSqWidth;
		}
	
		m_iHeight			= m_iSqHeight;
		m_iWidth			= m_iSqWidth;

		SafeDeleteArray( m_pImage );
		m_pImage = sqImage;
	}
	else
	{
		surf_desc.dwHeight	= m_iHeight;
		surf_desc.dwWidth	= m_iWidth;

		iBufferSize = 2 * m_iHeight * m_iWidth;

		if( dwFlags & TP_DDS_FILE )
		{
			iBufferSize += 124 + 4;

			//make copy of an array
			sqImage		= new BYTE[ iBufferSize ];
			sqImagePtr	= sqImage;

			if( !sqImage )
			{
				wErrMsg = TP_ERR_MEMALLOC;
				return;
			}
			
			ZeroMemory( sqImage, iBufferSize );
			
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;

			memcpy( sqImagePtr, m_pImage, 2 * m_iWidth * m_iHeight );

			SafeDeleteArray( m_pImage );
			m_pImage = sqImage;
		}
	}
}

void TexturePreprocessor::ChangeFormatTo_A1R5G5B5( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg )
{
	DWORD			dwMagic = 542327876;
	DDSURFACEDESC2	surf_desc;
	BYTE*			sqImage		= NULL;
	BYTE*			sqImagePtr	= NULL;
	BYTE*			pImagePtr	= NULL;

	//check flags
	if( !(dwFlags & TP_DDS_FILE) && !(dwFlags & TP_DATA_BUFFER) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	if( !(dwFlags & TP_SQUARED) && !(dwFlags & TP_NON_SQUARED) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	
	//surface
	ZeroMemory( &surf_desc, sizeof(DDSURFACEDESC2) );

	surf_desc.dwSize = 124;
	surf_desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
	
	//caps
	surf_desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	//pixel format
	surf_desc.ddpfPixelFormat.dwSize			= 32;
	surf_desc.ddpfPixelFormat.dwFlags			= DDPF_RGB | DDPF_ALPHAPIXELS;
	surf_desc.ddpfPixelFormat.dwRGBBitCount		= 16;
	surf_desc.ddpfPixelFormat.dwRGBAlphaBitMask = 32768;
	surf_desc.ddpfPixelFormat.dwRBitMask		= 31744;
	surf_desc.ddpfPixelFormat.dwGBitMask		= 992;
	surf_desc.ddpfPixelFormat.dwBBitMask		= 31;

	//convert m_pImage 2 A1R5G5B5  ==floyd-steinberg DITHERING==
	BYTE*	pImage		= NULL;
	BYTE*	pTemp		= NULL;
	BYTE*	pImageTemp	= NULL;
	WORD*	pImageTemp2	= NULL;
	ErrorBlock*	pErrorVals	= NULL;

	pImage		= new BYTE[ 2 * m_iWidth * m_iHeight ];
	pErrorVals	= new ErrorBlock[ m_iWidth * m_iHeight ];
	pImageTemp2	= (WORD*)pImage;
	ZeroMemory( pImage, 2 * m_iWidth * m_iHeight );
	ZeroMemory( pErrorVals, sizeof(ErrorBlock) * m_iWidth * m_iHeight );


	//convert to 565 format
	//for( int i = 0; i < m_iWidth * m_iHeight; i++ )
	//	pImageTemp2[i] = ConvertTo_R5G6B5( &m_pImage[i] );

	
	pImageTemp2	= (WORD*)pImage;
	
	int a, r, g, b, _a, _r, _g, _b;
	float hex_part_r, hex_part_g, hex_part_b;
	int indx, t_indx;

	//// dither image ////
	//collect error values
	for( int i = 0; i < m_iHeight; i++ )
	{
		for( int j = 0; j < m_iWidth; j++ )
		{
			indx = i * m_iWidth + j;

			//pixel itself
			_a = 0;

			// convert alpha according to flags
			if( dwFlags & TP_ALPHA_MIDDLE )
			{
				if( ( ( ((DWORD*)m_pImage)[indx] >> 24 ) & 0xff ) > 128 )
					_a = 255;
			}
			else
			{
				if( ( ( ((DWORD*)m_pImage)[indx] >> 24 ) & 0xff ) > 0 )
					_a = 255;
			}

			_r = ( ( ((DWORD*)m_pImage)[indx] >> 16 ) & 0xff ) + pErrorVals[indx].r;
			_g = ( ( ((DWORD*)m_pImage)[indx] >> 8 ) & 0xff ) + pErrorVals[indx].g;
			_b = ( ((DWORD*)m_pImage)[indx] & 0xff ) + pErrorVals[indx].b;

			//ClampColor( _a );
			ClampColor( _r );
			ClampColor( _g );
			ClampColor( _b );
				
			DWORD temp = ( ( _a & 0xff ) << 24 ) | ( ( _r & 0xff ) << 16 ) | ( ( _g & 0xff ) << 8 ) | ( _b & 0xff );
			WORD temp2 = ConvertTo_A1R5G5B5( &temp );

			GetColorsFrom_A1R5G5B5( &temp2, r, g, b, a );
			
			pImageTemp2[indx] = temp2;

			hex_part_r = (float)( _r - r ) / 16;
			hex_part_g = (float)( _g - g ) / 16;
			hex_part_b = (float)( _b - b ) / 16;

			if( j + 1 < m_iWidth )
			{
				t_indx = indx + 1;
				pErrorVals[t_indx].r += hex_part_r * 7;
				pErrorVals[t_indx].g += hex_part_g * 7;
				pErrorVals[t_indx].b += hex_part_b * 7;
			}

			if( i + 1 < m_iHeight )
			{
				t_indx = indx + m_iWidth;

				pErrorVals[t_indx].r += hex_part_r * 5;
				pErrorVals[t_indx].g += hex_part_g * 5;
				pErrorVals[t_indx].b += hex_part_b * 5;

				if( j + 1 < m_iWidth )
				{ 
					t_indx = indx + m_iWidth + 1;

					pErrorVals[t_indx].r += hex_part_r;
					pErrorVals[t_indx].g += hex_part_g;
					pErrorVals[t_indx].b += hex_part_b;
				}

				if( j - 1 > 0 )
				{
					t_indx = indx + m_iWidth - 1;

					pErrorVals[t_indx].r += hex_part_r * 3;
					pErrorVals[t_indx].g += hex_part_g * 3;
					pErrorVals[t_indx].b += hex_part_b * 3;
				}
			}

		}
	}

	//replace current image
	SafeDeleteArray( m_pImage );
	SafeDeleteArray( pErrorVals );
	m_pImage = (BYTE*)pImageTemp2;

	pImagePtr = m_pImage;


	// sq requested
	if( dwFlags & TP_SQUARED )
	{
		surf_desc.dwHeight	= m_iSqHeight;
		surf_desc.dwWidth	= m_iSqWidth;

		iBufferSize = 2 * m_iSqWidth * m_iSqHeight;

		if( dwFlags & TP_DDS_FILE )
			iBufferSize += 124 + 4;

		//make copy of an array
		sqImage		= new BYTE[ iBufferSize ];
		sqImagePtr	= sqImage;

		if( !sqImage )
		{
			wErrMsg = TP_ERR_MEMALLOC;
			return;
		}

		ZeroMemory( sqImage, iBufferSize );

		if( dwFlags & TP_DDS_FILE )
		{
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;
		}

		//squarify mage
		for( int i = 0; i < m_iHeight; i++ )
		{
			memcpy( sqImagePtr, pImagePtr, 2 * m_iWidth );
			pImagePtr	+= 2 * m_iWidth;
			sqImagePtr	+= 2 * m_iSqWidth;
		}
	
		m_iHeight			= m_iSqHeight;
		m_iWidth			= m_iSqWidth;

		SafeDeleteArray( m_pImage );
		m_pImage = sqImage;
	}
	else
	{
		surf_desc.dwHeight	= m_iHeight;
		surf_desc.dwWidth	= m_iWidth;

		iBufferSize = 2 * m_iHeight * m_iWidth;

		if( dwFlags & TP_DDS_FILE )
		{
			iBufferSize += 124 + 4;

			//make copy of an array
			sqImage		= new BYTE[ iBufferSize ];
			sqImagePtr	= sqImage;

			if( !sqImage )
			{
				wErrMsg = TP_ERR_MEMALLOC;
				return;
			}
			
			ZeroMemory( sqImage, iBufferSize );
			
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;

			memcpy( sqImagePtr, m_pImage, 2 * m_iWidth * m_iHeight );

			SafeDeleteArray( m_pImage );
			m_pImage = sqImage;
		}
	}
}

void TexturePreprocessor::ChangeFormatTo_R5G6B5( DWORD dwFlags, int& iBufferSize, WORD& wErrMsg )
{
	DWORD			dwMagic = 542327876;
	DDSURFACEDESC2	surf_desc;
	BYTE*			sqImage		= NULL;
	BYTE*			sqImagePtr	= NULL;
	BYTE*			pImagePtr	= NULL;

	//check flags
	if( !(dwFlags & TP_DDS_FILE) && !(dwFlags & TP_DATA_BUFFER) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	if( !(dwFlags & TP_SQUARED) && !(dwFlags & TP_NON_SQUARED) )
	{
		wErrMsg = TP_ERR_NOFLAG;
	}

	
	//surface
	ZeroMemory( &surf_desc, sizeof(DDSURFACEDESC2) );

	surf_desc.dwSize = 124;
	surf_desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
	
	//caps
	surf_desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	//pixel format
	surf_desc.ddpfPixelFormat.dwSize			= 32;
	surf_desc.ddpfPixelFormat.dwFlags			= DDPF_RGB;
	surf_desc.ddpfPixelFormat.dwRGBBitCount		= 16; 
	surf_desc.ddpfPixelFormat.dwRBitMask		= 0xf800;
	surf_desc.ddpfPixelFormat.dwGBitMask		= 0x07e0;
	surf_desc.ddpfPixelFormat.dwBBitMask		= 0x001f;

	//convert m_pImage 2 R5G6B5  ==floyd-steinberg DITHERING==
	BYTE*	pImage		= NULL;
	BYTE*	pTemp		= NULL;
	BYTE*	pImageTemp	= NULL;
	ErrorBlock*	pErrorVals	= NULL;

	pImage		= new BYTE[ 2 * m_iWidth * m_iHeight ];
	pErrorVals	= new ErrorBlock[ m_iWidth * m_iHeight ];
	ZeroMemory( pImage, 2 * m_iWidth * m_iHeight );
	ZeroMemory( pErrorVals, sizeof(ErrorBlock) * m_iWidth * m_iHeight );
	
	int r, g, b, _r, _g, _b, t_r = 0, t_g = 0, t_b = 0;
	int indx, t_indx;

	//// dither image ////
	//collect error values
	for( int i = 0; i < m_iHeight; i++ )
	{
		for( int j = 0; j < m_iWidth; j++ )
		{
			indx = i * m_iWidth + j;

			//pixel itself
			_r = ( ( ((DWORD*)m_pImage)[indx] >> 16 ) & 0xff ) + pErrorVals[indx].r;
			_g = ( ( ((DWORD*)m_pImage)[indx] >> 8 ) & 0xff ) + pErrorVals[indx].g;
			_b = ( ((DWORD*)m_pImage)[indx] & 0xff ) + pErrorVals[indx].b;

			ClampColor( _r );
			ClampColor( _g );
			ClampColor( _b );

			DWORD temp = ( ( _r & 0xff ) << 16 ) | ( ( _g & 0xff ) << 8 ) | ( _b & 0xff );
			((WORD*)pImage)[indx] = ConvertTo_R5G6B5( &temp );
			GetColorsFrom_R5G6B5( &((WORD*)pImage)[indx], r, g, b );

			t_r = _r - r;
			t_g = _g - g;
			t_b = _b - b;

			if( t_r < 0 )
				t_r = 63 + t_r * -1;

			if( t_g < 0 )
				t_g = 63 + t_g * -1;

			if( t_b < 0 )
				t_b = 63 + t_b * -1;

			if( j + 1 < m_iWidth )
			{
				t_indx = indx + 1;
				pErrorVals[t_indx].r += m_fPrecalcErrors[t_r][3];
				pErrorVals[t_indx].g += m_fPrecalcErrors[t_g][3];
				pErrorVals[t_indx].b += m_fPrecalcErrors[t_b][3];
			}

			if( i + 1 < m_iHeight )
			{
				t_indx = indx + m_iWidth;

				pErrorVals[t_indx].r += m_fPrecalcErrors[t_r][2];
				pErrorVals[t_indx].g += m_fPrecalcErrors[t_g][2];
				pErrorVals[t_indx].b += m_fPrecalcErrors[t_b][2];


				if( j + 1 < m_iWidth )
				{ 
					t_indx = indx + m_iWidth + 1;

					pErrorVals[t_indx].r += m_fPrecalcErrors[t_r][0];
					pErrorVals[t_indx].g += m_fPrecalcErrors[t_g][0];
					pErrorVals[t_indx].b += m_fPrecalcErrors[t_b][0];
				}

				if( j - 1 > 0 )
				{
					t_indx = indx + m_iWidth - 1;

					pErrorVals[t_indx].r += m_fPrecalcErrors[t_r][1];
					pErrorVals[t_indx].g += m_fPrecalcErrors[t_g][1];
					pErrorVals[t_indx].b += m_fPrecalcErrors[t_b][1];
				}
			}

		}
	}		

	//replace current image
	SafeDeleteArray( m_pImage );
	SafeDeleteArray( pErrorVals );
	m_pImage = pImage;

	pImagePtr = m_pImage;


	// sq requested
	if( dwFlags & TP_SQUARED )
	{
		surf_desc.dwHeight	= m_iSqHeight;
		surf_desc.dwWidth	= m_iSqWidth;

		iBufferSize = 2 * m_iSqHeight * m_iSqWidth;

		if( dwFlags & TP_DDS_FILE )
			iBufferSize += 124 + 4;

		//make copy of an array
		sqImage		= new BYTE[ iBufferSize ];
		sqImagePtr	= sqImage;

		if( !sqImage )
		{
			wErrMsg = TP_ERR_MEMALLOC;
			return;
		}

		ZeroMemory( sqImage, iBufferSize );

		if( dwFlags & TP_DDS_FILE )
		{
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;
		}

		//squarify mage
		for( int i = 0; i < m_iHeight; i++ )
		{
			memcpy( sqImagePtr, pImagePtr, 2 * m_iWidth );
			pImagePtr	+= 2 * m_iWidth;
			sqImagePtr	+= 2 * m_iSqWidth;
		}

		m_iHeight			= m_iSqHeight;
		m_iWidth			= m_iSqWidth;
	
		SafeDeleteArray( m_pImage );
		m_pImage = sqImage;
	}
	else
	{
		surf_desc.dwHeight	= m_iHeight;
		surf_desc.dwWidth	= m_iWidth;

		iBufferSize = 2 * m_iHeight * m_iWidth;

		if( dwFlags & TP_DDS_FILE )
		{
			iBufferSize += 124 + 4;

			//make copy of an array
			sqImage		= new BYTE[ iBufferSize ];
			sqImagePtr	= sqImage;

			if( !sqImage )
			{
				wErrMsg = TP_ERR_MEMALLOC;
				return;
			}
			
			ZeroMemory( sqImage, iBufferSize );
			
			memcpy( sqImagePtr, &dwMagic, 4 );
			sqImagePtr += 4;
			memcpy( sqImagePtr, &surf_desc, 124 );
			sqImagePtr += 124;

			memcpy( sqImagePtr, m_pImage, 2 * m_iWidth * m_iHeight );

			SafeDeleteArray( m_pImage );
			m_pImage = sqImage;
		}
	}
}

int TexturePreprocessor::DecodeAlpha( const int x, const int y, const DXT1Block &rBlock )
{
	// cast to alpha block
	const DXT3AlphaBlock &rAlpha = reinterpret_cast< const DXT3AlphaBlock& >( rBlock );

	// get index
	const int nIndex = (y * 4 + x) / 2;
	unsigned int nValue = rAlpha.nAlpha[nIndex];

	// switch between upper and lower nibble
	if( x & 1 )
	{
		nValue &= 0xf0;
	}
	else
	{
		nValue &= 0x0f;
		nValue <<= 4;
	}

	//hack!!! bit shifting gives error: returns alpha in range 0 - 240, so 15 vals are lost
	//use linear distribution to compensate the error
	return nValue + ( 15.0f / 240 * nValue );
}

// Decode alpha interpolated value
int TexturePreprocessor::DecodeInterpolatedAlpha( const int x, const int y, const DXT1Block &rBlock )
{
	// cast to alpha block
	const DXT5AlphaBlock &rAlpha = reinterpret_cast< const DXT5AlphaBlock& >( rBlock );

	unsigned int nAlpha;
	int			 a0		= rAlpha.nAlpha0;
	int			 a1		= rAlpha.nAlpha1;
	int			 i		= ( y * 4 + x) * 3;	// bit index
	int			 j		= i / 8;				// byte index
	int			 nCode	= rAlpha.nBits[j];
	
	i -= j * 8;		// get number of bits to shift
	nCode >>= i;	// shift to bit offset
	nCode &= 7;		// mask upper bits away
	i = 8 - i;		// find out how many bits are available in this byte

	if( i == 1 )
	{
		// need two more bits from next byte
		nCode |= ( rAlpha.nBits[ j + 1 ] & 3 ) << 1;
	}
	else if( i == 2 )
	{
		// need one more bit from next byte
		nCode |= ( rAlpha.nBits[ j + 1 ] & 1 ) << 2;
	} 
	else
	{
		// there are enough bits in this byte - code is valid
	}

	if( nCode == 0 )
	{
		nAlpha = a0;
	}
	else if( nCode == 1 )
	{
		nAlpha = a1;
	}
	else if( rAlpha.nAlpha0 > rAlpha.nAlpha1 )
	{
		// Use interpolation variant A)
		// instead of a switch statement, I put the pattern into a single formula,
		// which adds 2 more ops (minus and assignment) but keeps the code clean
		i = 8 - nCode;				
		nAlpha = ( a0 * i + a1 * ( 7 - i ) ) / 7;
	}
	else if ( nCode < 6 )
	{
		// Use interpolation variant B)
		i = 6 - nCode;
		nAlpha = ( a0 * i + a1 * ( 5 - i ) ) / 5;
	} else
	{
		// Either opaque or fully transparent
		nAlpha = ( nCode == 6 ) ? 0 : 255;
	}

	return nAlpha & 0xff;
}

void TexturePreprocessor::DecompressTexture( string strCompressionFormat, unsigned char* pFile )
{
	unsigned char*	pStream = pFile;
	DWORD*			pImage = (DWORD*)m_pImage;

	pStream += 124 + 4;

	for( int i = 0; i < (m_iHeight / 4); i++ )
	{
		for( int j = 0; j < (m_iWidth / 4); j++ )
		{
			/* read block */
			DXT1Block m_blOne, m_blAlpha;

			if( strCompressionFormat == "DXT3" || strCompressionFormat == "DXT5" )
			{
				memcpy( &m_blAlpha, pStream, sizeof(DXT1Block) );
				pStream += sizeof(DXT1Block);
			}

			memcpy( &m_blOne, pStream, sizeof(DXT1Block) );
			pStream += sizeof(DXT1Block);

			/* decompress block */
			int r0, g0, b0, r1, g1, b1, alpha = 255;
			DWORD auiColor[5];

			// expand block base colors (two 16 bit color vectors)
			r0 = m_blOne.c0.r << 3; r1 = m_blOne.c1.r << 3;
			g0 = m_blOne.c0.g << 2; g1 = m_blOne.c1.g << 2;
			b0 = m_blOne.c0.b << 3; b1 = m_blOne.c1.b << 3;


			auiColor[0] = CombineColor( r0, g0, b0, alpha );
			auiColor[1] = CombineColor( r1, g1, b1, alpha );
			
			int rx, gx, bx;

			// extrapolate packed colors
			rx = DECODE( r0, r1 );
			gx = DECODE( g0, g1 );
			bx = DECODE( b0, b1 );

			auiColor[2] = CombineColor( rx, gx, bx, alpha );

			rx = DECODE( r1, r0 );
			gx = DECODE( g1, g0 );
			bx = DECODE( b1, b0 );

			auiColor[3] = CombineColor( rx, gx, bx, alpha );

			// fourth color vector is interpolated from the two first (explicit) color vectors
			auiColor[4] = CombineColor( ( r0 + r1 ) >> 1, ( g0 + g1 ) >> 1, ( b0 + b1 ) >> 1, alpha );

			int aiIndex[4] = { 0, 1, 2, 3 };

			if( m_blOne.v0 < m_blOne.v1 )
			{
				// block contains transparent pixels - let code "2" map to color 4 and make color 3
				// fully transparent.
				aiIndex[2] = 4;
				auiColor[3]= 0;
			}

			for( int x = 0; x < 4; x++ )
			{
				for( int y = 0; y < 4; y++ )
				{
					// find color index for this pixel
					int nCode = m_blOne.code[y] & CODEMASK[x];
					nCode >>= CODESHIFT[x];
					
					//unmap color vector
					int indx = (i * 4 + y) * m_iWidth + (j * 4 + x); 

					if( strCompressionFormat == "DXT1" )
					{
						pImage[indx] = ( ( auiColor[ aiIndex[ nCode ] ] >> 16 ) & 0xff ) << 16 |
										 ( ( auiColor[ aiIndex[ nCode ] ] >> 8 ) & 0xff ) << 8 |
										 ( auiColor[ aiIndex[ nCode ] ] & 0xff );
					}
					else if( strCompressionFormat == "DXT3" )
					{
						pImage[indx] = ( DecodeAlpha( x, y, m_blAlpha ) << 24 ) |
										 ( ( auiColor[ aiIndex[ nCode ] ] >> 16 ) & 0xff ) << 16 |
										 ( ( auiColor[ aiIndex[ nCode ] ] >> 8 ) & 0xff ) << 8 |
										 ( auiColor[ aiIndex[ nCode ] ] & 0xff );
					}
					else if( strCompressionFormat == "DXT5" )
					{
						pImage[indx] = ( DecodeInterpolatedAlpha( x, y, m_blAlpha ) << 24 ) |
										 ( ( auiColor[ aiIndex[ nCode ] ] >> 16 ) & 0xff ) << 16 |
										 ( ( auiColor[ aiIndex[ nCode ] ] >> 8 ) & 0xff ) << 8 |
										 ( auiColor[ aiIndex[ nCode ] ] & 0xff );
					}
				}
			}

		} //for( int j = 0; j < (m_iWidth / 4); j++ )
	} // for( int i = 0; i < (m_iHeight / 4); i++ )
}