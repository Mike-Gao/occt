// Created:	Tue Nov  2 13:14:31 1999
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

#include <Express_Item.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Item,Standard_Transient)

//=======================================================================
//function : Express_Item
//purpose  : 
//=======================================================================

Express_Item::Express_Item (const Standard_CString name)
{
  myName = new TCollection_HAsciiString ( name );
  myMark = Standard_False;
  myShortName = new TCollection_HAsciiString;
  myCategory = new TCollection_HAsciiString;
  myhasCheck = Standard_False;
  myhasFillShared = Standard_False;
}

//=======================================================================
//function : Express_Item
//purpose  : 
//=======================================================================

Express_Item::Express_Item (const Handle(TCollection_HAsciiString) &name)
{
  myName = name;
  myMark = Standard_False;
  myShortName = new TCollection_HAsciiString;
  myCategory = new TCollection_HAsciiString;
  myhasCheck = Standard_False;
  myhasFillShared = Standard_False;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

const Handle(TCollection_HAsciiString) &Express_Item::Name () const
{
  return myName;
}

//=======================================================================
//function : CPPName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_Item::CPPName () const
{
  Handle(TCollection_HAsciiString) name = GetPackageName ( Standard_True )->Cat ( "_" );
  name->AssignCat ( myName );
  return name;
}

//=======================================================================
//function : CDLName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_Item::CDLName () const
{
  Handle(TCollection_HAsciiString) name = myName->Cat ( " from " );
  name->AssignCat ( GetPackageName ( Standard_True ) );
  return name;
}

//=======================================================================
//function : SetPackageName
//purpose  : 
//=======================================================================

void Express_Item::SetPackageName (const Handle(TCollection_HAsciiString) &pack)
{
  myPack = pack;
}

//=======================================================================
//function : GetPackageName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) Express_Item::GetPackageName (const Standard_Boolean Auto) const
{
  if ( myPack.IsNull() && Auto ) return new TCollection_HAsciiString ( "StepStep" );
  return myPack;
}

//=======================================================================
//function : GetMark
//purpose  : 
//=======================================================================

Standard_Boolean Express_Item::GetMark () const
{
  return myMark;
}

//=======================================================================
//function : SetMark
//purpose  : 
//=======================================================================

void Express_Item::SetMark (const Standard_Boolean mark) 
{
  myMark = mark;
}

//=======================================================================
//function : Generate
//purpose  : 
//=======================================================================

Standard_Boolean Express_Item::Generate ()
{
  if ( ! GetMark() ) return Standard_False;
  if ( GetPackageName().IsNull() )
    SetPackageName ( GetPackageName ( Standard_True ) );
  SetMark ( Standard_False );
  return GenerateClass();
}

//=======================================================================
//function : Use
//purpose  : 
//=======================================================================

Standard_Boolean Express_Item::Use (const Handle(TCollection_HAsciiString) &pack, 
				     const Standard_Boolean defer) 
{
  if ( ! GetPackageName().IsNull() ) {
    // issue a warning message if item from known package uses item from unknown package (StepStep)
    if ( pack->String().IsDifferent ( "StepStep" ) &&
         GetPackageName()->String().IsEqual ( "StepStep" ) ) {
      std::cout << "Warning: item " << Name()->ToCString() << " has no package assigned, but used from package " << pack->ToCString() << std::endl;
    }
  
    return Standard_False;
  }
  
  // issue a warning message if type still does not have package assigned
  if ( pack->String().IsDifferent ( "StepStep" ) ) {
    std::cout << "Warning: item " << Name()->ToCString() << " has no package assigned, setting " << pack->ToCString() << std::endl;
  }
  
  SetPackageName ( pack );
  SetMark ( Standard_True );
  if ( defer ) return Standard_False;
  
  // manage indent for cout in order to mark structure of calls
  static Standard_Integer shift=0;
  shift++;
  for ( Standard_Integer i=0; i < shift; i++ ) std::cout << "  ";
  Standard_Boolean res = Generate(); 
  shift--;
  return res;
}
//=======================================================================
//function : SetCategory
//purpose  : 
//=======================================================================

 void Express_Item::SetCategory(const Handle(TCollection_HAsciiString)& categ) 
{
   myCategory = categ;
}

//=======================================================================
//function : Cartegory
//purpose  : 
//=======================================================================

 Handle(TCollection_HAsciiString) Express_Item::Category() const
{
  return myCategory;
}

//=======================================================================
//function : SetShortName
//purpose  : 
//=======================================================================

 void Express_Item::SetShortName(const Handle(TCollection_HAsciiString)& shname) 
{
   myShortName = shname;
}

//=======================================================================
//function : ShortName
//purpose  : 
//=======================================================================

 Handle(TCollection_HAsciiString) Express_Item::ShortName() const
{
  return  myShortName;
}
//=======================================================================
//function : SetCheckFlag
//purpose  : 
//=======================================================================

 void Express_Item::SetCheckFlag(const Standard_Boolean checkFlag) 
{
  myhasCheck = checkFlag;
}

//=======================================================================
//function : CheckFlag
//purpose  : 
//=======================================================================

 Standard_Boolean Express_Item::CheckFlag() const
{
  return myhasCheck;
}

//=======================================================================
//function : SetFillSharedFlag
//purpose  : 
//=======================================================================

 void Express_Item::SetFillSharedFlag(const Standard_Boolean fillsharedFlag) 
{
  myhasFillShared = fillsharedFlag;
}

//=======================================================================
//function : FillSharedFlag
//purpose  : 
//=======================================================================

 Standard_Boolean Express_Item::FillSharedFlag() const
{
  return myhasFillShared;
}

