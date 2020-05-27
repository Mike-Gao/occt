// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <OpenGl_GraphicPipeline.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore20.hxx>
#include <OpenGl_ShaderManager.hxx>

//! Return GL enumeration for specified face.
static GLenum faceCullToGl (Graphic3d_FaceCulling theFace)
{
  switch (theFace)
  {
    case Graphic3d_FaceCulling_Back:  return GL_BACK;
    case Graphic3d_FaceCulling_Front: return GL_FRONT;
    case Graphic3d_FaceCulling_Both:  return GL_FRONT_AND_BACK;
  }
  return GL_BACK;
}

//! Return GL enumeration for stencil operation.
static GLenum stencilOpToGl (Graphic3d_StencilOp theOp)
{
  switch (theOp)
  {
    case Graphic3d_StencilOp_Keep:      return GL_KEEP;
    case Graphic3d_StencilOp_Zero:      return GL_ZERO;
    case Graphic3d_StencilOp_Replace:   return GL_REPLACE;
    case Graphic3d_StencilOp_IncrClamp: return GL_INCR;
    case Graphic3d_StencilOp_DecrClamp: return GL_DECR;
    case Graphic3d_StencilOp_Invert:    return GL_INVERT;
    case Graphic3d_StencilOp_IncrWrap:  return GL_INCR_WRAP;
    case Graphic3d_StencilOp_DecrWrap:  return GL_DECR_WRAP;
  }
  return GL_KEEP;
}

//! Return GL enumeration for compare function.
static GLenum compareFuncToGl (Graphic3d_CompareFunc theFunc)
{
  switch (theFunc)
  {
    case Graphic3d_CompareFunc_Always:       return GL_ALWAYS;
    case Graphic3d_CompareFunc_Never:        return GL_NEVER;
    case Graphic3d_CompareFunc_Less:         return GL_LESS;
    case Graphic3d_CompareFunc_Equal:        return GL_EQUAL;
    case Graphic3d_CompareFunc_LessEqual:    return GL_LEQUAL;
    case Graphic3d_CompareFunc_Greater:      return GL_GREATER;
    case Graphic3d_CompareFunc_NotEqual:     return GL_NOTEQUAL;
    case Graphic3d_CompareFunc_GreaterEqual: return GL_GEQUAL;
  }
  return GL_ALWAYS;
}

//! Return compare function from GL enumeration.
static Graphic3d_CompareFunc compareFuncFromGl (GLenum theFunc)
{
  switch (theFunc)
  {
    case GL_ALWAYS:   return Graphic3d_CompareFunc_Always;
    case GL_NEVER:    return Graphic3d_CompareFunc_Never;
    case GL_LESS:     return Graphic3d_CompareFunc_Less;
    case GL_EQUAL:    return Graphic3d_CompareFunc_Equal;
    case GL_LEQUAL:   return Graphic3d_CompareFunc_LessEqual;
    case GL_GREATER:  return Graphic3d_CompareFunc_Greater;
    case GL_NOTEQUAL: return Graphic3d_CompareFunc_NotEqual;
    case GL_GEQUAL:   return Graphic3d_CompareFunc_GreaterEqual;
  }
  return Graphic3d_CompareFunc_Always;
}

//! Return stencil operation from GL enumeration.
static Graphic3d_StencilOp stencilOpFromGl (GLenum theOp)
{
  switch (theOp)
  {
    case GL_KEEP:      return Graphic3d_StencilOp_Keep;
    case GL_ZERO:      return Graphic3d_StencilOp_Zero;
    case GL_REPLACE:   return Graphic3d_StencilOp_Replace;
    case GL_INCR:      return Graphic3d_StencilOp_IncrClamp;
    case GL_DECR:      return Graphic3d_StencilOp_DecrClamp;
    case GL_INVERT:    return Graphic3d_StencilOp_Invert;
    case GL_INCR_WRAP: return Graphic3d_StencilOp_IncrWrap;
    case GL_DECR_WRAP: return Graphic3d_StencilOp_DecrWrap;
  }
  return Graphic3d_StencilOp_Keep;
}

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_GraphicPipeline, OpenGl_NamedResource)

// =======================================================================
// function : OpenGl_GraphicPipeline
// purpose  :
// =======================================================================
OpenGl_GraphicPipeline::OpenGl_GraphicPipeline (const TCollection_AsciiString& theName)
: OpenGl_NamedResource (theName)
{
  //
}

// =======================================================================
// function : ~OpenGl_GraphicPipeline
// purpose  :
// =======================================================================
OpenGl_GraphicPipeline::~OpenGl_GraphicPipeline()
{
  //
}

// =======================================================================
// function : ApplyDrawState
// purpose  :
// =======================================================================
void OpenGl_GraphicPipeline::ApplyDrawState (const Handle(OpenGl_Context)& theCtx,
                                             const Handle(OpenGl_ShaderProgram)& theProgram,
                                             const OpenGl_GraphicPipeline* thePrevious)
{
  const Handle(OpenGl_ShaderProgram)& aNewProgram = !theProgram.IsNull() ? theProgram : myProgram;
  theCtx->BindProgram (aNewProgram);
  theCtx->ShaderManager()->PushState (aNewProgram);
  if (thePrevious == this)
  {
    return;
  }

  const Graphic3d_PipelineParams* aPrevParams = thePrevious != NULL ? &thePrevious->myParams : NULL;
  if (aPrevParams == NULL
   || myParams.ToWriteDepth != aPrevParams->ToWriteDepth
   || myParams.DepthCmpFunc != aPrevParams->DepthCmpFunc)
  {
    if (myParams.ToWriteDepth
     || myParams.DepthCmpFunc != Graphic3d_CompareFunc_Always)
    {
      theCtx->core11fwd->glEnable (GL_DEPTH_TEST);
      theCtx->core11fwd->glDepthFunc (compareFuncToGl (myParams.DepthCmpFunc));
      theCtx->core11fwd->glDepthMask (myParams.ToWriteDepth ? GL_TRUE : GL_FALSE);
    }
    else
    {
      theCtx->core11fwd->glDisable (GL_DEPTH_TEST);
    }
  }

  if (aPrevParams == NULL
   || myParams.ToWriteColor != aPrevParams->ToWriteColor)
  {
    theCtx->SetColorMask (myParams.ToWriteColor);
    //const GLboolean toWrite = myParams.ToWriteColor ? GL_TRUE : GL_FALSE; glColorMask (toWrite, toWrite, toWrite, false);
  }

  if (aPrevParams == NULL
   || myParams.ToEnableBlending != aPrevParams->ToEnableBlending)
  {
    if (myParams.ToEnableBlending)
    {
      theCtx->core11fwd->glEnable (GL_BLEND);
      theCtx->core11fwd->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
      theCtx->core11fwd->glDisable (GL_BLEND);
      theCtx->core11fwd->glBlendFunc (GL_ONE, GL_ZERO);
    }
  }

  if (aPrevParams == NULL
   || myParams.CullFaceEnabled != aPrevParams->CullFaceEnabled
   || myParams.CullFace != aPrevParams->CullFace)
  {
    //theCtx->SetCullBackFaces (myParams.CullFaceEnabled);
    if (myParams.CullFaceEnabled)
    {
      theCtx->core11fwd->glCullFace (faceCullToGl (myParams.CullFace));
      theCtx->core11fwd->glEnable (GL_CULL_FACE);
    }
    else
    {
      theCtx->core11fwd->glCullFace (GL_BACK);
      theCtx->core11fwd->glDisable (GL_CULL_FACE);
    }
  }

  if (myParams.StencilEnabled)
  {
    if (aPrevParams == NULL
    || !aPrevParams->StencilEnabled)
    {
      glEnable (GL_STENCIL_TEST);
    }
    if (theCtx->core20fwd != NULL)
    {
      if (aPrevParams == NULL
       || myParams.StencilRef   != aPrevParams->StencilRef
       || myParams.StencilMask  != aPrevParams->StencilMask
       || myParams.StencilFront != aPrevParams->StencilFront)
      {
        theCtx->core20fwd->glStencilFuncSeparate (GL_FRONT, compareFuncToGl (myParams.StencilFront.CmpFunc), myParams.StencilRef, myParams.StencilMask);
        theCtx->core20fwd->glStencilOpSeparate (GL_FRONT, stencilOpToGl (myParams.StencilFront.FailOp), stencilOpToGl (myParams.StencilFront.DepthFailOp), stencilOpToGl (myParams.StencilFront.PassOp));
      }
      if (aPrevParams == NULL
       || myParams.StencilRef  != aPrevParams->StencilRef
       || myParams.StencilMask != aPrevParams->StencilMask
       || myParams.StencilBack != aPrevParams->StencilBack)
      {
        theCtx->core20fwd->glStencilFuncSeparate (GL_BACK, compareFuncToGl (myParams.StencilBack.CmpFunc),  myParams.StencilRef, myParams.StencilMask);
        theCtx->core20fwd->glStencilOpSeparate (GL_BACK, stencilOpToGl (myParams.StencilBack.FailOp),  stencilOpToGl (myParams.StencilBack.DepthFailOp),  stencilOpToGl (myParams.StencilBack.PassOp));
      }
    }
    else
    {
      if (aPrevParams == NULL
       || myParams.StencilRef   != aPrevParams->StencilRef
       || myParams.StencilMask  != aPrevParams->StencilMask
       || myParams.StencilFront != aPrevParams->StencilFront)
      {
        theCtx->core11fwd->glStencilFunc (compareFuncToGl (myParams.StencilFront.CmpFunc), myParams.StencilRef, myParams.StencilMask);
        theCtx->core11fwd->glStencilOp (stencilOpToGl (myParams.StencilFront.FailOp), stencilOpToGl (myParams.StencilFront.DepthFailOp), stencilOpToGl (myParams.StencilFront.PassOp));
      }
    }
  }
  else
  {
    if (aPrevParams == NULL
     || aPrevParams->StencilEnabled)
    {
      glDisable (GL_STENCIL_TEST);
    }
  }
}

// =======================================================================
// function : FrtchDrawState
// purpose  :
// =======================================================================
void OpenGl_GraphicPipeline::FetchDrawState (const Handle(OpenGl_Context)& theCtx)
{
  myProgram = theCtx->ActiveProgram();
  {
    GLboolean wasEnabledTest = GL_FALSE, wasEnabledWrite = GL_FALSE;
    GLint aCompFunc = GL_LESS;
    theCtx->core11fwd->glGetBooleanv (GL_DEPTH_TEST, &wasEnabledTest);
    theCtx->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &wasEnabledWrite);
    theCtx->core11fwd->glGetIntegerv (GL_DEPTH_FUNC, &aCompFunc);
    myParams.ToWriteDepth = wasEnabledWrite == GL_TRUE;
    myParams.DepthCmpFunc = wasEnabledTest ? compareFuncFromGl (aCompFunc) : Graphic3d_CompareFunc_Always;
  }
  {
    GLboolean aColorMask[4] = { GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };
    theCtx->core11fwd->glGetBooleanv (GL_COLOR_WRITEMASK, aColorMask);
    myParams.ToWriteColor = aColorMask[0] == GL_TRUE;
  }
  {
    GLboolean wasEnabledBlending = GL_FALSE;
    theCtx->core11fwd->glGetBooleanv (GL_BLEND, &wasEnabledBlending);
    myParams.ToEnableBlending = wasEnabledBlending == GL_TRUE;
  }
  {
    GLboolean wasEnabledFaceCulling = GL_FALSE;
    theCtx->core11fwd->glGetBooleanv (GL_CULL_FACE, &wasEnabledFaceCulling);
    myParams.CullFaceEnabled = wasEnabledFaceCulling == GL_TRUE;
    GLint aFace = GL_BACK;
    theCtx->core11fwd->glGetIntegerv (GL_CULL_FACE_MODE, &aFace);
    myParams.CullFace = aFace == GL_FRONT_AND_BACK
                      ? Graphic3d_FaceCulling_Both
                      : (aFace == GL_FRONT
                       ? Graphic3d_FaceCulling_Front
                       : Graphic3d_FaceCulling_Back);
  }
  {
    GLboolean wasEnabledStencil = GL_FALSE;
    theCtx->core11fwd->glGetBooleanv (GL_STENCIL_TEST, &wasEnabledStencil);
    myParams.StencilEnabled = wasEnabledStencil == GL_TRUE;
    myParams.StencilFront = Graphic3d_StencilState();
    myParams.StencilBack  = Graphic3d_StencilState();
    {
      GLint aRef = 0, aMask = 0xFF;
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_REF, &aRef);
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_VALUE_MASK, &aMask);
      myParams.StencilRef  = (uint8_t )aRef;
      myParams.StencilMask = (uint8_t )aMask;
    }
    {
      GLint aFrontFunc = GL_ALWAYS;
      GLint aFrontFail = GL_KEEP, aFrontDepthFail = GL_KEEP, aFrontPass = GL_KEEP;
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_FUNC,            &aFrontFunc);
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_FAIL,            &aFrontFail);
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_PASS_DEPTH_FAIL, &aFrontDepthFail);
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_PASS_DEPTH_PASS, &aFrontPass);
      myParams.StencilFront.CmpFunc     = compareFuncFromGl (aFrontFunc);
      myParams.StencilFront.FailOp      = stencilOpFromGl (aFrontFail);
      myParams.StencilFront.DepthFailOp = stencilOpFromGl (aFrontFail);
      myParams.StencilFront.PassOp      = stencilOpFromGl (aFrontFail);
    }
    myParams.StencilBack = myParams.StencilFront;
    if (theCtx->core20fwd != NULL)
    {
      GLint aBackFunc = GL_ALWAYS;
      GLint aBackFail = GL_KEEP, aBackDepthFail = GL_KEEP, aBackPass = GL_KEEP;
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_BACK_FUNC,            &aBackFunc);
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_BACK_FAIL,            &aBackFail);
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_BACK_PASS_DEPTH_FAIL, &aBackDepthFail);
      theCtx->core11fwd->glGetIntegerv (GL_STENCIL_BACK_PASS_DEPTH_PASS, &aBackPass);
      myParams.StencilBack.CmpFunc     = compareFuncFromGl (aBackFunc);
      myParams.StencilBack.FailOp      = stencilOpFromGl (aBackFail);
      myParams.StencilBack.DepthFailOp = stencilOpFromGl (aBackFail);
      myParams.StencilBack.PassOp      = stencilOpFromGl (aBackFail);
    }
  }
}
