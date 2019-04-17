// Created on: 2018-08-10
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

#include <inspector/VInspectorPaneAIS_ItemPrs3dDrawer.hxx>

#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/TreeModel_ItemProperties.hxx>

#include <AIS_ColoredShape.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspectorPaneAIS_ItemPrs3dDrawer::Init()
{
  Handle(Prs3d_Drawer) aDrawer;
  TCollection_AsciiString aName;
  VInspector_ItemPresentableObjectPtr aParentItem = itemDynamicCast<VInspector_ItemPresentableObject>(Parent());
  if (!aParentItem)
  {
    VInspector_ItemPrs3dDrawer::Init();
    return;
  }

  Handle(AIS_ColoredShape) aPrs = Handle(AIS_ColoredShape)::DownCast (aParentItem->GetInteractiveObject());
  const AIS_DataMapOfShapeDrawer& anAspectsMap = aPrs->CustomAspectsMap();

  int aRowId = Row();
  int anItemRows = TreeModel_ItemBase::RowCountWithoutProperties (aParentItem);
  int aCurrentRow = anItemRows;
  for (AIS_DataMapOfShapeDrawer::Iterator anIter (anAspectsMap); anIter.More(); anIter.Next(), aCurrentRow++)
  {
    if (aRowId != aCurrentRow)
      continue;

    aDrawer = anIter.Value();
    break;
  }
  aName = aDrawer.IsNull() ? "NULL" : (TCollection_AsciiString(aDrawer->DynamicType()->Name()) + "_" + (aRowId - anItemRows));
  setDrawer (aDrawer, aName);
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspectorPaneAIS_ItemPrs3dDrawer::Reset()
{
  VInspector_ItemPrs3dDrawer::Reset();

  myShape = TopoDS_Shape();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspectorPaneAIS_ItemPrs3dDrawer::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspectorPaneAIS_ItemPrs3dDrawer*>(this)->Init();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspectorPaneAIS_ItemPrs3dDrawer::GetTableRowCount() const
{
  Handle(Prs3d_Drawer) aDrawer = GetDrawer();
  if (aDrawer.IsNull())
    return 0;

  return 1 + VInspector_ItemPrs3dDrawer::GetTableRowCount();
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspectorPaneAIS_ItemPrs3dDrawer::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  Handle(Prs3d_Drawer) aDrawer = GetDrawer();
  if (aDrawer.IsNull())
    return QVariant();

  if (theRow == 0)
  {
    if (theRole == Qt::DisplayRole)
      return theColumn == 0 ? QVariant ("TopoDS_Shape") : QVariant();
    else
      return QVariant();
  }

  return VInspector_ItemPrs3dDrawer::GetTableData (theRow, theColumn, theRole);
}

