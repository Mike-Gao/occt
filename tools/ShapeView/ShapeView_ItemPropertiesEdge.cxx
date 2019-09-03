// Created on: 2019-02-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <inspector/ShapeView_ItemPropertiesEdge.hxx>
#include <inspector/ShapeView_ItemShape.hxx>
#include <inspector/ShapeView_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Adaptor3d_Curve.hxx>
#include <AIS_Shape.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <GCPnts_AbscissaPoint.hxx>

#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeView_ItemPropertiesEdge, TreeModel_ItemProperties)

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int ShapeView_ItemPropertiesEdge::GetTableRowCount() const
{
  return ShapeView_Tools::GetShapeGlobalPropertiesCount() + 7;
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>

QVariant ShapeView_ItemPropertiesEdge::GetTableData (const int theRow, const int theColumn, int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  TopoDS_Shape aShape = getItemShape();

  if (aShape.ShapeType() != TopAbs_EDGE)
    return QVariant();

  TopoDS_Edge anEdge = TopoDS::Edge(aShape);
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);

  GeomAdaptor_Curve aAdaptor(aCurve, aFirst, aLast);
  gp_Pnt aFirstPnt = aAdaptor.Value(aFirst);
  gp_Pnt aLastPnt = aAdaptor.Value(aLast);

  BRepAdaptor_Curve aBRepAdaptor = BRepAdaptor_Curve(anEdge);
  Adaptor3d_Curve* anAdaptor3d = &aBRepAdaptor;

  bool isFirstColumn = theColumn == 0;
  switch (theRow)
  {
    case 0:  return isFirstColumn ? "Length" : QString::number (GCPnts_AbscissaPoint::Length(*anAdaptor3d));
    case 1:  return isFirstColumn ? "Geom_Curve" : aCurve->DynamicType()->Name();
    case 2:  return isFirstColumn ? "FirstPnt" : ViewControl_Tools::ToString (aFirstPnt).ToCString();
    case 3:  return isFirstColumn ? "LastPnt" : ViewControl_Tools::ToString (aLastPnt).ToCString();
    case 4: return isFirstColumn ? "Continuity" : ShapeView_Tools::ToName (aCurve->Continuity());
    case 5: return isFirstColumn ? "IsClosed" : ShapeView_Tools::ToString (aCurve->IsClosed());
    case 6: return isFirstColumn ? "IsPeriodic" :  aCurve->IsPeriodic() ? QString::number (aCurve->Period())
                                                                        : ShapeView_Tools::ToString (aCurve->IsPeriodic());
    default: break;
  }

  int anOwnRows = 7;
  /*int aTableRow = theRow - anOwnRows;
  // BRep_Tool::PolygonOnTriangulation // find the representation

  TopoDS_Edge E = TopoDS::Edge(aShape);
  Handle(Poly_PolygonOnTriangulation) P;
  Handle(Poly_Triangulation) T;
  TopLoc_Location L;

  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(anEdge.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  int aCurRow = 0;
  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnTriangulation()) {
      if (aCurRow < aTableRow)
        continue;
      const BRep_PolygonOnTriangulation* PT =
        static_cast<const BRep_PolygonOnTriangulation*>(cr.get());
      P = PT->PolygonOnTriangulation();
      T = PT->Triangulation();
      L = E.Location() * PT->Location();
      return isFirstColumn ? QVariant("Polygon: ") : P.;
    }
    itcr.Next();
  }
  L.Identity();
  P.Nullify();
  T.Nullify();


  anOwnRows += aTableRow;
  */
  return ShapeView_Tools::GetShapeGlobalProperties (aShape, theRow - anOwnRows, theColumn);
}

// =======================================================================
// function : getItemShape
// purpose :
// =======================================================================

TopoDS_Shape ShapeView_ItemPropertiesEdge::getItemShape() const
{
  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(getItem());
  if (!aShapeItem)
    return TopoDS_Shape();

  return aShapeItem->GetItemShape();
}
