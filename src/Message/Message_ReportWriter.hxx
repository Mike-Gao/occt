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

#ifndef _Message_ReportWriter_HeaderFile
#define _Message_ReportWriter_HeaderFile

#include <Standard_Handle.hxx>
#include <Message_Gravity.hxx>
#include <TCollection_AsciiString.hxx>

class Message_Report;
class Message_ReportWriter;
DEFINE_STANDARD_HANDLE(Message_ReportWriter, Standard_Transient)

//! Base class to store/restore Message_Report content in XML document
class Message_ReportWriter : public Standard_Transient
{
public:
  //! Constructor
  Message_ReportWriter() {}

  //! Returns the name of the output file
  TCollection_AsciiString FileName() const { return myFileName; }

  //! Sets the name of the file for output report
  //! \param theFileName the name
  void SetFileName (const TCollection_AsciiString& theFileName) { myFileName = theFileName; }

  //! Returns the gravity of the output messages
  Message_Gravity Gravity() const { return myGravity; }

  //! Sets the gravity of the output messages
  void SetGravity (const Message_Gravity theGravity) { myGravity = theGravity; }

  //! Stores the report into a file. The file format is an XML document.
  //! \param theReport the source report
  //! \param theFileName a file name
  //! \return true if success
  virtual Standard_Boolean ExportReport (const Handle(Message_Report)& theReport) = 0;

  //! Restores the report from the file. The file format should be an XML document.
  //! \return theReport the report to get the file content
  //! \param theFileName a file name
  //! \return true if success
  virtual Standard_Boolean ImportReport (const Handle(Message_Report)& theReport) = 0;

  DEFINE_STANDARD_RTTI_INLINE(Message_ReportWriter, Standard_Transient)

protected:
  TCollection_AsciiString myFileName; //! file name
  Message_Gravity myGravity; //! gravity level of messages
};

#endif // _Message_ReportWriter_HeaderFile
