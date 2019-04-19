// Created on: 2019-04-11
// Created by: Timur Izmaylov
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

#ifndef _StdPrs_SectionLines_HeaderFile
#define _StdPrs_SectionLines_HeaderFile

#include <Standard_Handle.hxx>

class Graphic3d_ArrayOfPrimitives;
class TopoDS_Shape;
class gp_Pln;
class TopoDS_Compound;

//! Tool for computing a cross section of a TopoDS_Shape with some plane.
class StdPrs_SectionLines
{
public:
  //! Computes a cross section of the shape with the plane
  //! @param theShape the shape consisting of triangulated faces
  //! @param theSectionPlane the plane that intersects the shape
  //! @return a resulting shape of a cross section
  Standard_EXPORT static TopoDS_Compound CrossSectionAsShape (const TopoDS_Shape& theShape,
                                                              const gp_Pln&       theSectionPlane);

  //! Computes a cross section of the shape with the plane
  //! @param theShape the shape consisting of triangulated faces
  //! @param theSectionPlane the plane that intersects the shape
  //! @return a resulting list of segments
  Standard_EXPORT static Handle (Graphic3d_ArrayOfPrimitives)
    CrossSectionAsArrayOfSegments (const TopoDS_Shape& theShape, const gp_Pln& theSectionPlane);

  //! Computes a cross section of the shape with the plane
  //! @param theShape the shape consisting of triangulated faces
  //! @param theSectionPlane the plane that intersects the shape
  //! @param theArrayOfSectionTriangles a resulting list of triangles
  //! @param theArrayOfSectionSegments a resulting list of line segments
  Standard_EXPORT static void CrossSectionAsArraysOfTrianglesAndSegments (const TopoDS_Shape& theShape,
                                                                          const gp_Pln&       theSectionPlane,
                                                                          Handle (Graphic3d_ArrayOfPrimitives)
                                                                            & theArrayOfSectionTriangles,
                                                                          Handle (Graphic3d_ArrayOfPrimitives)
                                                                            & theArrayOfSectionSegments);
};

#endif // _StdPrs_SectionLines_HeaderFile
