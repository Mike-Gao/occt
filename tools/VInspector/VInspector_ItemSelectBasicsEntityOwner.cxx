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

#include <inspector/VInspector_ItemSelectBasicsEntityOwner.hxx>
#include <inspector/VInspector_ItemSelectMgrSensitiveEntity.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <SelectMgr_SensitiveEntity.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <Standard_Version.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QItemSelectionModel>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectBasicsEntityOwner::initValue(int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  switch (theItemRole)
  { 
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      Handle(SelectBasics_EntityOwner) anOwner = getEntityOwner();
      if (anOwner.IsNull())
        return QVariant();

      switch (Column())
      {
        case 0: return anOwner->DynamicType()->Name();
        default: break;
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
            return (theItemRole == Qt::BackgroundRole) ? QColor (Qt::darkBlue) : QColor (Qt::white);
        }
      }
      VInspector_ItemSelectMgrSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSensitiveEntity>(Parent());
      if (aParentItem)
        return aParentItem->data(QModelIndex(), theItemRole);
      break;
    }
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsEntityOwner::Init()
{
  Handle(SelectBasics_EntityOwner) anOwner;

  VInspector_ItemSelectMgrSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSensitiveEntity>(Parent());
  if (aParentItem)
  {
    Handle(SelectMgr_SensitiveEntity) anEntity = aParentItem->GetSensitiveEntity();
    if (!anEntity.IsNull() && !anEntity->BaseSensitive().IsNull())
      anOwner = anEntity->BaseSensitive()->OwnerId();
  }
  else
  {
    VInspector_ItemPresentableObjectPtr aPOItem = itemDynamicCast<VInspector_ItemPresentableObject>(Parent());
    if (aPOItem)
    {
      Handle(AIS_InteractiveObject) anIO = aPOItem->GetInteractiveObject();

      int aRowId = Row();
      int aCurrentIndex = 0;
#if OCC_VERSION_HEX < 0x070201
      for (anIO->Init(); anIO->More() && anOwner.IsNull(); anIO->Next())
      {
        const Handle(SelectMgr_Selection)& aSelection = anIO->CurrentSelection();
        for (aSelection->Init(); aSelection->More() && anOwner.IsNull(); aSelection->Next())
        {
          Handle(SelectMgr_SensitiveEntity) anEntity = aSelection->Sensitive();
#else
      for (SelectMgr_SequenceOfSelection::Iterator aSelIter (anIO->Selections()); aSelIter.More() && anOwner.IsNull(); aSelIter.Next())
      {
        const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
        for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSelection->Entities()); aSelEntIter.More() && anOwner.IsNull(); aSelEntIter.Next())
        {
          Handle(SelectMgr_SensitiveEntity) anEntity = aSelEntIter.Value();
#endif
          const Handle(SelectBasics_SensitiveEntity)& aBase = anEntity->BaseSensitive();
          if (!aBase.IsNull())
          {
            if (aRowId == aCurrentIndex)
              anOwner = aBase->OwnerId();
            aCurrentIndex++;
          }
        }
      }
    }
  }
  myOwner = anOwner;
  UpdatePresentationShape();
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsEntityOwner::Reset()
{
  VInspector_ItemBase::Reset();
  SetContext (NULL);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsEntityOwner::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectBasicsEntityOwner*>(this)->Init();
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsEntityOwner::initStream (Standard_OStream& theOStream) const
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = getEntityOwner();
  if (anEntityOwner.IsNull())
    return;

  anEntityOwner->DumpJson (theOStream);
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemSelectBasicsEntityOwner::buildPresentationShape()
{
  Handle(StdSelect_BRepOwner) aBROwner = Handle(StdSelect_BRepOwner)::DownCast (myOwner);
  if (aBROwner.IsNull())
    return TopoDS_Shape();

  return aBROwner->Shape();
}
