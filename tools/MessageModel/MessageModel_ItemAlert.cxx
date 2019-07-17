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

#include <inspector/MessageModel_ItemPropertiesAttributeStream.hxx>
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
  if (theRole == Qt::ForegroundRole)
  {
    MessageModel_ItemReportPtr aReportItem = MessageModel_ItemReport::FindReportItem (Parent());
    if (!aReportItem)
      return QVariant();

    Handle(Message_Report) aReport = aReportItem->GetReport();
    if (aReport.IsNull())
      return QVariant();

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
  // if the alert is composite, process the real alert
  if (theRole == Qt::DecorationRole && Column() == 0)
  {
    Handle(Message_AlertExtended) anExtAttribute = Handle(Message_AlertExtended)::DownCast (anAlert);
    if (anExtAttribute.IsNull())
      return QVariant();

    Handle(Message_Attribute) anAttribute = anExtAttribute->Attribute();

    if (anAttribute->IsKind (STANDARD_TYPE (TopoDS_AlertAttribute)))
      return QIcon (":/icons/item_shape.png");
    else if (!Handle(Message_AttributeStream)::DownCast (anAttribute).IsNull())
      return QIcon (":/icons/item_vectorOfValues.png");
    else
      return QVariant();
  }

  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  const Message_ListOfAlert& aUnitedAlerts = GetUnitedAlerts();
  if (anAlert.IsNull() && !aUnitedAlerts.IsEmpty()) // give information about last united alert
    anAlert = !isReversed() ? aUnitedAlerts.Last() : aUnitedAlerts.First();
  if (anAlert.IsNull())
    return QVariant();

  switch (Column())
  {
    case 0:
      return theRole == Qt::DisplayRole ? anAlert->GetMessageKey() : anAlert->DynamicType()->Name();
    case 1: return QVariant(); // visibility state
    case 2: return rowCount() > 0 ? QVariant (rowCount()) : QVariant();
    //case 3: return anAlert->ElapsedTime() < 0 ? QVariant() : anAlert->ElapsedTime();
    case 3: return AmountElapsedTime (anAlert);
    case 4:
    {
      TreeModel_ItemBasePtr aParentItem = Parent();
      MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport> (aParentItem);
      while (!aReportItem)
      {
        aParentItem = aParentItem->Parent();
        aReportItem = itemDynamicCast<MessageModel_ItemReport> (aParentItem);
      }
      double aDivideTo = MessageModel_ItemReport::AmountElapsedTime (aReportItem->GetReport());
      return aDivideTo == 0 ? QVariant() : 100. * AmountElapsedTime (anAlert) / aDivideTo;
    }
    case 5: return MessageModel_Tools::GetPointerAlertInfo (anAlert).ToCString();
    case 6: return MessageModel_Tools::GetShapeTypeAlertInfo (anAlert).ToCString();
    case 7: return MessageModel_Tools::GetStandardTypeAlertInfo (anAlert).ToCString();
    case 8:
    {
      Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlert);
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
    return GetUnitedAlerts().Size();

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlert);
  if (anExtendedAlert.IsNull())
    return GetUnitedAlerts().Size();

  Handle(Message_CompositeAlerts) aCompositeAlert = anExtendedAlert->GetCompositeAlerts();
  if (aCompositeAlert.IsNull())
    return GetUnitedAlerts().Size();

  MessageModel_ItemAlert* aCurrentItem = (MessageModel_ItemAlert*)this;
  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    const Message_ListOfAlert& anAlerts  = aCompositeAlert->GetAlerts ((Message_Gravity)aGravityId);
    if (isUniteAlerts())
    {
      MessageModel_Tools::GetUnitedAlerts (anAlerts, aCurrentItem->myChildAlerts);
    }
    else
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
      if (anAlerts.Size() == 1)
        myAlert = anAlerts.First();
      else
        myUnitedAlerts = anAlerts;
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
        if (anAlerts.Size() == 1)
          myAlert = anAlerts.First();
        else
          myUnitedAlerts = anAlerts;
      }
    }
  }

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(myAlert);
  if (!anExtendedAlert.IsNull() && !anExtendedAlert->Attribute().IsNull())
  {
    Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();

    if (!anAttribute.IsNull())
    {
      if (!Handle(Message_AttributeStream)::DownCast(anAttribute).IsNull())
      {
        TreeModel_ItemBasePtr anItem = Parent()->Child (Row(), Column(), false);
        SetProperties (new MessageModel_ItemPropertiesAttributeStream (anItem));
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
  }
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
  myUnitedAlerts.Clear();
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

// =======================================================================
// function : getUnitedAlerts
// purpose :
// =======================================================================
const Message_ListOfAlert& MessageModel_ItemAlert::getUnitedAlerts() const
{
  initItem();
  return myUnitedAlerts;
}

// =======================================================================
// function : AmountElapsedTime
// purpose :
// =======================================================================
double MessageModel_ItemAlert::AmountElapsedTime(const Handle(Message_Alert)& theAlert)
{
  if (theAlert.IsNull())
    return 0;

  double anAmountTime = 0;
  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(theAlert);
  if (anExtendedAlert->GetCumulativeTime() > 0)
    anAmountTime = anExtendedAlert->GetCumulativeTime();

  return anAmountTime;
}
