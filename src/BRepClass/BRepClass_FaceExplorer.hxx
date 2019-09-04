// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepClass_FaceExplorer_HeaderFile
#define _BRepClass_FaceExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
#include <BVH_BoxSet.hxx>
class TopoDS_Face;
class gp_Pnt2d;
class gp_Lin2d;
class BRepClass_Edge;


//! Provide an   exploration of a  BRep Face   for the
//! classification. Return UV edges.
class BRepClass_FaceExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepClass_FaceExplorer(const TopoDS_Face& F);

  //! Checks the point and change its coords if it is located too far
  //! from the bounding box of the face. New Coordinates of the point 
  //! will be on the line between the point and the center of the 
  //! bounding box. Returns True if point was not changed.
  Standard_EXPORT Standard_Boolean CheckPoint (gp_Pnt2d& thePoint);
  
  //! Should  return  True  if the  point  is  outside a
  //! bounding volume of the face.
  Standard_EXPORT Standard_Boolean Reject (const gp_Pnt2d& P) const;
  
  //! Returns  in <L>, <Par>  a segment having at least
  //! one  intersection  with  the   face  boundary  to
  //! compute  intersections.
  Standard_EXPORT Standard_Boolean Segment (const gp_Pnt2d& P, gp_Lin2d& L, Standard_Real& Par);
  
  //! Returns  in <L>, <Par>  a segment having at least
  //! one  intersection  with  the   face  boundary  to
  //! compute  intersections. Each call gives another segment.
  Standard_EXPORT Standard_Boolean OtherSegment (const gp_Pnt2d& P, gp_Lin2d& L, Standard_Real& Par);
  
  //! Starts an exploration of the wires.
  Standard_EXPORT void InitWires();
  
  //! Returns True if there is  a current wire.
    Standard_Boolean MoreWires() const;
  
  //! Sets the explorer  to the  next  wire.
    void NextWire();
  
  //! Returns True  if the wire  bounding volume does not
  //! intersect the segment.
  Standard_EXPORT Standard_Boolean RejectWire (const gp_Lin2d& L, const Standard_Real Par) const;
  
  //! Starts an exploration of the  edges of the current
  //! wire.
  Standard_EXPORT void InitEdges();
  
  //! Returns True if there is a current edge.
    Standard_Boolean MoreEdges() const;
  
  //! Sets the explorer  to the  next  edge.
    void NextEdge();
  
  //! Returns True  if the edge  bounding volume does not
  //! intersect the segment.
  Standard_EXPORT Standard_Boolean RejectEdge (const gp_Lin2d& L, const Standard_Real Par) const;
  
  //! Current edge in current wire and its orientation.
  Standard_EXPORT void CurrentEdge (BRepClass_Edge& E, TopAbs_Orientation& Or) const;

  //! Returns the cached BVH_BoxSet.
  //! @param toBuild controls if the set should be constructed if it is null
  Standard_EXPORT const opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>>&
    BVHBoxSet (Standard_Boolean toBuild = Standard_True) const;

  //! Checks the On status of a point evaluated from surface for the face using the
  //! real tolerances of the sub-shapes of the face
  Standard_EXPORT Standard_Boolean IsPointOnFace (const gp_Pnt2d& thePoint) const;

  //! Checks the On status of a point for the face using the
  //! real tolerances of the sub-shapes of the face
  Standard_EXPORT Standard_Boolean IsPointOnFace (const gp_Pnt& thePoint) const;


protected:

  //! Computes UV bounds of a face
  Standard_EXPORT void ComputeFaceBounds();


private:



  TopoDS_Face myFace;
  TopExp_Explorer myWExplorer;
  TopExp_Explorer myEExplorer;
  Standard_Integer myCurEdgeInd;
  Standard_Real myCurEdgePar;

  Standard_Real myUMin;
  Standard_Real myUMax;
  Standard_Real myVMin;
  Standard_Real myVMax;

  mutable opencascade::handle <BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>> myBVHSet;
};


#include <BRepClass_FaceExplorer.lxx>





#endif // _BRepClass_FaceExplorer_HeaderFile
