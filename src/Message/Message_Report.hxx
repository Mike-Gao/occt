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

#ifndef _Message_Report_HeaderFile
#define _Message_Report_HeaderFile

#include <Message_Gravity.hxx>
#include <Message_Level.hxx>
#include <Message_ListOfAlert.hxx>
#include <Message_MetricType.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Mutex.hxx>

class Message_CompositeAlerts;
class Message_Messenger;
class Message_Report;

DEFINE_STANDARD_HANDLE(Message_Report, Standard_Transient)

//! Container for alert messages, sorted according to their gravity.
//! 
//! For each gravity level, alerts are stored in simple list.
//! If alert being added can be merged with another alert of the same
//! type already in the list, it is merged and not added to the list.
//! 
//! This class is intended to be used as follows:
//!
//! - In the process of execution, algorithm fills report by alert objects
//!   using methods AddAlert()
//!
//! - The result can be queried for presence of particular alert using 
//!   methods HasAlert()
//!
//! - The reports produced by nested or sequentially executed algorithms
//!   can be collected in one using method Merge()
//!
//! - The report can be shown to the user either as plain text with method 
//!   Dump() or in more advanced way, by iterating over lists returned by GetAlerts()
//!
//! - Report can be cleared by methods Clear() (usually after reporting)

class Message_Report : public Standard_Transient
{
public:

  //! Empty constructor
  Standard_EXPORT Message_Report ();
  
  //! Returns true if report printer is registered in the messenger
  Standard_EXPORT Standard_Boolean IsActiveInMessenger() const;

  //! Add new level of alerts
  Standard_EXPORT void AddLevel (Message_Level* theLevel);

  //! Remove level of alerts
  Standard_EXPORT void RemoveLevel (Message_Level* theLevel);

  //! Add alert with specified gravity.
  //! This method is thread-safe, i.e. alerts can be added from parallel threads safely.
  Standard_EXPORT void AddAlert (Message_Gravity theGravity, const Handle(Message_Alert)& theAlert);

  //! Returns list of collected alerts with specified gravity
  Standard_EXPORT const Message_ListOfAlert& GetAlerts (Message_Gravity theGravity) const;

  //! Returns true if specific type of alert is recorded
  Standard_EXPORT Standard_Boolean HasAlert (const Handle(Standard_Type)& theType);

  //! Returns true if specific type of alert is recorded with specified gravity
  Standard_EXPORT Standard_Boolean HasAlert (const Handle(Standard_Type)& theType, Message_Gravity theGravity);

  //! Clears all collected alerts
  Standard_EXPORT void Clear ();

  //! Clears collected alerts with specified gravity
  Standard_EXPORT void Clear (Message_Gravity theGravity);

  //! Clears collected alerts with specified type
  Standard_EXPORT void Clear (const Handle(Standard_Type)& theType);

  //! Dumps all collected alerts to stream
  Standard_EXPORT void Dump (Standard_OStream& theOS);

  //! Dumps collected alerts with specified gravity to stream
  Standard_EXPORT void Dump (Standard_OStream& theOS, Message_Gravity theGravity);

  //! Sends all collected alerts to messenger
  Standard_EXPORT void SendMessages (const Handle(Message_Messenger)& theMessenger);

  //! Dumps collected alerts with specified gravity to messenger
  Standard_EXPORT void SendMessages (const Handle(Message_Messenger)& theMessenger, Message_Gravity theGravity);

  //! Merges data from theOther report into this
  Standard_EXPORT void Merge (const Handle(Message_Report)& theOther);

  //! Merges alerts with specified gravity from theOther report into this
  Standard_EXPORT void Merge (const Handle(Message_Report)& theOther, Message_Gravity theGravity);

  //! Sets metrics to compute when alerts are performed
  //! \param theMetrics container of metrics
  Standard_EXPORT void SetActiveMetric (const Message_MetricType theMetricType, const Standard_Boolean theActivate);

  //! Returns computed metrics when alerts are performed
  const NCollection_Map<Message_MetricType>& ActiveMetrics() const { return myActiveMetrics; }

  //! Removes all activated metrics
  void ClearMetrics() { myActiveMetrics.Clear(); }

  //! Returns maximum number of collecting alerts. If the limit is achieved,
  //! adding of a new alert after removing the first cached alert.
  //! \return the limit value
  Standard_EXPORT Standard_Integer GetLimit() const { return myLimit; }

  //! Sets maximum number of collecting alerts.
  //! \return theLimit limit value
  Standard_EXPORT void SetLimit(const Standard_Integer theLimit) { myLimit = theLimit; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, const Standard_Integer theDepth = -1) const;

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_Report,Standard_Transient)

protected:
  //! Returns class provided hierarchy of alerts if created or create if the parameter is true
  //! \param isCreate if composite alert has not been created for this alert, it should be created
  //! \return instance or NULL
  Standard_EXPORT Handle (Message_CompositeAlerts) compositeAlerts (const Standard_Boolean isCreate = Standard_False);

protected:
  Standard_Mutex myMutex;

  // store messages in a lists sorted by gravity;
  // here we rely on knowledge that Message_Fail is the last element of the enum
  Handle(Message_CompositeAlerts) myCompositAlerts;

  Standard_Integer myLimit; //! Maximum number of collected alerts

  NCollection_Sequence <Message_Level*> myAlertLevels; //! container of alerts, new alerts are added below the latest level
  NCollection_Map<Message_MetricType> myActiveMetrics; //! metrics to compute on alerts
};

#endif // _Message_Report_HeaderFile
