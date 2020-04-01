// Created:	Tue Nov  2 15:27:26 1999
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

#include <Express_Integer.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Integer,Express_PredefinedType)

//=======================================================================
//function : Express_Integer
//purpose  : 
//=======================================================================

Express_Integer::Express_Integer () 
{
}

//=======================================================================
//function : CPPName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_Integer::CPPName () const
{
  return new TCollection_HAsciiString ( "Standard_Integer" );
}

//=======================================================================
//function : CDLName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_Integer::CDLName () const
{
  return new TCollection_HAsciiString ( "Integer" );
}
