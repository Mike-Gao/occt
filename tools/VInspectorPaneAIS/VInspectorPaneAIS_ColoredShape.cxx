// Created on: 2019-04-14
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

#include <inspector/VInspectorPaneAIS_ColoredShape.hxx>
#include <inspector/VInspectorPaneAIS_ItemPrs3dDrawer.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <inspector/VInspector_Tools.hxx>

#include <AIS_ColoredShape.hxx>

// =======================================================================
// function : ChildItemCount
// purpose :
// =======================================================================
int VInspectorPaneAIS_ColoredShape::ChildItemCount() const
{
  Handle(AIS_ColoredShape) aPrs = GetPresentation();
  if (aPrs.IsNull())
    return 0;

  return aPrs->CustomAspectsMap().Size();
}

// =======================================================================
// function : CreateChildItem
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspectorPaneAIS_ColoredShape::CreateChildItem (int theRow, int theColumn) const
{
  Handle(AIS_ColoredShape) aPrs = GetPresentation();
  if (aPrs.IsNull())
    return TreeModel_ItemBasePtr();

  return VInspectorPaneAIS_ItemPrs3dDrawer::CreateItem (getItem(), theRow, theColumn);
}

// =======================================================================
// function : GetPresentation
// purpose :
// =======================================================================
Handle(AIS_ColoredShape) VInspectorPaneAIS_ColoredShape::GetPresentation() const
{
  return Handle(AIS_ColoredShape)::DownCast (getItem()->GetObject());
}
