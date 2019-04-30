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

#include <inspector/VInspector_ItemBVHTree.hxx>

#include <inspector/VInspector_ItemSelectMgrSelectableObjectSet.hxx>
#include <inspector/VInspector_ItemBVHTreeNode.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ItemBVHTree::VInspector_ItemBVHTree (TreeModel_ItemBasePtr theParent,
                                                const int theRow, const int theColumn)
: VInspector_ItemBase(theParent, theRow, theColumn)
{
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemBVHTree::initRowCount() const
{
  if (Column() != 0)
    return  0;

  opencascade::handle<BVH_Tree<Standard_Real, 3> > aTree =  GetTree();
  if (aTree.IsNull())
    return 0;

  return aTree->Length();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemBVHTree::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  if (GetTree().IsNull())
    return Column() == 0 ? "Empty BVH tree" : "";

  switch (Column())
  {
    case 0:
    {
      TCollection_AsciiString aName = TCollection_AsciiString (GetTree()->DynamicType()->Name()) +
                                      TCollection_AsciiString (" (") + myName.ToCString() + ")";
      return aName.ToCString();
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

void VInspector_ItemBVHTree::Init()
{
  VInspector_ItemSelectMgrSelectableObjectSetPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSelectableObjectSet>(Parent());

  //! Returns child BVH tree of the row
  setTree (aParentItem->GetBVHTree (Row(), myName));

  //Handle(SelectMgr_ViewerSelector) aViewerSelector;
  //if (aParentItem)
  //{
  //  VInspector_ItemContextPtr aParentContextItem = itemDynamicCast<VInspector_ItemContext>(aParentItem->Parent());
  //  if (aParentContextItem)
  //  {
  //    Handle(AIS_InteractiveContext) aContext = aParentContextItem->GetContext();
  //    aViewerSelector = aContext->MainSelector();
  //  }
  //}
  //setViewerSelector (aViewerSelector);

  //UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemBVHTree::Reset()
{
  VInspector_ItemBase::Reset();

  setTree (NULL);
  myName = TCollection_AsciiString();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemBVHTree::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemBVHTree*>(this)->Init();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemBVHTree::GetTableRowCount() const
{
  return 60;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemBVHTree::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  //if (theRole != Qt::DisplayRole)
  //  return QVariant();

  //Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector();
  //if (aViewerSelector.IsNull())
  //  return QVariant();

  //bool isFirstColumn = theColumn == 0;
  //switch (theRow)
  //{
  //  case 0: return isFirstColumn ? QVariant ("Sensitivity") : QVariant (aViewerSelector->Sensitivity());
  //  case 1: return isFirstColumn ? QVariant ("IsPickClosest") : QVariant (aViewerSelector->IsPickClosest());
  //  case 2: return isFirstColumn ? QVariant ("NbPicked") : QVariant (aViewerSelector->NbPicked());

  //  case 3: return ViewControl_Table::SeparatorData();
  //  case 4: return isFirstColumn ? QVariant ("ClearPicked") : QVariant ("DO");

  //  case 5: return ViewControl_Table::SeparatorData();
  //  case 6: return isFirstColumn ? QVariant ("X (pixel)") : QVariant (myXPix);
  //  case 7: return isFirstColumn ? QVariant ("Y (pixel)") : QVariant (myYPix);
  //  case 8: return isFirstColumn ? QVariant ("Pick") : QVariant ("DO");

  //  case 9: return ViewControl_Table::SeparatorData();
  //  case 10: return isFirstColumn ? QVariant ("X Min (pixel)") : QVariant (myXMinPix);
  //  case 11: return isFirstColumn ? QVariant ("Y Min (pixel)") : QVariant (myXMinPix);
  //  case 12: return isFirstColumn ? QVariant ("X Max (pixel)") : QVariant (myXMaxPix);
  //  case 13: return isFirstColumn ? QVariant ("Y Max (pixel)") : QVariant (myYMaxPix);
  //  case 14: return isFirstColumn ? QVariant ("Pick") : QVariant ("DO");

  //  default: break;
  //}
  return QVariant();
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspector_ItemBVHTree::GetTableEditType (const int theRow, const int) const
{
  switch (theRow)
  {
    //case 4: return ViewControl_EditType_DoAction;
    //case 6: return ViewControl_EditType_Spin;
    //case 7: return ViewControl_EditType_Spin;
    //case 8: return ViewControl_EditType_DoAction;
    //case 10: return ViewControl_EditType_Spin;
    //case 11: return ViewControl_EditType_Spin;
    //case 12: return ViewControl_EditType_Spin;
    //case 13: return ViewControl_EditType_Spin;
    //case 14: return ViewControl_EditType_DoAction;
    default: return ViewControl_EditType_None;
  }
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspector_ItemBVHTree::SetTableData (const int theRow, const int, const QVariant& theValue)
{
  //Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector();
  //if (aViewerSelector.IsNull())
  //  return Standard_False;

  //switch (theRow)
  //{
  //  case 4: aViewerSelector->ClearPicked(); break;
  //  case 6: myXPix = theValue.toInt();
  //  case 7: myYPix = theValue.toInt();
  //  case 8:
  //  {
  //    Handle(StdSelect_ViewerSelector3d) aSelector3d = Handle(StdSelect_ViewerSelector3d)::DownCast(aViewerSelector);
  //    if (!aSelector3d.IsNull())
  //      aSelector3d->Pick (myXPix, myYPix, View_Tools::FindActiveView (GetContext()));
  //    break;
  //  }
  //  case 10: myXMinPix = theValue.toInt();
  //  case 11: myXMinPix = theValue.toInt();
  //  case 12: myXMaxPix = theValue.toInt();
  //  case 13: myYMaxPix = theValue.toInt();
  //  case 14:
  //  {
  //    Handle(StdSelect_ViewerSelector3d) aSelector3d = Handle(StdSelect_ViewerSelector3d)::DownCast(aViewerSelector);
  //    if (!aSelector3d.IsNull())
  //      aSelector3d->Pick (myXMinPix, myYMinPix, myXMaxPix, myYMaxPix, View_Tools::FindActiveView (GetContext()));
  //    break;
  //  }
  //  default: break;
  //}
  return Standard_True;
}

// =======================================================================
// function : Dump
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemBVHTree::Dump (Standard_OStream& OS) const
{
  //Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector();
  //if (aViewerSelector.IsNull())
  //  return Standard_False;

  //aViewerSelector->Dump (OS);
  return Standard_True;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemBVHTree::createChild (int theRow, int theColumn)
{
  return VInspector_ItemBVHTreeNode::CreateItem (currentItem(), theRow, theColumn);
}
