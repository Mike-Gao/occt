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

#include <Express_Enum.hxx>
#include <TCollection_HAsciiString.hxx>
#include <OSD_Protection.hxx>
#include <OSD_Path.hxx>
#include <OSD_Directory.hxx>
#include <Express.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Enum,Express_Item)

//=======================================================================
//function : Express_Enum
//purpose  : 
//=======================================================================

Express_Enum::Express_Enum (const Standard_CString name, 
			      const Handle(TColStd_HSequenceOfHAsciiString) &names)
     : Express_Item(name), myNames(names)
{
}

//=======================================================================
//function : Names
//purpose  : 
//=======================================================================

const Handle(TColStd_HSequenceOfHAsciiString) &Express_Enum::Names () const
{
  return myNames;
}

//=======================================================================
//function : GenerateClass
//purpose  : 
//=======================================================================

Standard_Boolean Express_Enum::GenerateClass () const
{
  std::cout << "Generating ENUMERATION " << CPPName()->ToCString() << std::endl;
  
  // create a package directory (if not yet exist)
  OSD_Protection prot ( OSD_RX, OSD_RWX, OSD_RX, OSD_RX );
  TCollection_AsciiString pack = GetPackageName()->String();
  OSD_Path path ( pack );
  OSD_Directory dir ( path );
  dir.Build ( prot );
  pack += "/";
  pack += GetPackageName()->String();
  
  // Open CDL file 
  std::ofstream os ( pack.Cat ( ".cdl" ).ToCString(), std::ios::out | std::ios::ate );

  os << std::endl << "    enumeration " << Name()->ToCString() << " is " << std::endl;
  Handle(TCollection_HAsciiString) prefix = Express::EnumPrefix(Name());
  for ( Standard_Integer i=1; i <= myNames->Length(); i++ ) {
    if ( i >1 ) os << "," << std::endl;
    os << "	" << prefix->ToCString()<<myNames->Value(i)->ToCString();
  }
  os << std::endl << "    end;" << std::endl;
  os.close();
  
  return Standard_False;
}
