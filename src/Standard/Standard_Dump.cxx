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

#include <Standard_Dump.hxx>

// =======================================================================
// function : Sentry constructor
// purpose :
// =======================================================================
Standard_Dump::Sentry::Sentry (Standard_OStream& theOStream,
                               const TCollection_AsciiString& theClassName,
                               const void* thePointer)
: myOStream (&theOStream), myClassName (theClassName)
{
  (*myOStream) << startKey (myClassName);
  if (thePointer)
  {
    TCollection_AsciiString Pointer = Standard_Dump::GetPointerInfo (thePointer);
    DUMP_FIELD_VALUES(theOStream, Pointer)
  }
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString Standard_Dump::GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                       const bool isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  return GetPointerInfo (thePointer.get(), isShortInfo);
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString Standard_Dump::GetPointerInfo (const void* thePointer, const bool isShortInfo)
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
      anInfoPtr = anInfoPtr.SubString (aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend (GetPointerPrefix());
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
void Standard_Dump::Split (const Standard_SStream& theStream,
                           NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues,
                           TCollection_AsciiString& theStreamKey)
{
  TCollection_AsciiString aStreamStr (theStream.str().c_str());

  split (aStreamStr, theValues, theStreamKey);
}

// ----------------------------------------------------------------------------
// split
// ----------------------------------------------------------------------------
void Standard_Dump::split (const TCollection_AsciiString& theStreamStr,
                           NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues,
                           TCollection_AsciiString& theStreamKey)
{
  TCollection_AsciiString aStreamStr = theStreamStr;

  TCollection_AsciiString aSplitValue, aTailValue, aKey;
  if (splitDumped (aStreamStr, aSplitValue, aTailValue, aKey))
  {
    if (aTailValue.IsEmpty())
    {
      theStreamKey = aKey;
      splitKey (aSplitValue, aStreamStr, aKey);
    }
  }

  NCollection_List<TCollection_AsciiString> aValues;
  splitValue (aStreamStr, DumpSeparator(), aValues);

  for (NCollection_List<TCollection_AsciiString>::Iterator anIterator (aValues); anIterator.More(); anIterator.Next())
  {
    TCollection_AsciiString aKey = anIterator.Value();
    anIterator.Next();
    if (!anIterator.More())
      break;
    theValues.Add (aKey, anIterator.Value());
  }
}

// ----------------------------------------------------------------------------
// splitValue
// ----------------------------------------------------------------------------
void Standard_Dump::splitValue (const TCollection_AsciiString& theValue,
                                const TCollection_AsciiString& theSeparator,
                                NCollection_List<TCollection_AsciiString>& theValues)
{
  TCollection_AsciiString aCurrentString = theValue;

  TCollection_AsciiString aSplitValue, aTailValue, aKey;
  while (!aCurrentString.IsEmpty())
  {
    TCollection_AsciiString aValueString = aCurrentString;
    if (splitDumped (aValueString, aSplitValue, aTailValue, aKey))
    {
      aValueString = aSplitValue;
      aCurrentString = aTailValue;
    }
    else
    {
      Standard_Integer aPosition = aValueString.Search (theSeparator);
      if (aPosition < 0 )
        break;
      aCurrentString = aValueString.Split (aPosition - 1);
    }
    theValues.Append (aValueString);
    if (aCurrentString.IsEmpty())
      break;

    aCurrentString = aCurrentString.Split (theSeparator.Length());
  }
}

// ----------------------------------------------------------------------------
// DumpFieldToName
// ----------------------------------------------------------------------------
void Standard_Dump::DumpFieldToName (const char* theField, const char*& theName)
{
  theName = theField;

  if (theName[0] == '&')
  {
    theName = theName + 1;
  }
  if (::LowerCase (theName[0]) == 'm' && theName[1] == 'y')
  {
    theName = theName + 2;
  }
}

// ----------------------------------------------------------------------------
// ToString
// ----------------------------------------------------------------------------
TCollection_AsciiString Standard_Dump::ToDumpString (const Standard_SStream& theStream)
{
  return TCollection_AsciiString (theStream.str().c_str());
}

// ----------------------------------------------------------------------------
// ConvertDumpToText
// ----------------------------------------------------------------------------
TCollection_AsciiString Standard_Dump::ConvertDumpToText (const Standard_SStream& theStream)
{
  TCollection_AsciiString aText;

  Standard_Integer aLevel = 0;
  convertDumpToText (ToDumpString (theStream), aLevel, aText);

  return aText;
}

TCollection_AsciiString getLevelIndent (const int theLevel)
{
  TCollection_AsciiString aLevelIndent;
  for (int i = 0; i < theLevel; i++)
    aLevelIndent += " ";
  return aLevelIndent;
}

// ----------------------------------------------------------------------------
// convertDumpToText
// ----------------------------------------------------------------------------
void Standard_Dump::convertDumpToText (const TCollection_AsciiString& theStreamStr,
                                       const Standard_Integer theLevel,
                                       TCollection_AsciiString& theText)
{

  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aSplitValues;
  TCollection_AsciiString aStreamKey;
  split (theStreamStr, aSplitValues, aStreamKey);
  Standard_Integer aLevel = theLevel;
  //if (!aStreamKey.IsEmpty())
  //{
  //  theText += getLevelIndent (aLevel) + aStreamKey + "\n";
  //  aLevel++;
  //}

  for (Standard_Integer anIndex = 1; anIndex <= aSplitValues.Size(); anIndex++)
  {
    TCollection_AsciiString aValue = aSplitValues.FindFromIndex (anIndex);
    TCollection_AsciiString aKey = aSplitValues.FindKey (anIndex);

    if (Standard_Dump::HasBracketKey (aValue))
    {
      theText += getLevelIndent (aLevel + 1) + aKey + "\n";
      convertDumpToText (aValue, aLevel + 2, theText);
    }
    else
    {
      theText += getLevelIndent (aLevel + 1) + aKey + " = " + aValue + "\n";
    }
  }
}

// ----------------------------------------------------------------------------
// splitDumped
// ----------------------------------------------------------------------------
Standard_Boolean Standard_Dump::splitDumped (const TCollection_AsciiString& theSourceValue,
                                             TCollection_AsciiString& theSplitValue,
                                             TCollection_AsciiString& theTailValue,
                                             TCollection_AsciiString& theKey)
{
  Standard_Integer aBracketPosition = theSourceValue.Search (XMLBracketOpen());
  // the first symbol is bracket
  if (aBracketPosition != 1 || aBracketPosition >= theSourceValue.Length())
    return Standard_False;

  TCollection_AsciiString aValue = theSourceValue.SubString (aBracketPosition + 1, theSourceValue.Length());

  aBracketPosition = aValue.Search (XMLBracketClose());
  if (aBracketPosition <= 1 || aBracketPosition >= theSourceValue.Length())
    return Standard_False;

  theKey = aValue;

  TCollection_AsciiString aTailValue = theKey.Split (aBracketPosition - 1);
  aTailValue = aTailValue.SubString (2, aTailValue.Length()); // remove close bracket
  TCollection_AsciiString aStopKey = stopKey (theKey);

  aBracketPosition = theSourceValue.Search (aStopKey);

  Standard_Integer aStopKeyLastPosition = aBracketPosition + aStopKey.Length() - 1;

  if (aBracketPosition <= 1 || aStopKeyLastPosition  > theSourceValue.Length())
    return Standard_False;

  theSplitValue = theSourceValue;
  theTailValue = theSplitValue.Split (aStopKeyLastPosition);

  return Standard_True;
}

// ----------------------------------------------------------------------------
// splitKey
// ----------------------------------------------------------------------------
Standard_Boolean Standard_Dump::splitKey (const TCollection_AsciiString& theSourceValue,
                                          TCollection_AsciiString& theSplitValue,
                                          TCollection_AsciiString& theKey)
{
  Standard_Integer aBracketPosition = theSourceValue.Search (XMLBracketOpen());
  // the first symbol is bracket
  if (aBracketPosition != 1 || aBracketPosition >= theSourceValue.Length())
    return Standard_False;

  TCollection_AsciiString aValue = theSourceValue.SubString (aBracketPosition + 1, theSourceValue.Length());

  aBracketPosition = aValue.Search (XMLBracketClose());
  if (aBracketPosition <= 1 || aBracketPosition >= theSourceValue.Length())
    return Standard_False;

  theKey = aValue;

  TCollection_AsciiString aTailValue = theKey.Split (aBracketPosition - 1);
  aTailValue = aTailValue.SubString (2, aTailValue.Length()); // remove close bracket
  TCollection_AsciiString aStopKey = stopKey (theKey);

  aBracketPosition = aTailValue.Search (aStopKey);
  if (aBracketPosition <= 1 || aBracketPosition  >= aTailValue.Length())
    return Standard_False;

  theSplitValue = aTailValue;
  aTailValue = theSplitValue.Split (aBracketPosition - 1);

  return Standard_True;
}
