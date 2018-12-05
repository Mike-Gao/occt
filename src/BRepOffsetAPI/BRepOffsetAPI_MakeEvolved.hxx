// Created on: 1995-09-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepOffsetAPI_MakeEvolved_HeaderFile
#define _BRepOffsetAPI_MakeEvolved_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_Evolved.hxx>
#include <BRepFill_Voluved.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <GeomAbs_JoinType.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Wire;
class TopoDS_Face;
class BRepFill_Evolved;
class TopoDS_Shape;


//! Describes functions to build evolved shapes.
//! An evolved shape is built from a planar spine (face or
//! wire) and a profile (wire). The evolved shape is the
//! unlooped sweep (pipe) of the profile along the spine.
//! Self-intersections are removed.
//! A MakeEvolved object provides a framework for:
//! - defining the construction of an evolved shape,
//! - implementing the construction algorithm, and
//! - consulting the result.
//! Computes an Evolved by
//! 1 - sweeping a profil along a spine.
//! 2 - removing the self-intersections.
//!
//! The profile is defined in a Referential R. The position of
//! the profile at the current point of the  spine is given by
//! confusing R  and the local  referential given by (  D0, D1
//! and the normal of the Spine)
//!
//! If the Boolean <AxeProf> is  true, R is  O,X,Y,Z
//! else R is defined as the local refential at the nearest
//! point of the profil to the spine.
//!
//! if <Solid> is TRUE the Shape result  is completed to be a
//! solid or a compound of solids.
class BRepOffsetAPI_MakeEvolved  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepOffsetAPI_MakeEvolved();
  
  Standard_EXPORT BRepOffsetAPI_MakeEvolved(const TopoDS_Wire& Spine, const TopoDS_Wire& Profil, const GeomAbs_JoinType Join/* = GeomAbs_Arc*/, const Standard_Boolean AxeProf/* = Standard_True*/, const Standard_Boolean Solid/* = Standard_False*/, const Standard_Boolean ProfOnSpine/* = Standard_False*/, const Standard_Boolean theIsVolume, const Standard_Real Tol/* = Precision::Confusion()*/, const Standard_Boolean theRunInParallel);
  
  //! These constructors construct an evolved shape by sweeping the profile
  //! Profile along the spine Spine.
  //! The profile is defined in a coordinate system R.
  //! The coordinate system is determined by AxeProf:
  //! - if AxeProf is true, R is the global coordinate system,
  //! - if AxeProf is false, R is computed so that:
  //! - its origin is given by the point on the spine which is
  //! closest to the profile,
  //! - its "X Axis" is given by the tangent to the spine at this point, and
  //! - its "Z Axis" is the normal to the plane which contains the spine.
  //! The position of the profile at the current point of the
  //! spine is given by making R coincident with the local
  //! coordinate system given by the current point, the
  //! tangent vector and the normal to the spine.
  //! Join defines the type of pipe generated by the salient
  //! vertices of the spine. The default type is GeomAbs_Arc
  //! where the vertices generate revolved pipes about the
  //! axis passing along the vertex and the normal to the
  //! plane of the spine. At present, this is the only
  //! construction type implemented.
  Standard_EXPORT BRepOffsetAPI_MakeEvolved(const TopoDS_Face& Spine, const TopoDS_Wire& Profil, const GeomAbs_JoinType Join/* = GeomAbs_Arc*/, const Standard_Boolean AxeProf/* = Standard_True*/, const Standard_Boolean Solid/* = Standard_False*/, const Standard_Boolean ProfOnSpine/* = Standard_False*/, const Standard_Boolean theIsVolume, const Standard_Real Tol/* = Precision::Confusion()*/, const Standard_Boolean theRunInParallel);
  
  Standard_EXPORT const BRepFill_Evolved& Evolved() const;
  
  //! Builds the resulting shape (redefined from MakeShape).
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;
  
  //! Returns   the  shapes  created  from   a  subshape
  //! <SpineShape>  of     the  spine   and   a subshape
  //! <ProfShape> on the profile.
  Standard_EXPORT const TopTools_ListOfShape& GeneratedShapes (const TopoDS_Shape& SpineShape, const TopoDS_Shape& ProfShape) const;
  
  //! Return the face Top if <Solid> is True in the constructor.
  Standard_EXPORT const TopoDS_Shape& Top() const;
  
  //! Return the face Bottom  if <Solid> is True in the constructor.
  Standard_EXPORT const TopoDS_Shape& Bottom() const;




protected:





private:


  BRepFill_Evolved myEvolved;
  BRepFill_Voluved myVolume;

};







#endif // _BRepOffsetAPI_MakeEvolved_HeaderFile
