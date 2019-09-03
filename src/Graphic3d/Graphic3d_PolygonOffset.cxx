// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Graphic3d_PolygonOffset.hxx>

#include <Aspect.hxx>
#include <TCollection.hxx>

const TCollection_AsciiString Graphic3d_PolygonOffset_ClassName = "Graphic3d_PolygonOffset";

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Graphic3d_PolygonOffset::Dump (Standard_OStream& OS) const
{
  DUMP_START_KEY (OS, Graphic3d_PolygonOffset_ClassName);

  DUMP_VALUES (OS, "Mode", Aspect::PolygonOffsetModeToString (Mode));
  DUMP_VALUES (OS, "Factor", Factor);
  DUMP_VALUES (OS, "Units", Units);

  DUMP_STOP_KEY (OS, Graphic3d_PolygonOffset_ClassName);
}
