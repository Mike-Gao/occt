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

#include <IMeshTools_Context.hxx>

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void IMeshTools_Context::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, IMeshData_Shape)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myModelBuilder.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myModel.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myEdgeDiscret.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myModelHealer.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myPreProcessor.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myFaceDiscret.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myPostProcessor.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myParameters)
}

