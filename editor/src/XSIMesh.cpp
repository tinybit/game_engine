#pragma warning( disable: 4996 )

#include "sys/stdafx.h"
#include "sys/CookieMachine.h"
#include "XSIMesh.h"
#include "XSIFile.h"
#include "EfnMeshlist.h"
#include "dialogs/ValidateDlg.h"

void CXSIMesh::InitTrianglesList( int iTrianglesNum )
{
	//clean tri list
	SAFE_DELETE_ARRAY(m_iTriangles);

	//set count
	m_iTriCount = iTrianglesNum;

	//allocate memory
	m_iTriangles = new int [iTrianglesNum * 3];
}

void CXSIMesh::InitVerteciesList( int iVerteciesNum )
{
	int i, j, frames;

	if( m_pParent->GetType() == FT_XSI )
		frames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		frames = 1;

	//clean vert list
	SAFE_DELETE_ARRAY(m_cvVertecies);

	//set count
	m_iVertCount		= iVerteciesNum;
	m_iInitVertCount	= iVerteciesNum;

	//allocate memory
	m_cvVertecies = new CVERTEX*[frames];

	for( i = 0; i < frames; i++ )
	{
		m_cvVertecies[i] = new CVERTEX[iVerteciesNum];

		ZeroMemory( m_cvVertecies[i], sizeof( CVERTEX ) * iVerteciesNum );

		for( j = 0; j < iVerteciesNum; j++ )
		{
			m_cvVertecies[i][j].u = -1.0f;
			m_cvVertecies[i][j].v = -1.0f;
			m_cvVertecies[i][j].color = 0xffffffff;
		}
	}
}

void CXSIMesh::InverseNormals()
{
	int frames;
	
	if( m_pParent->GetType() == FT_XSI )
		frames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		frames = 1;

	int		iTemp;
	CVector temp;

	// inverse normals
	for( int i = 0; i < frames; i++ )
	{
		for( int j = 0; j < m_iVertCount; j++ )
		{
			temp.SetData( m_cvVertecies[i][j].nx, m_cvVertecies[i][j].ny, m_cvVertecies[i][j].nz );
			temp.Flip();

			m_cvVertecies[i][j].nx = temp.x; 
			m_cvVertecies[i][j].ny = temp.y;
			m_cvVertecies[i][j].nz = temp.z;
		}
	}

	// convert tri data
	for( int i = 0; i < m_iTriCount; i++ )
	{
		iTemp = m_iTriangles[i * 3];
		m_iTriangles[i * 3] = m_iTriangles[i * 3 + 2];
		m_iTriangles[i * 3 + 2] = iTemp;
	}
}

void CXSIMesh::Destroy()
{
	int i, frames;
	
	if( m_pParent->GetType() == FT_XSI )
		frames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		frames = 1;

	if( m_cvVertecies )
	{
		for (i = 0; i < frames; i++)
			SAFE_DELETE_ARRAY( (m_cvVertecies[i]) );

		 SAFE_DELETE_ARRAY( m_cvVertecies );
	}

	SAFE_DELETE_ARRAY( m_iTriangles );
	SAFE_DELETE_ARRAY( m_cvUV2 );
  
	//reset counters
	m_iTriCount = m_iVertCount = 0;

	//reset color
	m_fColor[0] = m_fColor[1] = m_fColor[2] = 0.0;

	//reset parent class pointer
	m_pParent = NULL;
}

void CXSIMesh::InitSecondaryUV()
{
	m_cvUV2 = new CVERTEX_UV[m_iVertCount];
}

void CXSIMesh::Initialize()
{
	//reset lists pointers
	m_iTriangles	= NULL;
	m_cvVertecies	= NULL;
	m_cvUV2			= NULL;

	//reset counters
	m_iTriCount = m_iTriCount = 0;

	//reset color
	m_fColor[0] = m_fColor[1] = m_fColor[2] = 0.0;

	//reset name
	sprintf( m_szName, "%s", "" );

	//reset parent class pointer
	m_pParent	= NULL;
	m_bVisible	= true;
}

void CXSIMesh::SetVertexData( int iFrameIndex, int iVertexIndex, float x, float y, float z, long color )
{
	int frames;

	if( m_pParent->GetType() == FT_XSI )
		frames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		frames = 1;

	//check frames range
	if( iFrameIndex >= 0 && iFrameIndex < frames )
	{
		//check vertex range
		if( iVertexIndex >= 0 && iVertexIndex < m_iVertCount )
		{
			m_cvVertecies[iFrameIndex][iVertexIndex].x		= x;
			m_cvVertecies[iFrameIndex][iVertexIndex].y		= y;
			m_cvVertecies[iFrameIndex][iVertexIndex].z		= z;
			m_cvVertecies[iFrameIndex][iVertexIndex].color	= color;
		}
	}
}

void CXSIMesh::SetVertexData( int iFrameIndex, int iVertexIndex, float x, float y, float z, float nx, float ny, float nz, long color )
{
	int frames;

	if( m_pParent->GetType() == FT_XSI )
		frames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		frames = 1;

	//check frames range
	if(iFrameIndex >= 0 && iFrameIndex < frames)
	{
		//check vertex range
		if(iVertexIndex >= 0 && iVertexIndex < m_iVertCount)
		{
			m_cvVertecies[iFrameIndex][iVertexIndex].x	= x;
			m_cvVertecies[iFrameIndex][iVertexIndex].y	= y;
			m_cvVertecies[iFrameIndex][iVertexIndex].z	= z;
			m_cvVertecies[iFrameIndex][iVertexIndex].nx = nx;
			m_cvVertecies[iFrameIndex][iVertexIndex].ny = ny;
			m_cvVertecies[iFrameIndex][iVertexIndex].nz = nz;
			m_cvVertecies[iFrameIndex][iVertexIndex].color = color;
		}
	}
}

void CXSIMesh::SetNormalData( int iFrameIndex, int iNormalIndex, float x, float y, float z )
{
	int frames;

	if( m_pParent->GetType() == FT_XSI )
		frames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		frames = 1;

	//check frames range
	if(iFrameIndex >= 0 && iFrameIndex < frames)
	{
		//check vertex range
		if(iNormalIndex >= 0 && iNormalIndex < m_iTriCount * 3)
		{
			m_cvVertecies[iFrameIndex][m_iTriangles[iNormalIndex]].nx = x;
			m_cvVertecies[iFrameIndex][m_iTriangles[iNormalIndex]].ny = y;
			m_cvVertecies[iFrameIndex][m_iTriangles[iNormalIndex]].nz = z;
		}
	}
}

void CXSIMesh::SetColor( float a, float r, float g, float b )
{
	m_fColor[0] = r;
	m_fColor[1] = g;
	m_fColor[2] = b;
	m_fColor[3] = a;
}

void CXSIMesh::GetColor( float& a, float& r, float& g, float& b )
{
	r = m_fColor[0];
	g = m_fColor[1];
	b = m_fColor[2];
	a = m_fColor[3];
}

void CXSIMesh::GetColor( D3DCOLORVALUE& color )
{
	color.r = m_fColor[0];
	color.g = m_fColor[1];
	color.b = m_fColor[2];
	color.a = m_fColor[3];
}

void CXSIMesh::PrecalculateNormals( CWnd* pWnd )
{
	int i, j, k, vert1, vert2, vert3, iFrames;
	
	CVector vA, vB, vC, vTmp1, vTmp2;
	CVector** vTriNormals;
	CString msg, msg_temp;

	if( m_pParent->GetType() == FT_XSI )
		iFrames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		iFrames = 1;

	//-----------------------------polygon normals calculations---------------------------
	//allocate memory
	vTriNormals = new CVector*[iFrames];

	for( i = 0; i < iFrames; i++ )
	{
	  vTriNormals[i] = new CVector[m_iTriCount];
	}

	for( j = 0; j < iFrames; j++ )
	{
		for (i = 0; i < m_iTriCount; i++)
		{
			if( pWnd )
			{
				if( (j * m_iTriCount + i + 1) % 2000 == 0 )
				{
					int curr_tri_i_indx = j * m_iTriCount + i + 1;

					if( (j * m_iTriCount + i + 1) + 2000 > iFrames * m_iTriCount )
						curr_tri_i_indx = iFrames * m_iTriCount;

					msg.Format( "Poly normals: %i of %i",  curr_tri_i_indx,  iFrames * m_iTriCount );
					((CValidateDlg*)pWnd)->SetProgressMessageStrC( msg );

					//for(int uu = 0; uu < 2000; uu++)
					//	((CValidateDlg*)pWnd)->Step();
				}
			}

			//get indexes of vertecies from triangle
			vert1 = m_iTriangles[i * 3 + 0];
			vert2 = m_iTriangles[i * 3 + 1];
			vert3 = m_iTriangles[i * 3 + 2];

			vA.SetData(m_cvVertecies[j][vert1].x, m_cvVertecies[j][vert1].y, m_cvVertecies[j][vert1].z);
			vB.SetData(m_cvVertecies[j][vert2].x, m_cvVertecies[j][vert2].y, m_cvVertecies[j][vert2].z);
			vC.SetData(m_cvVertecies[j][vert3].x, m_cvVertecies[j][vert3].y, m_cvVertecies[j][vert3].z);

			vTmp1 = vB - vA;
			vTmp2 = vC - vA;

			vTriNormals[j][i] = vTmp1 ^ vTmp2;
			vTriNormals[j][i].Normalize();
			vTriNormals[j][i].Flip();
		}
	}

	msg_temp = msg;
	//------------------------END--polygon normals calculations---------------------------

	//-----------------------------vertex normals calculations----------------------------

	//////////////////////////////////////////////
	//		create triangles index cache		//
	//////////////////////////////////////////////
	// cache structure - 3d array of ints
	// [] - index of vertex
	// [] - list of triangles consisting this spesific vertex, [0] consits num of items of this level

	int**	tri_cache = NULL;
	int		tri_consisting_vertex;

	//build cache
	tri_cache = new int*[m_iVertCount];

	//go throught all the vertices, find out which tris belong to each of the vertices
	for( int i = 0; i < m_iVertCount; i++ )
	{
		tri_consisting_vertex = 0;

		//run through tri index
		for( int j = 0; j < m_iTriCount; j++ )
		{
			if(	m_iTriangles[j * 3 + 0] == i || 
				m_iTriangles[j * 3 + 1] == i ||
				m_iTriangles[j * 3 + 2] == i )
				tri_consisting_vertex++;
		}

		//create list of tris consisting vertex
		tri_cache[i] = new int[tri_consisting_vertex + 1];
		tri_cache[i][0] = tri_consisting_vertex;

		tri_consisting_vertex = 1;

		//copy data
		//run through tri index
		for( int j = 0; j < m_iTriCount; j++ )
		{
			if(	m_iTriangles[j * 3 + 0] == i || 
				m_iTriangles[j * 3 + 1] == i ||
				m_iTriangles[j * 3 + 2] == i )
			{
				tri_cache[i][tri_consisting_vertex] = j;
				tri_consisting_vertex++;
			}
		}
	}
	//////////////////////////////////////////////
	//	END	create triangles index cache		//
	//////////////////////////////////////////////

	int count = 0;

	for (int j = 0; j < iFrames; j++)
	{
		//summ up normals using cache
		for (k = 0; k < m_iVertCount; k++)
		{
			//update progress window
			if( pWnd )
			{
				if( (j * m_iVertCount + k) % 500 == 0 )
				{
					int curr_tri_i_indx = j * m_iVertCount + k;

					if( curr_tri_i_indx + 500 >  iFrames * m_iVertCount )
						curr_tri_i_indx =  iFrames * m_iVertCount;

					msg.Format( "%s    Vertex normals: %i of %i", msg_temp, curr_tri_i_indx,  iFrames * m_iVertCount );
					((CValidateDlg*)pWnd)->SetProgressMessageStrC( msg );
				}
			}

			count = 0;
			vTmp1.Clear();

			if( tri_cache[k][0] > 2 )
			{
				for( int m = 1; m < tri_cache[k][0]; m++ )
				{
					int tri_index = tri_cache[k][m];

					vTmp1 += vTriNormals[j][ tri_index ];
					count++;
				}
			}

			if( tri_cache[k][0] > 0 && tri_cache[k][0] < 3 )
			{
				vTmp1 += vTriNormals[j][ tri_cache[k][1] ];
				count = 1;
			}


			//calc average vector
			if( count < 0 )
			{
				vTmp1 /= (float)count;
				vTmp1.UnitVector();
			}

			vTmp1.Flip();
				
			m_cvVertecies[j][k].nx = vTmp1.x;
			m_cvVertecies[j][k].ny = vTmp1.y;
			m_cvVertecies[j][k].nz = vTmp1.z;
		}

	}  // for (j = 0; j < iFrames; j++)

	//free memory
	for( int i = 0; i < iFrames; i++ )
		SAFE_DELETE_ARRAY( vTriNormals[i] );

	SAFE_DELETE_ARRAY( vTriNormals );

	for( int i = 0; i < m_iVertCount; i++ )
		SAFE_DELETE_ARRAY( tri_cache[i] );

	SAFE_DELETE_ARRAY( tri_cache );

	//------------------------END--vertex normals calculations----------------------------
}

void CXSIMesh::ConnectAllTris()
{
	CVERTEX**	ppVertecies	= NULL;
	int			iFrames		= ((CXSIFile*)m_pParent)->GetFramesCount();
	vector<int> tmp_index, tmp_index2, tmpVec;

	//count unique vertecies 
	for( int i = 0; i < m_iTriCount * 3; i++ )
	{
		tmp_index.push_back( m_iTriangles[i] );

		for( int j = 0; j < i; j++ )
		{
			if( ( m_iTriangles[i] != m_iTriangles[j] ) && ( m_cvVertecies[0][m_iTriangles[i]] == m_cvVertecies[0][m_iTriangles[j]] ) )
			{
				tmp_index[i] = m_iTriangles[j];
				break;
			}
		}
	}

	tmp_index2.assign( tmp_index.begin(), tmp_index.end() );
	RemoveDupesFromVector( &tmp_index2 );


	// check whether model is already connected
	if( tmp_index2.size() == m_iVertCount )
		return;


	// build indexed vertex array, eliminating equal vertecies
	for( int i = 0; i < m_iTriCount * 3; i++ )
		tmpVec.push_back( -1 );

	for( int i = 0; i < m_iTriCount * 3; i++ )
	{
		for( UINT j = 0; j < tmp_index2.size(); j++ )
		{
			if( tmp_index[i] == tmp_index2[j] && tmpVec[i] == -1 )
				tmpVec[i] = j;
		}
	}
	tmp_index.assign( tmpVec.begin(), tmpVec.end() );


	ppVertecies = new CVERTEX*[iFrames];

	for( int i = 0; i < iFrames; i++ )
	{
		ppVertecies[i] = new CVERTEX[tmp_index2.size()];

		for( UINT j = 0; j < tmp_index2.size(); j++ )
			ppVertecies[i][j] = m_cvVertecies[i][tmp_index2[j]];

		SAFE_DELETE_ARRAY( m_cvVertecies[i] )
	}

	SAFE_DELETE_ARRAY( m_cvVertecies );

	m_cvVertecies = ppVertecies;


	for( int i = 0; i < m_iTriCount * 3; i++ )
		m_iTriangles[i] = tmp_index[i];

	m_iVertCount = tmp_index2.size();

	PrecalculateNormals( NULL );
}

void CXSIMesh::DisconnectAllTris()
{
	CVERTEX**	ppVertecies	= NULL;
	int			iFrames		= ((CXSIFile*)m_pParent)->GetFramesCount();

	ppVertecies = new CVERTEX*[iFrames];
	
	for( int i = 0; i < iFrames; i++ )
	{
		ppVertecies[i] = new CVERTEX[m_iTriCount * 3];

		for( int j = 0; j < m_iTriCount * 3; j++ )
		{
			ppVertecies[i][j] = m_cvVertecies[i][m_iTriangles[j]];
		}

		SAFE_DELETE_ARRAY( m_cvVertecies[i] )
	}

	SAFE_DELETE_ARRAY( m_cvVertecies );
	m_cvVertecies = ppVertecies;

	for( int i = 0; i < m_iTriCount * 3; i++ )
		m_iTriangles[i] = i;

	m_iVertCount = m_iTriCount * 3;

	PrecalculateNormals( NULL );
}

//void CXSIMesh::SetUVDataSecondary( int iVertexIndex, float fU, float fV )
//{
//	int vIdx = m_iTriangles[iVertexIndex];
//
//	if( vIdx > m_iVertCount )
//	{
//		int t = 0;
//	}
//
//	m_cvUV2[vIdx].u = fU;
//	m_cvUV2[vIdx].v = fV;
//}

void CXSIMesh::SetUVData( int iTriIndex, int iVertexIndex, float fU, float fV, float fU2, float fV2 )
{
	int vIdx = m_iTriangles[iVertexIndex];

	if( m_cvVertecies[0][vIdx].u == -1.0f && 
		m_cvVertecies[0][vIdx].v == -1.0f )
	{
		m_cvVertecies[0][vIdx].u = fU;
		m_cvVertecies[0][vIdx].v = 1.0f - fV;

		if( m_cvUV2 )
		{
			m_cvUV2[vIdx].u = fU2;
			m_cvUV2[vIdx].v = 1.0f - fV2;
		}
	}
	else if( m_cvVertecies[0][vIdx].u != fU ||
			 m_cvVertecies[0][vIdx].v != 1.0f - fV )
	{
		bool vlist_modifyed = false;
		bool dupe_found		= false;

		for( int i = 0; i < ((CXSIFile*)m_pParent)->GetFramesCount(); i++ )
		{		
			//create new vertex duoplicate for current frame
			CVERTEX vtx = m_cvVertecies[i][vIdx];

			vtx.u = fU;
			vtx.v = 1.0f - fV;


			for( int j = 0; j < m_iVertCount; j++ )
			{
				if( m_cvVertecies[0][j].x == vtx.x &&
					m_cvVertecies[0][j].y == vtx.y && 
					m_cvVertecies[0][j].z == vtx.z && 
					m_cvVertecies[0][j].u == vtx.u && 
					m_cvVertecies[0][j].v == vtx.v )
				{
					m_iTriangles[iVertexIndex] = j;
					dupe_found = true;
					break;
				}
			}

			if( dupe_found )
				break;

			vlist_modifyed = true;

			//detach vertex from the list
			CVERTEX*	pVertecies	= new CVERTEX[m_iVertCount + 1];
			

			//copy data from old vert list
			memcpy( pVertecies, m_cvVertecies[i], sizeof( CVERTEX ) * m_iVertCount );

			//copy vertex
			pVertecies[m_iVertCount]	= vtx;

			if( m_cvUV2 )
			{
				CVERTEX_UV*	pUV2		= new CVERTEX_UV[m_iVertCount + 1];

				memcpy( pUV2, m_cvUV2, sizeof( CVERTEX_UV ) * m_iVertCount );

				pUV2[m_iVertCount].u = fU2;
				pUV2[m_iVertCount].v = 1.0f - fV2;
				SAFE_DELETE_ARRAY( m_cvUV2 );
				m_cvUV2 = pUV2;
			}

			SAFE_DELETE_ARRAY( m_cvVertecies[i] );
			m_cvVertecies[i]	= pVertecies;
		}

		if( vlist_modifyed )
		{
			m_iTriangles[iVertexIndex] = m_iVertCount;
			m_iVertCount++;
		}
	}
}

void CXSIMesh::GetUVData( int iVertexIndex, float& fU, float& fV )
{
	fU = m_cvVertecies[0][m_iTriangles[iVertexIndex]].u;
	fV = m_cvVertecies[0][m_iTriangles[iVertexIndex]].v;
}

bool CXSIMesh::VerteciesEqual( CVERTEX& vertex1, CVERTEX& vertex2 )
{
	if( vertex1.x == vertex2.x && vertex1.y == vertex2.y && vertex1.z == vertex2.z )
		return true;
	else
		return false;
}

void CXSIMesh::SwapYZCoords()
{
	int iFrames;
	 
	if( m_pParent->GetType() == FT_XSI )
		iFrames = ((CXSIFile *)m_pParent)->GetFramesCount();

	if( m_pParent->GetType() == FT_EFN_MESHLIST )
		iFrames = 1;

	for( int i = 0; i < iFrames; i++)
	{
		for( int j = 0; j < m_iVertCount; j++)
		{
			float temp					= -m_cvVertecies[i][j].ny;
			m_cvVertecies[i][j].ny		= m_cvVertecies[i][j].nz;
			m_cvVertecies[i][j].nz		= temp;

			temp						= -m_cvVertecies[i][j].y;
			m_cvVertecies[i][j].y		= m_cvVertecies[i][j].z;
			m_cvVertecies[i][j].z		= temp;
		}
	}
}
