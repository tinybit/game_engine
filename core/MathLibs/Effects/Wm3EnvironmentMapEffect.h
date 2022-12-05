// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3ENVIRONMENTMAPEFFECT_H
#define WM3ENVIRONMENTMAPEFFECT_H

#include "Wm3Effect.h"

namespace Wm3
{

class WM3_ITEM EnvironmentMapEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    EnvironmentMapEffect (Image* pkImage, int iApplyMode);
    virtual ~EnvironmentMapEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

protected:
    EnvironmentMapEffect ();
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(EnvironmentMapEffect);
typedef Pointer<EnvironmentMapEffect> EnvironmentMapEffectPtr;

}

#endif


