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

#include <inspector/ShapeView_ItemPropertiesVertex.hxx>
#include <inspector/ShapeView_ItemShape.hxx>
#include <inspector/ShapeView_Tools.hxx>

#include <inspector/ViewControl_Tools.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeView_ItemPropertiesVertex, TreeModel_ItemProperties)

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int ShapeView_ItemPropertiesVertex::GetTableRowCount() const
{
  return ShapeView_Tools::GetShapeGlobalPropertiesCount() + 1;
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant ShapeView_ItemPropertiesVertex::GetTableData (const int theRow, const int theColumn, int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  TopoDS_Shape aShape = getItemShape();
  switch (theRow)
  {
    case 0:  return theColumn == 0 ? "Vertex" : ViewControl_Tools::ToString (BRep_Tool::Pnt (TopoDS::Vertex (aShape))).ToCString();
    default: break;
  }
  int anOwnRows = 1;

  return ShapeView_Tools::GetShapeGlobalProperties (aShape, theRow - anOwnRows, theColumn);
}

// =======================================================================
// function : getItemShape
// purpose :
// =======================================================================

TopoDS_Shape ShapeView_ItemPropertiesVertex::getItemShape() const
{
  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(getItem());
  if (!aShapeItem)
    return TopoDS_Shape();

  return aShapeItem->GetItemShape();
}
