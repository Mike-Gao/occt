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
#include <RWStepKinematics_RWLowOrderKinematicPairWithRange.hxx>
#include <StepKinematics_LowOrderKinematicPairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWLowOrderKinematicPairWithRange
//purpose  : 
//=======================================================================

RWStepKinematics_RWLowOrderKinematicPairWithRange::RWStepKinematics_RWLowOrderKinematicPairWithRange() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLowOrderKinematicPairWithRange::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                  const Standard_Integer num,
                                                                  Handle(Interface_Check)& ach,
                                                                  const Handle(StepKinematics_LowOrderKinematicPairWithRange)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,24,ach,"low_order_kinematic_pair_with_range") ) return;

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

  // Inherited fields of LowOrderKinematicPair

  Standard_Boolean aLowOrderKinematicPair_TX;
  data->ReadBoolean (num, 7, "low_order_kinematic_pair.t_x", ach, aLowOrderKinematicPair_TX);

  Standard_Boolean aLowOrderKinematicPair_TY;
  data->ReadBoolean (num, 8, "low_order_kinematic_pair.t_y", ach, aLowOrderKinematicPair_TY);

  Standard_Boolean aLowOrderKinematicPair_TZ;
  data->ReadBoolean (num, 9, "low_order_kinematic_pair.t_z", ach, aLowOrderKinematicPair_TZ);

  Standard_Boolean aLowOrderKinematicPair_RX;
  data->ReadBoolean (num, 10, "low_order_kinematic_pair.r_x", ach, aLowOrderKinematicPair_RX);

  Standard_Boolean aLowOrderKinematicPair_RY;
  data->ReadBoolean (num, 11, "low_order_kinematic_pair.r_y", ach, aLowOrderKinematicPair_RY);

  Standard_Boolean aLowOrderKinematicPair_RZ;
  data->ReadBoolean (num, 12, "low_order_kinematic_pair.r_z", ach, aLowOrderKinematicPair_RZ);

  // Own fields of LowOrderKinematicPairWithRange

  Standard_Real aLowerLimitActualRotationX;
  Standard_Boolean hasLowerLimitActualRotationX = Standard_True;
  if ( data->IsParamDefined (num,13) ) {
    data->ReadReal (num, 13, "lower_limit_actual_rotation_x", ach, aLowerLimitActualRotationX);
  }
  else {
    hasLowerLimitActualRotationX = Standard_False;
    aLowerLimitActualRotationX = 0;
  }

  Standard_Real aUpperLimitActualRotationX;
  Standard_Boolean hasUpperLimitActualRotationX = Standard_True;
  if ( data->IsParamDefined (num,14) ) {
    data->ReadReal (num, 14, "upper_limit_actual_rotation_x", ach, aUpperLimitActualRotationX);
  }
  else {
    hasUpperLimitActualRotationX = Standard_False;
    aUpperLimitActualRotationX = 0;
  }

  Standard_Real aLowerLimitActualRotationY;
  Standard_Boolean hasLowerLimitActualRotationY = Standard_True;
  if ( data->IsParamDefined (num,15) ) {
    data->ReadReal (num, 15, "lower_limit_actual_rotation_y", ach, aLowerLimitActualRotationY);
  }
  else {
    hasLowerLimitActualRotationY = Standard_False;
    aLowerLimitActualRotationY = 0;
  }

  Standard_Real aUpperLimitActualRotationY;
  Standard_Boolean hasUpperLimitActualRotationY = Standard_True;
  if ( data->IsParamDefined (num,16) ) {
    data->ReadReal (num, 16, "upper_limit_actual_rotation_y", ach, aUpperLimitActualRotationY);
  }
  else {
    hasUpperLimitActualRotationY = Standard_False;
    aUpperLimitActualRotationY = 0;
  }

  Standard_Real aLowerLimitActualRotationZ;
  Standard_Boolean hasLowerLimitActualRotationZ = Standard_True;
  if ( data->IsParamDefined (num,17) ) {
    data->ReadReal (num, 17, "lower_limit_actual_rotation_z", ach, aLowerLimitActualRotationZ);
  }
  else {
    hasLowerLimitActualRotationZ = Standard_False;
    aLowerLimitActualRotationZ = 0;
  }

  Standard_Real aUpperLimitActualRotationZ;
  Standard_Boolean hasUpperLimitActualRotationZ = Standard_True;
  if ( data->IsParamDefined (num,18) ) {
    data->ReadReal (num, 18, "upper_limit_actual_rotation_z", ach, aUpperLimitActualRotationZ);
  }
  else {
    hasUpperLimitActualRotationZ = Standard_False;
    aUpperLimitActualRotationZ = 0;
  }

  Standard_Real aLowerLimitActualTranslationX;
  Standard_Boolean hasLowerLimitActualTranslationX = Standard_True;
  if ( data->IsParamDefined (num,19) ) {
    data->ReadReal (num, 19, "lower_limit_actual_translation_x", ach, aLowerLimitActualTranslationX);
  }
  else {
    hasLowerLimitActualTranslationX = Standard_False;
    aLowerLimitActualTranslationX = 0;
  }

  Standard_Real aUpperLimitActualTranslationX;
  Standard_Boolean hasUpperLimitActualTranslationX = Standard_True;
  if ( data->IsParamDefined (num,20) ) {
    data->ReadReal (num, 20, "upper_limit_actual_translation_x", ach, aUpperLimitActualTranslationX);
  }
  else {
    hasUpperLimitActualTranslationX = Standard_False;
    aUpperLimitActualTranslationX = 0;
  }

  Standard_Real aLowerLimitActualTranslationY;
  Standard_Boolean hasLowerLimitActualTranslationY = Standard_True;
  if ( data->IsParamDefined (num,21) ) {
    data->ReadReal (num, 21, "lower_limit_actual_translation_y", ach, aLowerLimitActualTranslationY);
  }
  else {
    hasLowerLimitActualTranslationY = Standard_False;
    aLowerLimitActualTranslationY = 0;
  }

  Standard_Real aUpperLimitActualTranslationY;
  Standard_Boolean hasUpperLimitActualTranslationY = Standard_True;
  if ( data->IsParamDefined (num,22) ) {
    data->ReadReal (num, 22, "upper_limit_actual_translation_y", ach, aUpperLimitActualTranslationY);
  }
  else {
    hasUpperLimitActualTranslationY = Standard_False;
    aUpperLimitActualTranslationY = 0;
  }

  Standard_Real aLowerLimitActualTranslationZ;
  Standard_Boolean hasLowerLimitActualTranslationZ = Standard_True;
  if ( data->IsParamDefined (num,23) ) {
    data->ReadReal (num, 23, "lower_limit_actual_translation_z", ach, aLowerLimitActualTranslationZ);
  }
  else {
    hasLowerLimitActualTranslationZ = Standard_False;
    aLowerLimitActualTranslationZ = 0;
  }

  Standard_Real aUpperLimitActualTranslationZ;
  Standard_Boolean hasUpperLimitActualTranslationZ = Standard_True;
  if ( data->IsParamDefined (num,24) ) {
    data->ReadReal (num, 24, "upper_limit_actual_translation_z", ach, aUpperLimitActualTranslationZ);
  }
  else {
    hasUpperLimitActualTranslationZ = Standard_False;
    aUpperLimitActualTranslationZ = 0;
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aLowOrderKinematicPair_TX,
            aLowOrderKinematicPair_TY,
            aLowOrderKinematicPair_TZ,
            aLowOrderKinematicPair_RX,
            aLowOrderKinematicPair_RY,
            aLowOrderKinematicPair_RZ,
            hasLowerLimitActualRotationX,
            aLowerLimitActualRotationX,
            hasUpperLimitActualRotationX,
            aUpperLimitActualRotationX,
            hasLowerLimitActualRotationY,
            aLowerLimitActualRotationY,
            hasUpperLimitActualRotationY,
            aUpperLimitActualRotationY,
            hasLowerLimitActualRotationZ,
            aLowerLimitActualRotationZ,
            hasUpperLimitActualRotationZ,
            aUpperLimitActualRotationZ,
            hasLowerLimitActualTranslationX,
            aLowerLimitActualTranslationX,
            hasUpperLimitActualTranslationX,
            aUpperLimitActualTranslationX,
            hasLowerLimitActualTranslationY,
            aLowerLimitActualTranslationY,
            hasUpperLimitActualTranslationY,
            aUpperLimitActualTranslationY,
            hasLowerLimitActualTranslationZ,
            aLowerLimitActualTranslationZ,
            hasUpperLimitActualTranslationZ,
            aUpperLimitActualTranslationZ);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLowOrderKinematicPairWithRange::WriteStep (StepData_StepWriter& SW,
                                                                   const Handle(StepKinematics_LowOrderKinematicPairWithRange)& ent) const
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

  // Own fields of LowOrderKinematicPair

  SW.SendBoolean (ent->TX());

  SW.SendBoolean (ent->TY());

  SW.SendBoolean (ent->TZ());

  SW.SendBoolean (ent->RX());

  SW.SendBoolean (ent->RY());

  SW.SendBoolean (ent->RZ());

  // Own fields of LowOrderKinematicPairWithRange

  if ( ent->HasLowerLimitActualRotationX() ) {
    SW.Send (ent->LowerLimitActualRotationX());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitActualRotationX() ) {
    SW.Send (ent->UpperLimitActualRotationX());
  }
  else SW.SendUndef();

  if ( ent->HasLowerLimitActualRotationY() ) {
    SW.Send (ent->LowerLimitActualRotationY());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitActualRotationY() ) {
    SW.Send (ent->UpperLimitActualRotationY());
  }
  else SW.SendUndef();

  if ( ent->HasLowerLimitActualRotationZ() ) {
    SW.Send (ent->LowerLimitActualRotationZ());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitActualRotationZ() ) {
    SW.Send (ent->UpperLimitActualRotationZ());
  }
  else SW.SendUndef();

  if ( ent->HasLowerLimitActualTranslationX() ) {
    SW.Send (ent->LowerLimitActualTranslationX());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitActualTranslationX() ) {
    SW.Send (ent->UpperLimitActualTranslationX());
  }
  else SW.SendUndef();

  if ( ent->HasLowerLimitActualTranslationY() ) {
    SW.Send (ent->LowerLimitActualTranslationY());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitActualTranslationY() ) {
    SW.Send (ent->UpperLimitActualTranslationY());
  }
  else SW.SendUndef();

  if ( ent->HasLowerLimitActualTranslationZ() ) {
    SW.Send (ent->LowerLimitActualTranslationZ());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitActualTranslationZ() ) {
    SW.Send (ent->UpperLimitActualTranslationZ());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLowOrderKinematicPairWithRange::Share (const Handle(StepKinematics_LowOrderKinematicPairWithRange)& ent,
                                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (ent->StepKinematics_KinematicPair::Joint());

  // Inherited fields of LowOrderKinematicPair

  // Own fields of LowOrderKinematicPairWithRange
}
