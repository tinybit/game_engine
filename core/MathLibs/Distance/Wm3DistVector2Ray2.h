// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3DISTVECTOR2RAY2_H
#define WM3DISTVECTOR2RAY2_H

#include "Wm3Distance.h"
#include "Wm3Ray2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistVector2Ray2 : public Distance<Real,Vector2<Real> >
{
public:
    DistVector2Ray2 (const Vector2<Real>& rkVector,
        const Ray2<Real>& rkRay);

    // object access
    const Vector2<Real>& GetVector () const;
    const Ray2<Real>& GetRay () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);

private:
    using Distance<Real,Vector2<Real> >::m_kClosestPoint0;
    using Distance<Real,Vector2<Real> >::m_kClosestPoint1;

    const Vector2<Real>& m_rkVector;
    const Ray2<Real>& m_rkRay;
};

typedef DistVector2Ray2<float> DistVector2Ray2f;
typedef DistVector2Ray2<double> DistVector2Ray2d;

}

#endif


