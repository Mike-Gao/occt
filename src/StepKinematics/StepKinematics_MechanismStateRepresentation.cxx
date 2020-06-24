// Created on: 2020-06-18
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


#include <Standard_Type.hxx>
#include <StepKinematics_MechanismStateRepresentation.hxx>
#include <StepKinematics_MechanismRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_MechanismStateRepresentation, StepRepr_Representation)

StepKinematics_MechanismStateRepresentation::StepKinematics_MechanismStateRepresentation()  {}

void StepKinematics_MechanismStateRepresentation::Init(const Handle(TCollection_HAsciiString)& aName,
	                                                     const Handle(StepRepr_HArray1OfRepresentationItem)& aItems,
	                                                     const Handle(StepRepr_RepresentationContext)& aContextOfItems,
	                                                     const Handle(StepKinematics_MechanismRepresentation) aMechanism)
{
	StepRepr_Representation::Init(aName, aItems, aContextOfItems);
	representedMechanism = aMechanism;
}

void StepKinematics_MechanismStateRepresentation::SetMechanism(const Handle(StepKinematics_MechanismRepresentation)& aMechanism)
{
  representedMechanism = aMechanism;
}

Handle(StepKinematics_MechanismRepresentation) StepKinematics_MechanismStateRepresentation::Mechanism() const
{
  return representedMechanism;
}