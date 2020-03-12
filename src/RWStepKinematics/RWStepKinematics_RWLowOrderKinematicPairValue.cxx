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
#include <RWStepKinematics_RWLowOrderKinematicPairValue.hxx>
#include <StepKinematics_LowOrderKinematicPairValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWLowOrderKinematicPairValue
//purpose  : 
//=======================================================================

RWStepKinematics_RWLowOrderKinematicPairValue::RWStepKinematics_RWLowOrderKinematicPairValue() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLowOrderKinematicPairValue::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepKinematics_LowOrderKinematicPairValue)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,8,ach,"low_order_kinematic_pair_value") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of PairValue

  Handle(StepKinematics_KinematicPair) aPairValue_AppliesToPair;
  data->ReadEntity (num, 2, "pair_value.applies_to_pair", ach, STANDARD_TYPE(StepKinematics_KinematicPair), aPairValue_AppliesToPair);

  // Own fields of LowOrderKinematicPairValue

  Standard_Real aActualTranslationX;
  data->ReadReal (num, 3, "actual_translation_x", ach, aActualTranslationX);

  Standard_Real aActualTranslationY;
  data->ReadReal (num, 4, "actual_translation_y", ach, aActualTranslationY);

  Standard_Real aActualTranslationZ;
  data->ReadReal (num, 5, "actual_translation_z", ach, aActualTranslationZ);

  Standard_Real aActualRotationX;
  data->ReadReal (num, 6, "actual_rotation_x", ach, aActualRotationX);

  Standard_Real aActualRotationY;
  data->ReadReal (num, 7, "actual_rotation_y", ach, aActualRotationY);

  Standard_Real aActualRotationZ;
  data->ReadReal (num, 8, "actual_rotation_z", ach, aActualRotationZ);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aPairValue_AppliesToPair,
            aActualTranslationX,
            aActualTranslationY,
            aActualTranslationZ,
            aActualRotationX,
            aActualRotationY,
            aActualRotationZ);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLowOrderKinematicPairValue::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepKinematics_LowOrderKinematicPairValue)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Own fields of PairValue

  SW.Send (ent->AppliesToPair());

  // Own fields of LowOrderKinematicPairValue

  SW.Send (ent->ActualTranslationX());

  SW.Send (ent->ActualTranslationY());

  SW.Send (ent->ActualTranslationZ());

  SW.Send (ent->ActualRotationX());

  SW.Send (ent->ActualRotationY());

  SW.Send (ent->ActualRotationZ());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWLowOrderKinematicPairValue::Share (const Handle(StepKinematics_LowOrderKinematicPairValue)& ent,
                                                           Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of PairValue

  iter.AddItem (ent->StepKinematics_PairValue::AppliesToPair());

  // Own fields of LowOrderKinematicPairValue
}
