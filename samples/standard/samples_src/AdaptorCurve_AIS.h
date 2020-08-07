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

#ifndef ADAPTOR_CURVE_AIS_H
#define ADAPTOR_CURVE_AIS_H

#include <AIS_InteractiveObject.hxx>
#include <Geom_Curve.hxx>



class AdaptorCurve_AIS;
DEFINE_STANDARD_HANDLE(AdaptorCurve_AIS, AIS_InteractiveObject)
class AdaptorCurve_AIS : public AIS_InteractiveObject
{

public:
  AdaptorCurve_AIS(const Handle(Geom_Curve)& theCurve) : myCurve(theCurve) {}
  virtual ~AdaptorCurve_AIS() {}
  DEFINE_STANDARD_RTTI_INLINE(AdaptorCurve_AIS, AIS_InteractiveObject)
private:

  Standard_EXPORT virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0);
  virtual void Compute(const Handle(Prs3d_Projector)&, const Handle(Prs3d_Presentation)&) {}
  virtual void ComputeSelection(const Handle(SelectMgr_Selection)&, const Standard_Integer) {}

private:
  Handle(Geom_Curve) myCurve;
};
#endif