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

#include <Message_AlertNode.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeVectorOfReal.hxx>
#include <Message_AttributeVectorOfRealVec3.hxx>
#include <Message_Gravity.hxx>
#include <Message_Report.hxx>

static Handle(Message_Alert) OCCT_Message_Alert;

#define MESSAGE_INFO(Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertNode::AddAlert (Message_Report::CurrentReport(), \
        new Message_Attribute (Name, Description), PerfMeter, ParentAlert); \
    } \
  }

#define MESSAGE_INFO_OBJECT(Object, Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertNode::AddAlert (Message_Report::CurrentReport(), \
        new Message_AttributeObject (Object, Name, Description), PerfMeter, ParentAlert ); \
    } \
  }

#define MESSAGE_INFO_VECTOR_OF_REAL(Vector, ColumnCount, Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertNode::AddAlert (Message_Report::CurrentReport(), \
        new Message_AttributeVectorOfReal (Vector, ColumnCount, Name, Description), PerfMeter, ParentAlert ); \
    } \
  }

#define MESSAGE_INFO_VECTOR_OF_REAL_VEC3(Vector, ColumnCount, Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertNode::AddAlert (Message_Report::CurrentReport(), \
        new Message_AttributeVectorOfRealVec3 (Vector, ColumnCount, Name, Description), PerfMeter, ParentAlert); \
    } \
  }

#endif // _Message_Alerts_HeaderFile
