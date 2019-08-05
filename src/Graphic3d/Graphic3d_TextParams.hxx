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

#ifndef _Graphic3d_TextParams_HeaderFile
#define _Graphic3d_TextParams_HeaderFile

#include <gp_Ax2.hxx>

#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_TextPath.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <Font_TextFormatter.hxx>

//! This class describes texture parameters.
class Graphic3d_TextParams : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_TextParams, Standard_Transient)
public:

  //! Creates the string <AText> at position <APoint>.
  //! The 3D point of attachment is projected. The text is
  //! written in the plane of projection.
  //! The attributes are given with respect to the plane of
  //! projection.
  //! AHeight : Height of text.
  //! (Relative to the Normalized Projection
  //! Coordinates (NPC) Space).
  //! AAngle  : Orientation of the text
  //! (with respect to the horizontal).
  Standard_EXPORT Graphic3d_TextParams (const Standard_Real theHeight);

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_TextParams() {}

  //! Fill with text and point
  Standard_EXPORT void Init (const NCollection_String theText,
                             const Graphic3d_Vertex& thePoint,
                             const Graphic3d_HorizontalTextAlignment theHta = Graphic3d_HTA_LEFT,
                             const Graphic3d_VerticalTextAlignment theVta = Graphic3d_VTA_BOTTOM);

  //! Fill with text and orientation
  Standard_EXPORT void Init (const NCollection_String theText,
                             const gp_Ax2& theOrientation,
                             const Standard_Boolean theHasOwnAnchor = Standard_True,
                             const Graphic3d_HorizontalTextAlignment theHta = Graphic3d_HTA_LEFT,
                             const Graphic3d_VerticalTextAlignment theVta = Graphic3d_VTA_BOTTOM);

  //! Fill by text formatter
  Standard_EXPORT void Init (const Handle(Font_TextFormatter)& theTextFormatter,
                             const gp_Ax2& theOrientation,
                             const Standard_Boolean theHasOwnAnchor = Standard_True);

  //! Returns true if the text is filled by a point
  Standard_Boolean HasPlane() const { return myHasPlane; }

  const NCollection_String& Text() const { return myText; }

  //! Returns text point.
  const Graphic3d_Vertex& Position() const { return myPoint; }

  //! Sets text point.
  void SetPosition (const Graphic3d_Vertex& thePoint) { myPoint.SetCoord (thePoint.X(), thePoint.Y(), thePoint.Z()); }

  //! Returns true if the text has an anchor point
  Standard_Boolean HasOwnAnchorPoint() const { return myHasOwnAnchor; }

  //! Returns text orientation in 3D space.
  const gp_Ax2& Orientation() const { return myOrientation; }

  Standard_Integer Height() const { return myHeight; }
  void SetHeight (const Standard_Integer theHeight) { myHeight = theHeight; }

  Graphic3d_HorizontalTextAlignment HAlignment() const { return myHAlign; }
  Graphic3d_VerticalTextAlignment VAlignment() const { return myVAlign; }

  //! Returns text formatter
  const Handle(Font_TextFormatter)& TextFormatter() const { return myTextFormatter; }

protected:
  //! Resets parameters to default
  void reset();

private:
  NCollection_String myText;
  Handle(Font_TextFormatter) myTextFormatter;

  bool myHasPlane; //!< Check if text have orientation in 3D space.
  Graphic3d_Vertex myPoint;
  gp_Ax2 myOrientation; //!< Text orientation in 3D space.

private:
  // general properties
  Standard_Integer myHeight;
  Graphic3d_HorizontalTextAlignment myHAlign;
  Graphic3d_VerticalTextAlignment myVAlign;
  Standard_Boolean myHasOwnAnchor;
};

DEFINE_STANDARD_HANDLE(Graphic3d_TextParams, Standard_Transient)

#endif // _Graphic3d_TextParams_HeaderFile
