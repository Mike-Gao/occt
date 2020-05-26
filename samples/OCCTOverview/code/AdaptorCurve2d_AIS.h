// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef ADAPTOR_CURVE2D_AIS_H
#define ADAPTOR_CURVE2D_AIS_H

#include <AIS_InteractiveObject.hxx>
#include <Geom2d_Curve.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_WidthOfLine.hxx>

class AdaptorCurve2d_AIS;
DEFINE_STANDARD_HANDLE(AdaptorCurve2d_AIS, AIS_InteractiveObject)

//! AIS interactive Object for Geom2d_Curve
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
