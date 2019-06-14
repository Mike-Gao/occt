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

#include <inspector/ViewControl_TableModelValuesDefault.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : ColumnCount
// purpose :
// =======================================================================

int ViewControl_TableModelValuesDefault::ColumnCount (const QModelIndex&) const
{
  Qt::Orientation anAdditionalOrientation = myOrientation == Qt::Vertical ? Qt::Horizontal : Qt::Vertical;
  if (myHeaderValues[anAdditionalOrientation].size() > 0)
    return myHeaderValues[anAdditionalOrientation].size();

  return myValues.size();
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant ViewControl_TableModelValuesDefault::Data (const int theRow, const int theColumn, int theRole) const
{
  if (theRole == Qt::DisplayRole)
    return myValues.at ((int)getPosition (theRow, theColumn));

  return ViewControl_TableModelValues::Data (theRow, theColumn, theRole);
}
