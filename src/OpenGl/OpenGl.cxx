// Created on: 2019-03-16
// Created by: Natalia Ermolaeva
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <OpenGl.hxx>

#include <TCollection_AsciiString.hxx>

namespace
{
  static Standard_CString OpenGl_Table_PrintLayerFilter[4] =
  {
    "ALL", "UPPER", "BOTTOM", "DEFAULT"
  };
}

//=======================================================================
//function : LayerFilterToString
//purpose  :
//=======================================================================
Standard_CString OpenGl::LayerFilterToString (const OpenGl_LayerFilter theType)
{
  return OpenGl_Table_PrintLayerFilter[theType];
}

//=======================================================================
//function : LayerFilterFromString
//purpose  :
//=======================================================================
Standard_Boolean OpenGl::LayerFilterFromString (const Standard_CString theTypeString,
                                                OpenGl_LayerFilter& theType)
{
  TCollection_AsciiString aName (theTypeString);
  aName.UpperCase();
  for (Standard_Integer aTypeIter = 0; aTypeIter <= OpenGl_LF_Default; ++aTypeIter)
  {
    Standard_CString aTypeName = OpenGl_Table_PrintLayerFilter[aTypeIter];
    if (aName == aTypeName)
    {
      theType = OpenGl_LayerFilter (aTypeIter);
      return Standard_True;
    }
  }
  return Standard_False;
}
