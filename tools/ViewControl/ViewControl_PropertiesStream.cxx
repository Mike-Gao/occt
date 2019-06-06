// Created on: 2019-04-28
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

#include <inspector/ViewControl_PropertiesStream.hxx>
#include <inspector/ViewControl_TransientShape.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>

#include <Message.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ViewControl_PropertiesStream, TreeModel_ItemProperties)

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void ViewControl_PropertiesStream::Init()
{
  if (!getItem() || !myValues.IsEmpty())
    return;

  Standard_SStream aSStream;
  getItem()->Dump (aSStream);

  Message::ConvertStream (aSStream, myColumnCount, myValues); 
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void ViewControl_PropertiesStream::Reset()
{
  if (!getItem())
    return;
  myValues.Clear();
  myColumnCount = 0;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void ViewControl_PropertiesStream::initItem() const
{
  const_cast<ViewControl_PropertiesStream*>(this)->Init();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================

int ViewControl_PropertiesStream::GetTableRowCount() const
{
  initItem();

  return getValues().Length() / myColumnCount;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================

QVariant ViewControl_PropertiesStream::GetTableData (const int theRow, const int theColumn, int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  initItem();

  int anIndex = theRow * myColumnCount + theColumn;
  if (theRole == Qt::DisplayRole && anIndex < getValues().Length())
    return getValues().Value(anIndex).ToCString();

  return QVariant();
}

// =======================================================================
// function : GetPresentations
// purpose :
// =======================================================================

void ViewControl_PropertiesStream::GetPresentations (const int theRow,
                                                     const int theColumn,
  NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  if (theColumn == 0)
    return;

  QVariant aValue = GetTableData (theRow, theColumn, Qt::DisplayRole);
  TCollection_AsciiString aStrValue = aValue.toString().toStdString().c_str();

  gp_XYZ aPoint;
  if (!aPoint.FromString (aStrValue))
    return;

  thePresentations.Append (new ViewControl_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
}
