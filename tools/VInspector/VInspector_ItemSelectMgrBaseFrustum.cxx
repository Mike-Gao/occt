// Created on: 2019-02-04
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

#include <inspector/VInspector_ItemSelectMgrBaseFrustum.hxx>

#include <inspector/VInspector_ItemSelectMgrSelectingVolumeManager.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrBaseFrustum::initRowCount() const
{
  if (Column() != 0)
    return 0;

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrBaseFrustum::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle (SelectMgr_BaseFrustum) aFrustum = GetFrustum();
  if (aFrustum.IsNull())
    return Column() == 0 ? "Empty frustum" : "";

  switch (Column())
  {
    case 0: return aFrustum->DynamicType()->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrBaseFrustum::Init()
{
  VInspector_ItemSelectMgrSelectingVolumeManagerPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSelectingVolumeManager>(Parent());

  Handle(SelectMgr_BaseFrustum) aFrustum;
  if (aParentItem)
  {
    SelectMgr_SelectingVolumeManager aVolumeManager;
    if (aParentItem->GetViewerSelector (aVolumeManager))
    {
      aFrustum = aVolumeManager.ActiveVolume();
    }
  }
  setFrustum (aFrustum);

  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrBaseFrustum::Reset()
{
  VInspector_ItemBase::Reset();

  setFrustum (NULL);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrBaseFrustum::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrBaseFrustum*>(this)->Init();
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrBaseFrustum::initStream (Standard_OStream& theOStream) const
{
  Handle(SelectMgr_BaseFrustum) aFrustum = GetFrustum();

  if (aFrustum.IsNull())
    return;

  aFrustum->DumpJson (theOStream);
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrBaseFrustum::createChild (int /*theRow*/, int /*theColumn*/)
{
  //if (theRow == 0)
  //  return VInspector_ItemGraphic3dCamera::CreateItem (currentItem(), theRow, theColumn);
  //else if (theRow == 1)
  //  return VInspector_ItemAspectWindow::CreateItem (currentItem(), theRow, theColumn);
  //else if (theRow == 2)
  //  return VInspector_ItemGraphic3dCView::CreateItem (currentItem(), theRow, theColumn);
  //
  return TreeModel_ItemBasePtr();
}
