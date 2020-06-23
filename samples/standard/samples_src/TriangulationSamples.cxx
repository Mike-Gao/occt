#include "TriangulationSamples.h"
#include "MakeBottle.h"

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Triangulation.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <Poly_Triangulation.hxx>



void TriangulationSamples::ExecuteSample(TCollection_AsciiString theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "Triangulation3dSample")
    Triangulation3dSample();
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}


void TriangulationSamples::Triangulation3dSample()
{
  TopoDS_Shape aBottle = MakeBottle(50, 70, 30);
  BRepMesh_IncrementalMesh(aBottle, 1);

  Standard_Integer aNbTriangles(0);
  for (TopExp_Explorer anExplorer(aBottle, TopAbs_FACE); anExplorer.More(); anExplorer.Next()) 
  {
    TopoDS_Face aFace = TopoDS::Face(anExplorer.Current());
    TopLoc_Location aLocation;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aFace, aLocation);
    Handle(AIS_Triangulation) anAisTriangulation = new AIS_Triangulation(aTriangulation);
    aNbTriangles += aTriangulation->NbTriangles();
    myObject3d.Append(anAisTriangulation);
  }
  Handle(AIS_Shape) AISBottle = new AIS_Shape(aBottle);
  myObject3d.Append(AISBottle);
  myResult << "Compute the triangulation on a shape: " << aNbTriangles;
}
