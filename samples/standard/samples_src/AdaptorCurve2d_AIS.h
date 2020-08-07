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

#ifndef ADAPTOR_CURVE2D_AIS_H
#define ADAPTOR_CURVE2D_AIS_H

#include <AIS_InteractiveObject.hxx>
#include <Geom2d_Curve.hxx>
#include <Aspect_TypeOfline.hxx>
#include <Aspect_WidthOfline.hxx>

class AdaptorCurve2d_AIS;
DEFINE_STANDARD_HANDLE(AdaptorCurve2d_AIS, AIS_InteractiveObject)
class AdaptorCurve2d_AIS : public AIS_InteractiveObject
{
public:
  AdaptorCurve2d_AIS(const Handle(Geom2d_Curve) aGeom2dCurve,
                     const Aspect_TypeOfLine aTypeOfline = Aspect_TOL_SOLID,
                     const Aspect_WidthOfLine aWidthOfLine = Aspect_WOL_MEDIUM,
                     const Standard_Integer aColorIndex = 4);

  DEFINE_STANDARD_RTTI_INLINE(AdaptorCurve2d_AIS, AIS_InteractiveObject)

  Standard_Integer   NbPossibleSelection() const { return 1; } ;
  Aspect_TypeOfLine  GetTypeOfLine()       const { return myTypeOfLine; }
  void               SetTypeOfLine(const Aspect_TypeOfLine aNewTypeOfLine) { myTypeOfLine = aNewTypeOfLine; }
  Aspect_WidthOfLine GetWidthOfLine()      const { return myWidthOfLine; }
  void               SetWidthOfLine(const Aspect_WidthOfLine aNewWidthOfLine) { myWidthOfLine = aNewWidthOfLine; }
  Standard_Integer   GetColorIndex()       const { return myColorIndex; }
  void               SetColorIndex(const Standard_Integer aNewColorIndex) { myColorIndex = aNewColorIndex; }
  Standard_Boolean   GetDisplayPole()      const { return myDisplayPole; }
  void               SetDisplayPole(const Standard_Boolean aNewDisplayPole) { myDisplayPole = aNewDisplayPole; }
  Standard_Boolean   GetDisplayCurbure()   const { return myDisplayCurbure; }
  void               SetDisplayCurbure(const Standard_Boolean aNewDisplayCurbure) { myDisplayCurbure = aNewDisplayCurbure; }
  Standard_Real      GetDiscretisation()   const { return myDiscretisation; }
  void               SetDiscretisation(const Standard_Real aNewDiscretisation) { myDiscretisation = aNewDiscretisation; }

private:
  virtual Standard_Boolean AcceptDisplayMode(const Standard_Integer theMode) const { return theMode == 0; }
  virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0);
  void ComputeSelection(const Handle(SelectMgr_Selection)&, const Standard_Integer) {}

  Handle(Geom2d_Curve) myGeom2dCurve;
  Aspect_TypeOfLine   myTypeOfLine;
  Aspect_WidthOfLine  myWidthOfLine;
  Standard_Integer    myColorIndex;
  Standard_Boolean    myDisplayPole;
  Standard_Boolean    myDisplayCurbure;
  Standard_Real       myDiscretisation;
  Standard_Real       myradiusmax;
  Standard_Real       myradiusratio;
};
#endif
