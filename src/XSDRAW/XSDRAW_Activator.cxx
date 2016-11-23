// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Dico_DictionaryOfInteger.hxx>
#include <Dico_IteratorOfDictionaryOfInteger.hxx>
#include <XSDRAW_Activator.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfTransient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSDRAW_Activator,MMgt_TShared)

static Handle(Dico_DictionaryOfInteger) thedico; // = new Dico_DictionaryOfInteger;
static TColStd_SequenceOfInteger   thenums, themodes;
static TColStd_SequenceOfTransient theacts;


    void XSDRAW_Activator::Adding
  (const Handle(XSDRAW_Activator)& actor,
   const Standard_Integer number,
   const Standard_CString command,
   const Standard_Integer mode)
{
  Standard_Boolean deja;
  if (thedico.IsNull()) thedico = new Dico_DictionaryOfInteger;
  Standard_Integer& num = thedico->NewItem(command,deja,Standard_True);
  if (deja) {
#ifdef OCCT_DEBUG
    cout<<"****  XSTEP commands, name conflict on "<<command<<" first defined remains  ****"<<endl;
//    Standard_DomainError::Raise("XSDRAW_Activator : Add");
#endif
  }
  num = thenums.Length() + 1;
  thenums.Append(number);
  theacts.Append(actor);
  themodes.Append(mode);
}

    void XSDRAW_Activator::Add
  (const Standard_Integer number, const Standard_CString command) const
      {  Adding (this,number,command,0);  }

    void XSDRAW_Activator::AddSet
  (const Standard_Integer number, const Standard_CString command) const
      {  Adding (this,number,command,1);  }

    void XSDRAW_Activator::Remove (const Standard_CString command)
      {  thedico->RemoveItem(command);  }

    Standard_Boolean XSDRAW_Activator::Select
  (const Standard_CString command, Standard_Integer& number,
   Handle(XSDRAW_Activator)& actor)
{
  Standard_Integer num;
  if (!thedico->GetItem(command,num,Standard_False)) return Standard_False;
  number = thenums(num);
  actor = Handle(XSDRAW_Activator)::DownCast(theacts(num));
  return Standard_True;
}

    Standard_Integer XSDRAW_Activator::Mode
  (const Standard_CString command)
{
  Standard_Integer num;
  if (!thedico->GetItem(command,num,Standard_False)) return -1;
  return themodes(num);
}


    Handle(TColStd_HSequenceOfAsciiString) XSDRAW_Activator::Commands
  (const Standard_Integer mode, const Standard_CString command)
{
  Standard_Integer num;
  Dico_IteratorOfDictionaryOfInteger iter (thedico,command);
  Handle(TColStd_HSequenceOfAsciiString) list =
    new  TColStd_HSequenceOfAsciiString();
  for (iter.Start(); iter.More(); iter.Next()) {
    if (mode < 0) {
      DeclareAndCast(XSDRAW_Activator,acti,theacts(iter.Value()));
      if (acti.IsNull()) continue;
      if (command[0] == '\0' || !strcmp(command,acti->Group()) )
	list->Append(iter.Name());
    } else {
      num = iter.Value();
      if (themodes(num) == mode) list->Append(iter.Name());
    }
  }
  return list;
}


    XSDRAW_Activator::XSDRAW_Activator ()
    : thegroup ("XSTEP")    {  }

    void  XSDRAW_Activator::SetForGroup
  (const Standard_CString group, const Standard_CString file)
    {  thegroup.Clear();  thegroup.AssignCat (group);
       thefile.Clear();   thefile.AssignCat  (file);   }

    Standard_CString  XSDRAW_Activator::Group () const
    {  return thegroup.ToCString();  }

    Standard_CString  XSDRAW_Activator::File  () const
    {  return thefile.ToCString();   }
