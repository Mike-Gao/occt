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

#include "BaseSample.h"

#include <iostream>
#include <regex>
#include <exception>
#include <stack>

#include <AIS_ViewCube.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QDir>
#include <Standard_WarningsRestore.hxx>

const TCollection_AsciiString BaseSample::FILE_EXTENSION = "cxx";

void BaseSample::Clear()
{
  myObject3d.Clear();
  myObject2d.Clear();
  myCode.Clear();
  myResult.str("");
}

Standard_Boolean BaseSample::IsProcessed()
{
  return myIsProcessed;
}

const NCollection_Vector<Handle(AIS_InteractiveObject)>& BaseSample::Get2dObjects()
{
  return myObject2d;
}

const NCollection_Vector<Handle(AIS_InteractiveObject)>& BaseSample::Get3dObjects()
{
  return myObject3d;
}

TCollection_AsciiString BaseSample::GetResult()
{
  TCollection_AsciiString aResult(myResult.str().c_str());
  myResult.str("");
  return aResult;
}

TCollection_AsciiString BaseSample::GetCode()
{
  return myCode;
}

void BaseSample::AppendCube()
{
  Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
  myObject3d.Append(aViewCube);
}

void BaseSample::Process(TCollection_AsciiString theSampleName)
{
  myObject3d.Clear();
  myObject2d.Clear();
  myCode.Clear();
  myIsProcessed = Standard_False;
  try
  {
    ExecuteSample(theSampleName);
    if (!myObject3d.IsEmpty())
    {
      Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
      myObject3d.Append(aViewCube);
    }
  }
  catch (...)
  {
    TraceError(TCollection_AsciiString("Error in sample: ") + theSampleName);
  }
}

void BaseSample::TraceError(TCollection_AsciiString theErrorMessage)
{
  std::cerr << std::endl << "ERROR: " << theErrorMessage.ToCString() << std::endl ;
  myResult << "\nERROR: " << theErrorMessage;
}

void BaseSample::FindSourceCode(TCollection_AsciiString theSampleName)
{
  TCollection_AsciiString aClassName = DynamicType()->Name();
  char aSeparator = QDir::separator().toLatin1();
  TCollection_AsciiString aCxxFilePach = myCodePath + aSeparator + aClassName + '.' + FILE_EXTENSION;
  OSD_File aCxxFile(aCxxFilePach);
  try
  {
    const Standard_Integer aFileBufferSize = 100 * 1024;
    TCollection_AsciiString aReadedText(aFileBufferSize);
    aCxxFile.Open(OSD_ReadOnly, OSD_Protection());
    aCxxFile.Read(aReadedText, aFileBufferSize);
    TCollection_AsciiString aRegexpTemplate = aClassName + "::" + theSampleName + "[\\n\\s]*\\([\\n\\s]*\\)[\\n\\s]*\\{";
    Standard_Integer aOpeningBracketPosition = FindEndOfPhrase(aReadedText, aRegexpTemplate);
    Standard_Integer aClosingBracketPosition = FindClosingBracket(aReadedText, aOpeningBracketPosition, '}');
    myCode = aReadedText.SubString(aOpeningBracketPosition + 1, aClosingBracketPosition - 1);
  }
  catch (...)
  {
    TraceError(TCollection_AsciiString("Cannot open file: ") + aCxxFilePach);
  }

}

Standard_Integer BaseSample::FindEndOfPhrase(TCollection_AsciiString theText,
  TCollection_AsciiString theRegexpTemplate)
{
  Standard_Integer aIndexOfLastFoundSymbol = -1;
  std::string aStdText = theText.ToCString();
  std::string aRegexpTemplate = theRegexpTemplate.ToCString();

  try
  {
    std::regex aRegex(theRegexpTemplate.ToCString());

    std::sregex_iterator aDetectIterator = std::sregex_iterator(aStdText.begin(), aStdText.end(), aRegex);
    if (aDetectIterator != std::sregex_iterator())
    {
      std::smatch aMatch = *aDetectIterator;
      std::string aFoundString = aMatch.str();
      aIndexOfLastFoundSymbol = static_cast<Standard_Integer>(aStdText.find(aFoundString) + aFoundString.length());
    }
    else
    {
      TraceError(TCollection_AsciiString("No code found for template: ") + theRegexpTemplate);
    }
  }
  catch (const std::regex_error& aRegError)
  {
    TraceError(TCollection_AsciiString("regex_error: ") + aRegError.what());
  }
  catch (const std::exception& aEx)
  {
    TraceError(TCollection_AsciiString("common error: ") + aEx.what());
  }
  catch (...)
  {
    TraceError("unknown error!");
  }
  return aIndexOfLastFoundSymbol;
}

Standard_Integer BaseSample::FindClosingBracket(TCollection_AsciiString theText,
  Standard_Integer theOpeningBracketIndex,
  Standard_Character theClosingBracketSymbol)
{
  // TODO this function not implemented at least 2 cases:
  // - brackets in strings & chars
  // - brackets in comments
  Standard_Integer aClosingBracketIndex = -1;
  Standard_Character anOpeningBracketSymbol = theText.Value(theOpeningBracketIndex);
  TCollection_AsciiString aBracketsSet(theClosingBracketSymbol);
  aBracketsSet += anOpeningBracketSymbol;
  Standard_Integer aBracketDepth = 1;
  Standard_Integer aStartFindIndex = theOpeningBracketIndex + 1;
  //Standard_Character aStartFindChar = theText.Value(aStartFindIndex-1);
  while (aBracketDepth)
  {
    aStartFindIndex = theText.FirstLocationInSet(aBracketsSet, aStartFindIndex, theText.Length());
    if (!aStartFindIndex)
    {
      TraceError("No closing bracket found!");
      break;
    }
    TCollection_AsciiString aRSubstr = theText.SubString(aStartFindIndex, theText.Length());
    if (theText.Value(aStartFindIndex) == anOpeningBracketSymbol)
      aBracketDepth++;
    else if (theText.Value(aStartFindIndex) == theClosingBracketSymbol)
      aBracketDepth--;
    if (!aBracketDepth)
    {
      aClosingBracketIndex = aStartFindIndex;
      break;
    }
    aStartFindIndex++;
  }
  return aClosingBracketIndex;
}
