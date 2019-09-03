// Created on: 2019-03-15
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

#include <inspector/VInspector_ItemOpenGlLayerList.hxx>

#include <inspector/VInspector_ItemGraphic3dGroup.hxx>
#include <inspector/VInspector_ItemGraphic3dCView.hxx>
#include <inspector/VInspector_ItemOpenGlLayer.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Text.hxx>
#include <SelectMgr.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemOpenGlLayerList::Init()
{
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemOpenGlLayerList::Reset()
{
  VInspector_ItemBase::Reset();
}

// =======================================================================
// function : GetLayerList
// purpose :
// =======================================================================
const OpenGl_LayerList& VInspector_ItemOpenGlLayerList::GetLayerList (Standard_Boolean& isDefault) const
{
  VInspector_ItemGraphic3dCViewPtr aParentItem = itemDynamicCast<VInspector_ItemGraphic3dCView>(Parent());
  return aParentItem->GetLayerList (isDefault);
}

// =======================================================================
// function : GetLayer
// purpose :
// =======================================================================
Handle(OpenGl_Layer) VInspector_ItemOpenGlLayerList::GetLayer (const int theRow, Graphic3d_ZLayerId& theLayerId) const
{
  return NULL;
  /*
  Standard_Boolean isDefault;
  const OpenGl_LayerList& aLayerList = GetLayerList (isDefault);

  if (isDefault)
    return NULL;

  Standard_Integer aLayerIndex = (int)theRow + 1;
  Handle(OpenGl_Layer) aLayer = aLayerList.Layers().Value (aLayerIndex); // OpenGl_LayerSeqIds range is in [1...N]
  if (aLayer.IsNull())
    return NULL;

  const OpenGl_LayerSeqIds& aLayerIDs = aLayerList.LayerIDs();
  for (OpenGl_LayerSeqIds::Iterator aMapIt (aLayerIDs); aMapIt.More(); aMapIt.Next())
  {
    Standard_Integer anIndex = aMapIt.Value();
    if (anIndex != aLayerIndex)
      continue;

    theLayerId = aMapIt.Key();
  }

  return aLayer;
  */
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemOpenGlLayerList::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemOpenGlLayerList*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemOpenGlLayerList::initRowCount() const
{
  if (Column() != 0)
    return 0;

  Standard_Boolean isDefault;
  const OpenGl_LayerList& aListOfLayers = GetLayerList (isDefault);
  if (isDefault)
    return 0;

  //return aListOfLayers.Layers().Length();
  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlLayerList::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();


  switch (Column())
  {
    case 0:
    {
      Standard_Boolean isDefault;
      GetLayerList (isDefault);
      if (isDefault)
        return 0;

      return isDefault ? "Empty layer list" : "OpenGl_LayerList";
    }
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemOpenGlLayerList::GetTableRowCount() const
{
  return 3;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlLayerList::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  Standard_Boolean isDefault;
  const OpenGl_LayerList& aListOfLayers = GetLayerList (isDefault);
  if (isDefault)
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  switch (theRow)
  {
    case 0: return isFirstColumn ? QVariant ("NbPriorities") : QVariant (aListOfLayers.NbPriorities());
    case 1: return isFirstColumn ? QVariant ("NbStructures") : QVariant (aListOfLayers.NbStructures());
    case 2: return isFirstColumn ? QVariant ("NbImmediateStructures") : QVariant (aListOfLayers.NbImmediateStructures());

    default: return QVariant();
  }

  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemOpenGlLayerList::createChild (int theRow, int theColumn)
{
  return VInspector_ItemOpenGlLayer::CreateItem (currentItem(), theRow, theColumn);
}
