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

#include "TriangulationSamples.h"
#include "MakeBottle.h"

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Triangulation.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
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

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  Standard_Integer aNbTriangles(0);
  for (TopExp_Explorer anExplorer(aBottle, TopAbs_FACE); anExplorer.More(); anExplorer.Next()) {
    TopoDS_Face aFace = TopoDS::Face(anExplorer.Current());
    TopLoc_Location aLocation;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aFace, aLocation);

    TColgp_Array1OfPnt aTriangNodes(1, (aTriangulation->NbNodes()));
    aTriangNodes = aTriangulation->Nodes();
    Poly_Array1OfTriangle aTriangles(1, aTriangulation->NbTriangles());
    aTriangles = aTriangulation->Triangles();

    for (Standard_Integer i = 1; i <= (aTriangulation->NbTriangles()); i++) {
      Poly_Triangle trian = aTriangles.Value(i);
      Standard_Integer index1, index2, index3, M = 0, N = 0;
      trian.Get(index1, index2, index3);

      for (Standard_Integer j = 1; j <= 3; j++) {
        switch (j) {
        case 1:
          M = index1;
          N = index2;
          break;
        case 2:
          N = index3;
          break;
        case 3:
          M = index2;
        }

        BRepBuilderAPI_MakeEdge anEdgeMaker(aTriangNodes.Value(M), aTriangNodes.Value(N));
        if (anEdgeMaker.IsDone()) {
          aBuilder.Add(aCompound, anEdgeMaker.Edge());
        }
      }
    }
    Handle(AIS_Triangulation) anAisTriangulation = new AIS_Triangulation(aTriangulation);
    aNbTriangles += aTriangulation->NbTriangles();
    myObject3d.Append(anAisTriangulation);
  }
  Handle(AIS_Shape)	anAisCompound = new AIS_Shape(aCompound);
  myObject3d.Append(anAisCompound);
  Handle(AIS_Shape) AISBottle = new AIS_Shape(aBottle);
  myObject3d.Append(AISBottle);

  myResult << "Compute the triangulation on a shape: " << aNbTriangles;
}
