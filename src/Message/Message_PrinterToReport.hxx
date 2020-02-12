// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _Message_PrinterToReport_HeaderFile
#define _Message_PrinterToReport_HeaderFile

#include <Message_Printer.hxx>
#include <Standard_Address.hxx>
#include <Standard_OStream.hxx>
#include <TCollection_AsciiString.hxx>

class Message_Report;

class Message_PrinterToReport;
DEFINE_STANDARD_HANDLE(Message_PrinterToReport, Message_Printer)

//! Implementation of a message printer associated with Message_Report
//! Send will create a new alert of the report. If string is sent, an alert is created by Eol only.
//! The alerts are sent into set report or default report of Message.
class Message_PrinterToReport : public Message_Printer
{
  DEFINE_STANDARD_RTTIEXT(Message_PrinterToReport, Message_Printer)

public:
  //! Create printer for redirecting messages into report.
  Message_PrinterToReport() {}

  ~Message_PrinterToReport() {}

  //! Returns the current or default report
  Standard_EXPORT const Handle(Message_Report)& Report() const;

  //! Sets the printer report
  //! @param theReport report for messages processing, if NULL, the default report is used
  Standard_EXPORT void SetReport (const Handle(Message_Report)& theReport) { myReport = theReport; }

  //!< Returns the first sent value
  const TCollection_AsciiString& Value() { return myValue; }

  //! Clears current values
  Standard_EXPORT void Clear() { myValue.Clear(); }

  //! Appends a new alert into message report if Endl is false or send a new extended alert with attribute on value
  //! Use Message_EndLine to finalize the current alert
  Standard_EXPORT virtual void Send (const TCollection_ExtendedString& theString,
                                     const Message_Gravity theGravity,
                                     const Standard_Boolean putEndl = Standard_True) const Standard_OVERRIDE;

  //! Send a string message with specified trace level.
  //! Stream is converted to string value.
  //! The parameter theToPutEol specified whether end-of-line should be added to the end of the message.
  //! Default implementation calls first method Send().
  Standard_EXPORT virtual void Send (const Standard_SStream& theStream,
                                     const Message_Gravity theGravity,
                                     const Standard_Boolean theToPutEol) const Standard_OVERRIDE;

  //! Send a string message with specified trace level.
  //! The object is converted to string in format: <object kind> : <object pointer>.
  //! The parameter theToPutEol specified whether end-of-line should be added to the end of the message.
  //! Default implementation calls first method Send().
  Standard_EXPORT virtual void Send (const Handle(Standard_Transient)& theObject,
                                     const Message_Gravity theGravity,
                                     const Standard_Boolean theToPutEol) const Standard_OVERRIDE;
protected:

  //! Send an alert with metrics active in the current report
  Standard_EXPORT void sendMetricAlert (const TCollection_AsciiString theValue,
                                        const Message_Gravity theGravity) const;

private:
  Handle(Message_Report) myReport; //!< the report for sending alerts
  TCollection_AsciiString myValue; //!< the union of sent strings until Eol
};

#endif // _Message_PrinterToReport_HeaderFile
