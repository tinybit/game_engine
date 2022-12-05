// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3TEXTUREEFFECT_H
#define WM3TEXTUREEFFECT_H

#include "Wm3Effect.h"

namespace Wm3
{

class WM3_ITEM TextureEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    TextureEffect (Texture* pkTexture, Vector2fArray* pkUVs);
    virtual ~TextureEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

protected:
    TextureEffect ();
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(TextureEffect);
typedef Pointer<TextureEffect> TextureEffectPtr;

}

#endif


