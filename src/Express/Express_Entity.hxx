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

#ifndef _Express_Entity_HeaderFile
#define _Express_Entity_HeaderFile

#include <Standard_Type.hxx>

#include <Express_Item.hxx>
#include <Standard_OStream.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <NCollection_DataMap.hxx>

class Express_HSequenceOfEntity;
class Express_HSequenceOfField;
class Dico_DictionaryOfInteger;


//! Implements ENTITY item of the EXPRESS
//! schema, with interface for deferred Item class.
class Express_Entity : public Express_Item
{

public:

  
  //! Create ENTITY item and initialize it
  //! flags hasCheck and hasFillShared mark if generated class has
  //! methods Check and FillShared cprrespondingly.
  Standard_EXPORT Express_Entity(const Standard_CString name, const Handle(TColStd_HSequenceOfHAsciiString)& inherit, const Handle(Express_HSequenceOfField)& flds);
  
  //! Returns sequence of inherited classes (names)
  Standard_EXPORT  const  Handle(TColStd_HSequenceOfHAsciiString)& SuperTypes()  const;
  
  //! Returns sequence of inherited items
  Standard_EXPORT  const  Handle(Express_HSequenceOfEntity)& Inherit()  const;
  
  //! Returns sequence of fields
  Standard_EXPORT  const  Handle(Express_HSequenceOfField)& Fields()  const;
  
  //! Returns number of fields (only own fields if inherited is False
  //! and including fields of all supertypes if it is True)
  Standard_EXPORT   Standard_Integer NbFields (const Standard_Boolean inherited = Standard_False)  const;
  
  //! Sets abstruct flag for entity;
  Standard_EXPORT   void SetAbstractFlag (const Standard_Boolean isAbstract) ;
  
  //! Returns abstract flag.
  Standard_EXPORT   Standard_Boolean AbstractFlag()  const;




  DEFINE_STANDARD_RTTIEXT(Express_Entity,Express_Item)

protected:




private: 

  typedef NCollection_DataMap<TCollection_AsciiString,Standard_Integer,TCollection_AsciiString> DataMapOfStringInteger;
  
  //! Create CDL/CXX files from item
  Standard_EXPORT virtual   Standard_Boolean GenerateClass()  const;
  
  //! Writes uses section of CDL
  Standard_EXPORT   Standard_Boolean WriteUses (Standard_OStream& os, const Standard_Boolean start, DataMapOfStringInteger& dict)  const;
  
  //! Writes #include declarations for RW class
  Standard_EXPORT   void WriteRWInclude (Standard_OStream& os, DataMapOfStringInteger& dict)  const;
  
  //! Writes code for reading all fields
  Standard_EXPORT   Standard_Integer WriteRWReadCode (Standard_OStream& os, const Standard_Integer start, const Standard_Integer own)  const;
  
  //! Writes code for writing all fields
  Standard_EXPORT   Standard_Integer WriteRWWriteCode (Standard_OStream& os, const Standard_Integer start, const Standard_Integer own)  const;
  
  //! Writes code for adding shared entities to the graph
  Standard_EXPORT   Standard_Integer WriteRWShareCode (Standard_OStream& os, const Standard_Integer start, const Standard_Integer own)  const;
  
  //! Writes arguments and code for method Init()
  //! Mode identifyes what code is being written:
  //! 0 - CDL declaration
  //! 1 - CXX declaration
  //! 2 - call (argument list)
  //! 3 - implementation
  Standard_EXPORT   Standard_Integer MakeInit (Standard_OStream& os, const Standard_Integer shift, const Standard_Integer own, const Standard_Integer mode)  const;

  Handle(TColStd_HSequenceOfHAsciiString) mySupers;
  Handle(Express_HSequenceOfEntity) myInherit;
  Handle(Express_HSequenceOfField) myFields;
  Standard_Boolean myisAbstract;
};







#endif // _Express_Entity_HeaderFile
