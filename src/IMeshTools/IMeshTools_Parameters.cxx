// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <IMeshTools_Parameters.hxx>
#include <Standard_Dump.hxx>

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
void IMeshTools_Parameters::DumpJson (Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Angle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Deflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, AngleInterior)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, DeflectionInterior)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, MinSize)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, InParallel)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Relative)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, InternalVerticesMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, ControlSurfaceDeflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, CleanModel)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, AdjustMinSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, ForceFaceDeflection)
}
