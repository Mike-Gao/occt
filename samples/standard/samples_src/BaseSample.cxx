

#include "BaseSample.h"

#include <iostream>
#include <regex>
#include <exception>
#include <stack>

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_Point.hxx>
#include <AIS_TextLabel.hxx>

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
  return myResult;
}

TCollection_AsciiString BaseSample::GetCode() 
{
  return myCode;
}

void BaseSample::Process(TCollection_AsciiString theSampleName)
{
  myObject3d.Clear();
  myObject2d.Clear();
  myResult.Clear();
  myCode.Clear();
  myIsProcessed = Standard_False;
  try
  {
    Standard_Boolean anIsSamplePresent = Standard_True;
    FindSourceCode(theSampleName);
    if (theSampleName == "ZeroDimensionObjects3dSample")
      ZeroDimensionObjects3dSample();
    else if (theSampleName == "ZeroDimensionObjects3dSample")
      Vectors3dSample();
    else
    {
      myResult += TCollection_AsciiString("No function found: ") + theSampleName;
      myCode += TCollection_AsciiString("No function found: ") + theSampleName;
      anIsSamplePresent = Standard_False;
    }
    myIsProcessed = anIsSamplePresent;
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
  myResult += TCollection_AsciiString("\nERROR: ") + theErrorMessage;
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

void BaseSample::ZeroDimensionObjects3dSample()
{
  // gp_Pnt describes a point in 3D space.A Geom_CartesianPoint is defined by 
  // a gp_Pnt point, with its three Cartesian coordinates X, Y and Z.
  gp_Pnt aCoordPnt(10, 20, 30);
  Handle(Geom_CartesianPoint) aCoordGeomPoint = new Geom_CartesianPoint(aCoordPnt);
  Handle(AIS_Point) aCoordAisPoint = new AIS_Point(aCoordGeomPoint);
  myObject3d.Append(aCoordAisPoint);

  Handle(AIS_TextLabel) aPntLabel = new AIS_TextLabel();
  aPntLabel->SetText ("gp_Pnt");
  aPntLabel->SetPosition(gp_Pnt (aCoordPnt.X(), aCoordPnt.Y(), aCoordPnt.Z() + 5));
  myObject3d.Append(aPntLabel);

  // gp_XYZ class describes a cartesian coordinate entity in 3D space (X,Y,Z).
  // This entity is used for algebraic calculation.
  // This entity can be transformed with a "Trsf" or a "GTrsf" from package "gp".
  // It is used in vectorial computations or for holding this type of information in data structures.
  gp_XYZ aXyz1(10, 20, 30);
  gp_XYZ aXyz2(20, 10, 30);
  gp_XYZ aXyzSum = aXyz1 + aXyz2;
  gp_Pnt aSumPnt(aXyzSum);
  Handle(Geom_CartesianPoint) aXyzGeomPoint = new Geom_CartesianPoint(aSumPnt);
  Handle(AIS_Point) aSumAisPoint = new AIS_Point(aXyzGeomPoint);
  myObject3d.Append(aSumAisPoint);

  Handle(AIS_TextLabel) aXyzLabel = new AIS_TextLabel();
  aXyzLabel->SetText("gp_Pnt");
  aXyzLabel->SetPosition(gp_Pnt(aXyzSum.X(), aXyzSum.Y(), aXyzSum.Z() + 5));
  myObject3d.Append(aXyzLabel);

  myResult += "ZeroDimensionObjects3dSample result";

}

void BaseSample::Vectors3dSample()
{
  myResult += "Vectors3dSample result";

}

// 

