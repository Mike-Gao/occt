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

#ifndef _BRepMesh_FaceIntersectionsSplitter_HeaderFile
#define _BRepMesh_FaceIntersectionsSplitter_HeaderFile

#include <BRepMesh_SegmentedFace.hxx>

//! Auxiliary class splitting intersecting segments of wires of target face.
//! Explodes wires of discrete face on sets of segments using tessellation 
//! data stored in model. Each segment is then checked for intersection with
//! other ones and split on intersection point.
class BRepMesh_FaceIntersectionsSplitter : public BRepMesh_SegmentedFace
{
public: //! @name mesher API

  //! Default constructor
  Standard_EXPORT BRepMesh_FaceIntersectionsSplitter(
    const IMeshData::IFaceHandle& theFace,
    const IMeshTools_Parameters&  theParameters);

  //! Destructor
  Standard_EXPORT virtual ~BRepMesh_FaceIntersectionsSplitter();

  //! Performs split of intersecting segments of wires of the face.
  //! @return True if there is no intersection, False elsewhere.
  Standard_EXPORT Standard_Boolean Perform();

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_FaceIntersectionsSplitter, BRepMesh_SegmentedFace)

private:

  //! Splits wire with the given index at first intersection point with others.
  Standard_Boolean perform(const Standard_Integer theWireIndex);

private:

  BRepMesh_FaceIntersectionsSplitter (const BRepMesh_FaceIntersectionsSplitter& theOther);

  void operator=(const BRepMesh_FaceIntersectionsSplitter& theOther);
};

#endif
