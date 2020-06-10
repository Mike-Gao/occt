#include "TopologySamples.h"




void TopologySamples::ExecuteSample(TCollection_AsciiString theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "Vertex3dSample")
    Vertex3dSample();
  else if (theSampleName == "Edge3dSample")
    Edge3dSample();
  else if (theSampleName == "Face3dSample")
    Face3dSample();
  else if (theSampleName == "Wire3dSample")
    Wire3dSample();
  else if (theSampleName == "Shell3dSample")
    Shell3dSample();
  else if (theSampleName == "Solid3dSample")
    Solid3dSample();
  else if (theSampleName == "Edge2dSample")
    Edge2dSample();
  else if (theSampleName == "Box3dSample")
    Box3dSample();
  else if (theSampleName == "Cylinder3dSample")
    Cylinder3dSample();
  else if (theSampleName == "Revolution3dSample")
    Revolution3dSample();
  else if (theSampleName == "TopologyIterator3dSample")
    TopologyIterator3dSample();
  else if (theSampleName == "AssessToCurve3dSample")
    AssessToCurve3dSample();
  else if (theSampleName == "AssessToCompositeCurve3dSample")
    AssessToCompositeCurve3dSample();
  else if (theSampleName == "AssessToSurface3dSample")
    AssessToSurface3dSample();
  else if (theSampleName == "Common3dSample")
    Common3dSample();
  else if (theSampleName == "Cut3dSample")
    Cut3dSample();
  else if (theSampleName == "Cut3dSample")
    Cut3dSample();
  else if (theSampleName == "Fuse3dSample")
    Fuse3dSample();
  else if (theSampleName == "Section3dSample")
    Section3dSample();
  else if (theSampleName == "Splitter3dSample")
    Splitter3dSample();
  else if (theSampleName == "Defeaturing3dSample")
    Defeaturing3dSample();
  else if (theSampleName == "Fillet3dSample")
    Fillet3dSample();
  else if (theSampleName == "Chamfer3dSample")
    Chamfer3dSample();
  else if (theSampleName == "Offset3dSample")
    Offset3dSample();
  else if (theSampleName == "Sweep3dSample")
    Sweep3dSample();
  else if (theSampleName == "Evolved3dSample")
    Evolved3dSample();
  else if (theSampleName == "Copy3dSample")
    Copy3dSample();
  else if (theSampleName == "Transform3dSample")
    Transform3dSample();
  else if (theSampleName == "ConvertToNurbs3dSample")
    ConvertToNurbs3dSample();
  else if (theSampleName == "SewContiguousFaces3dSample")
    SewContiguousFaces3dSample();
  else if (theSampleName == "CheckValidity3dSample")
    CheckValidity3dSample();
  else if (theSampleName == "ComputeLinearProperties3dSample")
    ComputeLinearProperties3dSample();
  else if (theSampleName == "ComputeSurfaceProperties3dSample")
    ComputeSurfaceProperties3dSample();
  else if (theSampleName == "ComputeVolumeProperties3dSample")
    ComputeVolumeProperties3dSample();
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}


void TopologySamples::Vertex3dSample()
{
  myResult << "Vertex3dSample" << std::endl;
}

void TopologySamples::Edge3dSample()
{

}

void TopologySamples::Face3dSample()
{

}

void TopologySamples::Wire3dSample()
{

}

void TopologySamples::Shell3dSample()
{

}
void TopologySamples::Solid3dSample()
{

}

void TopologySamples::Edge2dSample()
{

}

void TopologySamples::Box3dSample()
{

}

void TopologySamples::Cylinder3dSample()
{

}

void TopologySamples::Revolution3dSample()
{

}

void TopologySamples::TopologyIterator3dSample()
{

}

void TopologySamples::AssessToCurve3dSample()
{

}

void TopologySamples::AssessToCompositeCurve3dSample()
{

}

void TopologySamples::AssessToSurface3dSample()
{

}

void TopologySamples::Common3dSample()
{

}

void TopologySamples::Cut3dSample()
{

}

void TopologySamples::Fuse3dSample()
{

}

void TopologySamples::Section3dSample()
{

}

void TopologySamples::Splitter3dSample()
{

}

void TopologySamples::Defeaturing3dSample()
{

}

void TopologySamples::Fillet3dSample()
{

}

void TopologySamples::Chamfer3dSample()
{

}

void TopologySamples::Offset3dSample()
{

}

void TopologySamples::Sweep3dSample()
{

}

void TopologySamples::Evolved3dSample()
{

}

void TopologySamples::Copy3dSample()
{

}

void TopologySamples::Transform3dSample()
{

}

void TopologySamples::ConvertToNurbs3dSample()
{

}

void TopologySamples::SewContiguousFaces3dSample()
{

}

void TopologySamples::CheckValidity3dSample()
{

}

void TopologySamples::ComputeLinearProperties3dSample()
{

}

void TopologySamples::ComputeSurfaceProperties3dSample()
{

}

void TopologySamples::ComputeVolumeProperties3dSample()
{

}


