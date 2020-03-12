// Created on: 2020-03-30
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

#include <XCAFDoc_KinematicPairValue.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_KinematicPair.hxx>
#include <XCAFKinematics_PairValueObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_KinematicPairValue, TDF_Attribute)

// Gor first children are reserved for kinematic pair parameters
enum ValueLab
{
  ValueLab_Values = 5
};

//=======================================================================
//function : XCAFDoc_KinematicPairValue
//purpose  : 
//=======================================================================
XCAFDoc_KinematicPairValue::XCAFDoc_KinematicPairValue()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicPairValue::GetID()
{
  static Standard_GUID PairValueID ("4F451E46-0AC9-4270-90E4-D96605CDE635");
  return PairValueID; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(XCAFDoc_KinematicPairValue) XCAFDoc_KinematicPairValue::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_KinematicPairValue) anAttr;
  if (!theLabel.FindAttribute(XCAFDoc_KinematicPairValue::GetID(), anAttr)) {
    anAttr = new XCAFDoc_KinematicPairValue();
    theLabel.AddAttribute(anAttr);
  }
  return anAttr;
}

//=======================================================================
//function : SetObject
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPairValue::SetObject(const Handle(XCAFKinematics_PairValueObject)& theObject)
{
  Backup();

  Label().FindChild(ValueLab_Values).ForgetAllAttributes();

  // Check if kinematic pair and pair value on the given label are consistent
  Handle(XCAFDoc_KinematicPair) aPairAttr;
  if (!Label().FindAttribute(XCAFDoc_KinematicPair::GetID(), aPairAttr))
    return;
  Handle(TDataStd_Integer) aMainTypeAttr;
  if (Label().FindAttribute(TDataStd_Integer::GetID(), aMainTypeAttr))
  {
    int aType = aMainTypeAttr->Get();
    if (aType != theObject->Type())
      return;
  }

  // Check validity of object
  if (theObject->GetAllValues()->Length() == 0)
    return;

  TDataStd_Name::Set(Label().FindChild(ValueLab_Values), "Value");
  TDataStd_Integer::Set(Label().FindChild(ValueLab_Values), theObject->Type());

  Handle(TDataStd_RealArray) aValuesAttr;
  aValuesAttr = TDataStd_RealArray::Set(Label().FindChild(ValueLab_Values), 1, theObject->GetAllValues()->Length());
  aValuesAttr->ChangeArray(theObject->GetAllValues());
}

//=======================================================================
//function : GetObject
//purpose  : 
//=======================================================================
Handle(XCAFKinematics_PairValueObject) XCAFDoc_KinematicPairValue::GetObject()  const
{
  // Type
  Handle(TDataStd_Integer) aTypeAttr;
  Handle(XCAFKinematics_PairValueObject) anObject = new XCAFKinematics_PairValueObject();
  if (Label().FindChild(ValueLab_Values).FindAttribute(TDataStd_Integer::GetID(), aTypeAttr))
  {
    int aType = aTypeAttr->Get();
    anObject->SetType((XCAFKinematics_PairType)aType);
  }

  // Values
  Handle(TDataStd_RealArray) aValuesAttr;
  if (Label().FindChild(ValueLab_Values).FindAttribute(TDataStd_RealArray::GetID(), aValuesAttr)) {
    Handle(TColStd_HArray1OfReal) aValuesArray = aValuesAttr->Array();
    anObject->SetAllValues(aValuesArray);
  }

  return anObject;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicPairValue::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPairValue::Restore(const Handle(TDF_Attribute)& /*With*/)
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XCAFDoc_KinematicPairValue::NewEmpty() const
{
  return new XCAFDoc_KinematicPairValue();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPairValue::Paste(const Handle(TDF_Attribute)& /*Into*/,
                                       const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPairValue::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, TDF_Attribute)
}
