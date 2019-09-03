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

#ifndef ShapeView_Tools_H
#define ShapeView_Tools_H

#include <Standard.hxx>

#include <inspector/TreeModel_ItemBase.hxx>

#include <NCollection_List.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>

#include <inspector/ViewControl_TableModelValues.hxx>

#include <GeomAbs_Shape.hxx>
#include <Standard_WarningsDisable.hxx>
#include <QList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

//! \class ShapeView_Tools
//! It gives auxiliary methods for TopoDS_Shape manipulation
class ShapeView_Tools
{
public:

  //! Read Shape using BREP reader
  //! \param theFileName a file name
  //! \return shape or NULL
  Standard_EXPORT static TopoDS_Shape ReadShape (const TCollection_AsciiString& theFileName);

  //! Checks whether it is possible to expode the shape. The search is recursive untill all types are collected.
  //! \param theShape source shape object
  //! \param theExplodeTypes container of possible shape types to be exploded
  //! \return true if explode is finished, all types are collected.
  Standard_EXPORT static Standard_Boolean IsPossibleToExplode(const TopoDS_Shape& theShape,
    NCollection_List<TopAbs_ShapeEnum>& theExplodeTypes);

  //! Fills container of table values
  //! \param theAlert a message alert
  //! \param theTableValue container of values
  Standard_EXPORT static void GetPropertyTableValues (const TreeModel_ItemBasePtr& theItem,
                                                      QList<ViewControl_TableModelValues*>& theTableValues);

  //! Returns count of gloal properties (6)
  //! \return count value
  Standard_EXPORT static int GetShapeGlobalPropertiesCount();

  //! Returns shape global property by row/column
  //! \param theShape investigated shape
  //! \param theRow row index in [0, 6]
  //! \param theColumn column index in [0, 1]
  Standard_EXPORT static QVariant GetShapeGlobalProperties (const TopoDS_Shape& theShape,
                                                            const int theRow,
                                                            const int theColumn);

  //! Returns text presentation of boolean value
  Standard_EXPORT static QString ToString (const Standard_Boolean& theValue);

  //! Returns shape type name
  Standard_EXPORT static QString ToName (const GeomAbs_Shape& theType);

};

#endif
