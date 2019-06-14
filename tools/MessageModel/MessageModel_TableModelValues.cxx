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

#include <inspector/MessageModel_TableModelValues.hxx>

#include <inspector/ViewControl_TableModel.hxx>

#include <Message_AttributeVectorOfValues.hxx>

const int REAL_SIGNS = 16;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================

MessageModel_TableModelValues::MessageModel_TableModelValues (const Handle(Message_Attribute)& theAttribute,
  const int theSectionWidth)
 : myAttribute (Handle(Message_AttributeVectorOfValues)::DownCast (theAttribute))
{
  SetDefaultSectionSize (Qt::Horizontal, theSectionWidth);
}

// =======================================================================
// function : ColumnCount
// purpose :
// =======================================================================

int MessageModel_TableModelValues::ColumnCount (const QModelIndex&) const
{
  return myAttribute->GetColumnCount();
}

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================
int MessageModel_TableModelValues::RowCount (const QModelIndex& theParent) const
{
  int aColumnCount = ColumnCount (theParent);
  if (!aColumnCount)
    return 0;

  return myAttribute->GetValues().Length() / aColumnCount;
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant MessageModel_TableModelValues::Data (const int theRow, const int theColumn, int theRole) const
{
  int aColumnCount = ColumnCount (QModelIndex());
  int anIndex = theRow * aColumnCount + theColumn;

  if (theRole == Qt::DisplayRole && anIndex < myAttribute->GetValues().Length())
  {
    return myAttribute->GetValues().Value(anIndex).ToCString();
  }

  return QVariant();
}
