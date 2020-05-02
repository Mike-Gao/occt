// Created on : Sat May 02 12:41:14 2020 
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
#include <RWStepRepr_RWRepresentationContextReference.hxx>
#include <StepRepr_RepresentationContextReference.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : RWStepRepr_RWRepresentationContextReference
//purpose  : 
//=======================================================================

RWStepRepr_RWRepresentationContextReference::RWStepRepr_RWRepresentationContextReference() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationContextReference::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepRepr_RepresentationContextReference)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"representation_context_reference") ) return;

  // Own fields of RepresentationContextReference

  Handle(TCollection_HAsciiString) aContextIdentifier;
  data->ReadString (num, 1, "context_identifier", ach, aContextIdentifier);

  // Initialize entity
  ent->Init(aContextIdentifier);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationContextReference::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepRepr_RepresentationContextReference)& ent) const
{

  // Own fields of RepresentationContextReference

  SW.Send (ent->ContextIdentifier());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationContextReference::Share (const Handle(StepRepr_RepresentationContextReference)& /*ent*/,
                                                         Interface_EntityIterator& /*iter*/) const
{

  // Own fields of RepresentationContextReference
}
