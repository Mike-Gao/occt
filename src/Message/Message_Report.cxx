// Created on: 2017-06-26
// Created by: Andrey Betenev
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

#include <Message_Report.hxx>

#include <Message_AlertExtended.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <Message_PerfMeter.hxx>
#include <Message_ReportCallBack.hxx>

#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Report,Standard_Transient)

static Handle(Message_Report) MyReport;

//=======================================================================
//function : Message_Report
//purpose  :
//=======================================================================

Message_Report::Message_Report ()
: myPerfMeterMode (Message_PerfMeterMode_None), myLimit (-1)
{
  SetActive (Standard_True);
}

//=======================================================================
//function : CurrentReport
//purpose  :
//=======================================================================
Handle(Message_Report) Message_Report::CurrentReport(const Standard_Boolean theToCreate)
{
  if (MyReport.IsNull() && theToCreate)
    MyReport = new Message_Report();

  return MyReport;
}

//=======================================================================
//function : AddAlert
//purpose  :
//=======================================================================

void Message_Report::AddAlert (Message_Gravity theGravity, const Handle(Message_Alert)& theAlert)
{
  if (!IsActive (theGravity))
    return;

  Standard_ASSERT_RETURN (! theAlert.IsNull(), "Attempt to add null alert",);
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Adding alert with gravity not in valid range",);

  Standard_Mutex::Sentry aSentry (myMutex);

  // iterate by already recorded alerts and try to merge new one with one of those
  Message_ListOfAlert &aList = myAlerts[theGravity];
  if (theAlert->SupportsMerge() && ! aList.IsEmpty())
  {
    // merge is performed only for alerts of exactly same type
    const Handle(Standard_Type)& aType = theAlert->DynamicType();
    for (Message_ListOfAlert::Iterator anIt(aList); anIt.More(); anIt.Next())
    {
      // if merged successfully, just return
      if (aType == anIt.Value()->DynamicType() && theAlert->Merge (anIt.Value()))
        return;
    }
  }

  // if not merged, just add to the list
  aList.Append (theAlert);

  if (!myCallBack.IsNull())
    myCallBack->Update (theAlert);
}

//=======================================================================
//function : AddAlert
//purpose  :
//=======================================================================

void Message_Report::AddAlert (const Message_Gravity theGravity, const Handle(Message_Alert)& theAlert,
                               Message_PerfMeter* thePerfMeter, const Handle(Message_Alert)& theParentAlert)
{
  if (!IsActive (theGravity))
    return;

  Standard_ASSERT_RETURN (!theAlert.IsNull(), "Attempt to add null alert",);
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Adding alert with gravity not in valid range",);

  Standard_Mutex::Sentry aSentry (myMutex);

  // iterate by already recorded alerts and try to merge new one with one of those
  Handle(Message_Alert) aParentAlert = theParentAlert;
  if (aParentAlert.IsNull() && thePerfMeter)
    aParentAlert = thePerfMeter->GetAlert();
  if (aParentAlert.IsNull())
    aParentAlert = getLastAlertInPerfMeter(theGravity);

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(aParentAlert);
  Handle(Message_CompositeAlerts) aCompositeAlert = !anExtendedAlert.IsNull() ? anExtendedAlert->GetCompositeAlerts (Standard_True)
    : Handle(Message_CompositeAlerts)();
  Message_ListOfAlert& aList = !aCompositeAlert.IsNull() ? aCompositeAlert->GetAlerts (theGravity) : myAlerts[theGravity];

  if (theAlert->SupportsMerge() && !aList.IsEmpty())
  {
    // merge is performed only for alerts of exactly same type and same name
    const Handle(Standard_Type)& aType = theAlert->DynamicType();
    for (Message_ListOfAlert::Iterator anIt(aList); anIt.More(); anIt.Next())
    {
      // if merged successfully, just return
      if (aType == anIt.Value()->DynamicType() && theAlert->Merge (anIt.Value()))
        return;
    }
  }

  // if not merged, just add to the list
  aList.Append (theAlert);
  // remove alerts under the report only
  if (theParentAlert.IsNull() && aParentAlert.IsNull() && myLimit > 0 && aList.Extent() > myLimit)
    aList.RemoveFirst();

  if (thePerfMeter)
    thePerfMeter->AddAlert (theAlert, PerfMeterMode());

  if (!myCallBack.IsNull())
    myCallBack->Update(theAlert);
}

//=======================================================================
//function : GetAlerts
//purpose  :
//=======================================================================

const Message_ListOfAlert& Message_Report::GetAlerts (Message_Gravity theGravity) const
{
  static const Message_ListOfAlert anEmptyList;
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", anEmptyList);
  return myAlerts[theGravity];
}

//=======================================================================
//function : CumulativeMetric
//purpose  :
//=======================================================================

Standard_Real Message_Report::CumulativeMetric (const Message_Gravity theGravity) const
{
  switch (myPerfMeterMode)
  {
    case Message_PerfMeterMode_None: return 0.0;
    case Message_PerfMeterMode_UserTimeCPU:
    case Message_PerfMeterMode_SystemTimeInfo:
    {
      Standard_Real aStartTime = Message_AlertExtended::GetUndefinedMetric();
      Standard_Boolean isFound = Standard_False;
      for (Message_ListOfAlert::Iterator anAlertsIt (GetAlerts (theGravity)); anAlertsIt.More(); anAlertsIt.Next())
      {
        Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlertsIt.Value());
        if (anExtendedAlert.IsNull() && anExtendedAlert->IsMetricValid())
        {
          aStartTime = anExtendedAlert->MetricStart();
          isFound = Standard_True;
          break;
        }
      }
      if (!isFound)
        return 0.0;

      Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(getLastAlert (theGravity));
      if (anExtendedAlert.IsNull() || !anExtendedAlert->IsMetricValid())
        return 0.0;

      return anExtendedAlert->MetricStop() - aStartTime;
    }
    case Message_PerfMeterMode_MemPrivate:
    case Message_PerfMeterMode_MemVirtual:
    case Message_PerfMeterMode_MemWorkingSet:
    {
      Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(getLastAlert (theGravity));
      if (!anExtendedAlert.IsNull())
        return anExtendedAlert->MetricStop();
    }
  }
  return 0.0;
}

//=======================================================================
//function : getLastAlert
//purpose  :
//=======================================================================

Handle(Message_Alert) Message_Report::getLastAlert (const Message_Gravity theGravity) const
{
  if (!IsActive (theGravity))
    return Handle(Message_Alert)();

  const Message_ListOfAlert& anAlerts = GetAlerts (theGravity);
  if (anAlerts.IsEmpty())
    return Handle(Message_Alert)();

  Handle(Message_Alert) aLastAlert = anAlerts.Last();

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast (aLastAlert);
  if (anExtendedAlert.IsNull())
    return aLastAlert;
  Handle(Message_CompositeAlerts) aCompositeAlert = anExtendedAlert->GetCompositeAlerts (Standard_True);
  if (aCompositeAlert.IsNull())
    return aLastAlert;

  while (!aCompositeAlert.IsNull() && !aCompositeAlert->GetAlerts (theGravity).IsEmpty())
  {
    Handle(Message_Alert) anAlert = aCompositeAlert->GetAlerts (theGravity).Last();

    Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast (anAlert);
    if (anExtendedAlert.IsNull())
    {
      aLastAlert = anAlert;
      break;
    }

    aLastAlert = anExtendedAlert;
    aCompositeAlert = anExtendedAlert->GetCompositeAlerts();
  }

  // if alert has perf meter, use as the last alert, an alert of the perf meter
  anExtendedAlert = Handle(Message_AlertExtended)::DownCast (aLastAlert);
  if (anExtendedAlert.IsNull())
    return aLastAlert;

  return aLastAlert;
}

//=======================================================================
//function : getLastAlertInPerfMeter
//purpose  :
//=======================================================================

Handle(Message_Alert) Message_Report::getLastAlertInPerfMeter (const Message_Gravity theGravity) const
{
  if (!IsActive (theGravity))
    return Handle(Message_Alert)();

  const Message_ListOfAlert& anAlerts = GetAlerts (theGravity);
  if (anAlerts.IsEmpty())
    return Handle(Message_Alert)();

  Handle(Message_Alert) aLastAlert = anAlerts.Last();

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast (aLastAlert);
  if (anExtendedAlert.IsNull())
    return aLastAlert;
  Handle(Message_CompositeAlerts) aCompositeAlert = anExtendedAlert->GetCompositeAlerts (Standard_True);
  if (aCompositeAlert.IsNull())
    return aLastAlert;

  Standard_Boolean aHasPerfMeter = anExtendedAlert->GetPerfMeter() != NULL;
  if (!aHasPerfMeter) // the alert has finished and is not the last alert anymore
    return Handle(Message_Alert)();

  while (!aCompositeAlert.IsNull() && !aCompositeAlert->GetAlerts (theGravity).IsEmpty())
  {
    Handle(Message_Alert) anAlert = aCompositeAlert->GetAlerts (theGravity).Last();

    Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast (anAlert);
    if (anExtendedAlert.IsNull())
    {
      if (!aHasPerfMeter) // if there is a perf meter, use alert of it
        aLastAlert = anAlert;
      break;
    }

    if (!aHasPerfMeter)
      aHasPerfMeter = anExtendedAlert->GetPerfMeter() != NULL;
    else if (!anExtendedAlert->GetPerfMeter())
      break; // last alert is the previous alert where perf meter is not NULL

    aLastAlert = anExtendedAlert;
    aCompositeAlert = anExtendedAlert->GetCompositeAlerts();
  }

  // if alert has perf meter, use as the last alert, an alert of the perf meter
  anExtendedAlert = Handle(Message_AlertExtended)::DownCast (aLastAlert);
  if (anExtendedAlert.IsNull())
    return aLastAlert;
  //if (anExtendedAlert->GetPerfMeter())
  //  aLastAlert = anExtendedAlert->GetPerfMeter()->GetAlert();

  return aLastAlert;
}

//=======================================================================
//function : HasAlert
//purpose  :
//=======================================================================

Standard_Boolean Message_Report::HasAlert (const Handle(Standard_Type)& theType)
{
  for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
  {
    if (HasAlert (theType, (Message_Gravity)iGravity))
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : HasAlert
//purpose  :
//=======================================================================

Standard_Boolean Message_Report::HasAlert (const Handle(Standard_Type)& theType, Message_Gravity theGravity)
{
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", Standard_False);
  for (Message_ListOfAlert::Iterator anIt (myAlerts[theGravity]); anIt.More(); anIt.Next())
  {
    if (anIt.Value()->IsInstance(theType))
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Message_Report::Clear ()
{
  for (unsigned int i = 0; i < sizeof(myAlerts)/sizeof(myAlerts[0]); ++i)
  {
    myAlerts[i].Clear();
  }
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Message_Report::Clear (Message_Gravity theGravity)
{
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", );
  myAlerts[theGravity].Clear();
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Message_Report::Clear (const Handle(Standard_Type)& theType)
{
  for (unsigned int i = 0; i < sizeof(myAlerts)/sizeof(myAlerts[0]); ++i)
  {
    for (Message_ListOfAlert::Iterator anIt (myAlerts[i]); anIt.More(); )
    {
      if (anIt.Value().IsNull() || anIt.Value()->IsInstance (theType))
      {
        myAlerts[i].Remove (anIt);
      }
      else
      {
        anIt.More();
      }
    }
  }
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Message_Report::Dump (Standard_OStream& theOS)
{
  for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
  {
    Dump (theOS, (Message_Gravity)iGravity);
  }
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Message_Report::Dump (Standard_OStream& theOS, Message_Gravity theGravity)
{
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", );

  // report each type of warning only once
  NCollection_Map<Handle(Standard_Type)> aPassedAlerts;
  for (Message_ListOfAlert::Iterator anIt (myAlerts[theGravity]); anIt.More(); anIt.Next())
  {
    if (aPassedAlerts.Add (anIt.Value()->DynamicType()))
    {
      Message_Msg aMsg (anIt.Value()->GetMessageKey());
      theOS << aMsg.Original() << std::endl;
    }
  }
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Message_Report::SendMessages (const Handle(Message_Messenger)& theMessenger)
{
  for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
  {
    SendMessages (theMessenger, (Message_Gravity)iGravity);
  }
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Message_Report::SendMessages (const Handle(Message_Messenger)& theMessenger, Message_Gravity theGravity)
{
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", );

  // report each type of warning only once
  NCollection_Map<Handle(Standard_Type)> aPassedAlerts;
  for (Message_ListOfAlert::Iterator anIt (myAlerts[theGravity]); anIt.More(); anIt.Next())
  {
    if (aPassedAlerts.Add (anIt.Value()->DynamicType()))
    {
      Message_Msg aMsg (anIt.Value()->GetMessageKey());
      theMessenger->Send (aMsg, theGravity);
    }
  }
}

//=======================================================================
//function : Merge
//purpose  :
//=======================================================================

void Message_Report::Merge (const Handle(Message_Report)& theOther)
{
  for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
  {
    Merge (theOther, (Message_Gravity)iGravity);
  }
}

//=======================================================================
//function : Merge
//purpose  :
//=======================================================================

void Message_Report::Merge (const Handle(Message_Report)& theOther, Message_Gravity theGravity)
{
  for (Message_ListOfAlert::Iterator anIt (theOther->GetAlerts(theGravity)); anIt.More(); anIt.Next())
  {
    AddAlert (theGravity, anIt.Value());
  }
}

//=======================================================================
//function : SetActive
//purpose  :
//=======================================================================

void Message_Report::SetActive (const Standard_Boolean theActive, const Standard_Integer theGravity)
{
  if (theGravity < 0)
  {
    for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
      SetActive (theActive, iGravity);
    return;
  }

  Standard_ASSERT_RETURN (theGravity >= 0 && size_t (theGravity) < sizeof (myAlerts) / sizeof (myAlerts[0]), 
                          "Set active report with gravity not in valid range", );
  myIsActive[theGravity] = theActive;
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Message_Report::DumpJson (Standard_OStream& theOStream, const Standard_Integer) const
{
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myPerfMeterMode);
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myLimit);
}

//=======================================================================
//function : InitJson
//purpose  : 
//=======================================================================
Standard_Boolean Message_Report::InitJson (const Standard_SStream& theSStream, Standard_Integer& theStreamPos)
{
  Standard_Integer aPos = theStreamPos;

  Standard_Real PerfMeterMode;
  OCCT_INIT_FIELD_VALUE_NUMERICAL (theSStream, aPos, PerfMeterMode);
  myPerfMeterMode = (Message_PerfMeterMode)((Standard_Integer)PerfMeterMode);

  Standard_Real Limit;
  OCCT_INIT_FIELD_VALUE_NUMERICAL (theSStream, aPos, Limit);
  myLimit = (Standard_Integer)Limit;

  theStreamPos = aPos;
  return Standard_True;
}
