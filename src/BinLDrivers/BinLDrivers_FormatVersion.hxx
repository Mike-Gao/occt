// Created on: 2020-03-06
// Created by: Andrey SURAVENKOV
// Copyright (c) 2002-2020 OPEN CASCADE SAS
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

#ifndef _BinLDriversFormatVersion_HeaderFile
#define _BinLDriversFormatVersion_HeaderFile

#include <Standard_Typedef.hxx>

enum class BinLDrivers_FormatVersion : Standard_Integer
{
  VERSION_2 = 2, // First supported version
  VERSION_3,     // Add Delta to numbers data, changes in ShapeSection
  VERSION_4,     // entry, ContextLabel for tree
  VERSION_5,     // Convert old format to new
  VERSION_6,     // Add location
  VERSION_7,     // Add orientation, type migration
  VERSION_8,     // Stop convert old format
  VERSION_9,     // Add GUIDs, ReadTOC changed
  VERSION_10,
  VERSION_11     // Add normals to Shape
};

#endif
