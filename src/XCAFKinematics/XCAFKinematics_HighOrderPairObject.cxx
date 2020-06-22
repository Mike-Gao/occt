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

#include <XCAFKinematics_HighOrderPairObject.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinematics_HighOrderPairObject, XCAFKinematics_PairObject)

//=======================================================================
//function : XCAFKinematics_HighOrderPairObject
//purpose  : 
//=======================================================================
XCAFKinematics_HighOrderPairObject::
XCAFKinematics_HighOrderPairObject()
{
  myLimits = NULL;
}

//=======================================================================
//function : XCAFKinematics_HighOrderPairObject
//purpose  : 
//=======================================================================
XCAFKinematics_HighOrderPairObject::
XCAFKinematics_HighOrderPairObject(const Handle(XCAFKinematics_HighOrderPairObject)& theObj)
{
  SetName(theObj->Name());
  SetType(theObj->Type());
  SetFirstTransformation(theObj->FirstTransformation());
  SetSecondTransformation(theObj->SecondTransformation());
  myOrientation = theObj->myOrientation;
  myLimits = theObj->myLimits;
  myGeom = theObj->myGeom;
}

//=======================================================================
//function : SetType
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetType(const XCAFKinematics_PairType theType)
{
  XCAFKinematics_PairObject::SetType(theType);
  switch (theType) {
  case XCAFKinematics_PairType_PointOnSurface: {
    myLimits = new TColStd_HArray1OfReal(1, 6);
    myGeom = NCollection_Array1<Handle(Geom_Geometry)>(1, 1);
    break;
  }
  case XCAFKinematics_PairType_SlidingSurface: {
    myLimits = new TColStd_HArray1OfReal(1, 2);
    myGeom = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
    break;
  }
  case XCAFKinematics_PairType_RollingSurface: {
    myLimits = new TColStd_HArray1OfReal(1, 2);
    myGeom = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
    break;
  }
  case XCAFKinematics_PairType_PointOnPlanarCurve: {
    myLimits = new TColStd_HArray1OfReal(1, 6);
    myGeom = NCollection_Array1<Handle(Geom_Geometry)>(1, 1);
    break;
  }
  case XCAFKinematics_PairType_SlidingCurve: {
    myLimits = NULL;
    myGeom = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
    break;
  }
  case XCAFKinematics_PairType_RollingCurve: {
    myLimits = NULL;
    myGeom = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
    break;
  }
  }
}

//=======================================================================
//function : SetAllLimits
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits)
{
  Standard_Integer aNbLimits = 0;
  switch (Type()) {
  case XCAFKinematics_PairType_PointOnSurface: {
    aNbLimits = 6;
    break;
  }
  case XCAFKinematics_PairType_SlidingSurface: {
    aNbLimits = 2;
    break;
  }
  case XCAFKinematics_PairType_RollingSurface: {
    aNbLimits = 2;
    break;
  }
  case XCAFKinematics_PairType_PointOnPlanarCurve: {
    aNbLimits = 6;
    break;
  }
  }

  if (theLimits->Length() == aNbLimits)
    myLimits = theLimits;
}

//=======================================================================
//function : SetAllLimits
//purpose  : 
//=======================================================================
Standard_Boolean XCAFKinematics_HighOrderPairObject::HasLimits() const
{
  Standard_Boolean IsConstraint = Standard_False;
  for (Standard_Integer anInd = 1; anInd < myLimits->Length() && !IsConstraint; ++anInd)
    IsConstraint = myLimits->Value(anInd) != NULL;
  return IsConstraint;
}

//=======================================================================
//function : SetRotationLowLimit
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetRotationLowLimit(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    myLimits->ChangeValue(1) = theLimit;
}

//=======================================================================
//function : RotationLowLimit
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::RotationLowLimit()
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    return myLimits->Value(1);
  return 0;
}

//=======================================================================
//function : SetRotationUpperLimit
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetRotationUpperLimit(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    myLimits->ChangeValue(2) = theLimit;
}

//=======================================================================
//function : RotationUpperLimit
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::RotationUpperLimit()
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    return myLimits->Value(2);
  return 0;
}

//=======================================================================
//function : SetLowLimitYaw
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetLowLimitYaw(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    myLimits->ChangeValue(1) = theLimit;
}

//=======================================================================
//function : LowLimitYaw
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::LowLimitYaw()
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    return myLimits->Value(1);
  return 0;
}

//=======================================================================
//function : SetUpperLimitYaw
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetUpperLimitYaw(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    myLimits->ChangeValue(2) = theLimit;
}

//=======================================================================
//function : UpperLimitYaw
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::UpperLimitYaw()
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    return myLimits->Value(2);
  return 0;
}

//=======================================================================
//function : SetLowLimitRoll
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetLowLimitRoll(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    myLimits->ChangeValue(3) = theLimit;
}

//=======================================================================
//function : LowLimitRoll
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::LowLimitRoll()
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    return myLimits->Value(3);
  return 0;
}

//=======================================================================
//function : SetUpperLimitRoll
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetUpperLimitRoll(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    myLimits->ChangeValue(4) = theLimit;
}

//=======================================================================
//function : UpperLimitRoll
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::UpperLimitRoll()
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    return myLimits->Value(4);
  return 0;
}

//=======================================================================
//function : SetLowLimitPitch
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetLowLimitPitch(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    myLimits->ChangeValue(5) = theLimit;
}

//=======================================================================
//function : LowLimitPitch
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::LowLimitPitch()
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    return myLimits->Value(5);
  return 0;
}

//=======================================================================
//function : SetUpperLimitPitch
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetUpperLimitPitch(const Standard_Real theLimit)
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    myLimits->ChangeValue(6) = theLimit;
}

//=======================================================================
//function : UpperLimitPitch
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_HighOrderPairObject::UpperLimitPitch()
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve || Type() == XCAFKinematics_PairType_PointOnSurface)
    return myLimits->Value(6);
  return 0;
}

//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetCurve(const Handle(Geom_Curve)& theCurve)
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve)
    myGeom.ChangeFirst() = theCurve;
}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================
Handle(Geom_Curve) XCAFKinematics_HighOrderPairObject::Curve() const
{
  if (Type() == XCAFKinematics_PairType_PointOnPlanarCurve)
    return Handle(Geom_Curve)::DownCast(myGeom.First());
  return NULL;
}

//=======================================================================
//function : SetFirstCurve
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetFirstCurve(const Handle(Geom_Curve)& theCurve)
{
  if (Type() == XCAFKinematics_PairType_SlidingCurve || Type() == XCAFKinematics_PairType_RollingCurve)
    myGeom.ChangeFirst() = theCurve;
}

//=======================================================================
//function : FirstCurve
//purpose  : 
//=======================================================================
Handle(Geom_Curve) XCAFKinematics_HighOrderPairObject::FirstCurve() const
{
  if (Type() == XCAFKinematics_PairType_SlidingCurve || Type() == XCAFKinematics_PairType_RollingCurve)
    return Handle(Geom_Curve)::DownCast(myGeom.First());
  return NULL;
}

//=======================================================================
//function : SetSecondCurve
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetSecondCurve(const Handle(Geom_Curve)& theCurve)
{
  if (Type() == XCAFKinematics_PairType_SlidingCurve || Type() == XCAFKinematics_PairType_RollingCurve)
    myGeom.ChangeLast() = theCurve;
}

//=======================================================================
//function : SecondCurve
//purpose  : 
//=======================================================================
Handle(Geom_Curve) XCAFKinematics_HighOrderPairObject::SecondCurve() const
{
  if (Type() == XCAFKinematics_PairType_SlidingCurve || Type() == XCAFKinematics_PairType_RollingCurve)
    return Handle(Geom_Curve)::DownCast(myGeom.Last());
  return NULL;
}

//=======================================================================
//function : SetSurface
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetSurface(const Handle(Geom_Surface)& theSurface)
{
  if (Type() == XCAFKinematics_PairType_PointOnSurface)
    myGeom.ChangeFirst() = theSurface;
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) XCAFKinematics_HighOrderPairObject::Surface() const
{
  if (Type() == XCAFKinematics_PairType_PointOnSurface)
    return Handle(Geom_Surface)::DownCast(myGeom.First());
  return NULL;
}

//=======================================================================
//function : SetFirstSurface
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetFirstSurface(const Handle(Geom_Surface)& theSurface)
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    myGeom.ChangeFirst() = theSurface;
}

//=======================================================================
//function : FirstSurface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) XCAFKinematics_HighOrderPairObject::FirstSurface() const
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    return Handle(Geom_Surface)::DownCast(myGeom.First());
  return NULL;
}

//=======================================================================
//function : SetSecondSurface
//purpose  : 
//=======================================================================
void XCAFKinematics_HighOrderPairObject::SetSecondSurface(const Handle(Geom_Surface)& theSurface)
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    myGeom.ChangeLast() = theSurface;
}

//=======================================================================
//function : SecondSurface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) XCAFKinematics_HighOrderPairObject::SecondSurface() const
{
  if (Type() == XCAFKinematics_PairType_SlidingSurface || Type() == XCAFKinematics_PairType_RollingSurface)
    return Handle(Geom_Surface)::DownCast(myGeom.Last());
  return NULL;
}
