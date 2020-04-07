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
#include <RWStepKinematics_RWKinematicLinkRepresentationAssociation.hxx>
#include <StepKinematics_KinematicLinkRepresentationAssociation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationOrRepresentationReference.hxx>
#include <StepRepr_Representation.hxx>

//=======================================================================
//function : RWStepKinematics_RWKinematicLinkRepresentationAssociation
//purpose  : 
//=======================================================================

RWStepKinematics_RWKinematicLinkRepresentationAssociation::RWStepKinematics_RWKinematicLinkRepresentationAssociation() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWKinematicLinkRepresentationAssociation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                          const Standard_Integer num,
                                                                          Handle(Interface_Check)& ach,
                                                                          const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"kinematic_link_representation_association") ) return;

  // Inherited fields of RepresentationRelationship

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Name;
  data->ReadString (num, 1, "representation_relationship.name", ach, aRepresentationRelationship_Name);

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Description;
  Standard_Boolean hasRepresentationRelationship_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "representation_relationship.description", ach, aRepresentationRelationship_Description);
  }
  else {
    hasRepresentationRelationship_Description = Standard_False;
    aRepresentationRelationship_Description.Nullify();
  }

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep1;
  data->ReadEntity (num, 3, "representation_relationship.rep1", ach, aRepresentationRelationship_Rep1);

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep2;
  data->ReadEntity (num, 4, "representation_relationship.rep2", ach, aRepresentationRelationship_Rep2);

  // Process only one type (Representaion)
  if (aRepresentationRelationship_Rep1.CaseNumber() != 1 ||
      aRepresentationRelationship_Rep1.CaseNumber() != 1)
    return;

  // Initialize entity
  ent->Init(aRepresentationRelationship_Name,
            /*hasRepresentationRelationship_Description,*/
            aRepresentationRelationship_Description,
            aRepresentationRelationship_Rep1.Representation(),
            aRepresentationRelationship_Rep2.Representation());
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWKinematicLinkRepresentationAssociation::WriteStep (StepData_StepWriter& SW,
                                                                           const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& ent) const
{

  // Own fields of RepresentationRelationship

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->Rep1());

  SW.Send (ent->Rep2());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWKinematicLinkRepresentationAssociation::Share (const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& ent,
                                                                       Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationRelationship

  iter.AddItem (ent->StepRepr_RepresentationRelationship::Rep1());

  iter.AddItem (ent->StepRepr_RepresentationRelationship::Rep2());
}
