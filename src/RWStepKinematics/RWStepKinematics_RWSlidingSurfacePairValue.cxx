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
#include <RWStepKinematics_RWSlidingSurfacePairValue.hxx>
#include <StepKinematics_SlidingSurfacePairValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepGeom_PointOnSurface.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWSlidingSurfacePairValue
//purpose  : 
//=======================================================================

RWStepKinematics_RWSlidingSurfacePairValue::RWStepKinematics_RWSlidingSurfacePairValue() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSlidingSurfacePairValue::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                           const Standard_Integer num,
                                                           Handle(Interface_Check)& ach,
                                                           const Handle(StepKinematics_SlidingSurfacePairValue)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"sliding_surface_pair_value") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of PairValue

  Handle(StepKinematics_KinematicPair) aPairValue_AppliesToPair;
  data->ReadEntity (num, 2, "pair_value.applies_to_pair", ach, STANDARD_TYPE(StepKinematics_KinematicPair), aPairValue_AppliesToPair);

  // Own fields of SlidingSurfacePairValue

  Handle(StepGeom_PointOnSurface) aActualPointOnSurface1;
  data->ReadEntity (num, 3, "actual_point_on_surface1", ach, STANDARD_TYPE(StepGeom_PointOnSurface), aActualPointOnSurface1);

  Handle(StepGeom_PointOnSurface) aActualPointOnSurface2;
  data->ReadEntity (num, 4, "actual_point_on_surface2", ach, STANDARD_TYPE(StepGeom_PointOnSurface), aActualPointOnSurface2);

  Standard_Real aActualRotation;
  data->ReadReal (num, 5, "actual_rotation", ach, aActualRotation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aPairValue_AppliesToPair,
            aActualPointOnSurface1,
            aActualPointOnSurface2,
            aActualRotation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSlidingSurfacePairValue::WriteStep (StepData_StepWriter& SW,
                                                            const Handle(StepKinematics_SlidingSurfacePairValue)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Own fields of PairValue

  SW.Send (ent->AppliesToPair());

  // Own fields of SlidingSurfacePairValue

  SW.Send (ent->ActualPointOnSurface1());

  SW.Send (ent->ActualPointOnSurface2());

  SW.Send (ent->ActualRotation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSlidingSurfacePairValue::Share (const Handle(StepKinematics_SlidingSurfacePairValue)& ent,
                                                        Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of PairValue

  iter.AddItem (ent->StepKinematics_PairValue::AppliesToPair());

  // Own fields of SlidingSurfacePairValue

  iter.AddItem (ent->ActualPointOnSurface1());

  iter.AddItem (ent->ActualPointOnSurface2());
}
