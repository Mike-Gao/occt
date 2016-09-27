// Created on: 1996-09-04
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Transfer_DataInfo_HeaderFile
#define _Transfer_DataInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Type.hxx>

//! Gives informations on an object
//! Used as template to instantiate Mapper and SimpleBinder
//! This class is for Transient
class Transfer_DataInfo 
{
 public:
  
  //! Returns Type Name (string)
  //! Allows to name type of non-handled objects
  static Standard_CString TypeName (const Handle(Standard_Transient)& ent) { return ent->DynamicType()->Name(); }
};

#endif // _Transfer_DataInfo_HeaderFile
