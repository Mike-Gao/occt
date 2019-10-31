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

#include <TopoDS_AlertAttribute.hxx>

#include <Message_Msg.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopoDS_AlertAttribute, Message_Attribute)

//=======================================================================
//function : TopoDS_AlertAttribute
//purpose  : 
//=======================================================================


TopoDS_AlertAttribute::TopoDS_AlertAttribute (const TopoDS_Shape& theShape,
  const TCollection_AsciiString& theName,
  const TCollection_AsciiString& theDescription)
: Message_AttributeStream (Standard_SStream(), theName, theDescription), myShape (theShape)
{
  Standard_SStream aStream;
  theShape.Dump (aStream);

  SetStream (aStream);
}