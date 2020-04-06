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

#include <Express_Select.hxx>
#include <Express.hxx>

#include <OSD_Protection.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Path.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

#include <Express_ComplexType.hxx>
#include <Express_Enum.hxx>
#include <Express_Entity.hxx>
#include <Express_Alias.hxx>
#include <Express_PredefinedType.hxx>
#include <Express_Type.hxx>
#include <Express_HSequenceOfItem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Select,Express_Item)

//=======================================================================
//function : Express_Select
//purpose  : 
//=======================================================================

Express_Select::Express_Select (const Standard_CString name, 
				  const Handle(TColStd_HSequenceOfHAsciiString) &names)
     : Express_Item(name), myNames(names)
{
  myItems = new Express_HSequenceOfItem;
}

//=======================================================================
//function : Names
//purpose  : 
//=======================================================================

const Handle(TColStd_HSequenceOfHAsciiString) &Express_Select::Names () const
{
  return myNames;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

const Handle(Express_HSequenceOfItem) &Express_Select::Items () const
{
  return myItems;
}

//=======================================================================
//function : GenerateClass
//purpose  : 
//=======================================================================

Standard_Boolean Express_Select::GenerateClass () const
{
  Handle(TCollection_HAsciiString) CPPname = CPPName();
  
  Handle(TColStd_HSequenceOfInteger) seqMember = new TColStd_HSequenceOfInteger;
  Handle(TColStd_HSequenceOfInteger) seqEntities = new TColStd_HSequenceOfInteger;
  for ( Standard_Integer i=1; i <= myItems->Length(); i++ ) {
    Handle(Express_Item) item = myItems->Value(i);
    if (item->IsKind(STANDARD_TYPE(Express_Entity)) || item->IsKind(STANDARD_TYPE(Express_Select))
      || item->IsKind(STANDARD_TYPE(Express_Alias)) || item->IsKind(STANDARD_TYPE(Express_ComplexType)))
       seqEntities->Append(i);
    else seqMember->Append(i);
    
  }
  std::cout << "Generating SELECT " << CPPname->ToCString() << std::endl;
  if(!seqMember->IsEmpty()) {
    std::cout << "Generating SELECTMember " << CPPname->ToCString() << "Member"<< std::endl;
    GenerateSelectMember(seqMember);
  }
  // create a package directory (if not yet exist)
  OSD_Protection prot ( OSD_RX, OSD_RWX, OSD_RX, OSD_RX );
  TCollection_AsciiString pack = GetPackageName()->String();
  OSD_Path path ( pack );
  OSD_Directory dir ( path );
  dir.Build ( prot );
  pack += "/";
  pack += CPPname->String();
  
  //===============================
  // Step 1: generating HXX
  
  // Open HXX file 
  std::ofstream os ( pack.Cat ( ".hxx" ).ToCString() );

  // write header
  Express::WriteFileStamp ( os );
  
  // write start define
  os << "#ifndef _" << CPPname->ToCString() << "_HeaderFile" << std::endl;
  os << "#define _" << CPPname->ToCString() << "_HeaderFile" << std::endl;
  os << std::endl;

  // write common includes
  os << "#include <Standard.hxx>" << std::endl;
  os << "#include <Standard_DefineAlloc.hxx>" << std::endl;
  os << "#include <Standard_Handle.hxx>" << std::endl;
  os << "#include <StepData_SelectType.hxx>" << std::endl;
  os << "#include <Standard_Integer.hxx>" << std::endl;
  os << std::endl;

  os << "class Standard_Transient;" << std::endl;
  if (!seqMember->IsEmpty())
    os << "class StepData_SelectMember;" << std::endl;

  Standard_Integer jj = 1;
  for (Standard_Integer i = 1; i <= myItems->Length(); i++) {
    Handle(Express_Item) item = myItems->Value(i);
    item->Use(GetPackageName());
    if (item->IsKind(STANDARD_TYPE(Express_Alias))) {
      Handle(Express_Type) type = Handle(Express_Alias)::DownCast(item)->Type();
      TCollection_AsciiString ats = type->CPPName()->String();
      if (type->IsStandard()) //(STANDARD_TYPE(Express_PredefinedType)))
        continue;
    }
    os << "class " << item->CPPName()->ToCString() << ";" << std::endl;
    jj++;
  }
  os << std::endl;

  // class declaration
  os << "//! Representation of STEP SELECT type " << Name()->ToCString() << std::endl;
  os << "class " << CPPname->ToCString() << " : public StepData_SelectType" << std::endl;
  os << "{" << std::endl;
  os << std::endl;
  os << "public:" << std::endl;
  os << std::endl;
  os << "  DEFINE_STANDARD_ALLOC" << std::endl;
  os << std::endl;

  // default constructor
  os << "  //! Empty constructor" << std::endl;
  os << "  Standard_EXPORT " << CPPname->ToCString() << "();" << std::endl;
  os << std::endl;
  
  // write common methods section
  os << "  //! Recognizes a kind of " << Name()->ToCString() << " select type" << std::endl;
  for (Standard_Integer i = 1; i <= seqEntities->Length(); i++) {
    Standard_Integer ind = seqEntities->Value(i);
    os << "  //! -- " << i << " -> " << myNames->Value(ind)->ToCString() << std::endl;
  }
  os << "  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;" << std::endl;
  os << std::endl;

  if(!seqMember->IsEmpty()) {
    os << "  //! Recognizes items of select member " << Name()->ToCString() << "Member" << std::endl;
    for (Standard_Integer i = 1; i <= seqMember->Length(); i++) {
      Standard_Integer ind = seqMember->Value(i);
      os << "  //! -- " << i << " -> " << myNames->Value(ind)->ToCString() << std::endl;
    }
    os << "  //! -- 0 else" << std::endl;
    os << "  Standard_EXPORT virtual Standard_Integer CaseMem (const Handle(StepData_SelectMember)& ent) const Standard_OVERRIDE;" << std::endl;
    os << std::endl;

    os << " //! Returns a new select member the type " << Name()->ToCString() << "Member" << std::endl;
    os << "  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const Standard_OVERRIDE;" << std::endl;
    os << std::endl;
  }

  // write methods get for entities
  for (Standard_Integer i = 1; i <= seqEntities->Length(); i++) {
    Standard_Integer ind = seqEntities->Value(i);
    Handle(Express_Item) item = myItems->Value(ind);
    Handle(TCollection_HAsciiString) name = item->Name();
    os << "  //! Returns Value as " << name->ToCString() << " (or Null if another type)" << std::endl;
    os << "  Standard_EXPORT Handle(" << item->CPPName()->ToCString() << ") "  << name->ToCString() << "() const;" << std::endl;
    os << std::endl;
  }

  // writes method set and get for enum , integer, real and string.
  for (Standard_Integer i = 1; i <= seqMember->Length(); i++) {
    Standard_Integer ind = seqMember->Value(i);
    Handle(Express_Item) item = myItems->Value(ind);
    Handle(TCollection_HAsciiString) name = item->Name();
    os << "  //! Set Value for " << name->ToCString() << std::endl;
    os << "  Standard_EXPORT void Set" << name->ToCString()
       << " (const " << item->CPPName()->ToCString() << " theVal);"<< std::endl;
    os << std::endl;
    os << "  //! Returns Value as " << name->ToCString() << " (or Null if another type)" << std::endl;
    os << "  " << item->CPPName()->ToCString() << " " << name->ToCString() << "() const;" << std::endl;
    os << std::endl;
  }
  
  // write end
  os << "};" << std::endl;
  os << "#endif // _" << CPPname->ToCString() << "_HeaderFile" << std::endl;
  os.close();
  
  //===============================
  // Step 2: generating CXX
  
  // Open CXX file 
  os.open ( pack.Cat ( ".cxx" ).ToCString() );

  // write header
  Express::WriteFileStamp ( os );

  // write include section
  os << "#include <" << CPPname->ToCString() << ".hxx>" << std::endl;
  if(!seqMember->IsEmpty()) {
    os << "#include <" << CPPName()->ToCString() << "Member.hxx>"<< std::endl;
    os << "#include <TCollection_HAsciiString.hxx>"<< std::endl;
  }
  for (Standard_Integer i = 1; i <= seqEntities->Length(); i++)  {
    Standard_Integer ind = seqEntities->Value(i);
    os << "#include <" <<
      myItems->Value(ind)->CPPName()->ToCString() << ".hxx>" << std::endl;
  }

  // write constructor
  Express::WriteMethodStamp ( os, CPPname );
  os << CPPname->ToCString() << "::" << CPPname->ToCString() << " ()" << std::endl;
  os << "{" << std::endl << "}" << std::endl;

  // write CaseNum method
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "CaseNum" ) );
  os << "Standard_Integer " << CPPname->ToCString() << "::CaseNum (const Handle(Standard_Transient)& ent) const" << std::endl;
  os << "{" << std::endl;
  
  if(!seqEntities->IsEmpty()) {
    os << "  if (ent.IsNull()) return 0;" << std::endl;
    for (Standard_Integer i=1; i <= seqEntities->Length(); i++ )  {
      Standard_Integer ind = seqEntities->Value(i);
      os << "  if (ent->IsKind(STANDARD_TYPE(" << 
        myItems->Value(ind)->CPPName()->ToCString() << "))) return " << i << ";" << std::endl;
    }
    os << "  return 0;\n}" << std::endl;
  }
  else os << "  return 0;\n}" << std::endl;

  if(!seqMember->IsEmpty()) { //gka for AP209
    //write CaseMem method
    Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "CaseMem" ) );
    os<<"Standard_Integer "<<CPPname->ToCString()<<"::CaseMem (const Handle(StepData_SelectMember)& ent) const"<< std::endl;
    os<<"{"<< std::endl;
    os<<" if(ent.IsNull()) return 0;"<< std::endl;
    //os<<" Handle("<< CPPName()->ToCString()<<"Member sm = Handle("<<CPPName()->ToCString()<<"Member)::DownCast(ent);"<< std::endl; 
    //os<<" if(sm.IsNull()) return 0;"<< std::endl;
    //os<<" Handle(TCollection_HAsciiString) name;"<< std::endl;
    for( int j = 1; j<= seqMember->Length(); j++) {
      Standard_Integer ind = seqMember->Value(j);
      //os<<"  name = new TCollection_HAsciiString(\""<<myNames->Value(ind)->ToCString()<<"\");"<< std::endl;
      if(j ==1)
        os<<" if(ent->Matches(\""<<myNames->Value(ind)->ToCString()<< "\")) return "<<j<<";"<< std::endl;
      else os<<" else if(ent->Matches(\""<<myNames->Value(ind)->ToCString()<< "\")) return "<<j<<";"<< std::endl;
    }
    os<<" else return 0;"<< std::endl;
    os<<"}"<< std::endl;
    
    //write NewMember method
     Express::WriteMethodStamp ( os, new TCollection_HAsciiString ( "NewMember" ) );
    os<<"Handle(StepData_SelectMember) "<<CPPname->ToCString()<<"::NewMember() const"<< std::endl;
    os<<"{"<< std::endl;
    os<<" return new "<<CPPname->ToCString()<<"Member;"<< std::endl;
    os<<"}"<< std::endl;
    
  }
  
  
  // write methods Get for entities.
  for (Standard_Integer i=1; i <= seqEntities->Length(); i++ ) {
    Standard_Integer ind = seqEntities->Value(i);
    Handle(Express_Item) item = myItems->Value(ind);
    Express::WriteMethodStamp ( os, item->Name() );
    os << "Handle(" << item->CPPName()->ToCString() << ") " << CPPname->ToCString() <<
          "::" << item->Name()->ToCString() << " () const" << std::endl;
    os << "{\n  return Handle(" << item->CPPName()->ToCString() << ")::DownCast(Value());\n}" << std::endl;
  }
  
  //write methods Set and Get for Integer, Real, String and Enum
  for (Standard_Integer i=1; i <= seqMember->Length(); i++ ) {
    Standard_Integer ind = seqMember->Value(i);
    Handle(Express_Item) item = myItems->Value(ind);
    Handle(TCollection_HAsciiString) stamp = new TCollection_HAsciiString("Set");
    stamp->AssignCat(item->Name());
    Express::WriteMethodStamp ( os, stamp );
    
    Standard_Boolean isString = (item->CPPName()->String().Location("HAsciiString",1,item->CPPName()->Length()) > 0);
    Handle(TCollection_HAsciiString) nameFunc = new TCollection_HAsciiString;
    Handle(TCollection_HAsciiString) Func = new TCollection_HAsciiString;
    Standard_Boolean isEnum = item->IsKind(STANDARD_TYPE(Express_Enum));
    if(!isEnum) {
      if(isString)
        Func->AssignCat("String");
      else {
        nameFunc->AssignCat(item->CPPName());
        Func = nameFunc;
        Standard_Integer splitind = nameFunc->String().Location("_",1,item->CPPName()->Length());
        if(splitind)
          Func = nameFunc->Split(splitind);
      }
    }
    
    //write method set
    if(isString) {
      os <<  "void "<<CPPname->ToCString() <<
        "::" << "Set"<<item->Name()->ToCString() << " (const Handle("<<item->CPPName()->ToCString() << ") &val)" << std::endl;
    }
    else  
      os <<  "void "<<CPPname->ToCString() <<
          "::" << "Set"<<item->Name()->ToCString() << " (const "<<item->CPPName()->ToCString() << " val)" << std::endl;

    os<<"{"<< std::endl;
    
    os<< "  Handle("<<CPPname->ToCString()<<"Member) SelMem = Handle(" << CPPname->ToCString()<<"Member)::DownCast(Value());"<< std::endl;
    os<<"  if(SelMem.IsNull()) return;"<< std::endl;
    os<<" Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString(\""<<item->Name()->ToCString()<<"\");"<< std::endl;
    os<<" SelMem->SetName(name->ToCString());"<< std::endl;
    if(isEnum) 
      os<<" SelMem->SetEnum((Standard_Integer)val);"<< std::endl;
    else if(isString)
      os<<" SelMem->Set"<<Func->ToCString()<<"(val->ToCString());"<< std::endl;
    else 
      os<<" SelMem->Set"<<Func->ToCString()<<"(val);"<< std::endl;
    
    os<<"}"<< std::endl;
    
    //write method get
    Express::WriteMethodStamp ( os, item->Name() );
    if(isString)
       os << "Handle("<<item->CPPName()->ToCString() << ") "<<CPPname->ToCString() <<
      "::" << item->Name()->ToCString() << " () const" << std::endl;
    else 
      os << item->CPPName()->ToCString() << " "<<CPPname->ToCString() <<
      "::" << item->Name()->ToCString() << " () const" << std::endl;
    
    os<<"{"<< std::endl;
    os<< "  Handle("<<CPPname->ToCString()<<"Member) SelMem = Handle(" << CPPname->ToCString()<<"Member)::DownCast(Value());"<< std::endl;
    os<<"  if(SelMem.IsNull()) return 0;"<< std::endl;
    os<<" Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;"<< std::endl;
    os<<" name->AssignCat(SelMem->Name());"<< std::endl;
    os<<" Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString(\""<<item->Name()->ToCString()<<"\");"<< std::endl;
    os<<" if(name->IsDifferent(nameitem)) return 0;"<< std::endl;
    if(isEnum) {
      //nameFunc.AssignCat(item->Name()->ToCString());
      os<<" Standard_Integer numit = SelMem->Enum();"<< std::endl;
      os<<"  "<<item->CPPName()->ToCString()<<" val;"<< std::endl;
      os<<" switch(numit) {"<< std::endl;
      Handle(Express_Enum) Enum = Handle(Express_Enum)::DownCast(item);
      Handle(TColStd_HSequenceOfHAsciiString) enitems =  Enum->Names();
      Handle(TCollection_HAsciiString) prefix = Express::EnumPrefix(item->Name());
      for(Standard_Integer k = 1; k <= enitems->Length(); k++)
        os<<"    case "<<k<<" : val = "<<item->Name()->ToCString()<<"_"<<prefix->ToCString()<<enitems->Value(k)->ToCString()<<"; break;"<< std::endl;
      os<<"    default : return 0;break;"<< std::endl;
      os<<"  }"<< std::endl;
    }
    else if(isString) {
      os<<" Handle(TCollection_HAsciiString) val = new TCollection_HAsciiString;"<< std::endl;
      os<<" val->AssignCat(SelMem->String());"<< std::endl;
    }
    
    else 
      os<<" "<<item->CPPName()->ToCString()<<" val = SelMem->"<<Func->ToCString()<<"();"<< std::endl;
      
    
    os<<" return val;"<< std::endl;
    os<<"}"<< std::endl;
      
  }
  // close CXX
  os.close();
    
  return Standard_True;
}
//=======================================================================
//function : GenerateSelectMember
//purpose  : 
//=======================================================================

 Standard_Boolean Express_Select::GenerateSelectMember(const Handle(TColStd_HSequenceOfInteger)& seqMember) const
{
  Handle(TCollection_HAsciiString) CPPname = new TCollection_HAsciiString;
  CPPname->AssignCat(CPPName());
  CPPname->AssignCat("Member");
    
  Handle(TCollection_HAsciiString) MemberName = new TCollection_HAsciiString;
  MemberName->AssignCat(Name());
  MemberName->AssignCat("Member");
   // create a package directory (if not yet exist)
  OSD_Protection prot ( OSD_RX, OSD_RWX, OSD_RX, OSD_RX );
  TCollection_AsciiString pack = GetPackageName()->String();
  OSD_Path path ( pack );
  OSD_Directory dir ( path );
  dir.Build ( prot );
  pack += "/";
  pack += CPPname->String();
  // Step 1: generating HXX
  
  // Open HXX file 
  std::ofstream os ( pack.Cat ( ".hxx" ).ToCString() );
  // write header
  Express::WriteFileStamp ( os );

  // write start define
  os << "#ifndef _" << CPPname->ToCString() << "_HeaderFile" << std::endl;
  os << "#define _" << CPPname->ToCString() << "_HeaderFile" << std::endl;
  os << std::endl;

  // includes
  os << "#include <Standard.hxx>" << std::endl;
  os << "#include <Standard_Type.hxx>" << std::endl;
  os << "#include <Standard_Boolean.hxx>" << std::endl;
  os << "#include <Standard_CString.hxx>" << std::endl;
  os << "#include <Standard_Integer.hxx>" << std::endl;
  os << "#include <StepData_SelectNamed.hxx>" << std::endl;
  os << std::endl;
  os << "DEFINE_STANDARD_HANDLE(" << CPPname->ToCString() << ", StepData_SelectNamed)" << std::endl;
  os << std::endl;

  // write start of declaration (inheritance)
  os << "  //! Representation of member for STEP SELECT type " << Name()->ToCString() << std::endl;
  os << "class " << CPPname->ToCString() << " : public StepData_SelectNamed" << std::endl;
  os << "{" << std::endl;
  os << "public:" << std::endl;
  
   // write methods
  os << "  //! Empty constructor" << std::endl;
  os << "  Standard_EXPORT " << CPPname->ToCString() << "();" << std::endl;
  os << std::endl;
  
  os << "  //! Returns True if has name" << std::endl;
  os << "  Standard_EXPORT virtual Standard_Boolean HasName() const Standard_OVERRIDE;" << std::endl;
  os << std::endl;
  os << "  //! Returns set name" << std::endl;
  os << "  Standard_EXPORT virtual Standard_CString Name() const Standard_OVERRIDE;" << std::endl;
  os << std::endl;
  os << "  //! Set name" << std::endl;
  os << "  Standard_EXPORT virtual Standard_Boolean SetName(const Standard_CString name) Standard_OVERRIDE;" << std::endl;
  os << std::endl;
  os << "  //! Tells if the name of a SelectMember matches a given one;" << std::endl;
  os << "  Standard_EXPORT virtual Standard_Boolean Matches(const Standard_CString name) const Standard_OVERRIDE;" << std::endl;
  os << std::endl;

  //write fields
  os <<"private:"<< std::endl;
  os << "  Standard_Integer myCase;"<< std::endl<< std::endl;
  // write end
  os << "};" << std::endl;
  os << "#endif // _" << CPPname->ToCString() << "_HeaderFile" << std::endl;
  os.close();

  //===============================
  // Step 2: generating CXX
   // Open CXX file 
  os.open ( pack.Cat ( ".cxx" ).ToCString() );

  // write header
  Express::WriteFileStamp ( os );

  // write include section
  os << "#include <" << CPPname->ToCString() << ".hxx>" << std::endl;
  os << "#include <TCollection_HAsciiString.hxx>"<< std::endl;
  // write constructor
  Express::WriteMethodStamp ( os, CPPname );
  os << CPPname->ToCString() << "::" << CPPname->ToCString() << " () : myCase(0) " << std::endl;
  os << "{" << std::endl << "}" << std::endl;
  
  //write method HasName
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ("HasName") );
  os<<"Standard_Boolean "<<CPPname->ToCString()<<"::HasName() const"<< std::endl;
  os << "{" << std::endl;
  os<< " return myCase > 0;"<< std::endl;
  os<< "}" << std::endl;
  
  Standard_Boolean hasEnum = Standard_False;
  //write method Name
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ("Name") );
  os<<"Standard_CString "<<CPPname->ToCString()<<"::Name() const"<< std::endl;
  os << "{" << std::endl;
  os << "  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString;"<< std::endl;
  os << "  switch(myCase)  {"<< std::endl;
  Standard_Integer i = 1;
  for( i = 1; i <= seqMember->Length(); i++) {
    Standard_Integer ind = seqMember->Value(i);
    Handle(Express_Item) item = myItems->Value(ind);
    if(item->IsKind(STANDARD_TYPE(Express_Enum)))
      hasEnum = Standard_True;
    os << "    case "<<i<<"  : aName->AssignCat(\""<<myNames->Value(ind)->ToCString()<<"\"); break;"<< std::endl;
  }
  os << "    default : break;"<< std::endl;
  os<<"  }"<< std::endl;
  os<<"  return aName->ToCString();"<< std::endl;
  os<< "}" << std::endl;
  
  //write static method for compare name
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ("CompareNames") );
  os<<"static Standard_Integer CompareNames(const Standard_CString name";
  if(hasEnum)
    os<<",Standard_Integer &numen) "<< std::endl;
  else
    os<<")"<< std::endl;
  os << "{" << std::endl;
  os << "  Standard_Integer thecase = 0;"<< std::endl;
  os << "  if (!name || name[0] == \'/0\') thecase = 0;"<< std::endl;
  for( i = 1; i <= seqMember->Length(); i++) {
    Standard_Integer ind = seqMember->Value(i);
    Handle(Express_Item) item = myItems->Value(ind);
    if(item->IsKind(STANDARD_TYPE(Express_Enum))) {
      Handle(Express_Enum) en = Handle(Express_Enum)::DownCast(item);
      for(Standard_Integer k =1 ; k <= en->Names()->Length();k++) {
         os<<"   else if(!strcmp (name,\""<<en->Names()->Value(k)->ToCString()<<"\")) { "<< std::endl;
         os<<"     thecase = "<<i<<";"<< std::endl;
         os<<"     numen =  "<<k<<";"<< std::endl;
         os<<"   }"<< std::endl; 
         
      }
    }
    else
      os<<"   else if(!strcmp (name,\""<<myNames->Value(ind)->ToCString()<<"\")) thecase = "<<i<<";"<< std::endl;  
  }
  os << "  return thecase;"<< std::endl;
  os << "}" << std::endl;
  
  //write method SetName
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ("SetName") );
  os<<"Standard_Boolean "<<CPPname->ToCString()<<"::SetName(const Standard_CString name) "<< std::endl;
  os << "{" << std::endl;
  if(hasEnum) {
    os << "  Standard_Integer numit = 0;"<< std::endl;
    os << "  myCase = CompareNames(name,numit);"<< std::endl;
  
    os << "  if (numit) SetInteger(numit);"<< std::endl;
  }
  else
    os << "  myCase = CompareNames(name);"<< std::endl;
  os << "  return (myCase >0);"<< std::endl;
  
  os<< "}" << std::endl;
  //write method Matches
  Express::WriteMethodStamp ( os, new TCollection_HAsciiString ("Matches") );
  os<<"Standard_Boolean "<<CPPname->ToCString()<<"::Matches(const Standard_CString name) const"<< std::endl;
  os << "{" << std::endl;
  if(hasEnum) {
    os << "  Standard_Integer numit = 0;"<< std::endl;
    os<< " Standard_Integer thecase = CompareNames(name,numit);"<< std::endl;
  }
  else
    os<< " Standard_Integer thecase = CompareNames(name);"<< std::endl;
  os << "  return (thecase > 0);"<< std::endl;
  os << "}" << std::endl;
  return Standard_True;
}
