// Created on: 2016-04-07
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

#ifndef _IMeshData_Face_HeaderFile
#define _IMeshData_Face_HeaderFile

#include <IMeshData_TessellatedShape.hxx>
#include <IMeshData_StatusOwner.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <IMeshData_Status.hxx>
#include <IMeshData_Types.hxx>
#include <BRepAdaptor_HSurface.hxx>

class IMeshData_Wire;
class TopoDS_Wire;

//! Interface class representing discrete model of a face.
//! Face model contains one or several wires.
//! First wire is always outer one.
class IMeshData_Face : public IMeshData_TessellatedShape, public IMeshData_StatusOwner
{
public:

  //! Destructor.
  Standard_EXPORT virtual ~IMeshData_Face()
  {
  }

  //! Returns number of wires.
  Standard_EXPORT virtual Standard_Integer WiresNb () const = 0;

  //! Adds wire to discrete model of face.
  Standard_EXPORT virtual const IMeshData::IWireHandle& AddWire (
    const TopoDS_Wire&     theWire,
    const Standard_Integer theEdgeNb = 0) = 0;

  //! Returns discrete edge with the given index.
  Standard_EXPORT virtual const IMeshData::IWireHandle& GetWire (
    const Standard_Integer theIndex) const = 0;

  //! Returns face's surface.
  inline const Handle(BRepAdaptor_HSurface)& GetSurface() const
  {
    return mySurface;
  }

  //! Returns TopoDS_Face attached to model.
  inline const TopoDS_Face& GetFace () const
  {
    return TopoDS::Face (GetShape ());
  }

  //! Returns whether the face discrete model is valid.
  inline Standard_Boolean IsValid () const
  {
    return (IsEqual(IMeshData_NoError) ||
            IsEqual(IMeshData_ReMesh)  ||
            IsEqual(IMeshData_UnorientedWire));
  }

  //! Dumps the content of me into the stream
  virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE
  {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
    OCCT_DUMP_BASE_CLASS (theOStream, theDepth, IMeshData_TessellatedShape)

    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, mySurface.get())
  }

  DEFINE_STANDARD_RTTI_INLINE(IMeshData_Face, IMeshData_TessellatedShape)

protected:

  //! Constructor.
  //! Initializes empty model.
  Standard_EXPORT IMeshData_Face (const TopoDS_Face& theFace)
    : IMeshData_TessellatedShape(theFace)
  {
    BRepAdaptor_Surface aSurfAdaptor(GetFace(), Standard_False);
    mySurface = new BRepAdaptor_HSurface(aSurfAdaptor);
  }

private:

  mutable Handle(BRepAdaptor_HSurface)  mySurface;
};

#endif