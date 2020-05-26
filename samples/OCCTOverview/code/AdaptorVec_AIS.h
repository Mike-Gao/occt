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

#ifndef ADAPTOR_VEC_AIS_H
#define ADAPTOR_VEC_AIS_H

#include <AIS_InteractiveObject.hxx>

class AdaptorVec_AIS;
DEFINE_STANDARD_HANDLE(AdaptorVec_AIS, AIS_InteractiveObject)

//! AIS interactive Object for vector with arrow and text
class AdaptorVec_AIS : public AIS_InteractiveObject
{
public:

  AdaptorVec_AIS() { }
  AdaptorVec_AIS(const gp_Pnt& aPnt, const gp_Dir& aDir, Standard_Real aLength = 1, Standard_Real anArrowLength = 1)
    : myPnt(aPnt),
    myDir(aDir),
    myLength(aLength),
    myArrowLength(anArrowLength)
  {}

  AdaptorVec_AIS(const gp_Pnt& aPnt, const gp_Vec& aVec, Standard_Real anArrowLength = 1)
    : myPnt(aPnt),
    myDir(aVec),
    myArrowLength(anArrowLength)
  {
    myLength = aVec.Magnitude();
  }

  AdaptorVec_AIS(const gp_Pnt2d& aPnt2d, const gp_Dir2d& aDir2d, Standard_Real aLength = 1, Standard_Real anArrowLength = 1)
    : myPnt(gp_Pnt(aPnt2d.X(), aPnt2d.Y(), 0.0)),
    myDir(gp_Dir(aDir2d.X(), aDir2d.Y(), 0.0)),
    myLength(aLength),
    myArrowLength(anArrowLength) { }

  AdaptorVec_AIS(const gp_Pnt2d& aPnt2d, const gp_Vec2d& aVec2d, Standard_Real anArrowLength = 1)
    : myPnt(gp_Pnt(aPnt2d.X(), aPnt2d.Y(), 0.0)),
    myDir(gp_Dir(aVec2d.X(), aVec2d.Y(), 0.0)),
    myArrowLength(anArrowLength)
  {
    myLength = aVec2d.Magnitude();
  }
  AdaptorVec_AIS(const gp_Pnt2d& aPnt1, const gp_Pnt2d& aPnt2, Standard_Real anArrowLength = 1);

  virtual ~AdaptorVec_AIS() { }

  void SetText(const TCollection_AsciiString& theText)
  {
    myText = theText;
  }

  void SetLineAspect(const Handle(Prs3d_LineAspect)& theAspect)
  {
    myDrawer->SetLineAspect(theAspect);
  }

  DEFINE_STANDARD_RTTI_INLINE(AdaptorVec_AIS, AIS_InteractiveObject)

private:

  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
               const Handle(Prs3d_Presentation)& aPresentation,
               const Standard_Integer aMode);

  void ComputeSelection(const Handle(SelectMgr_Selection)&, const Standard_Integer) { }

  gp_Pnt myPnt;
  gp_Dir myDir;
  Standard_Real myLength;
  Standard_Real myArrowLength;
  TCollection_AsciiString myText;
};

#endif // ADAPTOR_VEC2D_AIS_H
