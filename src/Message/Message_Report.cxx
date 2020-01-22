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

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterToReport.hxx>

#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Report,Standard_Transient)

//=======================================================================
//function : Message_Report
//purpose  :
//=======================================================================

Message_Report::Message_Report ()
: myLimit (-1)
{
}

//=======================================================================
//function : IsActiveInMessenger
//purpose  :
//=======================================================================
Standard_Boolean Message_Report::IsActiveInMessenger() const
{
  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();
  for (Message_SequenceOfPrinters::Iterator anIterator (aMessenger->Printers()); anIterator.More(); anIterator.Next())
  {
    if (anIterator.Value()->IsKind(STANDARD_TYPE (Message_PrinterToReport)) &&
        Handle(Message_PrinterToReport)::DownCast (anIterator.Value())->Report() == this)
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : AddLevel
//purpose  :
//=======================================================================
void Message_Report::AddLevel (Message_Level* theLevel)
{
  myAlertLevels.Append (theLevel);
}

//=======================================================================
//function : RemoveLevel
//purpose  :
//=======================================================================
void Message_Report::RemoveLevel (Message_Level* theLevel)
{
  for (int aLevelIndex = myAlertLevels.Size(); aLevelIndex >= 1; aLevelIndex--)
  {
    Message_Level* aLevel = myAlertLevels.Value (aLevelIndex);
    myAlertLevels.Remove (aLevelIndex);

    if (aLevel == theLevel)
      return;
  }
}

//=======================================================================
//function : AddAlert
//purpose  :
//=======================================================================

void Message_Report::AddAlert (Message_Gravity theGravity, const Handle(Message_Alert)& theAlert)
{
  Standard_Mutex::Sentry aSentry (myMutex);

   // alerts of the top level
  if (myAlertLevels.IsEmpty())
  {
    Handle (Message_CompositeAlerts) aCompositeAlert = compositeAlerts (Standard_True);
    if (aCompositeAlert->AddAlert (theGravity, theAlert))
      return;

    // remove alerts under the report only
    const Message_ListOfAlert& anAlerts = aCompositeAlert->GetAlerts (theGravity);
    if (anAlerts.Extent() > myLimit)
    {
      aCompositeAlert->RemoveAlert (theGravity, anAlerts.First());
    }
    return;
  }

  // if there are some levels of alerts
  // iterate by already recorded alerts and try to merge new one with one of those
  Message_Level* aLevel = myAlertLevels.Last();
  if (!aLevel)
    return;

  // alert is placed below the level
  if (!aLevel->RootAlert().IsNull())
  {
    aLevel->AddAlert (theGravity, theAlert);
    return;
  }

  Handle(Message_AlertExtended) anAlert = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anAlert.IsNull())
    return;
  // place new alert as a root of the level, after place the level alert below the report or
  // below the previous level
  aLevel->SetRootAlert (anAlert);

  if (myAlertLevels.Size() == 1)
    compositeAlerts (Standard_True)->AddAlert (theGravity, theAlert);
  else
  {
    Message_Level* aPrevLevel = myAlertLevels.Value (myAlertLevels.Size() - 1); // previous level
    aPrevLevel->AddLevelAlert (theGravity, theAlert);
  }
}

//=======================================================================
//function : GetAlerts
//purpose  :
//=======================================================================

const Message_ListOfAlert& Message_Report::GetAlerts (Message_Gravity theGravity) const
{
  static const Message_ListOfAlert anEmptyList;
  if (myCompositAlerts.IsNull())
    return anEmptyList;

  return myCompositAlerts->GetAlerts (theGravity);
}

//=======================================================================
//function : SetActiveMetric
//purpose  :
//=======================================================================

void Message_Report::SetActiveMetric (const Message_MetricType theMetricType,
                                      const Standard_Boolean theActivate)
{
  if (theActivate == myActiveMetrics.Contains (theMetricType))
    return;

  if (theActivate)
    myActiveMetrics.Add (theMetricType);
  else
    myActiveMetrics.Remove (theMetricType);
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
  //Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
  //                        "Requesting alerts for gravity not in valid range", Standard_False);
  //for (Message_ListOfAlert::Iterator anIt (myAlerts[theGravity]); anIt.More(); anIt.Next())
  //{
  //  if (anIt.Value()->IsInstance(theType))
  //    return Standard_True;
  //}
  if (compositeAlerts().IsNull())
    return Standard_False;

  return compositeAlerts()->HasAlert (theType, theGravity);
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Message_Report::Clear()
{
  if (compositeAlerts().IsNull())
    return;

  compositeAlerts()->Clear();
  myAlertLevels.Clear();
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Message_Report::Clear (Message_Gravity theGravity)
{
  if (compositeAlerts().IsNull())
    return;

  compositeAlerts()->Clear (theGravity);
  myAlertLevels.Clear();
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Message_Report::Clear (const Handle(Standard_Type)& theType)
{
  if (compositeAlerts().IsNull())
    return;

  compositeAlerts()->Clear (theType);
  myAlertLevels.Clear();

  //for (unsigned int i = 0; i < sizeof(myAlerts)/sizeof(myAlerts[0]); ++i)
  //{
  //  for (Message_ListOfAlert::Iterator anIt (myAlerts[i]); anIt.More(); )
  //  {
  //    if (anIt.Value().IsNull() || anIt.Value()->IsInstance (theType))
  //    {
  //      myAlerts[i].Remove (anIt);
  //    }
  //    else
  //    {
  //      anIt.More();
  //    }
  //  }
  //}
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
  if (compositeAlerts().IsNull())
    return;
  const Message_ListOfAlert& anAlerts = compositeAlerts()->GetAlerts (theGravity);

  // report each type of warning only once
  NCollection_Map<Handle(Standard_Type)> aPassedAlerts;
  for (Message_ListOfAlert::Iterator anIt (anAlerts); anIt.More(); anIt.Next())
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
  if (compositeAlerts().IsNull())
    return;
  const Message_ListOfAlert& anAlerts = compositeAlerts()->GetAlerts (theGravity);
  //Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
  //                        "Requesting alerts for gravity not in valid range", );

  // report each type of warning only once
  NCollection_Map<Handle(Standard_Type)> aPassedAlerts;
  for (Message_ListOfAlert::Iterator anIt (anAlerts); anIt.More(); anIt.Next())
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
//function : ñompositeAlerts
//purpose  : 
//=======================================================================
Handle (Message_CompositeAlerts) Message_Report::compositeAlerts (const Standard_Boolean isCreate)
{
  if (myCompositAlerts.IsNull() && isCreate)
    myCompositAlerts = new Message_CompositeAlerts();

  return myCompositAlerts;
}
