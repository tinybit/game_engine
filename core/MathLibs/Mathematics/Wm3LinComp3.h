// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3LINCOMP3_H
#define WM3LINCOMP3_H

#include "Wm3LinComp.h"
#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class LinComp3 : public LinComp<Real>
{
public:
    // P is set to (0,0,0), D is set to (0,0,0), and [min,max] is set to
    // [MAX_REAL,-MAX_REAL].  The type is CT_EMPTY.
    LinComp3 ();

    // P and D are specified.  D must be unit length.
    LinComp3 (const Vector3<Real>& rkOrigin, const Vector3<Real>& rkDirection,
        Real fMin, Real fMax);

    // copy constructor
    LinComp3 (const LinComp3& rkComponent);

    // assignment
    LinComp3& operator= (const LinComp3& rkComponent);

    virtual void MakeCanonical ();

    Vector3<Real> Origin, Direction;

protected:
    using LinComp<Real>::CT_EMPTY;
    using LinComp<Real>::CT_POINT;
    using LinComp<Real>::CT_SEGMENT;
    using LinComp<Real>::CT_RAY;
    using LinComp<Real>::CT_LINE;
    using LinComp<Real>::m_iType;
    using LinComp<Real>::m_fMin;
    using LinComp<Real>::m_fMax;
};

#include "Wm3LinComp3.inl"

typedef LinComp3<float> LinComp3f;
typedef LinComp3<double> LinComp3d;

}

#endif


