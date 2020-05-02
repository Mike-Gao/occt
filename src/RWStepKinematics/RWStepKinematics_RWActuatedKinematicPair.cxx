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
#include <RWStepKinematics_RWActuatedKinematicPair.hxx>
#include <StepKinematics_ActuatedKinematicPair.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepKinematics_ActuatedDirection.hxx>

//=======================================================================
//function : RWStepKinematics_RWActuatedKinematicPair
//purpose  : 
//=======================================================================

RWStepKinematics_RWActuatedKinematicPair::RWStepKinematics_RWActuatedKinematicPair() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWActuatedKinematicPair::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepKinematics_ActuatedKinematicPair)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,12,ach,"actuated_kinematic_pair") ) return;

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

  // Own fields of ActuatedKinematicPair

  StepKinematics_ActuatedDirection aTX = StepKinematics_adNotActuated;
  Standard_Boolean hasTX = Standard_True;
  if ( data->IsParamDefined (num,7) ) {
    if (data->ParamType (num, 7) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 7);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aTX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTX = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #7 (t_x) has not allowed value");
    }
    else ach->AddFail("Parameter #7 (t_x) is not enumeration");
  }
  else {
    hasTX = Standard_False;
  }

  StepKinematics_ActuatedDirection aTY = StepKinematics_adNotActuated;
  Standard_Boolean hasTY = Standard_True;
  if ( data->IsParamDefined (num,8) ) {
    if (data->ParamType (num, 8) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 8);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aTY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTY = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #8 (t_y) has not allowed value");
    }
    else ach->AddFail("Parameter #8 (t_y) is not enumeration");
  }
  else {
    hasTY = Standard_False;
  }

  StepKinematics_ActuatedDirection aTZ = StepKinematics_adNotActuated;
  Standard_Boolean hasTZ = Standard_True;
  if ( data->IsParamDefined (num,9) ) {
    if (data->ParamType (num, 9) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 9);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aTZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTZ = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #9 (t_z) has not allowed value");
    }
    else ach->AddFail("Parameter #9 (t_z) is not enumeration");
  }
  else {
    hasTZ = Standard_False;
  }

  StepKinematics_ActuatedDirection aRX = StepKinematics_adNotActuated;
  Standard_Boolean hasRX = Standard_True;
  if ( data->IsParamDefined (num,10) ) {
    if (data->ParamType (num, 10) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 10);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aRX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRX = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #10 (r_x) has not allowed value");
    }
    else ach->AddFail("Parameter #10 (r_x) is not enumeration");
  }
  else {
    hasRX = Standard_False;
  }

  StepKinematics_ActuatedDirection aRY = StepKinematics_adNotActuated;
  Standard_Boolean hasRY = Standard_True;
  if ( data->IsParamDefined (num,11) ) {
    if (data->ParamType (num, 11) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 11);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aRY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRY = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #11 (r_y) has not allowed value");
    }
    else ach->AddFail("Parameter #11 (r_y) is not enumeration");
  }
  else {
    hasRY = Standard_False;
  }

  StepKinematics_ActuatedDirection aRZ = StepKinematics_adNotActuated;
  Standard_Boolean hasRZ = Standard_True;
  if ( data->IsParamDefined (num,12) ) {
    if (data->ParamType (num, 12) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 12);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aRZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRZ = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #12 (r_z) has not allowed value");
    }
    else ach->AddFail("Parameter #12 (r_z) is not enumeration");
  }
  else {
    hasRZ = Standard_False;
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            hasTX,
            aTX,
            hasTY,
            aTY,
            hasTZ,
            aTZ,
            hasRX,
            aRX,
            hasRY,
            aRY,
            hasRZ,
            aRZ);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWActuatedKinematicPair::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepKinematics_ActuatedKinematicPair)& ent) const
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

  // Own fields of ActuatedKinematicPair

  if ( ent->HasTX() ) {
    switch (ent->TX()) {
      case StepKinematics_adBidirectional: SW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: SW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: SW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: SW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if ( ent->HasTY() ) {
    switch (ent->TY()) {
      case StepKinematics_adBidirectional: SW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: SW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: SW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: SW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if ( ent->HasTZ() ) {
    switch (ent->TZ()) {
      case StepKinematics_adBidirectional: SW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: SW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: SW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: SW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if ( ent->HasRX() ) {
    switch (ent->RX()) {
      case StepKinematics_adBidirectional: SW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: SW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: SW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: SW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if ( ent->HasRY() ) {
    switch (ent->RY()) {
      case StepKinematics_adBidirectional: SW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: SW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: SW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: SW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if ( ent->HasRZ() ) {
    switch (ent->RZ()) {
      case StepKinematics_adBidirectional: SW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: SW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: SW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: SW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWActuatedKinematicPair::Share (const Handle(StepKinematics_ActuatedKinematicPair)& ent,
                                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (ent->StepKinematics_KinematicPair::Joint());

  // Own fields of ActuatedKinematicPair
}
