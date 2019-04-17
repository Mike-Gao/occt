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

#ifndef _BVH_BoxSet_Header
#define _BVH_BoxSet_Header

#include <BVH_PrimitiveSet.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_List.hxx>

//! Abstract class implementing the set of elements with bounding boxes
//! organized with bounding volume hierarchy (BVH).
//!
//! The elements and their boxes are always synchronized which allows
//! implementing convenient methods for elements selection by tree descend.
//!
//! The selection/rejection rules should be implemented in the derived Selector
//! class in the Accept/Reject methods.
//! The Reject() method is performed on the nodes (both inner and outer) to check
//! and reject the whole branches of the tree. It operates with boxes and usually
//! is simple enough.
//! The Accept() method on the other hand is applied to the leafs only (performs
//! the leaf-node operation). It operates with elements on which boxes are built
//! and implements the logic of the operation.
//!
//! Two selection methods are implemented - single tree descend used for elements selections
//! and parallel descend of two BVH trees for selections of pairs of elements of these trees.
//!
//! Here is the example of usage of the algorithm.
//! It implements the selection of pairs of sub-shapes of two shapes with
//! interfering bounding boxes. It uses the Linear builder for BVH construction:
//! ~~~~
//! // Selector implementing the Rejection/Selection rules
//! class PairShapesSelector : public BVH_BoxSet <Standard_Real, 3, TopoDS_Shape>::PairSelector
//! {
//! public:
//!   // Constructor
//!   PairShapesSelector() {}
//!
//!   // Returns the selected pairs
//!   const NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >& Pairs () const { return myPairs; }
//!
//! protected:
//!
//!   // Defines the rules for node rejection
//!   virtual Standard_Boolean Reject (const BVH_Box <Standard_Real, 3>& theBox1,
//!                                    const BVH_Box <Standard_Real, 3>& theBox2) const Standard_OVERRIDE
//!   {
//!     return theBox1.IsOut (theBox2);
//!   }
//!
//!   // Defines the rules for leaf acceptance
//!   virtual Standard_Boolean Accept (const TopoDS_Shape& theShape1, const TopoDS_Shape& theShape2) Standard_OVERRIDE
//!   {
//!     myPairs.Append (std::pair <TopoDS_Shape, TopoDS_Shape> (theShape1, theShape2));
//!     return Standard_True;
//!   }
//!
//! protected:
//!
//!   // Selected pairs
//!   NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> > myPairs;
//! };
//!
//! TopoDS_Shape aShape[2];
//! aShape[0] = ...; // First shape for BVH construction on its sub-shapes
//! aShape[1] = ...; // Second shape for BVH construction on its sub-shapes
//!
//! // Define BVH Builder
//! opencascade::handle <BVH_LinearBuilder <Standard_Real, 3> > aLBuilder = new BVH_LinearBuilder <Standard_Real, 3> ();
//!
//! // Create the ShapeSet
//! opencascade::handle <BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> > aShapeBoxSet[2];
//!
//! for (int i = 0; i < 2; ++i)
//! {
//!   aShapeBoxSet[i] = new BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> (aLBuilder);
//!
//!   // Add elements with boxes into set
//!   TopTools_IndexedMapOfShape aMapShapes;
//!   TopExp::MapShapes (aShape[i], TopAbs_VERTEX, aMapShapes);
//!   TopExp::MapShapes (aShape[i], TopAbs_EDGE, aMapShapes);
//!   TopExp::MapShapes (aShape[i], TopAbs_FACE, aMapShapes);
//!
//!   for (Standard_Integer iS = 1; iS <= aMapShapes.Extent (); ++iS)
//!   {
//!     const TopoDS_Shape& aS = aMapShapes (iS);
//!     Bnd_Box aSBox;
//!     BRepBndLib::Add (aS, aSBox);
//!     aShapeBoxSet[i]->Add (aS, Bnd_Tools::Bnd2BVH (aSBox));
//!   }
//!   // Build BVH
//!   aShapeBoxSet[i]->Build();
//! }
//!
//! // Initialize selector
//! PairShapesSelector aSelector;
//! // Select the pairs of elements
//! aShapeBoxSet[0]->Select (aShapeBoxSet[1], aSelector);
//!
//! const NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >& aPairs = aSelector.Pairs ();
//! ~~~~
//! \tparam NumType Numeric data type
//! \tparam Dimension Vector dimension
//! \tparam DataType Type of elements on which the boxes are built
template <class NumType, int Dimension, class DataType>
class BVH_BoxSet : public BVH_PrimitiveSet <NumType, Dimension>
{
public: //! @name Constructors

  //! Empty constructor for use the default BVH_Builder
  BVH_BoxSet()
    : BVH_PrimitiveSet <NumType, Dimension> ()
  {
  }

  //! Constructor for usage the custom BVH builder
  BVH_BoxSet (const opencascade::handle <BVH_Builder <NumType, Dimension> >& theBuilder)
    : BVH_PrimitiveSet <NumType, Dimension> (theBuilder)
  {
  }

public: //! @name Adding elements in BVH

  //! Adds the element into BVH
  void Add (const DataType& theElement, const BVH_Box<NumType, Dimension>& theBox)
  {
    myElements.Append (theElement);
    myBoxes.Append (theBox);

    BVH_Object<NumType, Dimension>::myIsDirty = Standard_True;
  }

public: //! @name Clearing the elements

  //! Clears the vectors of elements and boxes
  void Clear()
  {
    myElements.Clear();
    myBoxes.Clear();
  }

public: //! @name BVH construction

  //! Constructs the BVH
  void Build()
  {
    BVH_PrimitiveSet<NumType, Dimension>::Update();
  }

public: //! @name Necessary overrides for BVH construction

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_PrimitiveSet <NumType, Dimension>::Box;

  //! Returns the bounding box with the given index (starting at 0).
  BVH_Box <NumType, Dimension> Box (const Standard_Integer theIndex) const Standard_OVERRIDE
  {
    return myBoxes (theIndex);
  }

  //! Returns centroid position along specified axis (starting at 0).
  Standard_Real Center (const Standard_Integer theIndex,
                        const Standard_Integer theAxis) const Standard_OVERRIDE
  {
    return myBoxes (theIndex).Center().GetData()[theAxis];
  }

  //! Returns the number of boxes.
  Standard_Integer Size() const Standard_OVERRIDE
  {
    return myBoxes.Length();
  }

  //! Swaps indices of two specified boxes.
  void Swap (const Standard_Integer theIndex1,
             const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    std::swap (myBoxes    (theIndex1), myBoxes    (theIndex2));
    std::swap (myElements (theIndex1), myElements (theIndex2));
  }

public: //! @name Selectors

  //! Abstract Elements Selector.
  //! Defines the rules for nodes rejection/acceptance.
  class Selector
  {
  public: //! @name Constructor

    //! Constructor
    Selector() {}

    //! Destructor
    virtual ~Selector () {}

  public: //! @name Rules for Accept/Reject

    //! Rejection of the node by bounding box
    virtual Standard_Boolean Reject (const BVH_Box<NumType, Dimension>&) const = 0;

    //! Leaf acceptance
    virtual Standard_Boolean Accept (const DataType&) = 0;
  };

  //! Abstract Pair elements Selector.
  //! Defines the rules for rejection/acceptance of the pair of nodes of two BVH trees
  class PairSelector
  {
  public: //! @name Constructor

    //! Constructor
    PairSelector() {}

    //! Destructor
    virtual ~PairSelector () {}

  public: //! @name Rules for Accept/Reject

    //! Rejection of the node by bounding boxes
    virtual Standard_Boolean Reject (const BVH_Box<NumType, Dimension>& theBox1,
                                     const BVH_Box<NumType, Dimension>& theBox2) const = 0;

    //! Leaf acceptance
    virtual Standard_Boolean Accept (const DataType&, const DataType&) = 0;
  };

public: //! @name Selection of the elements by tree descend

  //! Selection of the elements from the BVH tree by the
  //! rules defined in Selector.
  Standard_Integer Select (Selector& theSelector);

  //! Selection of the pairs of elements of two BVH trees by the
  //! rules defined in Selector.
  Standard_Integer Select (const opencascade::handle <BVH_BoxSet <NumType, Dimension, DataType> >& theOther,
                           PairSelector& theSelector);

protected: //! @name Fields

  NCollection_Vector <BVH_Box <NumType, Dimension> > myBoxes;    //!< Bounding boxes for the elements
  NCollection_Vector <DataType>                      myElements; //!< Elements
};


//=======================================================================
//function : Select
//purpose : Selection from the tree by tree descend
//=======================================================================
template <class NumType, int Dimension, class DataType>
Standard_Integer BVH_BoxSet <NumType, Dimension, DataType>::Select (Selector& theSelector)
{
  Standard_Integer aNbAccepted = 0;

  const opencascade::handle <BVH_Tree <NumType, Dimension> >& aBVH = this->myBVH;
  if (aBVH.IsNull())
    return aNbAccepted;

  const BVH_Array4i& aBVHNodes = aBVH->NodeInfoBuffer();

  NCollection_List <int> aNodes;
  aNodes.Append (0);

  // Starting with the root of the tree descend to the leafs.
  for (NCollection_List <int>::Iterator it (aNodes); it.More(); aNodes.Remove (it))
  {
    int iNode = it.Value();
    BVH_Box <NumType, Dimension> aBox (aBVH->MinPoint (iNode), aBVH->MaxPoint (iNode));
    if (theSelector.Reject (aBox))
      continue;

    const BVH_Vec4i& aNode = aBVHNodes[iNode];
    if (aNode.x() == 0)
    {
      // Inner node - add its children for treatment
      aNodes.Append (aNode.y());
      aNodes.Append (aNode.z());
    }
    else
    {
      // Outer node - apply the leaf node operation to each element
      for (Standard_Integer iN = aNode.y(); iN <= aNode.z(); ++iN)
      {
        if (!theSelector.Reject (myBoxes (iN)) && theSelector.Accept (myElements (iN)))
          ++aNbAccepted;
      }
    }
  }
  return aNbAccepted;
}

//=======================================================================
//function : Select
//purpose : Selection from the tree by parallel tree descend
//=======================================================================
template <class NumType, int Dimension, class DataType>
Standard_Integer BVH_BoxSet <NumType, Dimension, DataType>::Select
  (const opencascade::handle <BVH_BoxSet <NumType, Dimension, DataType> >& theOther,
   PairSelector& theSelector)
{
  Standard_Integer aNbAccepted = 0;
  if (theOther.IsNull())
    return aNbAccepted;

  const opencascade::handle <BVH_Tree <NumType, Dimension> >& aBVH1 = this->myBVH;
  const opencascade::handle <BVH_Tree <NumType, Dimension> >& aBVH2 = theOther->myBVH;
  if (aBVH1.IsNull() || aBVH2.IsNull())
    return aNbAccepted;

  const BVH_Array4i& aBVHNodes1 = aBVH1->NodeInfoBuffer();
  const BVH_Array4i& aBVHNodes2 = aBVH2->NodeInfoBuffer();

  NCollection_List <std::pair <int, int> > aNodes;
  aNodes.Append (std::pair <int, int> (0, 0));

  // Starting with the roots of the trees descend to the leafs.
  for (NCollection_List <std::pair <int, int> >::Iterator it (aNodes); it.More(); aNodes.Remove (it))
  {
    std::pair <int, int> aPair = it.Value();

    BVH_Box <NumType, Dimension> aBox1 (aBVH1->MinPoint (aPair.first),  aBVH1->MaxPoint (aPair.first));
    BVH_Box <NumType, Dimension> aBox2 (aBVH2->MinPoint (aPair.second), aBVH2->MaxPoint (aPair.second));
    if (theSelector.Reject (aBox1, aBox2))
      continue;

    const BVH_Vec4i& aNode1 = aBVHNodes1[aPair.first];
    const BVH_Vec4i& aNode2 = aBVHNodes2[aPair.second];

    if (aNode1.x() == 0 && aNode2.x() == 0)
    {
      // Inner/Inner
      aNodes.Append (std::pair <int, int> (aNode1.y(), aNode2.y()));
      aNodes.Append (std::pair <int, int> (aNode1.y(), aNode2.z()));
      aNodes.Append (std::pair <int, int> (aNode1.z(), aNode2.y()));
      aNodes.Append (std::pair <int, int> (aNode1.z(), aNode2.z()));
    }
    else if (aNode1.x() == 0)
    {
      // Inner/Outer
      aNodes.Append (std::pair <int, int> (aNode1.y(), aPair.second));
      aNodes.Append (std::pair <int, int> (aNode1.z(), aPair.second));
    }
    else if (aNode2.x() == 0)
    {
      // Outer/Inner
      aNodes.Append (std::pair <int, int> (aPair.first, aNode2.y()));
      aNodes.Append (std::pair <int, int> (aPair.first, aNode2.z()));
    }
    else
    {
      // Outer/Outer
      for (Standard_Integer iN1 = aNode1.y(); iN1 <= aNode1.z(); ++iN1)
      {
        for (Standard_Integer iN2 = aNode2.y (); iN2 <= aNode2.z (); ++iN2)
        {
          if (!theSelector.Reject (myBoxes (iN1), theOther->myBoxes (iN2)) &&
              theSelector.Accept (myElements (iN1), theOther->myElements (iN2)))
            ++aNbAccepted;
        }
      }
    }
  }
  return aNbAccepted;
}

#endif // _BVH_BoxSet_Header
