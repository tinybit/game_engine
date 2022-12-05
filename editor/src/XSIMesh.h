#pragma once

#include "sys/stdafx.h"
#include "Vector.h"
#include "utils/Common.h"
#include "EfnCharacterData.cpp"

class CXSIMesh
{
	public:
		/* constructors */
		CXSIMesh()  { Initialize(); };               
		~CXSIMesh() { Destroy();    };
		void  Initialize();
		void  Destroy();

		/* utility functions */
		void	GetColor( float &a, float &r, float &g, float &b );
		void	SetColor( float a, float r, float g, float b );
		void	GetColor( D3DCOLORVALUE& color );

		char*	GetName()				{ return m_szName; };
		void	SetName( char* szName )	{ sprintf( m_szName, "%s", szName ); };

		void	InitTrianglesList( int iTrianglesNum );
		int		GetTrianglesCount()		{ return m_iTriCount; }; 
		void	SetTriangleData( int iIndex, int iValue )  { m_iTriangles[iIndex] = iValue; };

		void	InitVerteciesList( int iVerteciesNum );
		int		GetVerteciesCount()		{ return m_iVertCount; };
		void	SetVertexData( int iFrameIndex, int iVertexIndex, float x, float y, float z, long color );
		void	SetVertexData( int iFrameIndex, int iVertexIndex, float x, float y, float z, float nx, float ny, float nz, long color );
		void	SetNormalData( int iFrameIndex, int iNormalIndex, float x, float y, float z );

		bool	IsVisible()				{ return m_bVisible; };					//visibility
		void	SetVisibility( bool bIsVisible ) { m_bVisible = bIsVisible; };	
		void	PrecalculateNormals( CWnd* pWnd );

		void	SetUVData( int iTriIndex, int iVertexIndex, float fU, float fV, float fU2 = -1.0f, float fV2 = -1.0f );
		void	GetUVData( int iVertexIndex, float& fU, float& fV );
		//void	SetUVDataSecondary( int iVertexIndex, float fU, float fV, float fU2 = -1.0f, float fV2 = -1.0f );
		void	InitSecondaryUV();

		inline bool	VerteciesEqual( CVERTEX& vertex1, CVERTEX& vertex2 );
		void	InverseNormals();
		void	DisconnectAllTris();
		void	ConnectAllTris();

		void		SetParent( CBaseScene* pParent ) { m_pParent = pParent; };   //parent binding
		CBaseScene*	GetParent() { return m_pParent; };

		CVERTEX**		GetVerteciesList()	{ return m_cvVertecies; };
		CVERTEX_UV*		GetSecondaryUV()	{ return m_cvUV2; };
		int*			GetTrianglesList()	{ return m_iTriangles; };
		void			SwapYZCoords();
	

	private:                                             
		/* data */
		CBaseScene*		m_pParent;			//ссылка на родительский класс
		char			m_szName[64];		//имя
		int*			m_iTriangles;		//список треугольников
		int				m_iTriCount;		//кол-во треугольников в списке
		CVERTEX**		m_cvVertecies;		//список вершин
		int				m_iVertCount;		//кол-во вершин в списке   
		int				m_iInitVertCount;	//изначальное кол-во вершин в списке( до оптимизации текстурного шва )
		float			m_fColor[4];		//цвет diffuse (argb)
		bool			m_bVisible;			//флаг видимости
		CVERTEX_UV*		m_cvUV2;
};
