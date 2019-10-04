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

#include <inspector/VInspector_ItemGraphic3dTransformPers.hxx>
#include <inspector/VInspector_ItemFolderObject.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>

#include <inspector/VInspector_ItemPrs3dPresentation.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dTransformPers::Init()
{
  VInspector_ItemFolderObjectPtr aParentItem = itemDynamicCast<VInspector_ItemFolderObject>(Parent());
  if (aParentItem)
  {
   VInspector_ItemPresentableObjectPtr aParentPrsItem = itemDynamicCast<VInspector_ItemPresentableObject>(aParentItem->Parent());
    if (aParentPrsItem)
    {
      Handle(AIS_InteractiveObject) anObject = aParentPrsItem->GetInteractiveObject();
      myTransformPers = anObject->TransformPersistence();
    }
  }

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dTransformPers::Reset()
{
  VInspector_ItemBase::Reset();
  myTransformPers = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dTransformPers::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemGraphic3dTransformPers*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dTransformPers::initRowCount() const
{
    return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemGraphic3dTransformPers::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Graphic3d_TransformPers) aTransformPers = GetTransformPers();
  if (aTransformPers.IsNull())
    return Column() == 0 ? "Graphic3d_TransformPers" : "";

  switch (Column())
  {
    case 0: return aTransformPers->DynamicType()->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemGraphic3dTransformPers::createChild (int, int)
{
  return TreeModel_ItemBasePtr();
}
