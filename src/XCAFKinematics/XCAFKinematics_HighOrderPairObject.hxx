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


#ifndef _XCAFKinematics_HighOrderPairObject_HeaderFile
#define _XCAFKinematics_HighOrderPairObject_HeaderFile

#include <Geom_Geometry.hxx>
#include <XCAFKinematics_PairObject.hxx>
#include <TColStd_HArray1OfReal.hxx>

class Geom_Curve;
class Geom_Surface;
class Geom_RectangularTrimmedSurface;
class Geom_TrimmedCurve;

DEFINE_STANDARD_HANDLE(XCAFKinematics_HighOrderPairObject, XCAFKinematics_PairObject)

//! \class XCAFKinematics_HighOrderPairObject
//! \brief Object for high order kinematic pairs:
//! - XCAFKinematics_PairType_PointOnSurface
//! - XCAFKinematics_PairType_SlidingSurface
//! - XCAFKinematics_PairType_RollingSurface
//! - XCAFKinematics_PairType_PointOnPlanarCurve
//! - XCAFKinematics_PairType_SlidingCurve
//! - XCAFKinematics_PairType_RollingCurve
//! - XCAFKinematics_PairType_RollingCurve
//! - XCAFKinematics_PairType_LinearFlexibleAndPlanarCurve
class XCAFKinematics_HighOrderPairObject : public XCAFKinematics_PairObject
{

public:
  
  Standard_EXPORT XCAFKinematics_HighOrderPairObject();
  
  Standard_EXPORT XCAFKinematics_HighOrderPairObject
    (const Handle(XCAFKinematics_HighOrderPairObject)& theObj);

  Standard_EXPORT void SetType(const XCAFKinematics_PairType theType) Standard_OVERRIDE;

  void SetOrientation(const Standard_Boolean theOrientation)
  {
    myOrientation = theOrientation;
  }

  Standard_Boolean Orientation() const
  {
    return myOrientation;
  }

  Standard_EXPORT void SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits) Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean HasLimits() const Standard_OVERRIDE;

  Handle(TColStd_HArray1OfReal) GetAllLimits() const Standard_OVERRIDE
  {
    Handle(TColStd_HArray1OfReal) aLimitArray;
    if (HasLimits())
      aLimitArray = myLimits;
    return aLimitArray;
  }

  //! Sets low limit of rotation attribute (only for SlidingSurface and RollingSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetRotationLowLimit(const Standard_Real theLimit);

  //! Gets low rotation limit (only for SlidingSurface and RollingSurface)
  //! \return limit
  Standard_EXPORT Standard_Real RotationLowLimit();

  //! Sets upper limit of rotation attribute (only for SlidingSurface and RollingSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetRotationUpperLimit(const Standard_Real theLimit);

  //! Gets upper rotation limit (only for SlidingSurface and RollingSurface)
  //! \return limit
  Standard_EXPORT Standard_Real RotationUpperLimit();

  //! Sets low limit of yaw angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetLowLimitYaw(const Standard_Real theLimit);

  //! Gets low limit of yaw angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \return limit
  Standard_EXPORT Standard_Real LowLimitYaw();

  //! Sets upper limit of yaw angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetUpperLimitYaw(const Standard_Real theLimit);

  //! Gets upper limit of yaw angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \return limit
  Standard_EXPORT Standard_Real UpperLimitYaw();

  //! Sets low limit of roll angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetLowLimitRoll(const Standard_Real theLimit);

  //! Gets low limit of roll angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \return limit
  Standard_EXPORT Standard_Real LowLimitRoll();

  //! Sets upper limit of roll angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetUpperLimitRoll(const Standard_Real theLimit);

  //! Gets upper limit of roll angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \return limit
  Standard_EXPORT Standard_Real UpperLimitRoll();

  //! Sets low limit of pitch angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetLowLimitPitch(const Standard_Real theLimit);

  //! Gets low limit of pitch angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \return limit
  Standard_EXPORT Standard_Real LowLimitPitch();

  //! Sets upper limit of pitch angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \param[in] theLimit limit to set
  Standard_EXPORT void SetUpperLimitPitch(const Standard_Real theLimit);

  //! Gets upper limit of pitch angle (only for PointOnPlanarCurve and PointOnSurface)
  //! \return limit
  Standard_EXPORT Standard_Real UpperLimitPitch();

  //! Sets curve attribute (only for PointOnPlanarCurve)
  //! \param[in] theCurve curve
  Standard_EXPORT void SetCurve(const Handle(Geom_Curve)& theCurve);

  //! Gets curve attribute (only for PointOnPlanarCurve)
  //! \return curve
  Standard_EXPORT Handle(Geom_Curve) Curve() const;

  //! Sets curve attribute (only for SlidingCurve and RollingCurve)
  //! \param[in] theCurve curve
  Standard_EXPORT void SetFirstCurve(const Handle(Geom_Curve)& theCurve);

  //! Gets curve attribute (only for SlidingCurve and RollingCurve)
  //! \return curve
  Standard_EXPORT Handle(Geom_Curve) FirstCurve() const;

  //! Sets curve attribute (only for SlidingCurve and RollingCurve)
  //! \param[in] theCurve curve
  Standard_EXPORT void SetSecondCurve(const Handle(Geom_Curve)& theCurve);

  //! Gets curve attribute (only for SlidingCurve and RollingCurve)
  //! \return curve
  Standard_EXPORT Handle(Geom_Curve) SecondCurve() const;

  //! Sets curve attribute (only for PointOnCurve)
  //! \param[in] theTrimmedCurve trimmed curve
  //Standard_EXPORT void SetTrimmedCurve(const Standard_Real& theTrim1, const Standard_Real& theTrim2);

  //! Sets trimmed curve attribute (only for PointOnCurve)
  //! \param[in] aTrimCurve trimmed curve
  Standard_EXPORT void SetTrimmedCurve(const Handle(Geom_TrimmedCurve)& aTrimCurve);

  //! Gets trimmed curve attribute (only for PointOnCurve)
  //! \return trimmed curve
  Standard_EXPORT Handle(Geom_TrimmedCurve) TrimmedCurve() const;

  //! \return trimmed curve
  //Standard_EXPORT void TrimmedCurve(Standard_Real& theTrim1,Standard_Real& theTrim2) const;

  //! Sets surface attribute (only for PointOnSurface)
  //! \param[in] theSurface surface
  Standard_EXPORT void SetSurface(const Handle(Geom_Surface)& theSurface);

  //! Gets surface attribute (only for PointOnSurface)
  //! \return curve
  Standard_EXPORT Handle(Geom_Surface) Surface() const;

  //! Sets surface attribute (only for SlidingSurface and RollingSurface)
  //! \param[in] theSurface surface
  Standard_EXPORT void SetFirstSurface(const Handle(Geom_Surface)& theSurface);

  //! Gets surface attribute (only for SlidingSurface and RollingSurface)
  //! \return surface
  Standard_EXPORT Handle(Geom_Surface) FirstSurface() const;

  //! Sets surface attribute (only for SlidingSurface and RollingSurface)
  //! \param[in] theSurface surface
  Standard_EXPORT void SetSecondSurface(const Handle(Geom_Surface)& theSurface);

  //! Gets surface attribute (only for SlidingSurface and RollingSurface)
  //! \return surface
  Standard_EXPORT Handle(Geom_Surface) SecondSurface() const;

  //! Sets surface attribute (only for PointOnSurface)
  //! \param[in] theTrimmedSurface trimmed surface
  //Standard_EXPORT void SetTrimmedSurface(const Standard_Real& U1, const Standard_Real& U2, const Standard_Real& V1, const Standard_Real& V2);

  //! Sets trimmed surface attribute (only for PointOnSurface)
  //! \param[in] aTrimSurface trimmered surface
  Standard_EXPORT void SetTrimmedSurface(const Handle(Geom_RectangularTrimmedSurface)& aTrimSurface);

  //! Gets trimmed surface attribute (only for PointOnSurface)
  //! \return trimmed surface
  Standard_EXPORT Handle(Geom_RectangularTrimmedSurface) TrimmedSurface() const;

  DEFINE_STANDARD_RTTIEXT(XCAFKinematics_HighOrderPairObject, XCAFKinematics_PairObject)

private:

  Standard_Boolean myOrientation;                   //!< orientation
  Handle(TColStd_HArray1OfReal) myLimits;           //!< array of limits, size depends on type
  NCollection_Array1<Handle(Geom_Geometry)> myGeom; //!< curve(s) or surface(s) attributes
  Standard_Boolean myIsRanged;                      //!< flag "is limits defined"

};

#endif // _XCAFKinematics_HighOrderPairObject_HeaderFile
