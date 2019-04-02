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

#include <inspector/VInspector_ItemOpenGlLayer.hxx>

#include <inspector/VInspector_ItemOpenGlLayerList.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <Aspect.hxx>
#include <Graphic3d.hxx>
#include <OpenGl_Layer.hxx>
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
void VInspector_ItemOpenGlLayer::Init()
{
  VInspector_ItemOpenGlLayerListPtr aParentItem = itemDynamicCast<VInspector_ItemOpenGlLayerList>(Parent());
  myLayer = aParentItem->GetLayer (Row(), myLayerId);

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemOpenGlLayer::Reset()
{
  VInspector_ItemBase::Reset();
  myLayer = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemOpenGlLayer::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemOpenGlLayer*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemOpenGlLayer::initRowCount() const
{
  if (Column() != 0)
    return 0;

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlLayer::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(OpenGl_Layer) aLayer = GetLayer();
  if (aLayer.IsNull())
    return Column() == 0 ? "Empty element" : "";

  switch (Column())
  {
    case 0:
    {
      TCollection_AsciiString aLayerId = Graphic3d::ZLayerIdToString (myLayerId);
      if (aLayerId.IsEmpty())
        aLayerId = TCollection_AsciiString (myLayerId);
      return theItemRole == Qt::ToolTipRole ? QVariant ("")
                                            : QVariant (QString("%1 (%2)")
                                            .arg(aLayer->DynamicType()->Name())
                                            .arg (aLayerId.ToCString()));
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
int VInspector_ItemOpenGlLayer::GetTableRowCount() const
{
  return 40;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlLayer::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  Handle(OpenGl_Layer) aLayer = GetLayer();
  if (aLayer.IsNull())
    return QVariant();

  bool isFirstColumn = theColumn == 0;

  switch (theRow)
  {
    case 0: return isFirstColumn ? QVariant ("NbStructures") : QVariant (aLayer->NbStructures());
    case 1: return isFirstColumn ? QVariant ("NbStructuresNotCulled") : QVariant (aLayer->NbStructuresNotCulled());
    case 2: return isFirstColumn ? QVariant ("NbPriorities") : QVariant (aLayer->NbPriorities());

    case 3: return isFirstColumn ? QVariant ("ArrayOfStructures") : QVariant (aLayer->ArrayOfStructures().Size());
    case 4: return isFirstColumn ? QVariant ("IsCulled") : QVariant (aLayer->IsCulled());
    case 5: return isFirstColumn ? QVariant ("NbOfTransformPersistenceObjects") : QVariant (aLayer->NbOfTransformPersistenceObjects());

    case 6: return isFirstColumn ? QVariant ("CullableStructuresBVH") : QVariant (aLayer->CullableStructuresBVH().Size());
    case 7: return isFirstColumn ? QVariant ("CullableTrsfPersStructuresBVH") : QVariant (aLayer->CullableTrsfPersStructuresBVH().Size());
    case 8: return isFirstColumn ? QVariant ("NonCullableStructures") : QVariant (aLayer->NonCullableStructures().Size());

    default:
    break;
  }

  Standard_Integer aRow = theRow - 9;
  return getLayerSettingsTableData (aRow, theColumn, theRole, aLayer->LayerSettings());
}

// =======================================================================
// function : getLayerSettingsTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlLayer::getLayerSettingsTableData (const int theRow, const int theColumn, const int theRole,
                                                                const Graphic3d_ZLayerSettings& theSettings) const
{
  bool isFirstColumn = theColumn == 0;

  switch (theRow)
  {
    case 0: return isFirstColumn ? QVariant ("LayerSettings:") : QVariant();
    case 1: return isFirstColumn ? QVariant ("Name") : QVariant (theSettings.Name().ToCString());
    case 2: return isFirstColumn ? QVariant ("Lights") : QVariant (ViewControl_Tools::GetPointerInfo (theSettings.Lights()).ToCString());

    case 3: return isFirstColumn ? QVariant ("Origin") : QVariant (ViewControl_Tools::ToString (theSettings.Origin()).ToCString());
    case 4: return isFirstColumn ? QVariant ("OriginTransformation")
      : QVariant (ViewControl_Tools::ToString (theSettings.OriginTransformation()).ToCString());

    case 5: return isFirstColumn ? QVariant ("HasCullingDistance") : QVariant (theSettings.HasCullingDistance());
    case 6: return isFirstColumn ? QVariant ("CullingDistance")
      : QVariant (theSettings.HasCullingDistance() ? theSettings.CullingDistance() : 0);

    case 7: return isFirstColumn ? QVariant ("HasCullingSize") : QVariant (theSettings.HasCullingSize());
    case 8: return isFirstColumn ? QVariant ("CullingSize")
      : QVariant (theSettings.HasCullingSize() ? theSettings.CullingSize() : 0);

    case 9: return isFirstColumn ? QVariant ("IsImmediate") : QVariant (theSettings.IsImmediate());
    case 10: return isFirstColumn ? QVariant ("UseEnvironmentTexture") : QVariant (theSettings.UseEnvironmentTexture());
    case 11: return isFirstColumn ? QVariant ("ToEnableDepthTest") : QVariant (theSettings.ToEnableDepthTest());
    case 12: return isFirstColumn ? QVariant ("ToEnableDepthWrite") : QVariant (theSettings.ToEnableDepthWrite());
    case 13: return isFirstColumn ? QVariant ("ToClearDepth") : QVariant (theSettings.ToClearDepth());
    case 14: return isFirstColumn ? QVariant ("ToRenderInDepthPrepass") : QVariant (theSettings.ToRenderInDepthPrepass());

    case 15: return isFirstColumn ? QVariant ("PolygonOffset: Mode")
      : QVariant (Aspect::PolygonOffsetModeToString (theSettings.PolygonOffset().Mode));
    case 16: return isFirstColumn ? QVariant ("PolygonOffset: Factor") : QVariant (theSettings.PolygonOffset().Factor);
    case 17: return isFirstColumn ? QVariant ("PolygonOffset: Units") : QVariant (theSettings.PolygonOffset().Units);

    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemOpenGlLayer::createChild (int theRow, int theColumn)
{
  (void)theRow;
  (void)theColumn;
  return TreeModel_ItemBasePtr();
}
