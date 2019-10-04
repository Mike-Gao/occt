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

#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TreeModel_ItemProperties, Standard_Transient)

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void TreeModel_ItemProperties::Init (const TCollection_AsciiString& theStreamValue)
{
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aValues;
  if (!Standard_Dump::SplitJson (theStreamValue, aValues))
    return;

  for (Standard_Integer anIndex = 1; anIndex <= aValues.Size(); anIndex++)
  {
    TCollection_AsciiString aValue = aValues.FindFromIndex (anIndex);
    if (!Standard_Dump::HasChildKey (aValue))
      myValues.Add (aValues.FindKey (anIndex), aValue);
  }
}

// =======================================================================
// function :  Reset
// purpose :
// =======================================================================
void TreeModel_ItemProperties::Reset()
{
  myValues.Clear();
}

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int TreeModel_ItemProperties::RowCount() const
{
  return myValues.Size();
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant TreeModel_ItemProperties::Data (const int theRow, const int theColumn, int theRole) const
{
  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  if (theColumn == 0) return myValues.FindKey (theRow + 1).ToCString();
  else if (theColumn == 1) return myValues.FindFromIndex (theRow + 1).ToCString();

  return QVariant();
}

// =======================================================================
// function : GetPresentations
// purpose :
// =======================================================================

void TreeModel_ItemProperties::GetPresentations (const int theRow, const int theColumn,
                                                 NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  (void)thePresentations;

  if (theRow < 0) // full presentation
  {
    return;
  }


  if (theColumn == 0)
    return;

  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aValues;
  aValues.Add (Data (theRow, 0).toString().toStdString().c_str(), Data (theRow, theColumn).toString().toStdString().c_str());

  Standard_SStream aStreamOnSelected;
  Standard_Dump::JoinJson (aStreamOnSelected, aValues);

  //if (theRow < 0) // full presentation
  //{
    //Bnd_Box aBox;
    //if (aBox.Init (myStream))
    //  thePresentations.Append (new Convert_TransientShape (Convert_Tools::CreateShape (aBox)));
  //}

  gp_XYZ aPoint;
  Standard_Integer aPosition = 1;
  if (aPoint.InitJson (aStreamOnSelected, aPosition))
  {
    thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
  }


  //QVariant aValue = GetTableData (theRow, theColumn, Qt::DisplayRole);
  //TCollection_AsciiString aStrValue = aValue.toString().toStdString().c_str();

  //gp_XYZ aPoint;
  //if (!aPoint.Init (Standard_SStream (aStrValue.ToCString())))
  //  return;

  //thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
}

// =======================================================================
// function : GetTableFlags
// purpose :
// =======================================================================

Qt::ItemFlags TreeModel_ItemProperties::GetTableFlags (const int, const int theColumn) const
{
  Qt::ItemFlags aFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (theColumn == 1)
    aFlags = aFlags | Qt::ItemIsEditable;

  return aFlags;
}

