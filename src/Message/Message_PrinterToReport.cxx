// Created on: 2001-01-06
// Created by: OCC Team
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <Message_PrinterToReport.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_Attribute.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_Report.hxx>

#include <Standard_Dump.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_PrinterToReport, Message_Printer)

//=======================================================================
//function : Report
//purpose  : 
//=======================================================================
const Handle(Message_Report)& Message_PrinterToReport::Report() const
{
  if (!myReport.IsNull())
    return myReport;

  return Message::DefaultReport (Standard_True);
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================
void Message_PrinterToReport::Send (const TCollection_ExtendedString& theString,
                                    const Message_Gravity theGravity,
                                    const Standard_Boolean putEndl) const
{
  TCollection_AsciiString aString (theString/*, myUseUtf8 ? Standard_Character(0) : '?'*/);

  if (putEndl)
  {
    if (myValue.IsEmpty())
      return;

    const Handle(Message_Report)& aReport = Report();
    if (!aReport->ActiveMetrics().IsEmpty())
    {
      sendMetricAlert (myValue, theGravity);
      return;
    }
    Message_AlertExtended::AddAlert (aReport, new Message_Attribute (myValue), theGravity);
    ((Message_PrinterToReport*)this)->Clear();
    return;
  }
  ((Message_PrinterToReport*)this)->myValue += aString;
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================
void Message_PrinterToReport::Send (const Standard_SStream& theStream,
                                    const Message_Gravity theGravity,
                                    const Standard_Boolean /*putEndl*/) const
{
  const Handle(Message_Report)& aReport = Report();
  if (!aReport->ActiveMetrics().IsEmpty())
  {
    sendMetricAlert (myValue, theGravity);
    return;
  }
  Message_AlertExtended::AddAlert (aReport, new Message_AttributeStream (theStream, myValue), theGravity);
  ((Message_PrinterToReport*)this)->Clear();

  return;
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================
void Message_PrinterToReport::Send (const Handle(Standard_Transient)& theObject,
                                    const Message_Gravity theGravity,
                                    const Standard_Boolean /*putEndl*/) const
{
  const Handle(Message_Report)& aReport = Report();
  if (!aReport->ActiveMetrics().IsEmpty())
  {
    sendMetricAlert (myValue, theGravity);
    return;
  }

  Message_AlertExtended::AddAlert (aReport, new Message_AttributeObject (theObject, myValue), theGravity);
  ((Message_PrinterToReport*)this)->Clear();
}

//=======================================================================
//function : sendMetricAlert
//purpose  : 
//=======================================================================
void Message_PrinterToReport::sendMetricAlert (const TCollection_AsciiString theValue,
                                               const Message_Gravity theGravity) const
{
  Message_AlertExtended::AddAlert (Report(), new Message_AttributeMeter (theValue), theGravity);
  ((Message_PrinterToReport*)this)->Clear();
}
