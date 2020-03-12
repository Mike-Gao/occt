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

#include <StepKinematics_UnconstrainedPairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_UnconstrainedPairValue, StepKinematics_PairValue)

//=======================================================================
//function : StepKinematics_UnconstrainedPairValue
//purpose  : 
//=======================================================================

StepKinematics_UnconstrainedPairValue::StepKinematics_UnconstrainedPairValue ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepKinematics_UnconstrainedPairValue::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                                  const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                                                  const Handle(StepGeom_Axis2Placement3d)& theActualPlacement)
{
  StepKinematics_PairValue::Init(theRepresentationItem_Name,
                                 thePairValue_AppliesToPair);

  myActualPlacement = theActualPlacement;
}

//=======================================================================
//function : ActualPlacement
//purpose  : 
//=======================================================================

Handle(StepGeom_Axis2Placement3d) StepKinematics_UnconstrainedPairValue::ActualPlacement () const
{
  return myActualPlacement;
}

//=======================================================================
//function : SetActualPlacement
//purpose  : 
//=======================================================================

void StepKinematics_UnconstrainedPairValue::SetActualPlacement (const Handle(StepGeom_Axis2Placement3d)& theActualPlacement)
{
  myActualPlacement = theActualPlacement;
}
