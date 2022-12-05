// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

//----------------------------------------------------------------------------
template <class Real>
Segment2<Real>::Segment2 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Segment2<Real>::Segment2 (const Vector2<Real>& rkOrigin,
    const Vector2<Real>& rkDirection, Real fExtent)
    :
    Origin(rkOrigin),
    Direction(rkDirection),
    Extent(fExtent)
{
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Segment2<Real>::GetPosEnd () const
{
    return Origin + Direction*Extent;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Segment2<Real>::GetNegEnd () const
{
    return Origin - Direction*Extent;
}
//----------------------------------------------------------------------------


