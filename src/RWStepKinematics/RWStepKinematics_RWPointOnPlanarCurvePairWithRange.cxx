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
#include <RWStepKinematics_RWPointOnPlanarCurvePairWithRange.hxx>
#include <StepKinematics_PointOnPlanarCurvePairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Curve.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWPointOnPlanarCurvePairWithRange
//purpose  : 
//=======================================================================

RWStepKinematics_RWPointOnPlanarCurvePairWithRange::RWStepKinematics_RWPointOnPlanarCurvePairWithRange() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPointOnPlanarCurvePairWithRange::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                   const Standard_Integer num,
                                                                   Handle(Interface_Check)& ach,
                                                                   const Handle(StepKinematics_PointOnPlanarCurvePairWithRange)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,15,ach,"point_on_planar_curve_pair_with_range") ) return;

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

  // Inherited fields of PointOnPlanarCurvePair

  Handle(StepGeom_Curve) aPointOnPlanarCurvePair_PairCurve;
  data->ReadEntity (num, 7, "point_on_planar_curve_pair.pair_curve", ach, STANDARD_TYPE(StepGeom_Curve), aPointOnPlanarCurvePair_PairCurve);

  Standard_Boolean aPointOnPlanarCurvePair_Orientation;
  data->ReadBoolean (num, 8, "point_on_planar_curve_pair.orientation", ach, aPointOnPlanarCurvePair_Orientation);

  // Own fields of PointOnPlanarCurvePairWithRange

  Handle(StepGeom_TrimmedCurve) aRangeOnPairCurve;
  data->ReadEntity (num, 9, "range_on_pair_curve", ach, STANDARD_TYPE(StepGeom_TrimmedCurve), aRangeOnPairCurve);

  Standard_Real aLowerLimitYaw;
  Standard_Boolean hasLowerLimitYaw = Standard_True;
  if ( data->IsParamDefined (num,10) ) {
    data->ReadReal (num, 10, "lower_limit_yaw", ach, aLowerLimitYaw);
  }
  else {
    hasLowerLimitYaw = Standard_False;
    aLowerLimitYaw = 0;
  }

  Standard_Real aUpperLimitYaw;
  Standard_Boolean hasUpperLimitYaw = Standard_True;
  if ( data->IsParamDefined (num,11) ) {
    data->ReadReal (num, 11, "upper_limit_yaw", ach, aUpperLimitYaw);
  }
  else {
    hasUpperLimitYaw = Standard_False;
    aUpperLimitYaw = 0;
  }

  Standard_Real aLowerLimitPitch;
  Standard_Boolean hasLowerLimitPitch = Standard_True;
  if ( data->IsParamDefined (num,12) ) {
    data->ReadReal (num, 12, "lower_limit_pitch", ach, aLowerLimitPitch);
  }
  else {
    hasLowerLimitPitch = Standard_False;
    aLowerLimitPitch = 0;
  }

  Standard_Real aUpperLimitPitch;
  Standard_Boolean hasUpperLimitPitch = Standard_True;
  if ( data->IsParamDefined (num,13) ) {
    data->ReadReal (num, 13, "upper_limit_pitch", ach, aUpperLimitPitch);
  }
  else {
    hasUpperLimitPitch = Standard_False;
    aUpperLimitPitch = 0;
  }

  Standard_Real aLowerLimitRoll;
  Standard_Boolean hasLowerLimitRoll = Standard_True;
  if ( data->IsParamDefined (num,14) ) {
    data->ReadReal (num, 14, "lower_limit_roll", ach, aLowerLimitRoll);
  }
  else {
    hasLowerLimitRoll = Standard_False;
    aLowerLimitRoll = 0;
  }

  Standard_Real aUpperLimitRoll;
  Standard_Boolean hasUpperLimitRoll = Standard_True;
  if ( data->IsParamDefined (num,15) ) {
    data->ReadReal (num, 15, "upper_limit_roll", ach, aUpperLimitRoll);
  }
  else {
    hasUpperLimitRoll = Standard_False;
    aUpperLimitRoll = 0;
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
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

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPointOnPlanarCurvePairWithRange::WriteStep (StepData_StepWriter& SW,
                                                                    const Handle(StepKinematics_PointOnPlanarCurvePairWithRange)& ent) const
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

  // Own fields of PointOnPlanarCurvePair

  SW.Send (ent->PairCurve());

  SW.SendBoolean (ent->Orientation());

  // Own fields of PointOnPlanarCurvePairWithRange

  SW.Send (ent->RangeOnPairCurve());

  if ( ent->HasLowerLimitYaw() ) {
    SW.Send (ent->LowerLimitYaw());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitYaw() ) {
    SW.Send (ent->UpperLimitYaw());
  }
  else SW.SendUndef();

  if ( ent->HasLowerLimitPitch() ) {
    SW.Send (ent->LowerLimitPitch());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitPitch() ) {
    SW.Send (ent->UpperLimitPitch());
  }
  else SW.SendUndef();

  if ( ent->HasLowerLimitRoll() ) {
    SW.Send (ent->LowerLimitRoll());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitRoll() ) {
    SW.Send (ent->UpperLimitRoll());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPointOnPlanarCurvePairWithRange::Share (const Handle(StepKinematics_PointOnPlanarCurvePairWithRange)& ent,
                                                                Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (ent->StepKinematics_KinematicPair::Joint());

  // Inherited fields of PointOnPlanarCurvePair

  iter.AddItem (ent->StepKinematics_PointOnPlanarCurvePair::PairCurve());

  // Own fields of PointOnPlanarCurvePairWithRange

  iter.AddItem (ent->RangeOnPairCurve());
}
