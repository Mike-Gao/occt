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

#ifndef _RWStepRepr_RWRepresentationReference_HeaderFile_
#define _RWStepRepr_RWRepresentationReference_HeaderFile_

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepData_StepReaderData;
class Interface_Check;
class StepData_StepWriter;
class Interface_EntityIterator;
class StepRepr_RepresentationReference;

//! Read & Write tool for RepresentationReference
class RWStepRepr_RWRepresentationReference
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT RWStepRepr_RWRepresentationReference();

  Standard_EXPORT void ReadStep(const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepRepr_RepresentationReference)& ent) const;

  Standard_EXPORT void WriteStep(StepData_StepWriter& SW, const Handle(StepRepr_RepresentationReference)& ent) const;

  Standard_EXPORT void Share(const Handle(StepRepr_RepresentationReference)& ent, Interface_EntityIterator& iter) const;

};
#endif // _RWStepRepr_RWRepresentationReference_HeaderFile_
