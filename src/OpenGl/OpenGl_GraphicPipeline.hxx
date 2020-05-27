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

#ifndef _OpenGl_GraphicPipeline_HeaderFile
#define _OpenGl_GraphicPipeline_HeaderFile

#include <Graphic3d_PipelineParams.hxx>
#include <OpenGl_NamedResource.hxx>

//! Object defining properties of rendering pipeline (shading program + other states).
class OpenGl_GraphicPipeline : public OpenGl_NamedResource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_GraphicPipeline, OpenGl_NamedResource)
public:

  //! Creates new graphics object.
  OpenGl_GraphicPipeline (const TCollection_AsciiString& theName = "");

  //! Releases graphic resources.
  virtual ~OpenGl_GraphicPipeline();

  //! Release resources.
  virtual void Release (OpenGl_Context* ) Standard_OVERRIDE {}

  //! Returns estimated GPU memory usage.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

  //! Return pipeline parameters.
  const Graphic3d_PipelineParams& Params() const { return myParams; }

  //! Change pipeline parameters.
  Graphic3d_PipelineParams& ChangeParams() { return myParams; }

  //! Return default program for this pipeline.
  const Handle(OpenGl_ShaderProgram)& Program() const { return myProgram; }

  //! Set default program for this pipeline.
  void SetProgram (const Handle(OpenGl_ShaderProgram)& theProgram) { myProgram = theProgram; }

  //! Read active drawing state from OpenGL context.
  void FetchDrawState (const Handle(OpenGl_Context)& theCtx);

  //! Apply new drawing state.
  //! @param theCtx [in] active OpenGL context
  //! @param theProgram [in] program to activate or NULL to activate default
  //! @param thePrevious [in] if not NULL, only states different from previous one are modified
  void ApplyDrawState (const Handle(OpenGl_Context)& theCtx,
                       const Handle(OpenGl_ShaderProgram)& theProgram,
                       const OpenGl_GraphicPipeline* thePrevious);

protected:

  Handle(OpenGl_ShaderProgram) myProgram; //!< default pipeline program
  Graphic3d_PipelineParams     myParams;  //!< drawing parameters

};

#endif // _OpenGl_GraphicPipeline_HeaderFile
