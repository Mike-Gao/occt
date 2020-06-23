#include "AdaptorVec_AIS.h"

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Text.hxx>
#include <DsgPrs_LengthPresentation.hxx>


AdaptorVec_AIS::AdaptorVec_AIS()
{

}

AdaptorVec_AIS::AdaptorVec_AIS(const gp_Pnt& aPnt, const gp_Dir& aDir, Standard_Real aLength, Standard_Real anArrowLength)
  : myPnt(aPnt),
  myDir(aDir),
  myLength(aLength),
  myArrowLength(anArrowLength)
{}

AdaptorVec_AIS::AdaptorVec_AIS(const gp_Pnt& aPnt, const gp_Vec& aVec, Standard_Real anArrowLength)
  : myPnt(aPnt),
  myDir(aVec),
  myArrowLength(anArrowLength)
{
  myLength = aVec.Magnitude();
}

AdaptorVec_AIS::AdaptorVec_AIS(const gp_Pnt2d& aPnt2d, const gp_Dir2d& aDir2d, Standard_Real aLength, Standard_Real anArrowLength)
  : myPnt(gp_Pnt(aPnt2d.X(), aPnt2d.Y(), 0.0)),
  myDir(gp_Dir(aDir2d.X(), aDir2d.Y(), 0.0)),
  myLength(aLength),
  myArrowLength(anArrowLength)
{
}

AdaptorVec_AIS::AdaptorVec_AIS(const gp_Pnt2d& aPnt2d, const gp_Vec2d& aVec2d, Standard_Real anArrowLength)
  : myPnt(gp_Pnt(aPnt2d.X(), aPnt2d.Y(), 0.0)),
  myDir(gp_Dir(aVec2d.X(), aVec2d.Y(), 0.0)),
  myArrowLength(anArrowLength)
{
  myLength = aVec2d.Magnitude();
}

AdaptorVec_AIS::AdaptorVec_AIS(const gp_Pnt2d & aPnt1, const gp_Pnt2d & aPnt2, Standard_Real anArrowLength)
  : myPnt(gp_Pnt(aPnt1.X(), aPnt1.Y(), 0.0))
{
    gp_Vec aVec(aPnt2.X() - aPnt1.X(), aPnt2.Y() - aPnt1.Y(), 0.0);
    myDir = gp_Dir(aVec);
    myLength = aVec.Magnitude();
}


AdaptorVec_AIS::~AdaptorVec_AIS()
{
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
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(myDrawer->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  // Draw arrow
  Prs3d_Arrow::Draw(Prs3d_Root::CurrentGroup(aPresentation),
    aLastPoint,
    myDir,
    anArrowAspect->Angle(),
    anArrowAspect->Length());

  // Draw text
  if (myText.Length() != 0)
  {
    gp_Pnt aTextPosition = aLastPoint;
    Prs3d_Text::Draw(Prs3d_Root::CurrentGroup(aPresentation),
      myDrawer->TextAspect(),
      myText,
      aTextPosition);
  }
}


void AdaptorVec_AIS::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
  const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
}

void AdaptorVec_AIS::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
  const Standard_Integer /*aMode*/)
{
}

void AdaptorVec_AIS::SetText(const TCollection_AsciiString & theText)
{
  myText = theText;
}

void AdaptorVec_AIS::SetLineAspect(const Handle(Prs3d_LineAspect)& theAspect)
{
  myDrawer->SetLineAspect(theAspect);
}
