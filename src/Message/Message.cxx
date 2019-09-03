// Created on: 1999-11-23
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
#include <string.h>
//=======================================================================
//function : DefaultMessenger
//purpose  : 
//=======================================================================
const Handle(Message_Messenger)& Message::DefaultMessenger ()
{
  static Handle(Message_Messenger) aMessenger = new Message_Messenger;
  return aMessenger;
}

//=======================================================================
//function : FillTime
//purpose  : 
//=======================================================================

TCollection_AsciiString Message::FillTime (const Standard_Integer hour,
					       const Standard_Integer minute,
					       const Standard_Real second)
{
  char t [30];
  if (hour > 0)
    Sprintf (t, "%02dh:%02dm:%.2fs", hour, minute, second);
  else if (minute > 0)
    Sprintf (t, "%02dm:%.2fs", minute, second);
  else
    Sprintf (t, "%.2fs", second);
  return TCollection_AsciiString (t);
}

namespace
{
  static Standard_CString Message_Table_PrintGravityEnum[5] =
  {
    "TRACE", "INFO", "WARNING", "ALARM", "FAIL"
  };

  static Standard_CString Message_Table_PrintPerfMeterModeEnum[10] =
  {
    "NONE", "USER_TIME_CPU", "SYSTEM_TIME_CPU", "MEM_PRIVATE", "MEM_VIRTUAL",
    "MEM_WORKING_SET", "MEM_WORKING_SET_PEAK", "MEM_SWAP_USAGE", "MEM_SWAP_USAGE_PEAK", "MEM_HEAP_USAGE"
  };
}

//=======================================================================
//function : GravityToString
//purpose  :
//=======================================================================

Standard_CString Message::GravityToString (const Message_Gravity theGravity)
{
  return Message_Table_PrintGravityEnum[theGravity];
}

//=======================================================================
//function : GravityFromString
//purpose  :
//=======================================================================

Standard_Boolean Message::GravityFromString (const Standard_CString theGravityString,
                                             Message_Gravity& theGravity)
{
  TCollection_AsciiString aName (theGravityString);
  aName.UpperCase();
  for (Standard_Integer aGravityIter = 0; aGravityIter <= Message_Fail; ++aGravityIter)
  {
    Standard_CString aGravityName = Message_Table_PrintGravityEnum[aGravityIter];
    if (aName == aGravityName)
    {
      theGravity = Message_Gravity (aGravityIter);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : PerfMeterModeToString
//purpose  :
//=======================================================================

Standard_CString Message::PerfMeterModeToString (const Message_PerfMeterMode theValue)
{
  return Message_Table_PrintPerfMeterModeEnum[theValue];
}

//=======================================================================
//function : PerfMeterModeFromString
//purpose  :
//=======================================================================

Standard_Boolean Message::PerfMeterModeFromString (const Standard_CString theString,
                                                   Message_PerfMeterMode& theValue)
{
  TCollection_AsciiString aName (theString);
  aName.UpperCase();
  for (Standard_Integer aModeIter = 0; aModeIter <= Message_PerfMeterMode_MemHeapUsage; ++aModeIter)
  {
    Standard_CString aModeName = Message_Table_PrintPerfMeterModeEnum[aModeIter];
    if (aName == aModeName)
    {
      theValue = Message_PerfMeterMode (aModeIter);
      return Standard_True;
    }
  }
  return Standard_False;
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString Message::TransientToString (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  return PointerToString(thePointer.operator->(), isShortInfo);
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString Message::PointerToString (const void* thePointer, const bool isShortInfo)
{
  std::ostringstream aPtrStr;
  aPtrStr << thePointer;
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr (aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString(aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend("0x");
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// function : StrVectorToString
// purpose :
// =======================================================================
TCollection_AsciiString Message::StrVectorToString
    (const NCollection_Vector<TCollection_AsciiString>& theValues)
{
  TCollection_AsciiString aValue;
  for (NCollection_Vector<TCollection_AsciiString>::Iterator aValuesIt (theValues); aValuesIt.More(); aValuesIt.Next())
  {
    aValue += aValuesIt.Value();
    if (aValuesIt.More())
      aValue += VectorSeparator();
  }
  return aValue;
}

// =======================================================================
// function : StrVectorFromString
// purpose :
// =======================================================================
Standard_Boolean Message::StrVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<TCollection_AsciiString>& theValues)
{
  TCollection_AsciiString aCurrentString = theValue, aValueString;

  while (!aCurrentString.IsEmpty())
  {
    Standard_Integer aPosition = aCurrentString.Search (", ");
    aValueString = aCurrentString;
    if (aPosition > 0)
      aCurrentString = aValueString.Split (aPosition - 1);
    theValues.Append (aValueString.RealValue());
    if (aPosition > 0)
      aCurrentString = aCurrentString.Split (2);
  }
  return Standard_True;
}

// =======================================================================
// function : RealVectorToString
// purpose :
// =======================================================================
TCollection_AsciiString Message::RealVectorToString
    (const NCollection_Vector<Standard_Real>& theValues)
{
  TCollection_AsciiString aValue = ("(");

  for (NCollection_Vector<Standard_Real>::Iterator aValuesIt (theValues); aValuesIt.More(); aValuesIt.Next())
  {
    aValue += aValuesIt.Value();
    if (aValuesIt.More())
      aValue += VectorSeparator();
  }
  aValue += ")";

  return aValue;
}

// =======================================================================
// function : RealVectorFromString
// purpose :
// =======================================================================
Standard_Boolean Message::RealVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<Standard_Real>& theValues)
{
  TCollection_AsciiString aCurrentString = theValue, aValueString;

  Standard_Integer aPosition = aCurrentString.Search ("(");
  if (aPosition != 1)
    return Standard_False;
  aCurrentString = aCurrentString.Split (aPosition);

  aPosition = aCurrentString.Search (")");
  if (aPosition != 1)
    return Standard_False;
  aValueString = aCurrentString.Split (aPosition);


  while (!aCurrentString.IsEmpty())
  {
    // x value
    aPosition = aCurrentString.Search (", ");
    aValueString = aCurrentString;
    if (aPosition > 0)
      aCurrentString = aValueString.Split (aPosition - 1);
    theValues.Append (aValueString.RealValue());
    if (aPosition > 0)
      aCurrentString = aCurrentString.Split (2);
  }
  return Standard_True;
}

// =======================================================================
// function : CoordVectorToString
// purpose :
// =======================================================================
TCollection_AsciiString Message::CoordVectorToString
    (const NCollection_Vector<Standard_Real>& theValues)
{
  TCollection_AsciiString aValue = ("(");
  aValue += RealVectorToString (theValues);
  aValue += ")";

  return aValue;
}

// =======================================================================
// function : CoordVectorFromString
// purpose :
// =======================================================================
Standard_Boolean Message::CoordVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<Standard_Real>& theValues)
{
  TCollection_AsciiString aCurrentString = theValue, aValueString;

  Standard_Integer aPosition = aCurrentString.Search ("(");
  if (aPosition != 1)
    return Standard_False;
  aCurrentString = aCurrentString.Split (aPosition);

  aPosition = aCurrentString.Search (")");
  if (aPosition != 1)
    return Standard_False;
  aValueString = aCurrentString.Split (aPosition);

  return RealVectorFromString (aCurrentString, theValues); 
}

// =======================================================================
// function : ColorVectorToString
// purpose :
// =======================================================================
TCollection_AsciiString Message::ColorVectorToString
    (const NCollection_Vector<Standard_Real>& theValues)
{
  TCollection_AsciiString aValue = ("[");
  aValue += RealVectorToString (theValues);
  aValue += "]";

  return aValue;
}

// =======================================================================
// function : ColorVectorFromString
// purpose :
// =======================================================================
Standard_Boolean Message::ColorVectorFromString
    (const TCollection_AsciiString& theValue,
     NCollection_Vector<Standard_Real>& theValues)
{
  TCollection_AsciiString aCurrentString = theValue, aValueString;

  Standard_Integer aPosition = aCurrentString.Search ("[");
  if (aPosition != 1)
    return Standard_False;
  aCurrentString = aCurrentString.Split (aPosition);

  aPosition = aCurrentString.Search ("]");
  if (aPosition != 1)
    return Standard_False;
  aValueString = aCurrentString.Split (aPosition);

  return RealVectorFromString (aCurrentString, theValues); 
}

// =======================================================================
// function : ConvertStream
// purpose :
// =======================================================================
void Message::ConvertStream (const Standard_SStream& theStream,
  Standard_Integer& theColumnCount,
  NCollection_Vector<TCollection_AsciiString>& theValues)
{
  TCollection_AsciiString aStream (theStream.str().c_str());
  Standard_Character aSeparator = Message::DumpSeparator();
  Standard_Integer aColumnCount = 0;

  TCollection_AsciiString aCurrentString = aStream;
  Standard_Integer aPosition = aCurrentString.Search (aSeparator);
  if (aPosition >= 1)
  {
    TCollection_AsciiString aTailString = aCurrentString.Split (aPosition);
    Standard_Boolean aClassNameFound = Standard_False;
    while (!aCurrentString.IsEmpty())
    {
      TCollection_AsciiString aValueString = aCurrentString;
      aPosition = aValueString.Search (aSeparator);
      if (aPosition < 0 )
        break;
      aCurrentString = aValueString.Split (aPosition - 1);

      if (!aColumnCount)
      {
        if (!aClassNameFound)
          aClassNameFound = Standard_True;
        else
        {
          if (!aValueString.IsIntegerValue())
            break; // not correct Dump, in correct the first value is number of property columns
          aColumnCount = aValueString.IntegerValue();
        }
      }
      else
        theValues.Append (aValueString);

      if (aTailString.IsEmpty())
        break;
      aCurrentString = aTailString;
      aPosition = aCurrentString.Search (aSeparator);
      if (aPosition < 0 )
      {
        aCurrentString = aTailString;
        aTailString = TCollection_AsciiString();
      }
      else
        aTailString = aCurrentString.Split (aPosition);
    }
  }
  theColumnCount = aColumnCount;
}
