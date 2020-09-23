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

#include "AdaptorCurve_AIS.h"

#include <GeomAdaptor_Curve.hxx>
#include <Prs3d_LineAspect.hxx>
#include <StdPrs_PoleCurve.hxx>
#include <StdPrs_Curve.hxx>

void AdaptorCurve_AIS::Compute(const Handle(PrsMgr_PresentationManager3d)&,
  const Handle(Prs3d_Presentation)& thePrs,
  const Standard_Integer theMode)
{
  GeomAdaptor_Curve anAdaptorCurve(myCurve);
  switch (theMode) 
  {
  case 1:
  {
    Handle(Prs3d_Drawer) aPoleDrawer = new Prs3d_Drawer();
    aPoleDrawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0));
    StdPrs_PoleCurve::Add(thePrs, anAdaptorCurve, aPoleDrawer);
  }
  case 0:
    StdPrs_Curve::Add(thePrs, anAdaptorCurve, myDrawer);
    break;
  }
}