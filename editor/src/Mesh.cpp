#include "Mesh.h"
#include "EfnCharacter.h"
#include "EfnMeshlist.h"
#include "MemMappingsMgr.h"

#include <Mmsystem.h>
#include <windows.h>

CMesh::CMesh(void)
{
	m_pOOBBVB			= NULL;
	m_pBsphere			= NULL;
	m_pVBNull			= NULL;
	m_pNormalsCache		= NULL;

	m_strName			= "";

	//primary buffers
	m_iVerteciesCount	= 0;
	m_pVertecies		= NULL;		
	m_iIndicesCount		= 0;
	m_pIndices			= NULL;

	//editing buffers
	m_iVerteciesEdCount		= 0;
	m_pVertecies_Ed_Buff	= NULL;
	m_pVertecies_Move_Buff	= NULL;
	m_iIndicesEdCount		= 0;
	m_pIndicesEd_Buff		= NULL;

	m_iNullsCount		= 0;
	m_pNulls			= NULL;

	m_time				= 0;

	m_bIsSelected		= false;
	m_bIsParentMesh		= false;
	m_bIsVisible		= true;
	m_bIsOOBBVisible	= false;
	m_bIsBSphereVisible	= false;

	m_bIsVertexeditingMode	= false;
	m_pNullSphere			= NULL;
    m_ppReverseIndex		= NULL;
	m_iSelectedNull			= -1;
	m_pAlternateNullIcon	= NULL;
	m_bMaterialsEnabled		= true;


	// setup materials
	ZeroMemory( &m_BlackMtrl, sizeof( m_BlackMtrl ) );
	m_BlackMtrl.Diffuse = D3DXCOLOR( 0xff000000 );
	m_BlackMtrl.Ambient = D3DXCOLOR( 0xff000000 );

	ZeroMemory( &m_WhiteMtrl, sizeof( m_WhiteMtrl ) );
	m_WhiteMtrl.Diffuse = D3DXCOLOR( 0xffffffff );
	m_WhiteMtrl.Ambient = D3DXCOLOR( 0xffffffff );
	
	ZeroMemory( &m_SelectionMtrl, sizeof(m_SelectionMtrl) );
	m_SelectionMtrl.Diffuse = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );
	m_SelectionMtrl.Ambient = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );

	ZeroMemory( &m_BlueMtrl, sizeof(m_BlueMtrl) );
	m_BlueMtrl.Diffuse = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f );
	m_BlueMtrl.Ambient = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f );

	ZeroMemory( &m_GreenMtrl, sizeof( m_GreenMtrl ) );
	m_GreenMtrl.Diffuse = D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f );
	m_GreenMtrl.Ambient = D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f );

	ZeroMemory( &m_YellowMtrl, sizeof( m_YellowMtrl ) );
	m_YellowMtrl.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f );
	m_YellowMtrl.Ambient = D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f );

	m_Material = m_WhiteMtrl;
}

void CMesh::CancelVertexEditingMode()
{
	m_bIsVertexeditingMode = false;

	for( int i = 0; i < 10; i++ )
	{
		Sleep( 10 );
		g_MemMappingsMgr->m_MeshData->tricount = 0;
		g_MemMappingsMgr->PostMessageToEngine( g_MemMappingsMgr->QEM_QUEST_UPDATE_CUSTOM_MESH );
	}

	m_iVerteciesEdCount	= 0;
	m_iIndicesEdCount	= 0;

	SAFE_DELETE_ARRAY( m_pIndicesEd_Buff );
	SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
}

bool CMesh::IsVertexEditingMode()
{
	return m_bIsVertexeditingMode;
}

void CMesh::TranslateWholeSceneHack( D3DXVECTOR3* pTranslate )
{
	for( int i = 0; i < GetNullsCount(); i++ )
	{
		GetNullByIndex(i)->matrices[0][0] += pTranslate->x;
		GetNullByIndex(i)->matrices[0][1] += pTranslate->y;
		GetNullByIndex(i)->matrices[0][2] += pTranslate->z;
	}

	for( int i = 0; i < GetVerteciesCount(); i++ )
	{
		GetVertecies()[i].x += pTranslate->x;
		GetVertecies()[i].y += pTranslate->y;
		GetVertecies()[i].z += pTranslate->z;
	}

	CalcOOBB();
	CalcBoundingSphere();
}

void CMesh::DeleteSelectedVertecies()
{
	int iTris2Remove = 0;
	int iVert2Remove = 0;

	//buff of vertecies to delete and buff of tris to delete
	bool* bVert2Delete	= new bool[m_iVerteciesEdCount];
	bool* bObsoleteTris = new bool[m_iIndicesEdCount / 3];
	ZeroMemory( bVert2Delete, sizeof(bool) * m_iVerteciesEdCount );
	ZeroMemory( bObsoleteTris, sizeof(bool) * m_iIndicesEdCount / 3 );

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
			bVert2Delete[i] = true;

		if( bVert2Delete[i] )
		{
			for( int j = 0; j < m_iIndicesEdCount / 3; j++ )
			{				
				if( m_pIndicesEd_Buff[j * 3 + 0] == i || 
					m_pIndicesEd_Buff[j * 3 + 1] == i || 
					m_pIndicesEd_Buff[j * 3 + 2] == i )
				{
					bObsoleteTris[j] = true;
				}
			}
		}
	}

	//check for obsolete vertecies
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( bVert2Delete[i] == false )
		{
			bool obsolete = true;
			
			for( int j = 0; j < m_iIndicesEdCount / 3; j++ )
			{	
				if( m_pIndicesEd_Buff[j * 3 + 0] == i || 
					m_pIndicesEd_Buff[j * 3 + 1] == i || 
					m_pIndicesEd_Buff[j * 3 + 2] == i )
				{
					if( bObsoleteTris[j] == false )
						obsolete = false;
				}
			}

			if( obsolete )
				bVert2Delete[i] = true;
		}
	}	

	//count tris 2 delete
	for( int j = 0; j < m_iIndicesEdCount / 3; j++ )
	{
		if( bObsoleteTris[j] == true )
			iTris2Remove++;
	}

	//count vert 2 delete
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( bVert2Delete[i] )
			iVert2Remove++;
	}

	//no vert selected
	if( iTris2Remove == 0 )
	{
		SAFE_DELETE_ARRAY( bVert2Delete );
		SAFE_DELETE_ARRAY( bObsoleteTris );
		return;
	}

	//check amount of remaining tris
	if( iTris2Remove == m_iIndicesEdCount / 3 )
	{
		AfxMessageBox( "Нельзя удалить выделенные вершины, в объекте не останется полиногов." );
		SAFE_DELETE_ARRAY( bVert2Delete );
		SAFE_DELETE_ARRAY( bObsoleteTris );
		return;
	}


	//buffer of vertex indecies changes
	int* iBuff = new int[m_iVerteciesEdCount];
	
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
		iBuff[i] = -1;

	int iCounter = 0;

	//fill buffer of vert changes
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( bVert2Delete[i] == false )
		{
			iBuff[i] = iCounter;
			iCounter++;
		}
	}

	//rebuild index
	int* pNewIndex = new int[m_iIndicesEdCount - iTris2Remove * 3];

	iCounter = 0;

	for( int j = 0; j < m_iIndicesEdCount / 3; j++ )
	{
		if( bObsoleteTris[j] == false )
		{
			pNewIndex[iCounter * 3 + 0] = m_pIndicesEd_Buff[j * 3 + 0];
			pNewIndex[iCounter * 3 + 1] = m_pIndicesEd_Buff[j * 3 + 1];
			pNewIndex[iCounter * 3 + 2] = m_pIndicesEd_Buff[j * 3 + 2];
			iCounter++;
		}
	}

	for( int i = 0; i < m_iIndicesEdCount - iTris2Remove * 3; i++ )
	{
		if( iBuff[pNewIndex[i]] != -1 )
			pNewIndex[i] = iBuff[pNewIndex[i]];
	}

	//+++
	//SAFE_DELETE_ARRAY( pNewIndex );
	//+++

	SAFE_DELETE_ARRAY( m_pIndicesEd_Buff );
	m_iIndicesEdCount = iCounter * 3;
	m_pIndicesEd_Buff = pNewIndex;

	int newVertCount = m_iVerteciesEdCount - iVert2Remove;

	//rebuild vertecies
	CVERTEX* pNewVert = new CVERTEX[newVertCount];

	iCounter = 0;
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( bVert2Delete[i] == false ) 
		{
			pNewVert[iCounter] = m_pVertecies_Ed_Buff[i];
			iCounter++;
		}
	}

	//+++
	//SAFE_DELETE_ARRAY( pNewVert );
	//+++

	SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
	m_pVertecies_Ed_Buff	= pNewVert;
	m_iVerteciesEdCount		= newVertCount;

	SAFE_DELETE_ARRAY( m_pVertecies_Move_Buff );
	m_pVertecies_Move_Buff = new CVERTEX[newVertCount];
	memcpy( m_pVertecies_Move_Buff, pNewVert, sizeof( CVERTEX ) * newVertCount );

	//update shared mesh
	UpdateEngineTempMesh( true );

	//free buffers
	SAFE_DELETE_ARRAY( iBuff );
	SAFE_DELETE_ARRAY( bVert2Delete );
	SAFE_DELETE_ARRAY( bObsoleteTris );
}

void CMesh::SetVertexEditingMode( bool bValue )  
{
	m_bIsVertexeditingMode = bValue;

	if( bValue )
	{
		//create temp buffers for editing procedures
		m_iVerteciesEdCount	= m_iVerteciesCount;
		m_iIndicesEdCount	= m_iIndicesCount;

		SAFE_DELETE_ARRAY( m_pIndicesEd_Buff );
		m_pIndicesEd_Buff = new int[m_iIndicesCount];

		if( !m_pIndicesEd_Buff )
			throw "Memory allocation error!";

		SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
		m_pVertecies_Ed_Buff = new CVERTEX[m_iVerteciesCount];

		SAFE_DELETE_ARRAY( m_pVertecies_Move_Buff );
		m_pVertecies_Move_Buff = new CVERTEX[m_iVerteciesCount];

		if( !m_pVertecies_Ed_Buff )
			throw "Memory allocation error!";

		memcpy( m_pVertecies_Ed_Buff, m_pVertecies, sizeof( CVERTEX ) * m_iVerteciesCount );
		memcpy( m_pVertecies_Move_Buff, m_pVertecies, sizeof( CVERTEX ) * m_iVerteciesCount );
		memcpy( m_pIndicesEd_Buff, m_pIndices, sizeof( int ) * m_iIndicesCount );

		for( int i = 0; i < m_iVerteciesEdCount; i++ )
			m_pVertecies_Ed_Buff[i].color = 0xff0000ff;

		UpdateEngineTempMesh( true );
		//sprintf( g_MemMappingsMgr->m_MeshData->name, "%s", m_strName.data() );
	}
	else
	{
		g_MemMappingsMgr->m_MeshData->tricount = 0;

		//reinitialize primary buffers from editing buffers
		long old_col = m_pVertecies[0].color;

		KillReverseIndex();

		SAFE_DELETE_ARRAY( m_pVertecies );

		m_pVertecies = new CVERTEX[m_iVerteciesEdCount];
		memcpy( m_pVertecies, m_pVertecies_Ed_Buff, sizeof( CVERTEX ) * m_iVerteciesEdCount );
		m_iVerteciesCount = m_iVerteciesEdCount;

		if( !m_pVertecies )
			throw "Memory allocation error!";
		
		SAFE_DELETE_ARRAY( m_pIndices );

		m_pIndices = new int[m_iIndicesEdCount];
		memcpy( m_pIndices, m_pIndicesEd_Buff, sizeof( int ) * m_iIndicesEdCount );
		m_iIndicesCount = m_iIndicesEdCount;

		if( !m_pIndices )
			throw "Memory allocation error!";

		CalcReverseIndex();
		PrecalculateNormals();
		CalcOOBB();
		CalcBoundingSphere();

		for( int i = 0; i < 10; i++ )
		{
			Sleep( 10 );
			g_MemMappingsMgr->m_MeshData->tricount = 0;
			g_MemMappingsMgr->PostMessageToEngine( g_MemMappingsMgr->QEM_QUEST_UPDATE_CUSTOM_MESH );
		}
	}
}

void CMesh::PrecalculateNormals()
{
	if( m_iIndicesCount == 0 || m_iVerteciesCount == 0 )
		return;

	int				vert1, vert2, vert3, iTriCount = m_iIndicesCount / 3;
	D3DXVECTOR3*	vTriNormals;
	D3DXVECTOR3		vA, vB, vC, vTmp1, vTmp2;

	//----------------------------- polygon normals calculations ---------------------------//
	//allocate memory
	vTriNormals = new D3DXVECTOR3[iTriCount];

	for( int i = 0; i < iTriCount; i++ )
	{
		vert1 = m_pIndices[i * 3 + 0];
		vert2 = m_pIndices[i * 3 + 1];
		vert3 = m_pIndices[i * 3 + 2];

		vA = D3DXVECTOR3( m_pVertecies[vert1].x, m_pVertecies[vert1].y, m_pVertecies[vert1].z );
		vB = D3DXVECTOR3( m_pVertecies[vert2].x, m_pVertecies[vert2].y, m_pVertecies[vert2].z );
		vC = D3DXVECTOR3( m_pVertecies[vert3].x, m_pVertecies[vert3].y, m_pVertecies[vert3].z );

		vTmp1 = vB - vA;
		vTmp2 = vC - vA;

		D3DXVec3Cross( &vTriNormals[i], &vTmp1, &vTmp2 );

		D3DXVec3Normalize( &vTriNormals[i], &vTriNormals[i] );
		//D3DXVec3Scale( &vTriNormals[i], &vTriNormals[i], -1.0f );
	}
	//------------------------END-- polygon normals calculations ---------------------------//


	//----------------------------- vertex normals calculations ----------------------------//
	int count = 0;

	//summ up normals using cache
	for( int i = 0; i < m_iVerteciesCount; i++ )
	{
		count = 0;
		vTmp1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		for( int m = 0; m < m_ppReverseIndex[i][0]; m++ )
		{
			int tri_index = m_ppReverseIndex[i][m + 1];

			vTmp1 += vTriNormals[ tri_index ];
			count++;
		}

		//calc average vector
		vTmp1 *= 1.0f / count;
		D3DXVec3Normalize( &vTmp1, &vTmp1 );
		//D3DXVec3Scale( &vTmp1, &vTmp1, -1.0f );

		m_pVertecies[i].nx = vTmp1.x;
		m_pVertecies[i].ny = vTmp1.y;
		m_pVertecies[i].nz = vTmp1.z;
	}
	//----------------------------- vertex normals calculations ----------------------------//

	SAFE_DELETE_ARRAY( vTriNormals );
}

void CMesh::Kill()
{
	SAFE_RELEASE( m_pOOBBVB );
	SAFE_RELEASE( m_pBsphere );
	SAFE_RELEASE( m_pVBNull );

	SAFE_DELETE_ARRAY( m_pIndices );
	SAFE_DELETE_ARRAY( m_pVertecies );
	SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
	SAFE_DELETE_ARRAY( m_pVertecies_Move_Buff );
	SAFE_DELETE_ARRAY( m_pIndicesEd_Buff );
	SAFE_DELETE_ARRAY( m_pNormalsCache );

	KillNulls();
	KillReverseIndex();

	SAFE_RELEASE( m_pNullSphere );
}

void CMesh::KillReverseIndex()
{
	if( m_ppReverseIndex )
    {
        for( int i = 0; i < m_iVerteciesCount; i++ )
        {
            SAFE_DELETE_ARRAY( m_ppReverseIndex[i] );
        }

        SAFE_DELETE_ARRAY( m_ppReverseIndex );
    }
}

void CMesh::SetSelectedNull( str_t strNullID )
{
	for( int i = 0; i < m_iNullsCount; i++ )
	{
		if( m_pNulls[i].id == strNullID )
		{
			m_iSelectedNull = i;
			break;
		}
	}
}

CNULL* CMesh::GetSelectedNull()
{
	if( m_iSelectedNull == -1 )
		return NULL;

	return &m_pNulls[m_iSelectedNull];
}

void CMesh::CalcReverseIndex()
{
	KillReverseIndex();

	///////////////////////////////////////////////
	// create triangles index cache //
	///////////////////////////////////////////////
	// cache structure - 3d array of ints
	// [] - index of vertex
	// [] - list of triangles consisting this spesific vertex, [0] consits num of items of this level
	int	iTriCount	= m_iIndicesCount / 3;
	int	iVertCount	= m_iVerteciesCount;

	if( iTriCount == 0 || iVertCount == 0 )
		return;

	//build cache
	m_ppReverseIndex = new int*[iVertCount];

	//go throught all the vertices, find out which tris belong to each of the vertices
	for( int i = 0; i < iVertCount; i++ )
	{
		vector<int>			temp;
		vector<int>			tempStitch;
		vector<CVERTEX_XYZ> tempNorm;

		//run through tri index
		for( int j = 0; j < iTriCount; j++ )
		{
			if( (m_pIndices[j * 3 + 0] == i) || (m_pIndices[j * 3 + 1] == i) || (m_pIndices[j * 3 + 2] == i) )
			{
				bool found = false;
						
				CVERTEX_XYZ vC, vB, normal;

				vC.x = m_pVertecies[m_pIndices[j * 3 + 2]].x - m_pVertecies[m_pIndices[j * 3 + 0]].x;
				vC.y = m_pVertecies[m_pIndices[j * 3 + 2]].y - m_pVertecies[m_pIndices[j * 3 + 0]].y;
				vC.z = m_pVertecies[m_pIndices[j * 3 + 2]].z - m_pVertecies[m_pIndices[j * 3 + 0]].z;

				vB.x = m_pVertecies[m_pIndices[j * 3 + 1]].x - m_pVertecies[m_pIndices[j * 3 + 0]].x;
				vB.y = m_pVertecies[m_pIndices[j * 3 + 1]].y - m_pVertecies[m_pIndices[j * 3 + 0]].y;
				vB.z = m_pVertecies[m_pIndices[j * 3 + 1]].z - m_pVertecies[m_pIndices[j * 3 + 0]].z;

				normal.x = vB.y * vC.z - vB.z * vC.y;
				normal.y = vB.z * vC.x - vB.x * vC.z;
				normal.z = vB.x * vC.y - vB.y * vC.x;

				float len = 1.0f / sqrtf((float)(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z));

				normal.x *= len;
				normal.y *= len;
				normal.z *= len;

				if( !tempNorm.empty() )
				{
					for( UINT g = 0; g < tempNorm.size(); g++ )
					{
						if( tempNorm[g] ^ normal )
						{
							found = true;
							break;
						}
					}
				}

				if( !found )
				{
					temp.push_back( j );
					tempNorm.push_back( normal );
				}
			}
		}

		tempNorm.clear();

		if( m_iIndicesCount != m_iVerteciesCount )
		{
			for( int j = 0; j < iTriCount; j++ )
			{
				if( ( m_pVertecies[m_pIndices[j * 3 + 0]] |= m_pVertecies[i] ) ||
					( m_pVertecies[m_pIndices[j * 3 + 1]] |= m_pVertecies[i] ) ||
					( m_pVertecies[m_pIndices[j * 3 + 2]] |= m_pVertecies[i] ) )
				{
					for( int k = 0; k < iTriCount; k++ )
					{
						if( ( m_pVertecies[m_pIndices[k * 3 + 0]] %= m_pVertecies[i] ) || 
							( m_pVertecies[m_pIndices[k * 3 + 1]] %= m_pVertecies[i] ) ||
							( m_pVertecies[m_pIndices[k * 3 + 2]] %= m_pVertecies[i] ) )
						{
							bool found = false;
									
							CVERTEX_XYZ vC, vB, normal;

							vC.x = m_pVertecies[m_pIndices[k * 3 + 2]].x - m_pVertecies[m_pIndices[k * 3 + 0]].x;
							vC.y = m_pVertecies[m_pIndices[k * 3 + 2]].y - m_pVertecies[m_pIndices[k * 3 + 0]].y;
							vC.z = m_pVertecies[m_pIndices[k * 3 + 2]].z - m_pVertecies[m_pIndices[k * 3 + 0]].z;

							vB.x = m_pVertecies[m_pIndices[k * 3 + 1]].x - m_pVertecies[m_pIndices[k * 3 + 0]].x;
							vB.y = m_pVertecies[m_pIndices[k * 3 + 1]].y - m_pVertecies[m_pIndices[k * 3 + 0]].y;
							vB.z = m_pVertecies[m_pIndices[k * 3 + 1]].z - m_pVertecies[m_pIndices[k * 3 + 0]].z;

							normal.x = vB.y * vC.z - vB.z * vC.y;
							normal.y = vB.z * vC.x - vB.x * vC.z;
							normal.z = vB.x * vC.y - vB.y * vC.x;

							float len = 1.0f / sqrtf((float)(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z));

							normal.x *= len;
							normal.y *= len;
							normal.z *= len;

							if( !tempNorm.empty() )
							{
								for( UINT g = 0; g < tempNorm.size(); g++ )
								{
									if( tempNorm[g] ^ normal )
									{
										found = true;
										break;
									}
								}
							}

							if( !found )
							{
								tempStitch.push_back( k );
								tempNorm.push_back( normal );
							}
						}
					}

					break;
				}
			}
		}

		if( tempStitch.size() > 0 )
			temp.assign( tempStitch.begin(), tempStitch.end() );

		//create list of tris consisting vertex
		m_ppReverseIndex[i] = new int[temp.size() + 1];
		m_ppReverseIndex[i][0] = temp.size();

		for( UINT j = 0; j < temp.size(); j++ )
			m_ppReverseIndex[i][j + 1] = temp[j];
	}
}

bool CMesh::PrepareNulls( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_iNullsCount <= 0 )
		return true;

	VOID*		pVertices;
	CVERTEX*	lines_vertecies	= new CVERTEX[6];

	//prepare lines for drawing Nulls
	lines_vertecies[0].x = -1.0;
	lines_vertecies[0].y =	0.0;
	lines_vertecies[0].z = 0.0;

	lines_vertecies[1].x = 1.0;
	lines_vertecies[1].y =	0.0;
	lines_vertecies[1].z = 0.0;

	lines_vertecies[2].x = 0.0;
	lines_vertecies[2].y =	-1.0;
	lines_vertecies[2].z = 0.0;

	lines_vertecies[3].x = 0.0;
	lines_vertecies[3].y =	1.0;
	lines_vertecies[3].z = 0.0;

	lines_vertecies[4].x = 0.0;
	lines_vertecies[4].y =	0.0;
	lines_vertecies[4].z = -1.0;

	lines_vertecies[5].x = 0.0;
	lines_vertecies[5].y =	0.0;
	lines_vertecies[5].z = 1.0;

	long size = sizeof( CVERTEX ) * 6;

	pd3dDevice->CreateVertexBuffer( size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBNull, NULL );
	m_pVBNull->Lock( 0, size, (void**)&pVertices, 0 );
	memcpy( pVertices, lines_vertecies, size );
	m_pVBNull->Unlock();

	SAFE_DELETE_ARRAY( lines_vertecies );

	return true;
}

bool CMesh::SaveToFile( ofstream& file )
{
	PrecalculateNormals();
	CalcOOBB();
	CalcBoundingSphere();

	WR2Strm( file, m_strName );

	WR2Strm( file, m_iVerteciesCount );
	file.write( (const char *)m_pVertecies, sizeof(CVERTEX) * m_iVerteciesCount );
	
	WR2Strm( file, m_iIndicesCount );
	file.write( (const char *)m_pIndices, sizeof(int) * m_iIndicesCount );
	
	WR2Strm( file, m_BSphere.radius );
	WR2Strm( file, m_BSphere.pos );
	
	file.write( (const char *)m_OOBB, sizeof(D3DXVECTOR3) * 8 );

	WR2Strm( file, m_iNullsCount );

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		ClampAngle( m_pNulls[i].matrices[0][3] );
		ClampAngle( m_pNulls[i].matrices[0][4] );
		ClampAngle( m_pNulls[i].matrices[0][5] );


		WR2Strm( file, m_pNulls[i].id );
		file.write( (const char *)m_pNulls[i].matrices[0], sizeof(float) * 9 );
	}

    //store reverse index
	for( int i = 0; i < m_iVerteciesCount; i++ )
	{
		WR2Strm( file, m_ppReverseIndex[i][0] + 1 ); // store tris amount
		file.write( (const char *)m_ppReverseIndex[i], sizeof(int) * (m_ppReverseIndex[i][0] + 1) ); //store tris list
	}

	//store material
	file.write( (const char *)&m_Material, sizeof( D3DMATERIAL9 ) );

	return true;
}

void CMesh::LoadMaterialFromFile( ifstream& file )
{
	file.read( (char *)&m_Material, sizeof( D3DMATERIAL9 ) );

	if( m_strName == "Nulls" )
		m_Material = m_BlackMtrl;
}

void CMesh::SetMaterial( D3DMATERIAL9 material )
{
	m_Material = material;
}

D3DMATERIAL9 CMesh::GetMaterial()
{
	return m_Material;
}

void CMesh::LoadFromFile( ifstream& file )
{
	ReadStrm( file, &m_strName );

	/***************************************** load vertecies and indices *******************/
	ReadStrm( file, m_iVerteciesCount );

	if( m_iVerteciesCount > 0 )
	{
		m_pVertecies			= new CVERTEX[m_iVerteciesCount];
		m_pVertecies_Ed_Buff	= new CVERTEX[m_iVerteciesCount];
		m_pVertecies_Move_Buff	= new CVERTEX[m_iVerteciesCount];
		m_pIndicesEd_Buff		= new int[m_iIndicesCount];

		file.read( (char *)m_pVertecies, sizeof(CVERTEX) * m_iVerteciesCount );
	}

	ReadStrm( file, m_iIndicesCount );

	if( m_iIndicesCount > 0 )
	{
		m_pIndices = new int[m_iIndicesCount];
		file.read( (char *)m_pIndices, sizeof(int) * m_iIndicesCount );
	}
	/*************************************END* load vertecies and indices *******************/


	/***************************************** load B volumes *******************************/
	ReadStrm( file, m_BSphere.radius );
	ReadStrm( file, m_BSphere.pos );
	
	file.read( (char *)m_OOBB, sizeof(D3DXVECTOR3) * 8 );
	/*************************************END* load B volumes *******************************/


	/***************************************** load NULLS ***********************************/
	ReadStrm( file, m_iNullsCount );

	if( m_iNullsCount > 0 )
	{
		m_pNulls = new CNULL[m_iNullsCount];

		for( int i = 0; i < m_iNullsCount; i++ )
		{
			ReadStrm( file, &m_pNulls[i].id );
			m_pNulls[i].matrices = new float*[1];
			m_pNulls[i].matrices[0] = new float[9];
			file.read( (char *)m_pNulls[i].matrices[0], sizeof(float) * 9 );

			m_pNulls[i].attachment = NULL;
		}
	}
	/**************************************END* load NULLS ***********************************/


    /************************************ load reverse index *********************************/
	if( m_iVerteciesCount > 0 )
	{
		m_ppReverseIndex = new int*[m_iVerteciesCount];

		for( int i = 0; i < m_iVerteciesCount; i++ )
		{
			int temp_rv;

			// read tris amount
			file.read((char *)&temp_rv, sizeof(int));
			
			m_ppReverseIndex[i] = new int[temp_rv];

			// read tris list
			file.read((char *)m_ppReverseIndex[i], sizeof(int) * temp_rv);
		}
	}
	/********************************END* load reverse index *********************************/
}

bool CMesh::InitFromXSIMesh( CXSIMesh* xsi_mesh )
{
	Kill();

	if( !xsi_mesh )
		return false;

	//name
	m_strName = xsi_mesh->GetName();

	if( xsi_mesh->GetTrianglesCount() < 1 || xsi_mesh->GetVerteciesCount() < 3 )
		return true;

	//indices
	m_iIndicesCount	= xsi_mesh->GetTrianglesCount() * 3;
	m_pIndices		= new int[m_iIndicesCount];

	for( int i = 0; i < m_iIndicesCount; i++ )
		m_pIndices[i] = xsi_mesh->GetTrianglesList()[i];


	//vertecies
	m_iVerteciesCount		= xsi_mesh->GetVerteciesCount();
	m_pVertecies			= new CVERTEX[m_iVerteciesCount];
	m_pVertecies_Ed_Buff	= new CVERTEX[m_iVerteciesCount];
	m_pVertecies_Move_Buff	= new CVERTEX[m_iVerteciesCount];
	m_pIndicesEd_Buff		= new int[m_iIndicesCount];

	for( int i = 0; i < m_iVerteciesCount; i++ )
		m_pVertecies[i] = xsi_mesh->GetVerteciesList()[0][i];

	m_pNulls			= NULL;
	m_iNullsCount		= 0;

	/*xsi_mesh->GetColor( m_Material.Diffuse );
	xsi_mesh->GetColor( m_Material.Ambient );

	m_Material.Ambient.a = 1.0f;
	m_Material.Ambient.r = 1.0f;
	m_Material.Ambient.g = 1.0f;
	m_Material.Ambient.b = 1.0f;*/

	CalcOOBB();
	CalcBoundingSphere();
    CalcReverseIndex();

	return true;	
}

bool CMesh::Prepare( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !pd3dDevice )
		return false;

	if( m_iIndicesCount < 3 || m_iVerteciesCount < 3 )
	{
		PrepareNulls( pd3dDevice );
		return true;
	}

	PrecalculateNormals();

	VOID* pVertices;

	/* prepare OOBB buffer */
	SAFE_RELEASE( m_pOOBBVB );

	long	sz			= 24 * sizeof( CVERTEX );
	int		lines_idx[]	= { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };

	CVERTEX* lines_data  = new CVERTEX[24];

	//create lines
	for( int i = 0; i < 24; i++ )
	{
		lines_data[i].x = m_OOBB[ lines_idx[i] ].x;
		lines_data[i].y = m_OOBB[ lines_idx[i] ].y;
		lines_data[i].z = m_OOBB[ lines_idx[i] ].z;

		lines_data[i].color = 0xffffff00;
	}

	if( FAILED( pd3dDevice->CreateVertexBuffer( sz, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pOOBBVB, NULL ) ) )
		return false;

	pVertices = NULL;
		
	//lock buffer
	if( FAILED( m_pOOBBVB->Lock( 0, sz, (void**)&pVertices, 0 ) ) )
		return false;

	memcpy( (D3DXVECTOR3 *)pVertices, lines_data, sz );

	m_pOOBBVB->Unlock();

	SAFE_DELETE_ARRAY( lines_data );
		
	/* prepare BSphere */
	SAFE_RELEASE( m_pBsphere );
	D3DXCreateSphere( pd3dDevice, m_BSphere.radius, 10, 10, &m_pBsphere, NULL );

	PrepareNulls( pd3dDevice );


	if( m_pNormalsCache )
		SAFE_DELETE_ARRAY( m_pNormalsCache );

	//calc normals cache
	m_pNormalsCache = new CVERTEX_XYZ[m_iVerteciesCount * 2];

	for( int j = 0; j < m_iVerteciesCount; j++ )
	{
		CVERTEX_XYZ vecTmpPos;
		vecTmpPos.x = m_pVertecies[j].x;
		vecTmpPos.y = m_pVertecies[j].y;
		vecTmpPos.z = m_pVertecies[j].z;
		
		CVERTEX_XYZ vecTmpNorm;
		vecTmpNorm.x = m_pVertecies[j].nx;
		vecTmpNorm.y = m_pVertecies[j].ny;
		vecTmpNorm.z = m_pVertecies[j].nz;

		vecTmpNorm.x *= m_BSphere.radius * 0.15f;
		vecTmpNorm.y *= m_BSphere.radius * 0.15f;
		vecTmpNorm.z *= m_BSphere.radius * 0.15f;

		vecTmpNorm.x += vecTmpPos.x;
		vecTmpNorm.y += vecTmpPos.y;
		vecTmpNorm.z += vecTmpPos.z;

		m_pNormalsCache[j * 2]		= vecTmpPos;
		m_pNormalsCache[j * 2 + 1]	= vecTmpNorm;
	}

	return true;
}

void CMesh::RemoveNull( str_t strNullID )
{
	int idx2rem = 0;

	if( m_iNullsCount < 1 )
		return;

	if( m_iNullsCount == 1)
	{
		KillNulls();
		return;
	}

	//remove null from existing array
	for( int i = 0; i < m_iNullsCount; i++ )
	{
		if( m_pNulls[i].id == strNullID )
		{
			idx2rem = i;
			break;
		}
	}

	CNULL* new_arr = new CNULL[m_iNullsCount - 1];

	for( int i = 0; i < idx2rem; i++ )
	{
		new_arr[i].id			= m_pNulls[i].id;
		new_arr[i].matrices		= new float*[1];
		new_arr[i].matrices[0]	= new float[9];
		new_arr[i].attachment	= m_pNulls[i].attachment;
			
		for( int j = 0; j < 9; j++ )
			new_arr[i].matrices[0][j] = m_pNulls[i].matrices[0][j];
	}

	for( int i = idx2rem + 1; i < m_iNullsCount; i++ )
	{
		new_arr[i - 1].id			= m_pNulls[i].id;
		new_arr[i - 1].matrices		= new float*[1];
		new_arr[i - 1].matrices[0]	= new float[9];
		new_arr[i - 1].attachment	= m_pNulls[i].attachment;
			
		for( int j = 0; j < 9; j++ )
			new_arr[i - 1].matrices[0][j] = m_pNulls[i].matrices[0][j];
	}

	
	if( !m_pNulls )
		return;

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		SAFE_DELETE_ARRAY( m_pNulls[i].matrices[0] );
		SAFE_DELETE_ARRAY( m_pNulls[i].matrices );

		if( i == idx2rem )
			SAFE_DELETE( m_pNulls[i].attachment );
	}

	SAFE_DELETE_ARRAY( m_pNulls );


	m_iNullsCount--;
	m_pNulls = new_arr;
}

void CMesh::RemoveNull( CNULL* pNull )
{
	if( !pNull )
		return;

	RemoveNull( pNull->id );
}

void CMesh::CreateNull( str_t strID, D3DXVECTOR3* pos )
{
	CNULL* pNulls = new CNULL[ m_iNullsCount + 1 ];

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		pNulls[ i ].id			= m_pNulls[ i ].id;
		pNulls[ i ].attachment	= m_pNulls[ i ].attachment;
		pNulls[ i ].matrices	= m_pNulls[ i ].matrices;
	}

	pNulls[ m_iNullsCount ].id			= strID;
	pNulls[ m_iNullsCount ].attachment	= NULL;
	pNulls[ m_iNullsCount ].matrices	= new float*[1];
	pNulls[ m_iNullsCount ].matrices[0]	= new float[9];

	if( pos != NULL )
	{
		pNulls[ m_iNullsCount ].matrices[0][0] = pos->x;
		pNulls[ m_iNullsCount ].matrices[0][1] = pos->y;
		pNulls[ m_iNullsCount ].matrices[0][2] = pos->z;
	}
	else
	{
		pNulls[ m_iNullsCount ].matrices[0][0] = 0.0f;
		pNulls[ m_iNullsCount ].matrices[0][1] = 0.0f;
		pNulls[ m_iNullsCount ].matrices[0][2] = 0.0f;
	}

	pNulls[ m_iNullsCount ].matrices[0][3] = 0.0f;
	pNulls[ m_iNullsCount ].matrices[0][4] = 0.0f;
	pNulls[ m_iNullsCount ].matrices[0][5] = 0.0f;
	pNulls[ m_iNullsCount ].matrices[0][6] = 1.0f;
	pNulls[ m_iNullsCount ].matrices[0][7] = 1.0f;
	pNulls[ m_iNullsCount ].matrices[0][8] = 1.0f;

	SAFE_DELETE_ARRAY( m_pNulls );

	m_pNulls = pNulls;
	m_iNullsCount++;
}

void CMesh::UpdateEngineTempMesh( bool force )
{
	static long time = timeGetTime();

	if( timeGetTime() - time > (UINT)(m_bIsVertexeditingMode ? 33 : 500) || force )
	{
		if( g_MemMappingsMgr->IsExactScene() == false )
			return;

		g_MemMappingsMgr->m_MeshData->tricount	= m_iIndicesEdCount / 3;
		g_MemMappingsMgr->m_MeshData->vertcount	= m_iVerteciesEdCount;

		memcpy( g_MemMappingsMgr->m_MeshData->vertecies, m_pVertecies_Ed_Buff, sizeof( CVERTEX ) * m_iVerteciesEdCount );
		memcpy( g_MemMappingsMgr->m_MeshData->indices, m_pIndicesEd_Buff, sizeof( int ) * m_iIndicesEdCount );

		g_MemMappingsMgr->PostMessageToEngine( g_MemMappingsMgr->QEM_QUEST_UPDATE_CUSTOM_MESH );

		time = timeGetTime();
	}
}

void CMesh::MergePoints()
{
	if( !m_bIsVertexeditingMode )
		return;

	int		iSelectedVertCount = 0;
	CVERTEX vecBase;

	for( int i = 0; i < m_iVerteciesCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
		{
			iSelectedVertCount++;

			if( iSelectedVertCount == 1 )
			{
				vecBase = m_pVertecies_Ed_Buff[i];
			}
			else
			{
				m_pVertecies_Ed_Buff[i].x = vecBase.x;
				m_pVertecies_Ed_Buff[i].y = vecBase.y;
				m_pVertecies_Ed_Buff[i].z = vecBase.z;
			}
		}
	}
}

void CMesh::AlignPoints()
{
	if( !m_bIsVertexeditingMode )
		return;

	int iSelectedVertCount = 0;

	D3DXVECTOR3 baseVertecies[3];
	int			baseIndecies[3];

	for( int i = 0; i < m_iVerteciesCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
		{
			if( iSelectedVertCount < 3 )
			{
				baseVertecies[iSelectedVertCount] = D3DXVECTOR3( m_pVertecies_Ed_Buff[i].x, m_pVertecies_Ed_Buff[i].y, m_pVertecies_Ed_Buff[i].z );
				baseIndecies[iSelectedVertCount] = i;
			}

			iSelectedVertCount++;
		}
	}

	if( iSelectedVertCount != 3 )
		return;

	//create base plane
	CPlane		pl( &baseVertecies[0], &baseVertecies[1], &baseVertecies[2] );
	D3DXVECTOR3	dir( pl.N.x, pl.N.y, pl.N.z );

	//project vertecies on plane
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( i == baseIndecies[0] || 
			i == baseIndecies[1] ||
			i == baseIndecies[2] )
		{
			continue;
		}

		D3DXVECTOR3	vert( m_pVertecies_Ed_Buff[i].x, m_pVertecies_Ed_Buff[i].y, m_pVertecies_Ed_Buff[i].z );
		D3DXVECTOR3	out = pl.RayIntersection( &vert, &dir );

		m_pVertecies_Ed_Buff[i].x = out.x;
		m_pVertecies_Ed_Buff[i].y = out.y;
		m_pVertecies_Ed_Buff[i].z = out.z;
	}
}

void CMesh::MoveSelectedVerteciesXBegin( float x )
{
	if( !m_bIsVertexeditingMode ) 
		return;

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
			m_pVertecies_Ed_Buff[i].x = m_pVertecies_Move_Buff[i].x + x;
	}

	UpdateEngineTempMesh();
}

void CMesh::MoveSelectedVerteciesYBegin( float y )
{
	if( !m_bIsVertexeditingMode ) 
		return;

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
			m_pVertecies_Ed_Buff[i].y = m_pVertecies_Move_Buff[i].y + y;
	}

	UpdateEngineTempMesh();
}

void CMesh::MoveSelectedVerteciesZBegin( float z )
{
	if( !m_bIsVertexeditingMode )
		return;

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
			m_pVertecies_Ed_Buff[i].z = m_pVertecies_Move_Buff[i].z + z;
	}

	UpdateEngineTempMesh();
}

void CMesh::MoveSelectedVerteciesXYBegin( float x, float y )
{
	if( !m_bIsVertexeditingMode ) 
		return;

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
		{
			m_pVertecies_Ed_Buff[i].x = m_pVertecies_Move_Buff[i].x + x;
			m_pVertecies_Ed_Buff[i].y = m_pVertecies_Move_Buff[i].y + y;
		}
	}

	UpdateEngineTempMesh();
}

void CMesh::MoveSelectedVerteciesXZBegin( float x, float z )
{
	if( !m_bIsVertexeditingMode ) 
		return;

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
		{
			m_pVertecies_Ed_Buff[i].x = m_pVertecies_Move_Buff[i].x + x;
			m_pVertecies_Ed_Buff[i].z = m_pVertecies_Move_Buff[i].z + z;
		}
	}

	UpdateEngineTempMesh();
}

void CMesh::MoveSelectedVerteciesYZBegin( float y, float z )
{
	if( !m_bIsVertexeditingMode ) 
		return;

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
		{
			m_pVertecies_Ed_Buff[i].y = m_pVertecies_Move_Buff[i].y + y;
			m_pVertecies_Ed_Buff[i].z = m_pVertecies_Move_Buff[i].z + z;
		}
	}

	UpdateEngineTempMesh();
}

void CMesh::MoveSelectedVerteciesEnd()
{
	memcpy( m_pVertecies_Move_Buff, m_pVertecies_Ed_Buff, sizeof( CVERTEX ) * m_iVerteciesEdCount );
}

void CMesh::InverseNormals()
{
	for( int i = 0; i < m_iIndicesCount / 3; i++ )
	{
		int vTemp = m_pIndices[i * 3 + 0];

		m_pIndices[i * 3 + 0] = m_pIndices[i * 3 + 2];
		m_pIndices[i * 3 + 2] = vTemp;
	}

	CalcReverseIndex();
	PrecalculateNormals();
}

void CMesh::ConnectPolys()
{
	CVERTEX*	pVertecies = NULL;
	vector<int> tmp_index, tmp_index2, tmpVec;

	//count unique vertecies 
	for( int i = 0; i < m_iIndicesCount; i++ )
	{
		tmp_index.push_back( m_pIndices[i] );

		for( int j = 0; j < i; j++ )
		{
			if( ( m_pIndices[i] != m_pIndices[j] ) && ( m_pVertecies[m_pIndices[i]] == m_pVertecies[m_pIndices[j]] ) )
			{
				tmp_index[i] = m_pIndices[j];
				break;
			}
		}
	}

	tmp_index2.assign( tmp_index.begin(), tmp_index.end() );
	RemoveDupesFromVector( &tmp_index2 );

	// check whether model is already connected
	if( tmp_index2.size() == m_iVerteciesCount )
		return;

	// build indexed vertex array, eliminating equal vertecies
	for( int i = 0; i < m_iIndicesCount; i++ )
		tmpVec.push_back( -1 );

	for( int i = 0; i < m_iIndicesCount; i++ )
	{
		for( UINT j = 0; j < tmp_index2.size(); j++ )
		{
			if( tmp_index[i] == tmp_index2[j] && tmpVec[i] == -1 )
				tmpVec[i] = j;
		}
	}
	tmp_index.assign( tmpVec.begin(), tmpVec.end() );


	//kill buffers
	SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
	SAFE_DELETE_ARRAY( m_pVertecies_Move_Buff );

	pVertecies				= new CVERTEX[tmp_index2.size()];
	m_pVertecies_Ed_Buff	= new CVERTEX[tmp_index2.size()];
	m_pVertecies_Move_Buff	= new CVERTEX[tmp_index2.size()];

	//build vertecies buff
	for( UINT i = 0; i < tmp_index2.size(); i++ )
		pVertecies[i] = m_pVertecies[tmp_index2[i]];

	SAFE_DELETE_ARRAY( m_pVertecies );

	m_pVertecies = pVertecies;

	//build indicies buff
	for( int i = 0; i < m_iIndicesCount; i++ )
		m_pIndices[i] = tmp_index[i];

	KillReverseIndex();

	m_iVerteciesCount = tmp_index2.size();

	CalcReverseIndex();
	PrecalculateNormals();

	if( m_pNormalsCache )
		SAFE_DELETE_ARRAY( m_pNormalsCache );

	//calc normals cache
	m_pNormalsCache = new CVERTEX_XYZ[m_iVerteciesCount * 2];

	for( int j = 0; j < m_iVerteciesCount; j++ )
	{
		CVERTEX_XYZ vecTmpPos;
		vecTmpPos.x = m_pVertecies[j].x;
		vecTmpPos.y = m_pVertecies[j].y;
		vecTmpPos.z = m_pVertecies[j].z;
		
		CVERTEX_XYZ vecTmpNorm;
		vecTmpNorm.x = m_pVertecies[j].nx;
		vecTmpNorm.y = m_pVertecies[j].ny;
		vecTmpNorm.z = m_pVertecies[j].nz;

		vecTmpNorm.x *= m_BSphere.radius * 0.15f;
		vecTmpNorm.y *= m_BSphere.radius * 0.15f;
		vecTmpNorm.z *= m_BSphere.radius * 0.15f;

		vecTmpNorm.x += vecTmpPos.x;
		vecTmpNorm.y += vecTmpPos.y;
		vecTmpNorm.z += vecTmpPos.z;

		m_pNormalsCache[j * 2]		= vecTmpPos;
		m_pNormalsCache[j * 2 + 1]	= vecTmpNorm;
	}
}

void CMesh::RenderNormals( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !pd3dDevice || !m_bIsVisible || m_bIsVertexeditingMode )
		return;

	//pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );

	DWORD amb, lighting;
	BOOL light0;
	pd3dDevice->GetRenderState( D3DRS_AMBIENT, &amb );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &lighting );
	pd3dDevice->GetLightEnable( 0, &light0 );

	SetVColorFromMaterial( true, pd3dDevice );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
	pd3dDevice->LightEnable( 0, true );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff0000ff );

	pd3dDevice->SetMaterial( &m_BlueMtrl );
	pd3dDevice->SetFVF( D3DFVF_XYZ );

	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, m_iVerteciesCount, m_pNormalsCache, sizeof(CVERTEX_XYZ) ); 

	//SetVColorFromMaterial( false, pd3dDevice );
	pd3dDevice->LightEnable( 0, light0 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT, amb );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, lighting );
}

void CMesh::DisconnectPolys()
{
	CVERTEX* pVertecies	= NULL;

	pVertecies = new CVERTEX[m_iIndicesCount];

	for( int i = 0; i < m_iIndicesCount; i++ )
	{
		pVertecies[i] = m_pVertecies[m_pIndices[i]];
	}

	SAFE_DELETE_ARRAY( m_pVertecies );
	SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
	SAFE_DELETE_ARRAY( m_pVertecies_Move_Buff );
	SAFE_DELETE_ARRAY( m_pIndicesEd_Buff );

	m_pVertecies_Ed_Buff	= new CVERTEX[m_iIndicesCount];
	m_pVertecies_Move_Buff	= new CVERTEX[m_iIndicesCount];
	m_pVertecies			= pVertecies;

	for( int i = 0; i < m_iIndicesCount; i++ )
		m_pIndices[i] = i;

	KillReverseIndex();

	m_iVerteciesCount = m_iIndicesCount;

	CalcReverseIndex();
	PrecalculateNormals();

	if( m_pNormalsCache )
		SAFE_DELETE_ARRAY( m_pNormalsCache );

	//calc normals cache
	m_pNormalsCache = new CVERTEX_XYZ[m_iVerteciesCount * 2];

	for( int j = 0; j < m_iVerteciesCount; j++ )
	{
		CVERTEX_XYZ vecTmpPos;
		vecTmpPos.x = m_pVertecies[j].x;
		vecTmpPos.y = m_pVertecies[j].y;
		vecTmpPos.z = m_pVertecies[j].z;
		
		CVERTEX_XYZ vecTmpNorm;
		vecTmpNorm.x = m_pVertecies[j].nx;
		vecTmpNorm.y = m_pVertecies[j].ny;
		vecTmpNorm.z = m_pVertecies[j].nz;

		vecTmpNorm.x *= m_BSphere.radius * 0.15f;
		vecTmpNorm.y *= m_BSphere.radius * 0.15f;
		vecTmpNorm.z *= m_BSphere.radius * 0.15f;

		vecTmpNorm.x += vecTmpPos.x;
		vecTmpNorm.y += vecTmpPos.y;
		vecTmpNorm.z += vecTmpPos.z;

		m_pNormalsCache[j * 2]		= vecTmpPos;
		m_pNormalsCache[j * 2 + 1]	= vecTmpNorm;
	}
}

void CMesh::InitMaterialFromVColor()
{
	if( m_iVerteciesCount > 0 )
	{
		m_Material.Diffuse = D3DXCOLOR( m_pVertecies[0].color );

		if( m_Material.Diffuse.a == 0 )
			m_Material.Diffuse.a = 1.0f;

		m_Material.Ambient = D3DXCOLOR( m_pVertecies[0].color );

		if( m_Material.Ambient.a == 0 )
			m_Material.Ambient.a = 1.0f;
	}
}

void CMesh::SelectAllVertecies()
{
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
		m_pVertecies_Ed_Buff[i].color = 0xffff0000;
}

void CMesh::SelectVerteciesByFrustrum(  LPDIRECT3DDEVICE9 pd3dDevice, enum VERTEX_SELECTION_MODE SelectionMode, CPoint* vTopLeft, CPoint* vBottomRight )
{
	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		D3DXVECTOR3	vertex = D3DXVECTOR3( m_pVertecies_Ed_Buff[i].x, m_pVertecies_Ed_Buff[i].y, m_pVertecies_Ed_Buff[i].z );
		D3DXVECTOR3	out;

		D3DVIEWPORT9 viewport;
		D3DXMATRIX world, matProj, m_matView;
		D3DXMatrixIdentity( &world );
		pd3dDevice->GetViewport( &viewport );
		pd3dDevice->GetTransform( D3DTS_VIEW, &m_matView );
		pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
		D3DXVec3Project( &out, &vertex, &viewport, &matProj, &m_matView, &world );

		bool ok = false;

		if( out.x > vTopLeft->x && out.x < vBottomRight->x && out.y > vTopLeft->y && out.y < vBottomRight->y )
			ok = true;

		if( SelectionMode == VSM_DISCARDING )
		{
			if( ok )
				m_pVertecies_Ed_Buff[i].color = 0xffff0000;
			else
				m_pVertecies_Ed_Buff[i].color = 0xff0000ff;
		}
		else if( SelectionMode == VSM_ADDING )
		{
			if( ok )
				m_pVertecies_Ed_Buff[i].color = 0xffff0000;
		}
		else
		{
			if( ok )
				m_pVertecies_Ed_Buff[i].color = 0xff0000ff;
		}
	}
}

int CMesh::SelectNullsByFrustrum( LPDIRECT3DDEVICE9 pd3dDevice, CPoint* vTopLeft, CPoint* vBottomRight )
{
	int idx = -1;

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		D3DXVECTOR3	vertex = D3DXVECTOR3( m_pNulls[i].matrices[0][0], m_pNulls[i].matrices[0][1], m_pNulls[i].matrices[0][2] );
		D3DXVECTOR3	out;

		D3DVIEWPORT9 viewport;
		D3DXMATRIX world, matProj, m_matView;
		D3DXMatrixIdentity( &world );
		pd3dDevice->GetViewport( &viewport );
		pd3dDevice->GetTransform( D3DTS_VIEW, &m_matView );
		pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
		D3DXVec3Project( &out, &vertex, &viewport, &matProj, &m_matView, &world );

		bool ok = false;

		if( vTopLeft->x > vBottomRight->x )
		{
			if( out.x > vBottomRight->x && out.x < vTopLeft->x && out.y > vBottomRight->y && out.y < vTopLeft->y )
				ok = true;
		}
		else
		{
			if( out.x > vTopLeft->x && out.x < vBottomRight->x && out.y > vTopLeft->y && out.y < vBottomRight->y )
				ok = true;
		}

		if( ok )
			idx = i;
	}

	if( idx != -1  )
	{
		DeselectAllNulls();
		m_iSelectedNull = idx;
	}

	return idx;
}

inline void CMesh::SetVColorFromMaterial( bool	bValue, LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( bValue )
	{
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	 	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
	}
	else
	{
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2 );
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
	}
}

void CMesh::RenderMeshData( LPDIRECT3DDEVICE9 pd3dDevice, bool bUseMaterial )
{
	if( m_iVerteciesCount < 3 || m_iIndicesCount < 3 )
		return;

	//shape
	if( m_bIsVertexeditingMode )
	{
		//set states
		pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		SetVColorFromMaterial( true, pd3dDevice );

		pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
		
		if( m_RenderModeEnum == RM_WIREFRAME )
		{				
	        pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
			pd3dDevice->SetMaterial( &m_WhiteMtrl );
		}
		else
		{
	        pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff000000 );
			pd3dDevice->SetMaterial( &m_BlackMtrl );
		}

		// wireframe model
		pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, m_iVerteciesEdCount, m_iIndicesEdCount / 3,
											m_pIndicesEd_Buff, D3DFMT_INDEX32, m_pVertecies_Ed_Buff, sizeof( CVERTEX ) );

		//pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_iIndicesEdCount / 3, m_pVertecies_Ed_Buff, sizeof( CVERTEX ) );


		//set states
		pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
		pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1 );
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1 );


		// vertecies as points
		pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, m_iVerteciesEdCount, m_pVertecies_Ed_Buff, sizeof( CVERTEX ) );
	}
	else
	{
		if( m_bIsSelected )
		{
			pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	     	pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );
			pd3dDevice->SetMaterial( &m_SelectionMtrl );

			pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
			pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, m_iVerteciesCount, m_iIndicesCount / 3, 
												m_pIndices, D3DFMT_INDEX32, m_pVertecies, sizeof( CVERTEX ) );

			pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff000000 );
			pd3dDevice->SetMaterial( &m_BlackMtrl );

			pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
			pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, m_iVerteciesCount, m_iIndicesCount / 3, 
												m_pIndices, D3DFMT_INDEX32, m_pVertecies, sizeof( CVERTEX ) );
		}
		else
		{
			//SetVColorFromMaterial( true, pd3dDevice );

			if( m_bMaterialsEnabled )
			{
				// Enable alpha blending.
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

				// Set the source blend state.
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

				// Set the destination blend state.
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			
				pd3dDevice->SetMaterial( &m_Material );	
			}
			else
			{
				pd3dDevice->SetMaterial( &m_WhiteMtrl );
			}
						
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
			pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, m_iVerteciesCount, m_iIndicesCount / 3, 
											m_pIndices, D3DFMT_INDEX32, m_pVertecies, sizeof( CVERTEX ) );
		}
	}
	
	DWORD dwAlpha;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlpha );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	LPDIRECT3DTEXTURE9 pTexture;
	pd3dDevice->GetTexture( 0, (IDirect3DBaseTexture9**)&pTexture );
	pd3dDevice->SetTexture( 0, NULL );

	//set states
	SetVColorFromMaterial( true, pd3dDevice );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	//draw oobb
	if( m_bIsOOBBVisible && m_pOOBBVB )
	{
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffff00 );
		pd3dDevice->SetMaterial( &m_YellowMtrl );
		
		pd3dDevice->SetStreamSource( 0, m_pOOBBVB, 0, sizeof(CVERTEX) );
		pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
		pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 12 );
	}

	// bsphere
	if( m_bIsBSphereVisible && m_pBsphere )
	{
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff0000ff );
		pd3dDevice->SetMaterial( &m_BlueMtrl );

		D3DXMATRIX tr;
		D3DXMatrixTranslation( &tr, m_BSphere.pos.x,  m_BSphere.pos.y, m_BSphere.pos.z );

		D3DXMATRIX	matr_old_world;
		pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );

		pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( tr * matr_old_world ) );

		m_pBsphere->DrawSubset( 0 );
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlpha );
	pd3dDevice->SetTexture( 0, pTexture );
}

void CMesh::Render( LPDIRECT3DDEVICE9 pd3dDevice, float icon_scale, bool bUseMaterial )
{
	//UpdateEngineTempMesh();

	DWORD		param1;
	DWORD		param3;
	DWORD		param4;
	DWORD		param5;
	D3DXMATRIX	matr_old_world;
	BOOL light0;

	//remember states
	pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &param1 );
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &param3 );
	pd3dDevice->GetRenderState( D3DRS_CULLMODE, &param4 );
	pd3dDevice->GetRenderState( D3DRS_AMBIENT, &param5 );
	pd3dDevice->GetLightEnable( 0, &light0 );
	
	RenderMeshData( pd3dDevice, bUseMaterial );

	/* NULLS */

	// turn off texture
	//pd3dDevice->SetTexture( 0, NULL );

	if( m_iNullsCount > 0 )
	{
		int h = 0;
	}

	if( m_iNullsCount > 0 && m_pVBNull )
	{
		D3DXMATRIX rotX, rotY, rotZ, transl, scale, res;

		for( int i = 0; i < m_iNullsCount; i++ )
		{
			float** matrices = m_pNulls[i].matrices;

			D3DXMatrixTranslation( &transl, matrices[0][0], matrices[0][1], matrices[0][2] );

			D3DXMatrixRotationX( &rotX,	D3DXToRadian( matrices[0][3] ) );
			D3DXMatrixRotationY( &rotY,	D3DXToRadian( matrices[0][4] ) );
			D3DXMatrixRotationZ( &rotZ,	D3DXToRadian( matrices[0][5] ) );

			if( !m_pNulls[i].attachment )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

				pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				SetVColorFromMaterial( true, pd3dDevice );

				
				//set required material
				if( i == m_iSelectedNull )
				{
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );
					pd3dDevice->SetMaterial( &m_SelectionMtrl );
				}
				else
				{
					pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff00ff00 );
					pd3dDevice->SetMaterial( &m_GreenMtrl );
				}

				//render null icon
				if( !m_pAlternateNullIcon )
				{
					D3DXMatrixScaling( &scale, icon_scale * 6, icon_scale * 6, icon_scale * 6 );
					res = scale * rotX * rotY * rotZ * transl * matr_old_world;
					pd3dDevice->SetTransform( D3DTS_WORLD, &res );
					
					//draw cross
					pd3dDevice->SetStreamSource( 0, m_pVBNull, 0, sizeof(CVERTEX) );
					pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
					pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
				}
				else
				{
					D3DXMatrixScaling( &scale, icon_scale, icon_scale, icon_scale );

					D3DXMatrixRotationX( &rotX,	D3DXToRadian( matrices[0][3] ) );
					D3DXMatrixRotationY( &rotY,	D3DXToRadian( matrices[0][4] ) );
					D3DXMatrixRotationZ( &rotZ,	D3DXToRadian( matrices[0][5] ) );

					res = scale * rotX * rotY * rotZ * transl * matr_old_world;

					pd3dDevice->SetTransform( D3DTS_WORLD, &res );

					pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
					
					((CEfnCharacter*)m_pAlternateNullIcon)->Render( pd3dDevice, false, 0.0f, true );
				} // if( !m_pAlternateNullIcon )
			} 
			//else // if( !m_pNulls[i].attachment )
			//{
			//	pd3dDevice->SetMaterial( &m_Material );

			//	pd3dDevice->SetRenderState( D3DRS_AMBIENT, param5 );
			//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, param1 );
			//	pd3dDevice->SetRenderState( D3DRS_FILLMODE, param3 );
			//	pd3dDevice->SetRenderState( D3DRS_CULLMODE, param4 );

			//	D3DXMatrixScaling( &scale, matrices[0][6], matrices[0][7], matrices[0][8] );

			//	res = scale * rotX * rotY * rotZ * transl * matr_old_world;

			//	pd3dDevice->SetTransform( D3DTS_WORLD, &res );


			//	if( m_pNulls[i].attachment->GetType() == FT_EFN )
			//	{
			//		if( i == m_iSelectedNull )
			//		{
			//			pd3dDevice->SetMaterial( &m_SelectionMtrl );
			//			((CEfnCharacter*)m_pNulls[i].attachment)->Render( pd3dDevice, false, 0.0f, false );
			//		}
			//		else
			//			((CEfnCharacter*)m_pNulls[i].attachment)->Render( pd3dDevice, false, 0.0f );
			//	}
			//	else if( m_pNulls[i].attachment->GetType() == FT_EFN_MESHLIST ) 
			//	{
			//		if( i == m_iSelectedNull )
			//		{
			//			pd3dDevice->SetMaterial( &m_SelectionMtrl );
			//			((CEfnMeshlist*)m_pNulls[i].attachment)->Render( pd3dDevice, false );
			//		}
			//		else
			//			m_pNulls[i].attachment->Render( pd3dDevice );
			//	}
			//	else
			//	{}
			//}
		}
	}

	if( bUseMaterial )
		pd3dDevice->SetMaterial( &m_Material );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT, param5 );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, param1 );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, param3 );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, param4 );

	pd3dDevice->LightEnable( 0, light0 );

	pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );
}

void CMesh::SetVColor( DWORD dwColor )
{
	for( int i = 0; i < m_iVerteciesCount; i++ )
		m_pVertecies[i].color = dwColor;
}

bool CMesh::InitFromCMesh( CMesh* mesh )
{
	Kill();

	m_strName			= mesh->GetName();
	m_iVerteciesCount	= mesh->GetVerteciesCount();
	m_iIndicesCount		= mesh->GetIndicesCount();

	if( m_iVerteciesCount > 0 )
	{
		m_pVertecies			= new CVERTEX[m_iVerteciesCount];
		m_pVertecies_Ed_Buff	= new CVERTEX[m_iVerteciesCount];
		m_pVertecies_Move_Buff	= new CVERTEX[m_iVerteciesCount];

		memcpy( m_pVertecies, mesh->GetVertecies(), sizeof(CVERTEX) * m_iVerteciesCount );
	}

	if( m_iIndicesCount > 0 )
	{
		m_pIndices = new int[m_iIndicesCount];
		m_pIndicesEd_Buff = new int[m_iIndicesCount];
		memcpy( m_pIndices, mesh->GetIndices(), sizeof(int) * m_iIndicesCount );
	}

	m_BSphere.pos		= mesh->GetBSphere()->pos;
	m_BSphere.radius 	= mesh->GetBSphere()->radius;

	memcpy( m_OOBB, mesh->GetOOBB(), sizeof(D3DXVECTOR3) * 8 );

	m_bIsVisible		= mesh->GetVisible();
	m_bIsOOBBVisible	= mesh->GetOOBBVisible();

	m_iNullsCount		= mesh->GetNullsCount();

	if( m_iNullsCount > 0 )
		m_pNulls		= new CNULL[ m_iNullsCount ];
	
	m_bMaterialsEnabled = mesh->IsMaterialEnabled();


	CalcReverseIndex();

	//m_ppReverseIndex	= new int*[m_iVerteciesCount];

	//for( int i = 0; i < m_iVerteciesCount; i++ )
	//{
	//	m_ppReverseIndex[i] = new int[ mesh->GetReverseIndex()[i][0] + 1 ];
	//	memcpy( m_ppReverseIndex[i], mesh->GetReverseIndex()[i], sizeof(int) * ( mesh->GetReverseIndex()[i][0] + 1 ) );
	//}

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		m_pNulls[i].id			= mesh->m_pNulls[i].id;
		m_pNulls[i].matrices	= new float*[1];
		m_pNulls[i].matrices[0]	= new float[9];
		m_pNulls[i].attachment	= mesh->m_pNulls[i].attachment;

		for( int j = 0; j < 9; j++ )
			m_pNulls[i].matrices[0][j] = mesh->m_pNulls[i].matrices[0][j];
	}

	m_Material = mesh->GetMaterial();

	m_bIsOOBBVisible	= mesh->GetOOBBVisible();
	m_bIsBSphereVisible	= mesh->GetBSphereVisible();

	return true;
}

void CMesh::CalcBoundingSphere()
{
	CVector		one;
	CVector		two, two_temp;
	CVector		temp2;
	D3DXVECTOR3 center, one_temp, temp;
	float		length = 0.0f;

		// let's find center of the model from aabb 
		center.x = ( m_OOBB[1].x - m_OOBB[0].x) / 2.0f + m_OOBB[0].x;
		center.y = ( m_OOBB[0].y - m_OOBB[4].y) / 2.0f + m_OOBB[4].y;
		center.z = ( m_OOBB[0].z - m_OOBB[3].z) / 2.0f + m_OOBB[3].z;

		for( int i = 0; i < m_iVerteciesCount; i++)
		{
			one_temp.x = m_pVertecies[i].x;
			one_temp.y = m_pVertecies[i].y;
			one_temp.z = m_pVertecies[i].z;

			temp = center - one_temp;

			if( length < D3DXVec3Length( &temp ) )
				length = D3DXVec3Length( &temp );
		}

		m_BSphere.radius	= length;
		m_BSphere.pos		= center;
}

void CMesh::InitNullsFromXSICharacter( CXSIFile* xsi_file )
{
	if( xsi_file->GetNullsCount() < 1 )
		return;

	KillNulls();
	
	m_Material = m_WhiteMtrl;

	m_iNullsCount = xsi_file->GetNullsCount();

	m_pNulls = new CNULL[m_iNullsCount];

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		m_pNulls[i].id			= xsi_file->GetNullByIndex( i )->id;
		m_pNulls[i].matrices	= new float*[1];
		m_pNulls[i].matrices[0]	= new float[9];
		m_pNulls[i].attachment	= NULL;
		
		m_pNulls[i].matrices[0][0] = xsi_file->GetNullByIndex( i )->matrices[0].pos.x;
		m_pNulls[i].matrices[0][1] = xsi_file->GetNullByIndex( i )->matrices[0].pos.y;
		m_pNulls[i].matrices[0][2] = xsi_file->GetNullByIndex( i )->matrices[0].pos.z;
		m_pNulls[i].matrices[0][3] = xsi_file->GetNullByIndex( i )->matrices[0].rot.x;
		m_pNulls[i].matrices[0][4] = xsi_file->GetNullByIndex( i )->matrices[0].rot.y;
		m_pNulls[i].matrices[0][5] = xsi_file->GetNullByIndex( i )->matrices[0].rot.z;
		m_pNulls[i].matrices[0][6] = xsi_file->GetNullByIndex( i )->matrices[0].scl.x;
		m_pNulls[i].matrices[0][7] = xsi_file->GetNullByIndex( i )->matrices[0].scl.y;
		m_pNulls[i].matrices[0][8] = xsi_file->GetNullByIndex( i )->matrices[0].scl.z;
	}
}

void CMesh::RebuildEditBuffers()
{
	SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
	SAFE_DELETE_ARRAY( m_pVertecies_Move_Buff );

	m_pVertecies_Ed_Buff	= new CVERTEX[m_iVerteciesCount];
	m_pVertecies_Move_Buff	= new CVERTEX[m_iVerteciesCount];
}

bool CMesh::MergeWithCMesh( CMesh* mesh )
{
	//KillReverseIndex();

	CVERTEX*			pVertecies	= new CVERTEX[m_iVerteciesCount + mesh->GetVerteciesCount()];
	int*				pIndices	= new int[m_iIndicesCount + mesh->GetIndicesCount()];
	CVERTEX*			pVerteciesTemp;
	int*				pIndicesTemp;

	// merge vertecies
	memcpy( pVertecies, m_pVertecies, sizeof(CVERTEX) * m_iVerteciesCount );
	pVerteciesTemp = pVertecies;
	pVerteciesTemp += m_iVerteciesCount;
	memcpy( pVerteciesTemp, mesh->GetVertecies(), sizeof(CVERTEX) * mesh->GetVerteciesCount() );
	

	// merge indices
	memcpy( pIndices, m_pIndices, sizeof(int) * m_iIndicesCount );
	pIndicesTemp = pIndices;
	pIndicesTemp += m_iIndicesCount;
	memcpy( pIndicesTemp, mesh->GetIndices(), sizeof(int) * mesh->GetIndicesCount() );

	for( int i = 0; i < mesh->GetIndicesCount(); i++ )
		pIndices[ m_iIndicesCount + i ] += m_iVerteciesCount;

	m_iVerteciesCount += mesh->GetVerteciesCount();
	m_iIndicesCount += mesh->GetIndicesCount();

	SAFE_DELETE_ARRAY( m_pVertecies );
	SAFE_DELETE_ARRAY( m_pIndices );

	m_pVertecies	= pVertecies;
	m_pIndices		= pIndices;

	//CalcReverseIndex();
	//PrecalculateNormals();

	//ConnectPolys();
	
	return false;
}

void CMesh::KillNulls()
{
	if( !m_pNulls )
		return;

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		SAFE_DELETE_ARRAY( m_pNulls[i].matrices[0] );
		SAFE_DELETE_ARRAY( m_pNulls[i].matrices );
		//SAFE_DELETE( m_pNulls[i].attachment );
	}

	SAFE_DELETE_ARRAY( m_pNulls );

	m_iNullsCount = 0;
}

void CMesh::SetVertecies( CVERTEX* pVertecies, int iVerteciesCount )
{
	if( !pVertecies || iVerteciesCount == 0 )
		return;

	SAFE_DELETE_ARRAY( m_pVertecies );
	SAFE_DELETE_ARRAY( m_pVertecies_Ed_Buff );
	SAFE_DELETE_ARRAY( m_pVertecies_Move_Buff );

	m_pVertecies			= new CVERTEX[iVerteciesCount];
	m_pVertecies_Ed_Buff	= new CVERTEX[iVerteciesCount];
	m_pVertecies_Move_Buff	= new CVERTEX[iVerteciesCount];

	memcpy( m_pVertecies, pVertecies, sizeof( CVERTEX ) * iVerteciesCount );

	m_iVerteciesCount = iVerteciesCount;

	for( int i = 0; i < m_iVerteciesCount; i++ )
		m_pVertecies[i].color = 0xffffffff;
}

void CMesh::SetIndices( int* pIndecies, int iIndeciesCount )
{
	SAFE_DELETE_ARRAY( m_pIndices );

	m_pIndices = new int[iIndeciesCount];
	memcpy( m_pIndices, pIndecies, sizeof( int ) * iIndeciesCount );

	m_iIndicesCount = iIndeciesCount;
}

void CMesh::CalcOOBB()
{
	D3DXVECTOR3 min;
	D3DXVECTOR3 max;

	if( m_iVerteciesCount <= 0 )
		return;

	min.x	= m_pVertecies[0].x;
	min.y	= m_pVertecies[0].y;
	min.z	= m_pVertecies[0].z;

	max.x	= m_pVertecies[0].x;
	max.y	= m_pVertecies[0].y;
	max.z	= m_pVertecies[0].z;

	for( int i = 1; i < m_iVerteciesCount; i++ )
	{
		if( min.x > m_pVertecies[i].x )
			min.x = m_pVertecies[i].x;

		if( min.y > m_pVertecies[i].y )
			min.y = m_pVertecies[i].y;

		if( min.z > m_pVertecies[i].z )
			min.z = m_pVertecies[i].z;
	}

	for( int i = 1; i < m_iVerteciesCount; i++ )
	{
		if( max.x < m_pVertecies[i].x )
			max.x = m_pVertecies[i].x;

		if( max.y < m_pVertecies[i].y )
			max.y = m_pVertecies[i].y;

		if( max.z < m_pVertecies[i].z )
			max.z = m_pVertecies[i].z;
	}

	m_OOBB[0] = D3DXVECTOR3( min.x, max.y, max.z);
	m_OOBB[1] = D3DXVECTOR3( max.x, max.y, max.z); //max
	m_OOBB[2] = D3DXVECTOR3( max.x, max.y, min.z);
	m_OOBB[3] = D3DXVECTOR3( min.x, max.y, min.z);
	m_OOBB[4] = D3DXVECTOR3( min.x, min.y, max.z);
	m_OOBB[5] = D3DXVECTOR3( max.x, min.y, max.z);
	m_OOBB[6] = D3DXVECTOR3( max.x, min.y, min.z);
	m_OOBB[7] = D3DXVECTOR3( min.x, min.y, min.z); //min
}

//******** TOP VIEW OF AABB**********//
//
//     ----------max y---------
//
//     Z
//     ^  0----1
//     |  |    |
//     |  |    |
//     |  3----2
//     |
//     +---------->X
//
//     ----------min y---------
//
//     Z
//     ^  4----5
//     |  |    |
//     |  |    |
//     |  7----6
//     |
//     +---------->X
//
//******** END TOP VIEW OF AABB**********//

D3DXVECTOR3 CMesh::GetOOBBCenter()
{
	D3DXVECTOR3 res;

	res = m_OOBB[1] - m_OOBB[7];
	D3DXVec3Scale( &res, &res, 0.5f );

	res += m_OOBB[7];

	return res;
}

bool CMesh::IntersectOOBB( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation )
{
	float min_len = -1.0f;
	float dist[12];

	D3DXVECTOR3		vec0( m_OOBB[0].x, m_OOBB[0].y, m_OOBB[0].z );
	D3DXVECTOR3		vec1( m_OOBB[1].x, m_OOBB[1].y, m_OOBB[1].z );
	D3DXVECTOR3		vec2( m_OOBB[2].x, m_OOBB[2].y, m_OOBB[2].z );
	D3DXVECTOR3		vec3( m_OOBB[3].x, m_OOBB[3].y, m_OOBB[3].z );
	D3DXVECTOR3		vec4( m_OOBB[4].x, m_OOBB[4].y, m_OOBB[4].z );
	D3DXVECTOR3		vec5( m_OOBB[5].x, m_OOBB[5].y, m_OOBB[5].z );
	D3DXVECTOR3		vec6( m_OOBB[6].x, m_OOBB[6].y, m_OOBB[6].z );
	D3DXVECTOR3		vec7( m_OOBB[7].x, m_OOBB[7].y, m_OOBB[7].z );

	D3DXVec3TransformCoord( &vec0, &vec0, &mTransformation );
	D3DXVec3TransformCoord( &vec1, &vec1, &mTransformation );
	D3DXVec3TransformCoord( &vec2, &vec2, &mTransformation );
	D3DXVec3TransformCoord( &vec3, &vec3, &mTransformation );
	D3DXVec3TransformCoord( &vec4, &vec4, &mTransformation );
	D3DXVec3TransformCoord( &vec5, &vec5, &mTransformation );
	D3DXVec3TransformCoord( &vec6, &vec6, &mTransformation );
	D3DXVec3TransformCoord( &vec7, &vec7, &mTransformation );


	// front - back
	dist[0]		= CalcRayTriangleIntersection( &vec0, &vec1, &vec4, &pFrom, &pTo );
	dist[1]		= CalcRayTriangleIntersection( &vec1, &vec5, &vec4, &pFrom, &pTo );
	dist[2]		= CalcRayTriangleIntersection( &vec3, &vec2, &vec7, &pFrom, &pTo );
	dist[3]		= CalcRayTriangleIntersection( &vec2, &vec6, &vec7, &pFrom, &pTo );

	// right - left
	dist[4]		= CalcRayTriangleIntersection( &vec3, &vec0, &vec7, &pFrom, &pTo );
	dist[5]		= CalcRayTriangleIntersection( &vec0, &vec4, &vec7, &pFrom, &pTo );
	dist[6]		= CalcRayTriangleIntersection( &vec1, &vec2, &vec5, &pFrom, &pTo );
	dist[7]		= CalcRayTriangleIntersection( &vec2, &vec6, &vec5, &pFrom, &pTo );

	// top - bottom
	dist[8]		= CalcRayTriangleIntersection( &vec2, &vec1, &vec0, &pFrom, &pTo );
	dist[9]		= CalcRayTriangleIntersection( &vec2, &vec0, &vec3, &pFrom, &pTo );
	dist[10]	= CalcRayTriangleIntersection( &vec6, &vec5, &vec4, &pFrom, &pTo );
	dist[11]	= CalcRayTriangleIntersection( &vec6, &vec4, &vec7, &pFrom, &pTo );


	for( int i = 0; i < 12; i++)
	{
		if( dist[i] != -1.0f )
		{
			min_len = dist[i];
			break;
		}
	}

	for( int i = 0; i < 12; i++)
	{
		if( dist[i] != -1.0f && dist[i] < min_len )
			min_len = dist[i];
	}

	if( min_len != -1.0f )
		return true;

	return false;
}

bool CMesh::IntersectBSphere( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation )
{
	CVector		orig, dir;
	CSPHERE		sphere_res;
	D3DXVECTOR3	sph_pos	= m_BSphere.pos;

	D3DXVec3TransformCoord( &sph_pos, &sph_pos, &mTransformation );

	sphere_res.radius	= m_BSphere.radius;
	sphere_res.pos		= sph_pos;

	float dist = CalcRaySphereIntersection( sphere_res, &pFrom, &pTo );

	if( dist != -1.0f )
		return true;

	return false;
}

float CMesh::IntersectPolys( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation )
{
	float res = 999999.0f;

	for( int i = 0; i < m_iIndicesCount / 3; i++ )
	{
		CVERTEX v1 = m_pVertecies[m_pIndices[i * 3 + 0]];
		CVERTEX v2 = m_pVertecies[m_pIndices[i * 3 + 1]];
		CVERTEX v3 = m_pVertecies[m_pIndices[i * 3 + 2]];

		D3DXVECTOR3	vec1( v1.x, v1.y, v1.z ); 
		D3DXVECTOR3	vec2( v2.x, v2.y, v2.z );
		D3DXVECTOR3	vec3( v3.x, v3.y, v3.z );

		D3DXVec3TransformCoord( &vec1, &vec1, &mTransformation );
		D3DXVec3TransformCoord( &vec2, &vec2, &mTransformation );
		D3DXVec3TransformCoord( &vec3, &vec3, &mTransformation );

		float dist = CalcRayTriangleIntersection( &vec1, &vec2, &vec3, &pFrom, &pTo );

		if( dist != -1.0f )
		{
			if( dist < res )
				res = dist;
		}
	}

	if( res == 999999.0f )
		return -1.0f;

	return res;
}

CNULL* CMesh::GetNullByName( str_t name )
{
	for( int i = 0; i < m_iNullsCount; i++ ) 
	{
		if( m_pNulls[i].id == name ) 
			return &m_pNulls[i];
	};

	return NULL;
}

CNULL* CMesh::GetNullByName2( str_t name )
{
	for( int i = 0; i < m_iNullsCount; i++ ) 
	{
		str_t temp = m_pNulls[i].id;

		if( _2Upper( temp ) == _2Upper( name ) ) 
			return &m_pNulls[i];
	};

	return NULL;
}

int CMesh::GetSelectedVerteciesCount()
{
	int iCount = 0;

	if( !m_bIsVertexeditingMode ) 
		return 0;

	for( int i = 0; i < m_iVerteciesEdCount; i++ )
	{
		if( m_pVertecies_Ed_Buff[i].color == 0xffff0000 )
			iCount++;
	}

	return iCount;
}

void CMesh::AddNull( CNULL* pNull )
{
	if( !pNull )
		return;

	CNULL* new_arr = new CNULL[m_iNullsCount + 1];

	for( int i = 0; i < m_iNullsCount; i++ )
	{
		new_arr[i].id			= m_pNulls[i].id;
		new_arr[i].matrices		= new float*[1];
		new_arr[i].matrices[0]	= new float[9];
		
		for( int j = 0; j < 9; j++ )
			new_arr[i].matrices[0][j] = m_pNulls[i].matrices[0][j];

		new_arr[i].attachment = NULL;
	}

	new_arr[m_iNullsCount].id			= pNull->id;
	new_arr[m_iNullsCount].matrices		= new float*[1];
	new_arr[m_iNullsCount].matrices[0]	= new float[9];
	
	for( int i = 0; i < 9; i++ )
		new_arr[m_iNullsCount].matrices[0][i] = pNull->matrices[0][i];

	new_arr[m_iNullsCount].attachment = NULL;

	int iTemp = m_iNullsCount;

	KillNulls();

	m_iNullsCount = iTemp + 1;
	m_pNulls = new_arr;
}