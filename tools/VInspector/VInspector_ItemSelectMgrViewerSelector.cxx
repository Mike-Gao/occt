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

#include <inspector/VInspector_ItemSelectMgrViewerSelector.hxx>

#include <inspector/VInspector_ItemContainer.hxx>
#include <inspector/VInspector_ItemSelectMgrSelectableObjectSet.hxx>
#include <inspector/VInspector_ItemSelectMgrSelectingVolumeManager.hxx>
#include <inspector/VInspector_ItemSelectMgrSensitiveEntitySet.hxx>
#include <inspector/VInspector_ItemSelectMgrViewerSelectorPicked.hxx>

#include <inspector/VInspector_ItemFolderObject.hxx>
#include <inspector/VInspector_ItemContext.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/View_Tools.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Compound.hxx>

#include <StdSelect_ViewerSelector3d.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ItemSelectMgrViewerSelector::VInspector_ItemSelectMgrViewerSelector (TreeModel_ItemBasePtr theParent,
                                                                                const int theRow, const int theColumn)
: VInspector_ItemBase(theParent, theRow, theColumn),
  myXPix (150), myYPix (350), myXMinPix (150), myYMinPix (350), myXMaxPix (200), myYMaxPix (400)
{
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrViewerSelector::initRowCount() const
{
  if (Column() != 0)
    return 0;

  // SelectMgr_SelectingVolumeManager, VInspector_ItemSelectMgrSelectableObjectSet, VInspector_ItemContainer
  Standard_Integer aNbRows = GetFirstChildOfPicked();

  Handle(SelectMgr_ViewerSelector) aViewSelector = GetViewerSelector();
  if (!aViewSelector.IsNull())
  {
    for (Standard_Integer aDetIter = 1; aDetIter <= aViewSelector->NbPicked(); ++aDetIter)
    {
      aNbRows++;
      //Handle(SelectMgr_EntityOwner) anOwner = aViewSelector->->Picked (aDetIter);
    }
  }
  return aNbRows;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrViewerSelector::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  if (GetViewerSelector().IsNull())
    return Column() == 0 ? "Empty viewer selector" : "";

  switch (Column())
  {
    case 0: return GetViewerSelector()->DynamicType()->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrViewerSelector::Init()
{
  VInspector_ItemFolderObjectPtr aParentItem = itemDynamicCast<VInspector_ItemFolderObject>(Parent());
  Handle(SelectMgr_ViewerSelector) aViewerSelector;
  if (aParentItem)
  {
    VInspector_ItemContextPtr aParentContextItem = itemDynamicCast<VInspector_ItemContext>(aParentItem->Parent());
    if (aParentContextItem)
    {
      Handle(AIS_InteractiveContext) aContext = aParentContextItem->GetContext();
      aViewerSelector = aContext->MainSelector();
    }
  }
  setViewerSelector (aViewerSelector);

  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrViewerSelector::Reset()
{
  VInspector_ItemBase::Reset();

  setViewerSelector (NULL);
}

// =======================================================================
// function : GetSensitiveEntitySet
// purpose :
// =======================================================================
Handle(SelectMgr_SensitiveEntitySet) VInspector_ItemSelectMgrViewerSelector::GetSensitiveEntitySet (const int theRow,
  Handle(SelectMgr_SelectableObject)& theObject)
{
  Standard_Integer anIndex = 0;

  Handle(SelectMgr_ViewerSelector) aViewSelector = GetViewerSelector();
  if (aViewSelector.IsNull())
    return NULL;

  for (SelectMgr_MapOfObjectSensitivesIterator anIterator (aViewSelector->GetObjectSensitives()); anIterator.More(); anIterator.Next(), anIndex++)
  {
    if (anIndex != theRow)
      continue;

    theObject = anIterator.Key();
    return anIterator.Value();
  }
  return NULL;
}

// =======================================================================
// function : GetContainerRowCount
// purpose :
// =======================================================================

int VInspector_ItemSelectMgrViewerSelector::GetContainerRowCount (const int theContainerRow) const
{
  if (theContainerRow != 2)
    return 0;

  Handle(SelectMgr_ViewerSelector) aViewSelector = GetViewerSelector();
  if (aViewSelector.IsNull())
    return 0;

  return aViewSelector->GetObjectSensitives().Extent();
}

// =======================================================================
// function : GetContainerValue
// purpose :
// =======================================================================

QVariant VInspector_ItemSelectMgrViewerSelector::GetContainerValue (const int theContainerRow, const int theItemRole) const
{
  if (theContainerRow != 2)
    return 0;

  if (theItemRole != Qt::DisplayRole)
    return QVariant();

  Handle(SelectMgr_ViewerSelector) aViewSelector = GetViewerSelector();
  if (aViewSelector.IsNull())
    return QVariant();

  return "SelectMgr_MapOfObjectSensitives";
}

// =======================================================================
// function : CreateContainerChild
// purpose :
// =======================================================================

TreeModel_ItemBasePtr VInspector_ItemSelectMgrViewerSelector::CreateContainerChild (const TreeModel_ItemBasePtr& theParent, const int theContainerRow, int theRow, int theColumn)
{
  if (theContainerRow != 2)
    return TreeModel_ItemBasePtr();

  return VInspector_ItemSelectMgrSensitiveEntitySet::CreateItem (theParent, theRow, theColumn);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrViewerSelector::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrViewerSelector*>(this)->Init();
}

// =======================================================================
// function : GetStream
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrViewerSelector::GetStream (Standard_OStream& theOStream) const
{
  Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector();
  if (aViewerSelector.IsNull())
    return;

  aViewerSelector->DumpJson (theOStream);
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemSelectMgrViewerSelector::buildPresentationShape (const Handle(SelectMgr_ViewerSelector)& theViewSelector)
{
  if (theViewSelector.IsNull() || theViewSelector->NbPicked() == 0)
    return TopoDS_Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  for (Standard_Integer aRankId = 1; aRankId <= theViewSelector->NbPicked(); ++aRankId)
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (theViewSelector->PickedPoint (aRankId)));

  return aCompound;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrViewerSelector::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemSelectMgrSelectingVolumeManager::CreateItem (currentItem(), theRow, theColumn);
  else if (theRow == 1)
    return VInspector_ItemSelectMgrSelectableObjectSet::CreateItem (currentItem(), theRow, theColumn);
  else if (theRow == 2)
    return VInspector_ItemContainer::CreateItem (currentItem(), theRow, theColumn);
  else
    return VInspector_ItemSelectMgrViewerSelectorPicked::CreateItem (currentItem(), theRow, theColumn);
}
