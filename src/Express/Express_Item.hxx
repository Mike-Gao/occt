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

#ifndef _Express_Item_HeaderFile
#define _Express_Item_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
class TCollection_HAsciiString;


//! Base class for items of the schema. Stores a name of the class,
//! package name and flag used to mark items for generation.
//! Provides interface for writing generated class definitions to CDL
//! and CXX files.
class Express_Item : public Standard_Transient
{

public:

  
  //! Returns item name
  Standard_EXPORT  const  Handle(TCollection_HAsciiString)& Name()  const;
  
  //! Returns (generated) name for the item in CXX-style (Package_Class)
  Standard_EXPORT virtual   Handle(TCollection_HAsciiString) CPPName()  const;
  
  //! Returns (generated) name for the item in CDL-style (Class from Package)
  Standard_EXPORT virtual   Handle(TCollection_HAsciiString) CDLName()  const;
  
  //! Returns package name
  //! If not defined, returns NULL if auto is False (default)
  //! or "StepStep" if auto is True
  Standard_EXPORT   Handle(TCollection_HAsciiString) GetPackageName (const Standard_Boolean Auto = Standard_False)  const;
  
  //! Sets package name
  Standard_EXPORT   void SetPackageName (const Handle(TCollection_HAsciiString)& pack) ;
  
  //! Returns True if item is marked for generation
  Standard_EXPORT   Standard_Boolean GetMark()  const;
  
  //! Change generation mark flag
  Standard_EXPORT   void SetMark (const Standard_Boolean mark) ;
  
  //! General interface for creating CDL/CXX files from item
  Standard_EXPORT virtual   Standard_Boolean GenerateClass()  const = 0;
  
  //! Checks that item is marked for generation and if yes,
  //! generate it by calling GenerateClass. But firstly define
  //! PackageName to "StepStep" if not yet defined and drop Mark flag.
  Standard_EXPORT   Standard_Boolean Generate() ;
  
  //! Declares item as used by other item being generated
  //! Checks that item has package defined. If not, package is defined
  //! by pack argument (supposing that it is package of item that uses
  //! current one) and Mark flag is set. Then, if defer is False,
  //! calls Generate().
  Standard_EXPORT   Standard_Boolean Use (const Handle(TCollection_HAsciiString)& pack, const Standard_Boolean defer = Standard_False) ;
  
  //! Set category for item
  Standard_EXPORT   void SetCategory (const Handle(TCollection_HAsciiString)& categ) ;
  
  //! Get item category
  Standard_EXPORT   Handle(TCollection_HAsciiString) Category()  const;
  
  //! Set short name for item
  Standard_EXPORT   void SetShortName (const Handle(TCollection_HAsciiString)& shname) ;
  
  //! Get item short name
  Standard_EXPORT   Handle(TCollection_HAsciiString) ShortName()  const;
  
  //! Set flag for presence of method Check in the class
  Standard_EXPORT   void SetCheckFlag (const Standard_Boolean checkFlag) ;
  
  //! Get flag resposible for  presence of method Check in the class
  Standard_EXPORT   Standard_Boolean CheckFlag()  const;
  
  //! Set flag for presence of method FillShared in the class
  Standard_EXPORT   void SetFillSharedFlag (const Standard_Boolean fillsharedFlag) ;
  
  //! Get flag resposible for  presence of method FillShared in the class
  Standard_EXPORT   Standard_Boolean FillSharedFlag()  const;




  DEFINE_STANDARD_RTTIEXT(Express_Item,Standard_Transient)

protected:

  
  //! Creates object and initialises fields PackageName and
  //! CreateFlag by 0
  Standard_EXPORT Express_Item(const Standard_CString name);
  
  //! Creates object and initialises fields PackageName and
  //! CreateFlag by 0
  Standard_EXPORT Express_Item(const Handle(TCollection_HAsciiString)& name);



private: 


  Handle(TCollection_HAsciiString) myName;
  Handle(TCollection_HAsciiString) myPack;
  Standard_Boolean myMark;
  Handle(TCollection_HAsciiString) myShortName;
  Handle(TCollection_HAsciiString) myCategory;
  Standard_Boolean myhasCheck;
  Standard_Boolean myhasFillShared;


};







#endif // _Express_Item_HeaderFile
