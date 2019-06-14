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

#include <inspector/VInspector_ItemSelectMgrViewerSelectorPicked.hxx>

#include <inspector/VInspector_ItemSelectMgrViewerSelector.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrViewerSelectorPicked::initRowCount() const
{
  if (Column() != 0)
    return 0;

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrViewerSelectorPicked::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Standard_Integer aRankId;
  Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector (aRankId);
  if (aViewerSelector.IsNull())
    return Column() == 0 ? "Empty viewer selector" : "";

  switch (Column())
  {
    case 0:
    {
      TCollection_AsciiString aValue = TCollection_AsciiString (aRankId) + " - " +
                                       VInspector_Tools::GetPointerInfo (aViewerSelector->Picked (aRankId));
      return aValue.ToCString();
    }
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrViewerSelectorPicked::Init()
{
  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrViewerSelectorPicked::Reset()
{
  VInspector_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemSelectMgrViewerSelectorPicked::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrViewerSelectorPicked*>(this)->Init();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
Handle(SelectMgr_ViewerSelector) VInspector_ItemSelectMgrViewerSelectorPicked::GetViewerSelector (Standard_Integer& theRankId) const
{
  VInspector_ItemSelectMgrViewerSelectorPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrViewerSelector>(Parent());
  if (!aParentItem)
    return NULL;

  Handle(SelectMgr_ViewerSelector) aViewerSelector = aParentItem->GetViewerSelector();
  theRankId = Row() + 1 - aParentItem->GetFirstChildOfPicked();
  if (theRankId > aViewerSelector->NbPicked())
    return NULL;

  return aViewerSelector;
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrViewerSelectorPicked::GetTableRowCount() const
{
  return 60;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrViewerSelectorPicked::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  Standard_Integer aRankId;
  Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector (aRankId);
  if (aViewerSelector.IsNull())
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  switch (theRow)
  {
    case 0: return isFirstColumn ? "Picked" : 
      (theRole == Qt::DisplayRole ? VInspector_Tools::GetPointerInfo (aViewerSelector->Picked (aRankId)).ToCString()
                                  : aViewerSelector->Picked (aRankId)->DynamicType()->Name());
    case 1: return isFirstColumn ? "PickedEntity" :
      (theRole == Qt::DisplayRole ? VInspector_Tools::GetPointerInfo (aViewerSelector->PickedEntity (aRankId)).ToCString()
                                  : aViewerSelector->PickedEntity (aRankId)->DynamicType()->Name());
    case 2: return isFirstColumn ? "PickedPoint" : ViewControl_Tools::ToString (aViewerSelector->PickedPoint (aRankId)).ToCString();

    default: break;
  }
  Standard_Integer aLastIndex = 2;

  if (theRow > aLastIndex)
    return getTableSortCriterionData (theRow - (aLastIndex + 1), theColumn, theRole);

  return QVariant();
}

// =======================================================================
// function : getTableSortCriterionData
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrViewerSelectorPicked::getTableSortCriterionData (const Standard_Integer theIndex,
                                                                                  const int theColumn, const int theRole) const
{
  Standard_Integer aRankId;
  Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector (aRankId);
  if (aViewerSelector.IsNull())
    return QVariant();

  const SelectMgr_SortCriterion& aCriterion = aViewerSelector->PickedData (aRankId);
  bool isFirstColumn = theColumn == 0;
  switch (theIndex)
  {
    case 0: return isFirstColumn ? QVariant ("Depth") : QVariant (aCriterion.Depth);
    case 1: return isFirstColumn ? QVariant ("MinDist") : QVariant (aCriterion.MinDist);
    case 2: return isFirstColumn ? QVariant ("Tolerance") : QVariant (aCriterion.Tolerance);
    case 3: return isFirstColumn ? QVariant ("Priority") : QVariant (aCriterion.Priority);
    case 4: return isFirstColumn ? QVariant ("ZLayerPosition") : QVariant (aCriterion.ZLayerPosition);
    case 5: return isFirstColumn ? QVariant ("NbOwnerMatches") : QVariant (aCriterion.NbOwnerMatches);
    case 6: return isFirstColumn ? QVariant ("ToPreferClosest") : QVariant (aCriterion.ToPreferClosest);
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemSelectMgrViewerSelectorPicked::buildPresentationShape()
{
  Standard_Integer aRankId;
  Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector (aRankId);
  return buildPresentationShape (aViewerSelector, aRankId);
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemSelectMgrViewerSelectorPicked::buildPresentationShape (const Handle(SelectMgr_ViewerSelector)& theViewSelector,
                                                                                   const Standard_Integer theRowId)
{
  if (theViewSelector.IsNull() || theRowId + 1 > theViewSelector->NbPicked())
    return TopoDS_Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  Standard_Integer aRankId = theRowId + 1;
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (theViewSelector->PickedPoint (aRankId)));

  return aCompound;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrViewerSelectorPicked::createChild (int, int)
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
