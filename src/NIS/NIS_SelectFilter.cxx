// Created on: 2007-08-20
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <NIS_SelectFilter.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_SelectFilter, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (NIS_SelectFilter, Standard_Transient)

//=======================================================================
//function : IsOk
//purpose  : 
//=======================================================================

Standard_Boolean NIS_SelectFilter::IsOk
                                (const NIS_InteractiveObject * theObject,
                                 const gp_Ax1&,
                                 const Standard_Real,
                                 const Event theEvent) const
{
  Standard_Boolean aResult(Standard_False);
  if (theEvent == Indefinite || (theEvent & myEventFlags) != 0)
    aResult = IsOk(theObject, theEvent);
  return aResult;
}

//=======================================================================
//function : ActsOn
//purpose  : 
//=======================================================================

Standard_Boolean NIS_SelectFilter::ActsOn (const Handle_Standard_Type&) const
{
  return Standard_True;
}

