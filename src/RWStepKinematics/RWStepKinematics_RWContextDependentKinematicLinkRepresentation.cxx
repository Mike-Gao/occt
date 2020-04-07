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
#include <RWStepKinematics_RWContextDependentKinematicLinkRepresentation.hxx>
#include <StepKinematics_ContextDependentKinematicLinkRepresentation.hxx>
#include <StepKinematics_KinematicLinkRepresentationAssociation.hxx>
#include <StepKinematics_ProductDefinitionRelationshipKinematics.hxx>

//=======================================================================
//function : RWStepKinematics_RWContextDependentKinematicLinkRepresentation
//purpose  : 
//=======================================================================

RWStepKinematics_RWContextDependentKinematicLinkRepresentation::RWStepKinematics_RWContextDependentKinematicLinkRepresentation() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWContextDependentKinematicLinkRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                               const Standard_Integer num,
                                                                               Handle(Interface_Check)& ach,
                                                                               const Handle(StepKinematics_ContextDependentKinematicLinkRepresentation)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"context_dependent_kinematic_link_representation") ) return;

  // Own fields of ContextDependentKinematicLinkRepresentation

  Handle(StepKinematics_KinematicLinkRepresentationAssociation) aRepresentationRelation;
  data->ReadEntity (num, 1, "representation_relation", ach, STANDARD_TYPE(StepKinematics_KinematicLinkRepresentationAssociation), aRepresentationRelation);

  Handle(StepKinematics_ProductDefinitionRelationshipKinematics) aRepresentedProductRelation;
  data->ReadEntity (num, 2, "represented_product_relation", ach, STANDARD_TYPE(StepKinematics_ProductDefinitionRelationshipKinematics), aRepresentedProductRelation);

  // Initialize entity
  ent->Init(aRepresentationRelation,
            aRepresentedProductRelation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWContextDependentKinematicLinkRepresentation::WriteStep (StepData_StepWriter& SW,
                                                                                const Handle(StepKinematics_ContextDependentKinematicLinkRepresentation)& ent) const
{

  // Own fields of ContextDependentKinematicLinkRepresentation

  SW.Send (ent->RepresentationRelation());

  SW.Send (ent->RepresentedProductRelation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWContextDependentKinematicLinkRepresentation::Share (const Handle(StepKinematics_ContextDependentKinematicLinkRepresentation)& ent,
                                                                            Interface_EntityIterator& iter) const
{

  // Own fields of ContextDependentKinematicLinkRepresentation

  iter.AddItem (ent->RepresentationRelation());

  iter.AddItem (ent->RepresentedProductRelation());
}
