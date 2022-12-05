// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3TextureEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,TextureEffect,Effect);
WM3_IMPLEMENT_STREAM(TextureEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(TextureEffect,Effect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(TextureEffect,Effect);

//----------------------------------------------------------------------------
TextureEffect::TextureEffect (Texture* pkTexture, Vector2fArray* pkUVs)
{
    Textures.Append(pkTexture);
    UVs.Append(pkUVs);
}
//----------------------------------------------------------------------------
TextureEffect::TextureEffect ()
{
}
//----------------------------------------------------------------------------
TextureEffect::~TextureEffect ()
{
}
//----------------------------------------------------------------------------
Effect* TextureEffect::Clone ()
{
    TextureEffect* pkClone = new TextureEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void TextureEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);
}
//----------------------------------------------------------------------------


