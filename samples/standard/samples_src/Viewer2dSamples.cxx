#include "Viewer2dSamples.h"
#include "MakeBottle.h"

#include <AIS_Shape.hxx>

void Viewer2dSamples::ExecuteSample(TCollection_AsciiString theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "SpotLight3dSample")
    SpotLight3dSample();
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

void Viewer2dSamples::AppendBottle()
{
  TopoDS_Shape aBottle = MakeBottle(50, 70, 30);
  Handle(AIS_InteractiveObject) aShape = new AIS_Shape(aBottle);
  myObject3d.Append(aShape);
  myResult << "A bottle shape was created." << std::endl;
}

void Viewer2dSamples::SpotLight3dSample()
{

}
