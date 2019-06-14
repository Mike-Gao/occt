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

#include <inspector/ViewControl_TableModelValues.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : ColumnCount
// purpose :
// =======================================================================

int ViewControl_TableModelValues::ColumnCount (const QModelIndex&) const
{
  if (!GetProperties().IsNull())
    return GetProperties()->GetTableColumnCount();

  return 0;
}


// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int ViewControl_TableModelValues::RowCount (const QModelIndex&) const
{
  if (!GetProperties().IsNull())
    return GetProperties()->GetTableRowCount();

  return 0;
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant ViewControl_TableModelValues::Data (const int theRow, const int theColumn, int theRole) const
{
  if (!GetProperties().IsNull())
  {
    QVariant aValue = GetProperties()->GetTableData (theRow, theColumn, theRole);
    if (aValue.isValid())
      return aValue;
  }

  if (theRole == Qt::TextAlignmentRole) // for multi-lines text, align it to the top
    return Qt::AlignTop;

  if ((theRole == Qt::FontRole || theRole == Qt::ForegroundRole) && isItalicHeader (theRow, theColumn))
  {
    if (theRole == Qt::FontRole)
    {
      QFont aFont = qApp->font();
      aFont.setItalic (true);
      return aFont;
    }
    else
      QColor (Qt::darkGray).darker (150);
  }
  return QVariant();
}

// =======================================================================
// function : SetData
// purpose :
// =======================================================================

bool ViewControl_TableModelValues::SetData (const int theRow, const int theColumn, const QVariant& theValue, int)
{
  if (!GetProperties().IsNull())
    return GetProperties()->SetTableData (theRow, theColumn, theValue);

  return false;
}

// =======================================================================
// function : HeaderData
// purpose :
// =======================================================================

QVariant ViewControl_TableModelValues::HeaderData (int theSection, Qt::Orientation theOrientation, int theRole) const
{
  if (theRole == Qt::DisplayRole)
    return myHeaderValues.contains (theOrientation) ? myHeaderValues[theOrientation][theSection].GetName()
                                                    : QString::number (theSection + 1);
  else if (theRole == Qt::ForegroundRole)
    return QColor (Qt::darkGray);

  return QVariant();
}

// =======================================================================
// function : GetEditType
// purpose :
// =======================================================================
ViewControl_EditType ViewControl_TableModelValues::GetEditType (const int theRow, const int theColumn) const
{
  if (!GetProperties().IsNull())
  {
    return GetProperties()->GetTableEditType (theRow, theColumn);
  }
  return ViewControl_EditType_None;
}

// =======================================================================
// function : GetEnumValues
// purpose :
// =======================================================================
QList<QVariant> ViewControl_TableModelValues::GetEnumValues (const int theRow, const int theColumn) const
{
  if (!GetProperties().IsNull())
  {
    return GetProperties()->GetTableEnumValues (theRow, theColumn);
  }
  return QList<QVariant>();
}

// =======================================================================
// function : isItalicHeader
// purpose :
// =======================================================================
Qt::ItemFlags ViewControl_TableModelValues::Flags (const QModelIndex& theIndex) const
{
  if (!GetProperties().IsNull())
  {
    return GetProperties()->GetTableFlags (theIndex.row(), theIndex.column());
  }
  return theIndex.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags;
}

// =======================================================================
// function : isItalicHeader
// purpose :
// =======================================================================
bool ViewControl_TableModelValues::isItalicHeader (const int theRow, const int theColumn) const
{
  Qt::Orientation anOrientation = myOrientation == Qt::Vertical ? Qt::Horizontal : Qt::Vertical;
  int aCell = anOrientation == Qt::Horizontal ? theColumn : theRow;

  return GetHeaderItem (anOrientation, aCell).IsItalic();
}
