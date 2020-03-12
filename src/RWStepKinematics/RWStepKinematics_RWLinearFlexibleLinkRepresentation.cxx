// Created on : Fri May 08 19:02:07 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
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
#include <RWStepKinematics_RWLinearFlexibleLinkRepresentation.hxx>
#include <StepKinematics_LinearFlexibleLinkRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepKinematics_KinematicLink.hxx>

//=======================================================================
//function : RWStepKinematics_RWLinearFlexibleLinkRepresentation
//purpose  : 
//=======================================================================

RWStepKinematics_RWLinearFlexibleLinkRepresentation::RWStepKinematics_RWLinearFlexibleLinkRepresentation() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLinearFlexibleLinkRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                    const Standard_Integer num,
                                                                    Handle(Interface_Check)& ach,
                                                                    const Handle(StepKinematics_LinearFlexibleLinkRepresentation)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"linear_flexible_link_representation") ) return;

  // Inherited fields of Representation

  Handle(TCollection_HAsciiString) aRepresentation_Name;
  data->ReadString (num, 1, "representation.name", ach, aRepresentation_Name);

  Handle(StepRepr_HArray1OfRepresentationItem) aRepresentation_Items;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "representation.items", ach, sub2) ) {
    Standard_Integer nb0 = data->NbParams(sub2);
    aRepresentation_Items = new StepRepr_HArray1OfRepresentationItem (1, nb0);
    Standard_Integer num2 = sub2;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepRepr_RepresentationItem) anIt0;
      data->ReadEntity (num2, i0, "representation_item", ach, STANDARD_TYPE(StepRepr_RepresentationItem), anIt0);
      aRepresentation_Items->SetValue(i0, anIt0);
    }
  }

  Handle(StepRepr_RepresentationContext) aRepresentation_ContextOfItems;
  data->ReadEntity (num, 3, "representation.context_of_items", ach, STANDARD_TYPE(StepRepr_RepresentationContext), aRepresentation_ContextOfItems);

  // Inherited fields of KinematicLinkRepresentation

  Handle(StepKinematics_KinematicLink) aKinematicLinkRepresentation_RepresentedLink;
  data->ReadEntity (num, 4, "kinematic_link_representation.represented_link", ach, STANDARD_TYPE(StepKinematics_KinematicLink), aKinematicLinkRepresentation_RepresentedLink);

  // Initialize entity
  ent->Init(aRepresentation_Name,
            aRepresentation_Items,
            aRepresentation_ContextOfItems,
            aKinematicLinkRepresentation_RepresentedLink);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLinearFlexibleLinkRepresentation::WriteStep (StepData_StepWriter& SW,
                                                                     const Handle(StepKinematics_LinearFlexibleLinkRepresentation)& ent) const
{

  // Own fields of Representation

  SW.Send (ent->Name());

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->Items()->Length(); i1++ ) {
    Handle(StepRepr_RepresentationItem) Var0 = ent->Items()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();

  SW.Send (ent->ContextOfItems());

  // Own fields of KinematicLinkRepresentation

  SW.Send (ent->RepresentedLink());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLinearFlexibleLinkRepresentation::Share (const Handle(StepKinematics_LinearFlexibleLinkRepresentation)& ent,
                                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of Representation

  for (Standard_Integer i1=1; i1 <= ent->StepRepr_Representation::Items()->Length(); i1++ ) {
    Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
    iter.AddItem (Var0);
  }

  iter.AddItem (ent->StepRepr_Representation::ContextOfItems());

  // Inherited fields of KinematicLinkRepresentation

  iter.AddItem (ent->StepKinematics_KinematicLinkRepresentation::RepresentedLink());
}
