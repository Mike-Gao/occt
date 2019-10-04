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

#include <inspector/VInspector_ItemContainer.hxx>

#include <inspector/VInspector_ItemContainerAPI.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemContainer::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (Column() != 0 || (theItemRole != Qt::DisplayRole && theItemRole != Qt::ToolTipRole))
    return QVariant();

  VInspector_ItemContainerAPI* aContainerItem = dynamic_cast<VInspector_ItemContainerAPI*>(Parent().data());

  if (!aContainerItem)
    return "Empty item container";

  return aContainerItem->GetContainerValue (Row(), theItemRole);
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemContainer::initRowCount() const
{
  VInspector_ItemContainerAPI* aContainerItem = dynamic_cast<VInspector_ItemContainerAPI*>(Parent().data());

  if (!aContainerItem)
    return 0;

  return aContainerItem->GetContainerRowCount (Row());
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemContainer::createChild (int theRow, int theColumn)
{
  VInspector_ItemContainerAPI* aContainerItem = dynamic_cast<VInspector_ItemContainerAPI*>(Parent().data());

  if (!aContainerItem)
    return TreeModel_ItemBasePtr();

  return aContainerItem->CreateContainerChild (currentItem(), Row(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemContainer::Init()
{
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemContainer::Reset()
{
  VInspector_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemContainer::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemContainer*> (this)->Init();
}
