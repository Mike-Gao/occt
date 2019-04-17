// Created on: 2019-04-12
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

#ifndef TreeModel_ItemPropertiesCreator_H
#define TreeModel_ItemPropertiesCreator_H

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

class TreeModel_ItemProperties;

DEFINE_STANDARD_HANDLE (TreeModel_ItemPropertiesCreator, Standard_Transient)

//! \class TreeModel_ItemPropertiesCreator
//! \brief An interface to create custom panes by transient object name.
class TreeModel_ItemPropertiesCreator : public Standard_Transient
{
public:

  //! Constructor
  TreeModel_ItemPropertiesCreator() {}

  //! Destructor
  virtual ~TreeModel_ItemPropertiesCreator() {}

  //! Creates pane for type
  //! \param theName a type name
  //! \return a pane if it can be created for this type or NULL
  virtual TreeModel_ItemProperties* GetProperties (const TreeModel_ItemBasePtr& theItem) = 0;

  DEFINE_STANDARD_RTTIEXT (TreeModel_ItemPropertiesCreator, Standard_Transient)
};

#endif 
