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


#include <inspector/VInspector_ItemSelectMgrSensitiveEntitySet.hxx>
#include <inspector/VInspector_ItemSelectMgrViewerSelector.hxx>
#include <inspector/VInspector_ItemBVHTree.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <SelectMgr_SensitiveEntitySet.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrSensitiveEntitySet::initRowCount() const
{
  //Handle(SelectMgr_SensitiveEntitySet) aSensitiveSet = Handle(SelectMgr_SensitiveEntitySet)::DownCast (GetSensitiveEntitySet());
  //if (!aSensitiveSet.IsNull())
  //  return aSensitiveSet->Size();

  return 1; // for BVH_Tree
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrSensitiveEntitySet::initValue (int theItemRole) const
{
  if (theItemRole == Qt::DisplayRole && theItemRole == Qt::ToolTipRole && Column() == 2)
    return ViewControl_Tools::GetPointerInfo (GetSelectableObject(), true).ToCString();

  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(SelectMgr_SensitiveEntitySet) anEntitySet = GetSensitiveEntitySet();
  const Handle(SelectMgr_SelectableObject)& aSelectableObject = GetSelectableObject();

  if (anEntitySet.IsNull())
    return Column() == 0 ? "Empty sensitive entity set" : "";

  if (aSelectableObject.IsNull())
    return Column() == 0 ? "Empty selectable object for sensitive entity set" : "";

  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      switch (Column())
      {
        case 0: return aSelectableObject->DynamicType()->Name();
        default:
          break;
      }
      break;
    }
    default:
    break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrSensitiveEntitySet::createChild (int theRow, int theColumn)
{
  return VInspector_ItemBVHTree::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSensitiveEntitySet::Init()
{
  VInspector_ItemSelectMgrViewerSelectorPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrViewerSelector>(Parent()->Parent());

  Handle(SelectMgr_SelectableObject) anObject;
  Handle(SelectMgr_SensitiveEntitySet) anEntitySet = aParentItem->GetSensitiveEntitySet (Row(), anObject);

  myEntitySet = anEntitySet;
  mySelectableObject = anObject;

  UpdatePresentationShape();
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSensitiveEntitySet::Reset()
{
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();
  myEntitySet = NULL;
  mySelectableObject = NULL;
}

// =======================================================================
// function : GetBVHTree
// purpose :
// =======================================================================
opencascade::handle<BVH_Tree<Standard_Real, 3> > VInspector_ItemSelectMgrSensitiveEntitySet::GetBVHTree (const int /*theRow*/,
  TCollection_AsciiString& /*theName*/) const
{
  Handle(SelectMgr_SensitiveEntitySet) anEntitySet = GetSensitiveEntitySet();

  if (anEntitySet.IsNull())
    return NULL;

  return anEntitySet->BVH();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSensitiveEntitySet::initItem() const
{
  if (IsInitialized())
    return;

  const_cast<VInspector_ItemSelectMgrSensitiveEntitySet*>(this)->Init();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemSelectMgrSensitiveEntitySet::buildPresentationShape()
{
  //mySelectableObject
  return TopoDS_Shape();
}
