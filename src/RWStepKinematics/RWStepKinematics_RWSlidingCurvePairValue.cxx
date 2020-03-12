// Created on : Sat May 02 12:41:16 2020 
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
#include <RWStepKinematics_RWSlidingCurvePairValue.hxx>
#include <StepKinematics_SlidingCurvePairValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepGeom_PointOnCurve.hxx>

//=======================================================================
//function : RWStepKinematics_RWSlidingCurvePairValue
//purpose  : 
//=======================================================================

RWStepKinematics_RWSlidingCurvePairValue::RWStepKinematics_RWSlidingCurvePairValue() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSlidingCurvePairValue::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepKinematics_SlidingCurvePairValue)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"sliding_curve_pair_value") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of PairValue

  Handle(StepKinematics_KinematicPair) aPairValue_AppliesToPair;
  data->ReadEntity (num, 2, "pair_value.applies_to_pair", ach, STANDARD_TYPE(StepKinematics_KinematicPair), aPairValue_AppliesToPair);

  // Own fields of SlidingCurvePairValue

  Handle(StepGeom_PointOnCurve) aActualPointOnCurve1;
  data->ReadEntity (num, 3, "actual_point_on_curve1", ach, STANDARD_TYPE(StepGeom_PointOnCurve), aActualPointOnCurve1);

  Handle(StepGeom_PointOnCurve) aActualPointOnCurve2;
  data->ReadEntity (num, 4, "actual_point_on_curve2", ach, STANDARD_TYPE(StepGeom_PointOnCurve), aActualPointOnCurve2);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aPairValue_AppliesToPair,
            aActualPointOnCurve1,
            aActualPointOnCurve2);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSlidingCurvePairValue::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepKinematics_SlidingCurvePairValue)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Own fields of PairValue

  SW.Send (ent->AppliesToPair());

  // Own fields of SlidingCurvePairValue

  SW.Send (ent->ActualPointOnCurve1());

  SW.Send (ent->ActualPointOnCurve2());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSlidingCurvePairValue::Share (const Handle(StepKinematics_SlidingCurvePairValue)& ent,
                                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of PairValue

  iter.AddItem (ent->StepKinematics_PairValue::AppliesToPair());

  // Own fields of SlidingCurvePairValue

  iter.AddItem (ent->ActualPointOnCurve1());

  iter.AddItem (ent->ActualPointOnCurve2());
}
