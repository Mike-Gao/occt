// Created on: 2020-03-16
// Created by: Irina KRYLOVA
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


#ifndef _XCAFKinematics_PairType_HeaderFile
#define _XCAFKinematics_PairType_HeaderFile

//! Defines types of kinemtic pairs
enum XCAFKinematics_PairType
{
  // Auxiliary type
  XCAFKinematics_PairType_NoType,

  // Low order pairs
  XCAFKinematics_PairType_FullyConstrained, // no DOF
  XCAFKinematics_PairType_Revolute, // one rotation DOF
  XCAFKinematics_PairType_Prismatic, // one translation DOF
  XCAFKinematics_PairType_Cylindrical, // one rotation and one translation DOF
  XCAFKinematics_PairType_Universal, // two rotation DOF
  XCAFKinematics_PairType_Homokinetic, // two uniform rotation DOF
  XCAFKinematics_PairType_SphericalWithPin, // two rotation DOF
  XCAFKinematics_PairType_Spherical, // three rotation DOF
  XCAFKinematics_PairType_Planar, // one rotation and two translation DOF
  XCAFKinematics_PairType_Unconstrained, // three rotation and three translation DOF

  // Low order pairs with motion coupling
  XCAFKinematics_PairType_Screw,
  XCAFKinematics_PairType_RackAndPinion,
  XCAFKinematics_PairType_Gear,
  XCAFKinematics_LinearFlexibleAndPinionPair,

  // High order pairs
  XCAFKinematics_PairType_PointOnSurface,
  XCAFKinematics_PairType_SlidingSurface,
  XCAFKinematics_PairType_RollingSurface,
  XCAFKinematics_PairType_PointOnPlanarCurve,
  XCAFKinematics_PairType_SlidingCurve,
  XCAFKinematics_PairType_RollingCurve,
  XCAFKinematics_PairType_LinearFlexibleAndPlanarCurvePair
};

#endif // _XCAFKinematics_PairType_HeaderFile
