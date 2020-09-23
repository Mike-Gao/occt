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

#include "Sample2D_Markers.h"

#include <Aspect_TypeOfMarker.hxx>
#include <Prs3d_Root.hxx>

// generic marker
Sample2D_Markers::Sample2D_Markers(const Standard_Real theXPosition,
  const Standard_Real theYPosition,
  const Aspect_TypeOfMarker theMarkerType,
  const Quantity_Color theColor,
  const Standard_Real theScaleOrId)
  :AIS_InteractiveObject(), myArrayOfPoints(new Graphic3d_ArrayOfPoints(1))
{
  myXPosition = theXPosition;
  myYPosition = theYPosition;
  myMarkerType = theMarkerType;
  myColor = theColor;
  myIndex = theScaleOrId;
}

Sample2D_Markers::Sample2D_Markers(const Standard_Real theXPosition,
  const Standard_Real theYPosition,
  const Handle(Graphic3d_ArrayOfPoints)& theArrayOfPoints,
  const Aspect_TypeOfMarker theMarkerType,
  const Quantity_Color theColor,
  const Standard_Real theScaleOrId)
  :AIS_InteractiveObject(), myArrayOfPoints(new Graphic3d_ArrayOfPoints(6))
{
  myXPosition = theXPosition;
  myYPosition = theYPosition;
  myMarkerType = theMarkerType;
  myColor = theColor;
  myIndex = theScaleOrId;
  myArrayOfPoints = theArrayOfPoints;
}


void Sample2D_Markers::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
  const Handle(Prs3d_Presentation)& aPresentation,
  const Standard_Integer /*aMode*/)
{
  if (myMarkerType == Aspect_TOM_USERDEFINED) 
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(Aspect_TOM_POINT, myColor, myIndex);
    aPresentation->CurrentGroup()->SetGroupPrimitivesAspect(aMarker);
    aPresentation->CurrentGroup()->AddPrimitiveArray(myArrayOfPoints);
  }
  else 
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(myMarkerType, myColor, myIndex);
    aPresentation->CurrentGroup()->SetPrimitivesAspect(aMarker);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints(1);
    anArrayOfPoints->AddVertex(myXPosition, myYPosition, 0);
    aPresentation->CurrentGroup()->AddPrimitiveArray(anArrayOfPoints);
  }
}
