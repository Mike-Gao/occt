// Created on: 1993-01-14
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <TCollection.hxx>

#include <Standard_OutOfRange.hxx>
#include <TCollection_AsciiString.hxx>

// The array of prime numbers used as consequtive steps for 
// size of array of buckets in the map.
// The prime numbers are used for array size with the hope that this will 
// lead to less probablility of having the same hash codes for
// different map items (note that all hash codes are modulo that size).
// The value of each next step is chosen to be ~2 times greater than previous.
// Though this could be thought as too much, actually the amount of 
// memory overhead in that case is only ~15% as compared with total size of
// all auxiliary data structures (each map node takes ~24 bytes), 
// and this proves to pay off in performance (see OCC13189).
#define THE_NB_PRIMES 24
static const Standard_Integer THE_TCollection_Primes[THE_NB_PRIMES] =
{
         101,
        1009,
        2003,
        5003,
       10007,
       20011,
       37003,
       57037,
       65003,
      100019,
      209953, // The following are Pierpont primes [List of prime numbers]
      472393,
      995329,
     2359297,
     4478977,
     9437185,
    17915905,
    35831809,
    71663617,
   150994945,
   301989889,
   573308929,
  1019215873,
  2038431745
};

// =======================================================================
// function : NextPrimeForMap
// purpose  :
// =======================================================================
Standard_Integer TCollection::NextPrimeForMap(const Standard_Integer N)
{
  for (Standard_Integer aPrimeIter = 0; aPrimeIter < THE_NB_PRIMES; ++aPrimeIter)
  {
    if (THE_TCollection_Primes[aPrimeIter] > N)
    {
      return THE_TCollection_Primes[aPrimeIter];
    }
  }
  throw Standard_OutOfRange ("TCollection::NextPrimeForMap() - requested too big size");
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================

TCollection_AsciiString TCollection::GetPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  return GetPointerInfo(thePointer.operator->(), isShortInfo);
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================

TCollection_AsciiString TCollection::GetPointerInfo (const void* thePointer, const bool isShortInfo)
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
      anInfoPtr.Prepend(GetPointerPrefix());
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// ----------------------------------------------------------------------------
// Join
// ----------------------------------------------------------------------------

TCollection_AsciiString TCollection::Join (const NCollection_List<TCollection_AsciiString>& theValues,
                                           const TCollection_AsciiString& theSeparator)
{
  TCollection_AsciiString aValue;
  for (NCollection_List<TCollection_AsciiString>::Iterator anIterator (theValues); anIterator.More(); anIterator.Next())
  {
    if (!aValue.IsEmpty())
      aValue += theSeparator;

    aValue += anIterator.Value();
  }

  return aValue;
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------

void TCollection::Split (const Standard_SStream& theStream,
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues)
{
  TCollection_AsciiString aStreamStr (theStream.str().c_str());

  NCollection_List<TCollection_AsciiString> aValues;
  Split (aStreamStr, DumpSeparator(), aValues);

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
// Split
// ----------------------------------------------------------------------------

void TCollection::Split (const TCollection_AsciiString& theValue,
                         const TCollection_AsciiString& theSeparator,
                         NCollection_List<TCollection_AsciiString>& theValues)
{
  TCollection_AsciiString aCurrentString = theValue;

  TCollection_AsciiString aSplitValue, aTailValue, aKey;
  while (!aCurrentString.IsEmpty())
  {
    TCollection_AsciiString aValueString = aCurrentString;
    if (SplitDumped (aValueString, aSplitValue, aTailValue, aKey))
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

  if (theValues.Size() == 1)
  {
    TCollection_AsciiString aKey, aValue;
    if (!SplitKey (theValues.First(), aValue, aKey))
      return;

    theValues.Clear();
    Split (aValue, DumpSeparator(), theValues);
  }
}

// ----------------------------------------------------------------------------
// SplitReal
// ----------------------------------------------------------------------------

Standard_Boolean TCollection::SplitReal (const TCollection_AsciiString& theValue,
                                         const TCollection_AsciiString& theSeparator,
                                         NCollection_Vector<Standard_Real>& theValues)
{
  TCollection_AsciiString aCurrentString = theValue;

  while (!aCurrentString.IsEmpty())
  {
    TCollection_AsciiString aValueString = aCurrentString;
    Standard_Integer aPosition = aValueString.Search (theSeparator);
    if (aPosition < 0 )
      break;

    aCurrentString = aValueString.Split (aPosition);
    if (!aValueString.IsRealValue())
      return Standard_False;

    theValues.Append (aValueString.RealValue());
    aCurrentString = aCurrentString.Split (theSeparator.Length() + 1);
  }

  return !theValues.IsEmpty();
}

// ----------------------------------------------------------------------------
// ToDumpString
// ----------------------------------------------------------------------------

TCollection_AsciiString TCollection::ToDumpString (const NCollection_List<TCollection_AsciiString>& theValues)
{
  return TCollection::Join (theValues, TCollection::VectorSeparator());
}

// ----------------------------------------------------------------------------
// ToString
// ----------------------------------------------------------------------------

TCollection_AsciiString TCollection::ToDumpString (const Standard_SStream& theStream)
{
  return TCollection_AsciiString (theStream.str().c_str());
}

// ----------------------------------------------------------------------------
// ConvertDumpToText
// ----------------------------------------------------------------------------

TCollection_AsciiString TCollection::ConvertDumpToText (const TCollection_AsciiString& theValue)
{
  TCollection_AsciiString aText = theValue;

  //TCollection_AsciiString aCurrentString = theValue;
  //while (!aCurrentString.IsEmpty())
  //{
  //  TCollection_AsciiString aValueString = aCurrentString;
  //  Standard_Integer aPosition = aValueString.Search (theSeparator);
  //  if (aPosition < 0 )
  //    break;

  //  aCurrentString = aValueString.Split (aPosition);
  //  theValues.Append (aValueString);

  //  aCurrentString = aCurrentString.Split (theSeparator.Length() + 1);
  //}

  //TCollection_AsciiString XMLBracketOpen() { return TCollection_AsciiString ("<"); }
  //TCollection_AsciiString XMLBracketClose() { return TCollection_AsciiString (">"); }
  //TCollection_AsciiString XMLFinishKey() { return TCollection_AsciiString ("\\"); }

  return aText;
}

// ----------------------------------------------------------------------------
// SplitDumped
// ----------------------------------------------------------------------------

Standard_Boolean TCollection::SplitDumped (const TCollection_AsciiString& theSourceValue,
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
  TCollection_AsciiString aStopKey = StopKey (theKey);

  aBracketPosition = theSourceValue.Search (aStopKey);

  Standard_Integer aStopKeyLastPosition = aBracketPosition + aStopKey.Length() - 1;

  //aBracketPosition = aTailValue.Search (aStopKey);
  if (aBracketPosition <= 1 || aStopKeyLastPosition  > theSourceValue.Length())
    return Standard_False;

  theSplitValue = theSourceValue;
  theTailValue = theSplitValue.Split (aStopKeyLastPosition);
  //theSplitValue = aTailValue;
  //aTailValue = theSplitValue.Split (aBracketPosition - 1);

  //if (aTailValue.Length() < aStopKey.Length())
  //  return Standard_False;

  //if (aTailValue.Length() == aStopKey.Length())
  //{
  //  theTailValue = TCollection_AsciiString();
  //  return Standard_True;
  //}

  //theTailValue = aTailValue.Split (aStopKey.Length());
  return Standard_True;
}

// ----------------------------------------------------------------------------
// SplitKey
// ----------------------------------------------------------------------------

Standard_Boolean TCollection::SplitKey (const TCollection_AsciiString& theSourceValue,
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
  TCollection_AsciiString aStopKey = StopKey (theKey);

  aBracketPosition = theSourceValue.Search (aStopKey);
  if (aBracketPosition <= 1 || aBracketPosition  >= theSourceValue.Length())
    return Standard_False;

  theSplitValue = aTailValue;
  aTailValue = theSplitValue.Split (aBracketPosition - 1);
  return Standard_True;
}
