// Created on: 2016-10-13
// Created by: Alexander MALYSHEV
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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

#ifndef _BRepOffset_MakeSimpleOffset_HeaderFile
#define _BRepOffset_MakeSimpleOffset_HeaderFile

#include <NCollection_DataMap.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Real.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>

class BRepTools_Modifier;


enum BRepOffsetSimple_Status
{
  BRepOffsetSimple_OK,
  BRepOffsetSimple_NullInputShape,
  BRepOffsetSimple_ErrorOffsetComputation,
  BRepOffsetSimple_ErrorWallFaceComputation,
  BRepOffsetSimple_ErrorInvalidNbShells,
  BRepOffsetSimple_ErrorNonClosedShell
};

//! This class represents simple offset algorithm itself. It builds simple offset without intersection.
//! Solid can be created using SetBuildSolidFlag method (set flag to true). By default shell will be constructed.
//!
//! Algorithm:
//! 1. Build source-image maps for vertices, edges and faces.BRepTools_Modification class will be used
//!    to store this information. An image of a shared edge can be constructed from the corresponding edge
//!    of the first iterated face.
//! 2. Run BRepTools_Modifier to obtain offset shape.
//  3. Ensure topological integrity of the output shape.
//!
//! Limitations:
//! According to the algorithm nature result depends on the smoothness of input data. Smooth (G1-continuity) input shape
//! will lead to the good result.
class BRepOffset_MakeSimpleOffset
{
public:

  //! Constructor.
  Standard_EXPORT BRepOffset_MakeSimpleOffset(const TopoDS_Shape& theInputShape,
                                              const Standard_Real theOffsetValue);

  //! Computes offset shape.
  Standard_EXPORT void Perform();

  //! Gets error message.
  Standard_EXPORT TCollection_AsciiString GetErrorMessage() const;

  //! Gets error code.
  const BRepOffsetSimple_Status GetError() const { return myError; }

  // Inline methods.
  //! Gets solid building flag.
  const Standard_Boolean GetBuildSolidFlag() const { return myIsBuildSolid; }

  //! Sets solid building flag.
  void SetBuildSolidFlag(const Standard_Boolean theBuildFlag) { myIsBuildSolid = theBuildFlag; }

  //! Gets offset value.
  const Standard_Real GetOffsetValue() const { return myOffsetValue; }

  //! Sets offset value.
  void SetOffsetValue(const Standard_Real theOffsetValue) { myOffsetValue = theOffsetValue; }

  //! Gets done state.
  const Standard_Boolean IsDone() const { return myIsDone; } 

  //! Returns result shape.
  const TopoDS_Shape& GetResultShape() const { return myResShape; }

  //! Computes max safe offset value for the given tolerance.
  Standard_Real GetSafeOffset(const Standard_Real theExpectedToler);

protected:

  //! Computes max angle in faces junction.
  void ComputeMaxAngle();

  //! Clears previous result.
  void Clear();

private:

  //! Builds face on specified wall.
  TopoDS_Face BuildWallFace(const BRepTools_Modifier& theModifier,
                            const TopoDS_Edge& theOrigEdge);

  //! Builds missing walls.
  Standard_Boolean BuildMissingWalls(const BRepTools_Modifier& theModifier);

  // Input data.

  //! Input shape.
  TopoDS_Shape myInputShape;

  //! Offset value.
  Standard_Real myOffsetValue;

  //! Solid building flag. True means solid construction.
  Standard_Boolean myIsBuildSolid;

  // Internal data.

  //! Maximal angle in faces junction. This value helps to estimate result tolerance.
  Standard_Real myMaxAngle;

  //! Error message.
  BRepOffsetSimple_Status myError;

  //! Done state.
  Standard_Boolean myIsDone;

  //! Map of vertex - wall edge.
  //! Used to build shared edge between adjacent wall faces.
  NCollection_DataMap<TopoDS_Vertex, TopoDS_Edge> myMapVE;

  // Output data.

  //! Result shape.
  TopoDS_Shape myResShape;

};

#endif // _BRepOffset_MakeSimpleOffset_HeaderFile
