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

#include <Graphic3d_TextParams.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TextParams, Standard_Transient)

// =======================================================================
// function : Graphic3d_TextParams
// purpose  :
// =======================================================================
Graphic3d_TextParams::Graphic3d_TextParams (const Standard_Real theHeight)
: myHeight (theHeight)
{
  reset();
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Graphic3d_TextParams::Init (const NCollection_String theText,
                                 const Graphic3d_Vertex& thePoint,
                                 const Graphic3d_HorizontalTextAlignment theHta,
                                 const Graphic3d_VerticalTextAlignment theVta)
{
  reset();

  myHasPlane = Standard_False;
  myHasOwnAnchor = Standard_True;

  myText = theText;
  myPoint = thePoint;

  myHAlign = theHta;
  myVAlign = theVta;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Graphic3d_TextParams::Init (const NCollection_String theText,
                                 const gp_Ax2& theOrientation,
                                 const Standard_Boolean theHasOwnAnchor,
                                 const Graphic3d_HorizontalTextAlignment theHta,
                                 const Graphic3d_VerticalTextAlignment theVta)
{
  reset();

  myHasPlane = Standard_True;
  myHasOwnAnchor = theHasOwnAnchor;

  myText = theText;
  myOrientation = theOrientation;
  const gp_Pnt& aPoint = theOrientation.Location();
  myPoint = Graphic3d_Vertex (static_cast<Standard_ShortReal> (aPoint.X()),
                              static_cast<Standard_ShortReal> (aPoint.Y()),
                              static_cast<Standard_ShortReal> (aPoint.Z()));

  myHAlign = theHta;
  myVAlign = theVta;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Graphic3d_TextParams::Init (const Handle(Font_TextFormatter)& theTextFormatter,
                                 const gp_Ax2& theOrientation,
                                 const Standard_Boolean theHasOwnAnchor)
{
  reset();

  myHasPlane = Standard_True;
  myHasOwnAnchor = theHasOwnAnchor;

  myText = "";
  myTextFormatter = theTextFormatter;
  myOrientation = theOrientation;
  const gp_Pnt& aPoint = theOrientation.Location();
  myPoint = Graphic3d_Vertex (static_cast<Standard_ShortReal> (aPoint.X()),
                              static_cast<Standard_ShortReal> (aPoint.Y()),
                              static_cast<Standard_ShortReal> (aPoint.Z()));
}

// =======================================================================
// function : reset
// purpose  :
// =======================================================================
void Graphic3d_TextParams::reset()
{
  myHasPlane = Standard_False;
  myHasOwnAnchor = Standard_True;

  myText = "";
  myTextFormatter = NULL;
  myPoint = Graphic3d_Vertex (0.0f, 0.0f, 0.0f);
  myOrientation = gp_Ax2();

  myHAlign = Graphic3d_HTA_LEFT;
  myVAlign = Graphic3d_VTA_BOTTOM;
}
