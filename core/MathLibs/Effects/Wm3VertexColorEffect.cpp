// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3VertexColorEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,VertexColorEffect,Effect);
WM3_IMPLEMENT_STREAM(VertexColorEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(VertexColorEffect,Effect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(VertexColorEffect,Effect);

//----------------------------------------------------------------------------
VertexColorEffect::VertexColorEffect (ColorRGBArray* pkColorRGBs)
{
    ColorRGBs = pkColorRGBs;
}
//----------------------------------------------------------------------------
VertexColorEffect::VertexColorEffect (ColorRGBAArray* pkColorRGBAs)
{
    ColorRGBAs = pkColorRGBAs;
}
//----------------------------------------------------------------------------
VertexColorEffect::VertexColorEffect ()
{
}
//----------------------------------------------------------------------------
VertexColorEffect::~VertexColorEffect ()
{
}
//----------------------------------------------------------------------------
Effect* VertexColorEffect::Clone ()
{
    VertexColorEffect* pkClone = new VertexColorEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void VertexColorEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);
}
//----------------------------------------------------------------------------


