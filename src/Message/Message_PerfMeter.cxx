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

#include <Message_AlertNode.hxx>

#include <OSD_Chronometer.hxx>

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

Handle(Message_AlertNode) Message_PerfMeter::GetAlert (const Standard_Integer theLevelId) const
{
  if (!myActiveAlerts.IsBound (theLevelId))
    return Handle(Message_AlertNode)();

  return myActiveAlerts.Find (theLevelId).myAlert;
}

//=======================================================================
//function : AddAlert
//purpose  :
//=======================================================================

void Message_PerfMeter::AddAlert (const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertNode) aNodeAlert = Handle(Message_AlertNode)::DownCast (theAlert);
  if (aNodeAlert.IsNull())
    return;

  if (myActiveAlerts.IsEmpty())
  {
    setAlert (0, theAlert);
    return;
  }

  Standard_Integer aSize = myActiveAlerts.Size();
  // looking for the parent of the parameter alert to release the previous alert
  for (Standard_Integer aLevelId = 0; aLevelId < aSize; aLevelId++)
  {
    Handle(Message_AlertNode) anAlert = GetAlert (aLevelId);
    if (!anAlert->HasAlert (theAlert))
      continue;
    // parent alert is found
    if (myActiveAlerts.IsBound (aLevelId + 1))
      releaseAlert (aLevelId + 1);

    setAlert (aLevelId + 1, theAlert);
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
                                              const Handle(Message_Alert)& theAlert)
{
  if (!GetAlert (theLevelId).IsNull())
    return Standard_False;

  Handle(Message_AlertNode) anAlertExtended = Handle(Message_AlertNode)::DownCast (theAlert);
  if (anAlertExtended.IsNull())
    return Standard_False;

  myActiveAlerts.Bind (theLevelId, AlertInfo (anAlertExtended, getCurrentTime()));
  anAlertExtended->SetPerfMeter (this);
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
  Handle(Message_AlertNode) aNodeAlert = anAlertInfo.myAlert;

  aNodeAlert->SetCumulativeTime (getCurrentTime() - anAlertInfo.myStartTime);
  aNodeAlert->SetPerfMeter (0);
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
//function : getCurrentTime
//purpose  :
//=======================================================================

Standard_Real Message_PerfMeter::getCurrentTime()
{
  Standard_Real aSystemSeconds, aCurrentSeconds;
  OSD_Chronometer::GetThreadCPU (aCurrentSeconds, aSystemSeconds);

  return aCurrentSeconds;
}
