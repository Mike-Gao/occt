// Created on: 2010-06-02
// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#include <Draw.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <Bnd_OBB.hxx>
#include <Bnd_Box.hxx>
#include <Draw_Box.hxx>

// This file defines global functions not declared in any public header,
// intended for use from debugger prompt (Command Window in Visual Studio)

//! Execute given DRAW command
Standard_EXPORT const char* Draw_Eval (const char *theCommandStr)
{
  if (theCommandStr == 0)
  {
    return "Error: null command string";
  }
  try {
    OCC_CATCH_SIGNALS
    Draw_Interpretor& aCommands = Draw::GetInterpretor();
    aCommands.Eval (theCommandStr);
    std::cout << aCommands.Result() << std::endl;
    return aCommands.Result();
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//=======================================================================
//function : DBRep_SetOBB
//purpose  : Draw OBB
//=======================================================================
Standard_EXPORT const char* Draw_SetOBB(const char* theNameStr, void* theBox)
{
  if (theNameStr == 0 || theBox == 0)
  {
    return "Error: name or box is null";
  }
  try {
    Bnd_OBB B = *(Bnd_OBB*)theBox;
    Handle(Draw_Box) DB = new Draw_Box (B, Draw_orange);
    Draw::Set (theNameStr, DB);
    return theNameStr;
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//=======================================================================
//function : DBRep_SetBox
//purpose  : Draw Box
//=======================================================================
Standard_EXPORT const char* Draw_SetBox(const char* theNameStr, void* theBox)
{
  if (theNameStr == 0 || theBox == 0)
  {
    return "Error: name or box is null";
  }
  try {
    Bnd_Box B = *(Bnd_Box*)theBox;
    Handle(Draw_Box) DB = new Draw_Box (B, Draw_orange);
    Draw::Set (theNameStr, DB);
    return theNameStr;
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}
