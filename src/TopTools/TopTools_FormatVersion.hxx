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

#ifndef _TopToolsFormatVersion_HeaderFile
#define _TopToolsFormatVersion_HeaderFile

#include <Standard_Typedef.hxx>

//! Defined TopTools format version
enum TopTools_FormatVersion
{
  TOP_TOOLS_DEFAULT_VERSION = 0, //!< Default version
  TOP_TOOLS_VERSION_1 = 1,       //!< Does not write CurveOnSurface UV Points into 
                                 //!  the file. On reading calls Check() method.
  TOP_TOOLS_VERSION_2 = 2,       //!< Stores CurveOnSurface UV Points. On reading format is 
                                 //!  recognized from Version string.
  TOP_TOOLS_VERSION_3 = 3        //!< Stores per-vertex normal information in case
                                 //!  of triangulation-only Faces, because
                                 //!  no analytical geometry to restore normals
};

#endif
