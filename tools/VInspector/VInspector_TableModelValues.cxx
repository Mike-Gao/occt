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
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_TableModelFilter.hxx>
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

  SetUseTableSeparateSize (false);
  SetUseTableProperties (false);
  SetUseTablePropertiesXStep (false, -1);
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant VInspector_TableModelValues::Data (const int theRow, const int theColumn, int theRole) const
{
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
      else if (GetEditType (theRow, theColumn) != ViewControl_EditType_None)
        return ViewControl_TableModelValues::EditCellColor();
    }
    default:
    {
      VInspector_ItemBasePtr anItem = GetItem();
      Handle(Standard_Transient) anObject = anItem->GetObject();
      if (anObject.IsNull())
        return anItem->GetTableData (theRow, theColumn, theRole);

      int aCurrentRow = theRow;
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

  VInspector_ItemBasePtr anItem = GetItem();
  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return anItem->SetTableData (theRow, theColumn, theValue);

  int aCurrentRow = theRow;
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
// function : GetRangeValues
// purpose :
// =======================================================================

int VInspector_TableModelValues::GetValuesCount () const
{
  VInspector_ItemBasePtr anItem = GetItem();

  int aRowCount = anItem->GetTableRowCount();
  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return aRowCount * 2;

  for (NCollection_List<Handle(ViewControl_PaneCreator)>::Iterator anIterator (myCreators); anIterator.More(); anIterator.Next())
  {
    Handle(ViewControl_PaneCreator) aCreator = anIterator.Value();
    ViewControl_Pane* aPane = aCreator->GetPane (anObject->DynamicType()->Name());
    if (!aPane)
      continue;
    aRowCount += aPane->GetTableRowCount (anObject);
  }
  return aRowCount * 2;
}

// =======================================================================
// function : GetEditType
// purpose :
// =======================================================================

ViewControl_EditType VInspector_TableModelValues::GetEditType (const int theRow, const int theColumn) const
{
  if (theColumn == 0)
    return ViewControl_EditType_None;

  VInspector_ItemBasePtr anItem = GetItem();
  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return anItem->GetTableEditType (theRow, theColumn);

  int aCurrentRow = theRow;
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

  VInspector_ItemBasePtr anItem = GetItem();
  Handle(Standard_Transient) anObject = anItem->GetObject();
  if (anObject.IsNull())
    return anItem->GetTableEnumValues (theRow, theColumn);

  int aCurrentRow = theRow;
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
// function : GetItem
// purpose :
// =======================================================================

VInspector_ItemBasePtr VInspector_TableModelValues::GetItem() const
{
  return itemDynamicCast<VInspector_ItemBase>(myItem);
}
