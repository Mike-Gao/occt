// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_Workspace.hxx>

#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_GlCore15.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Window.hxx>

#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <NCollection_AlignedAllocator.hxx>

//#define DEBUG_INFO
#ifdef DEBUG_INFO
#include <Message_Alerts.hxx>
#include <Message_Level.hxx>
#endif // DEBUG_INFO

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Workspace,Standard_Transient)

namespace
{
  static const OpenGl_Vec4 THE_WHITE_COLOR (1.0f, 1.0f, 1.0f, 1.0f);
  static const OpenGl_Vec4 THE_BLACK_COLOR (0.0f, 0.0f, 0.0f, 1.0f);

  static const OpenGl_Matrix myDefaultMatrix =
  {
    {{ 1.0F, 0.0F, 0.0F, 0.0F },
     { 0.0F, 1.0F, 0.0F, 0.0F },
     { 0.0F, 0.0F, 1.0F, 0.0F },
     { 0.0F, 0.0F, 0.0F, 1.0F }}
  };

}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Material::Init (const Graphic3d_MaterialAspect& theMat,
                            const Quantity_Color&           theInteriorColor)
{
  const bool isPhysic = theMat.MaterialType (Graphic3d_MATERIAL_PHYSIC);
  ChangeShine()        = 128.0f * theMat.Shininess();
  ChangeTransparency() = theMat.Alpha();

  // ambient component
  if (theMat.ReflectionMode (Graphic3d_TOR_AMBIENT))
  {
    const OpenGl_Vec3& aSrcAmb = isPhysic ? theMat.AmbientColor() : theInteriorColor;
    Ambient = OpenGl_Vec4 (aSrcAmb * theMat.Ambient(), 1.0f);
  }
  else
  {
    Ambient = THE_BLACK_COLOR;
  }

  // diffusion component
  if (theMat.ReflectionMode (Graphic3d_TOR_DIFFUSE))
  {
    const OpenGl_Vec3& aSrcDif = isPhysic ? theMat.DiffuseColor() : theInteriorColor;
    Diffuse = OpenGl_Vec4 (aSrcDif * theMat.Diffuse(), 1.0f);
  }
  else
  {
    Diffuse = THE_BLACK_COLOR;
  }

  // specular component
  if (theMat.ReflectionMode (Graphic3d_TOR_SPECULAR))
  {
    const OpenGl_Vec3& aSrcSpe = isPhysic ? (const OpenGl_Vec3& )theMat.SpecularColor() : THE_WHITE_COLOR.rgb();
    Specular = OpenGl_Vec4 (aSrcSpe * theMat.Specular(), 1.0f);
  }
  else
  {
    Specular = THE_BLACK_COLOR;
  }

  // emission component
  if (theMat.ReflectionMode (Graphic3d_TOR_EMISSION))
  {
    const OpenGl_Vec3& aSrcEms = isPhysic ? theMat.EmissiveColor() : theInteriorColor;
    Emission = OpenGl_Vec4 (aSrcEms * theMat.Emissive(), 1.0f);
  }
  else
  {
    Emission = THE_BLACK_COLOR;
  }
}

// =======================================================================
// function : OpenGl_Workspace
// purpose  :
// =======================================================================
OpenGl_Workspace::OpenGl_Workspace (OpenGl_View* theView, const Handle(OpenGl_Window)& theWindow)
: myView (theView),
  myWindow (theWindow),
  myGlContext (!theWindow.IsNull() ? theWindow->GetGlContext() : NULL),
  myUseZBuffer    (Standard_True),
  myUseDepthWrite (Standard_True),
  //
  myNbSkippedTranspElems (0),
  myRenderFilter (OpenGl_RenderFilter_Empty),
  //
  myAspectsSet (&myDefaultAspects),
  //
  ViewMatrix_applied (&myDefaultMatrix),
  StructureMatrix_applied (&myDefaultMatrix),
  myToAllowFaceCulling (false),
  myModelViewMatrix (myDefaultMatrix)
{
  if (!myGlContext.IsNull() && myGlContext->MakeCurrent())
  {
    myGlContext->core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    // General initialization of the context
  #if !defined(GL_ES_VERSION_2_0)
    if (myGlContext->core11 != NULL)
    {
      // enable two-side lighting by default
      glLightModeli ((GLenum )GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      glHint (GL_POINT_SMOOTH_HINT, GL_FASTEST);
      if (myGlContext->caps->ffpEnable)
      {
        glHint (GL_FOG_HINT, GL_FASTEST);
      }
    }

    glHint (GL_LINE_SMOOTH_HINT,    GL_FASTEST);
    glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  #endif
  }

  myNoneCulling .Aspect()->SetSuppressBackFaces (false);
  myNoneCulling .Aspect()->SetDrawEdges (false);
  myNoneCulling .Aspect()->SetAlphaMode (Graphic3d_AlphaMode_Opaque);

  myFrontCulling.Aspect()->SetSuppressBackFaces (true);
  myFrontCulling.Aspect()->SetDrawEdges (false);
  myFrontCulling.Aspect()->SetAlphaMode (Graphic3d_AlphaMode_Opaque);
}

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::Activate()
{
  if (myWindow.IsNull() || !myWindow->Activate())
  {
    return Standard_False;
  }

  ViewMatrix_applied      = &myDefaultMatrix;
  StructureMatrix_applied = &myDefaultMatrix;

  ResetAppliedAspect();

  // reset state for safety
  myGlContext->BindProgram (Handle(OpenGl_ShaderProgram)());
  if (myGlContext->core20fwd != NULL)
  {
    myGlContext->core20fwd->glUseProgram (OpenGl_ShaderProgram::NO_PROGRAM);
  }
  if (myGlContext->caps->ffpEnable)
  {
    myGlContext->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());
  }
  return Standard_True;
}

//=======================================================================
//function : ResetAppliedAspect
//purpose  : Sets default values of GL parameters in accordance with default aspects
//=======================================================================
void OpenGl_Workspace::ResetAppliedAspect()
{
  myGlContext->BindDefaultVao();

  myHighlightStyle.Nullify();
  myToAllowFaceCulling  = false;
  myAspectsSet = &myDefaultAspects;
  myAspectsApplied.Nullify();
  myGlContext->SetPolygonOffset (Graphic3d_PolygonOffset());

  ApplyAspects();
  myGlContext->SetTypeOfLine (myDefaultAspects.Aspect()->LineType());
  myGlContext->SetLineWidth  (myDefaultAspects.Aspect()->LineWidth());
  if (myGlContext->core15fwd != NULL)
  {
    myGlContext->core15fwd->glActiveTexture (GL_TEXTURE0);
  }
}

// =======================================================================
// function : SetDefaultPolygonOffset
// purpose  :
// =======================================================================
Graphic3d_PolygonOffset OpenGl_Workspace::SetDefaultPolygonOffset (const Graphic3d_PolygonOffset& theOffset)
{
  Graphic3d_PolygonOffset aPrev = myDefaultAspects.Aspect()->PolygonOffset();
  myDefaultAspects.Aspect()->SetPolygonOffset (theOffset);
  if (myAspectsApplied == myDefaultAspects.Aspect()
   || myAspectsApplied.IsNull()
   || (myAspectsApplied->PolygonOffset().Mode & Aspect_POM_None) == Aspect_POM_None)
  {
    myGlContext->SetPolygonOffset (theOffset);
  }
  return aPrev;
}

// =======================================================================
// function : SetAspects
// purpose  :
// =======================================================================
const OpenGl_Aspects* OpenGl_Workspace::SetAspects (const OpenGl_Aspects* theAspect)
{
  const OpenGl_Aspects* aPrevAspects = myAspectsSet;
  myAspectsSet = theAspect;
  return aPrevAspects;
}

// =======================================================================
// function : ApplyAspects
// purpose  :
// =======================================================================
const OpenGl_Aspects* OpenGl_Workspace::ApplyAspects()
{
  if (myView->BackfacingModel() == Graphic3d_TOBM_AUTOMATIC)
  {
    bool toSuppressBackFaces = myToAllowFaceCulling
                            && myAspectsSet->Aspect()->ToSuppressBackFaces();
    if (toSuppressBackFaces)
    {
      if (myAspectsSet->Aspect()->InteriorStyle() == Aspect_IS_HATCH
       || myAspectsSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_Blend
       || myAspectsSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_Mask
       || (myAspectsSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_BlendAuto
        && myAspectsSet->Aspect()->FrontMaterial().Transparency() != 0.0f))
      {
        // disable culling in case of translucent shading aspect
        toSuppressBackFaces = false;
      }
    }
    myGlContext->SetCullBackFaces (toSuppressBackFaces);
  }

  if (myAspectsSet->Aspect() == myAspectsApplied
   && myHighlightStyle == myAspectFaceAppliedWithHL)
  {
    return myAspectsSet;
  }
  myAspectFaceAppliedWithHL = myHighlightStyle;

  // Aspect_POM_None means: do not change current settings
  if ((myAspectsSet->Aspect()->PolygonOffset().Mode & Aspect_POM_None) != Aspect_POM_None)
  {
    myGlContext->SetPolygonOffset (myAspectsSet->Aspect()->PolygonOffset());
  }

  const Aspect_InteriorStyle anIntstyle = myAspectsSet->Aspect()->InteriorStyle();
  if (myAspectsApplied.IsNull()
   || myAspectsApplied->InteriorStyle() != anIntstyle)
  {
  #if !defined(GL_ES_VERSION_2_0)
    myGlContext->SetPolygonMode (anIntstyle == Aspect_IS_POINT ? GL_POINT : GL_FILL);
    myGlContext->SetPolygonHatchEnabled (anIntstyle == Aspect_IS_HATCH);
  #endif
  }

#if !defined(GL_ES_VERSION_2_0)
  if (anIntstyle == Aspect_IS_HATCH)
  {
    myGlContext->SetPolygonHatchStyle (myAspectsSet->Aspect()->HatchStyle());
  }
#endif

  // Case of hidden line
  if (anIntstyle == Aspect_IS_HIDDENLINE)
  {
    // copy all values including line edge aspect
    *myAspectFaceHl.Aspect() = *myAspectsSet->Aspect();
    myAspectFaceHl.Aspect()->SetShadingModel (Graphic3d_TOSM_UNLIT);
    myAspectFaceHl.Aspect()->SetInteriorColor (myView->BackgroundColor().GetRGB());
    myAspectFaceHl.Aspect()->SetDistinguish (false);
    myAspectFaceHl.SetNoLighting();
    myAspectsSet = &myAspectFaceHl;
  }
  else
  {
    myGlContext->SetShadingMaterial (myAspectsSet, myHighlightStyle);
  }

  const Handle(OpenGl_TextureSet)& aTextureSet = myAspectsSet->TextureSet (myGlContext, ToHighlight());
  if (!aTextureSet.IsNull()
   || myAspectsSet->Aspect()->ToMapTexture())
  {
    myGlContext->BindTextures (aTextureSet);
  }
  else
  {
    myGlContext->BindTextures (myEnvironmentTexture);
  }

  myAspectsApplied = myAspectsSet->Aspect();
  return myAspectsSet;
}

// =======================================================================
// function : Width
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Workspace::Width()  const
{
  return !myView->GlWindow().IsNull() ? myView->GlWindow()->Width() : 0;
}

// =======================================================================
// function : Height
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Workspace::Height() const
{
  return !myView->GlWindow().IsNull() ? myView->GlWindow()->Height() : 0;
}

// =======================================================================
// function : FBOCreate
// purpose  :
// =======================================================================
Handle(OpenGl_FrameBuffer) OpenGl_Workspace::FBOCreate (const Standard_Integer theWidth,
                                                        const Standard_Integer theHeight)
{
  // activate OpenGL context
  if (!Activate())
    return Handle(OpenGl_FrameBuffer)();

  // create the FBO
  const Handle(OpenGl_Context)& aCtx = GetGlContext();
  aCtx->BindTextures (Handle(OpenGl_TextureSet)());
  Handle(OpenGl_FrameBuffer) aFrameBuffer = new OpenGl_FrameBuffer();
  if (!aFrameBuffer->Init (aCtx, theWidth, theHeight, GL_RGBA8, GL_DEPTH24_STENCIL8, 0))
  {
    aFrameBuffer->Release (aCtx.operator->());
    return Handle(OpenGl_FrameBuffer)();
  }
  return aFrameBuffer;
}

// =======================================================================
// function : FBORelease
// purpose  :
// =======================================================================
void OpenGl_Workspace::FBORelease (Handle(OpenGl_FrameBuffer)& theFbo)
{
  // activate OpenGL context
  if (!Activate()
   || theFbo.IsNull())
  {
    return;
  }

  theFbo->Release (GetGlContext().operator->());
  theFbo.Nullify();
}

// =======================================================================
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::BufferDump (const Handle(OpenGl_FrameBuffer)& theFbo,
                                               Image_PixMap&                     theImage,
                                               const Graphic3d_BufferType&       theBufferType)
{
  return !theImage.IsEmpty()
      && Activate()
      && OpenGl_FrameBuffer::BufferDump (GetGlContext(), theFbo, theImage, theBufferType);
}

// =======================================================================
// function : ShouldRender
// purpose  :
// =======================================================================
bool OpenGl_Workspace::ShouldRender (const OpenGl_Element* theElement)
{
#ifdef DEBUG_INFO
  MESSAGE_ADD_LEVEL_SENTRY
  MESSAGE_INFO_OBJECT(this, "Workspace")

  Standard_SStream aWorkspaceStream;
  DumpJson (aWorkspaceStream);
  MESSAGE_INFO_STREAM(aWorkspaceStream, "parameters")
#endif

  // render only non-raytracable elements when RayTracing is enabled
  if ((myRenderFilter & OpenGl_RenderFilter_NonRaytraceableOnly) != 0)
  {
    if (OpenGl_Raytrace::IsRaytracedElement (theElement))
    {
      return false;
    }
  }
  else if ((myRenderFilter & OpenGl_RenderFilter_FillModeOnly) != 0)
  {
    if (!theElement->IsFillDrawMode())
    {
      return false;
    }
  }

  // handle opaque/transparency render passes
  if ((myRenderFilter & OpenGl_RenderFilter_OpaqueOnly) != 0)
  {
    if (!theElement->IsFillDrawMode())
    {
      return true;
    }

    if (OpenGl_Context::CheckIsTransparent (myAspectsSet, myHighlightStyle))
    {
      ++myNbSkippedTranspElems;
      return false;
    }
  }
  else if ((myRenderFilter & OpenGl_RenderFilter_TransparentOnly) != 0)
  {
    if (!theElement->IsFillDrawMode())
    {
      if (dynamic_cast<const OpenGl_Aspects*> (theElement) == NULL)
      {
        return false;
      }
    }
    else if (!OpenGl_Context::CheckIsTransparent (myAspectsSet, myHighlightStyle))
    {
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_Workspace::DumpJson (Standard_OStream& theOStream, const Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, OpenGl_Workspace);

  //OpenGl_View*           myView;
  //Handle(OpenGl_Window)  myWindow;
  //Handle(OpenGl_Context) myGlContext;

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUseZBuffer);
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUseDepthWrite);
  
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myNoneCulling);
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myFrontCulling);
  
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myNbSkippedTranspElems);
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myRenderFilter);
  
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myDefaultAspects);

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myAspectsSet);
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myAspectsApplied.get());
  //Handle(Graphic3d_PresentationAttributes) myAspectFaceAppliedWithHL;

  //const OpenGl_Matrix* ViewMatrix_applied;
  //const OpenGl_Matrix* StructureMatrix_applied;
  
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToAllowFaceCulling);
  //Handle(Graphic3d_PresentationAttributes) myHighlightStyle;
  //OpenGl_Matrix myModelViewMatrix;
  
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAspectFaceHl);
  //Handle(OpenGl_TextureSet) myEnvironmentTexture;
}
