// Created on: 2016-07-04
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_FaceIntersectionsSplitter.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_FaceIntersectionsSplitter::BRepMesh_FaceIntersectionsSplitter(
  const IMeshData::IFaceHandle& theFace,
  const IMeshTools_Parameters&  theParameters)
  : BRepMesh_SegmentedFace (theFace, theParameters)
{
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_FaceIntersectionsSplitter::~BRepMesh_FaceIntersectionsSplitter()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FaceIntersectionsSplitter::Perform()
{
  for (Standard_Integer aWireIt = 0; aWireIt < myDFace->WiresNb(); ++aWireIt)
  {
    Standard_Boolean isSplit = Standard_True;
    while (isSplit)
    {
      collectSegments();

      isSplit = perform(aWireIt);

      myWiresSegments  .Nullify();
      myWiresBndBoxTree.Nullify();
    }
  }

  return Standard_False;
}

//=======================================================================
//function : perform
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FaceIntersectionsSplitter::perform(
  const Standard_Integer theWireIndex)
{
  const Handle(Segments)& aSegments1 = myWiresSegments->Value (theWireIndex);

  BRepMesh_SegmentedFace::BndBox2dTreeSelector aSelector (Standard_True);
  for (Standard_Integer aWireIt = theWireIndex; aWireIt < myDFace->WiresNb(); ++aWireIt)
  {
    const Handle(IMeshData::BndBox2dTree)& aBndBoxTree2 = myWiresBndBoxTree->Value(aWireIt);
    const Handle(Segments)&                aSegments2   = myWiresSegments  ->Value(aWireIt);

    aSelector.SetSegments(aSegments2);
    for (Standard_Integer aSegmentIt = 0; aSegmentIt < aSegments1->Size(); ++aSegmentIt)
    {
      const BRepMesh_SegmentedFace::Segment& aSegment1 = aSegments1->Value(aSegmentIt);
      aSelector.Reset(&aSegment1, (aWireIt == theWireIndex) ? aSegmentIt : -1);
      if (aBndBoxTree2->Select(aSelector) != 0)
      {
        const IMeshData::VectorOfInteger&                         aSegments  = aSelector.Indices();
        const BRepMesh_SegmentedFace::VectorOfIntersectionParams& aIntParams = aSelector.IntParams();
        for (Standard_Integer aSelIt = 0; aSelIt < aSegments.Size(); ++aSelIt)
        {
          const BRepMesh_SegmentedFace::Segment& aSegment2 = aSegments2->Value(aSegments(aSelIt));

          //aIntersections->Add(aSegment1.EdgePtr);
          //aIntersections->Add(aSegment2.EdgePtr);
        }

        //return Standard_False;
      }
    }
  }

  return Standard_False;
}
