// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef OpenGl_Text_Header
#define OpenGl_Text_Header

#include <OpenGl_Element.hxx>

#include <OpenGl_Aspects.hxx>
#include <NCollection_String.hxx>

#include <OpenGl_TextBuilder.hxx>

#include <TCollection_ExtendedString.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_Text.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>

#include <gp_Ax2.hxx>

class Font_TextFormatter;

//! Text rendering
class OpenGl_Text : public OpenGl_Element
{

public:

  //! Creates new text in 3D space.
  Standard_EXPORT OpenGl_Text (const Handle(Graphic3d_Text)& theTextParams);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_Text();

  //! Setup new string and position
  Standard_EXPORT void Init (const Handle(OpenGl_Context)& theCtx,
                             const Standard_Utf8Char*      theText,
                             const OpenGl_Vec3&            thePoint);

  //! Setup new string and parameters
  Standard_EXPORT void Init (const Handle(OpenGl_Context)& theCtx,
                             const NCollection_String      theText,
                             const OpenGl_Vec3&            thePoint,
                             const Standard_Boolean        theIs2d,
                             const Standard_Real           theHeight,
                             const Graphic3d_HorizontalTextAlignment theHta,
                             const Graphic3d_VerticalTextAlignment theVta);

  //! Setup new font size
  Standard_EXPORT void SetFontSize (const Handle(OpenGl_Context)& theContext,
                                    const Standard_Integer        theFontSize);

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

  //! Return defined text.
  const NCollection_String& Text() const { return myParams->Text(); }

  //! Return text formatting parameters.
  const Handle(Graphic3d_Text)& FormatParams() const { return myParams; }

public: //! @name methods for compatibility with layers

  //! Empty constructor
  Standard_EXPORT OpenGl_Text();

  //! Create key for shared resource
  Standard_EXPORT static TCollection_AsciiString FontKey (const OpenGl_Aspects& theAspect,
                                                          Standard_Integer theHeight,
                                                          unsigned int theResolution);

  //! Find shared resource for specified font or initialize new one
  Standard_EXPORT static Handle(OpenGl_Font) FindFont (const Handle(OpenGl_Context)& theCtx,
                                                       const OpenGl_Aspects& theAspect,
                                                       Standard_Integer theHeight,
                                                       unsigned int theResolution,
                                                       const TCollection_AsciiString& theKey);

  //! Compute text width
  Standard_EXPORT static void StringSize (const Handle(OpenGl_Context)& theCtx,
                                          const NCollection_String&     theText,
                                          const OpenGl_Aspects&         theTextAspect,
                                          const Standard_Real&          theHeight,
                                          const unsigned int            theResolution,
                                          Standard_ShortReal&           theWidth,
                                          Standard_ShortReal&           theAscent,
                                          Standard_ShortReal&           theDescent);

  //! Perform rendering
  Standard_EXPORT void Render (const Handle(OpenGl_Context)& theCtx,
                               const OpenGl_Aspects& theTextAspect,
                               unsigned int theResolution = Graphic3d_RenderingParams::THE_DEFAULT_RESOLUTION) const;

//! @name obsolete methods
public:
  //! Main constructor
  Standard_DEPRECATED("Deprecated method OpenGl_Text() with obsolete arguments")
  Standard_EXPORT OpenGl_Text (const Standard_Utf8Char* theText,
                               const OpenGl_Vec3&       thePoint,
                               const Graphic3d_Text&    theParams);

  //! Creates new text in 3D space.
  Standard_DEPRECATED("Deprecated method OpenGl_Text() with obsolete arguments")
  Standard_EXPORT OpenGl_Text (const Standard_Utf8Char* theText,
                               const gp_Ax2&            theOrientation,
                               const Graphic3d_Text&    theParams,
                               const bool               theHasOwnAnchor = true);

  //! Setup new string and parameters
  Standard_DEPRECATED("Deprecated method Init(), obsolete parameter Graphic3d_Text, use Graphic3d_Text instead of it")
  Standard_EXPORT void Init (const Handle(OpenGl_Context)& theCtx,
                             const Standard_Utf8Char*      theText,
                             const OpenGl_Vec3&            thePoint,
                             const Graphic3d_Text&         theParams);

  //! Setup new position
  Standard_DEPRECATED("Deprecated method SetPosition(), use Graphic3d_Text for it")
  Standard_EXPORT void SetPosition (const OpenGl_Vec3& thePoint);

  //! Setup new string and parameters
  Standard_DEPRECATED("Deprecated method Init(), obsolete parameter Graphic3d_Text, use Graphic3d_Text instead of it")
  Standard_EXPORT void Init (const Handle(OpenGl_Context)&     theCtx,
                             const TCollection_ExtendedString& theText,
                             const OpenGl_Vec2&                thePoint,
                             const Graphic3d_Text&             theParams);

  //! Compute text width
  Standard_DEPRECATED("Deprecated method StringSize() with obsolete Graphic3d_Text argument")
  Standard_EXPORT static void StringSize (const Handle(OpenGl_Context)& theCtx,
                                          const NCollection_String&     theText,
                                          const OpenGl_Aspects&         theTextAspect,
                                          const Graphic3d_Text&         theParams,
                                          const unsigned int            theResolution,
                                          Standard_ShortReal&           theWidth,
                                          Standard_ShortReal&           theAscent,
                                          Standard_ShortReal&           theDescent);

protected:

  friend class OpenGl_Trihedron;
  friend class OpenGl_GraduatedTrihedron;

  //! Release cached VBO resources
  Standard_EXPORT void releaseVbos (OpenGl_Context* theCtx);

  Standard_Boolean hasAnchorPoint() const { return myParams->HasOwnAnchorPoint() != Standard_False;}

private:

  //! Setup matrix.
  void setupMatrix (const Handle(OpenGl_Context)& theCtx,
                    const OpenGl_Aspects& theTextAspect,
                    const OpenGl_Vec3& theDVec) const;

  //! Draw arrays of vertices.
  void drawText (const Handle(OpenGl_Context)& theCtx,
                 const OpenGl_Aspects& theTextAspect) const;

  //! Draw rectangle from bounding text box.
  void drawRect (const Handle(OpenGl_Context)& theCtx,
                 const OpenGl_Aspects& theTextAspect,
                 const OpenGl_Vec4& theColorSubs) const;

  //! Main rendering code
  void render (const Handle(OpenGl_Context)& theCtx,
               const OpenGl_Aspects& theTextAspect,
               const OpenGl_Vec4& theColorText,
               const OpenGl_Vec4& theColorSubs,
               unsigned int theResolution) const;

protected:

  mutable Handle(OpenGl_Font)                             myFont;
  mutable NCollection_Vector<GLuint>                      myTextures;   //!< textures' IDs
  mutable NCollection_Vector<Handle(OpenGl_VertexBuffer)> myVertsVbo;   //!< VBOs of vertices
  mutable NCollection_Vector<Handle(OpenGl_VertexBuffer)> myTCrdsVbo;   //!< VBOs of texture coordinates
  mutable Handle(OpenGl_VertexBuffer)                     myBndVertsVbo;//!< VBOs of vertices for bounding box
  mutable Font_Rect                                       myBndBox;

protected:

  mutable OpenGl_Mat4d myProjMatrix;
  mutable OpenGl_Mat4d myModelMatrix;
  mutable OpenGl_Mat4d myOrientationMatrix;
  mutable GLdouble myWinX;
  mutable GLdouble myWinY;
  mutable GLdouble myWinZ;
  mutable GLdouble myScaleHeight;
  mutable GLdouble myExportHeight;

protected:

  Handle(Graphic3d_Text) myParams;
  bool myIs2d;
 
public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Text_Header
