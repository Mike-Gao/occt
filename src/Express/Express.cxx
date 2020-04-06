// Created:	Wed Nov  3 14:39:28 1999
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

#include <Express.hxx>
#include <Express_Schema.hxx>
#include <TCollection_HAsciiString.hxx>
#include <OSD_Process.hxx>

#include <time.h>
//#include <pwd.h>

//=======================================================================
//function : Schema
//purpose  : 
//=======================================================================

Handle(Express_Schema) &Express::Schema () 
{
  static Handle(Express_Schema) schema;
  return schema;
}
			       
//=======================================================================
//function : WriteFileStamp
//purpose  : 
//=======================================================================

// Write header of HXX or CXX file
void Express::WriteFileStamp (Standard_OStream &os)
{
  static const char *EC_VERSION = "2.0";

  Standard_CString comm = "//";
  TCollection_AsciiString timestring;
  if ( getenv ( "EXPTOCAS_TIME" ) ) timestring = getenv ( "EXPTOCAS_TIME" );
  else {
    time_t curtime;
    time ( &curtime );
    timestring = ctime ( &curtime );
    timestring.ChangeAll ( '\n', ' ' );
  }
  os << comm << " Created on : " << timestring << std::endl;

  OSD_Process aProcess;
  os << comm << " Created by: " << aProcess.UserName().ToCString() << std::endl;
  os << comm << " Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V" << EC_VERSION << std::endl;
  os << comm << " Copyright (c) Open CASCADE 2020" << std::endl;
  os << comm << std::endl;
  os << comm << " This file is part of Open CASCADE Technology software library." << std::endl;
  os << comm << std::endl;
  os << comm << " This library is free software; you can redistribute it and/or modify it under" << std::endl;
  os << comm << " the terms of the GNU Lesser General Public License version 2.1 as published" << std::endl;
  os << comm << " by the Free Software Foundation, with special exception defined in the file" << std::endl;
  os << comm << " OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT" << std::endl;
  os << comm << " distribution for complete text of the license and disclaimer of any warranty." << std::endl;
  os << comm << std::endl;
  os << comm << " Alternatively, this file may be used under the terms of Open CASCADE" << std::endl;
  os << comm << " commercial license or contractual agreement." << std::endl;
  os << std::endl;
}

//=======================================================================
//function : WriteMethodStamp
//purpose  : 
//=======================================================================

void Express::WriteMethodStamp (Standard_OStream &os, const Handle(TCollection_HAsciiString) &name)
{
  os << std::endl;
  os << "//=======================================================================" << std::endl;
  os << "//function : " << name->ToCString() << std::endl;
  os << "//purpose  : " << std::endl;
  os << "//=======================================================================" << std::endl;
  os << std::endl;
}

//=======================================================================
//function : ToStepName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express::ToStepName (const Handle(TCollection_HAsciiString) &name)
{
  Handle(TCollection_HAsciiString) stepname = new TCollection_HAsciiString ( name );
  for ( Standard_Integer i=2; i <= stepname->Length(); i++ )
    if ( isupper ( stepname->Value(i) ) ) stepname->Insert ( i++, '_' );
  stepname->LowerCase();
  return stepname;
}
//=======================================================================
//function : GetPrefixEnum
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express::EnumPrefix (const Handle(TCollection_HAsciiString) &name)
{
  Handle(TCollection_HAsciiString) stepname = new TCollection_HAsciiString; 
  for ( Standard_Integer i=1; i <= name->Length(); i++ )
    if ( isupper ( name->Value(i) ) ) {
      
      stepname->AssignCat (new TCollection_HAsciiString(name->Value(i)) );
    }
  stepname->LowerCase();
  return stepname;
}

