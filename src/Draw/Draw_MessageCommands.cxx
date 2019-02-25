// Created on: 2018-06-10
// Created by: Natalia Ermolaeva
// Copyright (c)  2017 OPEN CASCADE SAS
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
#include <Message.hxx>
#include <Message_Gravity.hxx>
#include <Message_Report.hxx>
#include <TCollection_AsciiString.hxx>

//==============================================================================
//function : ActivateReport
//purpose  :
//==============================================================================

static Standard_Integer ActivateReport (Draw_Interpretor&, Standard_Integer theArgsNb, const char** theArgVec)
{
  bool isActiveMode = true;
  bool isGravityDefined = false;
  Standard_Integer aGravity = -1;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg = theArgVec[anArgIt];
    TCollection_AsciiString anArgCase = anArg;
    anArgCase.LowerCase();
    if (anArgCase == "-mode")
    {
      if (anArgIt + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIt + 1], isActiveMode))
      {
        ++anArgIt;
      }
    }
    else if (anArgCase == "-gravity")
    {
      if (anArgIt + 1 < theArgsNb)
      {
        Message_Gravity aGravityType;
        isGravityDefined = Message::GravityFromString (theArgVec[anArgIt + 1], aGravityType);
        aGravity = (Standard_Integer)aGravityType;
        ++anArgIt;
      }
    }
    else
    {
      std::cout << "Syntax error: unknown argument " << anArg << ".\n";
      return 1;
    }
  }

  Handle(Message_Report) aCurrentReport = Message_Report::CurrentReport (isActiveMode);
  if (!aCurrentReport.IsNull())
    aCurrentReport->SetActive(isActiveMode, aGravity);

  return 0;
}

//==============================================================================
//function : ClearReport
//purpose  :
//==============================================================================

static Standard_Integer ClearReport (Draw_Interpretor&, Standard_Integer, const char**)
{
  Handle(Message_Report) aCurrentReport = Message_Report::CurrentReport (Standard_False);
  if (!aCurrentReport.IsNull())
    aCurrentReport->Clear(); 

  return 0;
}

//=======================================================================
//function : PloadCommands
//purpose  : 
//=======================================================================

void Draw::MessageCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean Done = Standard_False;
  if (Done) return;
  Done = Standard_True;

  const char* g =  "DRAW Message Commands";
  
  theCommands.Add("activateReport" ,
                  "activateReport [-mode {-on|-off}=-on]"
                                 "[-gravity {trace|info|warning|alarm|fail}]"
                  "Activates default Message_Report object" ,
		  __FILE__, ActivateReport, g);

  theCommands.Add("clearReport" , "clearReport: Clears content of default Message_Report object" ,
		  __FILE__, ClearReport, g);
}
