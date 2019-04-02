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

#include <inspector/MessageModel_ItemReportProperties.hxx>
#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/ShapeView_ItemShape.hxx>
#include <inspector/ShapeView_Tools.hxx>

#include <inspector/ViewControl_Tools.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MessageModel_ItemReportProperties, TreeModel_ItemProperties)

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int MessageModel_ItemReportProperties::GetTableRowCount() const
{
  return 1;
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant MessageModel_ItemReportProperties::GetTableData (const int theRow, const int theColumn, int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  Handle(Message_Report) aReport = getItemReport();
  switch (theRow)
  {
    case 0:  return theColumn == 0 ? QVariant ("Limit") : QVariant (aReport->GetLimit());
    default: break;
  }

  return QVariant();
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType MessageModel_ItemReportProperties::GetTableEditType (const int theRow, const int theColumn) const
{
  switch (theRow)
  {
    case 0: return ViewControl_EditType_Spin;
    default: return ViewControl_EditType_None;
  }
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool MessageModel_ItemReportProperties::SetTableData (const int theRow, const int theColumn, const QVariant& theValue)
{
  Handle(Message_Report) aReport = getItemReport();
  switch (theRow)
  {
    case 0:  aReport->SetLimit (theValue.toInt()); break;
    default: break;
  }
  return true;
}

// =======================================================================
// function : getItemReport
// purpose :
// =======================================================================

Handle(Message_Report) MessageModel_ItemReportProperties::getItemReport() const
{
  MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(myItem);
  if (!aReportItem)
    return NULL;

  return aReportItem->GetReport();
}
