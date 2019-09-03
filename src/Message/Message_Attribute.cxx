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

#include <Message_Attribute.hxx>

#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Attribute, Standard_Transient)

//=======================================================================
//function : GetMessageKey
//purpose  :
//=======================================================================

Standard_CString Message_Attribute::GetMessageKey () const
{
  return !myName.IsEmpty() ? myName.ToCString() : "";
}