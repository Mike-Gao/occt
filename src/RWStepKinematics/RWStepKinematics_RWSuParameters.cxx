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
#include <RWStepKinematics_RWSuParameters.hxx>
#include <StepKinematics_SuParameters.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWSuParameters
//purpose  : 
//=======================================================================

RWStepKinematics_RWSuParameters::RWStepKinematics_RWSuParameters() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSuParameters::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepKinematics_SuParameters)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,7,ach,"su_parameters") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of SuParameters

  Standard_Real aA;
  data->ReadReal (num, 2, "a", ach, aA);

  Standard_Real aAlpha;
  data->ReadReal (num, 3, "alpha", ach, aAlpha);

  Standard_Real aB;
  data->ReadReal (num, 4, "b", ach, aB);

  Standard_Real aBeta;
  data->ReadReal (num, 5, "beta", ach, aBeta);

  Standard_Real aC;
  data->ReadReal (num, 6, "c", ach, aC);

  Standard_Real aGamma;
  data->ReadReal (num, 7, "gamma", ach, aGamma);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aA,
            aAlpha,
            aB,
            aBeta,
            aC,
            aGamma);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSuParameters::WriteStep (StepData_StepWriter& SW,
                                                 const Handle(StepKinematics_SuParameters)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Own fields of SuParameters

  SW.Send (ent->A());

  SW.Send (ent->Alpha());

  SW.Send (ent->B());

  SW.Send (ent->Beta());

  SW.Send (ent->C());

  SW.Send (ent->Gamma());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSuParameters::Share (const Handle(StepKinematics_SuParameters)& /*ent*/,
                                             Interface_EntityIterator& /*iter*/) const
{

  // Inherited fields of RepresentationItem

  // Own fields of SuParameters
}
