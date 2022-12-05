// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3DISTLINE3RECTANGLE3_H
#define WM3DISTLINE3RECTANGLE3_H

#include "Wm3Distance.h"
#include "Wm3Line3.h"
#include "Wm3Rectangle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistLine3Rectangle3 : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Rectangle3 (const Line3<Real>& rkLine,
        const Rectangle3<Real>& rkRectangle);

    // object access
    const Line3<Real>& GetLine () const;
    const Rectangle3<Real>& GetRectangle () const;

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
    const Rectangle3<Real>& m_rkRectangle;
};

typedef DistLine3Rectangle3<float> DistLine3Rectangle3f;
typedef DistLine3Rectangle3<double> DistLine3Rectangle3d;

}

#endif


