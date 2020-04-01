// Created:	Tue Nov  2 15:13:31 1999
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

#include <Express_ComplexType.hxx>
#include <Express_String.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_ComplexType,Express_Type)

//=======================================================================
//function : Express_ComplexType
//purpose  : 
//=======================================================================

Express_ComplexType::Express_ComplexType (const Standard_Integer imin,
					    const Standard_Integer imax,
					    const Handle(Express_Type) &type) 
{
  myMin = imin;
  myMax = imax;
  myType = type;
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

const Handle(Express_Type) &Express_ComplexType::Type () const
{
  return myType;
}

//=======================================================================
//function : CPPName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_ComplexType::CPPName () const
{
   // check if array 2
  Handle(Express_Type) type = myType;
  if ( type->IsKind ( STANDARD_TYPE(Express_ComplexType) ) ) {
    Handle(Express_ComplexType) c2 = Handle(Express_ComplexType)::DownCast ( type );
    type = c2->Type();
  }

  // parse name of array argument
  Handle(TCollection_HAsciiString) name = type->CPPName();
  Standard_Integer split = name->Location ( 1, '_', 1, name->Length() );
  Handle(TCollection_HAsciiString) cls;
  if ( split ) cls = name->Split ( split );
  else cls = name;
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString("TCollection");
  Standard_Integer ind =name->Location(str,1,name->Length());
  if(ind) 
    name = new TCollection_HAsciiString ( "Interface_" );
  // generate name
  if ( type->IsStandard() || ! split ) {
      name = new TCollection_HAsciiString ( "TColStd_" );
  }
  if ( type == myType ) name->AssignCat ( "HArray1Of" );
  else                  name->AssignCat ( "HArray2Of" );
  name->AssignCat ( cls );
  return name;
}

//=======================================================================
//function : CDLName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_ComplexType::CDLName () const
{
  // check if array 2
  Handle(TCollection_HAsciiString) name;
  Handle(Express_Type) type = myType;
  if ( type->IsKind ( STANDARD_TYPE(Express_ComplexType) ) ) {
    Handle(Express_ComplexType) c2 = Handle(Express_ComplexType)::DownCast ( type );
    type = c2->Type();
    name = new TCollection_HAsciiString ( "HArray2Of" );
  }
  else 
    name = new TCollection_HAsciiString ( "HArray1Of" );
  
  // generate name
  name->AssignCat ( type->CDLName() );
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString("TCollection");
  Standard_Integer ind =name->Location(str,1,name->Length());
  if(ind) {
    Handle(TCollection_HAsciiString) tmp = name->Split(ind-1);
    name->AssignCat("Interface");
  }
  if ( type->IsStandard() ) 
    name->AssignCat ( " from TColStd" );
    
  
 
  return name;
}


//=======================================================================
//function : Use
//purpose  : 
//=======================================================================

Standard_Boolean Express_ComplexType::Use (const Handle(TCollection_HAsciiString) &pack,
					    const Standard_Boolean defer) const
{
  
  return myType->Use ( pack, defer );
}
