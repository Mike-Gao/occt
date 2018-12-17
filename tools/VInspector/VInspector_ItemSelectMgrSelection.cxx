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

#include <inspector/VInspector_ItemSelectMgrSelection.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SensitiveEntity.hxx>
#include <Standard_Version.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_ItemSelectMgrSensitiveEntity.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>

#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrSelection::initRowCount() const
{
  Handle(SelectMgr_Selection) aSelection = GetSelection();
#if OCC_VERSION_HEX < 0x070201
  int aRows = 0;
  for (aSelection->Init(); aSelection->More(); aSelection->Next())
    aRows++;
  return aRows;
#else
  return aSelection->Entities().Size();
#endif
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrSelection::initValue (int theItemRole) const
{
  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      switch (Column())
      {
        case 0: return GetSelection()->DynamicType()->Name();
        case 1: return rowCount();
        case 3:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "Mode";
          else
          {
            VInspector_ItemPresentableObjectPtr aParentItem = itemDynamicCast<VInspector_ItemPresentableObject>(Parent());
            return VInspector_Tools::SelectionModeToName(GetSelection()->Mode(), aParentItem->GetInteractiveObject()).ToCString();
          }
        }
        case 4:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "SelectMgr_StateOfSelection";
          else {
            int aNbSelected = 0;
            SelectMgr_StateOfSelection aState = GetSelection()->GetSelectionState();
            if (aState == SelectMgr_SOS_Activated || aState == SelectMgr_SOS_Any)
            {
              Handle(AIS_InteractiveContext) aContext = GetContext();
#if OCC_VERSION_HEX < 0x070201
              for (mySelection->Init(); mySelection->More(); mySelection->Next())
              {
                const Handle(SelectBasics_EntityOwner)& anOwner = mySelection->Sensitive()->BaseSensitive()->OwnerId();
#else
              for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (mySelection->Entities()); aSelEntIter.More(); aSelEntIter.Next())
              {
                const Handle(SelectBasics_EntityOwner)& anOwner = aSelEntIter.Value()->BaseSensitive()->OwnerId();
#endif
                if (VInspector_Tools::IsOwnerSelected(aContext, anOwner))
                  aNbSelected++;
              }
            }
            return aNbSelected > 0 ? QString::number (aNbSelected) : "";
          }
        }
        case 9:
        {
          SelectMgr_StateOfSelection aState = GetSelection()->GetSelectionState();
          return VInspector_Tools::ToName (VInspector_SelectionType_StateOfSelection, aState).ToCString();
        }
        case 10: return QString::number (GetSelection()->Sensitivity());
        case 11:
          return VInspector_Tools::ToName (VInspector_SelectionType_TypeOfUpdate,
                                           GetSelection()->UpdateStatus()).ToCString();
        case 12:
          return VInspector_Tools::ToName (VInspector_SelectionType_TypeOfBVHUpdate,
                                           GetSelection()->BVHUpdateStatus()).ToCString();
        default:
          break;
      }
      break;
    }
    case Qt::ForegroundRole:
    {
      SelectMgr_StateOfSelection aState = GetSelection()->GetSelectionState();
      return QVariant (aState == SelectMgr_SOS_Activated ? QColor (Qt::black) : QColor (Qt::darkGray));
    }
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrSelection::createChild (int theRow, int theColumn)
{
  return VInspector_ItemSelectMgrSensitiveEntity::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSelection::Init()
{
  VInspector_ItemPresentableObjectPtr aParentItem = itemDynamicCast<VInspector_ItemPresentableObject>(Parent());

  Handle(AIS_InteractiveObject) anIO = aParentItem->GetInteractiveObject();

  int aRowId = Row();
  int aDeltaIndex = 2; // properties, presentation items
  int aCurrentId = 0;
#if OCC_VERSION_HEX < 0x070201
  for (anIO->Init(); anIO->More(); anIO->Next(), aCurrentId++)
#else
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (anIO->Selections()); aSelIter.More(); aSelIter.Next(), aCurrentId++)
#endif
  {
    if (aCurrentId != aRowId - aDeltaIndex)
      continue;
#if OCC_VERSION_HEX < 0x070201
    mySelection = anIO->CurrentSelection();
#else
    mySelection = aSelIter.Value();
#endif
    break;
  }
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSelection::Reset()
{
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();

  mySelection = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrSelection::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrSelection*>(this)->Init();
  // an empty method to don't initialize the main label, as it was not cleared in Reset()
}
