// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
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

#include <inspector/Convert_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

// =======================================================================
// function : ReadShape
// purpose :
// =======================================================================
TopoDS_Shape Convert_Tools::ReadShape (const TCollection_AsciiString& theFileName)
{
  TopoDS_Shape aShape;

  BRep_Builder aBuilder;
  BRepTools::Read (aShape, theFileName.ToCString(), aBuilder);
  return aShape;
}

//=======================================================================
//function : ConvertStreamToPresentations
//purpose  :
//=======================================================================
void Convert_Tools::ConvertStreamToPresentations (const Standard_SStream&,
                                                  const Standard_Integer,
                                                  const Standard_Integer,
                                                  NCollection_List<Handle(Standard_Transient)>&)
{

  Select3D_BndBox3d aSelectBndBox;
  if (aSelectBndBox.InitFromJson (theSStream, aStartPos))
  {
    TopoDS_Shape aShape;

    gp_Pnt aPntMin = gp_Pnt (aSelectBndBox.CornerMin().x(), aSelectBndBox.CornerMin().y(), aSelectBndBox.CornerMin().z());
    gp_Pnt aPntMax = gp_Pnt (aSelectBndBox.CornerMax().x(), aSelectBndBox.CornerMax().y(), aSelectBndBox.CornerMax().z());
    if (CreateBoxShape (aPntMin, aPntMax, aShape))
      thePresentations.Append (new Convert_TransientShape (aShape));
    return;
  }
}

//=======================================================================
//function : ConvertStreamToColor
//purpose  :
//=======================================================================
Standard_Boolean Convert_Tools::ConvertStreamToColor (const Standard_SStream&,
                                                      Quantity_Color&)
{
  return Standard_False;
}
