// Created on: 1991-03-20
// Created by: Remi Lequette
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <TopoDS_Shape.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TCollection.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_TShape.hxx>

//=======================================================================
// function : HashCode
// purpose  :
//=======================================================================
Standard_Integer TopoDS_Shape::HashCode (const Standard_Integer theUpperBound) const
{
  // PKV
  const Standard_Integer aHS = ::HashCode (myTShape.get(), theUpperBound);
  const Standard_Integer aHL = myLocation.HashCode (theUpperBound);
  return ::HashCode (aHS ^ aHL, theUpperBound);
}

const TCollection_AsciiString TopoDS_Shape_ClassName = "TopoDS_Shape";

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void TopoDS_Shape::Dump (Standard_OStream& OS, const Standard_Integer /*theMask*/) const
{
  DUMP_START_KEY (OS, TopoDS_Shape_ClassName);

  DUMP_VALUES (OS, "TShape", TCollection::GetPointerInfo (myTShape));
  DUMP_VALUES (OS, "Orientation", TopAbs::ShapeOrientationToString (myOrient));
  {
    Standard_SStream aTmpStream;
    myLocation.Dump (aTmpStream);
    DUMP_VALUES (OS, "Location", TCollection::ToDumpString (aTmpStream));
  }

  DUMP_STOP_KEY (OS, TopoDS_Shape_ClassName);
}
