// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3OdeRungeKutta4.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
OdeRungeKutta4<Real>::OdeRungeKutta4 (int iDim, Real fStep,
    typename OdeSolver<Real>::Function oFunction, void* pvData)
    :
    OdeSolver<Real>(iDim,fStep,oFunction,pvData)
{
    m_fHalfStep = ((Real)0.5)*fStep;
    m_fSixthStep = fStep/((Real)6.0);
    m_afTemp1 = new Real[iDim];
    m_afTemp2 = new Real[iDim];
    m_afTemp3 = new Real[iDim];
    m_afTemp4 = new Real[iDim];
    m_afXTemp = new Real[iDim];
}
//----------------------------------------------------------------------------
template <class Real>
OdeRungeKutta4<Real>::~OdeRungeKutta4 ()
{
    delete[] m_afTemp1;
    delete[] m_afTemp2;
    delete[] m_afTemp3;
    delete[] m_afTemp4;
    delete[] m_afXTemp;
}
//----------------------------------------------------------------------------
template <class Real>
void OdeRungeKutta4<Real>::Update (Real fTIn, Real* afXIn, Real& rfTOut,
    Real* afXOut)
{
    int i;

    // first step
    m_oFunction(fTIn,afXIn,m_pvData,m_afTemp1);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp1[i];

    // second step
    Real fHalfT = fTIn + m_fHalfStep;
    m_oFunction(fHalfT,m_afXTemp,m_pvData,m_afTemp2);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp2[i];

    // third step
    m_oFunction(fHalfT,m_afXTemp,m_pvData,m_afTemp3);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fStep*m_afTemp3[i];

    // fourth step
    rfTOut = fTIn + m_fStep;
    m_oFunction(rfTOut,m_afXTemp,m_pvData,m_afTemp4);
    for (i = 0; i < m_iDim; i++)
    {
        afXOut[i] = afXIn[i] + m_fSixthStep*(m_afTemp1[i] +
            ((Real)2.0)*(m_afTemp2[i] + m_afTemp3[i]) + m_afTemp4[i]);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void OdeRungeKutta4<Real>::SetStepSize (Real fStep)
{
    m_fStep = fStep;
    m_fHalfStep = ((Real)0.5)*fStep;
    m_fSixthStep = fStep/((Real)6.0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class OdeRungeKutta4<float>;

// double
template WM3_ITEM
class OdeRungeKutta4<double>;
//----------------------------------------------------------------------------
}


