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
Standard_DumpSentry::Standard_DumpSentry (Standard_OStream& theOStream, const char* className)
: myOStream (&theOStream)
{
  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, className)
//(*myOStream) << "\"" << theClassName << "\": {";
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================
Standard_DumpSentry::~Standard_DumpSentry()
{
  //(*myOStream) << "}";
}

// =======================================================================
// function : AddValuesSeparator
// purpose :
// =======================================================================
void Standard_Dump::AddValuesSeparator (Standard_OStream& theOStream)
{
  Standard_SStream aStream;
  aStream << theOStream.rdbuf();
  TCollection_AsciiString aStreamStr = Standard_Dump::Text (aStream);
  if (!aStreamStr.IsEmpty() && !aStreamStr.EndsWith ("{"))
    theOStream << ", ";
}

//=======================================================================
//function : DumpKeyToClass
//purpose  : 
//=======================================================================
void Standard_Dump::DumpKeyToClass (Standard_OStream& theOStream,
                                    const TCollection_AsciiString& theKey,
                                    const TCollection_AsciiString& theField)
{
  AddValuesSeparator (theOStream);
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

//=======================================================================
//function : ProcessStreamName
//purpose  : 
//=======================================================================
Standard_Boolean Standard_Dump::ProcessStreamName (const Standard_SStream& theStream,
                                                   const TCollection_AsciiString& theName,
                                                   Standard_Integer& theStreamPos)
{
  TCollection_AsciiString aText = Text (theStream);
  if (aText.IsEmpty())
    return Standard_False;

  if (aText.Length () < theStreamPos)
    return Standard_False;

  TCollection_AsciiString aSubText = aText.SubString (theStreamPos, aText.Length());
  if (aSubText.StartsWith (JsonKeyToString (Standard_JsonKey_SeparatorValueToValue)))
  {
    theStreamPos += JsonKeyLength (Standard_JsonKey_SeparatorValueToValue);
    aSubText = aText.SubString (theStreamPos, aText.Length());
  }
  TCollection_AsciiString aKeyName = TCollection_AsciiString (JsonKeyToString (Standard_JsonKey_Quote))
    + theName
    + TCollection_AsciiString (JsonKeyToString (Standard_JsonKey_Quote))
    + JsonKeyToString (Standard_JsonKey_SeparatorKeyToValue);
  Standard_Boolean aResult = aSubText.StartsWith (aKeyName);
  if (aResult)
    theStreamPos += aKeyName.Length();

  return aResult;
}

//=======================================================================
//function : ProcessFieldName
//purpose  : 
//=======================================================================
Standard_Boolean Standard_Dump::ProcessFieldName (const Standard_SStream& theStream,
                                                  const TCollection_AsciiString& theName,
                                                  Standard_Integer& theStreamPos)
{
  TCollection_AsciiString aText = Text (theStream);
  if (aText.IsEmpty())
    return Standard_False;

  TCollection_AsciiString aSubText = aText.SubString (theStreamPos, aText.Length());
  if (aSubText.StartsWith (JsonKeyToString (Standard_JsonKey_SeparatorValueToValue)))
  {
    theStreamPos += JsonKeyLength (Standard_JsonKey_SeparatorValueToValue);
    aSubText = aText.SubString (theStreamPos, aText.Length());
  }

  TCollection_AsciiString aName = Standard_Dump::DumpFieldToName (theName.ToCString());
  TCollection_AsciiString aKeyName = TCollection_AsciiString (JsonKeyToString (Standard_JsonKey_Quote))
    + aName
    + TCollection_AsciiString (JsonKeyToString (Standard_JsonKey_Quote))
    + JsonKeyToString (Standard_JsonKey_SeparatorKeyToValue);

  Standard_Boolean aResult = aSubText.StartsWith (aKeyName);
  if (aResult)
    theStreamPos += aKeyName.Length();

  return aResult;
}

//=======================================================================
//function : InitRealValues
//purpose  : 
//=======================================================================
Standard_Boolean Standard_Dump::InitRealValues (const Standard_SStream& theStream,
                                                Standard_Integer& theStreamPos,
                                                int theCount, ...)
{
  Standard_Integer aStreamPos = theStreamPos + JsonKeyLength (Standard_JsonKey_OpenContainer);

  TCollection_AsciiString aText = Text (theStream);
  TCollection_AsciiString aSubText = aText.SubString (aStreamPos, aText.Length());

  va_list  vl;
  va_start(vl, theCount);
  aStreamPos = 1;
  Standard_Integer aClosePos = aSubText.Location (JsonKeyToString (Standard_JsonKey_CloseContainer), aStreamPos, aSubText.Length());
  for(int i = 0; i < theCount; ++i)
  {
    //if (i < theCount -1)
    //  theOStream << ", ";
    Standard_Integer aNextPos = (i < theCount-1) ? aSubText.Location (JsonKeyToString (Standard_JsonKey_SeparatorValueToValue), aStreamPos, aSubText.Length())
      : aClosePos;

    TCollection_AsciiString aValueText = aSubText.SubString (aStreamPos, aNextPos - 1);

    if (!aValueText.IsRealValue())
      return Standard_False;

    Standard_Real aValue = aValueText.RealValue();
    *(va_arg(vl, Standard_Real*)) = aValue;

    aStreamPos = aNextPos + JsonKeyLength (Standard_JsonKey_SeparatorValueToValue);
    //theOStream << va_arg(vl, Standard_Real);
  }
  va_end(vl);
  aClosePos = aText.Location (JsonKeyToString (Standard_JsonKey_CloseContainer), theStreamPos, aText.Length());
  theStreamPos = aClosePos + JsonKeyLength (Standard_JsonKey_CloseContainer);

  return Standard_True;
}

//=======================================================================
//function : InitValue
//purpose  : 
//=======================================================================
Standard_Boolean Standard_Dump::InitValue (const Standard_SStream& theStream,
                                           Standard_Integer& theStreamPos,
                                           TCollection_AsciiString& theValue)
{
  Standard_Integer aStreamPos = theStreamPos;

  TCollection_AsciiString aText = Text (theStream);
  TCollection_AsciiString aSubText = aText.SubString (aStreamPos, aText.Length());

  aStreamPos = 1;
  Standard_Integer aNextPos = aSubText.Location (JsonKeyToString (Standard_JsonKey_SeparatorValueToValue), aStreamPos, aSubText.Length());
  Standard_JsonKey aNextKey = Standard_JsonKey_SeparatorValueToValue;

  Standard_Integer aCloseChildPos = aSubText.Location (JsonKeyToString (Standard_JsonKey_CloseChild), aStreamPos, aSubText.Length());
  Standard_Boolean isUseClosePos = (aNextPos > 0 && aCloseChildPos > 0 && aCloseChildPos < aNextPos) || !aNextPos;
  if (isUseClosePos)
  {
    aNextPos = aCloseChildPos;
    aNextKey = Standard_JsonKey_CloseChild;
  }

  theValue = aNextPos ? aSubText.SubString (aStreamPos, aNextPos - 1) : aSubText;
  theStreamPos = aNextPos ? (theStreamPos + (aNextPos - aStreamPos) + JsonKeyLength (aNextKey)) : aText.Length();
  return Standard_True;
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
  if (!thePointer)
    return TCollection_AsciiString();

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
TCollection_AsciiString Standard_Dump::DumpFieldToName (const TCollection_AsciiString theField)
{
  TCollection_AsciiString aName = theField;
  if (theField.StartsWith ('&'))
  {
    aName.Remove (1, 1);
  }

  if (::LowerCase (aName.Value(1)) == 'a' && aName.Value (2) == 'n')
  {
    aName.Remove (1, 2);
  }
  else if (::LowerCase (aName.Value(1)) == 'a')
  {
    aName.Remove (1, 1);
  }
  else if (::LowerCase (aName.Value(1)) == 'm' && aName.Value (2) == 'y')
  {
    aName.Remove (1, 2);
  }
  
  if (TCollection_AsciiString (aName).EndsWith (".get()"))
  {
    aName = aName.SubString (1, aName.Length() - TCollection_AsciiString (".get()").Length());
  }
  else if (TCollection_AsciiString (aName).EndsWith ("()"))
  {
    aName = aName.SubString (1, aName.Length() - TCollection_AsciiString ("()").Length());
  }
  return aName;
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
                                           NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& theKeyToValues)
{
  Standard_Integer /*anIndex = 1, */aNextIndex = 1;
  while (aNextIndex < theStreamStr.Length())
  {
    Standard_JsonKey aKey;
    //Standard_Integer aNextIndex = 0;
    if (!jsonKey (theStreamStr, aNextIndex/*anIndex*/, aNextIndex, aKey))
      return Standard_False;

    Standard_Boolean aProcessed = Standard_False;
    switch (aKey)
    {
      case Standard_JsonKey_Quote:
      {
        aProcessed = splitKeyToValue (theStreamStr, aNextIndex/*anIndex*/, aNextIndex, theKeyToValues);
        //anIndex = aNextIndex;
      }
      break;
      case Standard_JsonKey_OpenChild:
      {
        Standard_Integer aStartIndex = aNextIndex;
        //Standard_Integer aNextIndex = anIndex;

        Standard_Integer aClosePos = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_OpenChild, Standard_JsonKey_CloseChild);
        if (!aClosePos)
          return Standard_False;

        TCollection_AsciiString aSubStreamStr = theStreamStr.SubString (aStartIndex + JsonKeyLength (aKey), aNextIndex - 2);
        if (!SplitJson (aSubStreamStr, theKeyToValues))
          return Standard_False;

        aNextIndex/*anIndex*/ = aClosePos + Standard_Integer (JsonKeyLength (Standard_JsonKey_CloseChild));
      }
      break;
      case Standard_JsonKey_SeparatorValueToValue:
      {
        //anIndex = aNextIndex;
        continue;
      }
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

  for (Standard_Integer anIndex = 1; anIndex <= theValues.Extent(); anIndex++)
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
                                                 NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& theValues)
{
  // find key value: "key"
  Standard_Integer aStartIndex = theStartIndex;
  Standard_Integer aCloseIndex = nextClosePosition (theStreamStr, aStartIndex + 1, Standard_JsonKey_None, Standard_JsonKey_Quote);
  if (!aCloseIndex)
    return Standard_False;

  TCollection_AsciiString aSplitKey = theStreamStr.SubString (aStartIndex/* + 1*/, aCloseIndex - 1);

  // key to value
  aStartIndex = aCloseIndex + 1;
  Standard_JsonKey aKey;
  if (!jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKey))
    return Standard_False;

  // find value
  aStartIndex = aCloseIndex;// + 1;
  /*Standard_JsonKey */aKey = Standard_JsonKey_None;
  if (jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKey))
  {
    //if (aKey == Standard_JsonKey_Quote && aCloseIndex - aStartIndex == 1)
    {
      //aKey = Standard_JsonKey_None;
      //aStartIndex = aCloseIndex + 1;
    }
    //else
    //  aStartIndex = aCloseIndex;// + 1;
    //Standard_JsonKey aKey;
    //if (!jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKey))
    //  return Standard_False;

    //aStartIndex = aCloseIndex;
    //aStartIndex = aCloseIndex + JsonKeyLength (Standard_JsonKey_Quote);
  }
  else
  {
    //aStartIndex = aCloseIndex;
  }

  //Standard_JsonKey aKey;
  //if (!jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKey))
  //  return Standard_False;

  //Standard_Integer aValueLength = (aCloseIndex - 1) - aStartIndex;
  aStartIndex = aCloseIndex;
  //if (!jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKey))
  //  return Standard_False;

  TCollection_AsciiString aSplitValue;
  theNextIndex = -1;
  switch (aKey)
  {
    case Standard_JsonKey_OpenChild:
    {
      aCloseIndex = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_OpenChild, Standard_JsonKey_CloseChild);
      if (aCloseIndex > aStartIndex)
        aSplitValue = theStreamStr.SubString (aStartIndex, aCloseIndex);
      theNextIndex = aCloseIndex + 1;
    }
    break;
    case Standard_JsonKey_OpenContainer:
    {
      aCloseIndex = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_OpenContainer, Standard_JsonKey_CloseContainer);
      if (aCloseIndex > aStartIndex)
        aSplitValue = theStreamStr.SubString (aStartIndex, aCloseIndex - 1);
      theNextIndex = aCloseIndex + 1;
    }
    break;
    case Standard_JsonKey_Quote:
    {
      Standard_JsonKey aKeyTmp;
      if (jsonKey (theStreamStr, aStartIndex, aCloseIndex, aKeyTmp) && aKeyTmp == Standard_JsonKey_Quote) // emptyValue
      {
        aSplitValue = "";
        theNextIndex = aCloseIndex;
      }
      else
      {
        aCloseIndex = nextClosePosition (theStreamStr, aStartIndex + 1, Standard_JsonKey_None, Standard_JsonKey_Quote);
        aSplitValue = theStreamStr.SubString (aStartIndex, aCloseIndex - 1);
        theNextIndex = aCloseIndex + 1;
      }
    }
    break;
    case Standard_JsonKey_None:
    {
      //if (aCloseIndex != aStartIndex) // case if the value is numerical and need not be closed
      {
        Standard_Integer aCloseIndex1 = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_None, Standard_JsonKey_CloseChild) - 1;
        Standard_Integer aCloseIndex2 = nextClosePosition (theStreamStr, aStartIndex, Standard_JsonKey_None, Standard_JsonKey_SeparatorValueToValue) - 1;
        aCloseIndex = aCloseIndex1 < aCloseIndex2 ? aCloseIndex1 : aCloseIndex2;
      }
      aSplitValue = aStartIndex <= aCloseIndex ? theStreamStr.SubString (aStartIndex, aCloseIndex) : "";
      theNextIndex = aCloseIndex + 1;
    }
    break;
    default:
      return Standard_False;
  }

  //TCollection_AsciiString aSplitValue = theStreamStr.SubString (aStartIndex, aCloseIndex - 1);
  //theNextIndex = aCloseIndex + 1;

  Standard_DumpValue aValue;
  if (theValues.FindFromKey (aSplitKey, aValue))
  {
    Standard_Integer anIndex = 1;
    // increment key until the new key does not exist in the container
    TCollection_AsciiString anIndexedSuffix = TCollection_AsciiString ("_") + TCollection_AsciiString (anIndex);
    while (theValues.FindFromKey (TCollection_AsciiString (aSplitKey + anIndexedSuffix), aValue))
    {
      anIndex++;
      anIndexedSuffix = TCollection_AsciiString ("_") + TCollection_AsciiString (anIndex);
    }
    aSplitKey = aSplitKey + anIndexedSuffix;
  }

  theValues.Add (aSplitKey, Standard_DumpValue (aSplitValue, aStartIndex));
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
  return theSourceValue.Search (JsonKeyToString (Standard_JsonKey_SeparatorKeyToValue)) >= 0;
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

    if (anOpenKeyPos && anOpenKeyPos <= aCloseKeyPos)
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
