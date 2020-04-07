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

#include <StepKinematics_RackAndPinionPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_RackAndPinionPair, StepKinematics_LowOrderKinematicPairWithMotionCoupling)

//=======================================================================
//function : StepKinematics_RackAndPinionPair
//purpose  : 
//=======================================================================

StepKinematics_RackAndPinionPair::StepKinematics_RackAndPinionPair ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepKinematics_RackAndPinionPair::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                             const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                                             const Standard_Boolean hasItemDefinedTransformation_Description,
                                             const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                                             const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                                             const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                                             const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                                             const Standard_Real thePinionRadius)
{
  StepKinematics_LowOrderKinematicPairWithMotionCoupling::Init(theRepresentationItem_Name,
                                                               theItemDefinedTransformation_Name,
                                                               hasItemDefinedTransformation_Description,
                                                               theItemDefinedTransformation_Description,
                                                               theItemDefinedTransformation_TransformItem1,
                                                               theItemDefinedTransformation_TransformItem2,
                                                               theKinematicPair_Joint);

  myPinionRadius = thePinionRadius;
}

//=======================================================================
//function : PinionRadius
//purpose  : 
//=======================================================================

Standard_Real StepKinematics_RackAndPinionPair::PinionRadius () const
{
  return myPinionRadius;
}

//=======================================================================
//function : SetPinionRadius
//purpose  : 
//=======================================================================

void StepKinematics_RackAndPinionPair::SetPinionRadius (const Standard_Real thePinionRadius)
{
  myPinionRadius = thePinionRadius;
}
