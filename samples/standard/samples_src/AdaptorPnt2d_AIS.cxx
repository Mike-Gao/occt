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

