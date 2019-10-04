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

#ifndef VInspector_ItemContainerAPI_H
#define VInspector_ItemContainerAPI_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

//! \class VInspector_ItemContainerAPI
class VInspector_ItemContainerAPI
{

public:
  //! Constructor
  VInspector_ItemContainerAPI () {}

  //! Destructor
  virtual ~VInspector_ItemContainerAPI() {};

  //! Returns number of item selected
  //! \return rows count
  virtual int GetContainerRowCount (const int theContainerRow) const = 0;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant GetContainerValue (const int theContainerRow, const int theItemRole) const = 0;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr CreateContainerChild (const TreeModel_ItemBasePtr& theParent, const int theContainerRow, int theRow, int theColumn) = 0;

};

#endif
