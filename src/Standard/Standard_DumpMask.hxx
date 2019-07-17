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

#ifndef _Standard_DumpMask_HeaderFile
#define _Standard_DumpMask_HeaderFile


//! Sets how much information should be dumped in Dump of the object
enum Standard_DumpMask
{
  Standard_DumpMask_Empty        = 0x0000, //! empty output
  Standard_DumpMask_Fields       = 0x0001, //! only class fields value
  Standard_DumpMask_Methods      = 0x0002, //! only methods result
  Standard_DumpMask_ClassName    = 0x0004, //! the first row of Dump has class name
  Standard_DumpMask_SingleValue  = 0x0008, //! dump is combined in one value
  Standard_DumpMask_Whole = Standard_DumpMask_Fields | Standard_DumpMask_Methods,
  Standard_DumpMask_WholeAndClassName = Standard_DumpMask_Whole | Standard_DumpMask_ClassName
};

#endif // _Standard_DumpMask_HeaderFile
