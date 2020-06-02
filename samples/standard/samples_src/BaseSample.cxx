

#include "BaseSample.hxx"

#include <iostream>

#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>


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
//      anIsSamplePresent = Standard_False;
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
  std::cout << std::endl << "ERROR: " << theErrorMessage.ToCString();
  myResult += TCollection_AsciiString("\nERROR: ") + theErrorMessage;
}

void BaseSample::FindSourceCode(TCollection_AsciiString theSampleName)
{
  TCollection_AsciiString aCxxFilePach = myCodePath + "\\BaseSample.cxx";
  OSD_File aCxxFile(aCxxFilePach);
  try
  {
    const Standard_Integer aFileBufferSize = 100 * 1024;
    TCollection_AsciiString aReadedText(aFileBufferSize);
    aCxxFile.Open(OSD_ReadOnly, OSD_Protection());
    aCxxFile.Read(aReadedText, aFileBufferSize);
    myCode = aReadedText;
  }
  catch (...)
  {
    TraceError(TCollection_AsciiString("Cannot open file: ") + aCxxFilePach);
  }

  Standard_Boolean anIsValidPath = OSD_Path::IsValid("C:\\Work\\occt-CR31570\\samples\\standard\\samples_src\\BaseSample.cxx");
  if (anIsValidPath)
  {
    myCode += myCodePath;
  }
}

void BaseSample::ZeroDimensionObjects3dSample()
{
  myResult += "ZeroDimensionObjects3dSample result";
  myCode += "ZeroDimensionObjects3dSample code";
}

void BaseSample::Vectors3dSample()
{
  myResult += "Vectors3dSample result";
  myCode += "Vectors3dSample code";
}

// 

