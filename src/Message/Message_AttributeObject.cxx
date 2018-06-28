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

#include <Message_AttributeObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeObject, Message_Attribute)

//=======================================================================
//function : Message_AttributeObject
//purpose  : 
//=======================================================================

Message_AttributeObject::Message_AttributeObject (const Handle(Standard_Transient)& theObject,
                                                  const TCollection_AsciiString& theName,
                                                  const TCollection_AsciiString& theDescription)
: Message_Attribute (theName, theDescription)
{
  myObject = theObject;
}

//=======================================================================
//function : IsSame
//purpose  :
//=======================================================================

Standard_Boolean Message_AttributeObject::IsSame (const Handle(Message_Attribute)& theAttribute)
{
  if (!Message_Attribute::IsSame (theAttribute))
    return Standard_False;

  Handle(Message_AttributeObject) anAttribute = Handle(Message_AttributeObject)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return Standard_False;

  if (myObject.IsNull() != anAttribute->GetObject().IsNull())
    return Standard_False;

  return Standard_True;
}
