// Created on : Mon Apr 13 15:22:03 2020 
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

#ifndef _RWStepKinematics_RWRollingCurvePairValue_HeaderFile_
#define _RWStepKinematics_RWRollingCurvePairValue_HeaderFile_

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepData_StepReaderData;
class Interface_Check;
class StepData_StepWriter;
class Interface_EntityIterator;
class StepKinematics_RollingCurvePairValue;

//! Read & Write tool for RollingCurvePairValue
class RWStepKinematics_RWRollingCurvePairValue
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT RWStepKinematics_RWRollingCurvePairValue();

  Standard_EXPORT void ReadStep(const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepKinematics_RollingCurvePairValue)& ent) const;

  Standard_EXPORT void WriteStep(StepData_StepWriter& SW, const Handle(StepKinematics_RollingCurvePairValue)& ent) const;

  Standard_EXPORT void Share(const Handle(StepKinematics_RollingCurvePairValue)& ent, Interface_EntityIterator& iter) const;

};
#endif // _RWStepKinematics_RWRollingCurvePairValue_HeaderFile_
