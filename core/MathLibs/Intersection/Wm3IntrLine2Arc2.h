// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3INTRLINE2ARC2_H
#define WM3INTRLINE2ARC2_H

#include "Wm3Intersector.h"
#include "Wm3Line2.h"
#include "Wm3Arc2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine2Arc2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Arc2 (const Line2<Real>& rkLine, const Arc2<Real>& rkArc);

    // object access
    const Line2<Real>& GetLine () const;
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
    const Line2<Real>& m_rkLine;
    const Arc2<Real>& m_rkArc;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrLine2Arc2<float> IntrLine2Arc2f;
typedef IntrLine2Arc2<double> IntrLine2Arc2d;

}

#endif


