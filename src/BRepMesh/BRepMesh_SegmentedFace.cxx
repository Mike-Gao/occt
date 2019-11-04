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

#include <BRepMesh_SegmentedFace.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <OSD_Parallel.hxx>

namespace
{
  //! Functor to be used to fill segments and bounding box tree in parallel.
  class SegmentsFiller
  {
  public:
    //! Constructor.
    SegmentsFiller(const IMeshData::IFaceHandle&                      theDFace,
                   Handle(BRepMesh_SegmentedFace::ArrayOfSegments)&   theWiresSegments,
                   Handle(BRepMesh_SegmentedFace::ArrayOfBndBoxTree)& theWiresBndBoxTree)
      : myDFace(theDFace),
        myWiresSegments(theWiresSegments),
        myWiresBndBoxTree(theWiresBndBoxTree)
    {
      myWiresSegments   = new BRepMesh_SegmentedFace::ArrayOfSegments   (0, myDFace->WiresNb() - 1);
      myWiresBndBoxTree = new BRepMesh_SegmentedFace::ArrayOfBndBoxTree (0, myDFace->WiresNb() - 1);
    }

    //! Performs initialization of wire with the given index.
    void operator()(const Standard_Integer theWireIndex) const
    {
      const IMeshData::IWireHandle& aDWire = myDFace->GetWire(theWireIndex);

      Handle(NCollection_IncAllocator) aTmpAlloc1 = new NCollection_IncAllocator();

      Handle(BRepMesh_SegmentedFace::Segments) aSegments = 
        new BRepMesh_SegmentedFace::Segments(aDWire->EdgesNb(), aTmpAlloc1);
      Handle(IMeshData::BndBox2dTree) aBndBoxTree = new IMeshData::BndBox2dTree(aTmpAlloc1);

      myWiresSegments  ->ChangeValue(theWireIndex) = aSegments;
      myWiresBndBoxTree->ChangeValue(theWireIndex) = aBndBoxTree;

      Handle(NCollection_IncAllocator) aTmpAlloc2 = new NCollection_IncAllocator();
      IMeshData::BndBox2dTreeFiller aBndBoxTreeFiller(*aBndBoxTree, aTmpAlloc2);

      for (Standard_Integer aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
      {
        // TODO: check 2d wire for consistency.

        const IMeshData::IEdgePtr&      aDEdge  = aDWire->GetEdge(aEdgeIt);
        const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(myDFace.get(), aDWire->GetEdgeOrientation(aEdgeIt));

        for (Standard_Integer aPointIt = 1; aPointIt < aPCurve->ParametersNb(); ++aPointIt)
        {
          gp_Pnt2d& aPnt1 = aPCurve->GetPoint(aPointIt - 1);
          gp_Pnt2d& aPnt2 = aPCurve->GetPoint(aPointIt);

          Bnd_Box2d aBox;
          aBox.Add(aPnt1);
          aBox.Add(aPnt2);
          aBox.Enlarge(Precision::Confusion());

          aBndBoxTreeFiller.Add(aSegments->Size(), aBox);
          aSegments->Append(BRepMesh_SegmentedFace::Segment(
            aDEdge, aPCurve.get(),
            &aPnt1, aPointIt - 1,
            &aPnt2, aPointIt));
        }
      }

      aBndBoxTreeFiller.Fill();
    }

  private:

    SegmentsFiller (const SegmentsFiller& theOther);

    void operator=(const SegmentsFiller& theOther);

  private:

    const IMeshData::IFaceHandle&                      myDFace;
    Handle(BRepMesh_SegmentedFace::ArrayOfSegments)&   myWiresSegments;
    Handle(BRepMesh_SegmentedFace::ArrayOfBndBoxTree)& myWiresBndBoxTree;
  };
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_SegmentedFace::BndBox2dTreeSelector::BndBox2dTreeSelector(
  const Standard_Boolean isCollectIntersectionParams)
  : myCollectIntersectionParams (isCollectIntersectionParams)
  , mySelfSegmentIndex(-1)
  , myIndices(256, new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE))
{
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================
void BRepMesh_SegmentedFace::BndBox2dTreeSelector::Reset(
  const BRepMesh_SegmentedFace::Segment* theSegment,
  const Standard_Integer                 theSelfSegmentIndex)
{
  myIndices  .Clear();
  myIntParams.Clear();

  mySelfSegmentIndex = theSelfSegmentIndex;
  mySegment = theSegment;

  myBox.SetVoid();
  myBox.Add(*mySegment->Point1);
  myBox.Add(*mySegment->Point2);
  myBox.Enlarge(Precision::Confusion());
}

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_SegmentedFace::BndBox2dTreeSelector::Reject(
  const Bnd_Box2d& theBox) const
{
  return myBox.IsOut(theBox);
}

//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_SegmentedFace::BndBox2dTreeSelector::Accept(
  const Standard_Integer& theSegmentIndex)
{
  const BRepMesh_SegmentedFace::Segment& aSegment = mySegments->Value(theSegmentIndex);

  IntersectionParams aIntNode;
  aIntNode.Type = BRepMesh_GeomTool::IntSegSeg(
    mySegment->Point1->XY(), mySegment->Point2->XY(),
    aSegment  .Point1->XY(), aSegment  .Point2->XY(),
    Standard_False, Standard_False,
    aIntNode.Point, aIntNode.Params);

  if (aIntNode.Type == BRepMesh_GeomTool::Cross          ||
      aIntNode.Type == BRepMesh_GeomTool::PointOnSegment ||
      aIntNode.Type == BRepMesh_GeomTool::Glued)
  {

    myIndices.Append(theSegmentIndex);

    if (myCollectIntersectionParams)
    {
      myIntParams.Append(aIntNode);
    }
    return Standard_True;
  }

  return Standard_False;
}




//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_SegmentedFace::BRepMesh_SegmentedFace(
  const IMeshData::IFaceHandle& theFace,
  const IMeshTools_Parameters&  theParameters)
  : myDFace     (theFace),
    myParameters(theParameters)
{
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_SegmentedFace::~BRepMesh_SegmentedFace()
{
}

//=======================================================================
//function : collectSegments
//purpose  : 
//=======================================================================
void BRepMesh_SegmentedFace::collectSegments()
{
  SegmentsFiller aSegmentsFiller(myDFace, myWiresSegments, myWiresBndBoxTree);
  OSD_Parallel::For(0, myDFace->WiresNb(), aSegmentsFiller, !isParallel());
}
