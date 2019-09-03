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

#include <inspector/TreeModel_ItemStream.hxx>

#include <inspector/TreeModel_ItemPropertiesStream.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant TreeModel_ItemStream::initValue (const int theItemRole) const
{
  QVariant aParentValue = TreeModel_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (Column() != 0)
    return QVariant();

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  TreeModel_ItemBasePtr aParentItem = Parent();
  if (!aParentItem)
    return QVariant();

  TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(aParentItem);
  if (!aStreamParent)
    return "Dump";

  Handle(TreeModel_ItemPropertiesStream) aStreamProperties = Handle(TreeModel_ItemPropertiesStream)::DownCast (aParentItem->GetProperties());
  if (aStreamProperties.IsNull())
    return QVariant();

  return aStreamProperties->GetChildKey (Row()).ToCString();
}

// =======================================================================
// function : GetStream
// purpose :
// =======================================================================
void TreeModel_ItemStream::GetStream (Standard_OStream& OS) const
{
  TreeModel_ItemBasePtr aParentItem = Parent();
  if (!aParentItem)
    return;

  Handle(TreeModel_ItemPropertiesStream) aStreamProperties = Handle(TreeModel_ItemPropertiesStream)::DownCast (aParentItem->GetProperties());
  if (aStreamProperties.IsNull())
    return;

  aStreamProperties->GetChildStream (Row(), OS);
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ItemStream::createChild (int, int)
{
  return TreeModel_ItemBasePtr();
}
