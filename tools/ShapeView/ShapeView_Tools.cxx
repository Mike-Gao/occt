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
#include <inspector/ShapeView_ItemShape.hxx>


#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

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

  if (!theExplodeTypes.Contains (aShapeType))
    theExplodeTypes.Append(aShapeType);

  if (theExplodeTypes.Extent() == TopAbs_SHAPE + 1) // all types are collected, stop
    return Standard_True;

  TopoDS_Iterator aSubShapeIt (theShape);
  for (int aCurrentIndex = 0; aSubShapeIt.More(); aSubShapeIt.Next(), aCurrentIndex++)
  {
    if (IsPossibleToExplode (aSubShapeIt.Value(), theExplodeTypes))
      return Standard_True;
  }
  return Standard_False;
}

// =======================================================================
// function : GetPropertyTableValues
// purpose :
// =======================================================================
void ShapeView_Tools::GetPropertyTableValues (const TreeModel_ItemBasePtr& theItem,
                                              QList<ViewControl_TableModelValues*>& theTableValues)
{
  TreeModel_ItemBasePtr anItem = theItem;
  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(anItem);
  if (!aShapeItem)
    return;

  Handle(TreeModel_ItemProperties) anItemProperties = aShapeItem->GetProperties();
  if (anItemProperties.IsNull())
    return;

  ViewControl_TableModelValues* aTableValues = new ViewControl_TableModelValues();
  aTableValues->SetProperties (anItemProperties);
  theTableValues.append (aTableValues);
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
QString ShapeView_Tools::ToString (const Standard_Boolean& theValue)
{
  return theValue ? "1" : "0";
}

// =======================================================================
// function : ToName
// purpose :
// =======================================================================
QString ShapeView_Tools::ToName (const GeomAbs_Shape& theType)
{
  switch (theType)
  {
    case GeomAbs_C0: return "GeomAbs_C0";
    case GeomAbs_G1: return "GeomAbs_G1";
    case GeomAbs_C1: return "GeomAbs_C1";
    case GeomAbs_G2: return "GeomAbs_G2";
    case GeomAbs_C2: return "GeomAbs_C2";
    case GeomAbs_C3: return "GeomAbs_C3";
    case GeomAbs_CN: return "GeomAbs_CN";
    default: break;
  }
  return QString();
}

// =======================================================================
// function : GetShapeGlobalPropertiesCount
// purpose :
// =======================================================================
int ShapeView_Tools::GetShapeGlobalPropertiesCount()
{
  return 6;
}

// =======================================================================
// function : GetShapeGlobalProperties
// purpose :
// =======================================================================
QVariant ShapeView_Tools::GetShapeGlobalProperties (const TopoDS_Shape& theShape,
                                                    const int theRow,
                                                    const int theColumn)
{
  bool isFirstColumn = theColumn == 0;

  switch (theRow)
  {
    case 0: return isFirstColumn ? "Checked" : ToString (theShape.Checked());
    case 1: return isFirstColumn ? "Closed" : ToString (theShape.Closed());
    case 2: return isFirstColumn ? "Infinite" : ToString (theShape.Infinite());
    case 3: return isFirstColumn ? "Locked" : ToString (theShape.Locked());
    case 4: return isFirstColumn ? "Modified" : ToString (theShape.Modified());
    case 5: return isFirstColumn ? "Orientable" : ToString (theShape.Orientable());
  }
  return QVariant();
}