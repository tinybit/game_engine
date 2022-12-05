#pragma once

#include "utils/Common.h"
#include "EfnCharacterAnimation.cpp"
#include <list>

typedef std::list<EFN_ANIMATION>	list_t;
typedef list_t::iterator			iterator_t;

struct EFN_CHARACTERDATA
{
	EFN_CHARACTERDATA()
	{
		name					= "";
		vertex_indices_list		= NULL;
		vertex_indices_count	= 0;
		posList					= NULL;
		normList				= NULL;
		uvList					= NULL;
		uvList2					= NULL;
		vcolList				= NULL;
		vertecies_count			= 0;
		reverse_index			= NULL;
		anim_frames_count		= 0;
		anim_frame_scripts		= NULL;
		z_coord_offset_list		= NULL;
		buffshorts				= NULL;

		// HACK!!! LOD A
		vertex_indices_count2		= 0;
		vertecies_per_frame_count2	= 0;
		vertex_indices_list2		= NULL;
		// END HACK

		vertecies_per_frame_count = 0;
		global_radius			= 0;
		fFPS					= 0;
		ZeroMemory( &material, sizeof(material) );
	}

	str_t					name;						//character name
	str_t*					anim_frame_scripts;			//script for each frame

	unsigned short*			buffshorts;					//compressed vertecies

	int						vertex_indices_count;		//index list items count
	int						vertecies_count;			//vertecies count
	int						anim_frames_count;			//frames count
	int						vertecies_per_frame_count;	//amount of vertecies per one animation frame
	
	int						vertex_indices_count2;		//index list items count 2
	int						vertecies_per_frame_count2;	//amount of vertecies per one animation frame 2
	int*					vertex_indices_list2;		//index list 2

	int*					vertex_indices_list;		//index list
	int**					reverse_index;
	
	//int					animations_count;			//animations count
	//EFN_ANIMATION*		animations_list;			//animations list

	CVERTEX_XYZ*			posList;
	CVERTEX_XYZ*			normList;
	CVERTEX_UV*				uvList;
	CVERTEX_UV*				uvList2;
	D3DXVECTOR3*			z_coord_offset_list;		//z coord offsets for each frame
	D3DMATERIAL9			material;
	long*					vcolList;

	float					global_radius;				//global raduis of the model (used in pathfinding routines)	
	float					fFPS;

	vector<CNULL2>			vecNulls;					//list of nulls
	vector<CCUSTPARAM>		vecCustomParams;			//list of custom parameters
	vector<EFN_ANIMATION>	vecAnimations;				//animations list
};