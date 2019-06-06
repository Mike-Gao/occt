// Created on: 2019-04-29
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

#include <inspector/VInspector_ItemSelectMgrSelectableObjectSet.hxx>

#include <inspector/VInspector_ItemBVHTree.hxx>
#include <inspector/VInspector_ItemSelectMgrViewerSelector.hxx>
#include <inspector/VInspector_ItemSelectMgrBaseFrustum.hxx>

#include <inspector/ViewControl_Tools.hxx>

#include <SelectMgr.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrSelectableObjectSet::initRowCount() const
{
  if (Column() != 0)
    return 0;

  return SelectMgr_SelectableObjectSet::BVHSubsetNb; // VInspector_ItemBVHTree items
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrSelectableObjectSet::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  SelectMgr_SelectableObjectSet aSet;
  if (!GetSelectableObjectSet (aSet))
    return Column() == 0 ? "Empty selectable object set" : "";

  switch (Column())
  {
    case 0: return "SelectMgr_SelectableObjectSet";
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrSelectableObjectSet::Init()
{
  //VInspector_ItemFolderObjectPtr aParentItem = itemDynamicCast<VInspector_ItemFolderObject>(Parent());
  //Handle(SelectMgr_SelectingVolumeManager) aVolumeMgr;
  //if (aParentItem)
  //{
  //  VInspector_ItemContextPtr aParentContextItem = itemDynamicCast<VInspector_ItemContext>(aParentItem->Parent());
  //  if (aParentContextItem)
  //  {
  //    Handle(AIS_InteractiveContext) aContext = aParentContextItem->GetContext();
  //    aVolumeMgr = aContext->MainSelector();
  //  }
  //}
  //setViewerSelector (aVolumeMgr);

  //UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrSelectableObjectSet::Reset()
{
  VInspector_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrSelectableObjectSet::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrSelectableObjectSet*>(this)->Init();
}

// =======================================================================
// function : GetSelectableObjectSet
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemSelectMgrSelectableObjectSet::GetSelectableObjectSet (SelectMgr_SelectableObjectSet& theSet) const
{
  VInspector_ItemSelectMgrViewerSelectorPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrViewerSelector>(Parent());

  if (!aParentItem || aParentItem->GetViewerSelector().IsNull())
    return Standard_False;

  theSet = aParentItem->GetViewerSelector()->GetSelectableObjects();
  return Standard_True;
}

// =======================================================================
// function : GetBVHTree
// purpose :
// =======================================================================
opencascade::handle<BVH_Tree<Standard_Real, 3> > VInspector_ItemSelectMgrSelectableObjectSet::GetBVHTree (const int theRow,
  TCollection_AsciiString& theName) const
{
  SelectMgr_SelectableObjectSet aSet;
  if (!GetSelectableObjectSet (aSet))
    return NULL;

  if (theRow >= SelectMgr_SelectableObjectSet::BVHSubsetNb)
    return NULL;

  SelectMgr_SelectableObjectSet::BVHSubset aBVHSubset = (SelectMgr_SelectableObjectSet::BVHSubset)theRow;
  theName = TCollection_AsciiString ("BVH_Tree_") + SelectMgr::BVHSubsetToString (aBVHSubset);

  return aSet.BVH (aBVHSubset);
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrSelectableObjectSet::GetTableRowCount() const
{
  return 60;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrSelectableObjectSet::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  //SelectMgr_SelectingVolumeManager aVolumeMgr;
  //if (!GetViewerSelector (aVolumeMgr))
  //  return QVariant();

  //bool isFirstColumn = theColumn == 0;
  //switch (theRow)
  //{
  //  case 0: return isFirstColumn ? QVariant ("GetActiveSelectionType") : QVariant (aVolumeMgr.GetActiveSelectionType());
  //  case 1: return isFirstColumn ? QVariant ("IsOverlapAllowed()") : QVariant (aVolumeMgr.IsOverlapAllowed());
  //  case 2: return isFirstColumn ? "GetNearPickedPnt" : ViewControl_Tools::ToString (aVolumeMgr.GetNearPickedPnt()).ToCString();
  //  case 3: return isFirstColumn ? "GetFarPickedPnt" : ViewControl_Tools::ToString (aVolumeMgr.GetFarPickedPnt()).ToCString();
  //  default: break;
  //}
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrSelectableObjectSet::createChild (int theRow, int theColumn)
{
  if (theRow == 0 || theRow == 1 || theRow == 2)
    return VInspector_ItemBVHTree::CreateItem (currentItem(), theRow, theColumn);
  return TreeModel_ItemBasePtr();
}
