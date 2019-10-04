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

#include <inspector/MessageModel_ItemReport.hxx>

#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_Tools.hxx>

#include <OSD_Path.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant MessageModel_ItemReport::initValue (const int theRole) const
{
  QVariant aParentValue = MessageModel_ItemBase::initValue (theRole);
  if (aParentValue.isValid())
    return aParentValue;

  const Handle(Message_Report)& aReport = getReport();
  if (aReport.IsNull())
    return QVariant();

  if (theRole == Qt::ForegroundRole)
  {
    if (!aReport->GetAlerts (Message_Fail).IsEmpty())
      return QColor(Qt::darkRed);

    for (int aGravityId = (int)Message_Trace; aGravityId <= (int)Message_Fail; aGravityId++)
    {
      if (!aReport->IsActive ((Message_Gravity)aGravityId))
        return QColor(Qt::darkGray);
    }
    return QVariant();
  }
  if (theRole == Qt::ToolTipRole && !myDescription.IsEmpty() && Column() == 8)
    return myDescription.ToCString();

  if (theRole != Qt::DisplayRole)
    return QVariant();

  switch (Column())
  {
    case 0: return aReport->DynamicType()->Name();
    //case 1: return QVariant(); // visibility state
    //case 2: return rowCount() > 0 ? QVariant (rowCount()) : QVariant();
    case 4: return aReport->CumulativeMetric (Message_Info);
    case 5:
    {
      if (aReport->PerfMeterMode() == Message_PerfMeterMode_UserTimeCPU ||
          aReport->PerfMeterMode() == Message_PerfMeterMode_SystemTimeInfo)
        return "100";
      else
        return QVariant();
    }
    case 9:
    {
      if (myDescription.IsEmpty())
        return QVariant();

      OSD_Path aPath(myDescription);
      return QString ("%1%2").arg (aPath.Name().ToCString()).arg (aPath.Extension().ToCString());
    }
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int MessageModel_ItemReport::initRowCount() const
{
  const Handle(Message_Report)& aReport = getReport();
  if (aReport.IsNull())
    return 0;

  MessageModel_ItemReport* aCurrentItem = (MessageModel_ItemReport*)this;
  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    const Message_ListOfAlert& anAlerts = aReport->GetAlerts ((Message_Gravity)aGravityId);
    for (Message_ListOfAlert::Iterator anIt(anAlerts); anIt.More(); anIt.Next())
    {
      Message_ListOfAlert aCurAlerts;
      aCurAlerts.Append (anIt.Value());
      aCurrentItem->myChildAlerts.Bind(myChildAlerts.Size(), aCurAlerts);
    }
  }
  return aCurrentItem->myChildAlerts.Size();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_ItemReport::createChild (int theRow, int theColumn)
{
  return MessageModel_ItemAlert::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageModel_ItemReport::Init()
{
  MessageModel_ItemRootPtr aRootItem = itemDynamicCast<MessageModel_ItemRoot> (Parent());
  myReport = aRootItem ? aRootItem->GetReport (Row(), myDescription) : Handle(Message_Report)();

  MessageModel_ItemBase::Init();
}

// =======================================================================
// function : getReport
// purpose :
// =======================================================================
const Handle(Message_Report)& MessageModel_ItemReport::getReport() const
{
  initItem();
  return myReport;
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void MessageModel_ItemReport::Reset()
{
  MessageModel_ItemBase::Reset();
  myReport = Handle(Message_Report)();
  myChildAlerts.Clear();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void MessageModel_ItemReport::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<MessageModel_ItemReport*>(this)->Init();
}

// =======================================================================
// function : FindReportItem
// purpose :
// =======================================================================
MessageModel_ItemReportPtr MessageModel_ItemReport::FindReportItem (const TreeModel_ItemBasePtr& theItem)
{
  TreeModel_ItemBasePtr anItem = theItem;
  while (anItem)
  {
    if (MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(anItem))
      return aReportItem;

    anItem = anItem->Parent();
  }
  return MessageModel_ItemReportPtr();
}

// =======================================================================
// function : FindReport
// purpose :
// =======================================================================
Handle(Message_Report) MessageModel_ItemReport::FindReport (const MessageModel_ItemBasePtr& theItem)
{
  Handle(Message_Report) aReport;

  MessageModel_ItemBasePtr anItem = theItem;
  while (anItem)
  {
    MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(anItem);

    if (aReportItem)
      return aReportItem->GetReport();

    anItem = itemDynamicCast<MessageModel_ItemBase>(anItem->Parent());
  }
  return NULL;
}
