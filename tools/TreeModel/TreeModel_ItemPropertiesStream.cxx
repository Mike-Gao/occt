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

#include <inspector/TreeModel_ItemPropertiesStream.hxx>
#include <inspector/TreeModel_ItemStream.hxx>

#include <inspector/Convert_TransientShape.hxx>
#include <inspector/Convert_Tools.hxx>

#include <Bnd_Box.hxx>
#include <gp_XYZ.hxx>
#include <TCollection.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TreeModel_ItemPropertiesStream, TreeModel_ItemProperties)

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void TreeModel_ItemPropertiesStream::Init (const Standard_SStream& theStream)
{
  myStream << theStream.str();

  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aValues;
  TCollection::Split (theStream, aValues, myKey);

  TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(getItem());
  if (!aStreamParent)
    myKey = "Dump";

  for (Standard_Integer anIndex = 1; anIndex <= aValues.Size(); anIndex++)
  {
    TCollection_AsciiString aValue = aValues.FindFromIndex (anIndex);
    if (TCollection::HasBracketKey (aValue))
      myChildren.Add (aValues.FindKey (anIndex), aValue);
    else
      myValues.Add (aValues.FindKey (anIndex), aValue);
  }
  TreeModel_ItemProperties::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void TreeModel_ItemPropertiesStream::Reset()
{
  myKey = "";
  myStream.str (std::string());
  myValues.Clear();
  myChildren.Clear();

  TreeModel_ItemProperties::Reset();
}

// =======================================================================
// function : GetChildKey
// purpose :
// =======================================================================
TCollection_AsciiString TreeModel_ItemPropertiesStream::GetChildKey (const Standard_Integer theRow) const
{
  return GetChildrenValues().FindKey (theRow + 1).ToCString();
}

// =======================================================================
// function : GetChildStream
// purpose :
// =======================================================================

void TreeModel_ItemPropertiesStream::GetChildStream (const Standard_Integer theRow, Standard_OStream& OS) const
{
  TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(getItem());
  if (!aStreamParent)
    OS << myStream.str();
  else
    OS << GetChildrenValues().FindFromIndex (theRow + 1);
}

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int TreeModel_ItemPropertiesStream::GetTableRowCount() const
{
  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& aValues = GetValues();
  return aValues.Size();
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant TreeModel_ItemPropertiesStream::GetTableData (const int theRow, const int theColumn, int theRole) const
{
  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& aValues = GetValues();

  if (theRole == Qt::DisplayRole || theRole == Qt::ToolTipRole)
  {
    if (theColumn == 0) return aValues.FindKey (theRow + 1).ToCString();
    else if (theColumn == 1) return aValues.FindFromIndex (theRow + 1).ToCString();
  }
  return QVariant();
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType TreeModel_ItemPropertiesStream::GetTableEditType (const int /*theRow*/, const int) const
{
  //switch (theRow)
  //{
  //  case 0: return ViewControl_EditType_Spin;
  //  default: return ViewControl_EditType_None;
  //}
  return ViewControl_EditType_None;
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool TreeModel_ItemPropertiesStream::SetTableData (const int /*theRow*/, const int /*theColumn*/, const QVariant& /*theValue*/)
{
  //Handle(Message_Report) aReport = getItemReport();
  //switch (theRow)
  //{
  //  case 0:  aReport->SetLimit (theValue.toInt()); break;
  //  default: break;
  //}
  return true;
}

// =======================================================================
// function : GetPresentations
// purpose :
// =======================================================================

void TreeModel_ItemPropertiesStream::GetPresentations (const int theRow, const int theColumn,
  NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  if (theColumn == 0)
    return;

  if (theRow < 0) // full presentation
  {
    Bnd_Box aBox;
    if (aBox.Init (myStream))
      thePresentations.Append (new Convert_TransientShape (Convert_Tools::CreateShape (aBox)));
  }


  //QVariant aValue = GetTableData (theRow, theColumn, Qt::DisplayRole);
  //TCollection_AsciiString aStrValue = aValue.toString().toStdString().c_str();

  //gp_XYZ aPoint;
  //if (!aPoint.Init (Standard_SStream (aStrValue.ToCString())))
  //  return;

  //thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
}

// =======================================================================
// function : ChildItemCount
// purpose :
// =======================================================================
int TreeModel_ItemPropertiesStream::ChildItemCount() const
{
  TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(getItem());
  if (!aStreamParent && (GetChildrenValues().Size() || GetValues().Size()))
    return 1; // "Dump" item

  return GetChildrenValues().Size();
}

// =======================================================================
// function : CreateChildItem
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ItemPropertiesStream::CreateChildItem (int theRow, int theColumn) const
{
  return TreeModel_ItemStream::CreateItem (getItem(), theRow, theColumn);
}
