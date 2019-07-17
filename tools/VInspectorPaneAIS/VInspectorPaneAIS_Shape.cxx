// Created on: 2018-12-08
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

#include <inspector/VInspectorPaneAIS_Shape.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <inspector/ViewControl_PaneItem.hxx>

#include <Prs3d.hxx>
#include <TopAbs.hxx>

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspectorPaneAIS_Shape::GetTableRowCount(const Handle(Standard_Transient)& theObject) const
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return 0;

  return 8;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspectorPaneAIS_Shape::GetTableData (const Handle(Standard_Transient)& theObject,
                                                const int theRow, const int theColumn, const int theRole) const
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return QVariant();

  if (theRole != Qt::DisplayRole)
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  switch (theRow)
  {
    case 0: return ViewControl_Table::SeparatorData();
    case 1: return isFirstColumn ? QVariant (STANDARD_TYPE (AIS_Shape)->Name())
                                 : ViewControl_Tools::GetPointerInfo (aPrs).ToCString();
    case 2: return ViewControl_Table::SeparatorData();
    case 3: return isFirstColumn ? QVariant ("TShape")
      : (aPrs->Shape().IsNull() ? QVariant ("") : ViewControl_Tools::GetPointerInfo (aPrs->Shape().TShape()).ToCString());
    case 4: return isFirstColumn ? QVariant ("ShapeType")
      : (aPrs->Shape().IsNull() ? QVariant ("") : TopAbs::ShapeTypeToString (aPrs->Shape().ShapeType()));
    case 5: return isFirstColumn ? QVariant ("Orientation")
      : (aPrs->Shape().IsNull() ? QVariant ("") : VInspector_Tools::OrientationToName (aPrs->Shape().Orientation()).ToCString());
    case 6: return isFirstColumn ? QVariant ("Location")
      : (aPrs->Shape().IsNull() ? QVariant ("") : ViewControl_Tools::ToString (aPrs->Shape().Location()).ToCString());
    case 7: return isFirstColumn ? QVariant ("Deflection")
      : (aPrs->Shape().IsNull() ? QVariant ("") : QVariant (Prs3d::GetDeflection(aPrs->Shape(), aPrs->Attributes())));

    default: return QVariant();
  }
}

// =======================================================================
// function : GetSelected
// purpose :
// =======================================================================
ViewControl_PaneItem* VInspectorPaneAIS_Shape::GetSelected (const Handle(Standard_Transient)& theObject,
                                                            const int /*theRow*/, const int /*theColumn*/)
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return NULL;

  ViewControl_PaneItem* aPaneItem = new ViewControl_PaneItem();
  aPaneItem->SetShape (aPrs->Shape());

  return aPaneItem;
}
