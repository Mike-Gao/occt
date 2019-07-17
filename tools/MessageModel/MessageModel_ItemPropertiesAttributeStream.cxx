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

#include <inspector/MessageModel_ItemPropertiesAttributeStream.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>

#include <inspector/ShapeView_ItemShape.hxx>
#include <inspector/ShapeView_Tools.hxx>

#include <inspector/ViewControl_Tools.hxx>
#include <BRep_Tool.hxx>
#include <Message_AlertExtended.hxx>
#include <TCollection.hxx>
#include <TopoDS.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MessageModel_ItemPropertiesAttributeStream, TreeModel_ItemProperties)

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void MessageModel_ItemPropertiesAttributeStream::Init()
{
  MessageModel_ItemAlertPtr anAlertItem = itemDynamicCast<MessageModel_ItemAlert>(getItem());
  if (!anAlertItem)
    return;

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlertItem->GetAlert());
  if (anExtendedAlert.IsNull() || anExtendedAlert->Attribute().IsNull())
    return;

  Handle(Message_AttributeStream) anAttributeStream = Handle(Message_AttributeStream)::DownCast (anExtendedAlert->Attribute());
  if (anAttributeStream.IsNull())
    return;

  TCollection::Split (anAttributeStream->GetStream(), myValues);

  TreeModel_ItemProperties::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void MessageModel_ItemPropertiesAttributeStream::Reset()
{
  myValues.Clear();

  TreeModel_ItemProperties::Reset();
}

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int MessageModel_ItemPropertiesAttributeStream::GetTableRowCount() const
{
  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& aValues = GetValues();
  return aValues.Size();
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant MessageModel_ItemPropertiesAttributeStream::GetTableData (const int theRow, const int theColumn, int theRole) const
{
  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& aValues = GetValues();

  if (theRole == Qt::DisplayRole)
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
ViewControl_EditType MessageModel_ItemPropertiesAttributeStream::GetTableEditType (const int theRow, const int) const
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
bool MessageModel_ItemPropertiesAttributeStream::SetTableData (const int theRow, const int theColumn, const QVariant& theValue)
{
  //Handle(Message_Report) aReport = getItemReport();
  //switch (theRow)
  //{
  //  case 0:  aReport->SetLimit (theValue.toInt()); break;
  //  default: break;
  //}
  return true;
}
