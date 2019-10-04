// Created on: 2018-06-10
// Created by: Natalia Ermolaeva
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

#include <Message_PerfMeter.hxx>

#include <Message_AlertExtended.hxx>
#include <Message_CompositeAlerts.hxx>

#include <OSD_Chronometer.hxx>
#include <OSD_MemInfo.hxx>

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================

Message_PerfMeter::~Message_PerfMeter()
{
  releaseAlert (0);
}

//=======================================================================
//function : GetAlert
//purpose  :
//=======================================================================

Handle(Message_AlertExtended) Message_PerfMeter::GetAlert (const Standard_Integer theLevelId) const
{
  if (!myActiveAlerts.IsBound (theLevelId))
    return Handle(Message_AlertExtended)();

  return myActiveAlerts.Find (theLevelId).myAlert;
}

//=======================================================================
//function : AddAlert
//purpose  :
//=======================================================================

void Message_PerfMeter::AddAlert (const Handle(Message_Alert)& theAlert,
                                  const Message_PerfMeterMode theMode)
{
  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anExtendedAlert.IsNull())
    return;

  if (myActiveAlerts.IsEmpty())
  {
    setAlert (0, theAlert, theMode);
    return;
  }

  Standard_Integer aSize = myActiveAlerts.Size();
  // looking for the parent of the parameter alert to release the previous alert
  for (Standard_Integer aLevelId = 0; aLevelId < aSize; aLevelId++)
  {
    Handle(Message_AlertExtended) anAlert = GetAlert (aLevelId);
    Handle(Message_CompositeAlerts) aCompositeAlert = anAlert->GetCompositeAlerts();
    Standard_ASSERT_RETURN (! aCompositeAlert.IsNull(), "Any alert of the performance meter has composite and children",);
    if (!aCompositeAlert->HasAlert (theAlert))
      continue;
    // parent alert is found
    if (myActiveAlerts.IsBound (aLevelId + 1))
      releaseAlert (aLevelId + 1);

    setAlert (aLevelId + 1, theAlert, theMode);
    return;
  }
  Standard_Boolean isDone = Standard_True;
  Standard_ASSERT_RETURN (!isDone, "Alert should be processed above",);
}

//=======================================================================
//function : setAlert
//purpose  :
//=======================================================================

Standard_Boolean Message_PerfMeter::setAlert (const Standard_Integer theLevelId,
                                              const Handle(Message_Alert)& theAlert,
                                              const Message_PerfMeterMode theMode)
{
  if (!GetAlert (theLevelId).IsNull())
    return Standard_False;

  Handle(Message_AlertExtended) anAlertExtended = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anAlertExtended.IsNull())
    return Standard_False;

  myActiveAlerts.Bind (theLevelId, AlertInfo (anAlertExtended, getCurrentInfo (theMode)));
  anAlertExtended->SetPerfMeter (this);
  myMode = theMode;
  return Standard_True;
}

//=======================================================================
//function : releaseAlert
//purpose  :
//=======================================================================

Standard_Boolean Message_PerfMeter::releaseAlert (const Standard_Integer theLevelId)
{
  // release alerts from the tail till the given alert
  for (Standard_Integer aLevelId = myActiveAlerts.Extent() - 1; aLevelId >= theLevelId; aLevelId--)
  {
    if (!myActiveAlerts.IsBound (aLevelId))
      continue;
    stopAlert (aLevelId);
  }

  return Standard_True;
}

//=======================================================================
//function : stopAlert
//purpose  :
//=======================================================================

Standard_Boolean Message_PerfMeter::stopAlert (const Standard_Integer theLevelId)
{
  if (!myActiveAlerts.IsBound (theLevelId))
    return Standard_False;

  AlertInfo anAlertInfo = myActiveAlerts.Find (theLevelId);
  Handle(Message_AlertExtended) anExtendedAlert = anAlertInfo.myAlert;

  anExtendedAlert->SetMetricValues (anAlertInfo.myStartValue, getCurrentInfo(myMode));
  anExtendedAlert->SetPerfMeter (0);
  myActiveAlerts.UnBind (theLevelId);

  return Standard_True;
}

//=======================================================================
//function : getLevel
//purpose  :
//=======================================================================

Standard_Integer Message_PerfMeter::getLevel (const Handle(Message_Alert)& theAlert)
{
  for (NCollection_DataMap<Standard_Integer, AlertInfo>::Iterator anIterator (myActiveAlerts);
    anIterator.More(); anIterator.Next())
  {
    if (anIterator.Value().myAlert == theAlert)
      return anIterator.Key();
  }
  return -1;
}

//=======================================================================
//function : getCurrentInfo
//purpose  :
//=======================================================================

Standard_Real Message_PerfMeter::getCurrentInfo (const Message_PerfMeterMode theMode)
{
  if (theMode == Message_PerfMeterMode_UserTimeCPU ||
      theMode == Message_PerfMeterMode_SystemTimeInfo)
  {
    Standard_Real aUserSeconds, aSystemSeconds;
    OSD_Chronometer::GetThreadCPU (aUserSeconds, aSystemSeconds);
    return theMode == Message_PerfMeterMode_UserTimeCPU ? aUserSeconds : aSystemSeconds;
  }

  OSD_MemInfo::Counter aType;
  switch (theMode)
  {
    case Message_PerfMeterMode_MemPrivate:        aType = OSD_MemInfo::MemPrivate; break;
    case Message_PerfMeterMode_MemVirtual:        aType = OSD_MemInfo::MemVirtual; break;
    case Message_PerfMeterMode_MemWorkingSet:     aType = OSD_MemInfo::MemWorkingSet; break;
    case Message_PerfMeterMode_MemWorkingSetPeak: aType = OSD_MemInfo::MemWorkingSetPeak; break;
    case Message_PerfMeterMode_MemSwapUsage:      aType = OSD_MemInfo::MemSwapUsage; break;
    case Message_PerfMeterMode_MemSwapUsagePeak:  aType = OSD_MemInfo::MemSwapUsagePeak; break;
    case Message_PerfMeterMode_MemHeapUsage:      aType = OSD_MemInfo::MemHeapUsage; break;
    default: return Message_AlertExtended::GetUndefinedMetric(); break;
  }

  OSD_MemInfo aMemInfo (Standard_False);
  aMemInfo.Update (aType);
  return (Standard_Real)aMemInfo.ValuePreciseMiB (aType);
}
