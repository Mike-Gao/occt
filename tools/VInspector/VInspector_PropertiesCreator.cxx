// Created on: 2019-04-28
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

#include <inspector/VInspector_PropertiesCreator.hxx>
#include <inspector/VInspectorPaneAIS_ColoredShape.hxx>
#include <inspector/ViewControl_PropertiesStream.hxx>

#include <Standard_OStream.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VInspector_PropertiesCreator, TreeModel_ItemPropertiesCreator)

// =======================================================================
// function : GetProperties
// purpose :
// =======================================================================
TreeModel_ItemProperties* VInspector_PropertiesCreator::GetProperties (const TreeModel_ItemBasePtr& theItem)
{
  Standard_SStream aSStream;
  if (theItem->Dump (aSStream))
    return new ViewControl_PropertiesStream (theItem);

  return NULL;
}

