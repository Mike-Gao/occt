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

#ifndef _Express_NamedType_HeaderFile
#define _Express_NamedType_HeaderFile

#include <Standard_Type.hxx>

#include <Express_Type.hxx>

class TCollection_HAsciiString;
class Express_Item;


//! Base class for complex types (ARRAY, LIST, BAG, SET)
//! in EXPRESS schema
//! Stores type of elements and
class Express_NamedType : public Express_Type
{

public:

  
  //! Creates an object and initializes by name
  Standard_EXPORT Express_NamedType(const Standard_CString name);
  
  //! Creates an object and initializes by name
  Standard_EXPORT Express_NamedType(const Handle(TCollection_HAsciiString)& name);
  
  //! Returns name of type (item in schema)
  Standard_EXPORT  const  Handle(TCollection_HAsciiString)& Name()  const;
  
  //! Returns handle to referred item in schema
  Standard_EXPORT  const  Handle(Express_Item)& Item()  const;
  
  //! Sets handle to referred item in schema
  Standard_EXPORT   void SetItem (const Handle(Express_Item)& it) ;
  
  //! Returns CPP-style name of the type
  Standard_EXPORT virtual   Handle(TCollection_HAsciiString) CPPName()  const;
  
  //! Return True if type is defined in package Standard
  Standard_EXPORT virtual   Standard_Boolean IsStandard()  const;
  
  //! Return True if type is simple (not a class)
  Standard_EXPORT virtual   Standard_Boolean IsSimple()  const;
  
  //! Return True if type is inherited from Transient
  Standard_EXPORT virtual   Standard_Boolean IsHandle()  const;
  
  //! Declares type as used by some item being generated.
  //! Calls Use() for referred item (found by name).
  Standard_EXPORT virtual   Standard_Boolean Use (const Handle(TCollection_HAsciiString)& pack, const Standard_Boolean defer = Standard_False)  const;




  DEFINE_STANDARD_RTTIEXT(Express_NamedType,Express_Type)

protected:




private: 


  Handle(TCollection_HAsciiString) myName;
  Handle(Express_Item) myItem;


};







#endif // _Express_NamedType_HeaderFile
