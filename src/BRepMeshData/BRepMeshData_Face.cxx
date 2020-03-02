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

#include <BRepMeshData_Face.hxx>
#include <BRepMeshData_Wire.hxx>
#include <BRepMesh_OrientedEdge.hxx>
#include <BRepMesh_Vertex.hxx>

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMeshData_Face::BRepMeshData_Face (
  const TopoDS_Face&                       theFace,
  const Handle (NCollection_IncAllocator)& theAllocator)
  : IMeshData_Face (theFace),
    myAllocator (theAllocator),
    myDWires (256, myAllocator)
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMeshData_Face::~BRepMeshData_Face ()
{
}

//=======================================================================
// Function: WiresNb
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_Face::WiresNb () const
{
  return myDWires.Size ();
}

//=======================================================================
// Function: AddWire
// Purpose : 
//=======================================================================
const IMeshData::IWireHandle& BRepMeshData_Face::AddWire (
  const TopoDS_Wire&     theWire,
  const Standard_Integer theEdgeNb)
{
  IMeshData::IWireHandle aWire (new (myAllocator) BRepMeshData_Wire (theWire, theEdgeNb, myAllocator));
  myDWires.Append (aWire);
  return GetWire (WiresNb () - 1);
}

//=======================================================================
// Function: GetWire
// Purpose : 
//=======================================================================
const IMeshData::IWireHandle& BRepMeshData_Face::GetWire (
  const Standard_Integer theIndex) const
{
  return myDWires (theIndex);
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void BRepMeshData_Face::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, IMeshData_Face)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDWires.Size())
  for (int aWireIt = 0; aWireIt < myDWires.Size(); aWireIt++)
  {
    const IMeshData::IWireHandle& aWire = GetWire (aWireIt);
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aWire.get())
  }
}
