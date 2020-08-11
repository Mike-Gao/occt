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


#ifndef _XCAFKinematics_LowOrderPairObject_HeaderFile
#define _XCAFKinematics_LowOrderPairObject_HeaderFile

#include <XCAFKinematics_PairObject.hxx>

class TColStd_HArray1OfReal;

DEFINE_STANDARD_HANDLE(XCAFKinematics_LowOrderPairObject, XCAFKinematics_PairObject)

//! \class XCAFKinematics_LowOrderPairObject
//! \brief Object for low order kinematic pairs: 
//! - XCAFKinematics_PairType_FullyConstrained <F, F, F, F, F, F>
//! - XCAFKinematics_PairType_Revolute         <F, F, T, F, F, F>
//! - XCAFKinematics_PairType_Prismatic        <F, F, F, T, F, F>
//! - XCAFKinematics_PairType_Cylindrical      <F, F, T, F, F, T>
//! - XCAFKinematics_PairType_Universal        <T, F, T, F, F, F>
//! - XCAFKinematics_PairType_Homokinetic      <T, F, T, F, F, F>
//! - XCAFKinematics_PairType_SphericalWithPin <F, T, T, F, F, F>
//! - XCAFKinematics_PairType_Spherical        <T, T, T, F, F, F>
//! - XCAFKinematics_PairType_Planar           <T, F, F, T, T, F>
//! - XCAFKinematics_PairType_Unconstrained    <T, T, T, T, T, T>
//! Not all limits are used for each pair, 
//! values should be specified in alphabetical order: <rX, rY, rZ, tX, tY, tZ>

class XCAFKinematics_LowOrderPairObject : public XCAFKinematics_PairObject
{

public:
  
  Standard_EXPORT XCAFKinematics_LowOrderPairObject();
  
  Standard_EXPORT XCAFKinematics_LowOrderPairObject
    (const Handle(XCAFKinematics_LowOrderPairObject)& theObj);

  // X rotation limits

  void SetMinRotationX(const Standard_Real theLimit)
  {
    myMinRotationX = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MinRotationX() const
  {
    return myMinRotationX;
  }

  void SetMaxRotationX(const Standard_Real theLimit)
  {
    myMaxRotationX = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MaxRotationX() const
  {
    return myMaxRotationX;
  }

  // Y rotation limits

  void SetMinRotationY(const Standard_Real theLimit)
  {
    myMinRotationY = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MinRotationY() const
  {
    return myMinRotationY;
  }

  void SetMaxRotationY(const Standard_Real theLimit)
  {
    myMaxRotationY = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MaxRotationY() const
  {
    return myMaxRotationY;
  }

  // Z rotation limits

  void SetMinRotationZ(const Standard_Real theLimit)
  {
    myMinRotationZ = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MinRotationZ() const
  {
    return myMinRotationZ;
  }

  void SetMaxRotationZ(const Standard_Real theLimit)
  {
    myMaxRotationZ = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MaxRotationZ() const
  {
    return myMaxRotationZ;
  }

  // X translation limits

  void SetMinTranslationX(const Standard_Real theLimit)
  {
    myMinTranslationX = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MinTranslationX() const
  {
    return myMinTranslationX;
  }

  void SetMaxTranslationX(const Standard_Real theLimit)
  {
    myMaxTranslationX = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MaxTranslationX() const
  {
    return myMaxTranslationX;
  }

  // Y translation limits

  void SetMinTranslationY(const Standard_Real theLimit)
  {
    myMinTranslationY = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MinTranslationY() const
  {
    return myMinTranslationY;
  }

  void SetMaxTranslationY(const Standard_Real theLimit)
  {
    myMaxTranslationY = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MaxTranslationY() const
  {
    return myMaxTranslationY;
  }

  // Z translation limits

  void SetMinTranslationZ(const Standard_Real theLimit)
  {
    myMinTranslationZ = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MinTranslationZ() const
  {
    return myMinTranslationZ;
  }

  void SetMaxTranslationZ(const Standard_Real theLimit)
  {
    myMaxTranslationZ = theLimit;
    myIsRanged = Standard_True;
  }

  Standard_Real MaxTranslationZ() const
  {
    return myMaxTranslationZ;
  }

  void SetSkewAngle(const Standard_Real theAngle)
  {
    if (Type() == XCAFKinematics_PairType_Universal || Type() == XCAFKinematics_PairType_Homokinetic)
      mySkewAngle = theAngle;
  }

  Standard_Real SkewAngle() const
  {
    if (Type() == XCAFKinematics_PairType_Universal || Type() == XCAFKinematics_PairType_Homokinetic)
      return mySkewAngle;
    else return 0;
  }

  //! Creates array with all limits
  //! \return created array
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetAllLimits() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean HasLimits() const Standard_OVERRIDE;

  Standard_EXPORT void SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XCAFKinematics_LowOrderPairObject, XCAFKinematics_PairObject)

private:

  Standard_Real myMinRotationX;    //!< minimum value of the yaw angle
  Standard_Real myMaxRotationX;    //!< maximum value of the yaw angle
  Standard_Real myMinRotationY;    //!< minimum value of the pitch angle
  Standard_Real myMaxRotationY;    //!< maximum value of the pitch angle
  Standard_Real myMinRotationZ;    //!< minimum value of the roll angle
  Standard_Real myMaxRotationZ;    //!< maximum value of the roll angle
  Standard_Real myMinTranslationX; //!< minimum value of translation in x-direction
  Standard_Real myMaxTranslationX; //!< maximum value of translation in x-direction
  Standard_Real myMinTranslationY; //!< minimum value of translation in y-direction
  Standard_Real myMaxTranslationY; //!< maximum value of translation in y-direction
  Standard_Real myMinTranslationZ; //!< minimum value of translation in z-direction
  Standard_Real myMaxTranslationZ; //!< maximum value of translation in z-direction
  Standard_Real mySkewAngle;       //!< param for universal/homokinetic pair
  Standard_Boolean myIsRanged;     //!< flag "is limits defined"

};

#endif // _XCAFKinematics_LowOrderPairObject_HeaderFile
