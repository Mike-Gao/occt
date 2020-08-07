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

#ifndef _Sample2D_Markers_HeaderFile
#define _Sample2D_Markers_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <AIS_InteractiveObject.hxx>

class Sample2D_Markers;
DEFINE_STANDARD_HANDLE(Sample2D_Markers,AIS_InteractiveObject)
class Sample2D_Markers : public AIS_InteractiveObject {

enum Sample2D_CurrentTypeOfMarker {
Sample2D_CTOM_Generic,
Sample2D_CTOM_Polyline,
Sample2D_CTOM_Circle,
Sample2D_CTOM_Ellips
} ;

public:
  DEFINE_STANDARD_RTTI_INLINE(Sample2D_Markers, AIS_InteractiveObject)
 // Generic marker
 Standard_EXPORT Sample2D_Markers (const Standard_Real theXPosition,
                   const Standard_Real theYPosition,
                   const Aspect_TypeOfMarker theMarkerType,
                   const Quantity_Color theColor,
                   const Standard_Real theScaleOrId=5.0);

 // Polyline marker
  Standard_EXPORT Sample2D_Markers (const Standard_Real theXPosition,
                   const Standard_Real theYPosition,
                   const Handle(Graphic3d_ArrayOfPoints)& theArrayOfPoints,
                   const Aspect_TypeOfMarker theMarkerType,
                   const Quantity_Color theColor,
                   const Standard_Real theScaleOrId=2.0);

private: 

virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                      const Handle(Prs3d_Presentation)& /*aPresentation*/,
                      const Standard_Integer /*aMode*/);

virtual void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/,
                               const Standard_Integer /*aMode*/) {}


Standard_EXPORT virtual void SetContext(const Handle(AIS_InteractiveContext)& /*theContext*/) {}



Sample2D_CurrentTypeOfMarker myCurrentTypeOfMarker;
Standard_Real  myXPosition;
Standard_Real  myYPosition;
Aspect_TypeOfMarker myMarkerType;
Quantity_Color myColor;
Standard_Real  myWidth;
Standard_Real  myHeight;
Standard_Real  myIndex;//myScaleOrId
// specific polyline marker
Handle(Graphic3d_ArrayOfPoints) myArrayOfPoints;
};



// other inCurve functions and methods (like "C++: function call" methods)
//



#endif
