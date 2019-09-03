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

#include <inspector/MessageModel_ItemAlert.hxx>

#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <Message_AlertExtended.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_CompositeAlerts.hxx>

#include <Bnd_Box.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_AlertAttribute.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <QIcon>
#include <Standard_WarningsRestore.hxx>


// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant MessageModel_ItemAlert::initValue (const int theRole) const
{
  QVariant aParentValue = MessageModel_ItemBase::initValue (theRole);
  if (aParentValue.isValid())
    return aParentValue;

  MessageModel_ItemReportPtr aReportItem = MessageModel_ItemReport::FindReportItem (Parent());
  if (!aReportItem)
    return QVariant();

  Handle(Message_Report) aReport = aReportItem->GetReport();
  if (aReport.IsNull())
    return QVariant();

  Standard_Boolean isTimeReport = aReport->PerfMeterMode() == Message_PerfMeterMode_UserTimeCPU ||
                                  aReport->PerfMeterMode() == Message_PerfMeterMode_SystemTimeInfo;

  if (aReport->PerfMeterMode() == Message_PerfMeterMode_UserTimeCPU ||
      aReport->PerfMeterMode() == Message_PerfMeterMode_SystemTimeInfo)

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

  Handle(Message_Alert) anAlert = getAlert();
  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlert);

  // if the alert is composite, process the real alert
  if (theRole == Qt::DecorationRole && Column() == 0)
  {
    if (anExtendedAlert.IsNull())
      return QVariant();

    Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();

    if (anAttribute->IsKind (STANDARD_TYPE (TopoDS_AlertAttribute)))
      return QIcon (":/icons/item_shape.png");
    else if (!Handle(Message_AttributeStream)::DownCast (anAttribute).IsNull())
      return QIcon (":/icons/item_vectorOfValues.png");
    else
      return QVariant();
  }

  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  if (anAlert.IsNull())
    return QVariant();

  switch (Column())
  {
    case 0:
      return theRole == Qt::DisplayRole ? anAlert->GetMessageKey() : anAlert->DynamicType()->Name();
    //case 1: return QVariant(); // visibility state
    //case 2: return rowCount() > 0 ? QVariant (rowCount()) : QVariant();
    //case 3: return anAlert->ElapsedTime() < 0 ? QVariant() : anAlert->ElapsedTime();
    case 4:
    {
      if (!anExtendedAlert->IsMetricValid())
        return QVariant();

      if (isTimeReport)
        return anExtendedAlert->MetricStop() - anExtendedAlert->MetricStart();
      else
        return anExtendedAlert->MetricStart();
    }
    case 5:
    {
      if (!anExtendedAlert->IsMetricValid())
        return QVariant();

      if (isTimeReport)
      {
        TreeModel_ItemBasePtr aParentItem = Parent();
        MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport> (aParentItem);
        while (!aReportItem)
        {
          aParentItem = aParentItem->Parent();
          aReportItem = itemDynamicCast<MessageModel_ItemReport> (aParentItem);
        }
        double aDivideTo = aReport->CumulativeMetric (Message_Info);
        double anAlertTime = anExtendedAlert->MetricStop() - anExtendedAlert->MetricStart();

        return aDivideTo == 0 ? QVariant() : 100. * anAlertTime / aDivideTo;
      }
      else
      {
        TreeModel_ItemBasePtr aParentItem = Parent();
        //MessageModel_ItemAlertPtr aParentAlertItem = itemDynamicCast<MessageModel_ItemAlert> (aParentItem);
        //if (aParentAlertItem)
        //{
          //double aDeltaToParent = CumulativeMetric (anAlert) - CumulativeMetric (aParentAlertItem->GetAlert());

          //return fabs (aDeltaToParent) > Precision::Confusion() ? QVariant (aDeltaToParent) : QVariant();
        //}
        double anAlertMem = anExtendedAlert->MetricStop() - anExtendedAlert->MetricStart();
        return anAlertMem;
      }
    }
    case 6: return MessageModel_Tools::GetPointerAlertInfo (anAlert).ToCString();
    case 7: return MessageModel_Tools::GetShapeTypeAlertInfo (anAlert).ToCString();
    case 8: return MessageModel_Tools::GetStandardTypeAlertInfo (anAlert).ToCString();
    case 9:
    {
      if (!anExtendedAlert.IsNull() && !anExtendedAlert->Attribute().IsNull())
      {
        TCollection_AsciiString aDescription = anExtendedAlert->Attribute()->GetDescription();
        return theRole == Qt::DisplayRole ? TreeModel_Tools::CutString (aDescription.ToCString())
                                          : aDescription.ToCString();
      }
    }
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int MessageModel_ItemAlert::initRowCount() const
{
  const Handle(Message_Alert)& anAlert = getAlert();
  if (anAlert.IsNull())
    return 0;

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlert);
  if (anExtendedAlert.IsNull())
    return 0;

  Handle(Message_CompositeAlerts) aCompositeAlert = anExtendedAlert->GetCompositeAlerts();
  if (aCompositeAlert.IsNull())
    return 0;

  MessageModel_ItemAlert* aCurrentItem = (MessageModel_ItemAlert*)this;
  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    const Message_ListOfAlert& anAlerts  = aCompositeAlert->GetAlerts ((Message_Gravity)aGravityId);
    {
      for (Message_ListOfAlert::Iterator anIt(anAlerts); anIt.More(); anIt.Next())
      {
        Message_ListOfAlert aCurAlerts;
        aCurAlerts.Append (anIt.Value());
        aCurrentItem->myChildAlerts.Bind(myChildAlerts.Size(), aCurAlerts);
      }
    }
  }

  return aCurrentItem->myChildAlerts.Size();
}

// =======================================================================
// function : GetStream
// purpose :
// =======================================================================
void MessageModel_ItemAlert::GetStream (Standard_OStream& OS) const
{
  return;
  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast (getAlert());
  if (!anExtendedAlert.IsNull() && !anExtendedAlert->Attribute().IsNull())
  {
    Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();
    if (!anAttribute.IsNull())
    {
      if (!Handle(Message_AttributeStream)::DownCast(anAttribute).IsNull())
      {
        //if (GetProperties().IsNull())
        //{
        //  TreeModel_ItemBasePtr anItem = Parent()->Child (Row(), Column(), false);
        //  SetProperties (new MessageModel_ItemPropertiesAttributeStream (anItem));
        //}
        Handle(Message_AttributeStream) anAttributeStream = Handle(Message_AttributeStream)::DownCast (anExtendedAlert->Attribute());
        //Handle(MessageModel_ItemPropertiesAttributeStream) aProperties = Handle(MessageModel_ItemPropertiesAttributeStream)::DownCast (GetProperties());
        OS << anAttributeStream->GetStream().str();
      }
    }
  }
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_ItemAlert::createChild (int theRow, int theColumn)
{
  return MessageModel_ItemAlert::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageModel_ItemAlert::Init()
{
  MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport> (Parent());
  MessageModel_ItemAlertPtr anAlertItem;
  Handle(Message_Alert) anAlert;
  if (aReportItem)
  {
    Message_ListOfAlert anAlerts;
    if (aReportItem->GetChildAlerts (Row(), anAlerts))
    {
      myAlert = anAlerts.First();
    }
  }
  else
  {
    anAlertItem = itemDynamicCast<MessageModel_ItemAlert> (Parent());
    if (anAlertItem)
    {
      Message_ListOfAlert anAlerts;
      if (anAlertItem->GetChildAlerts (Row(), anAlerts))
      {
        myAlert = anAlerts.First();
      }
    }
  }

  /*Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(myAlert);
  if (!anExtendedAlert.IsNull() && !anExtendedAlert->Attribute().IsNull())
  {
    Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();
    if (!anAttribute.IsNull())
    {
      if (!Handle(Message_AttributeStream)::DownCast(anAttribute).IsNull())
      {
        if (GetProperties().IsNull())
        {
          TreeModel_ItemBasePtr anItem = Parent()->Child (Row(), Column(), false);
          SetProperties (new MessageModel_ItemPropertiesAttributeStream (anItem));
        }
        Handle(Message_AttributeStream) anAttributeStream = Handle(Message_AttributeStream)::DownCast (anExtendedAlert->Attribute());
        Handle(MessageModel_ItemPropertiesAttributeStream) aProperties = Handle(MessageModel_ItemPropertiesAttributeStream)::DownCast (GetProperties());
        aProperties->Init (anAttributeStream->GetStream());
      }
      //if (anAttribute->IsKind (STANDARD_TYPE (Message_AttributeObject)))
      //  myPresentations.Append (Handle(Message_AttributeObject)::DownCast (anAttribute)->GetObject());
      //if (anAttribute->IsKind (STANDARD_TYPE (TopoDS_AlertAttribute)))
      //  myPresentations.Append (new Convert_TransientShape (Handle(TopoDS_AlertAttribute)::DownCast (anAttribute)->GetShape()));
    }
    //TCollection_AsciiString aDescription = anExtendedAlert->Attribute()->GetDescription();
    //Bnd_Box aBox;
    //if (aBox.Init (Standard_SStream (aDescription.ToCString())))
    //  myPresentations.Append (new Convert_TransientShape (Convert_Tools::CreateShape (aBox)));
  }*/
  MessageModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void MessageModel_ItemAlert::Reset()
{
  MessageModel_ItemBase::Reset();
  myAlert = Handle(Message_Alert)();
  myChildAlerts.Clear();
  myPresentations.Clear();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void MessageModel_ItemAlert::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<MessageModel_ItemAlert*>(this)->Init();
}

// =======================================================================
// function : getAlert
// purpose :
// =======================================================================
const Handle(Message_Alert)& MessageModel_ItemAlert::getAlert() const
{
  initItem();
  return myAlert;
}
