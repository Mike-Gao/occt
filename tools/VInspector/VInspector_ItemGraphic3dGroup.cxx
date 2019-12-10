// Created on: 2018-08-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <inspector/VInspector_ItemGraphic3dGroup.hxx>

#include <inspector/VInspector_ItemGraphic3dCStructure.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <OpenGl_Group.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dGroup::Init()
{
  VInspector_ItemGraphic3dCStructurePtr aParentItem = itemDynamicCast<VInspector_ItemGraphic3dCStructure>(Parent());
  myGroup = aParentItem->GetGroup (Row());

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dGroup::Reset()
{
  VInspector_ItemBase::Reset();
  myGroup = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dGroup::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemGraphic3dGroup*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dGroup::initRowCount() const
{
  if (Column() != 0)
    return 0;

  Handle(Graphic3d_Group) aGroup = GetGroup();
  Handle(OpenGl_Group) anOpenGlGroup = Handle(OpenGl_Group)::DownCast(aGroup);
  if (anOpenGlGroup.IsNull())
    return 0;

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemGraphic3dGroup::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Graphic3d_Group) aGroup = GetGroup();
  if (aGroup.IsNull())
    return Column() == 0 ? "Empty group" : "";

  switch (Column())
  {
    case 0: return theItemRole == Qt::DisplayRole ? aGroup->DynamicType()->Name()
                                                  : STANDARD_TYPE (Graphic3d_Group)->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dGroup::initStream (Standard_OStream& theOStream) const
{
  Handle(Graphic3d_Group) aGroup = GetGroup();
  if (aGroup.IsNull())
    return;

  aGroup->DumpJson (theOStream);
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemGraphic3dGroup::createChild (int, int)
{
  return TreeModel_ItemBasePtr();
}
