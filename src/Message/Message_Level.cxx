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

#include <Message_Level.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_Messenger.hxx>
#include <Message_Report.hxx>

#include <OSD_Chronometer.hxx>
#include <OSD_MemInfo.hxx>

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================
Message_Level::Message_Level()
{
  const Handle(Message_Report)& aDefaultReport = Message::DefaultReport();
  if (!aDefaultReport.IsNull() && aDefaultReport->IsActiveInMessenger())
    aDefaultReport->AddLevel (this);
}

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================
Message_Level::~Message_Level()
{
  Remove();
}

//=======================================================================
//function : SetRootAlert
//purpose  :
//=======================================================================
void Message_Level::SetRootAlert (const Handle(Message_AlertExtended)& theAlert)
{
  myRootAlert = theAlert;
  startAlert (myRootAlert);
}

//=======================================================================
//function : AddAlert
//purpose  :
//=======================================================================
Standard_Boolean Message_Level::AddAlert (const Message_Gravity theGravity,
                                          const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertExtended) anAlertExtended = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anAlertExtended.IsNull())
    return Standard_False;

  if (myRootAlert.IsNull())
    return Standard_False;

  // looking for the parent of the parameter alert to release the previous alert
  Handle(Message_AlertExtended) aRootAlert = myRootAlert;
  Handle(Message_CompositeAlerts) aCompositeAlert = aRootAlert->CompositeAlerts (Standard_True);

  // update metrics of the previous alert
  stopAlert (myLastAlert);

  myLastAlert = anAlertExtended;

  // set start metrics of the new alert
  startAlert (myLastAlert);

  // add child alert
  aCompositeAlert->AddAlert (theGravity, theAlert);

  return Standard_True;
}

//=======================================================================
//function : AddLevelAlert
//purpose  :
//=======================================================================
Standard_Boolean Message_Level::AddLevelAlert (const Message_Gravity theGravity,
                                               const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertExtended) aRootAlert = !myLastAlert.IsNull() ? myLastAlert : myRootAlert;
  if (aRootAlert.IsNull())
    return Standard_False;

  Handle(Message_CompositeAlerts) aCompositeAlert = aRootAlert->CompositeAlerts (Standard_True);
  // add child alert
  aCompositeAlert->AddAlert (theGravity, theAlert);

  return Standard_True;
}

//=======================================================================
//function : Remove()
//purpose  :
//=======================================================================
void Message_Level::Remove()
{
  const Handle(Message_Report)& aDefaultReport = Message::DefaultReport();
  if (aDefaultReport.IsNull() || !aDefaultReport->IsActiveInMessenger())
    return;

  stopAlert (myLastAlert);
  stopAlert (myRootAlert);

  if (!Message::DefaultReport().IsNull())
    Message::DefaultReport()->RemoveLevel (this);
}

//=======================================================================
//function : setAlertMetrics
//purpose  :
//=======================================================================
void Message_Level::setAlertMetrics (const Handle(Message_AlertExtended)& theAlert,
                                                 const Standard_Boolean theStartValue)
{
  if (theAlert.IsNull())
    return;

  Handle(Message_AttributeMeter) aMeterAttribute = Handle(Message_AttributeMeter)::DownCast (theAlert->Attribute());
  if (aMeterAttribute.IsNull())
    return;

  Handle(Message_Report) aReport = Message::DefaultReport (Standard_True);
  const NCollection_Map<Message_MetricType>& anActiveMetrics = aReport->ActiveMetrics();

  // time metrics
  if (anActiveMetrics.Contains (Message_MetricType_UserTimeCPU) ||
      anActiveMetrics.Contains (Message_MetricType_SystemTimeInfo))
  {
    Standard_Real aUserSeconds, aSystemSeconds;
    OSD_Chronometer::GetThreadCPU (aUserSeconds, aSystemSeconds);

    if (anActiveMetrics.Contains (Message_MetricType_UserTimeCPU))
    {
      if (theStartValue)
        aMeterAttribute->SetStartValue (Message_MetricType_UserTimeCPU, aUserSeconds);
      else
        aMeterAttribute->SetStopValue (Message_MetricType_UserTimeCPU, aUserSeconds);
    }
    if (anActiveMetrics.Contains (Message_MetricType_SystemTimeInfo))
    {
      if (theStartValue)
        aMeterAttribute->SetStartValue (Message_MetricType_SystemTimeInfo, aSystemSeconds);
      else
        aMeterAttribute->SetStopValue (Message_MetricType_SystemTimeInfo, aSystemSeconds);
    }
  }
  // memory metrics
  NCollection_Map<OSD_MemInfo::Counter> aCounters;
  for (NCollection_Map<Message_MetricType>::Iterator anIterator (anActiveMetrics); anIterator.More(); anIterator.Next())
  {
    OSD_MemInfo::Counter aMemInfo;
    if (!Message::ToOSDMetric (anIterator.Value(), aMemInfo))
      continue;

    aCounters.Add (aMemInfo);
  }
  if (aCounters.IsEmpty())
    return;

  OSD_MemInfo aMemInfo (Standard_False);
  //aMemInfo.SetActiveCounters (aCounters);
  aMemInfo.Update ();
  Message_MetricType aMetricType;
  for (NCollection_Map<OSD_MemInfo::Counter>::Iterator anIterator (aCounters); anIterator.More(); anIterator.Next())
  {
    if (!Message::ToMessageMetric (anIterator.Value(), aMetricType))
      continue;

    if (theStartValue)
      aMeterAttribute->SetStartValue (aMetricType, (Standard_Real)aMemInfo.ValuePreciseMiB (anIterator.Value()));
    else
      aMeterAttribute->SetStopValue (aMetricType, (Standard_Real)aMemInfo.ValuePreciseMiB (anIterator.Value()));
  }
}
