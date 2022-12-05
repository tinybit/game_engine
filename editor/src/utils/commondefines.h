#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

typedef	std::string str_t;

typedef map<int, int>			IIntMap;
typedef IIntMap::iterator		IIntMap_iter;

typedef map<int, str_t>			IStrMap;
typedef IStrMap::iterator		IStrMap_iter;

typedef map<str_t, str_t>		StrMap;
typedef StrMap::iterator		StrMap_iter;

typedef	vector<str_t>			StrVec;
typedef	StrVec::iterator		StrVec_iter;

typedef	list<StrVec>			ListStrVec;
typedef	ListStrVec::iterator	ListStrVec_iter;

struct FuncEntry
{
	int		iBind;
	int		iOffset;
	CString strClassName;
	CString strFuncName;
};

typedef multimap<int, FuncEntry*>	IFuncMap;
typedef pair<int, FuncEntry*>		IFuncMap_pair;
typedef IFuncMap::iterator			IFuncMap_iter;

#ifndef	SAFE_DELETE_ARRAY
#define	SAFE_DELETE_ARRAY(p)	{ if(p) { delete [](p);	(p) = NULL; } }
#endif

#ifndef	SAFE_DELETE
#define	SAFE_DELETE(p)			{ if(p) { delete (p); (p) = NULL; } }
#endif

#ifndef	SAFE_RELEASE
#define	SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p) = NULL; } }
#endif

#ifndef	SAFE_DELETE_ARRAY3D
#define	SAFE_DELETE_ARRAY3D( p, len )	{ if(p) { for( int i = 0; i < len; i++ ) (p)[i]->Release();	 SAFE_DELETE_ARRAY( (p) );  } }
#endif

namespace TREENODE
{
	enum TNODETYPE
	{
		N_UNKNOWN,
		N_NULLS,
		N_NULL,
		N_ATTACH,
		N_MATERIAL,
		N_TEXTURE,
		N_ANIMATIONS,
		N_ANIMATION,
		N_MESH,
		N_GLOB_RADIUS,
		N_MODEL_NAME,
		N_FPS,
		N_CUST_PARAMS,
		N_CUST_PARAM
	};

	struct TNODE
	{
		TNODE(): _data(NULL), _type(N_UNKNOWN) {}; 
		
		TNODE( enum TNODETYPE type, void* data ) 
		{ 
			_type	= type;
			_data	= data;
		};

		TNODE( void* data ) 
		{ 
			_data	= data;
		};

		~TNODE() {};

		enum TNODETYPE	_type;
		void*			_data;
	};
};