#pragma warning(disable: 4715 4244)

#include "sys/stdafx.h"
#include "EfnCharacter.h"
#include <iostream>
#include <fstream>
#include "d3dx9mesh.h"
#include "dialogs/CookieMachineDlg.h"

using namespace std;

#define CompressionFactor 65535

bool CEfnCharacter::IsSupportedVersion( DWORD dwVersion )
{
	if( dwVersion == EFN_SUPP_VERSION_01 || dwVersion == EFN_SUPP_VERSION_02 ||
		dwVersion == EFN_SUPP_VERSION_03 ||	dwVersion == EFN_SUPP_VERSION_04 || 
		dwVersion == EFN_SUPP_VERSION_05 || dwVersion == EFN_SUPP_VERSION_06 )
	{
		return true;
	}

	return false;
}

void CEfnCharacter::SetCompressionType( enum EFN_COMPRESSION_TYPE tCompessionType )
{
	m_tCompessionType = tCompessionType;
}

bool CEfnCharacter::LoadFromFile( str_t path )
{
	streampos		pos;
	int				iTemp;
	DWORD			temp;


	m_FileVersion = GetEfnVersion( path );

	if( IsSupportedVersion( m_FileVersion ) == false )
	{
		CString err_str;
		str_t curr_vers_str		= GetEfnVersionAsString( EFN_CURR_VERSION );
		str_t file_vers_str		= GetEfnVersionAsString( m_FileVersion );

		err_str.Format( "File: \"%s\" \n\rUnsupported EFN version: [%s], current version is: [%s] \n\r\n\rFile can not be opened.", path.c_str(), file_vers_str.c_str(), curr_vers_str.c_str() );

		MessageBox( NULL, err_str, "Warning!", MB_OK | MB_ICONEXCLAMATION );

		return false;
	}

	if( !FileExists(path) )
		return false;

	KillData();

	m_cdData = new EFN_CHARACTERDATA;

	if( !m_cdData )
		return false;
	else
		InitData();

	if( m_dlgValidate )
	{
		m_dlgValidate->SetProgressRange( 0, 5 );
		m_dlgValidate->SetProgressMessageStrA( "step 1 of 5" );
		m_dlgValidate->SetProgressMessageStrC( "Geometry..." );

		m_dlgValidate->Step();

		if( m_pParent )
			((CCookieMachineDlg*)m_pParent)->ProcessMessagesGlob();
	}

	ifstream file(path.data(), ios::binary);
	file.seekg(pos);
	
	ReadStrm( file, temp );							// skip version
	ReadStrm( file, iTemp );						// type 
	ReadStrm( file, &m_cdData->name );				// name

	if( m_FileVersion < EFN_VERSION_20 )
		ReadStrm( file, &m_strTexturePath );	// texture

	if( m_FileVersion > EFN_VERSION_22 )
	{
		int iComprType;
		ReadStrm( file, iComprType );

		m_tCompessionType = (enum EFN_COMPRESSION_TYPE)iComprType;
	}

	/************************************ load vertecies list *******************************/
	ReadStrm( file, m_cdData->vertecies_count );

	m_cdData->posList	= new CVERTEX_XYZ[m_cdData->vertecies_count];
	m_cdData->normList	= new CVERTEX_XYZ[m_cdData->vertecies_count];

	if( m_FileVersion > EFN_VERSION_22 )
	{
		ReadStrm( file, m_cdData->anim_frames_count ); // amount of frames

		if( m_cdData->anim_frames_count == 0 )
			m_cdData->vertecies_per_frame_count = 0;
		else
			m_cdData->vertecies_per_frame_count = m_cdData->vertecies_count / m_cdData->anim_frames_count;
	}

	//--

	if( m_cdData->vertecies_count > 0 )
	{
		if( m_FileVersion > EFN_VERSION_22 )
		{
			/************************************ load list of aabbs *********************************/
			m_aabbList = new D3DXVECTOR3*[m_cdData->anim_frames_count];

			for( int i = 0; i < m_cdData->anim_frames_count; i++ )
			{
				m_aabbList[i] = new D3DXVECTOR3[8];
				file.read( (char *)m_aabbList[i], sizeof(D3DXVECTOR3) * 8 );
			}
			/********************************END* load list of aabbs *********************************/

			/************************************ load list of bounding spheres ********************/
			m_pBoundingSpheres = new CSPHERE[ m_cdData->anim_frames_count ];
			file.read( (char *)m_pBoundingSpheres, sizeof(CSPHERE) * m_cdData->anim_frames_count );
			/********************************END* load list of bounding spheres *****************/
		}

		if( m_tCompessionType == ECT_NONE )
		{
			file.read((char *)m_cdData->posList, sizeof(CVERTEX_XYZ) * m_cdData->vertecies_count);
		}
		else if( m_tCompessionType == ECT_SHORT )
		{
			m_cdData->buffshorts = new unsigned short[m_cdData->vertecies_count * 3];

			int	iVPF = m_cdData->vertecies_per_frame_count;

			file.read( (char *)m_cdData->buffshorts, sizeof(unsigned short) * m_cdData->vertecies_count * 3 );

			for( int i = 0; i < m_cdData->anim_frames_count; i++ )
			{
				int		iFrmOff		= i * iVPF;
				int		iFrmOff2	= i * iVPF * 3;
				double	fFactorX	= (double)fabs( m_aabbList[i][1].x - m_aabbList[i][7].x ) /	CompressionFactor;
				double	fFactorY	= (double)fabs( m_aabbList[i][1].y - m_aabbList[i][7].y ) /	CompressionFactor;
				double	fFactorZ	= (double)fabs( m_aabbList[i][1].z - m_aabbList[i][7].z ) /	CompressionFactor;

				for( int j = 0; j < iVPF; j++ )
				{
					m_cdData->posList[iFrmOff + j].x = m_aabbList[i][7].x + m_cdData->buffshorts[iFrmOff2 + j * 3 + 0] * fFactorX;
					m_cdData->posList[iFrmOff + j].y = m_aabbList[i][7].y + m_cdData->buffshorts[iFrmOff2 + j * 3 + 1] * fFactorY;
					m_cdData->posList[iFrmOff + j].z = m_aabbList[i][7].z + m_cdData->buffshorts[iFrmOff2 + j * 3 + 2] * fFactorZ;
				}	
			}
		}

		if( m_FileVersion > EFN_VERSION_22 )
		{
			/************************************ load indices list **********************************/
			ReadStrm( file, m_cdData->vertex_indices_count );

			m_cdData->vertex_indices_list = new int[m_cdData->vertex_indices_count];


			if( !m_cdData->vertex_indices_list )
				return false;

			file.read( (char*)m_cdData->vertex_indices_list, sizeof(int) * m_cdData->vertex_indices_count );
			/********************************END* load indices list ********************************/
		}
	}
	/********************************END* load vertecies list *******************************/

	if( m_FileVersion < EFN_VERSION_23 )
	{
		/************************************ load indices list **********************************/
		ReadStrm( file, m_cdData->vertex_indices_count );

		m_cdData->vertex_indices_list = new int[m_cdData->vertex_indices_count];

		if( !m_cdData->vertex_indices_list )
			return false;

		file.read( (char*)m_cdData->vertex_indices_list, sizeof(int) * m_cdData->vertex_indices_count );
		/********************************END* load indices list ********************************/

		ReadStrm( file, m_cdData->vertecies_per_frame_count ); // amount of frames
		ReadStrm( file, m_cdData->anim_frames_count ); // amount of frames
	}

	//ReadStrm( file, m_cdData->vertex_indices_count );
	m_cdData->anim_frame_scripts = new str_t[GetFramesCount()];

	if( m_FileVersion > EFN_VERSION_22 )
	{
		bool bReadAnimScripts;
		ReadStrm( file, bReadAnimScripts );

		if( bReadAnimScripts )
		{
			for( int i = 0; i < m_cdData->anim_frames_count; i++ )
				ReadStrm( file, &m_cdData->anim_frame_scripts[i] );
		}
		else
		{
			for( int i = 0; i < m_cdData->anim_frames_count; i++ )
				m_cdData->anim_frame_scripts[i] = "";
		}
	}
	else
	{
		for( int i = 0; i < m_cdData->anim_frames_count; i++ )
			ReadStrm( file, &m_cdData->anim_frame_scripts[i] );
	}

	
	if( m_dlgValidate )
	{
		m_dlgValidate->SetProgressMessageStrA( "step 2 of 5" );
		m_dlgValidate->SetProgressMessageStrC( "Misc objects..." );
		
		m_dlgValidate->Step();

		if( m_pParent )
			((CCookieMachineDlg*)m_pParent)->ProcessMessagesGlob();
	}

	/***************************************** load nulls list ********************************/
	int iNullsCount = 0;
	ReadStrm( file, iNullsCount );

	for( int i = 0; i < iNullsCount; i++ )
	{
		CNULL2 nl;

		//read id
		ReadStrm( file, &nl.id );

		bool bPretransform;

		//read matrices data
		for( int j = 0; j < m_cdData->anim_frames_count; j++ )
		{
			CTRANSFORMEX trEx;

			file.read( (char*)&trEx.pos, sizeof(float) * 3 );
			file.read( (char*)&trEx.rot, sizeof(float) * 3 );
			file.read( (char*)&trEx.scl, sizeof(float) * 3 );
			
			nl.matrices.push_back( trEx );
		}
		
		if( m_FileVersion > EFN_VERSION_21 )
		{
			ReadStrm( file, bPretransform );

			if( bPretransform )
			{
				for( int j = 0; j < m_cdData->anim_frames_count; j++ )
				{
					file.read( (char*)&nl.matrices[j].pre_transform, sizeof(D3DXMATRIX) );
				}
			}
		}

		m_cdData->vecNulls.push_back( nl );	
	}
	/***********************************END* load nulls list ********************************/

	ReadStrm( file, m_cdData->global_radius ); // global radius


	if( m_FileVersion < EFN_VERSION_23 )
	{
		/************************************ load list of aabbs *********************************/
		m_aabbList = new D3DXVECTOR3*[m_cdData->anim_frames_count];

		for( int i = 0; i < m_cdData->anim_frames_count; i++)
		{
			m_aabbList[i] = new D3DXVECTOR3[8];
			file.read( (char *)m_aabbList[i], sizeof(D3DXVECTOR3) * 8 );
		}
		/********************************END* load list of aabbs *******************************/

		/************************************ load list of bounding spheres ********************/
		m_pBoundingSpheres = new CSPHERE[ m_cdData->anim_frames_count ];
		file.read( (char *)m_pBoundingSpheres, sizeof(CSPHERE) * m_cdData->anim_frames_count );
		/********************************END* load list of bounding spheres *****************/
	}

	/************************************ load list of z-offsets ******************************/
	m_cdData->z_coord_offset_list = new D3DXVECTOR3[m_cdData->anim_frames_count];

	if( m_FileVersion > EFN_VERSION_22 )
	{
		if( !m_cdData->z_coord_offset_list )
			return false;

		bool bReadZOffsets;
		ReadStrm( file, bReadZOffsets );

		if( bReadZOffsets )
		{
			file.read((char *)m_cdData->z_coord_offset_list, sizeof(D3DXVECTOR3) * m_cdData->anim_frames_count);
		}
		else
		{
			for( int i = 0; i < m_cdData->anim_frames_count; i++ )
				m_cdData->z_coord_offset_list[i] = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}
	}
	else
		file.read((char *)m_cdData->z_coord_offset_list, sizeof(D3DXVECTOR3) * m_cdData->anim_frames_count);
	/********************************END* load list of z-offsets ***************************/


	/************************************ load amount of animations **********************/
	int iAnimationsCount = 0;

	file.read( (char *)&iAnimationsCount, sizeof(int) );
	/************************************ load amount of animations **********************/


	/************************************ load list of animations ***************************/
	for( int i = 0; i < iAnimationsCount; i++ )
	{
		EFN_ANIMATION anim;

		ReadStrm( file, &anim.name );					//get name
		ReadStrm( file, anim.frames_count );			//get frames count

		anim.frames_list = new int[anim.frames_count]; //get frames list

		if( !anim.frames_list )
			return false;

		file.read( (char*)anim.frames_list, sizeof(int) * anim.frames_count );

		ReadStrm( file, anim.IsTransition );	//get transition flag  -- hack!!!

		m_cdData->vecAnimations.push_back( anim );
	}
	/********************************END* load list of animations **********************/

	if( m_dlgValidate )
	{
		m_dlgValidate->SetProgressMessageStrA( "step 3 of 5" );
		m_dlgValidate->SetProgressMessageStrC( "Reverse index..." );
		
		m_dlgValidate->Step();

		if( m_pParent )
			((CCookieMachineDlg*)m_pParent)->ProcessMessagesGlob();
	}

	/************************************ load reverse index *********************************/
	m_cdData->reverse_index = new int*[m_cdData->vertecies_per_frame_count];

	for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
	{
		int temp_rv;

		// read tris amount
		file.read((char *)&temp_rv, sizeof(int));
		
		m_cdData->reverse_index[i] = new int[temp_rv];

		// read tris list
		file.read((char *)m_cdData->reverse_index[i], sizeof(int) * temp_rv);
	}
	/********************************END* load reverse index *********************************/

	
	/************************************ load uv list ********************************************/
	m_cdData->uvList = new CVERTEX_UV[m_cdData->vertecies_per_frame_count];
	m_cdData->uvList2 = new CVERTEX_UV[m_cdData->vertecies_per_frame_count];

	file.read( (char *)m_cdData->uvList, sizeof(CVERTEX_UV) * m_cdData->vertecies_per_frame_count );
	/************************************END* load uv list **************************************/


	/************************************* load vert color list ************************************/
	m_cdData->vcolList = new long[m_cdData->vertecies_per_frame_count];

	file.read( (char *)m_cdData->vcolList, sizeof(long) * m_cdData->vertecies_per_frame_count );
	/********************************END* load vert color list ************************************/


	/************************************* load animation FPS **************************************/
	ReadStrm( file, m_cdData->fFPS );
	/*********************************END* load animation FPS **************************************/


	/************************************* load material *******************************************/
	if( m_FileVersion > EFN_VERSION_19 )
	{
		file.read( (char *)&m_cdData->material, sizeof( D3DMATERIAL9 ) );
	}
	else
	{
		m_cdData->material.Ambient = D3DXCOLOR( 0xffffffff );
		m_cdData->material.Diffuse = D3DXCOLOR( 0xffffffff );
	}
	/*********************************END* load material *******************************************/


	/************************************* load custom params ***************************************/
	if( m_FileVersion > EFN_VERSION_20 )
	{
		int iCustParamCounter;
		ReadStrm( file, iCustParamCounter );

		for( int i = 0; i < iCustParamCounter; i++ )
		{
			CCUSTPARAM par;

			float fVal;
			ReadStrm( file, &par.id );

			for( int j = 0; j < GetFramesCount(); j++ )
			{
				ReadStrm( file, fVal );		
				par.data.push_back( fVal );
			}

			m_cdData->vecCustomParams.push_back( par );
		}
	}
	/*********************************EFN* load custom params ***************************************/

	if( m_dlgValidate )
	{
		m_dlgValidate->SetProgressMessageStrA( "step 4 of 5" );
		m_dlgValidate->SetProgressMessageStrC( "Normals precalc..." );
		
		m_dlgValidate->Step();

		if( m_pParent )
			((CCookieMachineDlg*)m_pParent)->ProcessMessagesGlob();
	}

	file.close();

	PrecalculateNormals();
	
	//CalcAABBs();

	return true;
}

void CEfnCharacter::DegradeAnimation( float fPercentage )
{
	m_cdData->fFPS /= 2.0f;

	int iResFrames = m_cdData->anim_frames_count / 2;

	for( int i = 0; i < iResFrames; i++ )
	{
		DeleteFrames( i, i );
	}
}

bool CEfnCharacter::IsAnimsRangeOK()
{
	if( !m_cdData )
		return false;

	if( m_cdData->vecAnimations.empty() )
		return true;

	bool test_ok = true;

	for( UINT i = 0; i < m_cdData->vecAnimations.size(); i++ )
	{
		if( m_cdData->vecAnimations[i].frames_count > 0 )
		{
			int iStartingFrame	= m_cdData->vecAnimations[i].frames_list[0];
			int iEndingFrame	= m_cdData->vecAnimations[i].frames_list[m_cdData->vecAnimations[i].frames_count - 1];

			if( iStartingFrame < 0 || iStartingFrame >= m_cdData->anim_frames_count || 
				iEndingFrame < 0 || iEndingFrame >= m_cdData->anim_frames_count )
			{
				test_ok = false;
			}
		}
	}

	return test_ok;
}

void CEfnCharacter::AddAnimation( EFN_ANIMATION* pAnimation )
{
	m_cdData->vecAnimations.push_back( *pAnimation );
}

void CEfnCharacter::GetMinMaxDimentions( D3DXVECTOR3& vecMin, D3DXVECTOR3& vecMax )
{
	if( GetFramesCount() == 0 || m_cdData->vertecies_count == 0 )
	{
		vecMin =  D3DXVECTOR3( 0, 0, 0 );
		vecMax =  D3DXVECTOR3( 0, 0, 0 );
		return;
	}

	D3DXVECTOR3 minV( 999999, 999999, 999999 );
	D3DXVECTOR3 maxV( -999999, -999999, -999999 );

	for( int i = 0; i < GetFramesCount(); i++ )
	{
		if( m_aabbList[i][7].x < minV.x )
			minV.x = m_aabbList[i][7].x;

		if( m_aabbList[i][7].y < minV.y )
			minV.y = m_aabbList[i][7].y;

		if( m_aabbList[i][7].z < minV.z )
			minV.z = m_aabbList[i][7].z;

		if( m_aabbList[i][1].x < maxV.x )
			maxV.x = m_aabbList[i][1].x;

		if( m_aabbList[i][1].y < maxV.y )
			maxV.y = m_aabbList[i][1].y;

		if( m_aabbList[i][1].z < maxV.z )
			maxV.z = m_aabbList[i][1].z;
	}

	vecMin = minV;
	vecMax = maxV;
}

void CEfnCharacter::SetVeteciesColor( DWORD dwColor )
{
	for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
		m_cdData->vcolList[i] = dwColor;
}

bool CEfnCharacter::SaveToFile()
{
	return SaveToFile( this->GetFolder() + this->GetFilename() );
}

bool CEfnCharacter::SaveToFile( str_t path, bool bSilent )
{
	str_t curr_ver =  GetEfnVersionAsString( EFN_CURR_VERSION );
	str_t file_ver =  GetEfnVersionAsString( m_FileVersion );

	if( m_FileVersion != EFN_CURR_VERSION )
	{
		if( bSilent == false )
		{
			if( MessageBox( NULL,
							( "Файл имеет версию " + file_ver + ", он может быть сохранён только в\n\r версию " +
							curr_ver + ". Сохранить файл?" ).data(),
							"Внимание!",
							MB_YESNO ) != IDYES )
			{
				return true;
			}
		}

		m_FileVersion = EFN_CURR_VERSION; 
	}

	streampos pos;
	int et = efn_type;

	ofstream file( path.data(), ios::binary );
	file.seekp( pos );

	WR2Strm( file, (DWORD)EFN_CURR_VERSION );				//store efn file version
	file.write( (char *)&et, sizeof(int) );					//store efn filetype bit
	WR2Strm( file, m_cdData->name ); 						//store name
	//WR2Strm( file, m_cdData->texture_filename );			//store texture filename

	int iComprType = (int)m_tCompessionType;
	WR2Strm( file, iComprType );

	WR2Strm( file, m_cdData->vertecies_count );				//store vertecies count
	WR2Strm( file, m_cdData->anim_frames_count );			//store amount of frames

	if( m_cdData->vertecies_count > 0 )
	{
		//aabbs
		for( int i = 0; i < GetFramesCount(); i++ )
			file.write( (const char *)m_aabbList[i], sizeof(D3DXVECTOR3) * 8 );

		//store list of bounding spheres
		file.write( (const char *)m_pBoundingSpheres, sizeof(CSPHERE) * m_cdData->anim_frames_count );

		if( m_tCompessionType == ECT_NONE )
		{
			//store vertecies list
			file.write( (const char *)m_cdData->posList, sizeof(CVERTEX_XYZ) * m_cdData->vertecies_count );
		}
		else if( m_tCompessionType == ECT_SHORT )
		{
			if( m_cdData->buffshorts == NULL )
			{
				int	iVPF = m_cdData->vertecies_per_frame_count;

				m_cdData->buffshorts = new unsigned short[m_cdData->vertecies_count * 3];

				for( int i = 0; i < GetFramesCount(); i++ )
				{
					double	fFactorX	= (double)fabs(( m_aabbList[i][1].x - m_aabbList[i][7].x ) / CompressionFactor );
					double	fFactorY	= (double)fabs(( m_aabbList[i][1].y - m_aabbList[i][7].y ) / CompressionFactor );
					double	fFactorZ	= (double)fabs(( m_aabbList[i][1].z - m_aabbList[i][7].z ) / CompressionFactor );

					for( int j = 0; j < iVPF; j++ )
					{
						m_cdData->buffshorts[i * iVPF * 3 + j * 3 + 0] = abs( ( m_aabbList[i][7].x - m_cdData->posList[i * iVPF + j].x ) / fFactorX );
						m_cdData->buffshorts[i * iVPF * 3 + j * 3 + 1] = abs( ( m_aabbList[i][7].y - m_cdData->posList[i * iVPF + j].y ) / fFactorY );
						m_cdData->buffshorts[i * iVPF * 3 + j * 3 + 2] = abs( ( m_aabbList[i][7].z - m_cdData->posList[i * iVPF + j].z ) / fFactorZ );
					}	
				}
			}

			file.write( (const char *)m_cdData->buffshorts, sizeof(unsigned short) * m_cdData->vertecies_count * 3 ); 
		}

		WR2Strm( file, m_cdData->vertex_indices_count );		//store indices count

		//store indices list
		file.write( (const char *)m_cdData->vertex_indices_list, sizeof(int) * m_cdData->vertex_indices_count ); 
	}

	//frame scripts
	bool write_frm_scripts = false; 
	
	for( int i = 0; i < m_cdData->anim_frames_count; i++ )
	{
		CString strtest = m_cdData->anim_frame_scripts[i].data();
		strtest = strtest.Trim();
	
		if( strtest != "" )
		{
			write_frm_scripts = true;
			break;
		}
	}

	WR2Strm( file, write_frm_scripts );

	if( write_frm_scripts )
	{
		for( int i = 0; i < m_cdData->anim_frames_count; i++ )
			WR2Strm( file, m_cdData->anim_frame_scripts[i] );
	}

	//store nulls count
	WR2Strm( file, GetNullsCount() );			//store nulls count

	for( int i = 0; i < GetNullsCount(); i++ )	//store nulls data
	{
		WR2Strm( file, m_cdData->vecNulls[i].id );		//store null id

		for( int j = 0; j < GetFramesCount(); j++ )		//store matrices
		{
			ClampAngle( m_cdData->vecNulls[i].matrices[j].rot.x );
			ClampAngle( m_cdData->vecNulls[i].matrices[j].rot.y );
			ClampAngle( m_cdData->vecNulls[i].matrices[j].rot.z );

			file.write( (const char *)&m_cdData->vecNulls[i].matrices[j], sizeof(float) * 9 );
		}

		//check whether pretransformation should be stored
		bool bStore = false;

		for( int j = 0; j < GetFramesCount(); j++ )	//store matrices
		{
			if( D3DXMatrixIsIdentity( &m_cdData->vecNulls[i].matrices[j].pre_transform ) == false )
				bStore = true;
		}

		if( bStore )
		{
			WR2Strm( file, true );

			for( int j = 0; j < GetFramesCount(); j++ )
				file.write( (char*)&m_cdData->vecNulls[i].matrices[j].pre_transform, sizeof( D3DXMATRIX ) );
		}
		else
		{
			WR2Strm( file, false );
		}
	}

	WR2Strm( file, m_cdData->global_radius );				//store global radius

	//store z-offsets
	bool write_zoffsets = false; 

	for( int i = 0; i < m_cdData->anim_frames_count; i++ )
	{
		if( m_cdData->z_coord_offset_list[i].z != 0.0f )
		{
			write_zoffsets = true;
			break;
		}
	}

	WR2Strm( file, write_zoffsets );

	if( write_zoffsets )
	{	
		file.write( (char *)m_cdData->z_coord_offset_list, sizeof(D3DXVECTOR3) * m_cdData->anim_frames_count );
	}

	//store animations count
	WR2Strm( file, GetAnimationsCount() );

	//store animations list
	for( int i = 0; i < GetAnimationsCount(); i++ )
	{
		//store name
		WR2Strm( file, m_cdData->vecAnimations[i].name );

		//store count
		WR2Strm( file, m_cdData->vecAnimations[i].frames_count );

		//store list
		file.write( (const char *)m_cdData->vecAnimations[i].frames_list, sizeof(int) * m_cdData->vecAnimations[i].frames_count );


		/*********************/
		/******* HACK!!! ***/
		/*********************/
		// store transition flag
		WR2Strm( file, m_cdData->vecAnimations[i].IsTransition );
	}

	//store reverse index
	for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
	{
		WR2Strm( file, m_cdData->reverse_index[i][0] + 1 ); // store tris amount
		file.write( (const char *)m_cdData->reverse_index[i], sizeof(int) * (m_cdData->reverse_index[i][0] + 1) ); //store tris list
	}

	//store uv coords
	file.write( (const char *)m_cdData->uvList, sizeof(CVERTEX_UV) * m_cdData->vertecies_per_frame_count );

	//store vertecies colors
	file.write( (const char *)m_cdData->vcolList, sizeof(long) * m_cdData->vertecies_per_frame_count );

	//store FPS
	WR2Strm( file, m_cdData->fFPS );

	//store material
	file.write( (char *)&m_cdData->material, sizeof( D3DMATERIAL9 ) );

	//store custom parameters
	WR2Strm( file, GetCustParamsCount() );

	for( int i = 0; i < GetCustParamsCount(); i++ )
	{
		WR2Strm( file, m_cdData->vecCustomParams[i].id );
		//WR2Strm( file, (int)0 );
			
		for( int j = 0; j < GetFramesCount(); j++ )
			WR2Strm( file, (float)m_cdData->vecCustomParams[i].data[j] );		
	}


	file.close();

	return false;
}

void CEfnCharacter::ConnectAllTris()
{
	CVERTEX_XYZ*	pPosList	= NULL;
	CVERTEX_XYZ*	pNormList	= NULL;
	CVERTEX_UV*		pUVList		= NULL;
	int				iFrames		= GetFramesCount();
	int*			iIndecies	= m_cdData->vertex_indices_list;
	CVERTEX_XYZ*	pPos		= m_cdData->posList;
	CVERTEX_UV*		pUV			= m_cdData->uvList;
	vector<int>		tmp_index, tmp_index2, tmpVec;

	//count unique vertecies 
	for( int i = 0; i < m_cdData->vertex_indices_count; i++ )
	{
		tmp_index.push_back( iIndecies[i] );

		for( int j = 0; j < i; j++ )
		{
			if( ( iIndecies[i] != iIndecies[j] ) && ( pPos[iIndecies[i]] == pPos[iIndecies[j]] && pUV[iIndecies[i]] == pUV[iIndecies[j]] ) )
			{
				tmp_index[i] = iIndecies[j];
				break;
			}
		}
	}

	tmp_index2.assign( tmp_index.begin(), tmp_index.end() );
	RemoveDupesFromVector( &tmp_index2 );


	// check whether model is already connected
	if( tmp_index2.size() == m_cdData->vertecies_per_frame_count )
		return;


	// build indexed vertex array, eliminating equal vertecies
	for( int i = 0; i < m_cdData->vertex_indices_count; i++ )
		tmpVec.push_back( -1 );

	for( int i = 0; i < m_cdData->vertex_indices_count; i++ )
	{
		for( UINT j = 0; j < tmp_index2.size(); j++ )
		{
			if( tmp_index[i] == tmp_index2[j] && tmpVec[i] == -1 )
				tmpVec[i] = j;
		}
	}
	tmp_index.assign( tmpVec.begin(), tmpVec.end() );


	pPosList	= new CVERTEX_XYZ[iFrames * tmp_index2.size() ];
	pNormList	= new CVERTEX_XYZ[iFrames * tmp_index2.size() ];
	pUVList		= new CVERTEX_UV[tmp_index2.size() ];

	for( int i = 0; i < iFrames; i++ )
	{
		for( UINT j = 0; j < tmp_index2.size(); j++ )
		{
			int off = i * tmp_index2.size();
			int off2 = i * m_cdData->vertecies_per_frame_count;

			pPosList[off + j] = pPos[off2 + tmp_index2[j]];

			if( i == 0 )
				pUVList[off + j] = pUV[off2 + tmp_index2[j]];
		}
	}

	SAFE_DELETE_ARRAY( m_cdData->posList );
	SAFE_DELETE_ARRAY( m_cdData->normList );
	SAFE_DELETE_ARRAY( m_cdData->uvList )

	m_cdData->posList	= pPosList;
	m_cdData->normList	= pNormList;
	m_cdData->uvList	= pUVList;

	for( int i = 0; i < m_cdData->vertex_indices_count; i++ )
		iIndecies[i] = tmp_index[i];

	if( m_cdData->reverse_index )
	{
		for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
			SAFE_DELETE_ARRAY( m_cdData->reverse_index[i] );

		SAFE_DELETE_ARRAY( m_cdData->reverse_index );
	}

	m_cdData->vertecies_per_frame_count	= tmp_index2.size();
	m_cdData->vertecies_count			= tmp_index2.size() * iFrames;

	CalcReverseIndex();
	PrecalculateNormals();
}

void CEfnCharacter::DisconnectAllTris()
{
	CVERTEX_XYZ*	pPosList	= NULL;
	CVERTEX_XYZ*	pNormList	= NULL;
	CVERTEX_UV*		pUV			= NULL;
	long*			pVColors	= NULL;

	int iFrames	= GetFramesCount();

	pPosList	= new CVERTEX_XYZ[iFrames * m_cdData->vertex_indices_count];
	pNormList	= new CVERTEX_XYZ[iFrames * m_cdData->vertex_indices_count];
	pVColors	= new long[m_cdData->vertex_indices_count];
	pUV			= new CVERTEX_UV[m_cdData->vertex_indices_count];
	
	for( int i = 0; i < iFrames; i++ )
	{
		int off = i * m_cdData->vertex_indices_count;
		int off2 = i * m_cdData->vertecies_per_frame_count;
		
		for( int j = 0; j < m_cdData->vertex_indices_count; j++ )
		{	
			pPosList[off + j] = m_cdData->posList[off2 + m_cdData->vertex_indices_list[j]];

			if( i == 0 )
				pUV[j] = m_cdData->uvList[m_cdData->vertex_indices_list[j]];
		}
	}

	SAFE_DELETE_ARRAY( m_cdData->posList );
	SAFE_DELETE_ARRAY( m_cdData->normList );
	SAFE_DELETE_ARRAY( m_cdData->uvList );
	SAFE_DELETE_ARRAY( m_cdData->vcolList );

	m_cdData->posList	= pPosList;
	m_cdData->normList	= pNormList;
	m_cdData->uvList	= pUV;
	m_cdData->vcolList	= pVColors;

	for( int i = 0; i < m_cdData->vertex_indices_count; i++ )
		m_cdData->vertex_indices_list[i] = i;

	KillReverseIndex();

	m_cdData->vertecies_per_frame_count	= m_cdData->vertex_indices_count;
	m_cdData->vertecies_count			= m_cdData->vertex_indices_count * iFrames;

	CalcReverseIndex();
	PrecalculateNormals();
}

void CEfnCharacter::KillReverseIndex()
{
	if( m_cdData->reverse_index )
	{
		for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
			SAFE_DELETE_ARRAY( m_cdData->reverse_index[i] );

		SAFE_DELETE_ARRAY( m_cdData->reverse_index );
	}
}

void CEfnCharacter::PrecalculateNormals()
{
	int i, j, k, vert1, vert2, vert3, iFrames, iTriCount, iVPF;
	
	CVector			vA, vB, vC, vTmp1, vTmp2;
	CVector**		vTriNormals;
	CVERTEX_XYZ*	vlist;
	CVERTEX_XYZ*	nlist;
	CString			msg, msg_temp;

	iFrames		= GetFramesCount();
	iVPF		= m_cdData->vertecies_per_frame_count;
	iTriCount	= m_cdData->vertex_indices_count / 3;
	vlist		= m_cdData->posList;
	nlist		= m_cdData->normList;

	//-----------------------------polygon normals calculations---------------------------
	//allocate memory
	vTriNormals = new CVector*[iFrames];

	for( i = 0; i < iFrames; i++ )
		vTriNormals[i] = new CVector[iTriCount];

	for( j = 0; j < iFrames; j++ )
	{
		int iFrmOff = j * iVPF;

		for( i = 0; i < iTriCount; i++ )
		{
			//get indexes of vertecies from triangle
			int iOff = i * 3;

			vert1 = m_cdData->vertex_indices_list[iOff] + iFrmOff;
			vert2 = m_cdData->vertex_indices_list[iOff + 1] + iFrmOff;
			vert3 = m_cdData->vertex_indices_list[iOff + 2] + iFrmOff;

			vC.x = vlist[ vert3 ].x - vlist[ vert1 ].x;
			vC.y = vlist[ vert3 ].y - vlist[ vert1 ].y;
			vC.z = vlist[ vert3 ].z - vlist[ vert1 ].z;

			vB.x = vlist[ vert2 ].x - vlist[ vert1 ].x;
			vB.y = vlist[ vert2 ].y - vlist[ vert1 ].y;
			vB.z = vlist[ vert2 ].z - vlist[ vert1 ].z;

			vTriNormals[j][i].x = vB.y * vC.z - vB.z * vC.y;
			vTriNormals[j][i].y = vB.z * vC.x - vB.x * vC.z;
			vTriNormals[j][i].z = vB.x * vC.y - vB.y * vC.x;

			float len = 1.0f / sqrtf((float)(vTriNormals[j][i].x * vTriNormals[j][i].x + vTriNormals[j][i].y * vTriNormals[j][i].y + vTriNormals[j][i].z * vTriNormals[j][i].z));

			vTriNormals[j][i].x *= len;
			vTriNormals[j][i].y *= len;
			vTriNormals[j][i].z *= len;
		}
	}
	//------------------------END--polygon normals calculations---------------------//

	//vector<CVector> collect_buff;

	//-----------------------------vertex normals calculations----------------------------//
	int count = 0;

	for( int j = 0; j < iFrames; j++ )
	{
		//summ up normals using cache
		for( k = 0; k < iVPF; k++ )
		{
			count = 0;
			vTmp1.Clear();
			//collect_buff.clear();

			for( int m = 0; m < m_cdData->reverse_index[k][0]; m++ )
			{
				int tri_index = m_cdData->reverse_index[k][m + 1];
				vTmp1 += vTriNormals[j][ tri_index ];

				count++;
			}

			//calc average vector
			if( count > 0 )
			{
				//for( int i = 0; i < vTmp1.si )
				//vTmp1 /= (float)count;

				vTmp1.x *= 1.0f / count;
				vTmp1.y *= 1.0f / count;
				vTmp1.z *= 1.0f / count;

				float len = 1.0f / sqrtf((float)(vTmp1.x * vTmp1.x + vTmp1.y * vTmp1.y + vTmp1.z * vTmp1.z));

				vTmp1.x *= len;
				vTmp1.y *= len;
				vTmp1.z *= len;
			}
				
			int iFrmOff = j * iVPF + k;

			nlist[iFrmOff].x = vTmp1.x;
			nlist[iFrmOff].y = vTmp1.y;
			nlist[iFrmOff].z = vTmp1.z;
		}
	}  // for (j = 0; j < iFrames; j++)

	for( i = 0; i < iFrames; i++ )
		SAFE_DELETE_ARRAY( vTriNormals[i] );

	SAFE_DELETE_ARRAY( vTriNormals );
}

void CEfnCharacter::Init()
{
	m_tCompessionType			= ECT_NONE;
	ppMesh						= NULL;
	m_cdData					= NULL;
	m_pNormalsCache				= NULL;

	//buffers pt init
	m_pIB						= NULL;
	m_pIB2						= NULL;
	m_pPosBuff					= NULL;
	m_pNormBuff					= NULL;	
	m_pUvBuff					= NULL;
	m_pUvBuff2					= NULL;
	m_pVColorBuff				= NULL;
	m_pVertexDeclaration		= NULL;
	m_pVBLines					= NULL;
	m_pVBNull					= NULL;
	m_pVBSelectedNull			= NULL;
	m_pTexture					= NULL;
	m_pParent					= NULL;
	m_pEffect					= NULL;

	m_iSequenceFrames			= NULL;
	m_iSequenceFramesCount		= 0;
	m_iCurrentFrame				= 0;
	m_iNextFrame				= 0;

	m_bIsRotationFinished		= true;
	m_bIsRotationWFinished		= true;
	m_bIsSequencePlaying		= false;
	m_bIsBoundingSphereVisible	= false;
	m_bIsAABBVisible			= false;
	m_bIsUV1Used				= true;
	m_bRenderNormals			= false;
	m_bLodExists				= false;

	m_fCurrentAngle				= 0.0f;

	m_vCharXZPos.SetData( 0.0f, 0.0f, 0.0f );
	m_vCharWorldPos.SetData( 0.0f, 0.0f, 0.0f );

	m_fZOffset					= 0.0f;

	m_iCurrentSequenceFrame		= 0;
	m_iZOffsetFrame				= 0;

	FileType					= FT_EFN;
	efn_type					= EF_ANIMATION;

	m_dlgValidate				= NULL;
	m_aabbList					= NULL;
	m_pBoundingSpheres			= NULL;
	ppMesh						= NULL;
	m_iSelectedNull				= -1;

	m_FileVersion				= EFN_CURR_VERSION;

	// setup materials
	ZeroMemory( &m_BlackMtrl, sizeof( m_BlackMtrl ) );
	m_BlackMtrl.Diffuse = D3DXCOLOR( 0xff000000 );
	m_BlackMtrl.Ambient = D3DXCOLOR( 0xff000000 );

	ZeroMemory( &m_WhiteMtrl, sizeof( m_WhiteMtrl ) );
	m_WhiteMtrl.Diffuse = D3DXCOLOR( 0xffffffff );
	m_WhiteMtrl.Ambient = D3DXCOLOR( 0xffffffff );

	ZeroMemory( &m_Material, sizeof(m_Material) );
	m_Material.Diffuse = D3DXCOLOR( 0xffffffff );
	m_Material.Ambient = D3DXCOLOR( 0x80808080 );
	
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
}

void CEfnCharacter::RecalcGlobalRadius()
{
	//int		amount_of_values	= 0;
	//int*	frames_array		= NULL;
	//int		offset				= 0;
	//int		animations_count	= 8;

	////go through list of animations looking for nessesary animations
	//str_t anim_names[] = {"начало походки", "цикл походки", "конец походки", "начало бега", "цикл бега", "конец бега", "поворот направо", "поворот налево"};

	////create list of frames from found animations

	////calc amount of found frames
	//for( int i = 0; i < animations_count; i++ )
	//{
	//	for( int j = 0; j < GetAnimationsCount(); j++ )
	//	{		
	//		if( m_cdData->vecAnimations[j].name == anim_names[i] )
	//			amount_of_values += m_cdData->animations_list[j].frames_count;
	//	}
	//}

	////no animation frames found
	//if( amount_of_values <= 0)
	//	return;

	////allocate memory
	//frames_array = new int[amount_of_values];

	////copy found frames
	//for( int i = 0; i < animations_count; i++ )
	//{
	//	for( int j = 0; j < m_cdData->animations_count; j++ )
	//	{		
	//		if( m_cdData->animations_list[j].name == anim_names[i] )
	//		{
	//			int y = 0;

	//			for( int u = offset; u < m_cdData->animations_list[j].frames_count + offset; u++)
	//			{
	//				frames_array[u] = m_cdData->animations_list[j].frames_list[ u - offset ];
	//			}

	//			offset += m_cdData->animations_list[j].frames_count;
	//		}
	//	}
	//}

	//D3DXVECTOR3 tmp, tmp2;
	//float					length = 0.0f;

	////take deviation of bspheres into account
	////find most faraway points
	//for( int i = 0; i < amount_of_values; i++ )
	//{
	//	//take sphere
	//	tmp = m_pBoundingSpheres[ frames_array[i] ].pos;

	//	//compare to all other spheres
	//	for( int j = 0; j < amount_of_values; j++ )
	//	{
	//		tmp2 = tmp - m_pBoundingSpheres[ frames_array[j] ].pos;

	//		if( length < D3DXVec3Length( &tmp2) )
	//			length	= D3DXVec3Length( &tmp2);
	//	}
	//}
	//
	////div length between most faraway points by 2
	//length /= 2.0f;
 //   
	////find biggest sphere radius from calculated framelist
	//float big_radius = 0.0f;

	//for( int i = 0; i < amount_of_values; i++ )
	//{
	//	if( big_radius < m_pBoundingSpheres[ frames_array[i] ].radius )
	//		big_radius = m_pBoundingSpheres[ frames_array[i] ].radius;
	//}

	////add half of deviation to the biggest sphere radius
	//m_cdData->global_radius = 0.3f; // length + big_radius;

	////free memory
	//SAFE_DELETE_ARRAY( frames_array );
}

void CEfnCharacter::Kill()
{
	if( !m_cdData )
		return;

	SAFE_DELETE_ARRAY( m_iSequenceFrames );
	m_strSequenceAnims.clear();

	// free main buffers
	SAFE_RELEASE( m_pIB );
	SAFE_RELEASE( m_pIB2 );
	SAFE_DELETE_ARRAY3D( m_pPosBuff, GetFramesCount() );
	SAFE_DELETE_ARRAY3D( m_pNormBuff, GetFramesCount() );
	SAFE_RELEASE( m_pUvBuff );
	SAFE_RELEASE( m_pUvBuff2 );
	SAFE_RELEASE( m_pVColorBuff );

	// free vertex declaration
	SAFE_RELEASE( m_pVertexDeclaration );

	// free misc buffers
	SAFE_DELETE_ARRAY3D( ppMesh, GetFramesCount() );
	SAFE_DELETE_ARRAY3D( m_pVBLines, GetFramesCount() );
	SAFE_RELEASE( m_pVBNull );
	SAFE_RELEASE( m_pVBSelectedNull );

	//delete texture
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pEffect );

	//delete data
	KillData();

	SAFE_DELETE( m_cdData );
	SAFE_DELETE_ARRAY( m_pNormalsCache );

	return;
}

bool CEfnCharacter::InitFromXSICharacter( CXSIFile *character )
{
	int i, j;
	
	//kill current data
	Kill();

	/******************************* create new data ********************************/
	m_cdData = new EFN_CHARACTERDATA;
	InitData();
	/***************************END* create new data ********************************/


	/******************************* create vertex indices list *********************/
	int* indexTemp		= NULL;
	int* indexLoc		= NULL;
	int  indexLocCount	= 0;
	int  iPrevMeshIndex	= -1;
	int	 offsetGlob		= 0;
	int  offsetLoc		= 0;

	//count num of indices in single frame index
	for( i = 0; i < character->GetMeshesCount(); i++ )
	{
		if( character->GetMeshByIndex(i)->IsVisible() )
		{
			indexLocCount += character->GetMeshByIndex(i)->GetTrianglesCount() * 3;
		}
	}

	if( character->GetMeshesCount() > 0 )
	{

		//new index list
		m_cdData->vertex_indices_list  = new int[indexLocCount];

		if( !m_cdData->vertex_indices_list )
			return false;

		m_cdData->vertex_indices_count = indexLocCount;

		indexLocCount = 0;

		//copy index data merging all the visible indecies lists

		for( i = 0; i < character->GetMeshesCount(); i++ )
		{
			if( character->GetMeshByIndex(i)->IsVisible() )
			{
				//get index list and count
				indexLoc      = character->GetMeshByIndex(i)->GetTrianglesList();
				indexLocCount = character->GetMeshByIndex(i)->GetTrianglesCount() * 3;

				//create temporary 
				indexTemp = new int[indexLocCount];

				if( !indexTemp )
					return false;

				//copy data to temp list
				for( j = 0; j < indexLocCount; j++ )
					indexTemp[j] = indexLoc[j];

				if( iPrevMeshIndex != -1 )
				{
					//calc buffer offset
					offsetGlob += character->GetMeshByIndex(iPrevMeshIndex)->GetTrianglesCount() * 3;		
					offsetLoc  += character->GetMeshByIndex(iPrevMeshIndex)->GetVerteciesCount();
	      
					//move indices in temp buffer
					for( j = 0; j < indexLocCount; j++ )
						indexTemp[j] += offsetLoc;
				}

				//copy indices from temp buffer to resulting index list
				for( j = 0; j < indexLocCount; j++ )
				m_cdData->vertex_indices_list[j + offsetGlob] = indexTemp[j];

				iPrevMeshIndex = i;

				SAFE_DELETE_ARRAY( indexTemp );
			}
		}
		/***************************END* create vertex indices list *********************/


		/******************************* create vertecies list **************************/
		int			vertexGlobCount = 0;
		int			vertexLocCount  = 0;
		CVERTEX**	vertList        = NULL;
		CVERTEX_UV* secondaryUV		= NULL;
		int			frmCounter      = 0;

		//count num of indices in all the frames
		for( i = 0; i < character->GetMeshesCount(); i++ )
		{
			if( character->GetMeshByIndex(i)->IsVisible() )
			{
				vertexGlobCount += character->GetMeshByIndex(i)->GetVerteciesCount(); 
			}
		}

		vertexGlobCount *= character->GetFramesCount();
		m_cdData->anim_frames_count = character->GetFramesCount();

		m_cdData->vertecies_per_frame_count = vertexGlobCount / character->GetFramesCount();

		//create complete vertex list
		m_cdData->posList	= new CVERTEX_XYZ[vertexGlobCount];
		m_cdData->normList	= new CVERTEX_XYZ[vertexGlobCount];
		m_cdData->uvList	= new CVERTEX_UV[m_cdData->vertecies_per_frame_count];
		m_cdData->uvList2	= new CVERTEX_UV[m_cdData->vertecies_per_frame_count];
		m_cdData->vcolList	= new long[m_cdData->vertecies_per_frame_count];

		if( !m_cdData->posList || !m_cdData->normList || !m_cdData->uvList || !m_cdData->vcolList )
			return false;

		m_cdData->vertecies_count = vertexGlobCount;

		for( frmCounter = 0; frmCounter < character->GetFramesCount(); frmCounter++ )
		{
			offsetGlob     = 0;
			iPrevMeshIndex = -1;

			for( i = 0; i < character->GetMeshesCount(); i++ )
			{
				if( character->GetMeshByIndex(i)->IsVisible() )
				{
					vertList		= character->GetMeshByIndex(i)->GetVerteciesList();
					vertexLocCount	= character->GetMeshByIndex(i)->GetVerteciesCount();
					secondaryUV		= character->GetMeshByIndex(i)->GetSecondaryUV();

					if (iPrevMeshIndex != -1)
						offsetGlob += character->GetMeshByIndex(iPrevMeshIndex)->GetVerteciesCount();

					//copy indices from temp buffer to resulting index list
					for( j = 0; j < vertexLocCount; j++ )
					{
						int v_idx = (frmCounter * m_cdData->vertecies_per_frame_count) + j + offsetGlob;
						m_cdData->posList[ v_idx ].x	= vertList[frmCounter][j].x;
						m_cdData->posList[ v_idx ].y	= vertList[frmCounter][j].y;
						m_cdData->posList[ v_idx ].z	= vertList[frmCounter][j].z;
						
						m_cdData->normList[ v_idx ].x	= vertList[frmCounter][j].nx;
						m_cdData->normList[ v_idx ].y	= vertList[frmCounter][j].ny;
						m_cdData->normList[ v_idx ].z	= vertList[frmCounter][j].nz;
					}

					for( j = 0; j < vertexLocCount; j++ )
					{
						m_cdData->uvList[ j + offsetGlob ].u	= vertList[0][j].u;
						m_cdData->uvList[ j + offsetGlob ].v	= vertList[0][j].v;
						m_cdData->vcolList[ j + offsetGlob ]	= vertList[0][j].color;

						if( secondaryUV )
						{
							m_cdData->uvList2[ j + offsetGlob ].u	= secondaryUV[j].u;
							m_cdData->uvList2[ j + offsetGlob ].v	= secondaryUV[j].v;
						}
					}

					//uvList2
					iPrevMeshIndex = i;
				}
			}
		}
		/***************************END* create vertecies list **************************/
	}
	else // no meshes
	{
		m_cdData->anim_frames_count = character->GetFramesCount();
		m_cdData->vertecies_count = 0;
		m_cdData->vertecies_per_frame_count = 0;
	}


	/******************************* create texture path ****************************/
	if(character->GetTexturePath().empty() == false)
	{
		m_strTexturePath = character->GetTexturePath();
	}
	/***************************END* create texture path ****************************/


	/******************************* create z-offsets list  *************************/
	m_cdData->z_coord_offset_list = new D3DXVECTOR3[GetFramesCount()];

	for( i = 0; i < GetFramesCount(); i++ )
		m_cdData->z_coord_offset_list[i].z = 0.0f;
	/***************************END* create z-offsets list  *************************/


	/******************************* create aabb list  ********************************/
	//create aabb list
	m_aabbList = new D3DXVECTOR3*[ GetFramesCount() ];

	for( int i = 0; i < GetFramesCount(); i++)
		m_aabbList[i] = new D3DXVECTOR3[8];
	
	CalcAABBs();
	/***************************END* create aabb list  ******************************/


	/******************************* create bounding spheres list  ******************/
	m_pBoundingSpheres = new CSPHERE[ GetFramesCount() ];
	CalcBoundingSpheres();
	/***************************END* create bounding spheres list  ****************/


	/**************************************** copy nulls list  **************************/
	for( int i = 0; i < character->GetNullsCount(); i++)
	{
		for( int j = 0; j < GetFramesCount(); j++ )
		{
			ClampAngle( character->GetNullByIndex( i )->matrices[j].rot.x );
			ClampAngle( character->GetNullByIndex( i )->matrices[j].rot.y );
			ClampAngle( character->GetNullByIndex( i )->matrices[j].rot.z );
		}

		m_cdData->vecNulls.push_back( *character->GetNullByIndex( i ) );
	}
	/**********************************END* copy nulls list  **************************/

	
	/**************************************** copy custom params  **************************/
	for( int i = 0; i < character->GetCustParamsCount(); i++ )
		m_cdData->vecCustomParams.push_back( *character->GetCustParamByIndex( i ) );
	/**************************************** copy custom params  *END**********************/


	m_cdData->reverse_index = NULL;

	CalcReverseIndex();
	PrecalculateNormals();

	m_cdData->global_radius = 1.0f;

	if( GetFramesCount() > 0 )
		m_cdData->anim_frame_scripts = new str_t[GetFramesCount()];

	m_cdData->fFPS  = 24.0f;

	return true;
}

CCUSTPARAM* CEfnCharacter::GetCustParamByName( str_t strName )
{
	for( int i = 0; i < GetCustParamsCount(); i++ )
	{
		str_t strTemp = m_cdData->vecCustomParams[i].id;

		if( _2Upper( strTemp ) == _2Upper( strName ) )
			return &m_cdData->vecCustomParams[i];
	}

	return NULL; 
}

bool CEfnCharacter::IsCustParamAnimated( str_t strCustParamID )
{
	CCUSTPARAM*	par = GetCustParamByName( strCustParamID );

	for( int i = 1; i < GetFramesCount(); i++ )
	{
		if( par->data[0] != par->data[i] )
			return true;
	}

	return false;
}

bool CEfnCharacter::IsCustParamAnimated( int iIndex )
{
	CCUSTPARAM*	par = GetCustParamByIndex( iIndex );

	for( int i = 1; i < GetFramesCount(); i++ )
	{
		if( par->data[0] != par->data[i] )
			return true;
	}

	return false;
}

bool CEfnCharacter::IntersectOOBB( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation )
{
	float min_len = -1.0f;
	float dist[12];


	D3DXVECTOR3		vec0 = m_aabbList[m_iCurrentFrame][0];
	D3DXVECTOR3		vec1 = m_aabbList[m_iCurrentFrame][1];
	D3DXVECTOR3		vec2 = m_aabbList[m_iCurrentFrame][2];
	D3DXVECTOR3		vec3 = m_aabbList[m_iCurrentFrame][3];
	D3DXVECTOR3		vec4 = m_aabbList[m_iCurrentFrame][4];
	D3DXVECTOR3		vec5 = m_aabbList[m_iCurrentFrame][5];
	D3DXVECTOR3		vec6 = m_aabbList[m_iCurrentFrame][6];
	D3DXVECTOR3		vec7 = m_aabbList[m_iCurrentFrame][7];

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

bool CEfnCharacter::IntersectBSphere( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation )
{
	CVector		orig, dir;
	CSPHERE		sphere_res;
	D3DXVECTOR3	sph_pos	= m_pBoundingSpheres[m_iCurrentFrame].pos;

	D3DXVec3TransformCoord( &sph_pos, &sph_pos, &mTransformation );

	sphere_res.radius	= m_pBoundingSpheres[m_iCurrentFrame].radius;
	sphere_res.pos		= sph_pos;

	float dist = CalcRaySphereIntersection( sphere_res, &pFrom, &pTo );

	if( dist != -1.0f )
		return true;

	return false;
}

float CEfnCharacter::IntersectPolys( D3DXVECTOR3& pFrom, D3DXVECTOR3& pTo, const D3DXMATRIX& mTransformation )
{
	float res = 999999.0f;

	for( int i = 0; i < m_cdData->vertex_indices_count / 3; i++ )
	{
		CVERTEX_XYZ v1 = m_cdData->posList[ m_iCurrentFrame * m_cdData->vertecies_per_frame_count + m_cdData->vertex_indices_list[i * 3 + 0] ];
		CVERTEX_XYZ v2 = m_cdData->posList[ m_iCurrentFrame * m_cdData->vertecies_per_frame_count + m_cdData->vertex_indices_list[i * 3 + 1] ];
		CVERTEX_XYZ v3 = m_cdData->posList[ m_iCurrentFrame * m_cdData->vertecies_per_frame_count + m_cdData->vertex_indices_list[i * 3 + 2] ];

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

void CEfnCharacter::CalcReverseIndex()
{
	KillReverseIndex();

	//////////////////////////////////////////////////
	//			create triangles index cache		//
	//////////////////////////////////////////////////
	// cache structure - 3d array of ints
	// [] - index of vertex
	// [] - list of triangles consisting this spesific vertex, [0] consits num of items of this level
	int				iTriCount	= m_cdData->vertex_indices_count / 3;
	int				iVertCount	= m_cdData->vertecies_per_frame_count;
	int*			iIndicies	= m_cdData->vertex_indices_list;
	int**			ppRI		= NULL;
	CVERTEX_XYZ*	pPos		= m_cdData->posList;
	CVERTEX_UV*		pUv			= m_cdData->uvList;
	
	//build cache
	m_cdData->reverse_index = new int*[iVertCount];
	ppRI					= m_cdData->reverse_index;

	//go throught all the vertices, find out which tris belong to each of the vertices
	for( int i = 0; i < iVertCount; i++ )
	{
		vector<int>			temp;
		vector<int>			tempStitch;
		vector<CVERTEX_XYZ> tempNorm;

		//run through tri index
		for( int j = 0; j < iTriCount; j++ )
		{
			if( iIndicies[j * 3 + 0] == i || iIndicies[j * 3 + 1] == i || iIndicies[j * 3 + 2] == i )
			{
				bool found = false;
						
				CVERTEX_XYZ vC, vB, normal;

				vC.x = pPos[iIndicies[j * 3 + 2]].x - pPos[iIndicies[j * 3 + 0]].x;
				vC.y = pPos[iIndicies[j * 3 + 2]].y - pPos[iIndicies[j * 3 + 0]].y;
				vC.z = pPos[iIndicies[j * 3 + 2]].z - pPos[iIndicies[j * 3 + 0]].z;

				vB.x = pPos[iIndicies[j * 3 + 1]].x - pPos[iIndicies[j * 3 + 0]].x;
				vB.y = pPos[iIndicies[j * 3 + 1]].y - pPos[iIndicies[j * 3 + 0]].y;
				vB.z = pPos[iIndicies[j * 3 + 1]].z - pPos[iIndicies[j * 3 + 0]].z;

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

		if( iTriCount * 3 != iVertCount )
		{
			for( int u = 0; u < iTriCount; u++ )
			{
				if( ( pPos[iIndicies[u * 3 + 0]] == pPos[i] && pUv[iIndicies[u * 3 + 0]] != pUv[i] ) ||
					( pPos[iIndicies[u * 3 + 1]] == pPos[i] && pUv[iIndicies[u * 3 + 1]] != pUv[i] ) ||
					( pPos[iIndicies[u * 3 + 2]] == pPos[i] && pUv[iIndicies[u * 3 + 2]] != pUv[i] ) )
				{
					for( int k = 0; k < iTriCount; k++ )
					{
						if( pPos[iIndicies[k * 3 + 0]] ^ pPos[i] || pPos[iIndicies[k * 3 + 1]] ^ pPos[i] || pPos[iIndicies[k * 3 + 2]] ^ pPos[i] )
						{
							bool found = false;
							
							CVERTEX_XYZ vC, vB, normal;

							vC.x = pPos[iIndicies[k * 3 + 2]].x - pPos[iIndicies[k * 3 + 0]].x;
							vC.y = pPos[iIndicies[k * 3 + 2]].y - pPos[iIndicies[k * 3 + 0]].y;
							vC.z = pPos[iIndicies[k * 3 + 2]].z - pPos[iIndicies[k * 3 + 0]].z;

							vB.x = pPos[iIndicies[k * 3 + 1]].x - pPos[iIndicies[k * 3 + 0]].x;
							vB.y = pPos[iIndicies[k * 3 + 1]].y - pPos[iIndicies[k * 3 + 0]].y;
							vB.z = pPos[iIndicies[k * 3 + 1]].z - pPos[iIndicies[k * 3 + 0]].z;

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
		ppRI[i] = new int[temp.size() + 1];
		ppRI[i][0] = temp.size();

		for( UINT w = 0; w < temp.size(); w++ )
			ppRI[i][w + 1] = temp[w];
	}
}

void CEfnCharacter::CalcBoundingSpheres()
{
	if( m_cdData->vertecies_count <= 0 )
		return;

	//DWORD			NumVertices;
	//D3DXVECTOR3*	pFirstPosition;

	//D3DXVECTOR3 pCenter;
	//FLOAT				pRadius;

	//int starting_index	= 0;
	//int finishing_index	= 0;
	//
	//pFirstPosition	= new D3DXVECTOR3[m_cdData->vertecies_per_frame_count];
	//NumVertices		= m_cdData->vertecies_per_frame_count;

	//for( int FrmCount = 0; FrmCount < m_cdData->anim_frames_count; FrmCount++ )
	//{
	//	starting_index	= FrmCount * m_cdData->vertecies_per_frame_count;
	//	finishing_index	= starting_index + m_cdData->vertecies_per_frame_count;

	//	for( int i = starting_index; i < finishing_index; i++)
	//	{
	//		pFirstPosition[i - starting_index].x = m_cdData->vertecies_list[i].x;
	//		pFirstPosition[i - starting_index].y = m_cdData->vertecies_list[i].y;
	//		pFirstPosition[i - starting_index].z = m_cdData->vertecies_list[i].z;
	//	}

	//	D3DXComputeBoundingSphere( pFirstPosition, NumVertices, sizeof( D3DXVECTOR3 ), &pCenter, &pRadius );

	//	m_pBoundingSpheres[FrmCount].radius	= pRadius;
	//	m_pBoundingSpheres[FrmCount].pos.x	= pCenter.x;
	//	m_pBoundingSpheres[FrmCount].pos.y	= pCenter.y;
	//	m_pBoundingSpheres[FrmCount].pos.z	= pCenter.z;
	//}

	//SAFE_DELETE_ARRAY( pFirstPosition );

	// more correct bounding sphere calculation /* CAN BE REALLY SLOW!!! */
	for( int FrmCount = 0; FrmCount < GetFramesCount(); FrmCount++ )
	{
		int				starting_index	= FrmCount * m_cdData->vertecies_per_frame_count;
		int				finishing_index	= starting_index + m_cdData->vertecies_per_frame_count;
		CVector		one;
		CVector		two, two_temp;
		CVector		temp2;
		D3DXVECTOR3 center, one_temp, temp;
		float			length = 0.0f;

		// let's find center of the model from aabb 
		center.x = (m_aabbList[FrmCount][1].x - m_aabbList[FrmCount][0].x) / 2.0f + m_aabbList[FrmCount][0].x;
		center.y = (m_aabbList[FrmCount][0].y - m_aabbList[FrmCount][4].y) / 2.0f + m_aabbList[FrmCount][4].y;
		center.z = (m_aabbList[FrmCount][0].z - m_aabbList[FrmCount][3].z) / 2.0f + m_aabbList[FrmCount][3].z;

		for( int i = starting_index; i < finishing_index; i++)
		{
			one_temp.x = m_cdData->posList[i].x;
			one_temp.y = m_cdData->posList[i].y;
			one_temp.z = m_cdData->posList[i].z;

			temp = center - one_temp;

			if( length < D3DXVec3Length( &temp ) )
				length = D3DXVec3Length( &temp );
		}

		m_pBoundingSpheres[FrmCount].radius	= length;
		m_pBoundingSpheres[FrmCount].pos		= center;
	}
}

void CEfnCharacter::CalcAABBs()
{
	for( int frm_counter = 0; frm_counter < GetFramesCount(); frm_counter++ )
	{
		D3DXVECTOR3 minV( 0, 0, 0 );
		D3DXVECTOR3 maxV( 0, 0, 0 );

		if( m_cdData->vertecies_count > 0 )
		{
			int starting_indx	= frm_counter * m_cdData->vertecies_per_frame_count;
			int finishing_indx	= starting_indx + m_cdData->vertecies_per_frame_count;

			minV.x	= m_cdData->posList[starting_indx].x;
			minV.y	= m_cdData->posList[starting_indx].y;
			minV.z	= m_cdData->posList[starting_indx].z;

			maxV.x	= m_cdData->posList[starting_indx].x;
			maxV.y	= m_cdData->posList[starting_indx].y;
			maxV.z	= m_cdData->posList[starting_indx].z;

			for( int i = starting_indx + 1; i < finishing_indx; i++)
			{
				if( minV.x > m_cdData->posList[i].x )
					minV.x = m_cdData->posList[i].x;

				if( minV.y > m_cdData->posList[i].y )
					minV.y = m_cdData->posList[i].y;

				if( minV.z > m_cdData->posList[i].z )
					minV.z = m_cdData->posList[i].z;

				if( maxV.x < m_cdData->posList[i].x )
					maxV.x = m_cdData->posList[i].x;

				if( maxV.y < m_cdData->posList[i].y )
					maxV.y = m_cdData->posList[i].y;

				if( maxV.z < m_cdData->posList[i].z )
					maxV.z = m_cdData->posList[i].z;
			}
		}

		m_aabbList[frm_counter][0] = D3DXVECTOR3( (float)minV.x, (float)maxV.y, (float)maxV.z );
		m_aabbList[frm_counter][1] = D3DXVECTOR3( (float)maxV.x, (float)maxV.y, (float)maxV.z ); // max
		m_aabbList[frm_counter][2] = D3DXVECTOR3( (float)maxV.x, (float)maxV.y, (float)minV.z );
		m_aabbList[frm_counter][3] = D3DXVECTOR3( (float)minV.x, (float)maxV.y, (float)minV.z );
		m_aabbList[frm_counter][4] = D3DXVECTOR3( (float)minV.x, (float)minV.y, (float)maxV.z );
		m_aabbList[frm_counter][5] = D3DXVECTOR3( (float)maxV.x, (float)minV.y, (float)maxV.z );
		m_aabbList[frm_counter][6] = D3DXVECTOR3( (float)maxV.x, (float)minV.y, (float)minV.z );
		m_aabbList[frm_counter][7] = D3DXVECTOR3( (float)minV.x, (float)minV.y, (float)minV.z ); // min
	}
}

void CEfnCharacter::RotateZBy90()
{
	D3DXVECTOR3 tmp;
	D3DXMATRIX rotZ;

	D3DXMatrixRotationZ( &rotZ, D3DX_PI / 2.0f );

	//transform vertecies
	for( int i = 0; i < m_cdData->vertecies_count; i++ )
	{
		tmp.x = m_cdData->posList[i].x;
		tmp.y = m_cdData->posList[i].y;
		tmp.z = m_cdData->posList[i].z;
		
		D3DXVec3TransformCoord( &tmp, &tmp, &rotZ );

		m_cdData->posList[i].x = tmp.x;
		m_cdData->posList[i].y = tmp.y;
		m_cdData->posList[i].z = tmp.z;

		tmp.x = m_cdData->normList[i].x;
		tmp.y = m_cdData->normList[i].y;
		tmp.z = m_cdData->normList[i].z;
		
		D3DXVec3TransformCoord( &tmp, &tmp, &rotZ );

		m_cdData->normList[i].x = tmp.x;
		m_cdData->normList[i].y = tmp.y;
		m_cdData->normList[i].z = tmp.z;
	}

	//recalc aabbs and bspheres
	CalcAABBs();
	CalcBoundingSpheres();

	//transform nulls
	for( int i = 0; i < GetNullsCount(); i++ )
	{
		for( int j = 0; j < GetFramesCount(); j++ )
		{
			m_cdData->vecNulls[i].matrices[j].pre_transform *= rotZ;
		}
	}

	for( int i = 0; i < GetFramesCount(); i++ )
	{
		m_cdData->z_coord_offset_list[i].x = fabs( m_cdData->z_coord_offset_list[i].x );
		m_cdData->z_coord_offset_list[i].y = fabs( m_cdData->z_coord_offset_list[i].y );
		m_cdData->z_coord_offset_list[i].z = fabs( m_cdData->z_coord_offset_list[i].z );
	}

	SAFE_DELETE_ARRAY( m_cdData->buffshorts );
}

void CEfnCharacter::InitData(void)
{
	m_cdData->anim_frames_count			= 0;
	m_cdData->vertecies_count			= 0;
	m_cdData->vertecies_per_frame_count	= 0;
	m_cdData->vertex_indices_count		= 0;
	m_cdData->name						= ""; 

	m_cdData->posList					= NULL;
	m_cdData->normList					= NULL;
	m_cdData->uvList					= NULL;
	m_cdData->vcolList					= NULL;

	m_cdData->vertex_indices_list		= NULL;
	m_cdData->z_coord_offset_list		= NULL;
	m_cdData->global_radius				= 0.0f;
	m_cdData->anim_frame_scripts		= NULL;

	ZeroMemory( &m_cdData->material, sizeof( D3DMATERIAL9 ) );

	m_cdData->material.Diffuse = D3DXCOLOR( 0xffffffff );
	m_cdData->material.Ambient = D3DXCOLOR( 0xffffffff );
}

//free memory
void CEfnCharacter::KillData()
{
	if( !m_cdData )
		return;

	SAFE_DELETE_ARRAY( m_cdData->anim_frame_scripts );
	SAFE_DELETE_ARRAY( m_cdData->uvList2 );

	// kill reverse index
	if( m_cdData->reverse_index )
	{
		for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
			SAFE_DELETE_ARRAY( m_cdData->reverse_index[i] );

		SAFE_DELETE_ARRAY( m_cdData->reverse_index );
	}

	//kill aabb list
	if( m_aabbList )
	{
		for( int i = 0; i < GetFramesCount(); i++)
			SAFE_DELETE_ARRAY( m_aabbList[i] );

		SAFE_DELETE_ARRAY( m_aabbList );
	}

	/*CCUSTPARAM* pPar = NULL;

	for( UINT i = 0; i < m_cdData->vecCustomParams.size(); i++ )
	{
		pPar = &(m_cdData->vecCustomParams[i]);
		SAFE_DELETE( pPar );
	}*/

	SAFE_DELETE_ARRAY( m_cdData->buffshorts );

	m_cdData->anim_frames_count			= 0;
	m_cdData->vertecies_count			= 0;
	m_cdData->vertecies_per_frame_count	= 0;
	m_cdData->vertex_indices_count		= 0;
	m_cdData->global_radius				= 0.0f;

	DeleteAllAnimations();

	SAFE_DELETE_ARRAY( m_cdData->posList );
	SAFE_DELETE_ARRAY( m_cdData->normList );
    SAFE_DELETE_ARRAY( m_cdData->uvList );
	SAFE_DELETE_ARRAY( m_cdData->vcolList );

	SAFE_DELETE_ARRAY( m_cdData->vertex_indices_list );
	SAFE_DELETE_ARRAY( m_cdData->vertex_indices_list2 );
	SAFE_DELETE_ARRAY( m_cdData->z_coord_offset_list );
	SAFE_DELETE_ARRAY( m_pBoundingSpheres );
}

void CEfnCharacter::CreateLod( int iIndeciesCount, int iVerteciesCount )
{
	if( !m_cdData )
		return;

	m_cdData->vertex_indices_count2			= iIndeciesCount;
	m_cdData->vertecies_per_frame_count2	= iVerteciesCount;
	m_cdData->vertex_indices_list2 = new int[iIndeciesCount];
}

void CEfnCharacter::AddLodFromAnimEfn( CEfnCharacter* pEfnLOD, LPDIRECT3DDEVICE9 pd3dDevice )
{
	int* lodIdx				= pEfnLOD->GetIndicesList();
	CVERTEX_XYZ* lodVert	= pEfnLOD->GetVerteciesList();
	int	 lodTriCount		= pEfnLOD->GetTrianglesCount();
	int	 lodVertCount		= pEfnLOD->GetVerteciesCount();

	CreateLod( lodTriCount * 3, lodVertCount );

	CVERTEX_XYZ* pVert		= GetVerteciesList();
	int*		 pIdx2		= GetIndicesList2();

	int sdf = 0;

	float valid = 0.00001f;

	for( int j = 0; j < lodVertCount; j++ )
	{
		for( int i = 0; i < GetVerteciesCount(); i++ )
		{
			//if( pVert[i] ^ lodVert[j] )
			if( ( fabs(pVert[i].x - lodVert[j].x) < valid ) && (fabs(pVert[i].y - lodVert[j].y ) < valid ) && (fabs(pVert[i].z - lodVert[j].z ) < valid ) )
			{
				sdf++;
				
				for( int k = 0; k < lodTriCount * 3; k++ )
				{
					if( lodIdx[k] == j )
					{
						pIdx2[k] = i;
						//break;
					}
				}

				break;
			}
		}
	}

	//for( int i = 0; i < lodTriCount * 3; i++ )
	//pIdx2[i] = 0;

	m_bLodExists = true;

	/******************************* create index buffer ****************************/
	SAFE_RELEASE( m_pIB2 );

	VOID* pIndices = NULL;

	//create buffer
	if( m_cdData->vertex_indices_count2 != 0 )
	{
		if( FAILED( pd3dDevice->CreateIndexBuffer( sizeof(int) * m_cdData->vertex_indices_count2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIB2, NULL ))) 
			return;

		//lock buffer
		if( FAILED( m_pIB2->Lock(0, sizeof(int) * m_cdData->vertex_indices_count2, (void**)&pIndices, 0)))
			return;

		memcpy( pIndices, m_cdData->vertex_indices_list2, sizeof(int) * m_cdData->vertex_indices_count2 );

		//unlock buffer
		if( FAILED(m_pIB2->Unlock()) )
			return;
	}
	/*****************************END* create index buffer **************************/
}

bool CEfnCharacter::Prepare( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !pd3dDevice || !m_cdData || GetFramesCount() <= 0 )
		return false;

	VOID*	pVertices;
	VOID*	pIndices;

	/******************************* create index buffer ****************************/
	SAFE_RELEASE( m_pIB );

	//create buffer
	if( m_cdData->vertex_indices_count != 0 )
	{
		if( FAILED( pd3dDevice->CreateIndexBuffer( sizeof(int) * m_cdData->vertex_indices_count, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIB, NULL ))) 
			return false;

		//lock buffer
		if( FAILED( m_pIB->Lock(0, sizeof(int) * m_cdData->vertex_indices_count, (void**)&pIndices, 0)))
			return false;

		memcpy( pIndices, m_cdData->vertex_indices_list, sizeof(int) * m_cdData->vertex_indices_count );

		//unlock buffer
		if( FAILED(m_pIB->Unlock()) )
			return false;
	}
	/*****************************END* create index buffer **************************/

	if( m_cdData->vertecies_count > 0 )
	{
		/******************************* create pos-norm-uv buffers **************************/
		SAFE_DELETE_ARRAY3D( m_pPosBuff, GetFramesCount() );
		SAFE_DELETE_ARRAY3D( m_pNormBuff, GetFramesCount() );
		SAFE_RELEASE( m_pUvBuff );
		SAFE_RELEASE( m_pUvBuff2 );
		SAFE_RELEASE( m_pVColorBuff );

		// allocate array of buffers, one for each frame

		m_pPosBuff	= new LPDIRECT3DVERTEXBUFFER9[ GetFramesCount() ];
		m_pNormBuff	= new LPDIRECT3DVERTEXBUFFER9[ GetFramesCount() ];

		//prepare temp buffers
		CVERTEX_XYZ*		pData		= NULL;
		CVERTEX_XYZ*		posArray	= m_cdData->posList;
		CVERTEX_XYZ*		normArray	= m_cdData->normList;
		CVERTEX_UV*			uvArray		= m_cdData->uvList;
		CVERTEX_UV*			uvArray2	= m_cdData->uvList2;
		long*				vcolArray	= m_cdData->vcolList;
		unsigned int		xyzSize		= sizeof( CVERTEX_XYZ ) * m_cdData->vertecies_per_frame_count;
		unsigned int		uvSize		= sizeof( CVERTEX_UV ) * m_cdData->vertecies_per_frame_count;
		unsigned int		vcolSize	= sizeof( long ) * m_cdData->vertecies_per_frame_count;

		//create buffers
		for( int i = 0; i < GetFramesCount(); i++ )
		{
			//pos
			pd3dDevice->CreateVertexBuffer( xyzSize, D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_MANAGED, &m_pPosBuff[i], NULL );
			m_pPosBuff[i]->Lock( 0, xyzSize, (void**)&pVertices, 0 );
			pData = posArray;
			pData += i * m_cdData->vertecies_per_frame_count;
			memcpy((CVERTEX_XYZ*)pVertices, pData, xyzSize );
			m_pPosBuff[i]->Unlock();
			
			//norm
			pd3dDevice->CreateVertexBuffer( xyzSize, D3DUSAGE_WRITEONLY, D3DFVF_NORMAL, D3DPOOL_MANAGED, &m_pNormBuff[i], NULL );
			m_pNormBuff[i]->Lock( 0, xyzSize, (void**)&pVertices, 0 );
			pData = normArray;
			pData += i * m_cdData->vertecies_per_frame_count;
			memcpy((CVERTEX_XYZ*)pVertices, pData, xyzSize );
			m_pNormBuff[i]->Unlock();
		}

		//uv
		pd3dDevice->CreateVertexBuffer( uvSize, D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0), D3DPOOL_MANAGED, &m_pUvBuff, NULL );
		m_pUvBuff->Lock( 0, uvSize, (void**)&pVertices, 0 );
		memcpy((CVERTEX_UV*)pVertices, uvArray, uvSize );
		m_pUvBuff->Unlock();

		//uv2
		pd3dDevice->CreateVertexBuffer( uvSize, D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0), D3DPOOL_MANAGED, &m_pUvBuff2, NULL );
		m_pUvBuff2->Lock( 0, uvSize, (void**)&pVertices, 0 );
		memcpy((CVERTEX_UV*)pVertices, uvArray2, uvSize );
		m_pUvBuff2->Unlock();

		//vertex color
		pd3dDevice->CreateVertexBuffer( vcolSize , D3DUSAGE_WRITEONLY, D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &m_pVColorBuff, NULL );
		m_pVColorBuff->Lock( 0, vcolSize, (void**)&pVertices, 0 );
		memcpy((long*)pVertices, vcolArray, vcolSize );
		m_pVColorBuff->Unlock();
		/**************************END* create pos-norm-uv buffers **************************/



		/******************************* create vertex declaration ******************************/
		SAFE_RELEASE( m_pVertexDeclaration );

		//create vertex declaration
		D3DVERTEXELEMENT9 tween_decl_ve[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
			{ 3, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
			{ 4, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 5, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			D3DDECL_END()
		};

		pd3dDevice->CreateVertexDeclaration( tween_decl_ve, &m_pVertexDeclaration );
		/************************END* create vertex declaration *******************************/

		/********************************* create aabbs buffers *********************************/
		SAFE_DELETE_ARRAY3D( m_pVBLines, GetFramesCount() );

		if( GetFramesCount() > 0 )
		{
			m_pVBLines = new LPDIRECT3DVERTEXBUFFER9[ GetFramesCount() ];

			long	sz = 24 * sizeof(CVERTEX);
			int		lines_idx[] = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };

			for(int frm_cnt = 0; frm_cnt < GetFramesCount(); frm_cnt++ )
			{
				CVERTEX* lines_data  = new CVERTEX[24];

				//create lines
				for(int i = 0; i < 24; i++)
				{
					lines_data[i].x = m_aabbList[frm_cnt][ lines_idx[i] ].x;
					lines_data[i].y = m_aabbList[frm_cnt][ lines_idx[i] ].y;
					lines_data[i].z = m_aabbList[frm_cnt][ lines_idx[i] ].z;

					lines_data[i].color = 0x00FFFFFF;
				}

				pd3dDevice->CreateVertexBuffer( sz, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBLines[frm_cnt], NULL );
				pVertices = NULL;
				
				//lock buffer
				if( FAILED( m_pVBLines[frm_cnt]->Lock( 0, sz, (void**)&pVertices, 0 ) ) )
					return false;

				memcpy( (D3DXVECTOR3 *)pVertices, lines_data, sz );

				m_pVBLines[frm_cnt]->Unlock();

				SAFE_DELETE_ARRAY( lines_data );
			}
		}
		/*****************************END* create aabbs buffers *********************************/


		/***************************** create bounding spheres buffers ********************************/
		SAFE_DELETE_ARRAY3D( ppMesh, GetFramesCount() );

		if( GetFramesCount() > 0 )
		{
			ppMesh = new LPD3DXMESH[GetFramesCount()];

			if( m_pBoundingSpheres )
			{
				for(int frm_cnt = 0; frm_cnt < GetFramesCount(); frm_cnt++ )
				{
					D3DXCreateSphere( pd3dDevice, m_pBoundingSpheres[frm_cnt].radius, 30, 30, &ppMesh[frm_cnt], NULL );
				}
			}
		}
		/************************END** create bounding spheres buffers ********************************/

		/******************************* create texture *********************************/
		//str_t path;

		//if (m_cdData->texture_filename.length() > 0 )
		//{
		//	path.append( this->GetFolder() );
		//	path.append(m_cdData->texture_filename);
		//	m_strTexturePath = path.data();

		//	SAFE_RELEASE(m_pTexture);

		//	D3DXCreateTextureFromFile(pd3dDevice, m_strTexturePath.GetBuffer(0), &m_pTexture);
		//}
		/***************************END* create texture *********************************/
	} // if( m_cdData->vertecies_count > 0 )

	//prepare null icon
	PrepareNullsData( pd3dDevice );

	for( UINT i = 0; i < m_cdData->vecNulls.size(); i++ )
	{
		for( UINT j = 0; j < m_cdData->vecNulls[i].attaches.size(); j++ )
		{
			m_cdData->vecNulls[i].attaches[j]->Prepare( pd3dDevice );
		}
	}
	
	if( m_pNormalsCache )
		SAFE_DELETE_ARRAY( m_pNormalsCache );

	//calc normals cache
	m_pNormalsCache = new CVERTEX_XYZ[m_cdData->vertecies_count * 2];

	for( int i = 0; i < GetFramesCount(); i++ )
	{
		for( int j = 0; j < m_cdData->vertecies_per_frame_count; j++ )
		{
			int offset = m_cdData->vertecies_per_frame_count * i;

			CVERTEX_XYZ vecTmpPos	= m_cdData->posList[offset + j];
			CVERTEX_XYZ vecTmpNorm	= m_cdData->normList[offset + j];

			vecTmpNorm.x *= m_pBoundingSpheres[0].radius * 0.15f;
			vecTmpNorm.y *= m_pBoundingSpheres[0].radius * 0.15f;
			vecTmpNorm.z *= m_pBoundingSpheres[0].radius * 0.15f;

			vecTmpNorm.x += vecTmpPos.x;
			vecTmpNorm.y += vecTmpPos.y;
			vecTmpNorm.z += vecTmpPos.z;

			m_pNormalsCache[(offset + j) * 2]		= vecTmpPos;
			m_pNormalsCache[(offset + j) * 2 + 1]	= vecTmpNorm;
		}
	}

	if( m_pParent )
	{
		SAFE_RELEASE( m_pEffect );

		str_t strShaderPath = ((CCookieMachineDlg*)m_pParent)->GetAppDir() + "\\data\\shaders\\phong.fx";
		D3DXCreateEffectFromFile( pd3dDevice, strShaderPath.data(), NULL, NULL, 0, NULL, &m_pEffect, NULL );
	}

	return true;
}

void CEfnCharacter::ShowAABB( bool bShow )
{ 
	m_bIsAABBVisible = bShow; 
	
	for( UINT i = 0; i < m_cdData->vecNulls.size(); i++ )
	{
		for( UINT j = 0; j < m_cdData->vecNulls[i].attaches.size(); j++ )
			((CEfnCharacter*)m_cdData->vecNulls[i].attaches[j])->ShowAABB( bShow );
	}
}

void CEfnCharacter::ShowBoundingSphere( bool bShow )
{
	m_bIsBoundingSphereVisible = bShow;

	for( UINT i = 0; i < m_cdData->vecNulls.size(); i++ )
	{
		for( UINT j = 0; j < m_cdData->vecNulls[i].attaches.size(); j++ )
			((CEfnCharacter*)m_cdData->vecNulls[i].attaches[j])->ShowBoundingSphere( bShow );
	}
}

void CEfnCharacter::CreateTexture( LPDIRECT3DTEXTURE9 tex, LPDIRECT3DDEVICE9 dev )
{
	bool ok = SUCCEEDED( D3DXCreateTexture(	dev, 128, 128, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tex ) );

	if( ok )
	{
		D3DLOCKED_RECT rect;
		tex->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);

		D3DCOLOR* temp =  (D3DCOLOR*)rect.pBits;
		int pitch_size = rect.Pitch >> 2;

		for( int j = 0; j < 128; j++)
		{
			for( int i = 0; i < 128; i++)
			{
				temp[ j * pitch_size + i ] = 0xFFFF0000;
			}
		}

		tex->UnlockRect(0);
	}
	else
	{
		SAFE_RELEASE(tex);
	}
}

CNULL2* CEfnCharacter::GetNullByIndex( int index )	
{ 
	if( index < GetNullsCount() )			
	{
		return &m_cdData->vecNulls[index];		
	}

	return NULL;
}

CNULL2* CEfnCharacter::GetNullByName( str_t name )	
{ 
	for( int i = 0; i < GetNullsCount(); i++ )			
	{
		str_t emp = m_cdData->vecNulls[i].id;

		if( _2Upper( emp ) == _2Upper( name ) )
		{
			return &m_cdData->vecNulls[i];	
		}
	}

	return NULL;
}

void CEfnCharacter::DeleteMeshData()
{
	SAFE_RELEASE( m_pIB );
	SAFE_RELEASE( m_pIB2 );
	SAFE_DELETE_ARRAY3D( m_pPosBuff, GetFramesCount() );
	SAFE_DELETE_ARRAY3D( m_pNormBuff, GetFramesCount() );
	SAFE_RELEASE( m_pUvBuff );
	SAFE_RELEASE( m_pUvBuff2 );
	SAFE_RELEASE( m_pVColorBuff );

	SAFE_DELETE_ARRAY( m_cdData->posList );
	SAFE_DELETE_ARRAY( m_cdData->normList );
	SAFE_DELETE_ARRAY( m_cdData->buffshorts );
	SAFE_DELETE_ARRAY( m_cdData->uvList );
	SAFE_DELETE_ARRAY( m_cdData->uvList2 );
	SAFE_DELETE_ARRAY( m_cdData->vcolList );
	SAFE_DELETE_ARRAY( m_cdData->vertex_indices_list );
	SAFE_DELETE_ARRAY( m_cdData->vertex_indices_list2 );

	for( int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
		SAFE_DELETE_ARRAY( m_cdData->reverse_index[i] );

	SAFE_DELETE_ARRAY( m_cdData->reverse_index );

	m_cdData->vertecies_count = 0;
	m_cdData->vertecies_per_frame_count = 0;
	m_cdData->vertecies_per_frame_count2 = 0;
	m_cdData->vertex_indices_count = 0;
	m_cdData->vertex_indices_count2 = 0;
}

void CEfnCharacter::DeleteAllNulls()
{
	m_cdData->vecNulls.clear();	
}

void CEfnCharacter::DeleteNullByName( str_t name )
{
	for( int i = 0; i < GetNullsCount(); i++ )			
	{
		if( m_cdData->vecNulls[i].id == name )
		{
			m_cdData->vecNulls.erase( m_cdData->vecNulls.begin() + i );	
			return;
		}
	}
}

void CEfnCharacter::ScaleZOffsets( float fScaleFactor )
{
	if( !m_cdData || m_cdData->anim_frames_count <= 0 )
		return;

	for( int i = 0; i < m_cdData->anim_frames_count; i++ )
		m_cdData->z_coord_offset_list[i] *= fScaleFactor;
}

void CEfnCharacter::RotateBy180DegAboutY()
{
	D3DXVECTOR3 tmp;
	D3DXMATRIX rotY;

	D3DXMatrixRotationY( &rotY, D3DX_PI );

	//transform vertecies
	for( int i = 0; i < m_cdData->vertecies_count; i++ )
	{
		tmp.x = m_cdData->posList[i].x;
		tmp.y = m_cdData->posList[i].y;
		tmp.z = m_cdData->posList[i].z;
		
		D3DXVec3TransformCoord( &tmp, &tmp, &rotY );

		m_cdData->posList[i].x = tmp.x;
		m_cdData->posList[i].y = tmp.y;
		m_cdData->posList[i].z = tmp.z;

		tmp.x = m_cdData->normList[i].x;
		tmp.y = m_cdData->normList[i].y;
		tmp.z = m_cdData->normList[i].z;
		
		D3DXVec3TransformCoord( &tmp, &tmp, &rotY );

		m_cdData->normList[i].x = tmp.x;
		m_cdData->normList[i].y = tmp.y;
		m_cdData->normList[i].z = tmp.z;
	}

	//recalc aabbs and bspheres
	CalcAABBs();
	CalcBoundingSpheres();

	//transform nulls
	for( int i = 0; i < GetNullsCount(); i++ )
	{
		for( int j = 0; j < GetFramesCount(); j++ )
		{
			m_cdData->vecNulls[i].matrices[j].pre_transform *= rotY;
		}
	}

	for( int i = 0; i < GetFramesCount(); i++ )
	{
		m_cdData->z_coord_offset_list[i].x = fabs( m_cdData->z_coord_offset_list[i].x );
		m_cdData->z_coord_offset_list[i].y = fabs( m_cdData->z_coord_offset_list[i].y );
		m_cdData->z_coord_offset_list[i].z = fabs( m_cdData->z_coord_offset_list[i].z );
	}

	SAFE_DELETE_ARRAY( m_cdData->buffshorts );
}

void CEfnCharacter::CreateNewNull( str_t name )
{
	CNULL2 nl;

	nl.id = name;

	//read matrices data
	for( int i = 0; i < m_cdData->anim_frames_count; i++ )
	{
		CTRANSFORMEX trEx;

		trEx.scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		
		nl.matrices.push_back( trEx );
	}

	m_cdData->vecNulls.push_back( nl );
}

bool CEfnCharacter::PrepareNullsData( LPDIRECT3DDEVICE9 pd3dDevice )
{
	/* normal null */
	VOID*		pVertices;
	CVERTEX		lines_vertecies[6];

	ZeroMemory( lines_vertecies, sizeof(lines_vertecies) );

	SAFE_RELEASE( m_pVBNull );

	//prepare lines for drawing Nulls
	lines_vertecies[0].x = -1.0;
	lines_vertecies[0].y =	0.0;
	lines_vertecies[0].z = 0.0;
	lines_vertecies[0].color = 0x0000FF00;

	lines_vertecies[1].x = 1.0;
	lines_vertecies[1].y =	0.0;
	lines_vertecies[1].z = 0.0;
	lines_vertecies[1].color = 0x0000FF00;

	lines_vertecies[2].x = 0.0;
	lines_vertecies[2].y =	-1.0;
	lines_vertecies[2].z = 0.0;
	lines_vertecies[2].color = 0x0000FF00;

	lines_vertecies[3].x = 0.0;
	lines_vertecies[3].y =	1.0;
	lines_vertecies[3].z = 0.0;
	lines_vertecies[3].color = 0x0000FF00;

	lines_vertecies[4].x = 0.0;
	lines_vertecies[4].y =	0.0;
	lines_vertecies[4].z = -1.0;
	lines_vertecies[4].color = 0x0000FF00;

	lines_vertecies[5].x = 0.0;
	lines_vertecies[5].y =	0.0;
	lines_vertecies[5].z = 1.0;
	lines_vertecies[5].color = 0x0000FF00;

	long size = sizeof( lines_vertecies );

	pd3dDevice->CreateVertexBuffer( size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBNull, NULL );
	m_pVBNull->Lock( 0, size, (void**)&pVertices, 0 );
	memcpy( pVertices, lines_vertecies, size );
	m_pVBNull->Unlock();
	
	
	/* selected null */
	ZeroMemory( lines_vertecies, sizeof(lines_vertecies) );

	//prepare lines for drawing Nulls
	lines_vertecies[0].x = -1.0;
	lines_vertecies[0].y =	0.0;
	lines_vertecies[0].z = 0.0;
	lines_vertecies[0].color = 0x00FF0000;

	lines_vertecies[1].x = 1.0;
	lines_vertecies[1].y =	0.0;
	lines_vertecies[1].z = 0.0;
	lines_vertecies[1].color = 0x00FF0000;

	lines_vertecies[2].x = 0.0;
	lines_vertecies[2].y =	-1.0;
	lines_vertecies[2].z = 0.0;
	lines_vertecies[2].color = 0x00FF0000;

	lines_vertecies[3].x = 0.0;
	lines_vertecies[3].y =	1.0;
	lines_vertecies[3].z = 0.0;
	lines_vertecies[3].color = 0x00FF0000;

	lines_vertecies[4].x = 0.0;
	lines_vertecies[4].y =	0.0;
	lines_vertecies[4].z = -1.0;
	lines_vertecies[4].color = 0x00FF0000;

	lines_vertecies[5].x = 0.0;
	lines_vertecies[5].y =	0.0;
	lines_vertecies[5].z = 1.0;
	lines_vertecies[5].color = 0x00FF0000;

	pd3dDevice->CreateVertexBuffer( size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_DESCRIPTOR, D3DPOOL_MANAGED, &m_pVBSelectedNull, NULL );
	m_pVBSelectedNull->Lock( 0, size, (void**)&pVertices, 0 );
	memcpy(pVertices, lines_vertecies, size);
	m_pVBSelectedNull->Unlock();

	return true;
}

void CEfnCharacter::SetRenderNormals( bool bValue )
{ 
	for( UINT i = 0; i < m_cdData->vecNulls.size(); i++ )
	{
		if( m_cdData->vecNulls[i].attaches.empty() == false )
		{
			for( UINT j = 0; j < m_cdData->vecNulls[i].attaches.size(); j++ )
				((CEfnCharacter*)m_cdData->vecNulls[i].attaches[j])->SetRenderNormals( bValue );
		}
	}

	m_bRenderNormals = bValue; 
}

void CEfnCharacter::RenderNormals( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !pd3dDevice || !m_cdData || !m_bRenderNormals )
		return;

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetTexture( 0, NULL );
	SetVColorFromMaterial( true, pd3dDevice );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
	pd3dDevice->LightEnable( 0, true );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff0000ff );

	pd3dDevice->SetMaterial( &m_BlueMtrl );
	pd3dDevice->SetFVF( D3DFVF_XYZ );

	int offset = (m_cdData->vertecies_per_frame_count * 2) * m_iCurrentFrame;

	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, m_cdData->vertecies_per_frame_count, m_pNormalsCache + offset, sizeof(CVERTEX_XYZ) ); 

	//g_pd3dDevice->SetTransform( D3DTS_WORLD, &identity );
}

void CEfnCharacter::Render( LPDIRECT3DDEVICE9 pd3dDevice, bool bWireframeMode, float fTweenFactor, bool bUseMaterial )
{
	if( !pd3dDevice || !m_cdData )
		return;

	float icon_scale = 1.0f;

	if( m_pParent )
		icon_scale = ((CCookieMachineDlg*)m_pParent)->GetIconsScale();

	pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
	pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );

	DWORD amb_old, fill_old, light_old;
	pd3dDevice->GetRenderState( D3DRS_AMBIENT, &amb_old );
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &fill_old );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &light_old );
	pd3dDevice->LightEnable( 0, TRUE );	

	D3DXMATRIX matr_old_world;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );

	//enable tweening
	pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_TWEENING );

	//draw MESH
	if( m_cdData->vertex_indices_count > 0 && m_cdData->vertecies_count && m_pIB && m_pPosBuff && m_pNormBuff && m_pUvBuff && m_pVColorBuff && m_pVertexDeclaration )
	{
		if( !bWireframeMode )
		{
			pd3dDevice->SetTexture( 0, m_pTexture );
			pd3dDevice->LightEnable( 0, TRUE );	
		}
		else
		{
			pd3dDevice->SetTexture( 0, NULL );
			pd3dDevice->LightEnable( 0, FALSE );
		}


		if( m_FileVersion >= EFN_VERSION_19 )
		{
			if( bUseMaterial )
				pd3dDevice->SetMaterial( &m_cdData->material );

			if( !m_pTexture )
			{
				// Enable alpha blending.
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

				// Set the source blend state.
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

				// Set the destination blend state.
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}
		}
		else
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		
		pd3dDevice->SetRenderState( D3DRS_TWEENFACTOR, FtoDW( fTweenFactor ) );
		
		// Set our vertex declaration
		pd3dDevice->SetVertexDeclaration( m_pVertexDeclaration );
		 

		// Set the stream sources that the vertex declaration will point to
		pd3dDevice->SetStreamSource( 0, m_pPosBuff[m_iCurrentFrame],	0, sizeof( CVERTEX_XYZ ) ); // set to your first keyframe
		pd3dDevice->SetStreamSource( 1, m_pNormBuff[m_iCurrentFrame],	0, sizeof( CVERTEX_XYZ ) ); // set to your first keyframe
		pd3dDevice->SetStreamSource( 2, m_pPosBuff[m_iNextFrame],		0, sizeof( CVERTEX_XYZ ) ); // set to your second keyframe
		pd3dDevice->SetStreamSource( 3, m_pNormBuff[m_iNextFrame],		0, sizeof( CVERTEX_XYZ ) ); // set to your second keyframe

		if( IsUV1Used() )
			pd3dDevice->SetStreamSource( 4, m_pUvBuff,	0, sizeof( CVERTEX_UV ) ); // set to your second keyframe
		else
			pd3dDevice->SetStreamSource( 4, m_pUvBuff2,	0, sizeof( CVERTEX_UV ) ); // set to your second keyframe

		pd3dDevice->SetStreamSource( 5, m_pVColorBuff,	0, sizeof( long ) ); // set to your second keyframe

		if( m_bLodExists )
			pd3dDevice->SetIndices( m_pIB2 );
		else
			pd3dDevice->SetIndices( m_pIB );
		
		

		//// Draw the tweened model!
		//if( m_pEffect && m_pParent )
		//{
		//	UINT cPasses;

		//	CViewMgr* ViewMgr = ((CCookieMachineDlg*)m_pParent)->GetViewMgrGlob();	

		//	D3DXVECTOR3 ltDir = ViewMgr->m_camCurrent->pos - ViewMgr->m_camCurrent->dir;
		//	D3DXVec3Normalize( &ltDir, &ltDir );

		//	m_pEffect->SetValue( "light_dir", ltDir, sizeof(D3DXVECTOR3) );
		//	m_pEffect->SetValue( "view_pos", ViewMgr->m_camCurrent->pos, sizeof(D3DXVECTOR3) );

		//	
		//	D3DXMATRIX mWorld, mView, mProj, mWorldViewProj;
		//	pd3dDevice->GetTransform( D3DTS_WORLD, &mWorld );
		//	pd3dDevice->GetTransform( D3DTS_VIEW, &mView );
		//	pd3dDevice->GetTransform( D3DTS_PROJECTION, &mProj );

		//	mWorldViewProj = mWorld * mView * mProj;

		//	D3DXMatrixTranspose( &mWorldViewProj, &mWorldViewProj );
		//	D3DXMatrixTranspose( &mWorld, &mWorld );
		//	
		//	m_pEffect->SetMatrix( "matViewProjection", &mWorldViewProj );
		//	m_pEffect->SetMatrix( "matWorld", &mWorld );

		//	m_pEffect->SetTexture( "base_Tex", m_pTexture );

		//	m_pEffect->SetTechnique( "Textured" );

	 //       m_pEffect->Begin( &cPasses, 0 );

		//	m_pEffect->BeginPass(0);

		//	// Render the mesh with the applied technique
		//	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);

		//	m_pEffect->EndPass();

		//	m_pEffect->End();
		//}
		//else
		//{

		if( !m_bLodExists )
 			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count, 0, m_cdData->vertex_indices_count / 3);
		else
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cdData->vertecies_per_frame_count2, 0, m_cdData->vertex_indices_count2 / 3);
		//}

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
		
	//render attachments
	for( int i = 0; i < GetNullsCount(); i++ )
	{
		if( m_cdData->vecNulls[i].attaches.size() > 0 )
		{
			for( UINT j = 0; j < m_cdData->vecNulls[i].attaches.size(); j++ )
			{
				vector<CTRANSFORMEX>* matrices = &m_cdData->vecNulls[i].matrices;

				
				D3DXMATRIX rotX, rotY, rotZ, transl, scale, res, pre;

				//tween null matrices
				float tr_x = (*matrices)[ m_iCurrentFrame ].pos.x + ( (*matrices)[ m_iNextFrame ].pos.x - (*matrices)[ m_iCurrentFrame ].pos.x ) * fTweenFactor;
				float tr_y = (*matrices)[ m_iCurrentFrame ].pos.y + ( (*matrices)[ m_iNextFrame ].pos.y - (*matrices)[ m_iCurrentFrame ].pos.y ) * fTweenFactor;
				float tr_z = (*matrices)[ m_iCurrentFrame ].pos.z + ( (*matrices)[ m_iNextFrame ].pos.z - (*matrices)[ m_iCurrentFrame ].pos.z ) * fTweenFactor;
				
				float rot_x = D3DXToRadian( ((*matrices)[ m_iCurrentFrame ].rot.x) ) + AngleDif( D3DXToRadian( (*matrices)[m_iNextFrame].rot.x ), D3DXToRadian( (*matrices)[m_iCurrentFrame].rot.x ) ) * fTweenFactor;
				float rot_y = D3DXToRadian( ((*matrices)[ m_iCurrentFrame ].rot.y) ) + AngleDif( D3DXToRadian( (*matrices)[m_iNextFrame].rot.y ), D3DXToRadian( (*matrices)[m_iCurrentFrame].rot.y ) ) * fTweenFactor;
				float rot_z = D3DXToRadian( ((*matrices)[ m_iCurrentFrame ].rot.z) ) + AngleDif( D3DXToRadian( (*matrices)[m_iNextFrame].rot.z ), D3DXToRadian( (*matrices)[m_iCurrentFrame].rot.z ) ) * fTweenFactor;

				//create matices
				D3DXMatrixTranslation( &transl, tr_x, tr_y, tr_z );

				D3DXMatrixRotationX( &rotX,	rot_x );
				D3DXMatrixRotationY( &rotY,	rot_y );
				D3DXMatrixRotationZ( &rotZ,	rot_z );

				D3DXMatrixScaling( &scale, (*matrices)[ m_iCurrentFrame ].scl.z, (*matrices)[ m_iCurrentFrame ].scl.z, (*matrices)[ m_iCurrentFrame ].scl.z );

				//interpolate matrices
				pre = InterpolateMatrices( &(*matrices)[ m_iCurrentFrame ].pre_transform, &(*matrices)[ m_iNextFrame ].pre_transform, fTweenFactor );

				res = scale * rotX * rotY * rotZ * transl * pre * matr_old_world;

				pd3dDevice->SetTransform( D3DTS_WORLD, &res );


				if( i == m_iSelectedNull )
				{
					SetVColorFromMaterial( true, pd3dDevice );

					//pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );
					pd3dDevice->SetMaterial( &m_SelectionMtrl );

					((CEfnCharacter*)m_cdData->vecNulls[i].attaches[j])->Render( pd3dDevice, bWireframeMode, fTweenFactor, false );

					SetVColorFromMaterial( false, pd3dDevice );
				}
				else
				{
					//pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
					pd3dDevice->SetMaterial( &m_WhiteMtrl );
					((CEfnCharacter*)m_cdData->vecNulls[i].attaches[j])->Render( pd3dDevice, bWireframeMode, fTweenFactor );
				}

				((CEfnCharacter*)m_cdData->vecNulls[i].attaches[j])->RenderNormals( pd3dDevice );
			}
		} //if( m_cdData->vecNulls[i].attaches.size() > 0 ) 
	}
	
	pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );


	//draw bounding box
	if( m_bIsAABBVisible && m_pVBLines )
	{
		SetVColorFromMaterial( true, pd3dDevice );

		//pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		//pd3dDevice->LightEnable( 0, FALSE );
		pd3dDevice->SetTexture( 0, NULL);

		pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffff00 );
		pd3dDevice->SetMaterial( &m_YellowMtrl );

		pd3dDevice->SetStreamSource( 0, m_pVBLines[m_iCurrentFrame], 0, sizeof(CVERTEX));
		pd3dDevice->SetFVF(D3DFVF_VERTEX_DESCRIPTOR);
		pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 12 );

		//pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		//pd3dDevice->LightEnable( 0, TRUE );
	}

	//draw bounding sphere
	if( ppMesh && m_pBoundingSpheres && m_bIsBoundingSphereVisible)
	{
		SetVColorFromMaterial( true, pd3dDevice );

		//pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		//pd3dDevice->LightEnable( 0, FALSE );
		pd3dDevice->SetTexture(0, NULL);

		D3DXMATRIXA16 matWorldTranformation;
		D3DXMATRIXA16 matSphereTranformation;
		pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldTranformation);

		D3DXMatrixTranslation( &matSphereTranformation, m_pBoundingSpheres[m_iCurrentFrame].pos.x, m_pBoundingSpheres[m_iCurrentFrame].pos.y, m_pBoundingSpheres[m_iCurrentFrame].pos.z );

		matSphereTranformation *= matWorldTranformation;

		pd3dDevice->SetTransform( D3DTS_WORLD, &matSphereTranformation );
		pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x0000ffff );
		pd3dDevice->SetMaterial( &m_BlueMtrl );

		ppMesh[m_iCurrentFrame]->DrawSubset( 0 );

		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldTranformation);
	}

	//disable tweening
	pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
		
	///////////////////////////////
	//       render nulls        //
	///////////////////////////////

	if( GetNullsCount() > 0 && m_pVBNull )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

		if( !bUseMaterial )
		{
			SetVColorFromMaterial( true, pd3dDevice );

			pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
			pd3dDevice->LightEnable( 0, true );
			pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffff0000 );
		}
		else
		{
			SetVColorFromMaterial( false, pd3dDevice );
			//kill the lights
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			pd3dDevice->LightEnable( 0, FALSE );		
		}

		D3DXMATRIX matr_old_world;
		pd3dDevice->GetTransform( D3DTS_WORLD, &matr_old_world );

		D3DXMATRIX rotX, rotY, rotZ, transl, scale, res, pre;

		// remember shading mode
		DWORD old_val;
		pd3dDevice->GetRenderState( D3DRS_FILLMODE, &old_val );

		// set current mode to solid
		pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

		// turn off texture
		pd3dDevice->SetTexture( 0, NULL );

		for( int i = 0; i < GetNullsCount(); i++ )
		{
			vector<CTRANSFORMEX>* matrices = &m_cdData->vecNulls[i].matrices;
		
			//tween null matrices
			float tr_x = (*matrices)[ m_iCurrentFrame ].pos.x + ( (*matrices)[ m_iNextFrame ].pos.x - (*matrices)[ m_iCurrentFrame ].pos.x ) * fTweenFactor;
			float tr_y = (*matrices)[ m_iCurrentFrame ].pos.y + ( (*matrices)[ m_iNextFrame ].pos.y - (*matrices)[ m_iCurrentFrame ].pos.y ) * fTweenFactor;
			float tr_z = (*matrices)[ m_iCurrentFrame ].pos.z + ( (*matrices)[ m_iNextFrame ].pos.z - (*matrices)[ m_iCurrentFrame ].pos.z ) * fTweenFactor;
			
			float rot_x = D3DXToRadian( ((*matrices)[ m_iCurrentFrame ].rot.x) ) + AngleDif( D3DXToRadian( (*matrices)[m_iNextFrame].rot.x ), D3DXToRadian( (*matrices)[m_iCurrentFrame].rot.x ) ) * fTweenFactor;
			float rot_y = D3DXToRadian( ((*matrices)[ m_iCurrentFrame ].rot.y) ) + AngleDif( D3DXToRadian( (*matrices)[m_iNextFrame].rot.y ), D3DXToRadian( (*matrices)[m_iCurrentFrame].rot.y ) ) * fTweenFactor;
			float rot_z = D3DXToRadian( ((*matrices)[ m_iCurrentFrame ].rot.z) ) + AngleDif( D3DXToRadian( (*matrices)[m_iNextFrame].rot.z ), D3DXToRadian( (*matrices)[m_iCurrentFrame].rot.z ) ) * fTweenFactor;

			//create matices
			D3DXMatrixTranslation( &transl, tr_x, tr_y, tr_z );

			D3DXMatrixRotationX( &rotX,	rot_x );
			D3DXMatrixRotationY( &rotY,	rot_y );
			D3DXMatrixRotationZ( &rotZ,	rot_z );

			D3DXMatrixScaling( &scale, icon_scale * (*matrices)[m_iCurrentFrame].scl.x, icon_scale * (*matrices)[m_iCurrentFrame].scl.y, icon_scale * (*matrices)[m_iCurrentFrame].scl.z );

			//interpolate matrices
			pre = InterpolateMatrices( &(*matrices)[ m_iCurrentFrame ].pre_transform, &(*matrices)[ m_iNextFrame ].pre_transform, fTweenFactor );

			res = scale * rotX * rotY * rotZ * transl * pre * matr_old_world;

			pd3dDevice->SetTransform( D3DTS_WORLD, &res );

			if( m_cdData->vecNulls[i].attaches.size() == 0 )
			{
				if( i == m_iSelectedNull )
				{
					pd3dDevice->SetStreamSource( 0, m_pVBSelectedNull, 0, sizeof(CVERTEX) );
					pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
					pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
				}
				else
				{
					pd3dDevice->SetStreamSource( 0, m_pVBNull, 0, sizeof(CVERTEX) );
					pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
					pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
				}
			}
		}

		//lights on!
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		pd3dDevice->LightEnable( 0, TRUE );	
	}

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, fill_old );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, light_old );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, amb_old );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matr_old_world );
}

void CEfnCharacter::SetSelectedNull( str_t name )
{
	for( int i = 0; i < GetNullsCount(); i++ )
	{
		if( m_cdData->vecNulls[i].id == name )
		{
			m_iSelectedNull = i;
			return;
		}
	}

	m_iSelectedNull = -1;
}

void CEfnCharacter::SetVColorFromMaterial( bool bValue, LPDIRECT3DDEVICE9 pd3dDevice )
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

void CEfnCharacter::SetCurrNextFrame( int iCurrFrameIndex, int iNextFrameIndex )
{
	if( !m_cdData )
		return;

	if ( iCurrFrameIndex >= 0 && iCurrFrameIndex < GetFramesCount() )
		m_iCurrentFrame = iCurrFrameIndex;

	if ( iNextFrameIndex >= 0 && iNextFrameIndex < GetFramesCount() )
		m_iNextFrame = iNextFrameIndex;
}

void CEfnCharacter::ResetTexture(LPDIRECT3DDEVICE9 pd3dDevice)
{
	SAFE_RELEASE( m_pTexture );
	D3DXCreateTextureFromFile( pd3dDevice, m_strTexturePath.data(), &m_pTexture );
}

int CEfnCharacter::GetFramesCount()
{
	if( m_cdData )
		return m_cdData->anim_frames_count;
	else
		return 0;
}

int CEfnCharacter::CreateSequence()
{
	str_list_iterator_t iter;
	int total_frames = 0;
	int offset = 0;

	/************ calculate total frames count ************/

	iter = m_strSequenceAnims.begin();

	//walk through sequence names
	for( unsigned int i = 0; i < m_strSequenceAnims.size(); i++ )
	{
		//find them in animations array
		for( int j = 0; j < GetAnimationsCount(); j++ )
		{
			//increment frames counter in case animation found
			if( strcmp( (&(*iter))->data(), m_cdData->vecAnimations[j].name.data()  ) == 0 )
			{
				total_frames += m_cdData->vecAnimations[j].frames_count;
			}
		}

		iter++;
	}

	/************ create sequence from animations data ************/
	m_iSequenceFrames      = new int[total_frames];
	m_iSequenceFramesCount = total_frames;

	iter = m_strSequenceAnims.begin();

	//walk through sequence names
	for( unsigned int i = 0; i < m_strSequenceAnims.size(); i++ )
	{
		//find them in animations array
		for( int j = 0; j < GetAnimationsCount(); j++ )
		{
			//increment frames counter in case animation found
			if( strcmp( (&(*iter))->data(), m_cdData->vecAnimations[j].name.data()  ) == 0 )
			{
				for( int k = 0; k < m_cdData->vecAnimations[j].frames_count; k++ )
				{
					m_iSequenceFrames[k + offset] = m_cdData->vecAnimations[j].frames_list[k];  
				}

				offset += m_cdData->vecAnimations[j].frames_count;
			}
		}

		iter++;
	}

	return 0;
}

float CEfnCharacter::GetZOffset( int iIndex )
{
	if( m_cdData && iIndex >= 0 && iIndex < GetFramesCount() )
	  return m_cdData->z_coord_offset_list[iIndex].z;
	else
		return 0.0f;
}

void CEfnCharacter::SetZOffset( int iIndex, float fValue )
{
	if( m_cdData && iIndex >= 0 && iIndex < GetFramesCount() )
	  m_cdData->z_coord_offset_list[iIndex].z = fValue;
}

int CEfnCharacter::GetTrianglesCount()
{
	if ( m_cdData )
		return m_cdData->vertex_indices_count / 3; 
	else
		return -1;
}

int CEfnCharacter::GetTrianglesCount2()
{
	if ( m_cdData )
		return m_cdData->vertex_indices_count2 / 3; 
	else
		return -1;
}

int CEfnCharacter::GetVerteciesCount()
{
	if( m_cdData )
		return m_cdData->vertecies_per_frame_count;
}

int CEfnCharacter::GetVerteciesCount2()
{
	if( m_cdData )
		return m_cdData->vertecies_per_frame_count2;
}

const char * CEfnCharacter::GetModelName()
{
	if( m_cdData )
		return m_cdData->name.data();
	else
		return NULL;
}

void CEfnCharacter::SetModelName(char *strName)
{
	if( m_cdData )
		m_cdData->name = strName;
}

void CEfnCharacter::DicardVerteciesColor()
{
	if( m_cdData )
	{
		for(int i = 0; i < m_cdData->vertecies_per_frame_count; i++ )
			m_cdData->vcolList[i] = 0xffffffff;
	}
}

int CEfnCharacter::GetAnimationsCount(void)
{
	if( m_cdData )
		return m_cdData->vecAnimations.size();
	else
		return 0;
}

void CEfnCharacter::AddAnimToSequence(char * szName)
{
	m_strSequenceAnims.push_back(szName);
}

int CEfnCharacter::GetSequenceLength(void)
{
	return m_iSequenceFramesCount;
}

int * CEfnCharacter::GetSequence(void)
{
	return m_iSequenceFrames;
}

void CEfnCharacter::KillSequence(void)
{
	SAFE_DELETE_ARRAY(m_iSequenceFrames);
	m_iSequenceFramesCount = 0;
	m_strSequenceAnims.clear();
}

float CEfnCharacter::GetAnimationVecLength( char* szAnimationName )
{
	float anim_length = 0.0f;

	EFN_ANIMATION* anim = GetAnimationByName( szAnimationName );

	//calc length
	for( int j = 0; j < anim->frames_count; j++ )
		anim_length += GetZOffset( anim->frames_list[j] );

	return anim_length;
}

int CEfnCharacter::ContinueSequence(float vec_length, bool walk_to_run)
{
	//int curr_anim_type = -1; // -1 no valid anim
	//                         //  0 walk
	//                         //  1 run

	//int    cyc_part_count = 0;
	//int   *cyc_part = NULL;
	//float  cyc_part_length = 0.0f;
	//float  cycles_len;
	//int    cycles_count;
	//float  cycle_remainder = 0.0f;
	//EFN_ANIMATION cyc_anim;
	//EFN_ANIMATION temp;
	//bool   found = false;
 // 

	////UINT i = 0;
	////UINT d = std::max( i-(UINT)1, 0 );


	//for( int i = 0; i < m_cdData->animations_count; i++ )
	//{
	//  for( int j = 0; j < m_cdData->animations_list[i].frames_count; j++ )
	//	{
	//	  if( m_cdData->animations_list[i].frames_list[j] == m_iCurrentFrame )
	//		{
	//			//store found animation
	//			cyc_anim = m_cdData->animations_list[i];
	//			cyc_part_count = cyc_anim.frames_count - j - 1;
	//			cyc_part = new int[cyc_part_count];
	//			
	//			for( int k = 0; k < cyc_part_count; k++ )
	//			{
	//			  cyc_part[k] = cyc_anim.frames_list[j + k + 1];
	//				cyc_part_length += GetZOffset(cyc_part[k]);
	//			}
 //       
	//			found = true;

	//			break;
	//		}
	//	}

	//	if( found )
	//		break;
	//}

	//if( strcmp(cyc_anim.name.data(), "цикл походки" ) == 0 )
	//	curr_anim_type = 0;

	//if( strcmp(cyc_anim.name.data(), "цикл бега" ) == 0 )
	//	curr_anim_type = 1;

	//switch( curr_anim_type )
	//{
	//  case -1:SAFE_DELETE_ARRAY(cyc_part); 
	//		      break;

	//	case 0: /* продолжить походку */
	//		      //убить предыдущую секвенцию
	//		      KillSequence();

	//					if( !walk_to_run )
	//					{
	//						//отнять от длины походки длину конца походки
	//						cycles_len   = vec_length - GetAnimationVecLength("конец походки");

	//						//отнять часть цикла оставшегося от предыдущей анимации
	//						cycles_len -= cyc_part_length;

	//						//вычислить количество циклов
	//						cycles_count = (int)(cycles_len / GetAnimationVecLength("цикл походки"));

	//						//чистый обрезок
	//						cycle_remainder = cycles_len - cycles_count * GetAnimationVecLength("цикл походки");

	//						if( GetAnimationVecLength("цикл походки") / 2 < cycle_remainder )
	//							cycles_count++;

	//						//построить новую секвенцию:
	//						//подсчитать количество кадров
	//						m_iSequenceFramesCount = cyc_part_count;

	//						for(int i = 0; i < cycles_count; i++)
	//							m_iSequenceFramesCount += cyc_anim->frames_count;
	//				    
	//						temp = GetAnimationByName("конец походки");

	//						m_iSequenceFramesCount += temp.frames_count;

	//						//новая сквенция
	//						m_iSequenceFrames = new int[m_iSequenceFramesCount];

	//						//копировать часть цикла оставшегося от предыдущей анимации
	//						for(int i = 0; i < cyc_part_count; i++)
	//							m_iSequenceFrames[i] = cyc_part[i];

	//						//стереть временный массив 
	//						SAFE_DELETE_ARRAY(cyc_part);

	//						//копировать циклы походки
	//						for(int k = 0; k < cycles_count; k++)
	//						{
	//							for(int j = 0; j < cyc_anim.frames_count; j++)
	//								m_iSequenceFrames[cyc_part_count + k * cyc_anim.frames_count + j] = cyc_anim.frames_list[j];
	//						}

	//						//копировать конец походки
	//						for(int k = 0; k < temp.frames_count; k++)
	//							m_iSequenceFrames[cyc_part_count + cycles_count * cyc_anim.frames_count + k] = temp.frames_list[k]; 
	//					}
	//					else
	//					{
	//						//walk to the right walking leg
	//						SAFE_DELETE_ARRAY(cyc_part);
 //             float run_end = GetAnimationVecLength("конец бега");
	//					  float cyc_run_end = 0.0f;

	//						//just add the half of the running cycle
	//						if( m_iCurrentFrame == 16 )
	//						{
	//							cyc_part = new int[11];
 //               cyc_part_count = 11;
	//							for(int i = 60; i < 71; i++)
	//							{
 //                 cyc_run_end += GetZOffset(i);
	//								cyc_part[i - 60] = i;
	//							} 
	//						}

	//						//walk and run
	//						if( m_iCurrentFrame < 16 )
	//						{
	//							int beg_offset = 16 - m_iCurrentFrame;

	//							cyc_part = new int[16 - m_iCurrentFrame + 11];
 //               cyc_part_count = 16 - m_iCurrentFrame + 11;
	//							for(int i = m_iCurrentFrame + 1; i <= 16; i++)
	//							{
 //                 cyc_run_end += GetZOffset(i);
	//								cyc_part[i - (m_iCurrentFrame + 1)] = i;
	//							}
	//							for(int i = 60; i < 71; i++)
	//							{
 //                 cyc_run_end += GetZOffset(i);
	//								cyc_part[beg_offset + (i - 60)] = i;
	//							}
	//						}

	//						if( m_iCurrentFrame > 16 )
	//						{
	//							cyc_part = new int[33 - m_iCurrentFrame];
 //               cyc_part_count = 33 - m_iCurrentFrame;

	//							for(int i = m_iCurrentFrame + 1; i <= 33; i++)
	//							{
 //                 cyc_run_end += GetZOffset(i);
	//								cyc_part[i - (m_iCurrentFrame + 1)] = i;
	//							}
	//						}

	//						float cycs_length = vec_length - (cyc_run_end + run_end);

	//						if( cycs_length >= 0)
	//						{
 //               int cycles = (int)(cycs_length / GetAnimationVecLength("цикл бега"));
	//							int frames = cyc_part_count + 
	//								           cycles * GetAnimationByName("цикл бега")->frames_count + 
	//													 GetAnimationByName("конец бега")->frames_count;

	//							KillSequence();

	//							//новая сквенция
	//					    m_iSequenceFrames = new int[frames];
	//							m_iSequenceFramesCount = frames;

	//							for(int i = 0; i < cyc_part_count; i++)
	//							{
 //                 m_iSequenceFrames[i] = cyc_part[i];
	//							}

 //               for(int i = 0; i < cycles; i++)
	//							{
	//							  for(int n = 0; n < GetAnimationByName("цикл бега")->frames_count; n++)
	//							  {
 //                   m_iSequenceFrames[cyc_part_count + i * GetAnimationByName("цикл бега")->frames_count + n] = GetAnimationByName("цикл бега")->frames_list[n];
	//								}
	//							}

	//							int _off_ = cyc_part_count + cycles * GetAnimationByName("цикл бега")->frames_count;

	//							for(int i = 0; i < GetAnimationByName("конец бега")->frames_count; i++)
	//							{
 //                 m_iSequenceFrames[_off_ + i] = GetAnimationByName("конец бега")->frames_list[i];
	//							}
	//							curr_anim_type = 2;
	//						}
	//						
	//						SAFE_DELETE_ARRAY(cyc_part);
	//					}
	//					break;

	//	case 1: /* продолжить бег */
	//		      //убить предыдущую секвенцию
	//		      KillSequence();

	//					//отнять от длины бега длину конца бега
	//					cycles_len   = vec_length - GetAnimationVecLength("конец бега");

	//					//отнять часть цикла оставшегося от предыдущей анимации
	//					cycles_len -= cyc_part_length;

	//					//вычислить количество циклов
	//					cycles_count = (int)(cycles_len / GetAnimationVecLength("цикл бега"));

	//					//чистый обрезок
	//					cycle_remainder = cycles_len - cycles_count * GetAnimationVecLength("цикл бега");

	//					if( GetAnimationVecLength("цикл бега") / 2 < cycle_remainder )
	//						cycles_count++;

	//					//построить новую секвенцию:
	//					//подсчитать количество кадров
	//					m_iSequenceFramesCount = cyc_part_count;

	//					for(int i = 0; i < cycles_count; i++)
	//						m_iSequenceFramesCount += cyc_anim.frames_count;
	//			    
	//					temp = GetAnimationByName("конец бега");

	//					m_iSequenceFramesCount += temp.frames_count;

	//					//новая сквенция
	//					m_iSequenceFrames = new int[m_iSequenceFramesCount];

	//					//копировать часть цикла оставшегося от предыдущей анимации
	//					for(int i = 0; i < cyc_part_count; i++)
	//						m_iSequenceFrames[i] = cyc_part[i];

	//					//стереть временный массив 
	//					SAFE_DELETE_ARRAY(cyc_part);

	//					//копировать циклы бега
	//					for(int k = 0; k < cycles_count; k++)
	//					{
	//						for(int j = 0; j < cyc_anim.frames_count; j++)
	//							m_iSequenceFrames[cyc_part_count + k * cyc_anim.frames_count + j] = cyc_anim.frames_list[j];
	//					}

	//					//копировать конец бега
	//					for(int k = 0; k < temp.frames_count; k++)
	//						m_iSequenceFrames[cyc_part_count + cycles_count * cyc_anim.frames_count + k] = temp.frames_list[k]; 

	//					break;
	//}

	return 0;
}

void CEfnCharacter::DeleteAllAnimations()
{
	if( !m_cdData || GetAnimationsCount() <= 0 )
		return;

	//delete animations lists
	for( int i = 0; i < GetAnimationsCount(); i++ )
		SAFE_DELETE_ARRAY( m_cdData->vecAnimations[i].frames_list );

	m_cdData->vecAnimations.clear();
}

EFN_ANIMATION* CEfnCharacter::GetAnimationByName( str_t strName )
{
	for( int i = 0; i < GetAnimationsCount(); i++ )
	{
		str_t tmp = m_cdData->vecAnimations[i].name;

		if( _2Upper(tmp) == _2Upper(strName) )
			return &m_cdData->vecAnimations[i];
	}

	return NULL;
}

void CEfnCharacter::DeleteAnimationByName( str_t strName )
{
	for( int i = 0; i < GetAnimationsCount(); i++ )
	{
		if( m_cdData->vecAnimations[i].name == strName )
		{
			SAFE_DELETE_ARRAY( m_cdData->vecAnimations[i].frames_list );
			m_cdData->vecAnimations.erase( m_cdData->vecAnimations.begin() + i );
		}
	}
}

EFN_ANIMATION* CEfnCharacter::GetAnimationByIndex( int iIndex )
{
	if( iIndex < 0 || iIndex >= GetAnimationsCount() )
		return NULL;

	return &m_cdData->vecAnimations[iIndex];
}

void CEfnCharacter::DeleteFrames( int iIndexBegin, int iIndexEnd )
{
	//check parameters
	if( (iIndexBegin >= 0 && iIndexEnd < GetFramesCount()) == false )
		return;

	//calc new amount of frames
	int	new_frame_count	= GetFramesCount() - (iIndexEnd - iIndexBegin + 1);
	int	new_size		= m_cdData->vertecies_per_frame_count * new_frame_count;
	int	beg_off			= m_cdData->vertecies_per_frame_count * iIndexBegin;

	unsigned short* pNewShortBuff = NULL;

	CVERTEX_XYZ* array_pos	= NULL;
	CVERTEX_XYZ* array_norm = NULL;

	if( new_size != 0 )
	{
		//////////////////////////////////////////
		//			PROCESS VERTECIES			//
		//////////////////////////////////////////

		//create new vertex array
		array_pos		= new CVERTEX_XYZ[new_size];
		array_norm		= new CVERTEX_XYZ[new_size];

		if( m_cdData->buffshorts )
			pNewShortBuff	= new unsigned short[new_size * 3];

		//copy data from front slice of the current vertecies data 
		if( beg_off > 0 )
		{
			for( int i = 0; i < m_cdData->vertecies_per_frame_count * iIndexBegin; i++ )
			{
				array_pos[i]	= m_cdData->posList[i];
				array_norm[i]	= m_cdData->normList[i];

				//cutcompressed data (if any)
				if( m_cdData->buffshorts )
				{
					pNewShortBuff[i * 3]	= m_cdData->buffshorts[i * 3];
					pNewShortBuff[i * 3 + 1]= m_cdData->buffshorts[i * 3 + 1];
					pNewShortBuff[i * 3 + 2]= m_cdData->buffshorts[i * 3 + 2];
				}
			}
		}

		if( iIndexEnd < GetFramesCount() - 1 )
		{
			for( int i = m_cdData->vertecies_per_frame_count * (iIndexEnd + 1); i < m_cdData->vertecies_count; i++ )
			{
				int off = beg_off + (i - m_cdData->vertecies_per_frame_count * (iIndexEnd + 1)); 
				array_pos[off]	= m_cdData->posList[i];
				array_norm[off]	= m_cdData->normList[i];

				//cut compressed data (if any)
				if( m_cdData->buffshorts )
				{
					pNewShortBuff[off * 3]	= m_cdData->buffshorts[i * 3];
					pNewShortBuff[off * 3 + 1]= m_cdData->buffshorts[i * 3 + 1];
					pNewShortBuff[off * 3 + 2]= m_cdData->buffshorts[i * 3 + 2];
				}
			}
		}
	}

	////////////////////////////////////////////////////////
	// PROCESS AABBS	& BSPHERES //
	////////////////////////////////////////////////////////
	D3DXVECTOR3**	aabbTemplist	= NULL;
	CSPHERE*		bsphTemplist	= NULL;
	D3DXVECTOR3*	z_off_list		= NULL;

	//in case aabblist exist in model
	if( m_aabbList )
	{
		aabbTemplist = new D3DXVECTOR3*[new_frame_count];

		for( int i = 0; i < new_frame_count; i++ )
			aabbTemplist[i] = new D3DXVECTOR3[8];
	}

	//in case bounding spheres exist in model
	if( m_pBoundingSpheres )
		bsphTemplist = new CSPHERE[new_frame_count];

	//in case zoffsets exist in model
	if( m_cdData->z_coord_offset_list )
		z_off_list = new D3DXVECTOR3[new_frame_count];

	if( iIndexBegin != 0 )
	{
		for( int i = 0; i < iIndexBegin; i++ )
		{
			//in case bounding spheres exist in model
			if( m_pBoundingSpheres )
				bsphTemplist[i] = m_pBoundingSpheres[i];

			//in case zoffsets exist in model
			if( m_cdData->z_coord_offset_list )
				z_off_list[i].z	= m_cdData->z_coord_offset_list[i].z;

			//in case aabblist exist in model
			if( m_aabbList )
			{
				for( int j = 0; j < 8; j++ )
					aabbTemplist[i][j] = m_aabbList[i][j];
			}
		}
	}

	if( iIndexEnd < GetFramesCount() - 1 )
	{
		for( int i = iIndexEnd + 1; i < GetFramesCount(); i++ )
		{
			int iOff = i - (iIndexEnd + 1);

			//in case bounding spheres exist in model
			if( m_pBoundingSpheres )
				bsphTemplist[iIndexBegin + iOff] = m_pBoundingSpheres[i];

			//in case zoffsets exist in model
			if( m_cdData->z_coord_offset_list )
				z_off_list[iIndexBegin + iOff]	 = m_cdData->z_coord_offset_list[i];

			//in case aabblist exist in model
			if( m_aabbList )
			{
				for( int j = 0; j < 8; j++ )
					aabbTemplist[ iIndexBegin + iOff][j] = m_aabbList[i][j];
			}
		}
	}


	///////////////////////////////////
	//			PROCESS NULLS		 //
	///////////////////////////////////
	for( int i = 0; i < GetNullsCount(); i++ )
	{
		for( int j = iIndexBegin; j <= iIndexEnd; j++ )
			m_cdData->vecNulls[i].matrices.erase( m_cdData->vecNulls[i].matrices.begin() + iIndexBegin );
	}


	//////////////////////////////////////////////
	//			SET NEW DATA TO CHAR			//
	//////////////////////////////////////////////
	if( new_size != 0 )
	{
		//set new vertices array
		SAFE_DELETE_ARRAY( m_cdData->posList );
		SAFE_DELETE_ARRAY( m_cdData->normList );
		SAFE_DELETE_ARRAY( m_cdData->buffshorts );
		m_cdData->posList	= array_pos;
		m_cdData->normList	= array_norm;

		if( m_cdData->buffshorts )
			m_cdData->buffshorts= pNewShortBuff;
	}

	//in case aabblist exist in model
	if( m_aabbList )
	{
		//set new aabb array
		for( int i = 0; i < GetFramesCount(); i++ )
			SAFE_DELETE_ARRAY( m_aabbList[i] );

		SAFE_DELETE_ARRAY( m_aabbList );

		m_aabbList = aabbTemplist;
	}

	//in case bounding spheres exist in model
	if( m_pBoundingSpheres )
	{
		//set new spheres array
		SAFE_DELETE_ARRAY( m_pBoundingSpheres );
		m_pBoundingSpheres = bsphTemplist;
	}

	//in case zoffsets exist in model
	if( m_cdData->z_coord_offset_list )
	{
		//set new offsets array
		SAFE_DELETE_ARRAY( m_cdData->z_coord_offset_list );
		m_cdData->z_coord_offset_list = z_off_list;
	}

	//safety check
	if( GetFramesCount() == 0 )
	{
		m_cdData->vertecies_count			= 0;
		m_cdData->vertecies_per_frame_count	= 0;
		m_cdData->vertex_indices_count		= 0;
		SAFE_DELETE_ARRAY( m_cdData->vertex_indices_list );
	}

	//delete existing vertex buffers that depend on frames count
	if( m_pVBLines )
	{
		for( i = 0; i < GetFramesCount(); i++ )
			SAFE_RELEASE( m_pVBLines[i] );

		SAFE_DELETE_ARRAY( m_pVBLines );
	}

	if( ppMesh )
	{
		for( i = 0; i < GetFramesCount(); i++ )
			SAFE_RELEASE( ppMesh[i] );

		SAFE_DELETE_ARRAY( ppMesh );
	}

	//set new data
	m_cdData->vertecies_count	= new_size;
	m_cdData->anim_frames_count	= new_frame_count;
}

void CEfnCharacter::DrawModel(LPDIRECT3DDEVICE9 pd3dDevice)
{
	//if( GetAnimationsCount() <= 0 )
	//	return;

	//float			_time;
	//static float	timeelapsed		= 0.0f;
	//static float	timeelapsed_rot = 0.0f;
	//static bool		anim_reset		= false;
	//CString			tmp;
	//static int		rot_frames		= 0;
	//int				rot_subdiv		= 10;

	//D3DXMATRIX matCharMatrix;

	//if( m_cdData )
	//{
	//	if( m_cdData->vertex_indices_count > 0 && m_cdData->vertecies_count && m_pIB )
	//	{
	//		//lights on
	//		//pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	//		//pd3dDevice->LightEnable( 0, TRUE );

	//		_time = DXUtil_Timer(TIMER_GETELAPSEDTIME);

	//		timeelapsed     += _time;
	//		timeelapsed_rot += _time;

	//		//ROTATE
	//		if( !m_bIsRotationFinished )   
	//		{				
	//			int starting_frame;
	//			int anim_rot_frame;


	//			if( m_fRotationIncrement >= 0.0f )
	//				starting_frame = GetAnimationByName("поворот направо")->frames_list[0];
	//			else
	//				starting_frame = GetAnimationByName("поворот налево")->frames_list[0];


	//			anim_rot_frame = starting_frame + rot_frames;

	//			//in case rotation angle < 23 grads -- rotate without animation and return
	//			if( fabs(m_fRotationIncrement * rot_subdiv) < 23.0f )
	//			{
	//				m_fCurrentAngle += m_fRotationIncrement * rot_subdiv;
	//				m_bIsRotationFinished = true;
	//			}
	//			else
	//			{
	//				if( rot_frames < rot_subdiv )
	//				{
	//					if( timeelapsed > 0.032 )
	//					{
	//						m_fCurrentAngle += m_fRotationIncrement;
	//						timeelapsed = 0.0f;

 //                           if( GetAnimationByName("поворот направо")->frames_count > 1 ||
 //                               GetAnimationByName("поворот налево")->frames_count > 1 )
 //                           {
 //                              m_fZOffset = 0.0f;                                                        //reset offset

 //				               for( int i = starting_frame; i < anim_rot_frame; i++ )          //calc offset
	//					          m_fZOffset += GetZOffset(i);

	//				           RecalcWorldPos( CalcCharacterMatrix() );

	//						   SetCurrNextFrame( anim_rot_frame, anim_rot_frame );
 //                           }
 //                           else
 //                             SetCurrNextFrame( starting_frame, starting_frame );
	//				    
	//						rot_frames++;
	//					}
	//				}
	//				else
	//				{
	//					rot_frames = 0;
	//					m_bIsRotationFinished = true;
 //                       RecalcXZPos();
	//				}
	//			}

	//			if( m_fCurrentAngle < 0.0f )
	//				m_fCurrentAngle = 360.0f + m_fCurrentAngle;

	//			if( m_fCurrentAngle > 360.0f )
	//				m_fCurrentAngle -= 360.0f;

	//			pd3dDevice->SetTransform( D3DTS_WORLD, &(CalcCharacterMatrix()) ); 
	//			Render(pd3dDevice, false, 0.0f );
	//			return;
	//		}

	//		//MOVE
	//		if( m_bIsSequencePlaying )
	//		{
	//			if( !m_bIsRotationWFinished )
	//			{
	//				if( rot_frames < rot_subdiv )
	//				{
	//					if( timeelapsed_rot > 0.01 )
	//					{
	//						m_fCurrentAngle += m_fRotationIncrement;
	//						rot_frames++;
	//						timeelapsed_rot = 0.0f;
	//					}
	//				}
	//				else
	//				{
	//					rot_frames = 0;
	//					m_bIsRotationWFinished = true;
	//				}
	//			}

	//			if( m_iCurrentSequenceFrame >= GetSequenceLength())
	//			{
	//				m_iCurrentSequenceFrame = 0;
	//				m_iZOffsetFrame = 0;
	//				m_bIsSequencePlaying = false;
	//				RecalcXZPos();
	//			}
	//			else
	//			{
	//				m_fZOffset = 0.0f;                                                        //reset offset

	//				for( int i = m_iZOffsetFrame; i < m_iCurrentSequenceFrame; i++ )          //calc offset
	//					m_fZOffset += GetZOffset(GetSequence()[i]);

	//				RecalcWorldPos( CalcCharacterMatrix() );

	//				SetCurrNextFrame( GetSequence()[m_iCurrentSequenceFrame], GetSequence()[m_iCurrentSequenceFrame] );

	//				//increment animation frame
	//				if( timeelapsed > 0.0417f )
	//				{
	//					m_iCurrentSequenceFrame++;
	//					timeelapsed = 0.0f;	
	//				}		
	//			}
	//		}
	//		else /******************* draw first frame *******************/
	//		{
	//			m_fZOffset = 0.0f;
	//			KillSequence();
	//			RecalcWorldPos( CalcCharacterMatrix() );
	//			SetCurrNextFrame( 0, 0 );
	//		}

	//		pd3dDevice->SetTransform( D3DTS_WORLD, &(CalcCharacterMatrix()) );
	//		Render( pd3dDevice, false, 0.0f );

	//	} // if( m_cdData->vertex_indices_count > 0 && m_cdData->vertecies_count && m_pVB && m_pIB)
	//} // if( m_cdData )
}

void CEfnCharacter::SetWorldTransformation(D3DXMATRIX matxWorld)
{
	m_matxWorld = matxWorld;
}

D3DXMATRIX CEfnCharacter::CalcCharacterMatrix(void)
{
	D3DXMATRIX charPos, charRot, charPathOff, res;

	D3DXMatrixTranslation( &charPos, m_vCharXZPos.x, m_vCharXZPos.y, m_vCharXZPos.z );
	D3DXMatrixRotationY( &charRot, D3DXToRadian(m_fCurrentAngle) ); 
	D3DXMatrixTranslation( &charPathOff, 0.0f, 0.0f, m_fZOffset );

	res = charPathOff * charRot * charPos * m_matxWorld;
	return res;
}

void CEfnCharacter::RecalcXZPos(void)
{
	D3DXMATRIX inv_matWorld;
	D3DXVECTOR3 grid_coords = ToDX3Vec(m_vCharWorldPos);

	D3DXMatrixInverse(&inv_matWorld, NULL, &m_matxWorld);
	D3DXVec3TransformCoord(&grid_coords, &grid_coords, &inv_matWorld);

	m_vCharXZPos.x = grid_coords.x;
	m_vCharXZPos.z = grid_coords.z;

	m_fZOffset = 0.0f;

	RecalcWorldPos( CalcCharacterMatrix() );
	
	m_iZOffsetFrame = m_iCurrentSequenceFrame;
}

void CEfnCharacter::RecalcWorldPos(const D3DXMATRIX matxChar)
{
	D3DXVECTOR3 tt(0.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&tt, &tt, &matxChar);

	m_vCharWorldPos.SetData( ToCVec(tt)->x, ToCVec(tt)->y, ToCVec(tt)->z );
}

void CEfnCharacter::ProcessClick(bool& bDoubleClicked, CVector& isect)
{
	RecalcXZPos();
	CVector movement_vector = isect - m_vCharWorldPos;

	float walk_begin_end_len = GetAnimationVecLength("начало походки") + 
											GetAnimationVecLength("конец походки");

	float run_begin_end_len = GetAnimationVecLength("начало бега") + 
											GetAnimationVecLength("конец бега");

	if( movement_vector.Length() <= walk_begin_end_len - 2.0f )
	{
		if( !m_bIsSequencePlaying )
			{
  				if( m_bIsRotationFinished )
				{
					CalcCharacterRotation(isect);
					m_bIsRotationFinished = false;
				}
			}
			return;
	}

	//sequence playing, so continue it
	if( m_bIsSequencePlaying )
	{
		if( m_bIsRotationWFinished )
		{
			int result = ContinueSequence( movement_vector.Length(), bDoubleClicked );

		  if( result != -1 )
		  {
				m_iCurrentSequenceFrame = 0;
				m_iZOffsetFrame = 0;
	
				CalcCharacterRotation(isect);
				m_bIsRotationWFinished = false;
			}
		}

    bDoubleClicked = false;
		return;
	}

	//no sequence playing, create new sequence
	if( m_bIsRotationFinished )
	{
		CalcCharacterRotation(isect);
		m_bIsRotationFinished = false;
	}

  KillSequence();

	//one step
	if( movement_vector.Length() > walk_begin_end_len - 2.0f && movement_vector.Length() < walk_begin_end_len * 2)
	{
		bDoubleClicked = false;
	  AddAnimToSequence("начало походки");
	  AddAnimToSequence("конец походки");
	  CreateSequence();
		m_bIsSequencePlaying = true;
	}

	//walking and running
	if( movement_vector.Length() >= walk_begin_end_len * 2 )
	{
    float cycles_len;
		float cycles_rem;
		int   cycles;

		if( !bDoubleClicked )
		{
			cycles_len = movement_vector.Length() - walk_begin_end_len;
			cycles     = (int)( cycles_len / GetAnimationVecLength("цикл походки") );
			cycles_rem = cycles_len - GetAnimationVecLength("цикл походки") * cycles;

			if( GetAnimationVecLength("цикл походки") / 2 < cycles_rem )
        cycles++;

			if (cycles == 0)
				cycles = 1;

			AddAnimToSequence("начало походки");
			
			for(int j = 0; j < cycles; j++)
				AddAnimToSequence("цикл походки");
			
			AddAnimToSequence("конец походки");
		}
		else
		{
			cycles_len = movement_vector.Length() - run_begin_end_len;
			cycles = (int)( cycles_len / GetAnimationVecLength("цикл бега") );
      cycles_rem = cycles_len - GetAnimationVecLength("цикл бега") * cycles;

			if( GetAnimationVecLength("цикл бега") / 2 < cycles_rem )
        cycles++;

			if (cycles == 0)
				cycles = 1;

		  AddAnimToSequence("начало бега");
			
			for(int j = 0; j < cycles; j++)
				AddAnimToSequence("цикл бега");
			
			AddAnimToSequence("конец бега");
		}

    bDoubleClicked = false;

	  CreateSequence();
		m_bIsSequencePlaying = true;
	}

}

void CEfnCharacter::CalcCharacterRotation(CVector isect)
{
	//calc vectors
	CVector ang_vec_current, ang_vec_mouse;
	D3DXMATRIXA16 matOff, matRot, matTmp, matWorldTransl;
	float   angle, oldAngle;
	CVector temp;

	oldAngle = m_fCurrentAngle;

	//rotation vectors calculations/////////////////////////
	//mouse vec
	ang_vec_mouse = isect - m_vCharWorldPos;
	ang_vec_mouse.Normalize();

	//char vec
	D3DXMATRIXA16 ext;
	D3DXMatrixTranslation( &ext, 0.0f, 0.0f, 5.0f );

	matTmp = ext * CalcCharacterMatrix();
	D3DXVECTOR3 ttt(0.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&ttt, &ttt, &matTmp);

	CVector temp_a( ToCVec(ttt)->x, ToCVec(ttt)->y, ToCVec(ttt)->z );
	ang_vec_current = temp_a - m_vCharWorldPos;
	ang_vec_current.Normalize();
	//rotation vectors calculations/////////////////////END/

	angle = D3DXToDegree( acosf( ClampFloat( ang_vec_current % ang_vec_mouse ) ) );

	m_fRotationIncrement = angle / 10.0f;

	//calc rotated char face vector
	ttt.x = 0.0f; 
	ttt.y = 0.0f;
	ttt.z = 0.0f;

	D3DXMatrixTranslation( &ext, 0.0f, 0.0f, 5.0f );

	m_fCurrentAngle += angle;
	matTmp = ext * CalcCharacterMatrix(); 
	
	D3DXVec3TransformCoord(&ttt, &ttt, &matTmp);
	CVector temp_b( ToCVec(ttt)->x, ToCVec(ttt)->y, ToCVec(ttt)->z );
	temp = temp_b - m_vCharWorldPos;
	temp.Normalize();

	if( (ang_vec_mouse - temp).Length() > 0.0005 )
		m_fRotationIncrement *= -1.0f;

  m_fCurrentAngle = oldAngle;
}

void CEfnCharacter::ConvertToLHCoords(void)
{
	D3DXMATRIX rotate;
	D3DXMatrixRotationY( &rotate, D3DXToRadian(180) ); 
  
	if( !m_cdData )
		return;

	for(int i = 0; i < m_cdData->vertecies_count; i++)
	{
		m_cdData->posList[i].z *= -1.0f;

		m_cdData->normList[i].x *= -1.0f; 
		m_cdData->normList[i].y *= -1.0f; 

		D3DXVECTOR3 ttt;
		ttt.x = m_cdData->normList[i].x; 
		ttt.y = m_cdData->normList[i].y;
		ttt.z = m_cdData->normList[i].z;
	
		D3DXVec3TransformCoord(&ttt, &ttt, &rotate);

		m_cdData->normList[i].x = ttt.x; 
		m_cdData->normList[i].y = ttt.y;
		m_cdData->normList[i].z = ttt.z;

		ttt.x = m_cdData->posList[i].x; 
		ttt.y = m_cdData->posList[i].y;
		ttt.z = m_cdData->posList[i].z;
	
		D3DXVec3TransformCoord(&ttt, &ttt, &rotate);

		m_cdData->posList[i].x = ttt.x; 
		m_cdData->posList[i].y = ttt.y;
		m_cdData->posList[i].z = ttt.z;
	}

	int tri_cout = m_cdData->vertex_indices_count / 3;

	for(int i = 0; i < tri_cout; i++)
	{
		int temp;
		temp = m_cdData->vertex_indices_list[i * 3 + 0];
		m_cdData->vertex_indices_list[i * 3 + 0] = m_cdData->vertex_indices_list[i * 3 + 2];
		m_cdData->vertex_indices_list[i * 3 + 2] = temp;
	}

	for(int i = 0; i < GetFramesCount(); i++)
	{
		D3DXVECTOR3* aabb = m_aabbList[i];

		for (int u = 0; u < 8; u++)
			aabb[u].x *= -1.0f;

		m_pBoundingSpheres[i].pos.x *= -1.0f;
	}
}

float CEfnCharacter::GetModelHeight(void)
{
	if( GetFramesCount() <= 0 )
		return -1.0f;

	float max_y = -99999.0f;
	float min_y = 99999.0f;

	for(int i = 0; i < m_cdData->vertecies_per_frame_count; i++)
	{
		if( max_y < m_cdData->posList[i].y )
			max_y = m_cdData->posList[i].y;
		
		if( min_y > m_cdData->posList[i].y )
			min_y = m_cdData->posList[i].y;
	}

	return max_y - min_y;
}

void CEfnCharacter::GetSelectedMeshesOOBB( D3DXVECTOR3* vMin, D3DXVECTOR3* vMax )
{
	*vMin = D3DXVECTOR3( 9999999, 9999999, 9999999 );
	*vMax = D3DXVECTOR3( -9999999, -9999999, -9999999 );

	//mesh
	D3DXVECTOR3* pOOBB = GetCurrentAABB();

	if( m_cdData->vertecies_per_frame_count > 1 )
	{
		*vMax = pOOBB[1];
		*vMin = pOOBB[7];
	}
	
    //nulls
	for( UINT i = 0; i < m_cdData->vecNulls.size(); i++ )
	{
		CTRANSFORMEX	transform = m_cdData->vecNulls[i].matrices[m_iCurrentFrame];
		D3DXMATRIX		pre	= transform.pre_transform;
		D3DXMATRIX		tr, res;

		D3DXMatrixTranslation( &tr, transform.pos.x, transform.pos.y, transform.pos.z );
		
		res = tr * pre;
	
		D3DXQUATERNION pQuat;
		D3DXVECTOR3 vScale, vTranslation;

		//decompose
		D3DXMatrixDecompose( &vScale, &pQuat, &vTranslation, &res );

		if( vTranslation.x < vMin->x )
			vMin->x = vTranslation.x;

		if( vTranslation.y < vMin->y )
			vMin->y = vTranslation.y;

		if( vTranslation.z < vMin->z )
			vMin->z = vTranslation.z;

		if( vTranslation.x > vMax->x )
			vMax->x = vTranslation.x;

		if( vTranslation.y > vMax->y )
			vMax->y = vTranslation.y;

		if( vTranslation.z > vMax->z )
			vMax->z = vTranslation.z;
	}

	if( m_cdData->vertecies_per_frame_count == 0 && D3DXVec3Length( &D3DXVECTOR3(*vMax - *vMin) ) < sqrt( 2.0 ) )
	{
		*vMin -= D3DXVECTOR3( 0.5f, 0.5f, 0.5f );
		*vMax += D3DXVECTOR3( 0.5f, 0.5f, 0.5f );
	}
}

CSPHERE CEfnCharacter::GetSelectedMeshesBSphere()
{
	CSPHERE sphere;

	D3DXVECTOR3 res, min, max;

	GetSelectedMeshesOOBB( &min, &max );

	res = max - min;
	res /= 2.0f;

	sphere.pos		= GetSelectedMeshesOOBBCenter();
	sphere.radius	= D3DXVec3Length( &res );

	return sphere;
}

D3DXVECTOR3 CEfnCharacter::GetSelectedMeshesOOBBCenter()
{
	D3DXVECTOR3 res, min, max;

	GetSelectedMeshesOOBB( &min, &max );

	res = max - min;
	res /= 2.0f;
	res += min;

	return res;
}

void CEfnCharacter::Scale( D3DXMATRIX matx_scale )
{
	D3DXVECTOR3 tmp;

	for( int i = 0; i < m_cdData->vertecies_count; i++ )
	{
		tmp.x = m_cdData->posList[i].x;
		tmp.y = m_cdData->posList[i].y;
		tmp.z = m_cdData->posList[i].z;
		
		D3DXVec3TransformCoord( &tmp, &tmp, &matx_scale );
		m_cdData->posList[i].x = tmp.x;
		m_cdData->posList[i].y = tmp.y;
		m_cdData->posList[i].z = tmp.z;
	}

	for( int i = 0; i < GetFramesCount(); i++ )
	{
		D3DXVec3TransformCoord( &m_cdData->z_coord_offset_list[i], &m_cdData->z_coord_offset_list[i], &matx_scale );
	}

	if( GetVerteciesCount() > 0 )
	{
		for( int i = 0; i < GetFramesCount(); i++ )
		{
			D3DXVec3TransformCoord( &m_pBoundingSpheres[i].pos, &m_pBoundingSpheres[i].pos, &matx_scale);

			tmp.x = m_pBoundingSpheres[i].radius;
			D3DXVec3TransformCoord( &tmp, &tmp, &matx_scale);
			m_pBoundingSpheres[i].radius = tmp.x;
		}

		for( int i = 0; i < GetFramesCount(); i++ )
		{
			for( int j = 0; j < 8; j++ )
			{
				D3DXVec3TransformCoord( &m_aabbList[i][j], &m_aabbList[i][j], &matx_scale);
			}
		}
	}

	for( int i = 0; i < GetNullsCount(); i++ )
	{
		for( int j = 0; j < GetFramesCount(); j++ )
		{
			D3DXVec3TransformCoord( &m_cdData->vecNulls[i].matrices[j].pos, &m_cdData->vecNulls[i].matrices[j].pos, &matx_scale );

			//m_cdData->vecNulls[i].matrices[j].scl.x *= matx_scale._11;
			//m_cdData->vecNulls[i].matrices[j].scl.y *= matx_scale._11;
			//m_cdData->vecNulls[i].matrices[j].scl.z *= matx_scale._11;

			D3DXVECTOR3 sc,tr;
			D3DXQUATERNION rt;
			D3DXMATRIX Mrt, Msc, Mtr;

			D3DXMatrixDecompose( &sc, &rt, &tr, &m_cdData->vecNulls[i].matrices[j].pre_transform );
			D3DXVec3TransformCoord( &tr, &tr, &matx_scale );

			D3DXMatrixTranslation( &Mtr, tr.x, tr.y, tr.z );
			D3DXMatrixScaling( &Msc, sc.x, sc.y, sc.z );

			rt.w *= -1.0f;

			D3DXMatrixRotationQuaternion( &Mrt, &rt );

			m_cdData->vecNulls[i].matrices[j].pre_transform = Msc * Mrt * Mtr;
		}
	}
}
