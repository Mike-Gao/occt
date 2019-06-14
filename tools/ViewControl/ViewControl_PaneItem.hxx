// Created on: 2018-12-08
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

#ifndef ViewControl_PaneItem_H
#define ViewControl_PaneItem_H

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>

#include <TopoDS_Shape.hxx>

//! \class ViewControl_PaneItem
//! Container of pane item information

class ViewControl_PaneItem
{
public:

  //! Returns shape of the item or NULL
  //! \return shape instance
  virtual TopoDS_Shape GetShape() { return myShape; }

  //! Sets shape
  //! \param theShape shape instance
  void SetShape (const TopoDS_Shape& theShape) { myShape = theShape; }

  //! Returns object or NULL
  //! \return current object
  virtual Handle(Standard_Transient) GetObject() { return myObject; }

  //! Sets object
  //! \param theObject an object
  virtual void SetObject (Handle(Standard_Transient)& theObject) { myObject = theObject; }

protected:
  TopoDS_Shape myShape; //!< current shape
  Handle(Standard_Transient) myObject; //!< current object
};

#endif