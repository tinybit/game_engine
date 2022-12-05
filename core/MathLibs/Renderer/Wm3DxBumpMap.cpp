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
#include "Wm3BumpMapEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void DxRenderer::DrawBumpMap ()
{
    assert( m_pkGeometry->GeometryType == Geometry::GT_TRIMESH );
    assert( DynamicCast<BumpMapEffect>(m_pkGeometry->GetEffect()) );
    BumpMapEffect* pkBMEffect = (BumpMapEffect*)m_pkGeometry->GetEffect();

    // set up the model-to-world transformation
    SetWorldTransformation();

    // set the vertex array
    EnableVertices();

    //*** FIRST PASS
    SetGlobalState(m_pkGeometry->States);

    // no lighting, the color array stores the light vectors
    pkBMEffect->ComputeLightVectors(m_pkGeometry);
    EnableColorRGBs();

    // texture unit 0 handles the normal map
    EnableTexture(0,1,pkBMEffect);

    // draw the object
    DrawElements();

    //*** SECOND PASS

    // enable lighting
    EnableLighting();

    // set alpha blend
    AlphaState kAS;
    kAS.BlendEnabled = true;
    kAS.SrcBlend = AlphaState::SBF_DST_COLOR;
    kAS.DstBlend = AlphaState::DBF_ZERO;
    SetAlphaState(&kAS);

    // set the normal array
    EnableNormals();

    // texture unit 0 handles the primary texture
    EnableTexture(0,0,pkBMEffect);

    // other textures handled by the remaining texture units
    int iTMax = pkBMEffect->Textures.GetQuantity();
    if ( iTMax > m_iMaxTextures )
        iTMax = m_iMaxTextures;

    int i, iUnit;
    for (i = 2, iUnit = 1; i < iTMax; i++)
        EnableTexture(iUnit++,i,pkBMEffect);

    // draw the object
    DrawElements();

    // Clean up state:

    // disable texture states
    DisableTexture(0,0,pkBMEffect);
    for (i = 2, iUnit = 1; i < iTMax; i++)
        DisableTexture(iUnit++,i,pkBMEffect);

    // disable normals
    DisableNormals();

    // disable color state
    DisableColorRGBs();

    // disable lighting
    DisableLighting();

    // disable vertices
    DisableVertices();

    // restore the default alpha state
    GlobalState* pkAState = GlobalState::Default[GlobalState::ALPHA];
    SetAlphaState((AlphaState*)pkAState);

    // restore the model-to-world transformation
    RestoreWorldTransformation();
}
//----------------------------------------------------------------------------


