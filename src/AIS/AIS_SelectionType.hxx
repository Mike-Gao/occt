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


#ifndef _AIS_SelectionType_HeaderFile
#define _AIS_SelectionType_HeaderFile


//! Sets selection schemes for interactive contexts.
//! It is possible to use combination of schemes.
enum AIS_SelectionType
{
  AIS_SelectionType_Select,
  AIS_SelectionType_SelectInRect,
  AIS_SelectionType_SelectInPoly,
  AIS_SelectionType_ShiftSelect,
  AIS_SelectionType_ShiftSelectInRect,
  AIS_SelectionType_ShiftSelectInPoly
};

#endif // _AIS_SelectionType_HeaderFile
