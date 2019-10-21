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


#include <inspector/VInspector_ItemSelectMgrSensitiveEntity.hxx>
#include <inspector/VInspector_ItemSelectBasicsSensitiveEntity.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SensitiveEntity.hxx>
#include <Standard_Version.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Shape.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemSelectBasicsEntityOwner.hxx>
#include <inspector/VInspector_ItemSelectMgrSelection.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrSensitiveEntity::initRowCount() const
{
  if (GetSensitiveEntity()->BaseSensitive().IsNull())
    return 0;

  return 2;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrSensitiveEntity::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  Handle(SelectMgr_SensitiveEntity) anEntity = GetSensitiveEntity();
  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      switch (Column())
      {
        case 0: return anEntity->DynamicType()->Name();
        default:
          break;
      }
      break;
    }
    case Qt::BackgroundRole:
    case Qt::ForegroundRole:
    {
      if (Column() == 2)
      {
        Handle(AIS_InteractiveContext) aContext = GetContext();
        if (!aContext.IsNull())
        {
          if (VInspector_Tools::IsOwnerSelected(aContext, getEntityOwner()))
            return QVariant ((theItemRole == Qt::BackgroundRole) ? QColor (Qt::darkBlue) : QColor (Qt::white));
        }
      }
      VInspector_ItemSelectMgrSelectionPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSelection>(Parent());
      if (aParentItem)
        return aParentItem->data(QModelIndex(), theItemRole);
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
TreeModel_ItemBasePtr VInspector_ItemSelectMgrSensitiveEntity::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemSelectBasicsEntityOwner::CreateItem (currentItem(), theRow, theColumn);
  else if (theRow == 1)
    return VInspector_ItemSelectBasicsSensitiveEntity::CreateItem (currentItem(), theRow, theColumn);

  return TreeModel_ItemBasePtr();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSensitiveEntity::Init()
{
  VInspector_ItemSelectMgrSelectionPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSelection>(Parent());

  Handle(SelectMgr_Selection) aSelection = aParentItem->GetSelection();

  int aRowId = Row();
  int aCurrentId = 0;
#if OCC_VERSION_HEX < 0x070201
  for (aSelection->Init(); aSelection->More(); aSelection->Next(), aCurrentId++)
#else
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSelection->Entities()); aSelEntIter.More(); aSelEntIter.Next(), aCurrentId++)
#endif
  {
    if (aCurrentId != aRowId)
      continue;
#if OCC_VERSION_HEX < 0x070201
    myEntity = aSelection->Sensitive();
#else
    myEntity = aSelEntIter.Value();
#endif
    break;
  }
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSensitiveEntity::Reset()
{
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();
  myEntity = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSensitiveEntity::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrSensitiveEntity*>(this)->Init();
}

// =======================================================================
// function : getEntityOwner
// purpose :
// =======================================================================
Handle(SelectBasics_EntityOwner) VInspector_ItemSelectMgrSensitiveEntity::getEntityOwner() const
{
  initItem();

  Handle(SelectBasics_EntityOwner) anOwner;
  const Handle(SelectBasics_SensitiveEntity)& aBase = myEntity->BaseSensitive();
  if (aBase.IsNull())
    return anOwner;
  return aBase->OwnerId();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrSensitiveEntity::GetTableRowCount() const
{
  return 1;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrSensitiveEntity::GetTableData(const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  Handle(SelectMgr_SensitiveEntity) anEntity = GetSensitiveEntity();
  switch (theRow)
  {
    case 0: return isFirstColumn ? QVariant ("IsActiveForSelection") : QVariant (anEntity->IsActiveForSelection());
    default: return QVariant();
  }
}