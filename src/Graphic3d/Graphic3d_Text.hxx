// Copyright (c) 2019 OPEN CASCADE SAS
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

//! This class with text parameters.
//! The text might be defined in some plane and has own attached point.
//! In case if text formatter is used, the text value is store in the formatter.
class Graphic3d_Text : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Text, Standard_Transient)

public:

  Standard_EXPORT Graphic3d_Text (const Standard_Real theHeight);

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_Text() {}

  //! Sets text value.
  const NCollection_String& Text() const { return myText; }

  //! Returns text value.
  void SetText (const NCollection_String& theText) { myText = theText; }

  //! The 3D point of attachment is projected.
  //! If the orientation is defined, the text is written in the plane of projection.
  const gp_Pnt& Position() const { return myOrientation.Location(); }

  //! Sets text point.
  void SetPosition (const gp_Pnt& thePoint) { myOrientation.SetLocation (thePoint); }

  //! Returns text orientation in 3D space.
  const gp_Ax2& Orientation() const { return myOrientation; }

  //! Returns true if the text is filled by a point
  Standard_Boolean HasPlane() const { return myHasPlane; }

  //! Sets text orientation in 3D space.
  void SetOrientation (const gp_Ax2& theOrientation) { myOrientation = theOrientation; myHasPlane = Standard_True; }

  //! Reset text orientation in 3D space.
  void ResetOrientation() { myOrientation = gp_Ax2(); myHasPlane = Standard_False; }

  //! Returns true if the text has an anchor point
  Standard_Boolean HasOwnAnchorPoint() const { return myHasOwnAnchor; }

  //! Returns true if the text has an anchor point
  void SetOwnAnchorPoint (const Standard_Boolean theHasOwnAnchor) { myHasOwnAnchor = theHasOwnAnchor; }

  //! Sets height of text. (Relative to the Normalized Projection Coordinates (NPC) Space).
  Standard_Real Height() const { return myHeight; }

  //! Returns height of text
  void SetHeight (const Standard_Real theHeight) { myHeight = theHeight; }

  //! Returns horizontal alignment of text.
  Graphic3d_HorizontalTextAlignment HAlignment() const { return myHAlign; }

  //! Sets horizontal alignment of text.
  void SetHAlignment (const Graphic3d_HorizontalTextAlignment theJustification) { myHAlign = theJustification; }

  //! Returns vertical alignment of text.
  Graphic3d_VerticalTextAlignment VAlignment() const { return myVAlign; }

  //! Sets vertical alignment of text.
  void SetVAlignment (const Graphic3d_VerticalTextAlignment theJustification) { myVAlign = theJustification; }

  //! Returns text formatter
  const Handle(Font_TextFormatter)& TextFormatter() const { return myTextFormatter; }

  //! Sets text formatter
  void SetTextFormatter (const Handle(Font_TextFormatter)& theTextFormatter) { myTextFormatter = theTextFormatter; }

private:
  NCollection_String myText; //!< text value
  Handle(Font_TextFormatter) myTextFormatter; //!< text formatter collect information about text letter positions

  bool myHasPlane; //!< Check if text have orientation in 3D space.
  gp_Ax2 myOrientation; //!< Text orientation in 3D space.

  Standard_Real myHeight; //!< height of text
  Graphic3d_HorizontalTextAlignment myHAlign; //!< horizontal alignment
  Graphic3d_VerticalTextAlignment myVAlign; //!< vertical alignment
  Standard_Boolean myHasOwnAnchor; //!< flag if text uses position as point of attach
};

DEFINE_STANDARD_HANDLE(Graphic3d_Text, Standard_Transient)

#endif // _Graphic3d_TextParams_HeaderFile
