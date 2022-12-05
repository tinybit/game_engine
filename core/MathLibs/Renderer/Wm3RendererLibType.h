// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3RENDERERLIBTYPE_H
#define WM3RENDERERLIBTYPE_H

// For the DLL library.
#ifdef WM3_RENDERER_DLL_EXPORT
#define WM3_RENDERER_ITEM __declspec(dllexport)

// For a client of the DLL library.
#else
#ifdef WM3_RENDERER_DLL_IMPORT
#define WM3_RENDERER_ITEM __declspec(dllimport)

// For the static library.
#else
#define WM3_RENDERER_ITEM

#endif
#endif
#endif


