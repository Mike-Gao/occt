// Created on: 2020-05-26
// Created by: PASUKHIN DMITRY
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepKinematics_RWActuatedKinPairAndOrderKinPair.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_ActuatedKinPairAndOrderKinPair.hxx>
#include <StepKinematics_ActuatedDirection.hxx>
#include <StepKinematics_ActuatedKinematicPair.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepKinematics_CylindricalPair.hxx>
#include <StepKinematics_CylindricalPairWithRange.hxx>
#include <StepKinematics_FullyConstrainedPair.hxx>
#include <StepKinematics_GearPair.hxx>
#include <StepKinematics_GearPairWithRange.hxx>
#include <StepKinematics_HomokineticPair.hxx>
#include <StepKinematics_KinematicTopologyStructure.hxx>
#include <StepKinematics_LowOrderKinematicPair.hxx>
#include <StepKinematics_LowOrderKinematicPairWithRange.hxx>
#include <StepKinematics_PlanarCurvePair.hxx>
#include <StepKinematics_PlanarCurvePairRange.hxx>
#include <StepKinematics_PlanarPair.hxx>
#include <StepKinematics_PlanarPairWithRange.hxx>
#include <StepKinematics_PointOnPlanarCurvePair.hxx>
#include <StepKinematics_PointOnPlanarCurvePairWithRange.hxx>
#include <StepKinematics_PointOnSurfacePair.hxx>
#include <StepKinematics_PointOnSurfacePairWithRange.hxx>
#include <StepKinematics_PrismaticPair.hxx>
#include <StepKinematics_PrismaticPairWithRange.hxx>
#include <StepKinematics_ProductDefinitionKinematics.hxx>
#include <StepKinematics_RackAndPinionPair.hxx>
#include <StepKinematics_RackAndPinionPairWithRange.hxx>
#include <StepKinematics_RevolutePair.hxx>
#include <StepKinematics_RevolutePairWithRange.hxx>
#include <StepKinematics_RollingCurvePair.hxx>
#include <StepKinematics_RollingSurfacePair.hxx>
#include <StepKinematics_ScrewPair.hxx>
#include <StepKinematics_ScrewPairWithRange.hxx>
#include <StepKinematics_SlidingCurvePair.hxx>
#include <StepKinematics_SlidingSurfacePair.hxx>
#include <StepKinematics_SphericalPair.hxx>
#include <StepKinematics_SphericalPairWithPin.hxx>
#include <StepKinematics_SphericalPairWithPinAndRange.hxx>
#include <StepKinematics_SphericalPairWithRange.hxx>
#include <StepKinematics_SurfacePairWithRange.hxx>
#include <StepKinematics_UnconstrainedPair.hxx>
#include <StepKinematics_UnconstrainedPairValue.hxx>
#include <StepKinematics_UniversalPair.hxx>
#include <StepKinematics_UniversalPairWithRange.hxx>

//=======================================================================
//function : RWStepDimTol_RWGeoTolAndGeoTolWthMod
//purpose  : 
//=======================================================================
RWStepKinematics_RWActuatedKinPairAndOrderKinPair::RWStepKinematics_RWActuatedKinPairAndOrderKinPair()
{
}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWActuatedKinPairAndOrderKinPair::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num0, Handle(Interface_Check)& ach,
   const Handle(StepKinematics_ActuatedKinPairAndOrderKinPair)& ent) const
{
  Standard_Integer num = 0;//num0;
  data->NamedForComplex("ACTUATED_KINEMATIC_PAIR", "ACKNPR", num0, num, ach);
  if (!data->CheckNbParams(num,6,ach,"kinematic actuated_kinematic_pair")) return;
  // Own fields of ActuatedKinematicPair
  Handle(StepKinematics_ActuatedKinematicPair) anActuatedKinematicPair =
    new StepKinematics_ActuatedKinematicPair;
  StepKinematics_ActuatedDirection aTX = StepKinematics_adNotActuated;
  Standard_Boolean hasTX = Standard_True;
  if (data->IsParamDefined(num, 1)) {
    if (data->ParamType(num, 1) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 1);
      if (strcmp(text, ".BIDIRECTIONAL.")) aTX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTX = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #1 (t_x) has not allowed value");
    }
    else ach->AddFail("Parameter #1 (t_x) is not enumeration");
  }
  else {
    hasTX = Standard_False;
  }

  StepKinematics_ActuatedDirection aTY = StepKinematics_adNotActuated;
  Standard_Boolean hasTY = Standard_True;
  if (data->IsParamDefined(num, 2)) {
    if (data->ParamType(num, 2) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 8);
      if (strcmp(text, ".BIDIRECTIONAL.")) aTY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTY = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #2 (t_y) has not allowed value");
    }
    else ach->AddFail("Parameter #2 (t_y) is not enumeration");
  }
  else {
    hasTY = Standard_False;
  }

  StepKinematics_ActuatedDirection aTZ = StepKinematics_adNotActuated;
  Standard_Boolean hasTZ = Standard_True;
  if (data->IsParamDefined(num, 3)) {
    if (data->ParamType(num, 3) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 9);
      if (strcmp(text, ".BIDIRECTIONAL.")) aTZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTZ = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #3 (t_z) has not allowed value");
    }
    else ach->AddFail("Parameter #3 (t_z) is not enumeration");
  }
  else {
    hasTZ = Standard_False;
  }

  StepKinematics_ActuatedDirection aRX = StepKinematics_adNotActuated;
  Standard_Boolean hasRX = Standard_True;
  if (data->IsParamDefined(num, 4)) {
    if (data->ParamType(num, 4) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 10);
      if (strcmp(text, ".BIDIRECTIONAL.")) aRX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRX = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #14 (r_x) has not allowed value");
    }
    else ach->AddFail("Parameter #14 (r_x) is not enumeration");
  }
  else {
    hasRX = Standard_False;
  }

  StepKinematics_ActuatedDirection aRY = StepKinematics_adNotActuated;
  Standard_Boolean hasRY = Standard_True;
  if (data->IsParamDefined(num, 5)) {
    if (data->ParamType(num, 5) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 11);
      if (strcmp(text, ".BIDIRECTIONAL.")) aRY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRY = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #5 (r_y) has not allowed value");
    }
    else ach->AddFail("Parameter #5 (r_y) is not enumeration");
  }
  else {
    hasRY = Standard_False;
  }

  StepKinematics_ActuatedDirection aRZ = StepKinematics_adNotActuated;
  Standard_Boolean hasRZ = Standard_True;
  if (data->IsParamDefined(num, 6)) {
    if (data->ParamType(num, 6) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num, 12);
      if (strcmp(text, ".BIDIRECTIONAL.")) aRZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRZ = StepKinematics_adNotActuated;
      else ach->AddFail("Parameter #6 (r_z) has not allowed value");
    }
    else ach->AddFail("Parameter #6 (r_z) is not enumeration");
  }
  else {
    hasRZ = Standard_False;
  }

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->NamedForComplex("GEOMETRIC_REPRESENTATION_ITEM", "GMRPIT", num0, num, ach);
  //geometricrepresentation_item is non-defined

  data->NamedForComplex("ITEM_DEFINED_TRANSFORMATION", "ITDFTR", num0, num, ach);
  // Own fields of item_defined_transformation

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Name;
  if (strcmp(data->ParamCValue(num, 1), "*"))
    data->ReadString(num, 1, "item_defined_transformation.name", ach, aItemDefinedTransformation_Name);

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Description;
  Standard_Boolean hasItemDefinedTransformation_Description = Standard_True;
  if (data->IsParamDefined(num, 2)) {
    data->ReadString(num, 2, "item_defined_transformation.description", ach, aItemDefinedTransformation_Description);
  }
  else {
    hasItemDefinedTransformation_Description = Standard_False;
    aItemDefinedTransformation_Description.Nullify();
  }

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem1;
  data->ReadEntity(num, 3, "ITEM_DEFINED_TRANSFORMATION", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem1);

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem2;
  data->ReadEntity(num, 4, "ITEM_DEFINED_TRANSFORMATION", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem2); 

  data->NamedForComplex("KINEMATIC_PAIR", "KNMPR", num0, num, ach);

  // Inherited fields of KinematicPair

  Handle(StepKinematics_KinematicJoint) aKinematicPair_Joint;
  data->ReadEntity(num, 1, "kinematic_pair.joint", ach, STANDARD_TYPE(StepKinematics_KinematicJoint), aKinematicPair_Joint);

  //Select pair supertype 
  num = data->NextForComplex(num);
  Standard_CString aPairSuperType = data->CType(num);
  Handle(StepKinematics_KinematicPair) aKinematicPair;
  if (!strcmp(aPairSuperType, "LOW_ORDER_KINEMATIC_PAIR"))
  {
    // Own fields of low_order_kinematic_pair
    Standard_Boolean aLowOrderKinematicPair_TX = Standard_True;
    Standard_Boolean aLowOrderKinematicPair_TY = Standard_True;
    Standard_Boolean aLowOrderKinematicPair_TZ = Standard_True;
    Standard_Boolean aLowOrderKinematicPair_RX = Standard_True;
    Standard_Boolean aLowOrderKinematicPair_RY = Standard_True;
    Standard_Boolean aLowOrderKinematicPair_RZ = Standard_True;
    if (data->CheckNbParams(num, 6, ach, "LOW_ORDER_KINEMATIC_PAIR"))
    {
      if (strcmp(data->ParamCValue(num, 1), "*"))
      data->ReadBoolean(num, 1, "low_order_kinematic_pair.t_x", ach, aLowOrderKinematicPair_TX);
      if (strcmp(data->ParamCValue(num, 2), "*"))
      data->ReadBoolean(num, 2, "low_order_kinematic_pair.t_y", ach, aLowOrderKinematicPair_TY);
      if (strcmp(data->ParamCValue(num, 3), "*"))
      data->ReadBoolean(num, 3, "low_order_kinematic_pair.t_z", ach, aLowOrderKinematicPair_TZ);
      if (strcmp(data->ParamCValue(num, 4), "*"))
      data->ReadBoolean(num, 4, "low_order_kinematic_pair.r_x", ach, aLowOrderKinematicPair_RX);
      if (strcmp(data->ParamCValue(num, 5), "*"))
      data->ReadBoolean(num, 5, "low_order_kinematic_pair.r_y", ach, aLowOrderKinematicPair_RY);
      if (strcmp(data->ParamCValue(num, 6), "*"))
      data->ReadBoolean(num, 6, "low_order_kinematic_pair.r_z", ach, aLowOrderKinematicPair_RZ);
    }
    else ach->AddFail("The supertype of kinematic pair is not supported");
    aKinematicPair = new StepKinematics_KinematicPair;
    aKinematicPair->Init(aRepresentationItem_Name,
      aItemDefinedTransformation_Name, hasItemDefinedTransformation_Description,
      aItemDefinedTransformation_Description,
      aItemDefinedTransformation_TransformItem1,
      aItemDefinedTransformation_TransformItem2,
      aKinematicPair_Joint);
    num = data->NextForComplex(num);
    Standard_CString aPairtype = data->CType(num);
    if (!strcmp(aPairtype,"REVOLUTE_PAIR"))
    {
      // Own fields of revolute_pair is non-defined

      if (data->NamedForComplex("REVOLUTE_PAIR_WITH_RANGE", "RPWR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 2, ach, "REVOLUTE_PAIR_WITH_RANGE"))
        {
          // Own fields of revolute_pair_with_range
          aKinematicPair = new StepKinematics_RevolutePairWithRange;
          Standard_Real aLowerLimitActualRotation;
          Standard_Boolean hasLowerLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_actual_rotation", ach, aLowerLimitActualRotation);
          }
          else {
            hasLowerLimitActualRotation = Standard_False;
            aLowerLimitActualRotation = 0;
          }

          Standard_Real aUpperLimitActualRotation;
          Standard_Boolean hasUpperLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_actual_rotation", ach, aUpperLimitActualRotation);
          }
          else {
            hasUpperLimitActualRotation = Standard_False;
            aUpperLimitActualRotation = 0;
          }
          Handle(StepKinematics_RevolutePairWithRange) aLocalPair = Handle(StepKinematics_RevolutePairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasLowerLimitActualRotation,
            aLowerLimitActualRotation,
            hasUpperLimitActualRotation,
            aUpperLimitActualRotation);
        }
        else // revolute_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_RevolutePair;
          Handle(StepKinematics_RevolutePair) aLocalPair = Handle(StepKinematics_RevolutePair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "CYLINDRICAL_PAIR"))
    {
      // Own fields of cylindrical_pair is non-defined

      if (data->NamedForComplex("CYLINDRICAL_PAIR_WITH_RANGE", "CPW0", num0, num, ach))
      {
        if (data->CheckNbParams(num, 4, ach, "CYLINDRICAL_PAIR_WITH_RANGE"))
        {
          // Own fields of cylindrical_pair_with_range
          aKinematicPair = new StepKinematics_CylindricalPairWithRange;

          Standard_Real aLowerLimitActualTranslation;
          Standard_Boolean hasLowerLimitActualTranslation = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_actual_translation", ach, aLowerLimitActualTranslation);
          }
          else {
            hasLowerLimitActualTranslation = Standard_False;
            aLowerLimitActualTranslation = 0;
          }

          Standard_Real aUpperLimitActualTranslation;
          Standard_Boolean hasUpperLimitActualTranslation = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_actual_translation", ach, aUpperLimitActualTranslation);
          }
          else {
            hasUpperLimitActualTranslation = Standard_False;
            aUpperLimitActualTranslation = 0;
          }

          Standard_Real aLowerLimitActualRotation;
          Standard_Boolean hasLowerLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "lower_limit_actual_rotation", ach, aLowerLimitActualRotation);
          }
          else {
            hasLowerLimitActualRotation = Standard_False;
            aLowerLimitActualRotation = 0;
          }

          Standard_Real aUpperLimitActualRotation;
          Standard_Boolean hasUpperLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 4)) {
            data->ReadReal(num, 4, "upper_limit_actual_rotation", ach, aUpperLimitActualRotation);
          }
          else {
            hasUpperLimitActualRotation = Standard_False;
            aUpperLimitActualRotation = 0;
          }
          Handle(StepKinematics_CylindricalPairWithRange) aLocalPair = Handle(StepKinematics_CylindricalPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasLowerLimitActualTranslation,
            aLowerLimitActualTranslation,
            hasUpperLimitActualTranslation,
            aUpperLimitActualTranslation,
            hasLowerLimitActualRotation,
            aLowerLimitActualRotation,
            hasUpperLimitActualRotation,
            aUpperLimitActualRotation);
        }
        else // cylindrical_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_CylindricalPair;
          Handle(StepKinematics_CylindricalPair) aLocalPair = Handle(StepKinematics_CylindricalPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "SPHERICAL_PAIR"))
    {
    // Own fields of cylindrical_pair is non-defined

      if (data->NamedForComplex("SPHERICAL_PAIR_WITH_RANGE", "SPW0", num0, num, ach))
      {
        if (data->CheckNbParams(num, 6, ach, "SPHERICAL_PAIR_WITH_RANGE"))
        {
          // Own fields of spherical_pair_with_range
          aKinematicPair = new StepKinematics_SphericalPairWithRange;

          Standard_Real aLowerLimitYaw;
          Standard_Boolean hasLowerLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_yaw", ach, aLowerLimitYaw);
          }
          else {
            hasLowerLimitYaw = Standard_False;
            aLowerLimitYaw = 0;
          }

          Standard_Real aUpperLimitYaw;
          Standard_Boolean hasUpperLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_yaw", ach, aUpperLimitYaw);
          }
          else {
            hasUpperLimitYaw = Standard_False;
            aUpperLimitYaw = 0;
          }

          Standard_Real aLowerLimitPitch;
          Standard_Boolean hasLowerLimitPitch = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "lower_limit_pitch", ach, aLowerLimitPitch);
          }
          else {
            hasLowerLimitPitch = Standard_False;
            aLowerLimitPitch = 0;
          }

          Standard_Real aUpperLimitPitch;
          Standard_Boolean hasUpperLimitPitch = Standard_True;
          if (data->IsParamDefined(num, 4)) {
            data->ReadReal(num, 4, "upper_limit_pitch", ach, aUpperLimitPitch);
          }
          else {
            hasUpperLimitPitch = Standard_False;
            aUpperLimitPitch = 0;
          }

          Standard_Real aLowerLimitRoll;
          Standard_Boolean hasLowerLimitRoll = Standard_True;
          if (data->IsParamDefined(num, 5)) {
            data->ReadReal(num, 5, "lower_limit_roll", ach, aLowerLimitRoll);
          }
          else {
            hasLowerLimitRoll = Standard_False;
            aLowerLimitRoll = 0;
          }

          Standard_Real aUpperLimitRoll;
          Standard_Boolean hasUpperLimitRoll = Standard_True;
          if (data->IsParamDefined(num, 6)) {
            data->ReadReal(num, 6, "upper_limit_roll", ach, aUpperLimitRoll);
          }
          else {
            hasUpperLimitRoll = Standard_False;
            aUpperLimitRoll = 0;
          }
          Handle(StepKinematics_SphericalPairWithRange) aLocalPair = Handle(StepKinematics_SphericalPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasLowerLimitYaw,
            aLowerLimitYaw,
            hasUpperLimitYaw,
            aUpperLimitYaw,
            hasLowerLimitPitch,
            aLowerLimitPitch,
            hasUpperLimitPitch,
            aUpperLimitPitch,
            hasLowerLimitRoll,
            aLowerLimitRoll,
            hasUpperLimitRoll,
            aUpperLimitRoll);
        }
        else // spherical_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_SphericalPair;
          Handle(StepKinematics_SphericalPair) aLocalPair = Handle(StepKinematics_SphericalPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "SPHERICAL_PAIR_WITH_PIN"))
    {
    // Own fields of spherical_pair_with_pin is non-defined

      if (data->NamedForComplex("SPHERICAL_PAIR_WITH_PIN_AND_RANGE", "SPWPAR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 4, ach, "SPHERICAL_PAIR_WITH_PIN_AND_RANGE"))
        {
          // Own fields of spherical_pair_with_pin_and_range
          aKinematicPair = new StepKinematics_SphericalPairWithPinAndRange;


          Standard_Real aLowerLimitYaw;
          Standard_Boolean hasLowerLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_yaw", ach, aLowerLimitYaw);
          }
          else {
            hasLowerLimitYaw = Standard_False;
            aLowerLimitYaw = 0;
          }

          Standard_Real aUpperLimitYaw;
          Standard_Boolean hasUpperLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_yaw", ach, aUpperLimitYaw);
          }
          else {
            hasUpperLimitYaw = Standard_False;
            aUpperLimitYaw = 0;
          }

          Standard_Real aLowerLimitRoll;
          Standard_Boolean hasLowerLimitRoll = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "lower_limit_roll", ach, aLowerLimitRoll);
          }
          else {
            hasLowerLimitRoll = Standard_False;
            aLowerLimitRoll = 0;
          }

          Standard_Real aUpperLimitRoll;
          Standard_Boolean hasUpperLimitRoll = Standard_True;
          if (data->IsParamDefined(num, 4)) {
            data->ReadReal(num, 4, "upper_limit_roll", ach, aUpperLimitRoll);
          }
          else {
            hasUpperLimitRoll = Standard_False;
            aUpperLimitRoll = 0;
          }
          Handle(StepKinematics_SphericalPairWithPinAndRange) aLocalPair = Handle(StepKinematics_SphericalPairWithPinAndRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasLowerLimitYaw,
            aLowerLimitYaw,
            hasUpperLimitYaw,
            aUpperLimitYaw,
            hasLowerLimitRoll,
            aLowerLimitRoll,
            hasUpperLimitRoll,
            aUpperLimitRoll);
        }
        else // spherical_pair_with_pin_and_range hasn't params
        {
          aKinematicPair = new StepKinematics_SphericalPairWithPin;
          Handle(StepKinematics_SphericalPairWithPin) aLocalPair = Handle(StepKinematics_SphericalPairWithPin)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            aLowOrderKinematicPair_RZ);
        }
      }
      else ach->AddFail("The type of kinematic pair with range is not supported");
    }
    else if (!strcmp(aPairtype, "PRISMATIC_PAIR"))
    {
    // Own fields of prismatic_pair is non-defined

      if (data->NamedForComplex("PRISMATIC_PAIR_WITH_RANGE", "PPW0", num0, num, ach))
      {
        if (data->CheckNbParams(num, 2, ach, "PRISMATIC_PAIR_WITH_RANGE"))
        {
          aKinematicPair = new StepKinematics_PrismaticPairWithRange;
          // Own fields of prismatic_pair_with_range

          Standard_Real aLowerLimitActualTranslation;
          Standard_Boolean hasLowerLimitActualTranslation = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_actual_translation", ach, aLowerLimitActualTranslation);
          }
          else {
            hasLowerLimitActualTranslation = Standard_False;
            aLowerLimitActualTranslation = 0;
          }

          Standard_Real aUpperLimitActualTranslation;
          Standard_Boolean hasUpperLimitActualTranslation = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_actual_translation", ach, aUpperLimitActualTranslation);
          }
          else {
            hasUpperLimitActualTranslation = Standard_False;
            aUpperLimitActualTranslation = 0;
          }
          Handle(StepKinematics_PrismaticPairWithRange) aLocalPair = Handle(StepKinematics_PrismaticPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasLowerLimitActualTranslation,
            aLowerLimitActualTranslation,
            hasUpperLimitActualTranslation,
            aUpperLimitActualTranslation);
        }
        else // prismatic_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_PrismaticPair;
          Handle(StepKinematics_PrismaticPair) aLocalPair = Handle(StepKinematics_PrismaticPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "UNIVERSAL_PAIR"))
    {
    // Own fields of universal_pair 

      aKinematicPair = new StepKinematics_UniversalPair;
      Standard_Real aInputSkewAngle;
      Standard_Boolean hasInputSkewAngle = Standard_True;
      if (data->IsParamDefined(num, 1)) {
        data->ReadReal(num, 1, "input_skew_angle", ach, aInputSkewAngle);
      }
      else {
        hasInputSkewAngle = Standard_False;
        aInputSkewAngle = 0;
      }

      if (data->NamedForComplex("UNIVERSAL_PAIR_WITH_RANGE", "UPWR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 4, ach, "UNIVERSAL_PAIR_WITH_RANGE"))
        {
          // Own fields of universal_pair_with_range

          Standard_Real aLowerLimitFirstRotation;
          Standard_Boolean hasLowerLimitFirstRotation = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_first_rotation", ach, aLowerLimitFirstRotation);
          }
          else {
            hasLowerLimitFirstRotation = Standard_False;
            aLowerLimitFirstRotation = 0;
          }

          Standard_Real aUpperLimitFirstRotation;
          Standard_Boolean hasUpperLimitFirstRotation = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_first_rotation", ach, aUpperLimitFirstRotation);
          }
          else {
            hasUpperLimitFirstRotation = Standard_False;
            aUpperLimitFirstRotation = 0;
          }

          Standard_Real aLowerLimitSecondRotation;
          Standard_Boolean hasLowerLimitSecondRotation = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "lower_limit_second_rotation", ach, aLowerLimitSecondRotation);
          }
          else {
            hasLowerLimitSecondRotation = Standard_False;
            aLowerLimitSecondRotation = 0;
          }

          Standard_Real aUpperLimitSecondRotation;
          Standard_Boolean hasUpperLimitSecondRotation = Standard_True;
          if (data->IsParamDefined(num, 4)) {
            data->ReadReal(num, 4, "upper_limit_second_rotation", ach, aUpperLimitSecondRotation);
          }
          else {
            hasUpperLimitSecondRotation = Standard_False;
            aUpperLimitSecondRotation = 0;
          }
          aKinematicPair = new StepKinematics_UniversalPairWithRange;
          Handle(StepKinematics_UniversalPairWithRange) aLocalPair = Handle(StepKinematics_UniversalPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasInputSkewAngle,
            aInputSkewAngle,
            hasLowerLimitFirstRotation,
            aLowerLimitFirstRotation,
            hasUpperLimitFirstRotation,
            aUpperLimitFirstRotation,
            hasLowerLimitSecondRotation,
            aLowerLimitSecondRotation,
            hasUpperLimitSecondRotation,
            aUpperLimitSecondRotation);
        }
        else // universal_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_UniversalPair;
          Handle(StepKinematics_UniversalPair) aLocalPair = Handle(StepKinematics_UniversalPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasInputSkewAngle,
            aInputSkewAngle);
        }
      }
    }
    else if (!strcmp(aPairtype, "PLANAR_PAIR"))
    {
    // Own fields of planar_pair is non-defined

      if (data->NamedForComplex("PLANAR_PAIR_WITH_RANGE", "PPWR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 6, ach, "UNIVERSAL_PAIR_WITH_RANGE"))
        {
          // Own fields of universal_pair_with_range

          aKinematicPair = new StepKinematics_PlanarPairWithRange;
          Standard_Real aLowerLimitActualRotation;
          Standard_Boolean hasLowerLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_actual_rotation", ach, aLowerLimitActualRotation);
          }
          else {
            hasLowerLimitActualRotation = Standard_False;
            aLowerLimitActualRotation = 0;
          }

          Standard_Real aUpperLimitActualRotation;
          Standard_Boolean hasUpperLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_actual_rotation", ach, aUpperLimitActualRotation);
          }
          else {
            hasUpperLimitActualRotation = Standard_False;
            aUpperLimitActualRotation = 0;
          }

          Standard_Real aLowerLimitActualTranslationX;
          Standard_Boolean hasLowerLimitActualTranslationX = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "lower_limit_actual_translation_x", ach, aLowerLimitActualTranslationX);
          }
          else {
            hasLowerLimitActualTranslationX = Standard_False;
            aLowerLimitActualTranslationX = 0;
          }

          Standard_Real aUpperLimitActualTranslationX;
          Standard_Boolean hasUpperLimitActualTranslationX = Standard_True;
          if (data->IsParamDefined(num, 4)) {
            data->ReadReal(num, 4, "upper_limit_actual_translation_x", ach, aUpperLimitActualTranslationX);
          }
          else {
            hasUpperLimitActualTranslationX = Standard_False;
            aUpperLimitActualTranslationX = 0;
          }

          Standard_Real aLowerLimitActualTranslationY;
          Standard_Boolean hasLowerLimitActualTranslationY = Standard_True;
          if (data->IsParamDefined(num, 5)) {
            data->ReadReal(num, 5, "lower_limit_actual_translation_y", ach, aLowerLimitActualTranslationY);
          }
          else {
            hasLowerLimitActualTranslationY = Standard_False;
            aLowerLimitActualTranslationY = 0;
          }

          Standard_Real aUpperLimitActualTranslationY;
          Standard_Boolean hasUpperLimitActualTranslationY = Standard_True;
          if (data->IsParamDefined(num, 6)) {
            data->ReadReal(num, 6, "upper_limit_actual_translation_y", ach, aUpperLimitActualTranslationY);
          }
          else {
            hasUpperLimitActualTranslationY = Standard_False;
            aUpperLimitActualTranslationY = 0;
          }
          Handle(StepKinematics_PlanarPairWithRange) aLocalPair = Handle(StepKinematics_PlanarPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            hasLowerLimitActualRotation,
            aLowerLimitActualRotation,
            hasUpperLimitActualRotation,
            aUpperLimitActualRotation,
            hasLowerLimitActualTranslationX,
            aLowerLimitActualTranslationX,
            hasUpperLimitActualTranslationX,
            aUpperLimitActualTranslationX,
            hasLowerLimitActualTranslationY,
            aLowerLimitActualTranslationY,
            hasUpperLimitActualTranslationY,
            aUpperLimitActualTranslationY);
        }
        else // universal_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_PlanarPair;
          Handle(StepKinematics_PlanarPair) aLocalPair = Handle(StepKinematics_PlanarPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
            aLowOrderKinematicPair_RZ);
        }
      }
    }
  }
  else if (!strcmp(aPairSuperType, "LOW_ORDER_KINEMATIC_PAIR_WITH_MOTION_COUPLING"))
  {
    num = data->NextForComplex(num);
    Standard_CString aPairtype = data->CType(num);
    if (!strcmp(aPairtype, "GEAR_PAIR"))
    {

      // Inherited fields of GearPair

      Standard_Real aGearPair_RadiusFirstLink;
      data->ReadReal(num, 1, "gear_pair.radius_first_link", ach, aGearPair_RadiusFirstLink);

      Standard_Real aGearPair_RadiusSecondLink;
      data->ReadReal(num, 2, "gear_pair.radius_second_link", ach, aGearPair_RadiusSecondLink);

      Standard_Real aGearPair_Bevel;
      data->ReadReal(num, 3, "gear_pair.bevel", ach, aGearPair_Bevel);

      Standard_Real aGearPair_HelicalAngle;
      data->ReadReal(num, 4, "gear_pair.helical_angle", ach, aGearPair_HelicalAngle);

      Standard_Real aGearPair_GearRatio;
      data->ReadReal(num, 5, "gear_pair.gear_ratio", ach, aGearPair_GearRatio);

      if (data->NamedForComplex("GEAR_PAIR_WITH_RANGE", "GPWR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 2, ach, "GEAR_PAIR_WITH_RANGE"))
        {
          // Own fields of GearPairWithRange

          aKinematicPair = new StepKinematics_GearPairWithRange;

          Standard_Real aLowerLimitActualRotation1;
          Standard_Boolean hasLowerLimitActualRotation1 = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_actual_rotation1", ach, aLowerLimitActualRotation1);
          }
          else {
            hasLowerLimitActualRotation1 = Standard_False;
            aLowerLimitActualRotation1 = 0;
          }

          Standard_Real aUpperLimitActualRotation1;
          Standard_Boolean hasUpperLimitActualRotation1 = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_actual_rotation1", ach, aUpperLimitActualRotation1);
          }
          else {
            hasUpperLimitActualRotation1 = Standard_False;
            aUpperLimitActualRotation1 = 0;
          }
          Handle(StepKinematics_GearPairWithRange) aLocalPair = Handle(StepKinematics_GearPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aGearPair_RadiusFirstLink,
            aGearPair_RadiusSecondLink,
            aGearPair_Bevel,
            aGearPair_HelicalAngle,
            aGearPair_GearRatio,
            hasLowerLimitActualRotation1,
            aLowerLimitActualRotation1,
            hasUpperLimitActualRotation1,
            aUpperLimitActualRotation1);

        }
        else // StepKinematics_GearPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_GearPair;
          Handle(StepKinematics_GearPair) aLocalPair = Handle(StepKinematics_GearPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aGearPair_RadiusFirstLink,
            aGearPair_RadiusSecondLink,
            aGearPair_Bevel,
            aGearPair_HelicalAngle,
            aGearPair_GearRatio);
        }
      }
    }
    else if (!strcmp(aPairtype, "RACK_AND_PINION_PAIR"))
    {

      // Inherited fields of RackAndPinionPair
      Standard_Real aRackAndPinionPair_PinionRadius;
      data->ReadReal(num, 1, "rack_and_pinion_pair.pinion_radius", ach, aRackAndPinionPair_PinionRadius);
  
      if (data->NamedForComplex("RACK_AND_PINION_PAIR_WITH_RANGE", "RAPPWR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 2, ach, "RACK_AND_PINION_PAIR_WITH_RANGE"))
        {
          // Own fields of GearPairWithRange

          aKinematicPair = new StepKinematics_RackAndPinionPairWithRange;


          Standard_Real aLowerLimitRackDisplacement;
          Standard_Boolean hasLowerLimitRackDisplacement = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_rack_displacement", ach, aLowerLimitRackDisplacement);
          }
          else {
            hasLowerLimitRackDisplacement = Standard_False;
            aLowerLimitRackDisplacement = 0;
          }

          Standard_Real aUpperLimitRackDisplacement;
          Standard_Boolean hasUpperLimitRackDisplacement = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_rack_displacement", ach, aUpperLimitRackDisplacement);
          }
          else {
            hasUpperLimitRackDisplacement = Standard_False;
            aUpperLimitRackDisplacement = 0;
          }

          Handle(StepKinematics_RackAndPinionPairWithRange) aLocalPair = Handle(StepKinematics_RackAndPinionPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
        else // StepKinematics_RackAndPinionPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_RackAndPinionPair;
          Handle(StepKinematics_RackAndPinionPair) aLocalPair = Handle(StepKinematics_RackAndPinionPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aRackAndPinionPair_PinionRadius);
        }
      }
    }
    else if (!strcmp(aPairtype, "SCREW_PAIR"))
    {
      // Inherited fields of ScrewPair

      Standard_Real aScrewPair_Pitch;
      data->ReadReal(num, 1, "screw_pair.pitch", ach, aScrewPair_Pitch);

      if (data->NamedForComplex("SCREW_PAIR_WITH_RANGE", "SPWR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 2, ach, "SCREW_PAIR_WITH_RANGE"))
        {
          // Own fields of ScrewPairWithRange

          aKinematicPair = new StepKinematics_ScrewPairWithRange;
          Standard_Real aLowerLimitActualRotation;
          Standard_Boolean hasLowerLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 1)) {
            data->ReadReal(num, 1, "lower_limit_actual_rotation", ach, aLowerLimitActualRotation);
          }
          else {
            hasLowerLimitActualRotation = Standard_False;
            aLowerLimitActualRotation = 0;
          }

          Standard_Real aUpperLimitActualRotation;
          Standard_Boolean hasUpperLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "upper_limit_actual_rotation", ach, aUpperLimitActualRotation);
          }
          else {
            hasUpperLimitActualRotation = Standard_False;
            aUpperLimitActualRotation = 0;
          }

          Handle(StepKinematics_ScrewPairWithRange) aLocalPair = Handle(StepKinematics_ScrewPairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aScrewPair_Pitch,
            hasLowerLimitActualRotation,
            aLowerLimitActualRotation,
            hasUpperLimitActualRotation,
            aUpperLimitActualRotation);
        }
        else // StepKinematics_ScrewPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_ScrewPair;
          Handle(StepKinematics_ScrewPair) aLocalPair = Handle(StepKinematics_ScrewPair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aScrewPair_Pitch);
        }
      }
    }
  }
  else // "HIGH_ORDER_KINEMATIC_PAIR"
  {
     num = data->NextForComplex(num);
     Standard_CString aPairtype = data->CType(num);
    if (!strcmp(aPairtype, "POINT_ON_PLANAR_CURVE_PAIR"))
     {

      // Inherited fields of PointOnPlanarCurvePair

      Handle(StepGeom_Curve) aPointOnPlanarCurvePair_PairCurve;
      data->ReadEntity(num, 1, "point_on_planar_curve_pair.pair_curve", ach, STANDARD_TYPE(StepGeom_Curve), aPointOnPlanarCurvePair_PairCurve);

      Standard_Boolean aPointOnPlanarCurvePair_Orientation;
      data->ReadBoolean(num, 2, "point_on_planar_curve_pair.orientation", ach, aPointOnPlanarCurvePair_Orientation);


      if (data->NamedForComplex("POINT_ON_PLANAR_CURVE_PAIR_WITH_RANGE", "POPCPW", num0, num, ach))
      {
        if (data->CheckNbParams(num, 7, ach, "POINT_ON_PLANAR_CURVE_PAIR_WITH_RANGE"))
        {
          // Own fields of PointOnPlanarCurvePairWithRange

          aKinematicPair = new StepKinematics_PointOnPlanarCurvePairWithRange;

          Handle(StepGeom_TrimmedCurve) aRangeOnPairCurve;
          data->ReadEntity(num, 1, "range_on_pair_curve", ach, STANDARD_TYPE(StepGeom_TrimmedCurve), aRangeOnPairCurve);
  
          Standard_Real aLowerLimitYaw;
          Standard_Boolean hasLowerLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "lower_limit_yaw", ach, aLowerLimitYaw);
          }
          else {
            hasLowerLimitYaw = Standard_False;
            aLowerLimitYaw = 0;
          }

          Standard_Real aUpperLimitYaw;
          Standard_Boolean hasUpperLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "upper_limit_yaw", ach, aUpperLimitYaw);
        }
        else {
          hasUpperLimitYaw = Standard_False;
          aUpperLimitYaw = 0;
        }

        Standard_Real aLowerLimitPitch;
        Standard_Boolean hasLowerLimitPitch = Standard_True;
        if (data->IsParamDefined(num, 4)) {
          data->ReadReal(num, 4, "lower_limit_pitch", ach, aLowerLimitPitch);
        }
        else {
          hasLowerLimitPitch = Standard_False;
          aLowerLimitPitch = 0;
        }

        Standard_Real aUpperLimitPitch;
        Standard_Boolean hasUpperLimitPitch = Standard_True;
        if (data->IsParamDefined(num, 5)) {
          data->ReadReal(num, 5, "upper_limit_pitch", ach, aUpperLimitPitch);
        }
        else {
          hasUpperLimitPitch = Standard_False;
          aUpperLimitPitch = 0;
        }

        Standard_Real aLowerLimitRoll;
        Standard_Boolean hasLowerLimitRoll = Standard_True;
        if (data->IsParamDefined(num, 6)) {
          data->ReadReal(num, 6, "lower_limit_roll", ach, aLowerLimitRoll);
        }
        else {
          hasLowerLimitRoll = Standard_False;
          aLowerLimitRoll = 0;
        }

        Standard_Real aUpperLimitRoll;
        Standard_Boolean hasUpperLimitRoll = Standard_True;
        if (data->IsParamDefined(num, 7)) {
          data->ReadReal(num, 7, "upper_limit_roll", ach, aUpperLimitRoll);
        }
        else {
          hasUpperLimitRoll = Standard_False;
          aUpperLimitRoll = 0;
        }
        Handle(StepKinematics_PointOnPlanarCurvePairWithRange) aLocalPair = Handle(StepKinematics_PointOnPlanarCurvePairWithRange)::DownCast(aKinematicPair);
        // Initialize kinematic_pair
        aLocalPair->Init(aRepresentationItem_Name,
          aItemDefinedTransformation_Name,
          hasItemDefinedTransformation_Description,
          aItemDefinedTransformation_Description,
          aItemDefinedTransformation_TransformItem1,
          aItemDefinedTransformation_TransformItem2,
          aKinematicPair_Joint,
          aPointOnPlanarCurvePair_PairCurve,
          aPointOnPlanarCurvePair_Orientation,
          aRangeOnPairCurve,
          hasLowerLimitYaw,
          aLowerLimitYaw,
          hasUpperLimitYaw,
          aUpperLimitYaw,
          hasLowerLimitPitch,
          aLowerLimitPitch,
          hasUpperLimitPitch,
          aUpperLimitPitch,
          hasLowerLimitRoll,
          aLowerLimitRoll,
          hasUpperLimitRoll,
          aUpperLimitRoll);

      }
      else // point_on_planar_curve_pair_with_range hasn't params
      {
        aKinematicPair = new StepKinematics_PointOnPlanarCurvePair;
        Handle(StepKinematics_PointOnPlanarCurvePair) aLocalPair = Handle(StepKinematics_PointOnPlanarCurvePair)::DownCast(aKinematicPair);
        // Initialize kinematic_pair
        aLocalPair->Init(aRepresentationItem_Name,
          aItemDefinedTransformation_Name,
          hasItemDefinedTransformation_Description,
          aItemDefinedTransformation_Description,
          aItemDefinedTransformation_TransformItem1,
          aItemDefinedTransformation_TransformItem2,
          aKinematicPair_Joint,
          aPointOnPlanarCurvePair_PairCurve,
          aPointOnPlanarCurvePair_Orientation);
      }
    }
  }
    else if (!strcmp(aPairtype, "POINT_ON_SURFACE_PAIR"))
    {
      // Inherited fields of PointOnSurfacePair

      Handle(StepGeom_Surface) aPointOnSurfacePair_PairSurface;
      data->ReadEntity(num, 1, "point_on_surface_pair.pair_surface", ach, STANDARD_TYPE(StepGeom_Surface), aPointOnSurfacePair_PairSurface);

      if (data->NamedForComplex("POINT_ON_SURFACE_PAIR_WITH_RANGE ", "RAPPWR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 2, ach, "POINT_ON_SURFACE_PAIR_WITH_RANGE "))
        {
          // Own fields of PointOnSurfacePairWithRange

          aKinematicPair = new StepKinematics_PointOnSurfacePairWithRange;


          Handle(StepGeom_RectangularTrimmedSurface) aRangeOnPairSurface;
          data->ReadEntity(num, 1, "range_on_pair_surface", ach, STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), aRangeOnPairSurface);

          Standard_Real aLowerLimitYaw;
          Standard_Boolean hasLowerLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 2)) {
            data->ReadReal(num, 2, "lower_limit_yaw", ach, aLowerLimitYaw);
          }
          else {
            hasLowerLimitYaw = Standard_False;
            aLowerLimitYaw = 0;
          }

          Standard_Real aUpperLimitYaw;
          Standard_Boolean hasUpperLimitYaw = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "upper_limit_yaw", ach, aUpperLimitYaw);
          }
          else {
            hasUpperLimitYaw = Standard_False;
            aUpperLimitYaw = 0;
          }

          Standard_Real aLowerLimitPitch;
          Standard_Boolean hasLowerLimitPitch = Standard_True;
          if (data->IsParamDefined(num, 4)) {
            data->ReadReal(num, 4, "lower_limit_pitch", ach, aLowerLimitPitch);
          }
          else {
            hasLowerLimitPitch = Standard_False;
            aLowerLimitPitch = 0;
          }

          Standard_Real aUpperLimitPitch;
          Standard_Boolean hasUpperLimitPitch = Standard_True;
          if (data->IsParamDefined(num, 5)) {
            data->ReadReal(num, 5, "upper_limit_pitch", ach, aUpperLimitPitch);
          }
          else {
            hasUpperLimitPitch = Standard_False;
            aUpperLimitPitch = 0;
          }

          Standard_Real aLowerLimitRoll;
          Standard_Boolean hasLowerLimitRoll = Standard_True;
          if (data->IsParamDefined(num, 6)) {
            data->ReadReal(num, 6, "lower_limit_roll", ach, aLowerLimitRoll);
          }
          else {
            hasLowerLimitRoll = Standard_False;
            aLowerLimitRoll = 0;
          }

          Standard_Real aUpperLimitRoll;
          Standard_Boolean hasUpperLimitRoll = Standard_True;
          if (data->IsParamDefined(num, 7)) {
            data->ReadReal(num, 7, "upper_limit_roll", ach, aUpperLimitRoll);
          }
          else {
            hasUpperLimitRoll = Standard_False;
            aUpperLimitRoll = 0;
          }
          Handle(StepKinematics_PointOnSurfacePairWithRange) aLocalPair = Handle(StepKinematics_PointOnSurfacePairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aPointOnSurfacePair_PairSurface,
            aRangeOnPairSurface,
            hasLowerLimitYaw,
            aLowerLimitYaw,
            hasUpperLimitYaw,
            aUpperLimitYaw,
            hasLowerLimitPitch,
            aLowerLimitPitch,
            hasUpperLimitPitch,
            aUpperLimitPitch,
            hasLowerLimitRoll,
            aLowerLimitRoll,
            hasUpperLimitRoll,
            aUpperLimitRoll);
        }
        else // StepKinematics_PointOnSurfacePairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_PointOnSurfacePair;
          Handle(StepKinematics_PointOnSurfacePair) aLocalPair = Handle(StepKinematics_PointOnSurfacePair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aPointOnSurfacePair_PairSurface);
        }
      }
    }
    else if (!strcmp(aPairtype, "PLANAR_CURVE_PAIR"))
    {
      // Inherited fields of PlanarCurvePair

      Handle(StepGeom_Curve) aPlanarCurvePair_Curve1;
      data->ReadEntity(num, 1, "planar_curve_pair.curve1", ach, STANDARD_TYPE(StepGeom_Curve), aPlanarCurvePair_Curve1);

      Handle(StepGeom_Curve) aPlanarCurvePair_Curve2;
      data->ReadEntity(num, 2, "planar_curve_pair.curve2", ach, STANDARD_TYPE(StepGeom_Curve), aPlanarCurvePair_Curve2);

      Standard_Boolean aPlanarCurvePair_Orientation;
      data->ReadBoolean(num, 3, "planar_curve_pair.orientation", ach, aPlanarCurvePair_Orientation);

      if (data->NamedForComplex("PLANAR_CURVE_PAIR_RANGE", "PCPR", num0, num, ach))
      {
        if (data->CheckNbParams(num, 2, ach, "PLANAR_CURVE_PAIR_RANGE"))
        {
          // Own fields of PlanarCurvePairRange
          aKinematicPair = new StepKinematics_PlanarCurvePairRange;

          Handle(StepGeom_TrimmedCurve) aRangeOnCurve1;
          data->ReadEntity(num, 1, "range_on_curve1", ach, STANDARD_TYPE(StepGeom_TrimmedCurve), aRangeOnCurve1);

          Handle(StepGeom_TrimmedCurve) aRangeOnCurve2;
          data->ReadEntity(num, 2, "range_on_curve2", ach, STANDARD_TYPE(StepGeom_TrimmedCurve), aRangeOnCurve2);


          Handle(StepKinematics_PlanarCurvePairRange) aLocalPair = Handle(StepKinematics_PlanarCurvePairRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aPlanarCurvePair_Curve1,
            aPlanarCurvePair_Curve2,
            aPlanarCurvePair_Orientation,
            aRangeOnCurve1,
            aRangeOnCurve2);
        }
        else // StepKinematics_ScrewPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_PlanarCurvePair;
          Handle(StepKinematics_PlanarCurvePair) aLocalPair = Handle(StepKinematics_PlanarCurvePair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aPlanarCurvePair_Curve1,
            aPlanarCurvePair_Curve2,
            aPlanarCurvePair_Orientation);
        }
      }
    }
    else if (!strcmp(aPairtype, "SURFACE_PAIR"))
    {
    // Inherited fields of SurfacePair

    Handle(StepGeom_Surface) aSurfacePair_Surface1;
    data->ReadEntity(num, 1, "surface_pair.surface1", ach, STANDARD_TYPE(StepGeom_Surface), aSurfacePair_Surface1);

    Handle(StepGeom_Surface) aSurfacePair_Surface2;
    data->ReadEntity(num, 2, "surface_pair.surface2", ach, STANDARD_TYPE(StepGeom_Surface), aSurfacePair_Surface2);

    Standard_Boolean aSurfacePair_Orientation;
    data->ReadBoolean(num, 3, "surface_pair.orientation", ach, aSurfacePair_Orientation);

      if (data->NamedForComplex("SURFACE_PAIR_WITH_RANGE", "SPW1", num0, num, ach))
      {
        if (data->CheckNbParams(num, 4, ach, "SURFACE_PAIR_WITH_RANGE"))
        {
          aKinematicPair = new StepKinematics_SurfacePairWithRange;
          // Own fields of SurfacePairWithRange

          Handle(StepGeom_RectangularTrimmedSurface) aRangeOnSurface1;
          data->ReadEntity(num, 1, "range_on_surface1", ach, STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), aRangeOnSurface1);

          Handle(StepGeom_RectangularTrimmedSurface) aRangeOnSurface2;
          data->ReadEntity(num, 2, "range_on_surface2", ach, STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), aRangeOnSurface2);

          Standard_Real aLowerLimitActualRotation;
          Standard_Boolean hasLowerLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 3)) {
            data->ReadReal(num, 3, "lower_limit_actual_rotation", ach, aLowerLimitActualRotation);
          }
          else {
            hasLowerLimitActualRotation = Standard_False;
            aLowerLimitActualRotation = 0;
          }

          Standard_Real aUpperLimitActualRotation;
          Standard_Boolean hasUpperLimitActualRotation = Standard_True;
          if (data->IsParamDefined(num, 4)) {
            data->ReadReal(num, 4, "upper_limit_actual_rotation", ach, aUpperLimitActualRotation);
          }
          else {
            hasUpperLimitActualRotation = Standard_False;
            aUpperLimitActualRotation = 0;
          }

          Handle(StepKinematics_SurfacePairWithRange) aLocalPair = Handle(StepKinematics_SurfacePairWithRange)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aSurfacePair_Surface1,
            aSurfacePair_Surface2,
            aSurfacePair_Orientation,
            aRangeOnSurface1,
            aRangeOnSurface2,
            hasLowerLimitActualRotation,
            aLowerLimitActualRotation,
            hasUpperLimitActualRotation,
            aUpperLimitActualRotation);
        }
        else // StepKinematics_SurfacePairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_SurfacePair;
          Handle(StepKinematics_SurfacePair) aLocalPair = Handle(StepKinematics_SurfacePair)::DownCast(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aSurfacePair_Surface1,
            aSurfacePair_Surface2,
            aSurfacePair_Orientation);
        }
      }
    }
  }
  data->NamedForComplex("REPRESENTATION_ITEM", "RPRITM", num0, num, ach);

  // Own fields of representation_item
  data->ReadString(num, 1, "representation_item.name", ach, aRepresentationItem_Name);
  anActuatedKinematicPair->Init(aRepresentationItem_Name, 
    aItemDefinedTransformation_Name, hasItemDefinedTransformation_Description,
    aItemDefinedTransformation_Description, 
    aItemDefinedTransformation_TransformItem1,
    aItemDefinedTransformation_TransformItem2, 
    aKinematicPair_Joint, hasTX, aTX, hasTY, aTY, hasTZ,
    aTZ, hasRX, aRX, hasRY, aRY, hasRZ, aRZ);

  // Initialize kinematic_pair
  aKinematicPair->SetName(aRepresentationItem_Name);
  // Initialize entity
  ent->Init(aRepresentationItem_Name,
    aItemDefinedTransformation_Name, 
    hasItemDefinedTransformation_Description,
    aItemDefinedTransformation_Description,
    aItemDefinedTransformation_TransformItem1,
    aItemDefinedTransformation_TransformItem2,
    aKinematicPair_Joint,
    anActuatedKinematicPair, aKinematicPair);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWActuatedKinPairAndOrderKinPair::WriteStep
(StepData_StepWriter& SW,
  const Handle(StepKinematics_ActuatedKinPairAndOrderKinPair)& ent) const
{
  // --- Instance of plex componant BoundedCurve ---

  SW.StartEntity("(ACTUATED_KINEMATIC_PAIR");

  if (ent->GetActuatedKinematicPair()->HasTX()) {
    switch (ent->GetActuatedKinematicPair()->TX()) {
    case StepKinematics_adBidirectional: SW.SendEnum(".BIDIRECTIONAL."); break;
    case StepKinematics_adPositiveOnly: SW.SendEnum(".POSITIVE_ONLY."); break;
    case StepKinematics_adNegativeOnly: SW.SendEnum(".NEGATIVE_ONLY."); break;
    case StepKinematics_adNotActuated: SW.SendEnum(".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if (ent->GetActuatedKinematicPair()->HasTY()) {
    switch (ent->GetActuatedKinematicPair()->TY()) {
    case StepKinematics_adBidirectional: SW.SendEnum(".BIDIRECTIONAL."); break;
    case StepKinematics_adPositiveOnly: SW.SendEnum(".POSITIVE_ONLY."); break;
    case StepKinematics_adNegativeOnly: SW.SendEnum(".NEGATIVE_ONLY."); break;
    case StepKinematics_adNotActuated: SW.SendEnum(".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if (ent->GetActuatedKinematicPair()->HasTZ()) {
    switch (ent->GetActuatedKinematicPair()->TZ()) {
    case StepKinematics_adBidirectional: SW.SendEnum(".BIDIRECTIONAL."); break;
    case StepKinematics_adPositiveOnly: SW.SendEnum(".POSITIVE_ONLY."); break;
    case StepKinematics_adNegativeOnly: SW.SendEnum(".NEGATIVE_ONLY."); break;
    case StepKinematics_adNotActuated: SW.SendEnum(".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if (ent->GetActuatedKinematicPair()->HasRX()) {
    switch (ent->GetActuatedKinematicPair()->RX()) {
    case StepKinematics_adBidirectional: SW.SendEnum(".BIDIRECTIONAL."); break;
    case StepKinematics_adPositiveOnly: SW.SendEnum(".POSITIVE_ONLY."); break;
    case StepKinematics_adNegativeOnly: SW.SendEnum(".NEGATIVE_ONLY."); break;
    case StepKinematics_adNotActuated: SW.SendEnum(".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if (ent->GetActuatedKinematicPair()->HasRY()) {
    switch (ent->GetActuatedKinematicPair()->RY()) {
    case StepKinematics_adBidirectional: SW.SendEnum(".BIDIRECTIONAL."); break;
    case StepKinematics_adPositiveOnly: SW.SendEnum(".POSITIVE_ONLY."); break;
    case StepKinematics_adNegativeOnly: SW.SendEnum(".NEGATIVE_ONLY."); break;
    case StepKinematics_adNotActuated: SW.SendEnum(".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();

  if (ent->GetActuatedKinematicPair()->HasRZ()) {
    switch (ent->GetActuatedKinematicPair()->RZ()) {
    case StepKinematics_adBidirectional: SW.SendEnum(".BIDIRECTIONAL."); break;
    case StepKinematics_adPositiveOnly: SW.SendEnum(".POSITIVE_ONLY."); break;
    case StepKinematics_adNegativeOnly: SW.SendEnum(".NEGATIVE_ONLY."); break;
    case StepKinematics_adNotActuated: SW.SendEnum(".NOT_ACTUATED."); break;
    }
  }
  else SW.SendUndef();
  SW.StartEntity("(GEOMETRIC_REPRESENTATION_ITEM");

  SW.StartEntity("(ITEM_DEFINED_TRANSFORMATION");
  // Inherited fields of ItemDefinedTransformation

  SW.Send(ent->ItemDefinedTransformation()->Name());

  if (ent->ItemDefinedTransformation()->HasDescription()) {
    SW.Send(ent->ItemDefinedTransformation()->Description());
  }
  else SW.SendUndef();

  SW.Send(ent->ItemDefinedTransformation()->TransformItem1());

  SW.Send(ent->ItemDefinedTransformation()->TransformItem2());

  SW.StartEntity("(KINEMATIC_PAIR");

  // Own fields of KinematicPair

  SW.Send(ent->Joint());
  if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_LowOrderKinematicPair)))
  {
    Handle(StepKinematics_LowOrderKinematicPair) aLowOrderKinematicPair = Handle(StepKinematics_LowOrderKinematicPair)::DownCast(ent->GetOrderKinematicPair());
    SW.StartEntity("(LOW_ORDER_KINEMATIC_PAIR");

    SW.SendBoolean(aLowOrderKinematicPair->TX());

    SW.SendBoolean(aLowOrderKinematicPair->TY());

    SW.SendBoolean(aLowOrderKinematicPair->TZ());

    SW.SendBoolean(aLowOrderKinematicPair->RX());

    SW.SendBoolean(aLowOrderKinematicPair->RY());

    SW.SendBoolean(aLowOrderKinematicPair->RZ());

    if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_RevolutePair)))
    {
      SW.StartEntity("(REVOLUTE_PAIR");
      SW.StartEntity("(REVOLUTE_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_RevolutePairWithRange)))
      {
        Handle(StepKinematics_RevolutePairWithRange) aRevolutePairWithRange = Handle(StepKinematics_RevolutePairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of RevolutePairWithRange

        if (aRevolutePairWithRange->HasLowerLimitActualRotation()) {
          SW.Send(aRevolutePairWithRange->LowerLimitActualRotation());
        }
        else SW.SendUndef();

        if (aRevolutePairWithRange->HasUpperLimitActualRotation()) {
          SW.Send(aRevolutePairWithRange->UpperLimitActualRotation());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_CylindricalPair)))
    {
      SW.StartEntity("(CYLINDRICAL_PAIR");
      SW.StartEntity("(CYLINDRICAL_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_CylindricalPairWithRange)))
      {
        Handle(StepKinematics_CylindricalPairWithRange) aCylindricalPairWithRange = Handle(StepKinematics_CylindricalPairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of CylindricalPairWithRange

        if (aCylindricalPairWithRange->HasLowerLimitActualTranslation()) {
          SW.Send(aCylindricalPairWithRange->LowerLimitActualTranslation());
        }
        else SW.SendUndef();

        if (aCylindricalPairWithRange->HasUpperLimitActualTranslation()) {
          SW.Send(aCylindricalPairWithRange->UpperLimitActualTranslation());
        }
        else SW.SendUndef();

        if (aCylindricalPairWithRange->HasLowerLimitActualRotation()) {
          SW.Send(aCylindricalPairWithRange->LowerLimitActualRotation());
        }
        else SW.SendUndef();

        if (aCylindricalPairWithRange->HasUpperLimitActualRotation()) {
          SW.Send(aCylindricalPairWithRange->UpperLimitActualRotation());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SphericalPair)))
    {
      SW.StartEntity("(SPHERICAL_PAIR");
      SW.StartEntity("(SPHERICAL_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SphericalPairWithRange)))
      {
        Handle(StepKinematics_SphericalPairWithRange) aSphericalPairWithRange = Handle(StepKinematics_SphericalPairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of SphericalPairWithRange

        if (aSphericalPairWithRange->HasLowerLimitYaw()) {
          SW.Send(aSphericalPairWithRange->LowerLimitYaw());
        }
        else SW.SendUndef();

        if (aSphericalPairWithRange->HasUpperLimitYaw()) {
          SW.Send(aSphericalPairWithRange->UpperLimitYaw());
        }
        else SW.SendUndef();

        if (aSphericalPairWithRange->HasLowerLimitPitch()) {
          SW.Send(aSphericalPairWithRange->LowerLimitPitch());
        }
        else SW.SendUndef();

        if (aSphericalPairWithRange->HasUpperLimitPitch()) {
          SW.Send(aSphericalPairWithRange->UpperLimitPitch());
        }
        else SW.SendUndef();

        if (aSphericalPairWithRange->HasLowerLimitRoll()) {
          SW.Send(aSphericalPairWithRange->LowerLimitRoll());
        }
        else SW.SendUndef();

        if (aSphericalPairWithRange->HasUpperLimitRoll()) {
          SW.Send(aSphericalPairWithRange->UpperLimitRoll());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SphericalPairWithPin)))
    {
      SW.StartEntity("(SPHERICAL_PAIR_WITH_PIN");
      SW.StartEntity("(SPHERICAL_PAIR_WITH_PIN_AND_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SphericalPairWithPinAndRange)))
      {
        Handle(StepKinematics_SphericalPairWithPinAndRange) aSphericalPairWithPinAndRange = Handle(StepKinematics_SphericalPairWithPinAndRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of SphericalPairWithPinAndRange

        if (aSphericalPairWithPinAndRange->HasLowerLimitYaw()) {
          SW.Send(aSphericalPairWithPinAndRange->LowerLimitYaw());
        }
        else SW.SendUndef();

        if (aSphericalPairWithPinAndRange->HasUpperLimitYaw()) {
          SW.Send(aSphericalPairWithPinAndRange->UpperLimitYaw());
        }
        else SW.SendUndef();

        if (aSphericalPairWithPinAndRange->HasLowerLimitRoll()) {
          SW.Send(aSphericalPairWithPinAndRange->LowerLimitRoll());
        }
        else SW.SendUndef();

        if (aSphericalPairWithPinAndRange->HasUpperLimitRoll()) {
          SW.Send(aSphericalPairWithPinAndRange->UpperLimitRoll());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PrismaticPair)))
    {
      SW.StartEntity("(PRISMATIC_PAIR");
      SW.StartEntity("(PRISMATIC_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PrismaticPairWithRange)))
      {
        Handle(StepKinematics_PrismaticPairWithRange) aPrismaticPairWithRange = Handle(StepKinematics_PrismaticPairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of PrismaticPairWithRange

        if (aPrismaticPairWithRange->HasLowerLimitActualTranslation()) {
          SW.Send(aPrismaticPairWithRange->LowerLimitActualTranslation());
        }
        else SW.SendUndef();

        if (aPrismaticPairWithRange->HasUpperLimitActualTranslation()) {
          SW.Send(aPrismaticPairWithRange->UpperLimitActualTranslation());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_UniversalPair)))
    {
      SW.StartEntity("(UNIVERSAL_PAIR");
      Handle(StepKinematics_UniversalPair) anUniversalPair = Handle(StepKinematics_UniversalPair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of UniversalPair

      if (anUniversalPair->HasInputSkewAngle()) {
        SW.Send(anUniversalPair->InputSkewAngle());
      }
      else SW.SendUndef();

      SW.StartEntity("(PRISMATIC_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PrismaticPairWithRange)))
      {
        Handle(StepKinematics_UniversalPairWithRange) anUniversalPairWithRange = Handle(StepKinematics_UniversalPairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of UniversalPairWithRange

        if (anUniversalPairWithRange->HasLowerLimitFirstRotation()) {
          SW.Send(anUniversalPairWithRange->LowerLimitFirstRotation());
        }
        else SW.SendUndef();

        if (anUniversalPairWithRange->HasUpperLimitFirstRotation()) {
          SW.Send(anUniversalPairWithRange->UpperLimitFirstRotation());
        }
        else SW.SendUndef();

        if (anUniversalPairWithRange->HasLowerLimitSecondRotation()) {
          SW.Send(anUniversalPairWithRange->LowerLimitSecondRotation());
        }
        else SW.SendUndef();

        if (anUniversalPairWithRange->HasUpperLimitSecondRotation()) {
          SW.Send(anUniversalPairWithRange->UpperLimitSecondRotation());
        }
        else SW.SendUndef();
      }
    }
  }
  else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_LowOrderKinematicPairWithMotionCoupling)))
  {
    if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_GearPair)))
    {
      SW.StartEntity("(GEAR_PAIR");
      Handle(StepKinematics_GearPair) aGearPair = Handle(StepKinematics_GearPair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of GearPair

      SW.Send(aGearPair->RadiusFirstLink());

      SW.Send(aGearPair->RadiusSecondLink());

      SW.Send(aGearPair->Bevel());

      SW.Send(aGearPair->HelicalAngle());

      SW.Send(aGearPair->GearRatio());

      SW.StartEntity("(PRISMATIC_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_GearPairWithRange)))
      {
        Handle(StepKinematics_GearPairWithRange) aGearPairWithRange = Handle(StepKinematics_GearPairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of GearPairWithRange

        if (aGearPairWithRange->HasLowerLimitActualRotation1()) {
          SW.Send(aGearPairWithRange->LowerLimitActualRotation1());
        }
        else SW.SendUndef();

        if (aGearPairWithRange->HasUpperLimitActualRotation1()) {
          SW.Send(aGearPairWithRange->UpperLimitActualRotation1());
        }
        else SW.SendUndef();
      }
    } 
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_RackAndPinionPair)))
    {
      SW.StartEntity("RACK_AND_PINION_PAIR");
      Handle(StepKinematics_RackAndPinionPair) aRackAndPinionPair = Handle(StepKinematics_RackAndPinionPair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of RackAndPinionPair

      SW.Send(aRackAndPinionPair->PinionRadius());

      SW.StartEntity("(RACK_AND_PINION_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_RackAndPinionPairWithRange)))
      {
        Handle(StepKinematics_RackAndPinionPairWithRange) aRackAndPinionPairWithRange = Handle(StepKinematics_RackAndPinionPairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of aRackAndPinionPairWithRange

        if (aRackAndPinionPairWithRange->HasLowerLimitRackDisplacement()) {
          SW.Send(aRackAndPinionPairWithRange->LowerLimitRackDisplacement());
        }
        else SW.SendUndef();

        if (aRackAndPinionPairWithRange->HasUpperLimitRackDisplacement()) {
          SW.Send(aRackAndPinionPairWithRange->UpperLimitRackDisplacement());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_ScrewPair)))
    {
      SW.StartEntity("SCREW_PAIR");
      Handle(StepKinematics_ScrewPair) aScrewPair = Handle(StepKinematics_ScrewPair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of ScrewPair

      SW.Send(aScrewPair->Pitch());

      SW.StartEntity("(SCREW_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_ScrewPairWithRange)))
      {
        Handle(StepKinematics_ScrewPairWithRange) aScrewPairWithRange = Handle(StepKinematics_ScrewPairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of aRackAndPinionPairWithRange

        if (aScrewPairWithRange->HasLowerLimitActualRotation()) {
          SW.Send(aScrewPairWithRange->LowerLimitActualRotation());
        }
        else SW.SendUndef();

        if (aScrewPairWithRange->HasUpperLimitActualRotation()) {
          SW.Send(aScrewPairWithRange->UpperLimitActualRotation());
        }
        else SW.SendUndef();
      }
    }
  }
  else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_HighOrderKinematicPair)))
  {
    if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_ScrewPair)))
    {
      SW.StartEntity("POINT_ON_PLANAR_CURVE_PAIR");
      Handle(StepKinematics_PointOnPlanarCurvePair) aPointOnPlanarCurvePair = Handle(StepKinematics_PointOnPlanarCurvePair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of PointOnPlanarCurvePair

      SW.Send(aPointOnPlanarCurvePair->PairCurve());

      SW.SendBoolean(aPointOnPlanarCurvePair->Orientation());

      SW.StartEntity("(POINT_ON_PLANAR_CURVE_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PointOnPlanarCurvePairWithRange)))
      {
        Handle(StepKinematics_PointOnPlanarCurvePairWithRange) aPointOnPlanarCurvePairWithRange = Handle(StepKinematics_PointOnPlanarCurvePairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of PointOnPlanarCurvePairWithRange

        SW.Send(aPointOnPlanarCurvePairWithRange->RangeOnPairCurve());

        if (aPointOnPlanarCurvePairWithRange->HasLowerLimitYaw()) {
          SW.Send(aPointOnPlanarCurvePairWithRange->LowerLimitYaw());
        }
        else SW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasUpperLimitYaw()) {
          SW.Send(aPointOnPlanarCurvePairWithRange->UpperLimitYaw());
        }
        else SW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasLowerLimitPitch()) {
          SW.Send(aPointOnPlanarCurvePairWithRange->LowerLimitPitch());
        }
        else SW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasUpperLimitPitch()) {
          SW.Send(aPointOnPlanarCurvePairWithRange->UpperLimitPitch());
        }
        else SW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasLowerLimitRoll()) {
          SW.Send(aPointOnPlanarCurvePairWithRange->LowerLimitRoll());
        }
        else SW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasUpperLimitRoll()) {
          SW.Send(aPointOnPlanarCurvePairWithRange->UpperLimitRoll());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PointOnSurfacePair)))
    {
      SW.StartEntity("POINT_ON_SURFACE_PAIR");
      Handle(StepKinematics_PointOnSurfacePair) aPointOnSurfacePair = Handle(StepKinematics_PointOnSurfacePair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of PointOnSurfacePair

        SW.Send(aPointOnSurfacePair->PairSurface());

      SW.StartEntity("(SCREW_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PointOnSurfacePairWithRange)))
      {
        Handle(StepKinematics_PointOnSurfacePairWithRange) aPointOnSurfacePairWithRange = Handle(StepKinematics_PointOnSurfacePairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of PointOnSurfacePairWithRange

        SW.Send(aPointOnSurfacePairWithRange->RangeOnPairSurface());

        if (aPointOnSurfacePairWithRange->HasLowerLimitYaw()) {
          SW.Send(aPointOnSurfacePairWithRange->LowerLimitYaw());
        }
        else SW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasUpperLimitYaw()) {
          SW.Send(aPointOnSurfacePairWithRange->UpperLimitYaw());
        }
        else SW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasLowerLimitPitch()) {
          SW.Send(aPointOnSurfacePairWithRange->LowerLimitPitch());
        }
        else SW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasUpperLimitPitch()) {
          SW.Send(aPointOnSurfacePairWithRange->UpperLimitPitch());
        }
        else SW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasLowerLimitRoll()) {
          SW.Send(aPointOnSurfacePairWithRange->LowerLimitRoll());
        }
        else SW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasUpperLimitRoll()) {
          SW.Send(aPointOnSurfacePairWithRange->UpperLimitRoll());
        }
        else SW.SendUndef();
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PlanarCurvePair)))
    {
      SW.StartEntity("PLANAR_CURVE_PAIR");
      Handle(StepKinematics_PlanarCurvePair) aPlanarCurvePair = Handle(StepKinematics_PlanarCurvePair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of PlanarCurvePair

      SW.Send(aPlanarCurvePair->Curve1());

      SW.Send(aPlanarCurvePair->Curve2());

      SW.StartEntity("(PLANAR_CURVE_PAIR_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PlanarCurvePairRange)))
      {
        Handle(StepKinematics_PlanarCurvePairRange) aPlanarCurvePairRange = Handle(StepKinematics_PlanarCurvePairRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of PlanarCurvePairRange

        SW.Send(aPlanarCurvePairRange->RangeOnCurve1());

        SW.Send(aPlanarCurvePairRange->RangeOnCurve2());
      }
    }
    else if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SurfacePair)))
    {
      SW.StartEntity("SURFACE_PAIR");
      Handle(StepKinematics_SurfacePair) aSurfacePair = Handle(StepKinematics_SurfacePair)::DownCast(ent->GetOrderKinematicPair());
      // Own fields of SurfacePair

      SW.Send(aSurfacePair->Surface1());

      SW.Send(aSurfacePair->Surface2());

      SW.SendBoolean(aSurfacePair->Orientation());

      SW.StartEntity("(SURFACE_PAIR_WITH_RANGE");
      if (ent->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SurfacePairWithRange)))
      {
        Handle(StepKinematics_SurfacePairWithRange) aSurfacePairWithRange = Handle(StepKinematics_SurfacePairWithRange)::DownCast(ent->GetOrderKinematicPair());
        // Own fields of SurfacePairWithRange

        SW.Send(aSurfacePairWithRange->RangeOnSurface1());

        SW.Send(aSurfacePairWithRange->RangeOnSurface2());

        if (aSurfacePairWithRange->HasLowerLimitActualRotation()) {
          SW.Send(aSurfacePairWithRange->LowerLimitActualRotation());
        }
        else SW.SendUndef();

        if (aSurfacePairWithRange->HasUpperLimitActualRotation()) {
          SW.Send(aSurfacePairWithRange->UpperLimitActualRotation());
        }
        else SW.SendUndef();
      }
    }
  }

  // Own fields of RepresentationItem
  SW.StartEntity("REPRESENTATION_ITEM");
  SW.Send(ent->Name());
}


//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWActuatedKinPairAndOrderKinPair::Share
  (const Handle(StepKinematics_ActuatedKinPairAndOrderKinPair)& ent,
   Interface_EntityIterator& iter) const
{
  iter.AddItem(ent->GetOrderKinematicPair());
  iter.AddItem(ent->GetOrderKinematicPair()->Joint());
  iter.AddItem(ent->GetActuatedKinematicPair());
  iter.AddItem(ent->GetOrderKinematicPair()->ItemDefinedTransformation());
}
