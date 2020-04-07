// Created on : Mon Apr 13 15:22:02 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2020
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

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <RWStepKinematics_RWOrientedJoint.hxx>
#include <StepKinematics_OrientedJoint.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_Edge.hxx>
#include <Standard_Boolean.hxx>

//=======================================================================
//function : RWStepKinematics_RWOrientedJoint
//purpose  : 
//=======================================================================

RWStepKinematics_RWOrientedJoint::RWStepKinematics_RWOrientedJoint() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWOrientedJoint::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepKinematics_OrientedJoint)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"oriented_joint") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of Edge

  Handle(StepShape_Vertex) aEdge_EdgeStart;
  data->ReadEntity (num, 2, "edge.edge_start", ach, STANDARD_TYPE(StepShape_Vertex), aEdge_EdgeStart);

  Handle(StepShape_Vertex) aEdge_EdgeEnd;
  data->ReadEntity (num, 3, "edge.edge_end", ach, STANDARD_TYPE(StepShape_Vertex), aEdge_EdgeEnd);

  // Inherited fields of OrientedEdge

  Handle(StepShape_Edge) aOrientedEdge_EdgeElement;
  data->ReadEntity (num, 4, "oriented_edge.edge_element", ach, STANDARD_TYPE(StepShape_Edge), aOrientedEdge_EdgeElement);

  Standard_Boolean aOrientedEdge_Orientation;
  data->ReadBoolean (num, 5, "oriented_edge.orientation", ach, aOrientedEdge_Orientation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aOrientedEdge_EdgeElement,
            aOrientedEdge_Orientation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWOrientedJoint::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepKinematics_OrientedJoint)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Own fields of Edge

  SW.Send (ent->EdgeStart());

  SW.Send (ent->EdgeEnd());

  // Own fields of OrientedEdge

  SW.Send (ent->EdgeElement());

  SW.SendBoolean (ent->Orientation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWOrientedJoint::Share (const Handle(StepKinematics_OrientedJoint)& ent,
                                              Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of Edge

  iter.AddItem (ent->StepShape_Edge::EdgeStart());

  iter.AddItem (ent->StepShape_Edge::EdgeEnd());

  // Inherited fields of OrientedEdge

  iter.AddItem (ent->StepShape_OrientedEdge::EdgeElement());
}
