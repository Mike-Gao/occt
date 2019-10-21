// Created on: 2019-02-25
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

#include <inspector/TreeModel_ItemProperties.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TreeModel_ItemProperties, Standard_Transient)

// =======================================================================
// function : GetTableFlags
// purpose :
// =======================================================================

Qt::ItemFlags TreeModel_ItemProperties::GetTableFlags (const int, const int theColumn) const
{
  Qt::ItemFlags aFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (theColumn == 1)
    aFlags = aFlags | Qt::ItemIsEditable;

  return aFlags;
}