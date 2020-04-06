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

#ifndef _Express_Alias_HeaderFile
#define _Express_Alias_HeaderFile

#include <Standard_Type.hxx>
#include <Express_Item.hxx>
#include <Express_Type.hxx>

class TCollection_HAsciiString;


//! Implements TYPE = type (alias) item of the EXPRESS
//! schema, with interface for deferred Item class.
class Express_Alias : public Express_Item
{

public:

  
  //! Create ALIAS item and initialize it
  Standard_EXPORT Express_Alias(const Standard_CString name, const Handle(Express_Type)& type);
  
  //! Returns aliased type
  Standard_EXPORT  const  Handle(Express_Type)& Type()  const;
  
  //! Returns name of aliased type
  Standard_EXPORT virtual   Handle(TCollection_HAsciiString) CPPName()  const;
  
  //! Create HXX/CXX files from item
  Standard_EXPORT virtual   Standard_Boolean GenerateClass()  const;




  DEFINE_STANDARD_RTTIEXT(Express_Alias,Express_Item)

protected:




private: 


  Handle(Express_Type) myType;


};







#endif // _Express_Alias_HeaderFile
