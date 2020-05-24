// Created on: 2013-08-15
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <NCollection_AlignedAllocator.hxx>
#include <OpenGl_CappingPlaneResource.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_CappingPlaneResource,OpenGl_Resource)

namespace
{
  //! 12 plane vertices, interleaved:
  //!  - 4 floats, position
  //!  - 4 floats, normal
  //!  - 4 floats, UV texture coordinates
  static const GLfloat THE_CAPPING_PLN_VERTS[12 * (4 + 4 + 4)] =
  {
    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f,-1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
   -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f,-1.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f, 0.0f
  };

  static const OpenGl_Matrix OpenGl_IdentityMatrix =
  {
    // mat[4][4]
    { { 1.0f, 0.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f } }
  };

  Handle(Graphic3d_Aspects) defaultMaterial()
  {
    Handle(Graphic3d_AspectFillArea3d) anAspect;
    const Graphic3d_MaterialAspect aMaterial (Graphic3d_NOM_DEFAULT);
    anAspect = new Graphic3d_AspectFillArea3d();
    anAspect->SetDistinguishOff();
    anAspect->SetFrontMaterial (aMaterial);
    anAspect->SetInteriorStyle (Aspect_IS_SOLID);
    anAspect->SetInteriorColor (aMaterial.Color());
    anAspect->SetSuppressBackFaces (false);
    return anAspect;
  }
}


// =======================================================================
// function : BuildInfinitPlaneVertices
// purpose  :
// =======================================================================
OpenGl_PrimitiveArray* OpenGl_CappingPlaneResource::BuildInfinitPlaneVertices()
{
  OpenGl_PrimitiveArray* aPrimitives = NULL;
  // Fill primitive array
  Handle(NCollection_AlignedAllocator) anAlloc = new NCollection_AlignedAllocator (16);
  Handle(Graphic3d_Buffer) anAttribs = new Graphic3d_Buffer (anAlloc);
  Graphic3d_Attribute anAttribInfo[] =
  {
    { Graphic3d_TOA_POS,  Graphic3d_TOD_VEC4 },
    { Graphic3d_TOA_NORM, Graphic3d_TOD_VEC4 },
    { Graphic3d_TOA_UV,   Graphic3d_TOD_VEC4 }
  };
  if (anAttribs->Init (12, anAttribInfo, 3))
  {
    memcpy (anAttribs->ChangeData(), THE_CAPPING_PLN_VERTS, sizeof(THE_CAPPING_PLN_VERTS));

    aPrimitives = new OpenGl_PrimitiveArray (NULL);
    aPrimitives->InitBuffers (NULL, Graphic3d_TOPA_TRIANGLES, NULL, anAttribs, NULL);
  }
  return aPrimitives;
}

// =======================================================================
// function : OpenGl_CappingPlaneResource
// purpose  :
// =======================================================================
OpenGl_CappingPlaneResource::OpenGl_CappingPlaneResource (const Handle(Graphic3d_ClipPlane)& thePlane,
                                                          const Handle(Graphic3d_AspectFillCapping)& theAspect)
: myPrimitives  (NULL),
  myPrimitivesUsed (Standard_False),
  myOrientation (OpenGl_IdentityMatrix),
  myAspect      (NULL),
  myPlaneRoot   (thePlane),
  myEquationMod ((unsigned int )-1),
  myAspectMod   ((unsigned int )-1),
  myCappingAspect(),//defaultMaterial()),
  myHatchingAspect(),//defaultMaterial()),
  myHatchingState (0)
{
  if (theAspect.IsNull() || !theAspect->ToDrawHatch())
  {
    // Fill primitive array
    Handle(NCollection_AlignedAllocator) anAlloc = new NCollection_AlignedAllocator (16);
    Handle(Graphic3d_Buffer) anAttribs = new Graphic3d_Buffer (anAlloc);
    Graphic3d_Attribute anAttribInfo[] =
    {
      { Graphic3d_TOA_POS,  Graphic3d_TOD_VEC4 },
      { Graphic3d_TOA_NORM, Graphic3d_TOD_VEC4 },
      { Graphic3d_TOA_UV,   Graphic3d_TOD_VEC4 }
    };
    if (anAttribs->Init (12, anAttribInfo, 3))
    {
      memcpy (anAttribs->ChangeData(), THE_CAPPING_PLN_VERTS, sizeof(THE_CAPPING_PLN_VERTS));
      myPrimitives.InitBuffers (NULL, Graphic3d_TOPA_TRIANGLES, NULL, anAttribs, NULL);
      myPrimitivesUsed = Standard_True;
    }
  }

  myCappingAspect.SetAspect (defaultMaterial());
  myHatchingAspect.SetAspect (defaultMaterial());

  SetAspect (theAspect);
}

// =======================================================================
// function : OpenGl_CappingPlaneResource
// purpose  :
// =======================================================================
OpenGl_CappingPlaneResource::~OpenGl_CappingPlaneResource()
{
  if (myPrimitivesUsed)
  {
    Release (NULL);
  }
}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::SetAspect (const Handle(Graphic3d_AspectFillCapping)& theAspect)
{
  myGraphicAspect = theAspect;

  if (theAspect.IsNull())
  {
    return;
  }

  if (!theAspect->ToDrawHatch())
    return;

  if (!theAspect->ToUseObjectMaterial()
   || !theAspect->ToUseObjectTexture()
   || !theAspect->ToUseObjectShader())
  {
    Handle(Graphic3d_Aspects) aFillAspect = myCappingAspect.Aspect();

    if (!theAspect->ToUseObjectMaterial())
    {
      aFillAspect->SetFrontMaterial (theAspect->Material());
      aFillAspect->SetInteriorColor (theAspect->Material().Color());
    }

    if (!theAspect->ToUseObjectTexture())
    {
      aFillAspect->SetTextureMap (theAspect->Texture());

      if (!theAspect->Texture().IsNull())
      {
        aFillAspect->SetTextureMapOn();
      }
      else
      {
        aFillAspect->SetTextureMapOff();
      }
    }
    else
    {
      aFillAspect->SetTextureMap (Handle(Graphic3d_TextureMap)());
      aFillAspect->SetTextureMapOff();
    }

    if (!theAspect->ToUseObjectShader())
    {
      aFillAspect->SetShaderProgram (theAspect->Shader());
    }

    myCappingAspect.SetAspect (aFillAspect);
  }

  if (theAspect->ToDrawHatch()
    && (theAspect->IsTextureHatch()
     || theAspect->IsStippleHatch()))
  {
    Handle(Graphic3d_Aspects) aFillAspect = myHatchingAspect.Aspect();

    aFillAspect->SetInteriorStyle (theAspect->IsStippleHatch() ? Aspect_IS_HATCH : Aspect_IS_SOLID);
    aFillAspect->SetHatchStyle    (theAspect->IsStippleHatch() ? theAspect->StippleHatch() : Handle(Graphic3d_HatchStyle)());
    aFillAspect->SetTextureMap    (theAspect->IsTextureHatch() ? theAspect->TextureHatch() : Handle(Graphic3d_TextureMap)());
    aFillAspect->SetFrontMaterial (theAspect->HatchMaterial());
    aFillAspect->SetInteriorColor (theAspect->HatchMaterial().Color());
    if (theAspect->IsTextureHatch())
    {
      aFillAspect->SetTextureMapOn();
    }
    else
    {
      aFillAspect->SetTextureMapOff();
    }

    myHatchingAspect.SetAspect (aFillAspect);
    myHatchingState = theAspect->HatchingState();
  }
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::Update (const Handle(OpenGl_Context)& theCtx,
                                          const Handle(Graphic3d_Aspects)& theObjAspect)
{
  if (!myPrimitivesUsed)
    return;

  updateTransform (theCtx);
  updateAspect (theObjAspect);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::Release (OpenGl_Context* theContext)
{
  if (myPrimitivesUsed)
  {
    OpenGl_Element::Destroy (theContext, myAspect);
    myPrimitives.Release (theContext);
    myEquationMod = (unsigned int )-1;
    myAspectMod   = (unsigned int )-1;
  }
  myCappingAspect .Release (theContext);
  myHatchingAspect.Release (theContext);
}

// =======================================================================
// function : CappingFaceAspect
// purpose  :
// =======================================================================
const OpenGl_Aspects* OpenGl_CappingPlaneResource::CappingFaceAspect (const OpenGl_Aspects* theObjectAspect) const
{
  if (myGraphicAspect.IsNull())
  {
    return NULL;
  }

  Handle(Graphic3d_Aspects) aFillAspect = myCappingAspect.Aspect();

  if (myGraphicAspect->ToUseObjectMaterial() && theObjectAspect != NULL)
  {
    // only front material currently supported by capping rendering
    aFillAspect->SetFrontMaterial (theObjectAspect->Aspect()->FrontMaterial());
    aFillAspect->SetInteriorColor (theObjectAspect->Aspect()->InteriorColor());
  }
  else
  {
    aFillAspect->SetFrontMaterial (myGraphicAspect->Material());
    aFillAspect->SetInteriorColor (myGraphicAspect->Material().Color());
  }

  if (myGraphicAspect->ToUseObjectTexture() && theObjectAspect != NULL)
  {
    if (theObjectAspect->Aspect()->ToMapTexture())
    {
      aFillAspect->SetTextureMap (theObjectAspect->Aspect()->TextureMap());
      aFillAspect->SetTextureMapOn();
    }
    else
    {
      aFillAspect->SetTextureMapOff();
    }
  }
  else
  {
    aFillAspect->SetTextureMap (myGraphicAspect->Texture());
    if (!myGraphicAspect->Texture().IsNull())
    {
      aFillAspect->SetTextureMapOn();
    }
    else
    {
      aFillAspect->SetTextureMapOff();
    }
  }

  if (myGraphicAspect->ToUseObjectShader() && theObjectAspect != NULL)
  {
    aFillAspect->SetShaderProgram (theObjectAspect->Aspect()->ShaderProgram());
  }
  else
  {
    aFillAspect->SetShaderProgram (myGraphicAspect->Shader());
  }

  myCappingAspect.SetAspect (aFillAspect);

  return &myCappingAspect;
}

// =======================================================================
// function : HatchingFaceAspect
// purpose  :
// =======================================================================
const OpenGl_Aspects* OpenGl_CappingPlaneResource::HatchingFaceAspect() const
{
  if (myGraphicAspect.IsNull())
  {
    return NULL;
  }

  const Standard_Size aHatchingState = myGraphicAspect->HatchingState();
  if (myHatchingState != aHatchingState)
  {
    if (myGraphicAspect->ToDrawHatch())
    {
      Handle(Graphic3d_Aspects) aFillAspect = myHatchingAspect.Aspect();

      aFillAspect->SetInteriorStyle (myGraphicAspect->IsStippleHatch() ? Aspect_IS_HATCH : Aspect_IS_SOLID);
      aFillAspect->SetHatchStyle    (myGraphicAspect->IsStippleHatch() ? myGraphicAspect->StippleHatch() : Handle(Graphic3d_HatchStyle)());
      aFillAspect->SetTextureMap    (myGraphicAspect->IsTextureHatch() ? myGraphicAspect->TextureHatch() : Handle(Graphic3d_TextureMap)());
      aFillAspect->SetFrontMaterial (myGraphicAspect->HatchMaterial());
      aFillAspect->SetInteriorColor (myGraphicAspect->HatchMaterial().Color());
      if (myGraphicAspect->IsTextureHatch())
      {
        aFillAspect->SetTextureMapOn();
      }
      else
      {
        aFillAspect->SetTextureMapOff();
      }
      myHatchingAspect.SetAspect (aFillAspect);
      myHatchingState = aHatchingState;
    }
  }

  return &myHatchingAspect;
}

// =======================================================================
// function : updateAspect
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::updateAspect (const Handle(Graphic3d_Aspects)& theObjAspect)
{
  if (myPlaneRoot.IsNull())
    return;

  if (myAspect == NULL)
  {
    myAspect = new OpenGl_Aspects();
    myAspectMod = myPlaneRoot->MCountAspect() - 1; // mark out of sync
  }

  if (theObjAspect.IsNull())
  {
    if (myAspectMod != myPlaneRoot->MCountAspect())
    {
      myAspect->SetAspect (myPlaneRoot->CappingAspect());
      myAspectMod = myPlaneRoot->MCountAspect();
    }
    return;
  }

  if (myFillAreaAspect.IsNull())
  {
    myFillAreaAspect = new Graphic3d_AspectFillArea3d();
  }
  if (myAspectMod != myPlaneRoot->MCountAspect()) 
  {
    *myFillAreaAspect = *myPlaneRoot->CappingAspect();
  }

  if (myPlaneRoot->ToUseObjectMaterial())
  {
    // only front material currently supported by capping rendering
    myFillAreaAspect->SetFrontMaterial (theObjAspect->FrontMaterial());
    myFillAreaAspect->SetInteriorColor (theObjAspect->InteriorColor());
  }
  if (myPlaneRoot->ToUseObjectTexture())
  {
    myFillAreaAspect->SetTextureSet (theObjAspect->TextureSet());
    if (theObjAspect->ToMapTexture())
    {
      myFillAreaAspect->SetTextureMapOn();
    }
    else
    {
      myFillAreaAspect->SetTextureMapOff();
    }
  }
  if (myPlaneRoot->ToUseObjectShader())
  {
    myFillAreaAspect->SetShaderProgram (theObjAspect->ShaderProgram());
  }

  myAspect->SetAspect (myFillAreaAspect);
}

// =======================================================================
// function : updateTransform
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::updateTransform (const Handle(OpenGl_Context)& theCtx)
{
  if (myPlaneRoot.IsNull())
    return;

  if (myEquationMod == myPlaneRoot->MCountEquation()
   && myLocalOrigin.IsEqual (theCtx->ShaderManager()->LocalOrigin(), gp::Resolution()))
  {
    return; // nothing to update
  }

  myEquationMod = myPlaneRoot->MCountEquation();
  myLocalOrigin = theCtx->ShaderManager()->LocalOrigin();

  const Graphic3d_ClipPlane::Equation& anEq = myPlaneRoot->GetEquation();
  const Standard_Real anEqW = theCtx->ShaderManager()->LocalClippingPlaneW (*myPlaneRoot);

  // re-evaluate infinite plane transformation matrix
  const Graphic3d_Vec3 aNorm (anEq.xyz());
  const Graphic3d_Vec3 T (anEq.xyz() * -anEqW);

  // project plane normal onto OX to find left vector
  const Standard_ShortReal aProjLen = sqrt ((Standard_ShortReal)anEq.xz().SquareModulus());
  Graphic3d_Vec3 aLeft;
  if (aProjLen < ShortRealSmall())
  {
    aLeft[0] = 1.0f;
  }
  else
  {
    aLeft[0] =  aNorm[2] / aProjLen;
    aLeft[2] = -aNorm[0] / aProjLen;
  }

  const Graphic3d_Vec3 F = Graphic3d_Vec3::Cross (-aLeft, aNorm);

  myOrientation.mat[0][0] = aLeft[0];
  myOrientation.mat[0][1] = aLeft[1];
  myOrientation.mat[0][2] = aLeft[2];
  myOrientation.mat[0][3] = 0.0f;

  myOrientation.mat[1][0] = aNorm[0];
  myOrientation.mat[1][1] = aNorm[1];
  myOrientation.mat[1][2] = aNorm[2];
  myOrientation.mat[1][3] = 0.0f;

  myOrientation.mat[2][0] = F[0];
  myOrientation.mat[2][1] = F[1];
  myOrientation.mat[2][2] = F[2];
  myOrientation.mat[2][3] = 0.0f;

  myOrientation.mat[3][0] = T[0];
  myOrientation.mat[3][1] = T[1];
  myOrientation.mat[3][2] = T[2];
  myOrientation.mat[3][3] = 1.0f;
}
