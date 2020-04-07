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
#include <RWStepKinematics_RWProductDefinitionRelationshipKinematics.hxx>
#include <StepKinematics_ProductDefinitionRelationshipKinematics.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>

//=======================================================================
//function : RWStepKinematics_RWProductDefinitionRelationshipKinematics
//purpose  : 
//=======================================================================

RWStepKinematics_RWProductDefinitionRelationshipKinematics::RWStepKinematics_RWProductDefinitionRelationshipKinematics() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWProductDefinitionRelationshipKinematics::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                           const Standard_Integer num,
                                                                           Handle(Interface_Check)& ach,
                                                                           const Handle(StepKinematics_ProductDefinitionRelationshipKinematics)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"product_definition_relationship_kinematics") ) return;

  // Inherited fields of PropertyDefinition

  Handle(TCollection_HAsciiString) aPropertyDefinition_Name;
  data->ReadString (num, 1, "property_definition.name", ach, aPropertyDefinition_Name);

  Handle(TCollection_HAsciiString) aPropertyDefinition_Description;
  Standard_Boolean hasPropertyDefinition_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "property_definition.description", ach, aPropertyDefinition_Description);
  }
  else {
    hasPropertyDefinition_Description = Standard_False;
    aPropertyDefinition_Description.Nullify();
  }

  StepRepr_CharacterizedDefinition aPropertyDefinition_Definition;
  data->ReadEntity (num, 3, "property_definition.definition", ach, aPropertyDefinition_Definition);

  // Initialize entity
  ent->Init(aPropertyDefinition_Name,
            hasPropertyDefinition_Description,
            aPropertyDefinition_Description,
            aPropertyDefinition_Definition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWProductDefinitionRelationshipKinematics::WriteStep (StepData_StepWriter& SW,
                                                                            const Handle(StepKinematics_ProductDefinitionRelationshipKinematics)& ent) const
{

  // Own fields of PropertyDefinition

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->Definition().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWProductDefinitionRelationshipKinematics::Share (const Handle(StepKinematics_ProductDefinitionRelationshipKinematics)& ent,
                                                                        Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinition

  iter.AddItem (ent->StepRepr_PropertyDefinition::Definition().Value());
}
