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

#include <BRepMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <BRepMesh_OrientedEdge.hxx>
#include <BRepMesh_Vertex.hxx>

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMeshData_Wire::BRepMeshData_Wire (
  const TopoDS_Wire&                       theWire,
  const Standard_Integer                   theEdgeNb,
  const Handle (NCollection_IncAllocator)& theAllocator)
  : IMeshData_Wire (theWire),
    myDEdges    (theEdgeNb > 0 ? theEdgeNb : 256, theAllocator),
    myDEdgesOri (theEdgeNb > 0 ? theEdgeNb : 256, theAllocator)
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMeshData_Wire::~BRepMeshData_Wire ()
{
}

//=======================================================================
// Function: EdgesNb
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_Wire::EdgesNb () const
{
  return myDEdges.Size ();
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_Wire::AddEdge (
  const IMeshData::IEdgePtr& theDEdge,
  const TopAbs_Orientation   theOrientation)
{
  const Standard_Integer aIndex = EdgesNb ();

  myDEdges   .Append (theDEdge);
  myDEdgesOri.Append (theOrientation);

  return aIndex;
}

//=======================================================================
// Function: GetEdge
// Purpose : 
//=======================================================================
const IMeshData::IEdgePtr& BRepMeshData_Wire::GetEdge (
  const Standard_Integer theIndex) const
{
  return myDEdges (theIndex);
}

//=======================================================================
// Function: GetEdgeOrientation
// Purpose : 
//=======================================================================
TopAbs_Orientation BRepMeshData_Wire::GetEdgeOrientation (
  const Standard_Integer theIndex) const
{
  return myDEdgesOri (theIndex);
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void BRepMeshData_Wire::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, IMeshData_Wire)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDEdges.Size())
  for (int anEdgeId = 0; anEdgeId < myDEdges.Size(); anEdgeId++)
  {
    const IMeshData::IEdgePtr& anEdge = GetEdge (anEdgeId);
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, anEdge)
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDEdgesOri.Size())
  for (int anEdgeId = 0; anEdgeId < myDEdgesOri.Size(); anEdgeId++)
  {
    TopAbs_Orientation anEdgeOri = GetEdgeOrientation (anEdgeId);
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, &anEdgeOri)
  }
}
