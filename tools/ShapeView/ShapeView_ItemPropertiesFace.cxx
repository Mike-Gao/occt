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

#include <inspector/ShapeView_ItemPropertiesFace.hxx>
#include <inspector/ShapeView_ItemShape.hxx>
#include <inspector/ShapeView_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeView_ItemPropertiesFace, TreeModel_ItemProperties)

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int ShapeView_ItemPropertiesFace::GetTableRowCount() const
{
  return ShapeView_Tools::GetShapeGlobalPropertiesCount();
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant ShapeView_ItemPropertiesFace::GetTableData (const int theRow, const int theColumn, int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  TopoDS_Shape aShape = getItemShape();

  QVariant aValue = ShapeView_Tools::GetShapeGlobalProperties (aShape, theRow, theColumn);

  return aValue;
}

// =======================================================================
// function : getItemShape
// purpose :
// =======================================================================

TopoDS_Shape ShapeView_ItemPropertiesFace::getItemShape() const
{
  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(getItem());
  if (!aShapeItem)
    return TopoDS_Shape();

  return aShapeItem->GetItemShape();
}
