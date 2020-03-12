// Created on : Sat May 02 12:41:15 2020 
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
#include <RWStepKinematics_RWKinematicPropertyMechanismRepresentation.hxx>
#include <StepKinematics_KinematicPropertyMechanismRepresentation.hxx>
#include <StepRepr_RepresentedDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepKinematics_KinematicLinkRepresentation.hxx>

//=======================================================================
//function : RWStepKinematics_RWKinematicPropertyMechanismRepresentation
//purpose  : 
//=======================================================================

RWStepKinematics_RWKinematicPropertyMechanismRepresentation::RWStepKinematics_RWKinematicPropertyMechanismRepresentation() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWKinematicPropertyMechanismRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                            const Standard_Integer num,
                                                                            Handle(Interface_Check)& ach,
                                                                            const Handle(StepKinematics_KinematicPropertyMechanismRepresentation)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"kinematic_property_mechanism_representation") ) return;

  // Inherited fields of PropertyDefinitionRepresentation

  StepRepr_RepresentedDefinition aPropertyDefinitionRepresentation_Definition;
  data->ReadEntity (num, 1, "property_definition_representation.definition", ach, aPropertyDefinitionRepresentation_Definition);

  Handle(StepRepr_Representation) aPropertyDefinitionRepresentation_UsedRepresentation;
  data->ReadEntity (num, 2, "property_definition_representation.used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aPropertyDefinitionRepresentation_UsedRepresentation);

  // Own fields of KinematicPropertyMechanismRepresentation

  Handle(StepKinematics_KinematicLinkRepresentation) aBase;
  data->ReadEntity (num, 3, "base", ach, STANDARD_TYPE(StepKinematics_KinematicLinkRepresentation), aBase);

  // Initialize entity
  ent->Init(aPropertyDefinitionRepresentation_Definition,
            aPropertyDefinitionRepresentation_UsedRepresentation,
            aBase);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWKinematicPropertyMechanismRepresentation::WriteStep (StepData_StepWriter& SW,
                                                                             const Handle(StepKinematics_KinematicPropertyMechanismRepresentation)& ent) const
{

  // Own fields of PropertyDefinitionRepresentation

  SW.Send (ent->Definition().Value());

  SW.Send (ent->UsedRepresentation());

  // Own fields of KinematicPropertyMechanismRepresentation

  SW.Send (ent->Base());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWKinematicPropertyMechanismRepresentation::Share (const Handle(StepKinematics_KinematicPropertyMechanismRepresentation)& ent,
                                                                         Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());

  // Own fields of KinematicPropertyMechanismRepresentation

  iter.AddItem (ent->Base());
}
