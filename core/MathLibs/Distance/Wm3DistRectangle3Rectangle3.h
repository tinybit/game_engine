// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3DISTRECTANGLE3RECTANGLE3_H
#define WM3DISTRECTANGLE3RECTANGLE3_H

#include "Wm3Distance.h"
#include "Wm3Rectangle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistRectangle3Rectangle3 : public Distance<Real,Vector3<Real> >
{
public:
    DistRectangle3Rectangle3 (const Rectangle3<Real>& rkRectangle0,
        const Rectangle3<Real>& rkRectangle1);

    // object access
    const Rectangle3<Real>& GetRectangle0 () const;
    const Rectangle3<Real>& GetRectangle1 () const;

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

    const Rectangle3<Real>& m_rkRectangle0;
    const Rectangle3<Real>& m_rkRectangle1;
};

typedef DistRectangle3Rectangle3<float> DistRectangle3Rectangle3f;
typedef DistRectangle3Rectangle3<double> DistRectangle3Rectangle3d;

}

#endif


