// Created on: 2018-06-10
// Created by: Natalia Ermolaeva
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Message_AttributeVectorOfRealVec3.hxx>

#include <Message_Msg.hxx>
#include <Message_PerfMeter.hxx>
#include <Message_Report.hxx>

#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeVectorOfRealVec3, Message_Attribute)

//=======================================================================
//function : Message_AttributeVectorOfRealVec3
//purpose  : 
//=======================================================================

Message_AttributeVectorOfRealVec3::Message_AttributeVectorOfRealVec3 (const NCollection_Vector<NCollection_Vec3<Standard_Real>>& theValues,
                                                                      const Standard_Integer& theColumnCount,
                                                                      const TCollection_AsciiString& theName,
                                                                      const TCollection_AsciiString& theDescription)
: Message_Attribute (theName, theDescription), myColumnCount (theColumnCount)
{
  SetValues (theValues);
}

//=======================================================================
//function : SetValues
//purpose  : 
//=======================================================================

void Message_AttributeVectorOfRealVec3::SetValues (const NCollection_Vector<NCollection_Vec3<Standard_Real>>& theValues)
{
  myValues = theValues;

  int aLength = (Standard_Integer)myValues.Length();
  if (aLength < 2)
    return;

  if (myColumnCount <= 0)
    myCachedValue = TCollection_AsciiString ("[") + aLength + "] : ";
  else
    myCachedValue = TCollection_AsciiString ("[") + (aLength / myColumnCount) +  ", " + myColumnCount + "] : ";

  NCollection_Vec3<Standard_Real> aValue = myValues.First();
  myCachedValue = myCachedValue + "(" + aValue.x() + "," + aValue.y() + "," + aValue.z() + ")";
  myCachedValue += " ... ";
  aValue = myValues.Last();
  myCachedValue = myCachedValue + "(" + aValue.x() + "," + aValue.y() + "," + aValue.z() + ")";
}

//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

const TCollection_AsciiString& Message_AttributeVectorOfRealVec3::GetDescription() const
{
  if (!Message_Attribute::GetDescription().IsEmpty())
    return Message_Attribute::GetDescription();

  return myCachedValue;
}

//=======================================================================
//function : IsSame
//purpose  :
//=======================================================================

Standard_Boolean Message_AttributeVectorOfRealVec3::IsSame (const Handle(Message_Attribute)& theAttribute)
{
  if (!Message_Attribute::IsSame (theAttribute))
    return Standard_False;

  Handle(Message_AttributeVectorOfRealVec3) anAttribute = Handle(Message_AttributeVectorOfRealVec3)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return Standard_False;

  NCollection_Vector<NCollection_Vec3<Standard_Real> >::Iterator anIterator (GetValues());
  NCollection_Vector<NCollection_Vec3<Standard_Real> >::Iterator anOtherIterator (anAttribute->GetValues());
  for (; anIterator.More() && anOtherIterator.More(); anIterator.Next(), anOtherIterator.Next())
  {
    if (fabs (anIterator.Value().x() - anOtherIterator.Value().x()) > Precision::Confusion() ||
        fabs (anIterator.Value().y() - anOtherIterator.Value().y()) > Precision::Confusion() ||
        fabs (anIterator.Value().z() - anOtherIterator.Value().z()) > Precision::Confusion())
      return Standard_False;
  }
  return Standard_True;
}
