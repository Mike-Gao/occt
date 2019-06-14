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

#include <inspector/VInspector_TableModelValues.hxx>

#include <inspector/ViewControl_Pane.hxx>
#include <inspector/ViewControl_PaneItem.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <inspector/ViewControl_PaneCreator.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================

VInspector_TableModelValues::VInspector_TableModelValues (const TreeModel_ItemBasePtr& theItem,
  const NCollection_List<Handle(ViewControl_PaneCreator)>& theCreators)
 : ViewControl_TableModelValues(), myItem (theItem), myCreators (theCreators)
{
  QList<TreeModel_HeaderSection> aHeaderValues;
  aHeaderValues.append(TreeModel_HeaderSection ("Function", 190));
  aHeaderValues.append(TreeModel_HeaderSection ("Value", -2));
  SetHeaderValues(aHeaderValues, Qt::Horizontal);

  SetHeaderVisible(Qt::Horizontal, Standard_False);
  SetHeaderVisible(Qt::Vertical, Standard_False);
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant VInspector_TableModelValues::Data (const int theRow, const int theColumn, int theRole) const
{
  int aRow = theRow;
  if (!myItem->GetProperties().IsNull())
  {
    int aPropertiesCount = myItem->GetProperties()->GetTableRowCount();
    if (aRow < aPropertiesCount)
      return myItem->GetProperties()->GetTableData (theRow, theColumn, theRole);
    else
      aRow = aRow - aPropertiesCount;
  }
  switch (theRole)
  {
    case Qt::FontRole:
    {
      if (theColumn == 0)
      {
        QFont aFont = qApp->font();
        aFont.setItalic (true);
        return aFont;
      }
    }
    case Qt::ForegroundRole:
    {
      if (theColumn == 0)
        return QColor (Qt::darkGray).darker(150);
      else if (GetEditType (aRow, theColumn) != ViewControl_EditType_None)
        return ViewControl_TableModelValues::EditCellColor();
    }
    default:
    {
      VInspector_ItemBasePtr anItem = GetItem();
      if (!anItem)
        return QVariant();
      Handle(Standard_Transient) anObject = anItem->GetObject();
      if (anObject.IsNull())
        return anItem->GetTableData (aRow, theColumn, theRole);

      int aCurrentRow = aRow;
      for (NCollection_List<Handle(ViewControl_PaneCreator)>::Iterator anIterator (myCreators); anIterator.More(); anIterator.Next())
      {
        Handle(ViewControl_PaneCreator) aCreator = anIterator.Value();
        ViewControl_Pane* aPane = aCreator->GetPane (anObject->DynamicType()->Name());
        if (!aPane)
          continue;
        if (aCurrentRow >= aPane->GetTableRowCount (anObject))
          aCurrentRow -= aPane->GetTableRowCount (anObject);
        else
          return aPane->GetTableData (anObject, aCurrentRow, theColumn, theRole);
      }
      return anItem->GetTableData (aCurrentRow, theColumn, theRole);
    }
  }
}

// =======================================================================
// function : GetRangeValues
// purpose :
// =======================================================================

bool VInspector_TableModelValues::SetData (const int theRow, const int theColumn, const QVariant& theValue, int theRole)
{
  if (theRole != Qt::EditRole)
    return false;

  int aRow = theRow;
  if (!myItem->GetProperties().IsNull())
  {
    int aPropertiesCount = myItem->GetProperties()->GetTableRowCount();
    if (aRow < aPropertiesCount)
      return myItem->GetProperties()->SetTableData (theRow, theColumn, theValue);
    else
      aRow = aRow - aPropertiesCount;
  }

  VInspector_ItemBasePtr anItem = GetItem();
  if (!anItem)
    return false;

  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return anItem->SetTableData (aRow, theColumn, theValue);

  int aCurrentRow = aRow;
  for (NCollection_List<Handle(ViewControl_PaneCreator)>::Iterator anIterator (myCreators); anIterator.More(); anIterator.Next())
  {
    Handle(ViewControl_PaneCreator) aCreator = anIterator.Value();
    ViewControl_Pane* aPane = aCreator->GetPane (anObject->DynamicType()->Name());
    if (!aPane)
      continue;
    if (aCurrentRow >= aPane->GetTableRowCount (anObject))
      aCurrentRow -= aPane->GetTableRowCount (anObject);
    else
      return aPane->SetTableData (anObject, aCurrentRow, theColumn, theValue);
  }
  return anItem->SetTableData (aCurrentRow, theColumn, theValue);
}

// =======================================================================
// function : Flags
// purpose :
// =======================================================================

Qt::ItemFlags VInspector_TableModelValues::Flags (const QModelIndex& theIndex) const
{
  Qt::ItemFlags aFlags = ViewControl_TableModelValues::Flags (theIndex);

  if (theIndex.column() == 1)
    aFlags = aFlags | Qt::ItemIsEditable;

  return aFlags;
}

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int VInspector_TableModelValues::RowCount (const QModelIndex& theParent) const
{
  int aRowCount = 0;
  if (!myItem->GetProperties().IsNull())
    aRowCount = myItem->GetProperties()->GetTableRowCount();

  VInspector_ItemBasePtr anItem = GetItem();
  if (!anItem)
    return aRowCount;

  aRowCount += anItem->GetTableRowCount();
  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return aRowCount;

  for (NCollection_List<Handle(ViewControl_PaneCreator)>::Iterator anIterator (myCreators); anIterator.More(); anIterator.Next())
  {
    Handle(ViewControl_PaneCreator) aCreator = anIterator.Value();
    ViewControl_Pane* aPane = aCreator->GetPane (anObject->DynamicType()->Name());
    if (!aPane)
      continue;
    aRowCount += aPane->GetTableRowCount (anObject);
  }
  return aRowCount;
}

// =======================================================================
// function : GetEditType
// purpose :
// =======================================================================

ViewControl_EditType VInspector_TableModelValues::GetEditType (const int theRow, const int theColumn) const
{
  int aRow = theRow;
  if (!myItem->GetProperties().IsNull())
  {
    int aPropertiesCount = myItem->GetProperties()->GetTableRowCount();
    if (aRow < aPropertiesCount)
      return myItem->GetProperties()->GetTableEditType (theRow, theColumn);
    else
      aRow = aRow - aPropertiesCount;
  }

  if (theColumn == 0)
    return ViewControl_EditType_None;

  VInspector_ItemBasePtr anItem = GetItem();
  if (!anItem)
    return ViewControl_EditType_None;

  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return anItem->GetTableEditType (aRow, theColumn);

  int aCurrentRow = aRow;
  for (NCollection_List<Handle(ViewControl_PaneCreator)>::Iterator anIterator (myCreators); anIterator.More(); anIterator.Next())
  {
    Handle(ViewControl_PaneCreator) aCreator = anIterator.Value();
    ViewControl_Pane* aPane = aCreator->GetPane (anObject->DynamicType()->Name());
    if (!aPane)
      continue;
    if (aCurrentRow >= aPane->GetTableRowCount (anObject))
      aCurrentRow -= aPane->GetTableRowCount (anObject);
    else
      return aPane->GetTableEditType (anObject, aCurrentRow, theColumn);
  }
  return anItem->GetTableEditType (aCurrentRow, theColumn);
}

// =======================================================================
// function : GetEnumValues
// purpose :
// =======================================================================

QList<QVariant> VInspector_TableModelValues::GetEnumValues (const int theRow, const int theColumn) const
{
  if (theColumn != 1)
    return QList<QVariant>();

  int aRow = theRow;
  if (!myItem->GetProperties().IsNull())
  {
    int aPropertiesCount = myItem->GetProperties()->GetTableRowCount();
    if (aRow < aPropertiesCount)
      return myItem->GetProperties()->GetTableEnumValues (theRow, theColumn);
    else
      aRow = aRow - aPropertiesCount;
  }

  VInspector_ItemBasePtr anItem = GetItem();
  if (!anItem)
    return QList<QVariant>();

  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return anItem->GetTableEnumValues (aRow, theColumn);

  int aCurrentRow = aRow;
  for (NCollection_List<Handle(ViewControl_PaneCreator)>::Iterator anIterator (myCreators); anIterator.More(); anIterator.Next())
  {
    Handle(ViewControl_PaneCreator) aCreator = anIterator.Value();
    ViewControl_Pane* aPane = aCreator->GetPane (anObject->DynamicType()->Name());
    if (!aPane)
      continue;
    if (aCurrentRow >= aPane->GetTableRowCount (anObject))
      aCurrentRow -= aPane->GetTableRowCount (anObject);
    else
      return aPane->GetTableEnumValues (anObject, aCurrentRow, theColumn);
  }
  return anItem->GetTableEnumValues (aCurrentRow, theColumn);
}

// =======================================================================
// function : GetPaneShapes
// purpose :
// =======================================================================

void VInspector_TableModelValues::GetPaneShapes (const int theRow, const int theColumn, NCollection_List<TopoDS_Shape>& theShapes)
{
  int aRow = theRow;
  if (!myItem->GetProperties().IsNull())
  {
    int aPropertiesCount = myItem->GetProperties()->GetTableRowCount();
    if (aRow < aPropertiesCount)
      return;// myItem->GetProperties()->GetEnumValues (theRow, theColumn);
    else
      aRow = aRow - aPropertiesCount;
  }

  VInspector_ItemBasePtr anItem = GetItem();
  if (!anItem)
    return;

  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return;

  for (NCollection_List<Handle(ViewControl_PaneCreator)>::Iterator anIterator (myCreators); anIterator.More(); anIterator.Next())
  {
    Handle(ViewControl_PaneCreator) aCreator = anIterator.Value();
    ViewControl_Pane* aPane = aCreator->GetPane (anObject->DynamicType()->Name());
    if (!aPane)
      continue;

    ViewControl_PaneItem* anItem = aPane->GetSelected (anObject, aRow, theColumn);
    if (anItem && !anItem->GetShape().IsNull())
      theShapes.Append (anItem->GetShape());
  }
}

// =======================================================================
// function : GetItem
// purpose :
// =======================================================================

VInspector_ItemBasePtr VInspector_TableModelValues::GetItem() const
{
  return itemDynamicCast<VInspector_ItemBase>(myItem);
}
