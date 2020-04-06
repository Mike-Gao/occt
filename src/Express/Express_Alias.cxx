// Created:	Tue Nov  2 14:40:06 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#include <Express_Alias.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Alias,Express_Item)

//=======================================================================
//function : Express_Alias
//purpose  : 
//=======================================================================

Express_Alias::Express_Alias (const Standard_CString name, 
				const Handle(Express_Type) &type)
     : Express_Item(name), myType(type)
{
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

const Handle(Express_Type) &Express_Alias::Type () const
{
  return myType;
}

//=======================================================================
//function : CPPName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_Alias::CPPName () const
{
  return myType->CPPName();
}

//=======================================================================
//function : GenerateClass
//purpose  : 
//=======================================================================

Standard_Boolean Express_Alias::GenerateClass () const
{
//  std::cout << "Warning: writing ALIAS is not yet implemented, cannot generate " << Name()->ToCString() << std::endl;
  std::cout << "ALIAS " << Name()->ToCString() << " = " << Type()->CPPName()->ToCString() << " used; no generation is needed" << std::endl;
  return Standard_False;
}
