// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3CIRCLE2_H
#define WM3CIRCLE2_H

#include "Wm3Vector2.h"

namespace Wm3
{

template <class Real>
class Circle2
{
public:
    // construction
    Circle2 ();  // uninitialized
    Circle2 (const Vector2<Real>& rkCenter, Real fRadius);

    Vector2<Real> Center;
    Real Radius;
};

#include "Wm3Circle2.inl"

typedef Circle2<float> Circle2f;
typedef Circle2<double> Circle2d;

}

#endif


