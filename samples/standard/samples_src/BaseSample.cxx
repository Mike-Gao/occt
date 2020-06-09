

#include "BaseSample.h"

#include <iostream>
#include <regex>
#include <exception>
#include <stack>


#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>

const TCollection_AsciiString BaseSample::FILE_EXTENSION = "cxx";



Standard_Boolean BaseSample::IsProcessed()
{
  return myIsProcessed;
}

NCollection_Vector<Handle(AIS_InteractiveObject) > BaseSample::Get2dObject()
{
  return myObject2d;
}

NCollection_Vector<Handle(AIS_InteractiveObject) > BaseSample::Get3dObject()
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

void BaseSample::Process(TCollection_AsciiString theSampleName)
{
  myObject3d.Clear();
  myObject2d.Clear();
  myCode.Clear();
  myIsProcessed = Standard_False;
  try
  {
    ExecuteSample(theSampleName);
  }
  catch (...)
  {
    TraceError(TCollection_AsciiString("Error in sample: ") + theSampleName);
  }

}

void BaseSample::SetCodePach(TCollection_AsciiString theSampleSourcePach)
{
  myCodePath = theSampleSourcePach;
}

void BaseSample::TraceError(TCollection_AsciiString theErrorMessage)
{
  std::cerr << std::endl << "ERROR: " << theErrorMessage.ToCString();
  myResult << "\nERROR: " << theErrorMessage;
}

void BaseSample::FindSourceCode(TCollection_AsciiString theSampleName)
{
  TCollection_AsciiString aClassName = DynamicType()->Name();
  TCollection_AsciiString aCxxFilePach = myCodePath + '\\' + aClassName + '.' + FILE_EXTENSION;
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
    myCode = aReadedText.SubString(aOpeningBracketPosition+1, aClosingBracketPosition-1);
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
      aIndexOfLastFoundSymbol = aStdText.find(aFoundString) + aFoundString.length();
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
  Standard_Character aStartFindChar = theText.Value(aStartFindIndex-1);
  while (aBracketDepth)
  {
    aStartFindIndex = theText.FirstLocationInSet(aBracketsSet, aStartFindIndex, theText.Length());
    if (!aStartFindIndex)
    {
      TraceError("No closing bracket found!");
      break;
    }
    Standard_Character aCh = theText.Value(aStartFindIndex);
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






