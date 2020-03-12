// Created on: 2020-03-19
// Created by: Irina KRYLOVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _XDEDRAW_Kinematics_HeaderFile
#define _XDEDRAW_Kinematics_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! Contains commands to work with GDTs
class XDEDRAW_Kinematics 
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void InitCommands (Draw_Interpretor& theCommands);
};

#endif // _XDEDRAW_Kinematics_HeaderFile
