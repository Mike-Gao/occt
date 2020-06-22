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

#include <XCAFKinematics_LowOrderPairObjectWithCoupling.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinematics_LowOrderPairObjectWithCoupling, XCAFKinematics_PairObject)

//=======================================================================
//function : XCAFKinematics_LowOrderPairObjectWithCoupling
//purpose  : 
//=======================================================================
XCAFKinematics_LowOrderPairObjectWithCoupling::
  XCAFKinematics_LowOrderPairObjectWithCoupling()
{
  myLowLimit = -Precision::Infinite();
  myUpperLimit = Precision::Infinite();
  myParams = NULL;
}

//=======================================================================
//function : XCAFKinematics_LowOrderPairObjectWithCoupling
//purpose  : 
//=======================================================================
XCAFKinematics_LowOrderPairObjectWithCoupling::
  XCAFKinematics_LowOrderPairObjectWithCoupling(const Handle(XCAFKinematics_LowOrderPairObjectWithCoupling)& theObj)
{
  SetName(theObj->Name());
  SetType(theObj->Type());
  SetFirstTransformation(theObj->FirstTransformation());
  SetSecondTransformation(theObj->SecondTransformation());
  myLowLimit = theObj->LowLimit();
  myUpperLimit = theObj->UpperLimit();
  myParams = theObj->GetAllParams();
}

//=======================================================================
//function : SetType
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetType(const XCAFKinematics_PairType theType)
{
  XCAFKinematics_PairObject::SetType(theType);
  if (theType == XCAFKinematics_PairType_Gear)
    myParams = new TColStd_HArray1OfReal(1, 5);
  else
    myParams = new TColStd_HArray1OfReal(1, 1);
}

//=======================================================================
//function : GetAllLimits
//purpose  : 
//=======================================================================
Handle(TColStd_HArray1OfReal) XCAFKinematics_LowOrderPairObjectWithCoupling::GetAllLimits() const
{
  Handle(TColStd_HArray1OfReal) aLimitArray = new TColStd_HArray1OfReal(1, 2);
  aLimitArray->ChangeValue(1) = myLowLimit;
  aLimitArray->ChangeValue(2) = myUpperLimit;
  return aLimitArray;
}

//=======================================================================
//function : SetPitch
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetPitch(const Standard_Real thePitch)
{
  if (Type() == XCAFKinematics_PairType_Screw)
    myParams->ChangeFirst() = thePitch;
}

//=======================================================================
//function : Pitch
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_LowOrderPairObjectWithCoupling::Pitch() const
{
  if (Type() == XCAFKinematics_PairType_Screw)
    return myParams->First();
  return 0;
}

//=======================================================================
//function : SetPinionRadius
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetPinionRadius(const Standard_Real theRadius)
{
  if (Type() == XCAFKinematics_PairType_RackAndPinion)
    myParams->ChangeFirst() = theRadius;
}

//=======================================================================
//function : PinionRadius
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_LowOrderPairObjectWithCoupling::PinionRadius() const
{
  if (Type() == XCAFKinematics_PairType_RackAndPinion)
    return myParams->First();
  return 0;
}

//=======================================================================
//function : SetRadiusFirstLink
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetRadiusFirstLink(const Standard_Real theRadius)
{
  if (Type() == XCAFKinematics_PairType_Gear)
    myParams->ChangeFirst() = theRadius;
}

//=======================================================================
//function : RadiusFirstLink
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_LowOrderPairObjectWithCoupling::RadiusFirstLink() const
{
  if (Type() == XCAFKinematics_PairType_Gear)
    return myParams->First();
  return 0;
}

//=======================================================================
//function : SetRadiusSecondLink
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetRadiusSecondLink(const Standard_Real theRadius)
{
  if (Type() == XCAFKinematics_PairType_Gear)
    myParams->ChangeValue(2) = theRadius;
}

//=======================================================================
//function : RadiusSecondLink
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_LowOrderPairObjectWithCoupling::RadiusSecondLink() const
{
  if (Type() == XCAFKinematics_PairType_Gear)
    return myParams->Value(2);
  return 0;
}

//=======================================================================
//function : SetBevel
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetBevel(const Standard_Real theBevel)
{
  if (Type() == XCAFKinematics_PairType_Gear)
    myParams->ChangeValue(3) = theBevel;
}

//=======================================================================
//function : Bevel
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_LowOrderPairObjectWithCoupling::Bevel() const
{
  if (Type() == XCAFKinematics_PairType_Gear)
    return myParams->Value(3);
  return 0;
}

//=======================================================================
//function : SetHelicalAngle
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetHelicalAngle(const Standard_Real theAngle)
{
  if (Type() == XCAFKinematics_PairType_Gear)
    myParams->ChangeValue(4) = theAngle;
}

//=======================================================================
//function : HelicalAngle
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_LowOrderPairObjectWithCoupling::HelicalAngle() const
{
  if (Type() == XCAFKinematics_PairType_Gear)
    return myParams->Value(4);
  return 0;
}

//=======================================================================
//function : SetGearRatio
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetGearRatio(const Standard_Real theGearRatio)
{
  if (Type() == XCAFKinematics_PairType_Gear)
    myParams->ChangeValue(5) = theGearRatio;
}

//=======================================================================
//function : GearRatio
//purpose  : 
//=======================================================================
Standard_Real XCAFKinematics_LowOrderPairObjectWithCoupling::GearRatio() const
{
  if (Type() == XCAFKinematics_PairType_Gear)
    return myParams->Value(5);
  return 0;
}

//=======================================================================
//function : SetAllParams
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetAllParams(const Handle(TColStd_HArray1OfReal)& theParams)
{
  if (theParams->Length() == myParams->Length())
    myParams = theParams;
}

//=======================================================================
//function : HasLimits
//purpose  : 
//=======================================================================
Standard_Boolean XCAFKinematics_LowOrderPairObjectWithCoupling::HasLimits() const
{
  return
    (  myLowLimit != -Precision::Infinite() ||
      myUpperLimit != Precision::Infinite()
    );
}

//=======================================================================
//function : SetAllLimits
//purpose  : 
//=======================================================================
void XCAFKinematics_LowOrderPairObjectWithCoupling::SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits)
{
  if (theLimits->Length() == 2)
  {
    myLowLimit = theLimits->Value(1);
    myUpperLimit = theLimits->Value(2);
  }
}
