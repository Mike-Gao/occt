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

#ifndef _Sample2D_Face_HeaderFile
#define _Sample2D_Face_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <TopoDS_Face.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Geom2d_TrimmedCurve.hxx>

class Sample2D_Face;
DEFINE_STANDARD_HANDLE(Sample2D_Face, AIS_InteractiveObject)
class Sample2D_Face : public AIS_InteractiveObject 
{
public:
  DEFINE_STANDARD_RTTI_INLINE(Sample2D_Face, AIS_InteractiveObject)
 // Methods PUBLIC
 // 
Standard_EXPORT Sample2D_Face (const TopoDS_Shape& theFace);

private:

//private fields
  TopoDS_Shape myshape;
  TColGeom_SequenceOfCurve mySeq_FORWARD;
  TColGeom_SequenceOfCurve mySeq_REVERSED;
  TColGeom_SequenceOfCurve mySeq_INTERNAL;
  TColGeom_SequenceOfCurve mySeq_EXTERNAL;

  Handle(Graphic3d_ArrayOfPolylines) myForwardArray;
  Handle(Graphic3d_ArrayOfPolylines) myReversedArray;
  Handle(Graphic3d_ArrayOfPolylines) myInternalArray;
  Handle(Graphic3d_ArrayOfPolylines) myExternalArray;

  Standard_Integer myForwardNum,
                    myReversedNum,
                    myInternalNum,
                    myExternalNum,
                    myForwardBounds,
                    myReversedBounds,
                    myInternalBounds,
                    myExternalBounds;

//private methods
  void FillData(Standard_Boolean isSizesRecomputed = Standard_False); 
  void ClearSelected ();
  void HilightSelected( const Handle(PrsMgr_PresentationManager3d)& thePM,
                        const SelectMgr_SequenceOfOwner& theOwners);

  void HilightOwnerWithColor( const Handle(PrsMgr_PresentationManager3d)& thePM,
                              const Handle(Prs3d_Drawer)& theStyle,
                              const Handle(SelectMgr_EntityOwner)& theOwner);

  void Compute (  const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                  const Handle(Prs3d_Presentation)& thePresentation,
                  const Standard_Integer theMode);

  void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
    const Standard_Integer theMode);

  void DrawMarker(const Handle(Geom2d_TrimmedCurve)& theCurve, const Handle(Prs3d_Presentation)& thePresentation);

public:
  Quantity_Color myFORWARDColor;
  Quantity_Color myREVERSEDColor;
  Quantity_Color myINTERNALColor;
  Quantity_Color myEXTERNALColor;
  Standard_Integer myWidthIndex;
  Standard_Integer myTypeIndex ;

  TopoDS_Shape& Shape(){return myshape;}
  void SetFace (const TopoDS_Shape& theFace){myshape = theFace;}

};

#endif
