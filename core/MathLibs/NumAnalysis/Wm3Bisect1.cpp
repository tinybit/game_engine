// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3Bisect1.h"
#include "Wm3Math.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Bisect1<Real>::Bisect1 (Function oF, int iMaxLevel, Real fTolerance)
{
    m_oF = oF;
    m_iMaxLevel = iMaxLevel;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect1<Real>::Bisect (Real fX0, Real fX1, Real& rfRoot)
{
    // test two endpoints
    Real fF0 = m_oF(fX0);
    if ( Math<Real>::FAbs(fF0) <= m_fTolerance )
    {
        rfRoot = fX0;
        return true;
    }

    Real fF1 = m_oF(fX1);
    if ( Math<Real>::FAbs(fF1) <= m_fTolerance )
    {
        rfRoot = fX1;
        return true;
    }

    if ( fF0*fF1 > (Real)0.0 )
        return false;

    for (int iLevel = 0; iLevel < m_iMaxLevel; iLevel++)
    {
        Real fXm = ((Real)0.5)*(fX0+fX1);
        Real fFm = m_oF(fXm);
        if ( Math<Real>::FAbs(fFm) <= m_fTolerance )
        {
            rfRoot = fXm;
            return true;
        }

        if ( fF0*fFm < (Real)0.0 )
        {
            fX1 = fXm;
            fF1 = fFm;
        }
        else if ( fF1*fFm < (Real)0.0 )
        {
            fX0 = fXm;
            fF0 = fFm;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class Bisect1<float>;

// double
template WM3_ITEM
class Bisect1<double>;
//----------------------------------------------------------------------------
}


