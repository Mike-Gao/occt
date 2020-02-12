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


#include <Draw.hxx>
#include <Draw_Printer.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <Message_PrinterToReport.hxx>
#include <Message_Report.hxx>
#include <Standard_Dump.hxx>

//==============================================================================
//function : printerType
//purpose  :
//==============================================================================
Standard_Boolean printerType (const TCollection_AsciiString& theTypeName,
                              Handle(Standard_Type)& theType)
{
  if (theTypeName == "ostream")
  {
    theType = STANDARD_TYPE(Message_PrinterOStream);
  }
  else if (theTypeName == "systemlog")
  {
    theType = STANDARD_TYPE(Message_PrinterSystemLog);
  }
  else if (theTypeName == "report")
  {
    theType = STANDARD_TYPE(Message_PrinterToReport);
  }
  else if (theTypeName == "draw")
  {
    theType = STANDARD_TYPE(Draw_Printer);
  }
  else
    return Standard_False;

  return Standard_True;
}

//==============================================================================
//function : createPrinter
//purpose  :
//==============================================================================
Handle(Message_Printer) createPrinter (const Handle(Standard_Type)& theType, Draw_Interpretor& theDI)
{
  const Standard_CString aTypeName = theType->Name();
  if (aTypeName == STANDARD_TYPE(Message_PrinterOStream)->Name())
  {
    return new Message_PrinterOStream();
  }
  else if (aTypeName == STANDARD_TYPE(Message_PrinterSystemLog)->Name())
  {
    return new Message_PrinterSystemLog ("draw_messages");
  }
  else if (aTypeName == STANDARD_TYPE(Message_PrinterToReport)->Name())
  {
    Handle(Message_PrinterToReport) aMessagePrinter = new Message_PrinterToReport();
    const Handle(Message_Report)& aReport = Message::DefaultReport (Standard_True);
    aMessagePrinter->SetReport (aReport);
    return aMessagePrinter;
  }
  else if (aTypeName == STANDARD_TYPE(Draw_Printer)->Name())
  {
    return new Draw_Printer (theDI);
  }
  return Handle(Message_Printer)();
}

//==============================================================================
//function : SendMessage
//purpose  :
//==============================================================================
static Standard_Integer SendMessage (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 2)
  {
    std::cout << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();
  Handle(Standard_Type) aPrinterType;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();

    aMessenger->Send (anArg);
  }

  return 0;
}

//==============================================================================
//function : DumpMessenger
//purpose  :
//==============================================================================
static Standard_Integer DumpMessenger (Draw_Interpretor& theDI, Standard_Integer, const char**)
{
  Standard_SStream aStream;
  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();
  aMessenger->DumpJson (aStream);

  theDI << "Message_Messenger\n";
  theDI << Standard_Dump::FormatJson (aStream);

  return 0;
}

//==============================================================================
//function : SetMessagePrinter
//purpose  :
//==============================================================================
static Standard_Integer SetMessagePrinter (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 2)
  {
    std::cout << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();
  Handle(Standard_Type) aPrinterType;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-clear")
    {
      aMessenger->ChangePrinters().Clear();
    }
    else if (anArg == "-type"
          && anArgIter + 1 < theArgNb)
    {
      TCollection_AsciiString aVal (theArgVec[++anArgIter]);
      if (!printerType (aVal, aPrinterType))
      {
        std::cout << "Syntax error: unknown printer type '" << aVal << "'\n";
        return 1;
      }
      if (aMessenger->HasPrinter (aPrinterType))
        continue;

      aMessenger->AddPrinter (createPrinter (aPrinterType, theDI));
    }
    else
    {
      std::cout << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  return 0;
}

//==============================================================================
//function : SetReportMetric
//purpose  :
//==============================================================================
static Standard_Integer SetReportMetric(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 1)
  {
    std::cout << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  const Handle(Message_Report)& aReport = Message::DefaultReport (Standard_True);
  if (aReport.IsNull())
    return 1;

  aReport->ClearMetrics();
  for (int i = 1; i < theArgNb; i++)
  {
    Standard_Integer aMetricId = Draw::Atoi (theArgVec[i]);
    if (aMetricId < Message_MetricType_UserTimeCPU || aMetricId > Message_MetricType_MemHeapUsage)
    {
      std::cout << "Unrecognized message metric: " << aMetricId << std::endl;
      return 1;
    }
    aReport->SetActiveMetric ((Message_MetricType)aMetricId, Standard_True);
  }
  return 0;
}

//==============================================================================
//function : PrintReport
//purpose  :
//==============================================================================
static Standard_Integer PrintReport(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 1)
  {
    std::cout << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  const Handle(Message_Report)& aReport = Message::DefaultReport (Standard_True);
  if (aReport.IsNull())
    return 1;

  if (theArgNb > 1) // default messenger
  {
    aReport->SendMessages (Message::DefaultMessenger());
  }
  else // stream
  {
    Standard_SStream aSStream;
    aReport->Dump (aSStream);
    theDI << aSStream;
  }

  return 0;
}

void Draw::MessageCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean Done = Standard_False;
  if (Done) return;
  Done = Standard_True;

  std::ios::sync_with_stdio();

  const char* group = "DRAW Message Commands";

  theCommands.Add("SetMessagePrinter",
    "SetMessagePrinter [-type ostream|systemlog|report|draw] [-clear]"
    "\n\t\t: Sets printer into messenger or remove all printers."
    "\n    : Option -type set type of printer. Printers are applyed with And combination."
    "\n    : Option -clear remove all printers in messenger",
    __FILE__, SetMessagePrinter, group);

  theCommands.Add("DumpMessenger",
    "DumpMessenger"
    "\n Prints DumpJson information about Messenger.\n",
    __FILE__, DumpMessenger, group);

  theCommands.Add("SendMessage",
    "SendMessage text [text ...]"
    "\n Sends the text into the messenger.\n",
    __FILE__, SendMessage, group);

  theCommands.Add("SetReportMetric", "SetReportMetric [metric...] \n Activate report metrics, deactivate all if there are no parameters.\n",
    __FILE__, SetReportMetric, group);

  theCommands.Add("PrintReport", "PrintReport [messenger] \n Send report content to default messenger or stream (if parameter is absent).\n",
    __FILE__, PrintReport, group);
}
