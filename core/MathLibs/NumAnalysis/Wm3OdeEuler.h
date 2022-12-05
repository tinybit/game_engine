// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3ODEEULER_H
#define WM3ODEEULER_H

#include "Wm3OdeSolver.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM OdeEuler : public OdeSolver<Real>
{
public:
    OdeEuler (int iDim, Real fStep,
        typename OdeSolver<Real>::Function oFunction, void* pvData = 0);

    virtual ~OdeEuler ();

    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    virtual void SetStepSize (Real fStep);

protected:
    using OdeSolver<Real>::m_iDim;
    using OdeSolver<Real>::m_fStep;
    using OdeSolver<Real>::m_pvData;
    using OdeSolver<Real>::m_afFValue;
};

typedef OdeEuler<float> OdeEulerf;
typedef OdeEuler<double> OdeEulerd;

}

#endif


