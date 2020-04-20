// Created on : Mon Apr 20 13:08:26 2020 
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
#include <RWStepKinematics_RWPairRepresentationRelationship.hxx>
#include <StepKinematics_PairRepresentationRelationship.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_RepresentationOrRepresentationReference.hxx>
#include <StepRepr_Transformation.hxx>
#include <StepRepr_Representation.hxx>

//=======================================================================
//function : RWStepKinematics_RWPairRepresentationRelationship
//purpose  : 
//=======================================================================

RWStepKinematics_RWPairRepresentationRelationship::RWStepKinematics_RWPairRepresentationRelationship() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPairRepresentationRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                  const Standard_Integer num,
                                                                  Handle(Interface_Check)& ach,
                                                                  const Handle(StepKinematics_PairRepresentationRelationship)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"pair_representation_relationship") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of RepresentationRelationship

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Name;
  data->ReadString (num, 2, "representation_relationship.name", ach, aRepresentationRelationship_Name);

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Description;
  Standard_Boolean hasRepresentationRelationship_Description = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "representation_relationship.description", ach, aRepresentationRelationship_Description);
  }
  else {
    hasRepresentationRelationship_Description = Standard_False;
    aRepresentationRelationship_Description.Nullify();
  }

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep1;
  data->ReadEntity (num, 4, "representation_relationship.rep1", ach, aRepresentationRelationship_Rep1);

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep2;
  data->ReadEntity (num, 5, "representation_relationship.rep2", ach, aRepresentationRelationship_Rep2);

  // Inherited fields of RepresentationRelationshipWithTransformation

  StepRepr_Transformation aRepresentationRelationshipWithTransformation_TransformationOperator;
  data->ReadEntity (num, 6, "representation_relationship_with_transformation.transformation_operator", ach, aRepresentationRelationshipWithTransformation_TransformationOperator);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aRepresentationRelationship_Name,
            hasRepresentationRelationship_Description,
            aRepresentationRelationship_Description,
            aRepresentationRelationship_Rep1,
            aRepresentationRelationship_Rep2,
            aRepresentationRelationshipWithTransformation_TransformationOperator);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPairRepresentationRelationship::WriteStep (StepData_StepWriter& SW,
                                                                   const Handle(StepKinematics_PairRepresentationRelationship)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Own fields of RepresentationRelationship

  SW.Send (ent->Name());

  if ( ent->RepresentationRelationshipWithTransformation()->HasDescription() ) {
    SW.Send(ent->RepresentationRelationshipWithTransformation()->Description());
  }
  else SW.SendUndef();

  SW.Send(ent->RepresentationRelationshipWithTransformation()->Rep1());

  SW.Send(ent->RepresentationRelationshipWithTransformation()->Rep2());

  // Inherited fields of RepresentationRelationshipWithTransformation

  SW.Send (ent->RepresentationRelationshipWithTransformation()->TransformationOperator().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPairRepresentationRelationship::Share (const Handle(StepKinematics_PairRepresentationRelationship)& ent,
                                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of RepresentationRelationship

  iter.AddItem(ent->RepresentationRelationshipWithTransformation()->Rep1());

  iter.AddItem(ent->RepresentationRelationshipWithTransformation()->Rep2());

  // Inherited fields of RepresentationRelationshipWithTransformation

  iter.AddItem (ent->RepresentationRelationshipWithTransformation()->TransformationOperator().Value());
}
