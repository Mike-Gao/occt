// Created on: 2019-04-15
// Created by: Natalia ERMOLAEVA
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

#ifndef Convert_TransientShape_H
#define Convert_TransientShape_H

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <TopoDS_Shape.hxx>

DEFINE_STANDARD_HANDLE (Convert_TransientShape, Standard_Transient)

//! \class Convert_TransientShape
//! \brief An interface to create custom panes by transient object name.
class Convert_TransientShape : public Standard_Transient
{
public:

  //! Constructor
  Convert_TransientShape (const TopoDS_Shape& theShape) { SetShape (theShape); }

  //! Destructor
  virtual ~Convert_TransientShape() {}

  //! Fills current shape
  void SetShape (const TopoDS_Shape& theShape) { myShape = theShape; }

  //! Returns current shape
  const TopoDS_Shape GetShape() const { return myShape; }

  DEFINE_STANDARD_RTTIEXT (Convert_TransientShape, Standard_Transient)

private:
  TopoDS_Shape myShape;
};

#endif 
