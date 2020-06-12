#include "AdaptorPnt2d_AIS.h"

#include <Geom_CartesianPoint.hxx>
#include <StdPrs_Point.hxx>

AdaptorPnt2d_AIS::AdaptorPnt2d_AIS(const Handle(Geom2d_Point) thePoint, Standard_Real theElevation)
  : myPoint(thePoint->X(), thePoint->Y(), theElevation)
{

}


AdaptorPnt2d_AIS::~AdaptorPnt2d_AIS()
{

}

void AdaptorPnt2d_AIS::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
  const Handle(Prs3d_Presentation)& aPresentation,
  const Standard_Integer /*aMode*/)
{
  Handle(Geom_CartesianPoint) aGeomPoint = new Geom_CartesianPoint(myPoint);

  StdPrs_Point::Add(aPresentation, aGeomPoint, myDrawer);
}


void AdaptorPnt2d_AIS::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
  const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
}

void AdaptorPnt2d_AIS::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
  const Standard_Integer /*unMode*/)
{
}

