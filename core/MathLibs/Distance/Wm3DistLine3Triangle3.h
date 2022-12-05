// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3DISTLINE3TRIANGLE3_H
#define WM3DISTLINE3TRIANGLE3_H

#include "Wm3Distance.h"
#include "Wm3Line3.h"
#include "Wm3Triangle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistLine3Triangle3 : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Triangle3 (const Line3<Real>& rkLine,
        const Triangle3<Real>& rkTriangle);

    // object access
    const Line3<Real>& GetLine () const;
    const Triangle3<Real>& GetTriangle () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    using Distance<Real,Vector3<Real> >::m_kClosestPoint0;
    using Distance<Real,Vector3<Real> >::m_kClosestPoint1;

    const Line3<Real>& m_rkLine;
    const Triangle3<Real>& m_rkTriangle;
};

typedef DistLine3Triangle3<float> DistLine3Triangle3f;
typedef DistLine3Triangle3<double> DistLine3Triangle3d;

}

#endif


