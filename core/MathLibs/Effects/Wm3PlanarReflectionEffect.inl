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
inline int PlanarReflectionEffect::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline void PlanarReflectionEffect::SetPlane (int i, TriMeshPtr spkPlane)
{
    assert( 0 <= i && i < m_iQuantity );
    m_aspkPlane[i] = spkPlane;
    m_aspkPlane[i]->ForceCull = true;
}
//----------------------------------------------------------------------------
inline TriMeshPtr PlanarReflectionEffect::GetPlane (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_aspkPlane[i];
}
//----------------------------------------------------------------------------
inline void PlanarReflectionEffect::SetReflectance (int i, float fReflectance)
{
    assert( 0 <= i && i < m_iQuantity );
    m_afReflectance[i] = fReflectance;
}
//----------------------------------------------------------------------------
inline float PlanarReflectionEffect::GetReflectance (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_afReflectance[i];
}
//----------------------------------------------------------------------------


