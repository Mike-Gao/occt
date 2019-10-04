// Created on: 1999-07-29
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Message_HeaderFile
#define _Message_HeaderFile

#include <Message_Gravity.hxx>
#include <Message_PerfMeterMode.hxx>
#include <NCollection_Vector.hxx>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TCollection_AsciiString.hxx>

class Message_Messenger;
class Message_Msg;
class Message_MsgFile;
class Message_Messenger;
class Message_Algorithm;
class Message_Printer;
class Message_PrinterOStream;
class Message_ProgressIndicator;
class Message_ProgressScale;
class Message_ProgressSentry;


//! Defines
//! - tools to work with messages
//! - basic tools intended for progress indication
class Message 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines default messenger for OCCT applications.
  //! This is global static instance of the messenger.
  //! By default, it contains single printer directed to std::cout.
  //! It can be customized according to the application needs.
  Standard_EXPORT static const Handle(Message_Messenger)& DefaultMessenger();
  
  //! Returns the string filled with values of hours, minutes and seconds.
  //! Example:
  //! 1. (5, 12, 26.3345) returns "05h:12m:26.33s",
  //! 2. (0,  6, 34.496 ) returns "06m:34.50s",
  //! 3. (0,  0,  4.5   ) returns "4.50s"
  Standard_EXPORT static TCollection_AsciiString FillTime (const Standard_Integer Hour, const Standard_Integer Minute, const Standard_Real Second);

  //! Returns the string name for a given gravity.
  //! @param Gravity gravity type
  //! @return string identifier from the list Message_Trace, Message_Info, Message_Warning, Message_Alarm and Message_Fail
  Standard_EXPORT static Standard_CString GravityToString (const Message_Gravity theGravity);

  //! Returns the gravity type from the given string identifier (using case-insensitive comparison).
  //! @param theGravityString string identifier
  //! @return gravity or Message_Trace if string identifier is invalid
  static Message_Gravity GravityFromString (const Standard_CString theGravityString)
  {
    Message_Gravity aGravity = Message_Trace;
    GravityFromString (theGravityString, aGravity);
    return aGravity;
  }

  //! Determines the gravity from the given string identifier (using case-insensitive comparison).
  //! @param theGravityString string identifier
  //! @param theGravity detected shape type
  //! @return TRUE if string identifier is known
  Standard_EXPORT static Standard_Boolean GravityFromString (const Standard_CString theGravityString,
                                                             Message_Gravity& theGravity);

  //! Returns the string name for a perf meter mode.
  //! @param theValue mode
  //! @return string identifier
  Standard_EXPORT static Standard_CString PerfMeterModeToString (const Message_PerfMeterMode theValue);

  //! Returns the enum value from the given string identifier (using case-insensitive comparison).
  //! @param theString string identifier
  //! @return enum or Message_PerfMeterMode_None if string identifier is invalid
  static Message_PerfMeterMode PerfMeterModeFromString (const Standard_CString theString)
  {
    Message_PerfMeterMode aValue = Message_PerfMeterMode_None;
    PerfMeterModeFromString (theString, aValue);
    return aValue;
  }

  //! Returns the enum value from the given string identifier (using case-insensitive comparison).
  //! @param theString string identifier
  //! @return enum or Message_PerfMeterMode_None if string identifier is invalid
  //! @return TRUE if string identifier is known
  Standard_EXPORT static Standard_Boolean PerfMeterModeFromString (const Standard_CString theString,
                                                                   Message_PerfMeterMode& theValue);


  //! Returns separator symbol of Dump information
  static Standard_Character DumpSeparator() { return '\\'; }

  //! Returns separator symbol of values vector union
  static TCollection_AsciiString VectorSeparator() { return " ,"; }

  //! Convert handle pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString TransientToString (const Handle(Standard_Transient)& thePointer,
                                                                    const bool isShortInfo = true);

  //! Convert pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString PointerToString (const void* thePointer,
                                                                  const bool isShortInfo = true);
  //! Convert vector of real values to string, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString StrVectorToString
    (const NCollection_Vector<TCollection_AsciiString>& theValues);

  //! Convert string to vector of real values, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static Standard_Boolean StrVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<TCollection_AsciiString>& theValues);

  //! Convert vector of real values to string, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString RealVectorToString
    (const NCollection_Vector<Standard_Real>& theValues);

  //! Convert string to vector of real values, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static Standard_Boolean RealVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<Standard_Real>& theValues);

  //! Convert vector of real values to string, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString CoordVectorToString
    (const NCollection_Vector<Standard_Real>& theValues);

  //! Convert string to vector of real values, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static Standard_Boolean CoordVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<Standard_Real>& theValues);

  //! Convert vector of real values to string, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString ColorVectorToString
    (const NCollection_Vector<Standard_Real>& theValues);

  //! Convert string to vector of real values, separator is vector separator
  //! \param thePointer a container of real values
  //! \return the string value
  Standard_EXPORT static Standard_Boolean ColorVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<Standard_Real>& theValues);

  //! Converts stream to vector of values and column count
  //! \param theStream stream value
  //! \param theColumnCount [out] number of columns
  //! \param theValues [out] container of split values
  static Standard_EXPORT void ConvertStream (const Standard_SStream& theStream,
    Standard_Integer& theColumnCount,
    NCollection_Vector<TCollection_AsciiString>& theValues);

protected:





private:




friend class Message_Msg;
friend class Message_MsgFile;
friend class Message_Messenger;
friend class Message_Algorithm;
friend class Message_Printer;
friend class Message_PrinterOStream;
friend class Message_ProgressIndicator;
friend class Message_ProgressScale;
friend class Message_ProgressSentry;

};







#endif // _Message_HeaderFile
