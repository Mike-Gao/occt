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

#ifndef _Message_Alerts_HeaderFile
#define _Message_Alerts_HeaderFile

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeVectorOfValues.hxx>
#include <Message_Gravity.hxx>
#include <Message_Report.hxx>

static Handle(Message_Alert) OCCT_Message_Alert;

#define MESSAGE_INFO(Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertExtended::AddAlert (Message_Report::CurrentReport(), \
        new Message_Attribute (Name, Description), PerfMeter, ParentAlert); \
    } \
  }

#define MESSAGE_INFO_OBJECT(Object, Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertExtended::AddAlert (Message_Report::CurrentReport(), \
        new Message_AttributeObject (Object, Name, Description), PerfMeter, ParentAlert ); \
    } \
  }

#define MESSAGE_INFO_VALUES(StreamValues, Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertExtended::AddAlert (Message_Report::CurrentReport(), \
        new Message_AttributeVectorOfValues (StreamValues, Name, Description), PerfMeter, ParentAlert ); \
    } \
  }

#define DUMP_VALUE(OS, Value) \
  { \
    OS << Value << Message::DumpSeparator(); \
  }

#define DUMP_VALUES(OS, Value1, Value2) \
  { \
    OS << Value1 << Message::DumpSeparator() << Value2 << Message::DumpSeparator(); \
  }


#endif // _Message_Alerts_HeaderFile
