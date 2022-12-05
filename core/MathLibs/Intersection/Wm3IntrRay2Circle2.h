// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3INTRRAY2CIRCLE2_H
#define WM3INTRRAY2CIRCLE2_H

#include "Wm3Intersector.h"
#include "Wm3Ray2.h"
#include "Wm3Circle2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrRay2Circle2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrRay2Circle2 (const Ray2<Real>& rkRay, const Circle2<Real>& rkCircle);

    // object access
    const Ray2<Real>& GetRay () const;
    const Circle2<Real>& GetCircle () const;

    // static intersection query
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    // the objects to intersect
    const Ray2<Real>& m_rkRay;
    const Circle2<Real>& m_rkCircle;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrRay2Circle2<float> IntrRay2Circle2f;
typedef IntrRay2Circle2<double> IntrRay2Circle2d;

}

#endif


