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
#include <RWStepRepr_RWRepresentationReference.hxx>
#include <StepRepr_RepresentationReference.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationContextReference.hxx>

//=======================================================================
//function : RWStepRepr_RWRepresentationReference
//purpose  : 
//=======================================================================

RWStepRepr_RWRepresentationReference::RWStepRepr_RWRepresentationReference() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationReference::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                     const Standard_Integer num,
                                                     Handle(Interface_Check)& ach,
                                                     const Handle(StepRepr_RepresentationReference)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"representation_reference") ) return;

  // Own fields of RepresentationReference

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(StepRepr_RepresentationContextReference) aContextOfItems;
  data->ReadEntity (num, 2, "context_of_items", ach, STANDARD_TYPE(StepRepr_RepresentationContextReference), aContextOfItems);

  // Initialize entity
  ent->Init(aId,
            aContextOfItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationReference::WriteStep (StepData_StepWriter& SW,
                                                      const Handle(StepRepr_RepresentationReference)& ent) const
{

  // Own fields of RepresentationReference

  SW.Send (ent->Id());

  SW.Send (ent->ContextOfItems());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationReference::Share (const Handle(StepRepr_RepresentationReference)& ent,
                                                  Interface_EntityIterator& iter) const
{

  // Own fields of RepresentationReference

  iter.AddItem (ent->ContextOfItems());
}
