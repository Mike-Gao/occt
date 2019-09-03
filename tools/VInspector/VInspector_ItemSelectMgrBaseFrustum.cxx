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
// function : Dump
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemSelectMgrBaseFrustum::Dump (Standard_OStream& OS) const
{
  Handle(SelectMgr_BaseFrustum) aFrustum = GetFrustum();

  if (aFrustum.IsNull())
    return Standard_False;

  aFrustum->Dump (OS);
  return Standard_True;
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrBaseFrustum::GetTableRowCount() const
{
  return 60;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrBaseFrustum::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  Handle(SelectMgr_BaseFrustum) aView = GetFrustum();
  if (aView.IsNull())
    return QVariant();

  //bool isFirstColumn = theColumn == 0;
  //switch (theRow)
  //{
  //  case 0:
  //  {
  //    if (isFirstColumn)
  //      return QVariant ("ImmediateUpdate");

  //    Standard_Boolean aCurState = aView->SetImmediateUpdate (Standard_False);
  //    aView->SetImmediateUpdate (aCurState);

  //    return aCurState;
  //  }
  //  break;
  //  case 1:
  //  {
  //    if (isFirstColumn)
  //      return QVariant ("ActiveLights");

  //    V3d_ListOfLightIterator aLightsIt = aView->ActiveLightIterator();
  //    Standard_Integer aNbOfLights = 0;
  //    while (aLightsIt.More())
  //    {
  //      aNbOfLights++;
  //      aLightsIt.Next();
  //    }
  //    return aNbOfLights;
  //  }
  //  case 2:
  //  {
  //    if (isFirstColumn)
  //      return QVariant ("Axis: origin");

  //    Standard_Real aX, anY, aZ, aVx, aVy, aVz;
  //    aView->Axis (aX, anY, aZ, aVx, aVy, aVz);

  //    return ViewControl_Tools::ToString (gp_Pnt (aX, anY, aZ)).ToCString();
  //  }
  //  case 3:
  //  {
  //    if (isFirstColumn)
  //      return QVariant ("Axis: direction");

  //    Standard_Real aX, anY, aZ, aVx, aVy, aVz;
  //    aView->Axis (aX, anY, aZ, aVx, aVy, aVz);

  //    return ViewControl_Tools::ToString (gp_Dir (aVx, aVy, aVz)).ToCString();
  //  }
  //  case 4: return isFirstColumn ? QVariant ("ComputedMode") : QVariant (aView->ComputedMode());
  //  case 5: return isFirstColumn ? QVariant ("AutoZFitMode") : QVariant (aView->AutoZFitMode());
  //  case 6: return isFirstColumn ? QVariant ("AutoZFitScaleFactor") : QVariant (aView->AutoZFitScaleFactor());
  //}
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrBaseFrustum::createChild (int theRow, int theColumn)
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
