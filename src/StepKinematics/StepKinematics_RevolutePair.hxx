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

#ifndef _StepKinematics_RevolutePair_HeaderFile_
#define _StepKinematics_RevolutePair_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_LowOrderKinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_RevolutePair, StepKinematics_LowOrderKinematicPair)

//! Representation of STEP entity RevolutePair
class StepKinematics_RevolutePair : public StepKinematics_LowOrderKinematicPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_RevolutePair();

DEFINE_STANDARD_RTTIEXT(StepKinematics_RevolutePair, StepKinematics_LowOrderKinematicPair)

};
#endif // _StepKinematics_RevolutePair_HeaderFile_
