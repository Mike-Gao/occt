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
#include <RWStepKinematics_RWSphericalPairValue.hxx>
#include <StepKinematics_SphericalPairValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepKinematics_SpatialRotation.hxx>

//=======================================================================
//function : RWStepKinematics_RWSphericalPairValue
//purpose  : 
//=======================================================================

RWStepKinematics_RWSphericalPairValue::RWStepKinematics_RWSphericalPairValue() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSphericalPairValue::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepKinematics_SphericalPairValue)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"spherical_pair_value") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of PairValue

  Handle(StepKinematics_KinematicPair) aPairValue_AppliesToPair;
  data->ReadEntity (num, 2, "pair_value.applies_to_pair", ach, STANDARD_TYPE(StepKinematics_KinematicPair), aPairValue_AppliesToPair);


  // Own fields of SphericalPairValue
  StepKinematics_SpatialRotation aInputOrientation;
  if (data->SubListNumber(num, 3, Standard_True))
  {
    Handle(TColStd_HArray1OfReal) aItems;
    Standard_Integer nsub = 0;
    if (data->ReadSubList(num, 3, "items", ach, nsub)) {
      Standard_Integer nb = data->NbParams(nsub);
      aItems = new TColStd_HArray1OfReal(1, nb);
      Standard_Integer num2 = nsub;
      for (Standard_Integer i0 = 1; i0 <= nb; i0++) {
        Standard_Real anIt0;
        data->ReadReal(num2, i0, "real", ach, anIt0);
        aItems->SetValue(i0, anIt0);
      }
    }
    aInputOrientation.SetValue(aItems);
  }
  else 
    data->ReadEntity (num, 3, "input_orientation", ach, aInputOrientation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aPairValue_AppliesToPair,
            aInputOrientation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSphericalPairValue::WriteStep(StepData_StepWriter& SW,
  const Handle(StepKinematics_SphericalPairValue)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send(ent->Name());

  // Own fields of PairValue

  SW.Send(ent->AppliesToPair());

  // Own fields of SphericalPairValue

  if (!ent->InputOrientation().YprRotation().IsNull())
  {
    // Inherited field : YPR
    SW.OpenSub();
    for (Standard_Integer i = 1; i <= ent->InputOrientation().YprRotation()->Length(); i++) {
      SW.Send(ent->InputOrientation().YprRotation()->Value(i));
    }
    SW.CloseSub();
  }
  else
    SW.Send(ent->InputOrientation().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSphericalPairValue::Share (const Handle(StepKinematics_SphericalPairValue)& ent,
                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of PairValue

  iter.AddItem (ent->StepKinematics_PairValue::AppliesToPair());

  // Own fields of SphericalPairValue

  if (!ent->InputOrientation().RotationAboutDirection().IsNull())
    iter.AddItem(ent->InputOrientation().Value());
}
