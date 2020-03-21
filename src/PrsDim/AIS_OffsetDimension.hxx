// Created on: 1997-03-04
// Created by: Jean-Pierre COMBE
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _AIS_OffsetDimension_HeaderFile
#define _AIS_OffsetDimension_HeaderFile

#include <AIS_Relation.hxx>
#include <AIS_KindOfDimension.hxx>

DEFINE_STANDARD_HANDLE(AIS_OffsetDimension, AIS_Relation)

//! A framework to display dimensions of offsets.
//! The relation between the offset and the basis shape
//! is indicated. This relation is displayed with arrows and
//! text. The text gives the dsitance between the offset
//! and the basis shape.
class AIS_OffsetDimension : public AIS_Relation
{
  DEFINE_STANDARD_RTTIEXT(AIS_OffsetDimension, AIS_Relation)
public:

  //! Constructs the offset display object defined by the
  //! first shape aFShape, the second shape aSShape, the
  //! dimension aVal, and the text aText.
  Standard_EXPORT AIS_OffsetDimension(const TopoDS_Shape& FistShape, const TopoDS_Shape& SecondShape, const Standard_Real aVal, const TCollection_ExtendedString& aText);

  //! Indicates that the dimension we are concerned with is an offset.
  virtual AIS_KindOfDimension KindOfDimension() const Standard_OVERRIDE { return AIS_KOD_OFFSET; }

  //! Returns true if the offset datum is movable.
  virtual Standard_Boolean IsMovable() const Standard_OVERRIDE { return Standard_True; }

  //! Sets a transformation aTrsf for presentation and
  //! selection to a relative position.
  void SetRelativePos (const gp_Trsf& aTrsf) { myRelativePos = aTrsf; }

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeTwoFacesOffset (const Handle(Prs3d_Presentation)& aPresentation, const gp_Trsf& aTrsf);
  
  Standard_EXPORT void ComputeTwoAxesOffset (const Handle(Prs3d_Presentation)& aPresentation, const gp_Trsf& aTrsf);
  
  Standard_EXPORT void ComputeAxeFaceOffset (const Handle(Prs3d_Presentation)& aPresentation, const gp_Trsf& aTrsf);

private:

  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Dir myDirAttach;
  gp_Dir myDirAttach2;
  gp_Trsf myRelativePos;

};

#endif // _AIS_OffsetDimension_HeaderFile
