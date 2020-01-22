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

#ifndef _Message_Level_HeaderFile
#define _Message_Level_HeaderFile

#include <Message_AlertExtended.hxx>
#include <Message_Gravity.hxx>
#include <Message_MetricType.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard.hxx>

//! This class defines level of alerts in the current report.
//! The first added alert is the root alert, next alerts are children of the root alert.
class Message_Level
{

public:
  //! Constructs and starts (if autoStart is true) the named meter
  //! One string key is used for all alert meters. The perf meter is not started automatically, it will be done in
  //! AddAlert method
  Standard_EXPORT Message_Level();

  //! Assures stopping upon destruction
  Standard_EXPORT ~Message_Level();

  //! Returns message alert on the level, by default the upper alert is returned
  //! \param theLevelId a level of child position inside the upper alert of performance meter
  //! \return alert instance or NULL
  Handle(Message_AlertExtended) RootAlert() const { return myRootAlert; }

  //! Returns message alert on the level, by default the upper alert is returned
  //! \param theLevelId a level of child position inside the upper alert of performance meter
  //! \return alert instance or NULL
  Standard_EXPORT void SetRootAlert (const Handle(Message_AlertExtended)& theAlert);

  //! Processes the parameter alert. There are some variants:
  //! - current alert is NULL, the alert becomes the current one and perf meter is started
  //! - last alert of the current alert is stopped (perf meter becomes NULL, time is calculated),
  //! the parameter alert is started (perf meter becomes the current one)
  Standard_EXPORT Standard_Boolean AddAlert (const Message_Gravity theGravity,
                                             const Handle(Message_Alert)& theAlert);

  //! Add level alert, that is placed under the previous last alert if exists or under the root alert
  Standard_EXPORT Standard_Boolean AddLevelAlert (const Message_Gravity theGravity,
                                                  const Handle(Message_Alert)& theAlert);

  //! Remove the current level from the report
  Standard_EXPORT void Remove();

protected:
  //! Sets stop time into the alert
  //! \param theAlert a level of child position inside the upper alert of performance meter
  Standard_Boolean startAlert (const Handle(Message_AlertExtended)& theAlert);

  //! Sets start time into the alert
  //! \param theAlert a level of child position inside the upper alert of performance meter
  Standard_Boolean stopAlert (const Handle(Message_AlertExtended)& theAlert);

  //! Fills the alert with metric values
  //! \param theAlert a level of child position inside the upper alert of performance meter
  //! \param theStartValue flag whether the computed value is the start value, if false, stop value is filled
  //! \return true if the alert is filled
  Standard_Boolean setAlertMetrics (const Handle(Message_AlertExtended)& theAlert,
                                    const Standard_Boolean theStartValue);

protected:
  Handle(Message_AlertExtended) myRootAlert; //!< root alerts
  Handle(Message_AlertExtended) myLastAlert; //!< last added alert
};

//! @def MESSAGE_NEW_LEVEL
//! Creates a new level instance of Sentry. This row should be inserted before messages using in the method.
#define MESSAGE_ADD_LEVEL_SENTRY Message_Level aLevel;

//! @def MESSAGE_NEW_LEVEL
//! Removed the current level from report. It is not necessary to call it as the level will be removed at the method end.
//! This macro might be useful for creation several levels in one method to remove previous and add new one.
#define MESSAGE_REMOVE_LEVEL_SENTRY() aLevel.Remove();


#endif // _Message_Level_HeaderFile
