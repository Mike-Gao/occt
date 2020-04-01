// Created:	Tue Nov  2 12:29:06 1999
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

#include <Express_Schema.hxx>

#include <Express_Type.hxx>
#include <Express_NamedType.hxx>
#include <Express_ComplexType.hxx>
#include <Express_Alias.hxx>
#include <Express_Select.hxx>
#include <Express_Enum.hxx>
#include <Express_Entity.hxx>
#include <Express_Field.hxx>
#include <Express_HSequenceOfField.hxx>
#include <Express_HSequenceOfEntity.hxx>

#include <TColStd_HSequenceOfHAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Schema,Standard_Transient)

//=======================================================================
//function : Express_Schema
//purpose  : 
//=======================================================================

Express_Schema::Express_Schema (const Standard_CString name,
				  const Handle(Express_HSequenceOfItem) &items)
{
  myName = new TCollection_HAsciiString ( name );
  myItems = items;
  Prepare();
}

//=======================================================================
//function : Express_Schema
//purpose  : 
//=======================================================================

Express_Schema::Express_Schema (const Handle(TCollection_HAsciiString) &name,
				  const Handle(Express_HSequenceOfItem) &items)
{
  myName = name;
  myItems = items;
  Prepare();
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

const Handle(TCollection_HAsciiString) &Express_Schema::Name () const
{
  return myName;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

const Handle(Express_HSequenceOfItem) &Express_Schema::Items () const
{
  return myItems;
}

//=======================================================================
//function : NbItems
//purpose  : 
//=======================================================================

Standard_Integer Express_Schema::NbItems () const
{
  return myItems->Length();
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const Standard_Integer num) const
{
  return myItems->Value(num);
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const Standard_CString name, 
					     const Standard_Boolean silent) const
{
  if ( ! myDict.IsBound ( name ) ) {
    if ( ! silent )
      std::cout << "Error: attempt to access unknown item by name " << name << std::endl;
    return 0;
  }
  return myDict.Find (name);
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const TCollection_AsciiString &name) const
{
  return Item ( name.ToCString() );
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const Handle(TCollection_HAsciiString) &name) const
{
  return Item ( name->ToCString() );
}

//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================

// Convert STEP-style name (lowercase, with underscores)
// to CASCADE-style name (each word starts with uppercase, no intervals)
static void NameToCasCade (const Handle(TCollection_HAsciiString) &name)
{
  if ( name.IsNull() ) return;
  for ( Standard_Integer i=1; i <= name->Length(); i++ ) {
    if ( name->Value(i) == '_' ) {
      name->Remove ( i );
    }
    else if ( i >1 ) continue;
    name->SetValue ( i, UpperCase ( name->Value(i) ) );
  }
}

// Convert names for Type object
static void NameToCasCade (const Handle(Express_Type) &type)
{
  if ( type->IsKind(STANDARD_TYPE(Express_NamedType)) ) {
    const Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    NameToCasCade ( named->Name() );
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_ComplexType)) ) {
    const Handle(Express_ComplexType) complex = Handle(Express_ComplexType)::DownCast ( type );
    NameToCasCade ( complex->Type() );
  }
}

// Prepare data: convert names to CasCade, fill dictionary of typenames
// and set handles to items where they are referenced by names
void Express_Schema::Prepare ()
{
  myDict.Clear();
  if ( myItems.IsNull() ) return;
  
  Standard_Integer nbItems = NbItems();
  
  // convert names annd fill dictionary
  for ( Standard_Integer num=1; num <= nbItems; num++ ) {
    // get item
    const Handle(Express_Item) item = Item(num);

    // change item name
    NameToCasCade ( item->Name() );

    // change names of referred types and other names
    if ( item->IsKind(STANDARD_TYPE(Express_Alias)) ) {
      const Handle(Express_Alias) alias = Handle(Express_Alias)::DownCast ( item );
      NameToCasCade ( alias->Type() );
    }
    else if ( item->IsKind(STANDARD_TYPE(Express_Select)) ) {
      const Handle(Express_Select) select = Handle(Express_Select)::DownCast ( item );
      for ( Standard_Integer i=1; i <= select->Names()->Length(); i++ )
	NameToCasCade ( select->Names()->Value(i) );
    }
    else if ( item->IsKind(STANDARD_TYPE(Express_Enum)) ) {
      const Handle(Express_Enum) en = Handle(Express_Enum)::DownCast ( item );
      for ( Standard_Integer i=1; i <= en->Names()->Length(); i++ )
	NameToCasCade ( en->Names()->Value(i) );
    }
    else if ( item->IsKind(STANDARD_TYPE(Express_Entity)) ) {
      const Handle(Express_Entity) ent = Handle(Express_Entity)::DownCast ( item );
      for ( Standard_Integer i=1; i <= ent->SuperTypes()->Length(); i++ )
	NameToCasCade ( ent->SuperTypes()->Value(i) );
      const Handle(Express_HSequenceOfField) fields = ent->Fields();
      for (Standard_Integer i=1; i <= fields->Length(); i++ ) {
	NameToCasCade ( fields->Value(i)->Name() );
	NameToCasCade ( fields->Value(i)->Type() );
      }
    }

    // add to dictionary
    myDict.Bind (item->Name()->String(), item);
  }

  // set references to items from other items and types
  for (Standard_Integer num=1; num <= nbItems; num++ ) {
    const Handle(Express_Item) item = Item(num);

    if ( item->IsKind(STANDARD_TYPE(Express_Alias)) ) {
      const Handle(Express_Alias) alias = Handle(Express_Alias)::DownCast ( item );
      PrepareType ( alias->Type() );
      // for aliases, define package to avoid warnings
      alias->SetPackageName ( new TCollection_HAsciiString ( "Standard" ) );
      continue;
    }
    else if ( item->IsKind(STANDARD_TYPE(Express_Select)) ) {
      const Handle(Express_Select) select = Handle(Express_Select)::DownCast ( item );
      Handle(TColStd_HSequenceOfHAsciiString) names = select->Names();
      Handle(Express_HSequenceOfItem) items = select->Items();
      for ( Standard_Integer i=1; i <= names->Length(); i++ ) {
	Handle(Express_Item) it = Item ( names->Value(i) );
	// if select refers to another select, expand it
	if ( it->IsKind(STANDARD_TYPE(Express_Select)) ) {
	  std::cout << "Info: SELECT " << item->Name()->ToCString() << " refers to another SELECT " << 
	               it->Name()->ToCString() << "; expanded" << std::endl;
	  const Handle(Express_Select) sel = Handle(Express_Select)::DownCast ( it );
          Standard_Integer j = 1;
	  for ( ; j <= sel->Names()->Length(); j++ )
	    names->InsertBefore ( i+j-1, sel->Names()->Value(j) );
	  names->Remove ( i+j-1 );
	  i--;
	  continue;
	}
	items->Append ( it );
      }
    }
    else if ( item->IsKind(STANDARD_TYPE(Express_Entity)) ) {
      const Handle(Express_Entity) ent = Handle(Express_Entity)::DownCast ( item );
      Handle(TColStd_HSequenceOfHAsciiString) names = ent->SuperTypes();
      Handle(Express_HSequenceOfEntity) inh = ent->Inherit();
      for ( Standard_Integer i=1; i <= names->Length(); i++ ) {
	Handle(Express_Entity) subent = Handle(Express_Entity)::DownCast ( Item ( names->Value(i) ) );
	if ( ! subent.IsNull() ) inh->Append ( subent );
	else 
        {
          std::cout << "Error in " << item->Name()->ToCString() << ": supertype " << 
                       names->Value(i)->ToCString() << " is not an ENTITY; ignored" << std::endl;
        }
      }
      const Handle(Express_HSequenceOfField) fields = ent->Fields();
      for (Standard_Integer i=1; i <= fields->Length(); i++ )
	PrepareType ( fields->Value(i)->Type() );
    }
  }

}

//=======================================================================
//function : PrepareType
//purpose  : 
//=======================================================================

void Express_Schema::PrepareType (const Handle(Express_Type) &type) const
{
  if ( type->IsKind(STANDARD_TYPE(Express_NamedType)) ) {
    Handle(Express_NamedType) named = Handle(Express_NamedType)::DownCast ( type );
    named->SetItem ( Item ( named->Name() ) );
  }
  else if ( type->IsKind(STANDARD_TYPE(Express_ComplexType)) ) {
    Handle(Express_ComplexType) complex = Handle(Express_ComplexType)::DownCast ( type );
    PrepareType ( complex->Type() );
  }
}
