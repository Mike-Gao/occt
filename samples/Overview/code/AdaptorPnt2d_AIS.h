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

#ifndef ADAPTOR_PNT2D_AIS_H
#define ADAPTOR_PNT2D_AIS_H

#include <Geom2d_Point.hxx>
#include <AIS_InteractiveObject.hxx>


class AdaptorPnt2d_AIS;
DEFINE_STANDARD_HANDLE(AdaptorPnt2d_AIS, AIS_InteractiveObject)
class AdaptorPnt2d_AIS : public AIS_InteractiveObject
{
public:
  AdaptorPnt2d_AIS(const Handle(Geom2d_Point) thePoint, Standard_Real theElevation = 0);
  virtual ~AdaptorPnt2d_AIS();

  DEFINE_STANDARD_RTTI_INLINE(AdaptorPnt2d_AIS, AIS_InteractiveObject)

private:
  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
               const Handle(Prs3d_Presentation)& aPresentation,
               const Standard_Integer aMode);

  void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer unMode);

  gp_Pnt myPoint;
};

#endif // ADAPTOR_PNT2D_AIS_H
