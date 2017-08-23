// Created on: 1999-12-08
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999 Matra Datavision
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

#ifndef _QANewDBRepNaming_HeaderFile
#define _QANewDBRepNaming_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! To test topological naming
class QANewDBRepNaming 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void AllCommands (Draw_Interpretor& Di);
  
  Standard_EXPORT static void PrimitiveCommands (Draw_Interpretor& DI);
  
  Standard_EXPORT static void FeatureCommands (Draw_Interpretor& DI);




protected:





private:





};







#endif // _QANewDBRepNaming_HeaderFile