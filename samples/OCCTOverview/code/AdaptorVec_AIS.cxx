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

#include "AdaptorVec_AIS.h"

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Text.hxx>
#include <DsgPrs_LengthPresentation.hxx>


AdaptorVec_AIS::AdaptorVec_AIS(const gp_Pnt2d & aPnt1, const gp_Pnt2d & aPnt2, Standard_Real anArrowLength)
  : myPnt(gp_Pnt(aPnt1.X(), aPnt1.Y(), 0.0)),
  myArrowLength(anArrowLength)
{
  gp_Vec aVec(aPnt2.X() - aPnt1.X(), aPnt2.Y() - aPnt1.Y(), 0.0);
  myDir = gp_Dir(aVec);
  myLength = aVec.Magnitude();
}


void AdaptorVec_AIS::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer /*aMode*/)
{
  // Set style for arrow
  Handle(Prs3d_ArrowAspect) anArrowAspect = myDrawer->ArrowAspect();
  anArrowAspect->SetLength(myArrowLength);

  gp_Pnt aLastPoint = myPnt;
  aLastPoint.Translate(myLength*gp_Vec(myDir));

  // Draw Line
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(myPnt);
  aPrims->AddVertex(aLastPoint);
  aPresentation->CurrentGroup()->SetPrimitivesAspect(myDrawer->LineAspect()->Aspect());
  aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);
  // Draw arrow
  Prs3d_Arrow::Draw(aPresentation->CurrentGroup(),
                    aLastPoint,
                    myDir,
                    anArrowAspect->Angle(),
                    anArrowAspect->Length());

  // Draw text
  if (myText.Length() != 0) 
  {
    gp_Pnt aTextPosition = aLastPoint;
    Prs3d_Text::Draw(aPresentation->CurrentGroup(),
                     myDrawer->TextAspect(),
                     myText,
                     aTextPosition);
  }
}
