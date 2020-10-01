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

#ifndef ADAPTOR_PNT2D_AIS_H
#define ADAPTOR_PNT2D_AIS_H

#include <Geom2d_Point.hxx>
#include <AIS_InteractiveObject.hxx>


class AdaptorPnt2d_AIS;
DEFINE_STANDARD_HANDLE(AdaptorPnt2d_AIS, AIS_InteractiveObject)

//! AIS interactive Object for Geom2d_Point
class AdaptorPnt2d_AIS : public AIS_InteractiveObject
{
public:
  AdaptorPnt2d_AIS(const Handle(Geom2d_Point) thePoint, Standard_Real theElevation = 0)
    : myPoint(thePoint->X(), thePoint->Y(), theElevation)
  { }
  virtual ~AdaptorPnt2d_AIS() { }

  DEFINE_STANDARD_RTTI_INLINE(AdaptorPnt2d_AIS, AIS_InteractiveObject)

private:
  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
               const Handle(Prs3d_Presentation)& aPresentation,
               const Standard_Integer aMode);

  gp_Pnt myPoint;
  void ComputeSelection(const Handle(SelectMgr_Selection)&, const Standard_Integer) { }
};

#endif // ADAPTOR_PNT2D_AIS_H
