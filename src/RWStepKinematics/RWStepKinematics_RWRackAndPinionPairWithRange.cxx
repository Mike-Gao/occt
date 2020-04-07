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
#include <RWStepKinematics_RWRackAndPinionPairWithRange.hxx>
#include <StepKinematics_RackAndPinionPairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWRackAndPinionPairWithRange
//purpose  : 
//=======================================================================

RWStepKinematics_RWRackAndPinionPairWithRange::RWStepKinematics_RWRackAndPinionPairWithRange() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWRackAndPinionPairWithRange::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepKinematics_RackAndPinionPairWithRange)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,9,ach,"rack_and_pinion_pair_with_range") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of ItemDefinedTransformation

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Name;
  data->ReadString (num, 2, "item_defined_transformation.name", ach, aItemDefinedTransformation_Name);

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Description;
  Standard_Boolean hasItemDefinedTransformation_Description = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "item_defined_transformation.description", ach, aItemDefinedTransformation_Description);
  }
  else {
    hasItemDefinedTransformation_Description = Standard_False;
    aItemDefinedTransformation_Description.Nullify();
  }

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem1;
  data->ReadEntity (num, 4, "item_defined_transformation.transform_item1", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem1);

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem2;
  data->ReadEntity (num, 5, "item_defined_transformation.transform_item2", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem2);

  // Inherited fields of KinematicPair

  Handle(StepKinematics_KinematicJoint) aKinematicPair_Joint;
  data->ReadEntity (num, 6, "kinematic_pair.joint", ach, STANDARD_TYPE(StepKinematics_KinematicJoint), aKinematicPair_Joint);

  // Inherited fields of RackAndPinionPair

  Standard_Real aRackAndPinionPair_PinionRadius;
  data->ReadReal (num, 7, "rack_and_pinion_pair.pinion_radius", ach, aRackAndPinionPair_PinionRadius);

  // Own fields of RackAndPinionPairWithRange

  Standard_Real aLowerLimitRackDisplacement;
  Standard_Boolean hasLowerLimitRackDisplacement = Standard_True;
  if ( data->IsParamDefined (num,8) ) {
    data->ReadReal (num, 8, "lower_limit_rack_displacement", ach, aLowerLimitRackDisplacement);
  }
  else {
    hasLowerLimitRackDisplacement = Standard_False;
    aLowerLimitRackDisplacement = 0;
  }

  Standard_Real aUpperLimitRackDisplacement;
  Standard_Boolean hasUpperLimitRackDisplacement = Standard_True;
  if ( data->IsParamDefined (num,9) ) {
    data->ReadReal (num, 9, "upper_limit_rack_displacement", ach, aUpperLimitRackDisplacement);
  }
  else {
    hasUpperLimitRackDisplacement = Standard_False;
    aUpperLimitRackDisplacement = 0;
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aRackAndPinionPair_PinionRadius,
            hasLowerLimitRackDisplacement,
            aLowerLimitRackDisplacement,
            hasUpperLimitRackDisplacement,
            aUpperLimitRackDisplacement);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWRackAndPinionPairWithRange::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepKinematics_RackAndPinionPairWithRange)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Inherited fields of ItemDefinedTransformation

  SW.Send (ent->ItemDefinedTransformation()->Name());

  if ( ent->ItemDefinedTransformation()->HasDescription() ) {
    SW.Send (ent->ItemDefinedTransformation()->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->ItemDefinedTransformation()->TransformItem1());

  SW.Send (ent->ItemDefinedTransformation()->TransformItem2());

  // Own fields of KinematicPair

  SW.Send (ent->Joint());

  // Own fields of RackAndPinionPair

  SW.Send (ent->PinionRadius());

  // Own fields of RackAndPinionPairWithRange

  if ( ent->HasLowerLimitRackDisplacement() ) {
    SW.Send (ent->LowerLimitRackDisplacement());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitRackDisplacement() ) {
    SW.Send (ent->UpperLimitRackDisplacement());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWRackAndPinionPairWithRange::Share (const Handle(StepKinematics_RackAndPinionPairWithRange)& ent,
                                                           Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (ent->StepKinematics_KinematicPair::Joint());

  // Inherited fields of RackAndPinionPair

  // Own fields of RackAndPinionPairWithRange
}
