// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/ShapeView_Tools.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <AIS_Shape.hxx>

#include <TopExp.hxx>

// =======================================================================
// function : ReadShape
// purpose :
// =======================================================================
TopoDS_Shape ShapeView_Tools::ReadShape (const TCollection_AsciiString& theFileName)
{
  TopoDS_Shape aShape;

  BRep_Builder aBuilder;
  BRepTools::Read (aShape, theFileName.ToCString(), aBuilder);
  return aShape;
}

// =======================================================================
// function : IsPossibleToExplode
// purpose :
// =======================================================================
Standard_Boolean ShapeView_Tools::IsPossibleToExplode (const TopoDS_Shape& theShape,
  NCollection_List<TopAbs_ShapeEnum>& theExplodeTypes)
{
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();

  TopTools_IndexedMapOfShape aSubShapes;
  for (int aType = aShapeType + 1; aType <= TopAbs_SHAPE; aType++)
  {
    aSubShapes.Clear();
    TopExp::MapShapes(theShape, (TopAbs_ShapeEnum)aType, aSubShapes);
    if (aSubShapes.Extent() > 0)
      theExplodeTypes.Append((TopAbs_ShapeEnum)aType);
  }

  return theExplodeTypes.Size() > 0;
}
