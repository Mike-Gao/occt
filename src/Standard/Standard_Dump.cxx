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

#include <stdarg.h>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
Standard_DumpSentry::Standard_DumpSentry (Standard_OStream& theOStream, const char* theClassName)
: myOStream (&theOStream)
{
  (*myOStream) << "\"" << theClassName << "\": {";
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================
Standard_DumpSentry::~Standard_DumpSentry()
{
  (*myOStream) << "}";
}

// =======================================================================
// function : EndsWith
// purpose :
// =======================================================================
Standard_Boolean Standard_Dump::EndsWith (const Standard_OStream& theOStream,
                                          const TCollection_AsciiString& theEndString)
{
  Standard_SStream aStream;
  aStream << theOStream.rdbuf();
  TCollection_AsciiString aStreamStr = Standard_Dump::Text (aStream);
  return aStreamStr.EndsWith (theEndString);
}

//=======================================================================
//function : DumpKeyToClass
//purpose  : 
//=======================================================================
void Standard_Dump::DumpKeyToClass (Standard_OStream& theOStream,
                                    const char* theKey,
                                    const TCollection_AsciiString& theField)
{
  if (!Standard_Dump::EndsWith (theOStream, "{"))
    theOStream << ", ";
  theOStream << "\"" << theKey << "\": {" << theField << "}";
}

//=======================================================================
//function : DumpCharacterValues
//purpose  : 
//=======================================================================
void Standard_Dump::DumpCharacterValues (Standard_OStream& theOStream, int theCount, ...)
{
  va_list  vl;
  va_start(vl, theCount);
  for(int i = 0; i < theCount; ++i)
  {
    if (i > 0)
      theOStream << ", ";
    theOStream << "\"" << va_arg(vl, char*) << "\"";
  }
  va_end(vl);
}

//=======================================================================
//function : DumpRealValues
//purpose  : 
//=======================================================================
void Standard_Dump::DumpRealValues (Standard_OStream& theOStream, int theCount, ...)
{
  va_list  vl;
  va_start(vl, theCount);
  for(int i = 0; i < theCount; ++i)
  {
    if (i > 0)
      theOStream << ", ";
    theOStream << va_arg(vl, Standard_Real);
  }
  va_end(vl);
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

// =======================================================================
// DumpFieldToName
// =======================================================================
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

// =======================================================================
// Text
// =======================================================================
TCollection_AsciiString Standard_Dump::Text (const Standard_SStream& theStream)
{
  return TCollection_AsciiString (theStream.str().c_str());
}

// =======================================================================
// FormatJson
// =======================================================================
TCollection_AsciiString Standard_Dump::FormatJson (const Standard_SStream& theStream,
                                                   const Standard_Integer theIndent)
{
  TCollection_AsciiString aStreamStr = Text (theStream);
  TCollection_AsciiString anIndentStr;
  for (Standard_Integer anIndentId = 0; anIndentId < theIndent; anIndentId++)
    anIndentStr.AssignCat (' ');

  TCollection_AsciiString aText;

  Standard_Integer anIndentCount = 0;
  Standard_Boolean isMassiveValues = Standard_False;
  for (Standard_Integer anIndex = 1; anIndex < aStreamStr.Length(); anIndex++)
  {
    Standard_Character aSymbol = aStreamStr.Value (anIndex);
    if (aSymbol == '{')
    {
      anIndentCount++;

      aText += aSymbol;
      aText += '\n';

      for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
        aText += anIndentStr;
    }
    else if (aSymbol == '}')
    {
      anIndentCount--;

      aText += '\n';
      for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
        aText += anIndentStr;
      aText += aSymbol;
    }
    else if (aSymbol == '[')
    {
      isMassiveValues = Standard_True;
      aText += aSymbol;
    }
    else if (aSymbol == ']')
    {
      isMassiveValues = Standard_False;
      aText += aSymbol;
    }
    else if (aSymbol == ',')
    {
      if (!isMassiveValues)
      {
        aText += aSymbol;
        aText += '\n';
        for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
          aText += anIndentStr;
        if (anIndex + 1 < aStreamStr.Length() && aStreamStr.Value (anIndex + 1) == ' ')
          anIndex++; // skip empty value after comma
      }
      else
        aText += aSymbol;
    }
    else
      aText += aSymbol;
  }
  return aText;
}

// ----------------------------------------------------------------------------
// SplitJson
// ----------------------------------------------------------------------------
Standard_Boolean Standard_Dump::SplitJson (const TCollection_AsciiString& theStreamStr,
                                           NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues)
{
  for (Standard_Integer anIndex = 1; anIndex < theStreamStr.Length(); anIndex++)
  {
    Standard_JsonKey aKey;
    if (!jsonKey (theStreamStr, anIndex, anIndex, aKey))
      return Standard_False;

    Standard_Boolean aProcessed = Standard_False;
    switch (aKey)
    {
      case Standard_JsonKey_Quote:
        aProcessed = splitKeyToValue (theStreamStr, anIndex, anIndex, theValues);
      break;
      case Standard_JsonKey_OpenChild:
      {
        Standard_Integer aStartIndex = anIndex;
        Standard_Integer aNextIndex = anIndex;

        Standard_Integer aClosePos = nextClosePosition (theStreamStr, aNextIndex, Standard_JsonKey_OpenChild, Standard_JsonKey_CloseChild);
        if (!aClosePos)
          return Standard_False;

        TCollection_AsciiString aSubStreamStr = theStreamStr.SubString (aStartIndex + 1, aNextIndex - 2);
        if (!SplitJson (aSubStreamStr, theValues))
          return Standard_False;

        anIndex = aClosePos + Standard_Integer (JsonKeyLength (Standard_JsonKey_CloseChild));
      }
      break;
      case Standard_JsonKey_SeparatorValueToValue:
        continue;
      default:
        break;
    }
    if (!aProcessed)
      return Standard_False;
  }
  return Standard_True;
}

// ----------------------------------------------------------------------------
// HierarchicalValueIndices
// ----------------------------------------------------------------------------
NCollection_List<Standard_Integer> Standard_Dump::HierarchicalValueIndices (
    const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues)
{
  NCollection_List<Standard_Integer> anIndices;

  for (Standard_Integer anIndex = 1; anIndex < theValues.Extent(); anIndex++)
  {
    if (HasChildKey (theValues.FindFromIndex (anIndex)))
      anIndices.Append (anIndex);
  }
  return anIndices;
}

// ----------------------------------------------------------------------------
// splitKeyToValue
// ----------------------------------------------------------------------------
Standard_Boolean Standard_Dump::splitKeyToValue (const TCollection_AsciiString& theStreamStr,
                                                 Standard_Integer theStartIndex,
                                                 Standard_Integer& theNextIndex,
                                                 NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues)
{
  // find key value: "key"
  Standard_Integer aStartIndex = theStartIndex;
  Standard_Integer aCloseIndex = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_None, Standard_JsonKey_Quote);
  if (!aCloseIndex)
    return Standard_False;

  TCollection_AsciiString aSplitKey = theStreamStr.SubString (aStartIndex, aCloseIndex - 1);

  // find value
  aStartIndex = aCloseIndex + JsonKeyLength (Standard_JsonKey_Quote);

  Standard_JsonKey aKey;
  if (!jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKey))
    return Standard_False;

  aStartIndex = aCloseIndex;
  if (!jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKey))
    return Standard_False;

  aStartIndex = aCloseIndex;
  switch (aKey)
  {
    case Standard_JsonKey_OpenChild:
    {
      aCloseIndex = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_OpenChild, Standard_JsonKey_CloseChild);
    }
    break;
    case Standard_JsonKey_OpenContainer:
    {
      aCloseIndex = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_OpenChild, Standard_JsonKey_CloseContainer);
    }
    break;
    case Standard_JsonKey_Quote:
    {
      aCloseIndex = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_OpenChild, Standard_JsonKey_CloseContainer);
      aCloseIndex += JsonKeyLength (Standard_JsonKey_CloseContainer) + 1;
    }
    break;
    default:
      return Standard_False;
  }

  TCollection_AsciiString aSplitValue = theStreamStr.SubString (aStartIndex, aCloseIndex);
  theValues.Add (aSplitKey, aSplitValue);
  theNextIndex = aCloseIndex;

  return Standard_True;
}

// ----------------------------------------------------------------------------
// jsonKey
// ----------------------------------------------------------------------------
Standard_Boolean Standard_Dump::jsonKey (const TCollection_AsciiString& theStreamStr,
                                         Standard_Integer theStartIndex,
                                         Standard_Integer& theNextIndex,
                                         Standard_JsonKey& theKey)
{
  TCollection_AsciiString aSubStreamStr = theStreamStr.SubString (theStartIndex, theStreamStr.Length());
  for (Standard_Integer aKeyId = (Standard_Integer)Standard_JsonKey_OpenChild; aKeyId <= Standard_JsonKey_SeparatorValueToValue; aKeyId++)
  {
    Standard_JsonKey aKey = (Standard_JsonKey)aKeyId;
    const Standard_CString aKeyToStr = JsonKeyToString (aKey);
    if (!aSubStreamStr.StartsWith (aKeyToStr))
      continue;

    theNextIndex = theStartIndex + Standard_Integer (JsonKeyLength (aKey));
    theKey = aKey;
    return Standard_True;
  }
  return Standard_False;
}

// ----------------------------------------------------------------------------
// HasChildKey
// ----------------------------------------------------------------------------
Standard_Boolean Standard_Dump::HasChildKey (const TCollection_AsciiString& theSourceValue)
{
  return theSourceValue.Search (JsonKeyToString (Standard_JsonKey_OpenChild)) >= 0;
}

// ----------------------------------------------------------------------------
// JsonKeyToString
// ----------------------------------------------------------------------------
const Standard_CString Standard_Dump::JsonKeyToString (const Standard_JsonKey theKey)
{
  switch (theKey)
  {
    case Standard_JsonKey_None: return "";
    case Standard_JsonKey_OpenChild: return "{";
    case Standard_JsonKey_CloseChild: return "}";
    case Standard_JsonKey_OpenContainer: return "[";
    case Standard_JsonKey_CloseContainer: return "]";
    case Standard_JsonKey_Quote: return "\"";
    case Standard_JsonKey_SeparatorKeyToValue: return ": ";
    case Standard_JsonKey_SeparatorValueToValue: return ", ";
    default: break;
  }

  return "";
}

// ----------------------------------------------------------------------------
// JsonKeyLength
// ----------------------------------------------------------------------------
Standard_Integer Standard_Dump::JsonKeyLength (const Standard_JsonKey theKey)
{
  return (Standard_Integer)strlen (JsonKeyToString (theKey));
}

// ----------------------------------------------------------------------------
// nextClosePosition
// ----------------------------------------------------------------------------
Standard_Integer Standard_Dump::nextClosePosition (const TCollection_AsciiString& theSourceValue,
                                                   const Standard_Integer theStartPosition,
                                                   const Standard_JsonKey theOpenKey,
                                                   const Standard_JsonKey theCloseKey)
{
  const Standard_CString anOpenKey = JsonKeyToString (theOpenKey);
  const Standard_CString aCloseKeyStr = JsonKeyToString (theCloseKey);

  Standard_Integer aStartPos = theStartPosition;
  Standard_Integer aDepthKey = 0;

  while (aStartPos < theSourceValue.Length())
  {
    Standard_Integer anOpenKeyPos = theSourceValue.Location (anOpenKey, aStartPos, theSourceValue.Length());
    Standard_Integer aCloseKeyPos = theSourceValue.Location (aCloseKeyStr, aStartPos, theSourceValue.Length());
    if (!aCloseKeyPos)
      break;

    if (anOpenKeyPos && anOpenKeyPos < aCloseKeyPos)
    {
      aDepthKey++;
      aStartPos = anOpenKeyPos + 1;
    }
    else
    {
      if (!aDepthKey)
        return aCloseKeyPos;
      else
      {
        aDepthKey--;
        aStartPos = aCloseKeyPos + 1;
      }
    }
  }
  return theSourceValue.Length();
}
