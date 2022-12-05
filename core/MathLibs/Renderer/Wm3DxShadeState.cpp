// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3DxRenderer.h"
using namespace Wm3;

DWORD DxRenderer::ms_adwShadeMode[ShadeState::SM_QUANTITY] =
{
    D3DSHADE_FLAT,      // ShadeState::SM_FLAT
    D3DSHADE_GOURAUD,   // ShadeState::SM_SMOOTH
};

//----------------------------------------------------------------------------
void DxRenderer::SetShadeState (ShadeState* pkState)
{
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_SHADEMODE,
        ms_adwShadeMode[pkState->Shade]);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------


