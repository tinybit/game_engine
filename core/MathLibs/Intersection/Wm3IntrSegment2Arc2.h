// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3INTRSEGMENT2ARC2_H
#define WM3INTRSEGMENT2ARC2_H

#include "Wm3Intersector.h"
#include "Wm3Segment2.h"
#include "Wm3Arc2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSegment2Arc2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrSegment2Arc2 (const Segment2<Real>& rkSegment,
        const Arc2<Real>& rkCircle);

    // object access
    const Segment2<Real>& GetSegment () const;
    const Arc2<Real>& GetArc () const;

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
    const Segment2<Real>& m_rkSegment;
    const Arc2<Real>& m_rkArc;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrSegment2Arc2<float> IntrSegment2Arc2f;
typedef IntrSegment2Arc2<double> IntrSegment2Arc2d;

}

#endif


