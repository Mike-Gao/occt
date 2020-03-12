// Created on : Sat May 02 12:41:15 2020 
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

#ifndef _StepKinematics_ActuatedKinPairAndOrderKinPair_HeaderFile_
#define _StepKinematics_ActuatedKinPairAndOrderKinPair_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <StepKinematics_KinematicPair.hxx>
class StepRepr_RepresentationItem;
class StepKinematics_ActuatedKinematicPair;
class StepKinematics_ActuatedKinPairAndOrderKinPair;

DEFINE_STANDARD_HANDLE(StepKinematics_ActuatedKinPairAndOrderKinPair, StepKinematics_KinematicPair)

//! Representation of STEP entity ActuatedKinPairAndOrderKinPair
class StepKinematics_ActuatedKinPairAndOrderKinPair : public StepKinematics_KinematicPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_ActuatedKinPairAndOrderKinPair();


  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                            const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                            const Standard_Boolean hasItemDefinedTransformation_Description,
                            const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                            const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                            const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                            const Handle(StepKinematics_KinematicJoint)& theJoint,
                            const Handle(StepKinematics_ActuatedKinematicPair)& theActuatedKinematicPair,
                            const Handle(StepKinematics_KinematicPair)& theOrderKinematicPair);
  ////! Initialize all fields for low order kinematic pair
  //Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
  //                          const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
  //                          const Standard_Boolean hasItemDefinedTransformation_Description,
  //                          const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
  //                          const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
  //                          const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
  //                          const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  //                          const Standard_Boolean theLowOrderKinematicPair_TX,
  //                          const Standard_Boolean theLowOrderKinematicPair_TY,
  //                          const Standard_Boolean theLowOrderKinematicPair_TZ,
  //                          const Standard_Boolean theLowOrderKinematicPair_RX,
  //                          const Standard_Boolean theLowOrderKinematicPair_RY,
  //                          const Standard_Boolean theLowOrderKinematicPair_RZ,
  //                          const Standard_Boolean hasUniversalPair_InputSkewAngle,
  //                          const Standard_Real theUniversalPair_InputSkewAngle,
  //                          const Standard_Boolean hasLowerLimitFirstRotation,
  //                          const Standard_Real theLowerLimitFirstRotation,
  //                          const Standard_Boolean hasUpperLimitFirstRotation,
  //                          const Standard_Real theUpperLimitFirstRotation,
  //                          const Standard_Boolean hasLowerLimitSecondRotation,
  //                          const Standard_Real theLowerLimitSecondRotation,
  //                          const Standard_Boolean hasUpperLimitSecondRotation,
  //                          const Standard_Real theUpperLimitSecondRotation);

  ////! Initialize all fields for low order kinematic pair with motions coupling
  //Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
  //                          const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
  //                          const Standard_Boolean hasItemDefinedTransformation_Description,
  //                          const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
  //                          const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
  //                          const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
  //                          const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  //                          const Standard_Real theScrewPair_Pitch,
  //                          const Standard_Real theGearPair_RadiusFirstLink,
  //                          const Standard_Real theGearPair_RadiusSecondLink,
  //                          const Standard_Real theGearPair_Bevel,
  //                          const Standard_Real theGearPair_HelicalAngle,
  //                          const Standard_Real theGearPair_GearRatio,
  //                          const Standard_Boolean hasLowerLimitActualRotation1,
  //                          const Standard_Real theLowerLimitActualRotation1,
  //                          const Standard_Boolean hasUpperLimitActualRotation1,
  //                          const Standard_Real theUpperLimitActualRotation1,
  //                          const Handle(TColStd_HArray1OfReal) theParams);

  ////! Initialize all fields for high order kinematic pair
  //Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
  //                          const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
  //                          const Standard_Boolean hasItemDefinedTransformation_Description,
  //                          const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
  //                          const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
  //                          const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
  //                          const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  //                          const Standard_Boolean theOrientation,
  //                          const Handle(TColStd_HArray1OfReal) theLimits,
  //                          const NCollection_Array1<Handle(Geom_Geometry)> theGeom);

  inline void SetActuatedKinematicPair(const Handle(StepKinematics_ActuatedKinematicPair)& aKP) {
    myActuatedKinematicPair = aKP;
  }

  inline Handle(StepKinematics_ActuatedKinematicPair) GetActuatedKinematicPair() const {
    return myActuatedKinematicPair;
  }
  inline void SetOrderKinematicPair(const Handle(StepKinematics_KinematicPair)& aKP) {
    myOrderKinematicPair = aKP;
  }

  inline Handle(StepKinematics_KinematicPair) GetOrderKinematicPair() const {
    return myOrderKinematicPair;
  }
  ////! Returns field LowerLimitFirstRotation
  //Standard_EXPORT Standard_Real LowerLimitFirstRotation() const;
  ////! Sets field LowerLimitFirstRotation
  //Standard_EXPORT void SetLowerLimitFirstRotation(const Standard_Real theLowerLimitFirstRotation);
  ////! Returns True if optional field LowerLimitFirstRotation is defined
  //Standard_EXPORT Standard_Boolean HasLowerLimitFirstRotation() const;

  ////! Returns field UpperLimitFirstRotation
  //Standard_EXPORT Standard_Real UpperLimitFirstRotation() const;
  ////! Sets field UpperLimitFirstRotation
  //Standard_EXPORT void SetUpperLimitFirstRotation(const Standard_Real theUpperLimitFirstRotation);
  ////! Returns True if optional field UpperLimitFirstRotation is defined
  //Standard_EXPORT Standard_Boolean HasUpperLimitFirstRotation() const;

  ////! Returns field LowerLimitSecondRotation
  //Standard_EXPORT Standard_Real LowerLimitSecondRotation() const;
  ////! Sets field LowerLimitSecondRotation
  //Standard_EXPORT void SetLowerLimitSecondRotation(const Standard_Real theLowerLimitSecondRotation);
  ////! Returns True if optional field LowerLimitSecondRotation is defined
  //Standard_EXPORT Standard_Boolean HasLowerLimitSecondRotation() const;

  ////! Returns field UpperLimitSecondRotation
  //Standard_EXPORT Standard_Real UpperLimitSecondRotation() const;
  ////! Sets field UpperLimitSecondRotation
  //Standard_EXPORT void SetUpperLimitSecondRotation(const Standard_Real theUpperLimitSecondRotation);
  ////! Returns True if optional field UpperLimitSecondRotation is defined
  //Standard_EXPORT Standard_Boolean HasUpperLimitSecondRotation() const;

  ////! Returns field InputSkewAngle
  //Standard_EXPORT Standard_Real InputSkewAngle() const;
  ////! Sets field InputSkewAngle
  //Standard_EXPORT void SetInputSkewAngle(const Standard_Real theInputSkewAngle);
  ////! Returns True if optional field InputSkewAngle is defined
  //Standard_EXPORT Standard_Boolean HasInputSkewAngle() const;


DEFINE_STANDARD_RTTIEXT(StepKinematics_ActuatedKinPairAndOrderKinPair, StepKinematics_KinematicPair)

private:
  Handle(StepKinematics_ActuatedKinematicPair) myActuatedKinematicPair;
  Handle(StepKinematics_KinematicPair) myOrderKinematicPair;
  //Standard_Boolean myTX;
  //Standard_Boolean myTY;
  //Standard_Boolean myTZ;
  //Standard_Boolean myRX;
  //Standard_Boolean myRY;
  //Standard_Boolean myRZ;
  //Standard_Boolean defInputSkewAngle; //!< flag "is InputSkewAngle defined"
  //Standard_Real myMinRotationX;    //!< minimum value of the yaw angle
  //Standard_Real myMaxRotationX;    //!< maximum value of the yaw angle
  //Standard_Real myMinRotationY;    //!< minimum value of the pitch angle
  //Standard_Real myMaxRotationY;    //!< maximum value of the pitch angle
  //Standard_Real myMinRotationZ;    //!< minimum value of the roll angle
  //Standard_Real myMaxRotationZ;    //!< maximum value of the roll angle
  //Standard_Real myMinTranslationX; //!< minimum value of translation in x-direction
  //Standard_Real myMaxTranslationX; //!< maximum value of translation in x-direction
  //Standard_Real myMinTranslationY; //!< minimum value of translation in y-direction
  //Standard_Real myMaxTranslationY; //!< maximum value of translation in y-direction
  //Standard_Real myMinTranslationZ; //!< minimum value of translation in z-direction
  //Standard_Real myMaxTranslationZ; //!< maximum value of translation in z-direction
  //Standard_Real myLowerLimitFirstRotation; //!< optional
  //Standard_Real myUpperLimitFirstRotation; //!< optional
  //Standard_Real myLowerLimitSecondRotation; //!< optional
  //Standard_Real myUpperLimitSecondRotation; //!< optional
  //Standard_Boolean defLowerLimitFirstRotation; //!< flag "is LowerLimitFirstRotation defined"
  //Standard_Boolean defUpperLimitFirstRotation; //!< flag "is UpperLimitFirstRotation defined"
  //Standard_Boolean defLowerLimitSecondRotation; //!< flag "is LowerLimitSecondRotation defined"
  //Standard_Boolean defUpperLimitSecondRotation; //!< flag "is UpperLimitSecondRotation defined"
  //Standard_Real myRadiusFirstLink;
  //Standard_Real myRadiusSecondLink;
  //Standard_Real myBevel;
  //Standard_Real myHelicalAngle;
  //Standard_Real myGearRatio;
  //Standard_Real mySkewAngle;       //!< param for universal/homokinetic pair
  //Standard_Real myLowLimit;               //!< low limit of motion range
  //Standard_Real myUpperLimit;             //!< upper limit of motion range
  //Handle(TColStd_HArray1OfReal) myParams; //!< additional parameters of kinematic pair
  //Standard_Boolean myOrientation;                   //!< orientation
  //Handle(TColStd_HArray1OfReal) myLimits;           //!< array of limits, size depends on type
  //NCollection_Array1<Handle(Geom_Geometry)> myGeom; //!< curve(s) or surface(s) attributes
};
#endif // StepKinematics_ActuatedKinPairAndOrderKinPair
