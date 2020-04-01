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

#include <Express_Entity.hxx>
#include <Express_Type.hxx>
#include <Express_NamedType.hxx>
#include <Express_Alias.hxx>
#include <Express_ComplexType.hxx>
#include <Express_String.hxx>
#include <Express_Boolean.hxx>
#include <Express_Logical.hxx>
#include <Express_Number.hxx>
#include <Express_Integer.hxx>
#include <Express_Real.hxx>
#include <Express_Select.hxx>
#include <Express_Field.hxx>
#include <Express.hxx>

#include <TCollection_HAsciiString.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <Express_Enum.hxx>
#include <Express_HSequenceOfEntity.hxx>
#include <Express_HSequenceOfField.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Entity,Express_Item)

//=======================================================================
//function : Express_Entity
//purpose  : 
//=======================================================================

Express_Entity::Express_Entity (const Standard_CString name, 
				  const Handle(TColStd_HSequenceOfHAsciiString) &inherit,
				  const Handle(Express_HSequenceOfField) &flds)
     : Express_Item(name), mySupers(inherit), myFields(flds)
       
{
  // make empty lists to avoid checking every time
  myInherit = new Express_HSequenceOfEntity;
  if ( mySupers.IsNull() ) mySupers = new TColStd_HSequenceOfHAsciiString;
  if ( myFields.IsNull() ) myFields = new Express_HSequenceOfField;
  myisAbstract = Standard_False;
}

//=======================================================================
//function : SuperTypes
//purpose  : 
//=======================================================================

const Handle(TColStd_HSequenceOfHAsciiString) &Express_Entity::SuperTypes () const
{
  return mySupers;
}

//=======================================================================
//function : Inherit
//purpose  : 
//=======================================================================

const Handle(Express_HSequenceOfEntity) &Express_Entity::Inherit () const
{
  return myInherit;
}

//=======================================================================
//function : Fields
//purpose  : 
//=======================================================================

const Handle(Express_HSequenceOfField) &Express_Entity::Fields () const
{
  return myFields;
}
//=======================================================================
//function : NbFields
//purpose  : 
//=======================================================================

Standard_Integer Express_Entity::NbFields (const Standard_Boolean inherited) const
{
  Standard_Integer num = myFields->Length();
  if ( inherited ) {
    for ( Standard_Integer i=1; i <= myInherit->Length(); i++ )
      num += myInherit->Value(i)->NbFields ( inherited );
  }
  return num;
}

//=======================================================================
//function : WriteGetMethod
//purpose  : 
//=======================================================================

static void WriteGetMethod (Standard_OStream &os, 
			    const Handle(TCollection_HAsciiString) name,
			    const Handle(TCollection_HAsciiString) field,
			    const Handle(TCollection_HAsciiString) type,
			    const Standard_Boolean isHandle,
			    const Standard_Boolean /*isSimple*/)
{
  Handle(TCollection_HAsciiString) method = field;//new TCollection_HAsciiString ( "Get" );
//  method->AssignCat ( field );
  Express::WriteMethodStamp ( os, method );
  os << ( isHandle ? "Handle(" : "" ) << type->ToCString() << ( isHandle ? ") " : " " ) <<
	name->ToCString() << "::" << method->ToCString() << " () const" << std::endl;
  os << "{" << std::endl << "  return the" << field->ToCString() << ";\n}" << std::endl;
}

//=======================================================================
//function : WriteSetMethod
//purpose  : 
//=======================================================================

static void WriteSetMethod (Standard_OStream &os, 
			    const Handle(TCollection_HAsciiString) name,
			    const Handle(TCollection_HAsciiString) field,
			    const Handle(TCollection_HAsciiString) type,
			    const Standard_Boolean isHandle,
			    const Standard_Boolean isSimple)
{
  Handle(TCollection_HAsciiString) method = new TCollection_HAsciiString ( "Set" );
  method->AssignCat ( field );
  Express::WriteMethodStamp ( os, method );
  os << "void " << name->ToCString() << "::" << method->ToCString() << " (const " <<
        ( isHandle ? "Handle(" : "" ) << type->ToCString() << ( isHandle ? ")" : "" ) <<
	" " << ( isSimple ? "" : "&" ) << "a" << field->ToCString() << ")" << std::endl;
  os << "{" << std::endl << "  the" << field->ToCString() << " = a" << field->ToCString() << ";\n}" << std::endl;
}

//=======================================================================
//function : WriteSpaces
//purpose  : 
//=======================================================================

static inline void WriteSpaces (Standard_OStream &os, Standard_Integer num)
{
  for ( Standard_Integer i=0; i < num; i++ ) os << " ";
}

//=======================================================================
//function : GenerateClass
//purpose  : 
//=======================================================================

Standard_Boolean Express_Entity::GenerateClass () const
{
  Handle(TCollection_HAsciiString) CPPname = CPPName();
  Handle(TCollection_HAsciiString) CDLname = CDLName();
  std::cout << "Generating ENTITY " << CPPname->ToCString() << std::endl;
  
  // create a package directory (if not yet exist)
  OSD_Protection prot ( OSD_RX, OSD_RWX, OSD_RX, OSD_RX );
  TCollection_AsciiString pack = GetPackageName()->String();
  OSD_Path path ( pack );
  OSD_Directory dir ( path );
  dir.Build ( prot );
  pack += "/";
  pack += CPPname->String();
  
  //===============================
  // Step 1: generating CDL
  
  // Open CDL file 
  std::ofstream os ( pack.Cat ( ".cdl" ).ToCString() );

  // write header
  Express::WriteFileStamp ( os, CPPname, Standard_True );

  // write start of declaration (inheritance)
  os << "class " << CDLname->ToCString() << std::endl << "inherits ";
  if ( myInherit->Length() >0 ) { // first inherited class will be actually inherited
    Handle(Express_Entity) it = myInherit->Value(1);
    it->Use ( GetPackageName() );
    os << it->CDLName()->ToCString();
    if ( myInherit->Length() >1 ) {
      std::cout << "Warning: ENTITY " << Name()->ToCString() << " defined with multiple inheritance;" << std::endl;
      std::cout << "Warning: only first base class is actually inherited, others are made fields" << std::endl;
    }
  }
  else os << "TShared from MMgt";
  os << std::endl << std::endl;
  
  os << "    ---Purpose: Representation of STEP entity " << Name()->ToCString() << std::endl;
  os << std::endl;
  
  // write "uses" section (also creates all used types)
  DataMapOfStringInteger dict;
  if ( ! WriteUses ( os, Standard_True, dict ) ) os << std::endl << std::endl;
    
  // start 'is' section
  os << "is" << std::endl;
  os << "    Create returns " << CDLname->ToCString() << ";" << std::endl;
  os << "	---Purpose: Empty constructor" << std::endl;
  os << std::endl;
  
  // write Init methods
  if ( myInherit->Length() >1 || myFields->Length() >0 ) {
    os << "    Init (me: mutable; ";
    MakeInit ( os, 23, Standard_True, 0 );
    os << ");" << std::endl;
    os << "	---Purpose: Initialize all fields (own and inherited)" << std::endl;
    os << std::endl;
  }
  
  // write methods Get/Set
  for ( Standard_Integer i=2; i <= myInherit->Length(); i++ ) {
    Handle(Express_Entity) it = myInherit->Value(i);
    Handle(TCollection_HAsciiString) name = it->Name();
    Handle(TCollection_HAsciiString) str = it->CDLName();
    os << "    " << name->ToCString() << " (me) returns " << str->ToCString() << ";" << std::endl;
    os << "	---Purpose: Returns data for supertype " << name->ToCString() << std::endl;
    os << "    Set" << name->ToCString() << " (me: mutable; " << name->ToCString() << ": " << str->ToCString() << ");" << std::endl;
    os << "	---Purpose: Set data for supertype " << name->ToCString() << std::endl;
    os << std::endl;
  }
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Field) field = myFields->Value ( i );
    Handle(TCollection_HAsciiString) str = field->Type()->CDLName();
    os << "    " << field->Name()->ToCString() << " (me) returns " << str->ToCString() << ";" << std::endl;
    os << "	---Purpose: Returns field " << field->Name()->ToCString() << std::endl;
    os << "    Set" << field->Name()->ToCString() << " (me: mutable; " << 
          field->Name()->ToCString() << ": " << str->ToCString() << ");" << std::endl;
    os << "	---Purpose: Set field " << field->Name()->ToCString() << std::endl;
    if ( field->IsOptional() ) {
      os << "    Has" << field->Name()->ToCString() << " (me) returns Boolean;" << std::endl;
      os << "	---Purpose: Returns True if optional field " << field->Name()->ToCString() << " is defined" << std::endl;
//      os << "	---C++: return &" << std::endl;
    }
    os << std::endl;
  }
  
  // write fields section
  if ( myInherit->Length() >1 || myFields->Length() >0 ) {
    os << "fields" << std::endl;
    for (Standard_Integer i=2; i <= myInherit->Length(); i++ ) {
      Handle(Express_Entity) it = myInherit->Value(i);
      Handle(TCollection_HAsciiString) name = it->Name();
      Handle(TCollection_HAsciiString) str = it->CDLName();
      os << "    the" << it->Name()->ToCString() << ": " <<
	    it->CDLName()->ToCString() << "; -- supertype" << std::endl;
    }
    for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
      Handle(Express_Field) field = myFields->Value ( i );
      os << "    the" << field->Name()->ToCString() << ": " << 
	    field->Type()->CDLName()->ToCString() << ";";
      if ( field->IsOptional() ) os << " -- optional";
      os << std::endl;
    }
    // optional fields: flag 'is field set'
    for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
      Handle(Express_Field) field = myFields->Value ( i );
      if ( ! field->IsOptional() ) continue;
      os << "    def" << field->Name()->ToCString() << ": Boolean; -- flag \"is " << 
	    field->Name()->ToCString() << " defined\"" << std::endl;
    }
    os << std::endl;
  }
      
  // write end
  os << "end " << Name()->ToCString() << ";" << std::endl;
  os.close();
  
  //===============================
  // Step 2: generating CXX
  
  // Open CXX file 
  os.open ( pack.Cat ( ".cxx" ).ToCString() );

  // write header
  Express::WriteFileStamp ( os, CPPname, Standard_False );

  // write include section
  os << "#include <" << CPPname->ToCString() << ".ixx>" << std::endl;
  
  // write constructor
  Express::WriteMethodStamp ( os, CPPname );
  os << CPPname->ToCString() << "::" << CPPname->ToCString() << " ()" << std::endl;
  os << "{" << std::endl;
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Field) field = myFields->Value ( i );
    if ( field->IsOptional() ) os << "  def" << field->Name()->ToCString() << " = Standard_False;" << std::endl;
  }
  os << "}" << std::endl;

  // write method Init()
  if ( myInherit->Length() >1 || myFields->Length() >0 ) {
    Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "Init" ) );
    os << "void " << CPPname->ToCString() << "::Init (";
    MakeInit ( os, 13 + CPPname->Length(), Standard_True, 1 );
    os << ")\n{";
    MakeInit ( os, -2, Standard_True, 3 );
    os << "\n}" << std::endl;
  }
  
  // write "methods" section
  for (Standard_Integer i=2; i <= myInherit->Length(); i++ ) {
    Handle(Express_Entity) it = myInherit->Value(i);
    WriteGetMethod ( os, CPPname, it->Name(), it->CPPName(), Standard_True, Standard_False );
    WriteSetMethod ( os, CPPname, it->Name(), it->CPPName(), Standard_True, Standard_False );
  }
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Field) field = myFields->Value ( i );
    Handle(Express_Type) type = field->Type();
    WriteGetMethod ( os, CPPname, field->Name(), type->CPPName(), type->IsHandle(), type->IsSimple() );
    WriteSetMethod ( os, CPPname, field->Name(), type->CPPName(), type->IsHandle(), type->IsSimple() );
    if ( ! field->IsOptional() ) continue;
    Handle(TCollection_HAsciiString) method = new TCollection_HAsciiString ( "Has" );
    method->AssignCat ( field->Name() );
    Express::WriteMethodStamp ( os, method );
    os << "Standard_Boolean " << CPPname->ToCString() << "::" << method->ToCString() << " () const" << std::endl;
    os << "{" << std::endl << "  return def" << field->Name()->ToCString() << ";\n}" << std::endl;
  }
  
  // close
  os.close();
  if(AbstractFlag()) return Standard_True;  
  //===============================
  // Step 3: generating CDL for Reader/Writer class
  
  // Open CDL file 
  Handle(TCollection_HAsciiString) RWCPPName = new TCollection_HAsciiString ( "RW" );
  RWCPPName->AssignCat ( GetPackageName() );
  RWCPPName->AssignCat ( "_RW" );
  RWCPPName->AssignCat ( Name() );
  Handle(TCollection_HAsciiString) RWCDLName = new TCollection_HAsciiString ( "RW" );
  RWCDLName->AssignCat ( Name() );
  RWCDLName->AssignCat ( " from RW" );
  RWCDLName->AssignCat ( GetPackageName() );

  pack = "RW";
  pack += GetPackageName()->String();
  OSD_Path rwpath ( pack );
  OSD_Directory rwdir ( rwpath );
  rwdir.Build ( prot );
  pack += "/";
  pack += RWCPPName->String();
  
  os.open ( pack.Cat ( ".cdl" ).ToCString() );

  // write header
  Express::WriteFileStamp ( os, RWCPPName, Standard_True );

  // write start of declaration (inheritance)
  os << "class " << RWCDLName->ToCString() << "  inherits RWClass from StepData" << std::endl << std::endl;
  
  os << "    ---Purpose: Read & Write tool for " << Name()->ToCString() << std::endl;
  os << std::endl;
  
  // write "uses" section
  os << "uses" << std::endl;
  os << "    Check from Interface," << std::endl;
  os << "    StepWriter from StepData," << std::endl;
  os << "    StepReaderData from StepData," << std::endl;
  os << "    EntityIterator from Interface," << std::endl;
  os << "    InterfaceModel from Interface," << std::endl;
  os << "    ShareTool from Interface," << std::endl;
  os << "    " << CDLname->ToCString() << std::endl;
  os << std::endl;

  // write "is" section
  os << "is" << std::endl;
  os << "    New(me) returns Transient;"<< std::endl; // << RWCDLName->ToCString() << ";" << std::endl;
  os << "	---Purpose: Returns new entity " << RWCDLName->ToCString()<< std::endl;
  os << std::endl;
  os << "    ReadStep (me; data: StepReaderData from StepData; num: Integer;" << std::endl;
  os << "                  ach : in out Check from Interface;" << std::endl;
  os << "                  entt : Transient);" << std::endl;
  os << "	---Purpose: Reads " << Name()->ToCString() << std::endl;
  os << std::endl;
  os << "    WriteStep (me; SW: in out StepWriter from StepData;" << std::endl;
  os << "                   entt: Transient);" << std::endl;
  os << "	---Purpose: Writes " << Name()->ToCString() << std::endl;
  os << std::endl;
  os << "    Share (me; entt : Transient;" << std::endl;
  os << "               iter: in out EntityIterator from Interface);" << std::endl;
  os << "	---Purpose: Fills data for graph (shared items)" << std::endl;
  if(CheckFlag()) {
     os << "    Check (me; entt : Transient;" << std::endl;
     os << "               shares : ShareTool from Interface;" << std::endl;
     os << "               ach : in out Check from Interface) is redefined;" << std::endl;
     os << "	---Purpose: Check data for entity " << std::endl;
    }
  if(FillSharedFlag()) {
     os << "    Share (me;   model : InterfaceModel from Interface;" << std::endl;
     os << "               entt : Transient;" << std::endl;
     os << "               iter: in out EntityIterator from Interface) is redefined;" << std::endl;
     os << "	---Purpose: Shared data for entity " << std::endl;
  }
  os << std::endl;

  // write end
  os << "end RW" << Name()->ToCString() << ";" << std::endl;
  os.close();
  
  //===============================
  // Step 4: generating CXX for Read/Write tool
  
  // Open CXX file 
  os.open ( pack.Cat ( ".cxx" ).ToCString() );

  // write header
  Express::WriteFileStamp ( os, RWCPPName, Standard_False );

  // write include section
  os << "#include <" << RWCPPName->ToCString() << ".ixx>" << std::endl;
  WriteRWInclude ( os, dict );
  os << "#include <" << CPPname->ToCString() << ".hxx>" << std::endl;
  // write method New
  Express::WriteMethodStamp ( os, RWCPPName );
  os << "Handle(Standard_Transient) " << RWCPPName->ToCString() << "::" << "New () const" << std::endl;
  os << "{" << std::endl;
  os << "  Handle("<<RWCPPName->ToCString()<<") newRW = new " <<RWCPPName->ToCString()<<";"<< std::endl;
  os << "  return newRW;"<< std::endl;
  os << "}" << std::endl;

  // write method ReadStep
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "ReadStep" ) );
  os << "void " << RWCPPName->ToCString() << "::ReadStep (const Handle(StepData_StepReaderData)& data," << std::endl;
  WriteSpaces ( os, 17 + RWCPPName->Length() );
  os << "const Standard_Integer num," << std::endl;
  WriteSpaces ( os, 17 + RWCPPName->Length() );
  os << "Interface_Check& ach," << std::endl;
  WriteSpaces ( os, 17 + RWCPPName->Length() );
  os << "const Handle(Standard_Transient) &entt) const" << std::endl;
  os << "{" << std::endl;
  Standard_Integer nbFld = NbFields ( Standard_True );
  //DownCast entity to it's type
  os<<"  Handle("<<CPPname->ToCString() << ") ent = Handle("<<CPPname->ToCString() <<")::DownCast(entt);"<< std::endl;
  
  os << "  // Check number of parameters" << std::endl;
  os << "  if ( ! data->CheckNbParams(num," << nbFld << ",ach,\"" << 
        Express::ToStepName ( Name() )->ToCString() << "\") ) return;" << std::endl;
  WriteRWReadCode ( os, 1, Standard_True ); // write code for reading inherited and own fields
  os << std::endl;
  os << "  // Initialize entity" << std::endl;
  os << "  ent->Init(";
  MakeInit ( os, 12, Standard_True, 2 );
  os << ");\n}" << std::endl;

  // write method WriteStep
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "WriteStep" ) );
  os << "void " << RWCPPName->ToCString() << "::WriteStep (StepData_StepWriter& SW," << std::endl;
  WriteSpaces ( os, 18 + RWCPPName->Length() );
  os << "const Handle(Standard_Transient) &entt) const" << std::endl;
  os << "{" << std::endl;
  //DownCast entity to it's type
  os<<"  Handle("<<CPPname->ToCString() << ") ent = Handle("<<CPPname->ToCString() <<")::DownCast(entt);"<< std::endl;
  
  WriteRWWriteCode ( os, 0, Standard_True ); // write code for writing inherited and own fields
  os << "}" << std::endl;

  // write method Share
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "Share" ) );
  os << "void " << RWCPPName->ToCString() << "::Share (const Handle(Standard_Transient) &entt," << std::endl;
  WriteSpaces ( os, 14 + RWCPPName->Length() );
  os << "Interface_EntityIterator& iter) const" << std::endl;
  os << "{" << std::endl;
  os<<"  Handle("<<CPPname->ToCString() << ") ent = Handle("<<CPPname->ToCString() <<")::DownCast(entt);"<< std::endl;
  WriteRWShareCode ( os, 1, Standard_True ); // write code for filling graph of references
  os << "}" << std::endl;
  if(CheckFlag()) {
    Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "Check" ) );
    os << "void " << RWCPPName->ToCString() << "::Check(const Handle(Standard_Transient) &entt," << std::endl;
    WriteSpaces ( os, 18 + RWCPPName->Length() );
    os << "const Interface_ShareTool& shares,"<< std::endl;
    WriteSpaces ( os, 18 + RWCPPName->Length() );
    os << "Interface_Check& check) const"<< std::endl;
    os << "{" << std::endl;
    //DownCast entity to it's type
    os<<"  Handle("<<CPPname->ToCString() << ") ent = Handle("<<CPPname->ToCString() <<")::DownCast(entt);"<< std::endl;
    os << "}" << std::endl;
  }
  if(FillSharedFlag()) {
    Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "FillShared" ) );
    os << "void " << RWCPPName->ToCString() << "::Share(const Handle(Interface_InterfaceModel)& model," << std::endl;
    WriteSpaces ( os, 18 + RWCPPName->Length() );
    os << "const Handle(Standard_Transient) &entt,"<< std::endl;
    WriteSpaces ( os, 18 + RWCPPName->Length() );
    os << "Interface_EntityIterator& iter) const"<< std::endl;
    os << "{" << std::endl;
    //DownCast entity to it's type
    os<<"  Handle("<<CPPname->ToCString() << ") ent = Handle("<<CPPname->ToCString() <<")::DownCast(entt)"<< std::endl;
    os << "}" << std::endl;
  }
  
  // close
  os.close();
  //adding method for registartion of entities and include in the two files
  
  TCollection_AsciiString packname = GetPackageName()->String();
  OSD_Path path_reg ( packname );
  OSD_Directory dir_reg ( packname );
  dir_reg.Build ( prot );
  
  TCollection_AsciiString packname_inc = packname +".cxx_inc"; 
  TCollection_AsciiString packname_rec = packname + ".cxx_reg";
  packname += "/";
  //write file with includes for registration
  os.open(packname.Cat(packname_inc).ToCString(),std::ios::app);
    os <<"#include <" << RWCPPName->ToCString() << ".hxx>" << std::endl;
    os.close();
  
  //write file with registration
  os.open(packname.Cat(packname_rec).ToCString(),std::ios::app);
  os<<"  //Registration class "<<CPPname->ToCString()<< std::endl;
  os<<"  stepname = "<<"\""<<CPPname->ToCString()<<"\""<<";"<< std::endl;
  if(!ShortName().IsNull() && ShortName()->Length() >0) {
    os<<"  shortname = "<<"\""<<ShortName()->ToCString()<<"\""<<";"<< std::endl;
  }
  if(!Category().IsNull() && Category()->Length() >0)
    os<<"   category = "<<"Interface_Category::Number(\""<<Category()->ToCString()<<"\");"<< std::endl;
  else 
    os<<"   category = vcateg;"<< std::endl;
//PDN modification of $ on handles
//  os<<"  Handle(" << RWCPPName->ToCString()<<")& rwclass = new "<<RWCPPName->ToCString()<<";"<< std::endl;
//  os<<"  Protocol->RegisterEntity(stepname,shortname,category,rwclass);"<< std::endl<< std::endl;
  os<<"  Protocol->RegisterEntity(stepname,shortname,category,new "<<RWCPPName->ToCString()<<");"<< std::endl;
  os<<"  shortname.Clear();"<< std::endl<< std::endl;
  os.close();
  
  return Standard_True;
}

//=======================================================================
//function : WriteUses
//purpose  : 
//=======================================================================

Standard_Boolean Express_Entity::WriteUses (Standard_OStream &os, 
					     const Standard_Boolean start,
                                             DataMapOfStringInteger &dict) const
{
  Standard_Boolean st = start;
  for ( Standard_Integer i=1; i <= myInherit->Length(); i++ ) {
    Handle(Express_Entity) it = myInherit->Value(i);
    st = it->WriteUses ( os, st, dict );
    if ( i <=1 ) continue;
      
    if ( dict.IsBound ( it->CDLName()->String() ) ) continue; // avoid duplicating
    dict.Bind ( it->CDLName()->String(), 1 );
    it->Use ( GetPackageName() );
    os << ( st ? "uses" : "," ) << std::endl;
    st = Standard_False;
    os << "    ";
    os << it->CDLName()->ToCString();
  }
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Type) type = myFields->Value(i)->Type();
    if ( dict.IsBound ( type->CDLName()->String() ) ) continue; // avoid duplicating
    dict.Bind ( type->CDLName()->String(), 1 );
    type->Use ( GetPackageName() );
    if ( type->IsStandard() ) continue;
    os << ( st ? "uses" : "," ) << std::endl;
    st = Standard_False;
    os << "    ";
    os << type->CDLName()->ToCString();
  }
  return st;
}

//=======================================================================
//function : WriteRWInclude
//purpose  : 
//=======================================================================

void Express_Entity::WriteRWInclude (Standard_OStream &os, DataMapOfStringInteger &dict) const
{
  for ( Standard_Integer i=1; i <= myInherit->Length(); i++ ) {
    Handle(Express_Entity) it = myInherit->Value(i);
    if ( i >1 ) { // include hxx for base classes which are implemented as fields
      os << "#include <" << it->CPPName()->ToCString() << ".hxx>" << std::endl;
    }
    it->WriteRWInclude ( os, dict );
  }
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Type) type = myFields->Value(i)->Type();
    
    for ( Standard_Integer deep=0; deep < 10; deep++ ) {
      if ( dict.IsBound ( type->CPPName()->String() ) ) break; // avoid duplicating
      dict.Bind ( type->CDLName()->String(), 1 );
    
      Standard_Boolean iscomplex = type->IsKind(STANDARD_TYPE(Express_ComplexType));
      if ( ! deep && ! iscomplex ) break; // only complex types and their elements need #include
    
      os << "#include <" << type->CPPName()->ToCString() << ".hxx>" << std::endl;
      
      if ( ! iscomplex ) break;
      Handle(Express_ComplexType) complex = Handle(Express_ComplexType)::DownCast ( type );
      type = complex->Type();
    }
  }
}

//=======================================================================
//function : WriteRWReadField
//purpose  : 
//=======================================================================

static Handle(TCollection_HAsciiString) TypeToSTEPName (const Handle(Express_Type) &type) 
{
  Handle(TCollection_HAsciiString) cppname = type->CPPName();
  Handle(TCollection_HAsciiString) cls = cppname->Token ( "_", 2 );
  if ( cls->Length() <1 ) cls = cppname;
  return Express::ToStepName ( cls );
}

static void WriteRWReadField (Standard_OStream &os, 
			      const Standard_CString index,
			      const Standard_CString STEPName,
			      const Standard_CString varname,
			      const Handle(Express_Type) &vartype, 
			      const Standard_Integer lev,
			      const Standard_Boolean optional)
{
  // indent
  TCollection_AsciiString shift ( "  " );
  Standard_Integer level = 0;
  for (; level < lev; level++ ) shift += "  ";
  level += 2;

  // name of variable identifying number of parameter in data
  TCollection_AsciiString param ( "num" );
  if ( lev >0 ) param += TCollection_AsciiString ( lev );
  
  // decode aliased types
  Handle(Express_Type) type = vartype;
  while ( type->IsKind(STANDARD_TYPE(Express_NamedType)) ) {
    Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    if ( ! named->Item()->IsKind(STANDARD_TYPE(Express_Alias)) ) break;
    Handle(Express_Alias) alias = Handle(Express_Alias)::DownCast ( named->Item() );
    type = alias->Type();
  }
    
  // declare variable
  if ( type->IsHandle() ) os << shift << "Handle(" << type->CPPName()->ToCString() << ") a";
  else os << shift << type->CPPName()->ToCString() << " a";
  os << varname << ";" << std::endl;

  if ( optional ) {
    os << shift << "Standard_Boolean has" << varname << " = Standard_True;" << std::endl;
    os << shift << "if ( data->IsParamDefined (" << param << "," << index << ") ) {" << std::endl;
    shift += "  ";
    level++;
  }
    
  // read variable depending on its type
  if ( type->IsKind(STANDARD_TYPE(Express_NamedType)) ) {
    Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    if ( named->Item()->IsKind(STANDARD_TYPE(Express_Entity)) ) {
      os << shift << "data->ReadEntity (" << param << ", " << index << ", \"" << STEPName <<
	             "\", ach, STANDARD_TYPE(" << named->CPPName()->ToCString() << "), a" << 
		     varname << ");" << std::endl;
    }
    else if ( named->Item()->IsKind(STANDARD_TYPE(Express_Select)) ) {
      os << shift << "data->ReadEntity (" << param << ", " << index << ", \"" << STEPName <<
	             "\", ach, a" << varname << ");" << std::endl;
    }
    else if ( named->Item()->IsKind(STANDARD_TYPE(Express_Enum)) ) {
      os << shift << "if (data->ParamType (" << param << ", " << 
	             index << ") == Interface_ParamEnum) {" << std::endl;
      os << shift << "  Standard_CString text = data->ParamCValue(" << 
	             param << ", " << index << ");" << std::endl;
      Handle(Express_Enum) en = Handle(Express_Enum)::DownCast ( named->Item() );
      Handle(TCollection_HAsciiString) prefix = Express::EnumPrefix(en->Name());
      Handle(TColStd_HSequenceOfHAsciiString) names = en->Names();
      TCollection_AsciiString enpack = en->GetPackageName()->String();
      for ( Standard_Integer i=1; i <= names->Length(); i++ ) {
	Handle(TCollection_HAsciiString) ename = Express::ToStepName ( names->Value(i) );
	ename->UpperCase();
	os << shift << ( i==1 ? "  if     " : "  else if" ) << 
	               " (strcmp(text, \"." << ename->ToCString() << ".\")) a" << 
		       varname << " = " << enpack.ToCString() << "_" << prefix->ToCString()<<
		       names->Value(i)->ToCString() << ";" << std::endl;
      }
      os << shift << "  else ach.AddFail(\"Parameter #" << index << " (" <<
	             STEPName << ") has not allowed value\");" << std::endl;
      os << shift << "}" << std::endl;
      os << shift << "else ach.AddFail(\"Parameter #" << index << " (" <<
	             STEPName << ") is not enumeration\");" << std::endl;
//      os << "!!! READING/WRITING ENUM NOT IMPLEMENTED !!!" << std::endl;
//      std::cout << "Warning: Reading/Writing ENUMERATION not yet implemented (" << 
//	STEPName << ", " << varname << ", " << type->CPPName()->ToCString() << ")" << std::endl;
    }
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_ComplexType)) ) {
   
    Handle(Express_ComplexType) complex = Handle(Express_ComplexType)::DownCast ( type );
    os << shift << "Standard_Integer sub" << index << " = 0;" << std::endl;
    os << shift << "if ( data->ReadSubList (" << param << ", " << index << ", \"" << 
                   STEPName << "\", ach, sub" << index << ") ) {" << std::endl;
    os << shift << "  Standard_Integer nb" << lev << " = data->NbParams(sub" << index << ");" << std::endl;
    TCollection_AsciiString iter = lev;
    iter.Prepend ( "i" );
    TCollection_AsciiString var = lev;
    var.Prepend("nIt");
    if ( complex->Type()->IsKind(STANDARD_TYPE(Express_ComplexType)) ) { // array 2
      Handle(Express_ComplexType) complex2 = Handle(Express_ComplexType)::DownCast ( complex->Type() );
      TCollection_AsciiString jter = lev;
      jter.Prepend ( "j" );
      os << shift << "  Standard_Integer nbj" << lev << " = data->NbParams(data->ParamNumber(sub" << index << ",1));" << std::endl;
      os << shift << "  a" << varname << " = new " << type->CPPName()->ToCString() << " (1, nb" << lev << ", 1, nbj" << lev << ");" << std::endl;
      os << shift << "  for ( Standard_Integer " << iter << "=1; " << iter << " <= nb" << lev << "; " << iter << "++ ) {" << std::endl;
      os << shift << "    Standard_Integer subj" << index << " = 0;" << std::endl;
      os << shift << "    if ( data->ReadSubList (sub" << index << ", " << iter << ", \"sub-part(" << 
                     STEPName << ")\", ach, subj" << index << ") ) {" << std::endl;
      os << shift << "      Standard_Integer num" << level+2 << " = subj" << index << ";" << std::endl;
      os << shift << "      for ( Standard_Integer " << jter << "=1; " << jter << " <= nbj" << lev << "; " << jter << "++ ) {" << std::endl;
      Handle(TCollection_HAsciiString) subName = TypeToSTEPName ( complex2->Type() );
      WriteRWReadField (os, jter.ToCString(), subName->ToCString(), var.ToCString(), complex2->Type(), level+2, Standard_False );
      os << shift << "        a" << varname << "->SetValue(" << iter << "," << jter << ", a" << var << ");" << std::endl;
      os << shift << "      }" << std::endl;
      os << shift << "    }" << std::endl;
    } 
    else { // simple array
      os << shift << "  a" << varname << " = new " << type->CPPName()->ToCString() << " (1, nb" << lev << ");" << std::endl;
      os << shift << "  Standard_Integer num" << level << " = sub" << index << ";" << std::endl;
      os << shift << "  for ( Standard_Integer " << iter << "=1; " << iter << " <= nb" << lev << "; " << iter << "++ ) {" << std::endl;
      Handle(TCollection_HAsciiString) subName = TypeToSTEPName ( complex->Type() );
      WriteRWReadField (os, iter.ToCString(), subName->ToCString(), var.ToCString(), complex->Type(), level, Standard_False );
      os << shift << "    a" << varname << "->SetValue(" << iter << ", a" << var << ");" << std::endl;
    }
    os << shift << "  }" << std::endl;
    os << shift << "}" << std::endl;
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_String)) ) {
    os << shift << "data->ReadString (" << param << ", " << index << ", \"" << STEPName <<
                   "\", ach, a" << varname << ");" << std::endl;
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_Logical)) ) {
    os << shift << "data->ReadLogical (" << param << ", " << index << ", \"" << STEPName <<
                   "\", ach, a" << varname << ");" << std::endl;
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_Boolean)) ) {
    os << shift << "data->ReadBoolean (" << param << ", " << index << ", \"" << STEPName <<
                   "\", ach, a" << varname << ");" << std::endl;
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_Number)) ||
	    type->IsKind(STANDARD_TYPE(Express_Integer)) ) {
    os << shift << "data->ReadInteger (" << param << ", " << index << ", \"" << STEPName <<
                   "\", ach, a" << varname << ");" << std::endl;
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_Real)) ) {
    os << shift << "data->ReadReal (" << param << ", " << index << ", \"" << STEPName <<
                   "\", ach, a" << varname << ");" << std::endl;
  }

  if ( optional ) {
    shift.Remove ( 1, 2 );
    os << shift << "}" << std::endl;
    os << shift << "else {" << std::endl;
    os << shift << "  has" << varname << " = Standard_False;" << std::endl;
    if ( ! type->IsHandle() ) os << shift << "  a" << varname << " = 0;" << std::endl;
    os << shift << "}" << std::endl;
  }
}

//=======================================================================
//function : WriteRWReadCode
//purpose  : 
//=======================================================================

Standard_Integer Express_Entity::WriteRWReadCode (Standard_OStream &os, 
						   const Standard_Integer start,
						   const Standard_Integer own) const
{
  Standard_Integer num = start;
  
  // write code for reading inherited fields
  for ( Standard_Integer i=1; i <= myInherit->Length(); i++ ) 
    num = myInherit->Value(i)->WriteRWReadCode ( os, num, Standard_False );

  // write code for reading own fields
  if ( myFields->Length() >0 ) {
    if ( num >0 ) os << std::endl;
    os << "  // " << ( own ? "Own" : "Inherited" ) << " fields of " << Name()->ToCString() << std::endl;
  }
  
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Field) field = myFields->Value(i);
    Handle(TCollection_HAsciiString) STEPName = Express::ToStepName ( field->Name() );
    if ( ! own ) STEPName->Prepend ( Express::ToStepName ( Name()->Cat ( "." ) ) );
    Handle(TCollection_HAsciiString) varName = new TCollection_HAsciiString ( field->Name() );
    if ( ! own ) varName->Prepend ( Name()->Cat ( "_" ) );
    
    os << std::endl;
    WriteRWReadField ( os, TCollection_AsciiString(num).ToCString(), STEPName->ToCString(), 
		       varName->ToCString(), field->Type(), 0, field->IsOptional() );
    num++;
  }
  return num;
}

//=======================================================================
//function : WriteRWWriteField
//purpose  : 
//=======================================================================

static void WriteRWWriteField (Standard_OStream &os, 
			       const Standard_CString varname,
			       const Handle(Express_Type) &vartype, 
			       const Standard_Integer index,
			       const Standard_Integer lev)
{
  // indent
  TCollection_AsciiString shift ( "  " );
  Standard_Integer level = 0;
  for (; level < lev; level++ ) shift += "  ";
  level++;

  // decode aliased types
  Handle(Express_Type) type = vartype;
  while ( type->IsKind(STANDARD_TYPE(Express_NamedType)) ) {
    Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    if ( ! named->Item()->IsKind(STANDARD_TYPE(Express_Alias)) ) break;
    Handle(Express_Alias) alias = Handle(Express_Alias)::DownCast ( named->Item() );
    type = alias->Type();
  }
    
  // write variable depending on its type
  if ( type->IsKind(STANDARD_TYPE(Express_ComplexType)) ) {
    
    Handle(Express_ComplexType) complex = Handle(Express_ComplexType)::DownCast ( type );
    type = complex->Type();
    TCollection_AsciiString var ( lev );
    var.Prepend ( "Var" );
    os << shift << "SW.OpenSub();" << std::endl;
    os << shift << "for (Standard_Integer i" << index << "=1; i" << index << " <= ";
    if ( type->IsKind(STANDARD_TYPE(Express_ComplexType)) ) { // array 2 
      Handle(Express_ComplexType) complex2 = Handle(Express_ComplexType)::DownCast ( type );
      type = complex2->Type();
      os << varname << "->RowLength(); i" << index << "++ ) {" << std::endl;
      os << shift << "  SW.NewLine(Standard_False);" << std::endl;
      os << shift << "  SW.OpenSub();" << std::endl;
      os << shift << "  for (Standard_Integer j" << index << "=1; j" << index << " <= " <<
            varname << "->ColLength(); j" << index << "++ ) {" << std::endl;
      os << shift << "  " << ( type->IsHandle() ? "  Handle(" : "  " ) << type->CPPName()->ToCString() <<
                             ( type->IsHandle() ? ") " : " " ) << var.ToCString() << " = " <<
		  varname << "->Value(i" << index << ",j" << index << ");" << std::endl;
      WriteRWWriteField (os, var.ToCString(), type, index+1, level+1 );
      os << shift << "  }" << std::endl;
      os << shift << "  SW.CloseSub();" << std::endl;
    } 
    else { // simple array
      os << varname << "->Length(); i" << index << "++ ) {" << std::endl;
      os << shift << ( type->IsHandle() ? "  Handle(" : "  " ) << type->CPPName()->ToCString() <<
                     ( type->IsHandle() ? ") " : " " ) << var.ToCString() << " = " <<
		     varname << "->Value(i" << index << ");" << std::endl;
      WriteRWWriteField (os, var.ToCString(), type, index+1, level );
    }
    os << shift << "}" << std::endl;
    os << shift << "SW.CloseSub();" << std::endl;
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_Boolean)) ) {
    os << shift << "SW.SendBoolean (" << varname << ");" << std::endl;
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_Logical)) ) {
    os << shift << "SW.SendLogical (" << varname << ");" << std::endl;
  }
  else {
    Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    if ( ! named.IsNull() && named->Item()->IsKind(STANDARD_TYPE(Express_Enum)) ) {
      Handle(Express_Enum) en = Handle(Express_Enum)::DownCast ( named->Item() );
      Handle(TCollection_HAsciiString) prefix = Express::EnumPrefix(en->Name());
      
      Handle(TColStd_HSequenceOfHAsciiString) names = en->Names();
      TCollection_AsciiString enpack = en->GetPackageName()->String();
      os << shift << "switch (" << varname << ") {" << std::endl;
      for ( Standard_Integer i=1; i <= names->Length(); i++ ) {
	Handle(TCollection_HAsciiString) ename = Express::ToStepName ( names->Value(i) );
	ename->UpperCase();
	os << shift << "  case " << enpack.ToCString() << "_" << prefix->ToCString()<<
	               names->Value(i)->ToCString() << ": SW.SendEnum (\"." << 
		       ename->ToCString() << ".\"); break;" << std::endl;
      }
      os << shift << "}" << std::endl;
    }
    else {
      os << shift << "SW.Send (" << varname;
      if ( ! named.IsNull() && named->Item()->IsKind(STANDARD_TYPE(Express_Select)) ) 
	os << ".Value()";
      os << ");" << std::endl;
    }
  }
}

//=======================================================================
//function : WriteRWWriteCode
//purpose  : 
//=======================================================================

Standard_Integer Express_Entity::WriteRWWriteCode (Standard_OStream &os, 
						    const Standard_Integer start,
						    const Standard_Integer own) const
{
  Standard_Integer num = start;
  
  // write code for writing inherited fields
  for ( Standard_Integer i=1; i <= myInherit->Length(); i++ ) 
    num = myInherit->Value(i)->WriteRWWriteCode ( os, num, ( i >1 ? -1 : 1 ) );

  // write code for writing own fields
  if ( myFields->Length() >0 ) {
    os << std::endl;
    os << "  // " << ( own == 1 ? "Own" : "Inherited" ) << " fields of " << Name()->ToCString() << std::endl;
  }
  
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Field) field = myFields->Value(i);
    TCollection_AsciiString varName ( field->Name()->String() );
    if ( ! own ) varName.Prepend ( CPPName()->Cat ( "::" )->String() );
    else if ( own ==-1 ) { // inherited base class implemented as field
      varName.Prepend ( Name()->Cat ( "()->" )->String() );
    }
    varName.Prepend ( "ent->" );
    varName.AssignCat ( "()" );
    os << std::endl;
    
    if ( field->IsOptional() ) {
      os << "  if ( ent->";
      if ( ! own ) os << CPPName()->ToCString() << "::";
      else if ( own ==-1 ) os << Name()->ToCString() << "()->";
      os << "Has" << field->Name()->ToCString() << "() ) {" << std::endl;
    }
    WriteRWWriteField ( os, varName.ToCString(), field->Type(), num, ( field->IsOptional() ? 1 : 0 ) );
    if ( field->IsOptional() ) {
      os << "  }\n  else SW.SendUndef();" << std::endl;
    }
    num++;
  }
  
  return num;
}

//=======================================================================
//function : WriteRWShareField
//purpose  : 
//=======================================================================

static Standard_Boolean WriteRWShareField (Standard_OStream &os, 
					   const Standard_CString varname,
					   const Handle(Express_Type) &vartype, 
					   const Standard_Integer index,
					   const Standard_Integer lev)
{
  // indent
  TCollection_AsciiString shift ( "  " );
  Standard_Integer level = 0;
  for (; level < lev; level++ ) shift += "  ";
  level++;

  // decode aliased types
  Handle(Express_Type) type = vartype;
  while ( type->IsKind(STANDARD_TYPE(Express_NamedType)) ) {
    Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    if ( ! named->Item()->IsKind(STANDARD_TYPE(Express_Alias)) ) break;
    Handle(Express_Alias) alias = Handle(Express_Alias)::DownCast ( named->Item() );
    type = alias->Type();
  }
    
  // write variable depending on its type
  if ( type->IsKind(STANDARD_TYPE(Express_ComplexType)) ) {
    Handle(Express_ComplexType) complex = Handle(Express_ComplexType)::DownCast ( type );
    type = complex->Type();
    TCollection_AsciiString var ( lev );
    var.Prepend ( "Var" );
    std::ostringstream oos;
    if ( ! WriteRWShareField (oos, var.ToCString(), type, index + 1, level ) ) 
      return Standard_False;
    os << shift << "for (Standard_Integer i" << index << "=1; i" << index << " <= " << 
                    varname << "->Length(); i" << index << "++ ) {" << std::endl;
    os << shift << ( type->IsHandle() ? "  Handle(" : "  " ) << type->CPPName()->ToCString() <<
                   ( type->IsHandle() ? ") " : " " ) << var.ToCString() << " = " <<
		   varname << "->Value(i" << index << ");" << std::endl;
    oos << '\0';
    os << oos.str();
    os << shift << "}" << std::endl;
    return Standard_True;
  }
  if ( type->IsKind(STANDARD_TYPE(Express_NamedType)) ) {
    Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    if ( named->Item()->IsKind(STANDARD_TYPE(Express_Entity)) ) {
      os << shift << "iter.AddItem (" << varname << ");" << std::endl;
      return Standard_True;
    }
    if ( named->Item()->IsKind(STANDARD_TYPE(Express_Select)) ) {
      os << shift << "iter.AddItem (" << varname << ".Value());" << std::endl;
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : WriteRWShareCode
//purpose  : 
//=======================================================================

Standard_Integer Express_Entity::WriteRWShareCode (Standard_OStream &os, 
						    const Standard_Integer start,
						    const Standard_Integer own) const
{
  Standard_Integer num = start;
  
  // write code for sharing inherited fields
  for ( Standard_Integer i=1; i <= myInherit->Length(); i++ ) 
    num = myInherit->Value(i)->WriteRWShareCode ( os, num, ( i >1 ? -1 : Standard_False ) );

  // write code for sharing own fields
  if ( myFields->Length() >0 ) {
    os << std::endl;
    os << "  // " << ( own == 1 ? "Own" : "Inherited" ) << " fields of " << Name()->ToCString() << std::endl;
  }
  
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Field) field = myFields->Value(i);
    TCollection_AsciiString varName ( field->Name()->String() );
    if ( ! own ) varName.Prepend ( CPPName()->Cat ( "::" )->String() );
    else if ( own ==-1 ) { // inherited base class implemented as field
      varName.Prepend ( Name()->Cat ( "()->" )->String() );
    }
    varName.Prepend ( "ent->" );
    varName.AssignCat ( "()" );
    
    std::ostringstream oos;
    if ( ! WriteRWShareField ( oos, varName.ToCString(), field->Type(), num,
			       ( field->IsOptional() ? 1 : 0 ) ) ) continue;
    num++;
    os << std::endl;
    if ( field->IsOptional() ) {
      os << "  if ( ent->";
      if ( ! own ) os << CPPName()->ToCString() << "::";
      else if ( own ==-1 ) os << Name()->ToCString() << "()->";
      os << "Has" << field->Name()->ToCString() << "() ) {" << std::endl;
    }
    oos << '\0';
    os << oos.str();
    if ( field->IsOptional() ) {
      os << "  }" << std::endl;
    }
  }
  return num;
}

//=======================================================================
//function : MakeInit
//purpose  : 
//=======================================================================

Standard_Integer Express_Entity::MakeInit (Standard_OStream &os, 
					    const Standard_Integer shift,
					    const Standard_Integer own,
					    const Standard_Integer mode) const
{
  Standard_Integer sh = shift;
  
  // write code for inherited fields
  for ( Standard_Integer i=1; i <= myInherit->Length(); i++ ) {
    Handle(Express_Entity) ent = myInherit->Value(i);
    if ( mode ==3 ) {
      Standard_Integer s = 0;
      if ( i >1 ) {
	os << "\n  the" << ent->Name()->ToCString() << " = new " << ent->CPPName()->ToCString() << ";";
	os << "\n  the" << ent->Name()->ToCString() << "->Init(";
	s = 12 + ent->Name()->Length();
      }
      else {
	os << "\n  " << ent->CPPName()->ToCString() << "::Init(";
	s = 9 + ent->CPPName()->Length();
      }
      ent->MakeInit ( os, s, Standard_False, 2 );
      os << ");";
    }
    else sh = ent->MakeInit ( os, sh, ( i >1 ? -1 : Standard_False ), mode );
  }

  // write code for own fields
  
  for (Standard_Integer i=1; i <= myFields->Length(); i++ ) {
    Handle(Express_Field) field = myFields->Value(i);
    Handle(TCollection_HAsciiString) varName = new TCollection_HAsciiString ( field->Name() );
    if ( own != 1 ) varName->Prepend ( Name()->Cat ( "_" ) );

    // make CR and indent
    TCollection_AsciiString space = "";
    for ( Standard_Integer sn=0; sn < abs(sh); sn++ ) space += " ";
    Standard_Character delim = ( mode ==0 ? ';' : ( mode == 3 ? '\n' : ',' ) );
    if ( sh <0 ) os << delim << "\n" << space;
    else sh = -sh;
    
    if ( field->IsOptional() ) {
      if ( mode ==0 ) os << "has" << varName->ToCString() << ": Boolean;\n" << space;
      else if ( mode ==1 ) os << "const Standard_Boolean has" << varName->ToCString() << ",\n" << space;
      else if ( mode ==2 ) os << "has" << varName->ToCString() << ",\n" << space;
    }
    
    // write field
    if ( mode ==0 ) {
      os << "a" << varName->ToCString() << ": " << field->Type()->CDLName()->ToCString();
    }
    else if ( mode ==1 ) {
      os << "const " << ( field->Type()->IsHandle() ? "Handle(" : "" ) <<
	    field->Type()->CPPName()->ToCString() << ( field->Type()->IsHandle() ? ")" : "" ) <<
	    ( field->Type()->IsSimple() ? " a" : " &a" ) << varName->ToCString();
    }
    else if ( mode ==2 ) {
      os << "a" << varName->ToCString();
    }
    else {
      if ( field->IsOptional() ) {
	os << "def" << field->Name()->ToCString() << " = has" << varName->ToCString() << ";" << std::endl;
	os << "  if (def" << field->Name()->ToCString() << ") {\n    ";
      }
      os << "the" << field->Name()->ToCString() << " = a" << varName->ToCString() << ";";
      if ( field->IsOptional() ) 
	if ( field->Type()->IsHandle() ) os << "\n  }\n  else the" << field->Name()->ToCString() << ".Nullify();";
	else os << "\n  }\n  else the" << field->Name()->ToCString() << " = 0;";
    }
    if ( sh >0 ) sh = -sh;
  }
  
  return sh;
}
//=======================================================================
//function : SetAbstractFlag
//purpose  : 
//=======================================================================

 void Express_Entity::SetAbstractFlag(const Standard_Boolean isAbstract) 
{
   myisAbstract = isAbstract;
}

//=======================================================================
//function : AbstractFlag
//purpose  : 
//=======================================================================

 Standard_Boolean Express_Entity::AbstractFlag() const
{
  return myisAbstract;
}

