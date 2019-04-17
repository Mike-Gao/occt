// Created by: Eugeny MALTCHIKOV
// Created on: 17/04/2019
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_Tools.hxx>

#include <BRep_Builder.hxx>

#include <BRepBndLib.hxx>

#include <BVH_Box.hxx>
#include <BVH_BoxSet.hxx>
#include <BVH_LinearBuilder.hxx>

#include <DBRep.hxx>

#include <TopExp.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

#include <TopTools_IndexedMapOfShape.hxx>

//=======================================================================
//function : ShapeSelector
//purpose : Implement the simplest shape's selector
//=======================================================================
class ShapeSelector : public BVH_BoxSet <Standard_Real, 3, TopoDS_Shape>::Selector
{
public:
  //! Constructor
  ShapeSelector() {}

  //! Sets the Box for selection
  void SetBox (const Bnd_Box& theBox)
  {
    myBox = Bnd_Tools::Bnd2BVH (theBox);
  }

  //! Returns the selected shapes
  const NCollection_List<TopoDS_Shape>& Shapes () const { return myShapes; }

public:

  //! Defines the rules for node rejection
  virtual Standard_Boolean Reject (const BVH_Box <Standard_Real, 3>& theBox) const Standard_OVERRIDE
  {
    return myBox.IsOut (theBox);
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const TopoDS_Shape& theShape) Standard_OVERRIDE
  {
    myShapes.Append (theShape);
    return Standard_True;
  }

protected:

  BVH_Box <Standard_Real, 3> myBox;         //!< Selection box
  NCollection_List <TopoDS_Shape> myShapes; //!< Selected shapes
};

//=======================================================================
//function : QABVH_ShapeSelect
//purpose : Test the work of BVH on the simple example of shapes selection
//=======================================================================
static Standard_Integer QABVH_ShapeSelect (Draw_Interpretor& theDI,
                                           Standard_Integer theArgc,
                                           const char** theArgv)
{
  if (theArgc != 4)
  {
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  // Get the shape to add its sub-shapes into BVH
  TopoDS_Shape aShape = DBRep::Get (theArgv [2]);
  if (aShape.IsNull())
  {
    std::cout << theArgv[2] << " does not exist" << std::endl;
    return 1;
  }

  // Get the shape to get the Box for selection
  TopoDS_Shape aBShape = DBRep::Get (theArgv [3]);
  if (aBShape.IsNull())
  {
    std::cout << theArgv[3] << " does not exist" << std::endl;
    return 1;
  }

  // Define BVH Builder
  opencascade::handle <BVH_LinearBuilder <Standard_Real, 3> > aLBuilder =
      new BVH_LinearBuilder <Standard_Real, 3>();

  // Create the ShapeSet
  opencascade::handle <BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> > aShapeBoxSet =
    new BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> (aLBuilder);

  // Add elements into BVH

  // Map the shape
  TopTools_IndexedMapOfShape aMapShapes;
  TopExp::MapShapes (aShape, TopAbs_VERTEX, aMapShapes);
  TopExp::MapShapes (aShape, TopAbs_EDGE,   aMapShapes);
  TopExp::MapShapes (aShape, TopAbs_FACE,   aMapShapes);

  for (Standard_Integer iS = 1; iS <= aMapShapes.Extent(); ++iS)
  {
    const TopoDS_Shape& aS = aMapShapes(iS);

    Bnd_Box aSBox;
    BRepBndLib::Add (aS, aSBox);

    aShapeBoxSet->Add (aS, Bnd_Tools::Bnd2BVH (aSBox));
  }

  // Build BVH
  aShapeBoxSet->Build();

  // Initialize selector
  ShapeSelector aSelector;
  Bnd_Box aSelectionBox;
  BRepBndLib::Add (aBShape, aSelectionBox);
  aSelector.SetBox (aSelectionBox);

  // Select the elements
  aShapeBoxSet->Select (aSelector);
  const TopTools_ListOfShape& aSelectedShapes = aSelector.Shapes();

  // Draw the selected shapes
  TopoDS_Compound aResult;
  BRep_Builder().MakeCompound (aResult);

  for (TopTools_ListOfShape::Iterator it (aSelectedShapes); it.More(); it.Next())
    BRep_Builder().Add (aResult, it.Value());

  DBRep::Set (theArgv[1], aResult);
  return 0;
}

//=======================================================================
//function : PairShapeSelector
//purpose : Implement the simplest shape's selector
//=======================================================================
class PairShapesSelector : public BVH_BoxSet <Standard_Real, 3, TopoDS_Shape>::PairSelector
{
public:
  //! Constructor
  PairShapesSelector() {}

  //! Returns the selected pairs of shapes
  const NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >& Pairs () const { return myPairs; }

public:

  //! Defines the rules for node rejection
  virtual Standard_Boolean Reject (const BVH_Box <Standard_Real, 3>& theBox1,
                                   const BVH_Box <Standard_Real, 3>& theBox2) const Standard_OVERRIDE
  {
    return theBox1.IsOut (theBox2);
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const TopoDS_Shape& theShape1, const TopoDS_Shape& theShape2) Standard_OVERRIDE
  {
    myPairs.Append (std::pair <TopoDS_Shape, TopoDS_Shape> (theShape1, theShape2));
    return Standard_True;
  }

protected:

  NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> > myPairs; //!< Selected pairs
};

//=======================================================================
//function : QABVH_PairSelect
//purpose : Test the work of BVH on the simple example of pairs of shapes selection
//=======================================================================
static Standard_Integer QABVH_PairSelect (Draw_Interpretor& theDI,
                                          Standard_Integer theArgc,
                                          const char** theArgv)
{
  if (theArgc != 4)
  {
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  TopoDS_Shape aShape[2];
  // Get the first shape
  aShape[0] = DBRep::Get (theArgv [2]);
  if (aShape[0].IsNull())
  {
    std::cout << theArgv[2] << " does not exist" << std::endl;
    return 1;
  }

  // Get the second shape
  aShape[1] = DBRep::Get (theArgv [3]);
  if (aShape[1].IsNull())
  {
    std::cout << theArgv[3] << " does not exist" << std::endl;
    return 1;
  }

  // Define BVH Builder
  opencascade::handle <BVH_LinearBuilder <Standard_Real, 3> > aLBuilder =
      new BVH_LinearBuilder <Standard_Real, 3>();

  // Create the ShapeSet
  opencascade::handle <BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> > aShapeBoxSet[2];

  for (Standard_Integer i = 0; i < 2; ++i)
  {
    aShapeBoxSet[i] = new BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> (aLBuilder);
    // Add elements into set
    TopTools_IndexedMapOfShape aMapShapes;
    TopExp::MapShapes (aShape[i], TopAbs_VERTEX, aMapShapes);
    TopExp::MapShapes (aShape[i], TopAbs_EDGE,   aMapShapes);
    TopExp::MapShapes (aShape[i], TopAbs_FACE,   aMapShapes);

    for (Standard_Integer iS = 1; iS <= aMapShapes.Extent(); ++iS)
    {
      const TopoDS_Shape& aS = aMapShapes(iS);
  
      Bnd_Box aSBox;
      BRepBndLib::Add (aS, aSBox);
  
      aShapeBoxSet[i]->Add (aS, Bnd_Tools::Bnd2BVH (aSBox));
    }
    // Build BVH
    aShapeBoxSet[i]->Build();
  }

  // Initialize selector
  PairShapesSelector aSelector;
  // Select the elements
  aShapeBoxSet[0]->Select (aShapeBoxSet[1], aSelector);

  const NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >& aPairs = aSelector.Pairs();

  // Draw the selected shapes
  TopoDS_Compound aResult;
  BRep_Builder().MakeCompound (aResult);

  for (NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >::Iterator it (aPairs); it.More(); it.Next())
  {
    TopoDS_Compound aPair;
    BRep_Builder().MakeCompound (aPair);

    BRep_Builder().Add (aPair, it.Value().first);
    BRep_Builder().Add (aPair, it.Value().second);

    BRep_Builder().Add (aResult, aPair);
  }

  DBRep::Set (theArgv[1], aResult);
  return 0;
}

//=======================================================================
//function : Commands_BVH
//purpose : BVH commands
//=======================================================================
void QABugs::Commands_BVH (Draw_Interpretor& theCommands)
{
  const char *group = "QABugs";

  theCommands.Add("QABVH_ShapeSelect",
                  "Tests the work of BHV_BoxSet algorithm on the simple example of selection of shapes located which boxes interfere with given box.\n"
                  "Usage: QABVH_ShapeSelect result shape box (defined as a solid)\n"
                  "\tResult should contain all sub-shapes of the shape interfering with given box",
                  __FILE__, QABVH_ShapeSelect, group);

  theCommands.Add("QABVH_PairSelect",
                  "Tests the work of BHV_BoxSet algorithm on the simple example of selection of pairs of shapes with interfering bounding boxes.\n"
                  "Usage: QABVH_PairSelect result shape1 shape2\n"
                  "\tResult should contain all interfering pairs (compound of pairs)",
                  __FILE__, QABVH_PairSelect, group);
}
