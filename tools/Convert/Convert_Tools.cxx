// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/Convert_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <AIS_Plane.hxx>
#include <Geom_Plane.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Standard_Dump.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

//=======================================================================
//function : CreateShape
//purpose  :
//=======================================================================
void Convert_Tools::ConvertStreamToPresentations (const Standard_SStream& theSStream,
                                                  const Standard_Integer theStartPos,
                                                  const Standard_Integer /*theLastPos*/,
                                                  NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  int aStartPos = theStartPos;
  gp_XYZ aPoint;
  if (aPoint.InitJson (theSStream, aStartPos))
  {
    thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
    return;
  }

  gp_Pnt aPnt;
  if (aPnt.InitJson (theSStream, aStartPos))
  {
    thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPnt)));
    return;
  }

  gp_Dir aDir;
  if (aDir.InitJson (theSStream, aStartPos))
  {
    thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeEdge (gp::Origin(), aDir.XYZ())));
    return;
  }

  gp_Ax3 aPln;
  if (aPln.InitJson (theSStream, aStartPos))
  {
    Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPln);
    Handle(AIS_Plane) aPlanePrs = new AIS_Plane (aGeomPlane);

    // TODO - default fields to be defined in another place
    aPlanePrs->Attributes()->SetPlaneAspect (new Prs3d_PlaneAspect());
    Handle (Prs3d_PlaneAspect) aPlaneAspect = aPlanePrs->Attributes()->PlaneAspect();
    aPlaneAspect->SetPlaneLength (100, 100);
    aPlaneAspect->SetDisplayCenterArrow (Standard_True);
    aPlaneAspect->SetDisplayEdgesArrows (Standard_True);
    aPlaneAspect->SetArrowsSize (100);
    aPlaneAspect->SetArrowsLength (100);
    aPlaneAspect->SetDisplayCenterArrow (Standard_True);
    aPlaneAspect->SetDisplayEdges (Standard_True);

    aPlanePrs->SetColor (Quantity_NOC_WHITE);
    aPlanePrs->SetTransparency (0);

    thePresentations.Append (aPlanePrs);
    return;
  }
}

//=======================================================================
//function : CreateShape
//purpose  :
//=======================================================================
TopoDS_Shape Convert_Tools::CreateShape (const Bnd_Box& theBoundingBox)
{
  if (theBoundingBox.IsVoid() || theBoundingBox.IsWhole())
    return TopoDS_Shape();

  Standard_Real aXmin, anYmin, aZmin, aXmax, anYmax, aZmax;
  theBoundingBox.Get (aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);

  gp_Pnt aPntMin = gp_Pnt (aXmin, anYmin, aZmin);
  gp_Pnt aPntMax = gp_Pnt (aXmax, anYmax, aZmax);

  return CreateBoxShape (aPntMin, aPntMax);
}

//=======================================================================
//function : CreateShape
//purpose  :
//=======================================================================
TopoDS_Shape Convert_Tools::CreateShape (const Bnd_OBB& theBoundingBox)
{
  if (theBoundingBox.IsVoid())
    return TopoDS_Shape();

  TColgp_Array1OfPnt anArrPnts(0, 8);
  theBoundingBox.GetVertex(&anArrPnts(0));

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(0)), gp_Pnt (anArrPnts.Value(1))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(0)), gp_Pnt (anArrPnts.Value(2))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(1)), gp_Pnt (anArrPnts.Value(3))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(2)), gp_Pnt (anArrPnts.Value(3))));

  return aCompound;
}

//=======================================================================
//function : CreateBoxShape
//purpose  :
//=======================================================================
TopoDS_Shape Convert_Tools::CreateBoxShape (const gp_Pnt& thePntMin, const gp_Pnt& thePntMax)
{
  Standard_Boolean aThinOnX = fabs (thePntMin.X() - thePntMax.X()) < Precision::Confusion();
  Standard_Boolean aThinOnY = fabs (thePntMin.Y() - thePntMax.Y()) < Precision::Confusion();
  Standard_Boolean aThinOnZ = fabs (thePntMin.Z() - thePntMax.Z()) < Precision::Confusion();

  if (((int)aThinOnX + (int)aThinOnY + (int)aThinOnZ) > 1) // thin box in several directions is a point
  {
    BRep_Builder aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound (aCompound);
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (thePntMin));
    return aCompound;
  }

  if (aThinOnX || aThinOnY || aThinOnZ)
  {
    gp_Pnt aPnt1, aPnt2, aPnt3, aPnt4 ;
    if (aThinOnX)
    {
      aPnt1 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMin.Z());
      aPnt2 = gp_Pnt(thePntMin.X(), thePntMax.Y(), thePntMin.Z());
      aPnt3 = gp_Pnt(thePntMin.X(), thePntMax.Y(), thePntMax.Z());
      aPnt4 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMax.Z());
    }
    else if (aThinOnY)
    {
      aPnt1 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMin.Z());
      aPnt2 = gp_Pnt(thePntMax.X(), thePntMin.Y(), thePntMin.Z());
      aPnt3 = gp_Pnt(thePntMax.X(), thePntMin.Y(), thePntMax.Z());
      aPnt4 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMax.Z());
    }
    else if (aThinOnZ)
    {
      aPnt1 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMin.Z());
      aPnt2 = gp_Pnt(thePntMax.X(), thePntMin.Y(), thePntMin.Z());
      aPnt3 = gp_Pnt(thePntMax.X(), thePntMax.Y(), thePntMin.Z());
      aPnt4 = gp_Pnt(thePntMin.X(), thePntMax.Y(), thePntMin.Z());
    }
    BRep_Builder aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound (aCompound);
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt1, aPnt2));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt2, aPnt3));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt3, aPnt4));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt4, aPnt1));

    return aCompound;
  }
  else
  {
    BRepPrimAPI_MakeBox aBoxBuilder (thePntMin, thePntMax);
    return aBoxBuilder.Shape();
  }
}
