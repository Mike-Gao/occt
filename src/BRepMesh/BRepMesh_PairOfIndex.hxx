// Created on: 2009-01-29
// Created by: Pavel TELKOV
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef BRepMesh_PairOfIndex_HeaderFile
#define BRepMesh_PairOfIndex_HeaderFile

#include <Standard_OutOfRange.hxx>

//! This class represents a pair of integer indices to store 
//! element indices connected to link. It is restricted to 
//! store more than two indices in it.
class BRepMesh_PairOfIndex
{
public:

  //! Default constructor
  BRepMesh_PairOfIndex()
  {
    Clear();
  }

  //! Clears indices.
  void Clear()
  {
    myIndex[0] = myIndex[1] = -1;
  }

  //! Appends index to the pair.
  inline void Append(const Standard_Integer theIndex)
  {
    if (myIndex[0] < 0)
      myIndex[0] = theIndex;
    else
    {
      if (myIndex[1] >= 0)
        throw Standard_OutOfRange("BRepMesh_PairOfIndex::Append, more than two index to store");

      myIndex[1] = theIndex;
    }
  }

  //! Prepends index to the pair.
  inline void Prepend(const Standard_Integer theIndex)
  {
    if (myIndex[1] >= 0)
      throw Standard_OutOfRange("BRepMesh_PairOfIndex::Prepend, more than two index to store");

    myIndex[1] = myIndex[0];
    myIndex[0] = theIndex;
  }

  //! Returns is pair is empty.
  inline Standard_Boolean IsEmpty() const
  {
    // Check only first index. It is impossible to update
    // second index if the first one is empty.
    return (myIndex[0] < 0);
  }

  //! Returns number of initialized indeces.
  inline Standard_Integer Extent() const
  {
    return (myIndex[0] < 0 ? 0 : (myIndex[1] < 0 ? 1 : 2));
  }

  //! Returns first index of pair.
  inline Standard_Integer FirstIndex() const
  {
    return myIndex[0];
  }

  //! Returns last index of pair
  inline Standard_Integer LastIndex() const
  {
    return (myIndex[1] < 0 ? myIndex[0] : myIndex[1]);
  }

  //! Returns index corresponding to the given position in the pair.
  //! @param thePairPos position of index in the pair (1 or 2).
  inline Standard_Integer Index(const Standard_Integer thePairPos) const
  {
    if (thePairPos != 1 && thePairPos != 2)
      throw Standard_OutOfRange("BRepMesh_PairOfIndex::Index, requested index is out of range");

    return myIndex[thePairPos - 1];
  }

  //! Sets index corresponding to the given position in the pair.
  //! @param thePairPos position of index in the pair (1 or 2).
  //! @param theIndex index to be stored.
  inline void SetIndex(const Standard_Integer thePairPos,
                       const Standard_Integer theIndex)
  {
    if (thePairPos != 1 && thePairPos != 2)
      throw Standard_OutOfRange("BRepMesh_PairOfIndex::SetIndex, requested index is out of range");

    myIndex[thePairPos - 1] = theIndex;
  }

  //! Remove index from the given position.
  //! @param thePairPos position of index in the pair (1 or 2).
  inline void RemoveIndex(const Standard_Integer thePairPos)
  {
    if (thePairPos != 1 && thePairPos != 2)
      throw Standard_OutOfRange("BRepMesh_PairOfIndex::RemoveIndex, requested index is out of range");

    if ( thePairPos == 1 )
      myIndex[0] = myIndex[1];

    myIndex[1] = -1;
  }

  //! Dumps the content of me into the stream
  virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
  {
    (void)theDepth;
    OCCT_DUMP_CLASS_BEGIN (theOStream, BRepMesh_PairOfIndex)

    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIndex[0])
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIndex[1])
  }

private:
  Standard_Integer myIndex[2];
};

#endif
