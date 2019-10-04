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

#ifndef ViewControl_PaneCreator_H
#define ViewControl_PaneCreator_H

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

class ViewControl_Pane;

DEFINE_STANDARD_HANDLE (ViewControl_PaneCreator, Standard_Transient)

//! \class ViewControl_PaneCreator
//! \brief An interface to create custom panes by transient object name.
class ViewControl_PaneCreator : public Standard_Transient
{
public:

  //! Constructor
  ViewControl_PaneCreator() {}

  //! Destructor
  virtual ~ViewControl_PaneCreator() {}

  //! Creates pane for type
  //! \param theName a type name
  //! \return a pane if it can be created for this type or NULL
  virtual ViewControl_Pane* GetPane (const Standard_CString& theName) = 0;

  DEFINE_STANDARD_RTTIEXT (ViewControl_PaneCreator, Standard_Transient)
};

#endif 
